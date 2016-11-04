#include	"core.h"
#include    "../Layer/layer.h"
/// <summary>
/// 全局系统信息
/// </summary>
struct sys_info  gSys_Info;

void iec_main_thread_entry(void *param);
#define MAIN_THREAD_PROI    (15)
#define MAIN_THREAD_TICK		(50)
#define MAIN_THREAD_STACK_SIZE   (512)
#define MAX_EVENT_COUNT			(5)


/**
 * 查找当前系统中某个链路
 *
 * @param info 系统信息实例
 * @param name 链路名称
 *
 * @return 链路ID  0 未找到
 */
 static struct link_obj *iec_sys_find_link(struct sys_info *info,char *name)
{
  int i=0;
  char link_name[CFG_NAME_MAX];
  struct link_obj *link=0;

  arraylist_iterate(info->sys_link_obj, i, link)
    {
      if(link!=0)
        {
          rt_memcpy(&link_name,(char *)link->name,CFG_NAME_MAX);
          if(rt_strcmp(name, link_name)==0)
            return link;
        }
    }

  return 0;
}


static struct app_info *iec_sys_find_app(struct sys_info *info,char *name)
{
  int i=0;
  char app_name[CFG_NAME_MAX];
  struct app_info *app=0;

  arraylist_iterate(info->sys_app_obj, i, app)
    {
      if(app!=0)
        {
          rt_memcpy(&app_name,(char *)app->cfg.name,CFG_NAME_MAX);
          if(rt_strcmp(name, app_name)==0)
            return app;
        }
    }

  return 0;
}

/**
 *  添加一个link
 *
 * @param info 系统信息实例
 * @param link_id 链路ID
 *
 * @return  1 添加成功  0 当前系统支持链路已满
 */
static int iec_sys_add_link(struct sys_info *info,void *link)
{
  if(arraylist_size(info->sys_link_obj)>CFG_LINK_MAX)
    {
      rt_kprintf("IEC:LINK: obj's buffer is full.\n");
      return 0;
    }

  arraylist_add(info->sys_link_obj, link);
  return 1;
}

static int iec_sys_add_app(struct sys_info *info,void *app)
{
  if(arraylist_size(info->sys_app_obj)>CFG_APP_MAX)
    {
      rt_kprintf("IEC:APP: obj's buffer is full.\n");
      return 0;
    }

  arraylist_add(info->sys_app_obj, app);
  return 1;
}



WEAK void iec_sys_create_app_complete(char *name,unsigned int appid)
{
}

WEAK void iec_sys_create_link_complete(char *name,unsigned int linkid)
{
}


WEAK void iec_sys_start_app_complete(char *name,unsigned int appid)
{
}


/**
 * 创建链路实例事件处理函数
 *
 * @param info 系统信息实例
 * @param evt  事件 包含链路创建时所需信息
 */
static void iec_sys_evt_create_link_handle(struct sys_info *info,struct iec_event *evt)
{
  int sub_evt=evt->evt_sub_type;

  struct link_param *link_p=0;
  struct serial_link_info *serial_link=0;
  struct net_link_info *net_link=0;

  switch(sub_evt)
    {
    case EVT_SUB_SYS_LINK_SERIAL:
      link_p=evt->sub_msg;
      serial_link=serial_link_create(link_p->name,link_p->link_addr,link_p->link_addr_len,link_p->link_dir);
      if(iec_sys_add_link(info, serial_link)==0) /*系统支持链路缓存已满  */
        {
          serial_link_del(serial_link);
          return;
        }
      iec_sys_create_link_complete(serial_link->obj.name, (unsigned int)serial_link);
      break;
    case EVT_SUB_SYS_LINK_SOCKET:
      link_p=evt->sub_msg;
      net_link=net_link_create(link_p->name,link_p->link_addr,link_p->link_dir);
      if(iec_sys_add_link(info, net_link)==0)
        {
          net_link_del(net_link);
          return;
        }
      iec_sys_create_link_complete(net_link->obj.name, (unsigned int)net_link);
      break;
    }
}


static void iec_sys_evt_create_app_handle(struct sys_info *info,struct iec_event *evt)
{
  int sub_evt=evt->evt_sub_type;

  struct app_param *app=evt->main_msg;
  struct app_info *app_i=0;
  app_i=(struct app_info *)app_create(app->name,app->asdu_addr,app->asdu_addr_len, app->cause_len, app->node_addr_len,app->sm2_enable);
   if(iec_sys_add_app(info, app_i)==0) /*系统支持链路缓存已满  */
        {
          return;
        }
   iec_sys_create_app_complete(app_i->cfg.name, (unsigned int)app_i);
}

/**
 * 系统启动事件处理函数
 *
 * @param info 系统信息实例
 * @param evt 事件  包含需启动链路或APP任务标识
 */
static void iec_sys_evt_start_handle(struct sys_info *info,struct iec_event *evt)
{
  int sub_evt=evt->evt_sub_type;
  struct link_obj *link=(struct link_obj *)(evt->sub_msg);
  struct app_info *app=(struct app_info *)(evt->sub_msg);

  switch(sub_evt)
    {
    case EVT_SUB_SYS:
      break;
    case EVT_SUB_SYS_APP:
      app_start((int)evt->sub_msg);
      iec_sys_start_app_complete(app->cfg.name,(unsigned int)app);
      break;
    case EVT_SUB_SYS_LINK_SERIAL:
      serial_link_thread_start((struct serial_link_info *)link);
      break;
    case EVT_SUB_SYS_LINK_SOCKET:
      net_link_thread_start((struct net_link_info*)link);
      break;
    }
}


static void iec_sys_app_bind_link(struct app_info *app,struct link_obj *link)
{
  link->applayer_id=(unsigned int)app;
   app_add_link(app,(unsigned int)link);
}

static void iec_sys_evt_edit_handle(struct sys_info *info,struct iec_event *evt)
{
  int sub_evt=evt->evt_sub_type;
  int *param=0;
  switch(sub_evt)
  {
  case EVT_SUB_SYS_BIND:
    param=evt->sub_msg;
   iec_sys_app_bind_link((struct app_info *)param[0],(struct link_obj *)param[1]);
    break;
  case EVT_SUB_APP_CTRL_CMD:
    param=evt->sub_msg;
    app_set_cmd_cb((struct app_info*)param[0],EVT_SUB_APP_CTRL_CMD, (void *)(ctrl_cmd_proc)param[1]);
    break;
  case EVT_SUB_APP_PARAM_CMD:
    param=evt->sub_msg;
    app_set_cmd_cb((struct app_info*)param[0],EVT_SUB_APP_PARAM_CMD, (void *)(param_cmd_proc)param[1]);
    break;
  case EVT_SUB_APP_FILE_CMD:
    param=evt->sub_msg;
    app_set_cmd_cb((struct app_info*)param[0],EVT_SUB_APP_FILE_CMD, (void *)(file_cmd_proc)param[1]);
    break;
  case EVT_SUB_APP_SYS_CMD:
    param=evt->sub_msg;
    app_set_cmd_cb((struct app_info*)param[0],EVT_SUB_APP_SYS_CMD, (void *)(sys_cmd_proc)param[1]);
    break;

  }
}

/// <summary>
/// 主任务
/// </summary>
/// <param name="param">The parameter.</param>
void iec_main_thread_entry(void *param)
{
	struct sys_info *info = (struct sys_info*)param;

	rt_kprintf("IEC:SYS: system startup.\n");

	struct iec_event *evt = 0;

	while (1)
	{
		evt=iec_recv_event(info->sys_event, RT_WAITING_FOREVER);

    if(evt==0)
      continue;

		switch (evt->evt_type)
		{
		case EVT_SYS_CREATE_LINK:
      iec_sys_evt_create_link_handle(info, evt);
			break;
		case EVT_SYS_CREATE_APP:
      iec_sys_evt_create_app_handle(info,evt);
			break;
		case EVT_SYS_DEL_LINK:
			break;
		case EVT_SYS_DEL_APP:
			break;
    case EVT_SYS_START:
      iec_sys_evt_start_handle(info, evt);
      break;
    case EVT_SYS_EDIT_PROFILE:
      iec_sys_evt_edit_handle(info,evt);
      break;
		default:
			break;
		}
    iec_free_event(evt);
	}

}



/// <summary>
/// 系统初始化
/// </summary>
void iec_sys_api_init_sysinfo()
{
	gSys_Info.communicate_role = CFG_ROLE_MODE;
	gSys_Info.sys_event = rt_mb_create("sysevt", MAX_EVENT_COUNT, RT_IPC_FLAG_FIFO);

  gSys_Info.sys_app_obj=arraylist_create();
  gSys_Info.sys_link_obj=arraylist_create();


}

/// <summary>
/// 系统启动
/// </summary>
void iec_sys_api_start_sys()
{
  rt_thread_t tid=rt_thread_create("sys", iec_main_thread_entry,&gSys_Info,MAIN_THREAD_STACK_SIZE,MAIN_THREAD_PROI,MAIN_THREAD_TICK);
  rt_thread_startup(tid);
}


/***************** API ************************/

/**
 * 创建链路实例---用户接口
 *
 * @param link_name  链路实例名称
 * @param link_type  链路类型  1 串行   2 网络eth
 * @param link_addr  链路地址  网络类型时 表示socket标号
 * @param link_addr_len  链路地址长度
 * @param dir   平衡传输或非平衡传输
 */
void iec_sys_api_create_link(char *link_name,int link_type,unsigned int link_addr,int link_addr_len,int dir)
{
  struct iec_event *evt=iec_create_event(0, (int)&gSys_Info, EVT_SYS_CREATE_LINK, 0, 0);
  if(link_type==EVT_SUB_SYS_LINK_SERIAL)// serial
    {
      struct link_param *link=rt_malloc(sizeof(struct link_param));
      rt_memset(link,0,sizeof(struct link_param));
      link->link_addr=link_addr;
      link->link_addr_len=link_addr_len;
      link->link_dir=dir;
      rt_memcpy(link->name, link_name, rt_strlen(link_name));
      iec_set_event_sub(evt, EVT_SUB_SYS_LINK_SERIAL, (int*)link, 1);
      iec_post_event(gSys_Info.sys_event, evt, 20);
    }
  else if(link_type==EVT_SUB_SYS_LINK_SOCKET) //socket
    {
      struct link_param *link=rt_malloc(sizeof(struct link_param));
      rt_memset(link,0,sizeof(struct link_param));
      link->link_addr=link_addr;
      link->link_addr_len=0;
      link->link_dir=dir;
      rt_memcpy(link->name, link_name, rt_strlen(link_name));
      iec_set_event_sub(evt, EVT_SUB_SYS_LINK_SOCKET, (int*)link, 1);
      iec_post_event(gSys_Info.sys_event, evt, 20);
    }
}

/**
 * 启动链路任务 --- 用户接口
 *
 * @param type  链路类型
 * @param link_name  对应链路名称
 */
void iec_sys_api_start_link(char *link_name)
{
  struct link_obj *link=0;
  link=iec_sys_find_link(&gSys_Info, link_name);
  int sub_evt=0;

  if(link==0)
    return;

  struct iec_event *evt=iec_create_event(0, (int)&gSys_Info, EVT_SYS_START, 0, 0);
  iec_set_event_sub(evt, link->link_type,(int*)link, 0);
  iec_post_event(gSys_Info.sys_event, evt, 20);
}


void iec_sys_api_create_app(char *name,unsigned int asdu_addr,unsigned int asdu_addr_len,unsigned int cause_len,
                            unsigned int node_addr_len,unsigned int sm2)
{
    struct app_param *app=rt_malloc(sizeof(struct app_param));
    rt_memset(app,0,sizeof(struct app_param));
    app->asdu_addr=asdu_addr;
    app->asdu_addr_len=asdu_addr_len;
    app->cause_len=cause_len;
    app->node_addr_len=node_addr_len;
    app->sm2_enable=sm2;
    rt_memcpy(app->name, name, rt_strlen(name));
    struct iec_event *evt=iec_create_event(0, (int)&gSys_Info, EVT_SYS_CREATE_APP, (int *)app, 1);
    iec_post_event(gSys_Info.sys_event, evt, 20);
}

void iec_sys_api_start_app(char *app_name)
{
  struct app_info *app=0;
  app=iec_sys_find_app(&gSys_Info, app_name);
  int sub_evt=0;

  if(app==0)
    return;

  struct iec_event *evt=iec_create_event(0, (int)&gSys_Info, EVT_SYS_START, 0, 0);
  iec_set_event_sub(evt, EVT_SUB_SYS_APP,(int*)app, 0);
  iec_post_event(gSys_Info.sys_event, evt, 20);
}


void iec_sys_api_app_bind_link(char *link_name,char *app_name)
{
  struct app_info *app=0;
  struct link_obj *link=0;
  app=iec_sys_find_app(&gSys_Info, app_name);
  int sub_evt=0;
  int *bind_info=rt_malloc(sizeof(int)*2);

  if(app==0)
    return;

  link=iec_sys_find_link(&gSys_Info, link_name);

  if(link==0)
    return;

  bind_info[0]=(int)app;
  bind_info[1]=(int)link;
  struct iec_event *evt=iec_create_event(0, (int)&gSys_Info, EVT_SYS_EDIT_PROFILE, 0, 0);
  iec_set_event_sub(evt, EVT_SUB_SYS_BIND,bind_info, 1);
  iec_post_event(gSys_Info.sys_event, evt, 20);
}

void iec_sys_api_app_set_cmd_cb(char *app_name,int cb_idx,void *cb)
{
  unsigned int *param=rt_malloc(sizeof(unsigned int)*2);
  struct app_info *app=iec_sys_find_app(&gSys_Info, app_name);
  if(app==0)
    return;

  param[0]=(unsigned int)app;
  param[1]=(unsigned int)cb;

  struct iec_event *evt=iec_create_event(0, (int)&gSys_Info,EVT_SYS_EDIT_PROFILE , 0, 0);
  iec_set_event_sub(evt,cb_idx,(int*)param,1);
  iec_post_event(gSys_Info.sys_event, evt, 20);
}

void iec_sys_api_send_phyid_recv(struct link_obj *link,char *buff, int len)
{
  struct link_recv_info *recv_info=rt_malloc(sizeof(struct link_recv_info));

  recv_info->recv_data=buff;
  recv_info->recv_len=len;

  struct iec_event *evt=iec_create_event(0, (int)&gSys_Info, EVT_LINK_RECV_DATA, 0, 0);
  iec_set_event_sub(evt, EVT_SUB_DAT_LINK_PHY,(int*)recv_info, 1);
  iec_post_event(link->mb_event, evt, 20);
}

void iec_sys_api_send_phy_recv(char *name,char *buff,int len)
{
  struct link_obj *link=iec_sys_find_link(&gSys_Info, name);
  if(link==0)
    return;
  iec_sys_api_send_phyid_recv(link,buff,len);
}

struct app_info* iec_sys_api_find_app(char *name)
{
  return iec_sys_find_app(&gSys_Info,name);
}

struct link_obj* iec_sys_api_find_link(char *name)
{
  return iec_sys_find_link(&gSys_Info,name);
}

void iec_sys_api_netlink_send_close(struct net_link_info *net_link)
{
   struct iec_event *evt=iec_create_event((int)net_link, (int)net_link, EVT_LINK_PHY_DISCONNECT, 0, 0);
  iec_set_event_sub(evt, EVT_SUB_DAT_LINK_PHY,(int*)0, 0);
  iec_post_event(net_link->obj.mb_event, evt, 20);
}