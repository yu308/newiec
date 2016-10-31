#include	"iec_cfg.h"
#include	"iec_event.h"
#include	"iec_sys.h"
#include "../Layer/Helper/layer_helper.h"
/// <summary>
/// 全局系统信息
/// </summary>
struct sys_info  gSys_Info;
rt_sem_t iec_sys_init_sem_done;


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

  arraylist_iterate(info->link_obj, i, link)
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

  arraylist_iterate(info->app_obj, i, app)
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
  if(arraylist_size(info->link_obj)>CFG_LINK_MAX)
    { 
      rt_kprintf("IEC:LINK: obj's buffer is full.\n");
      return 0;
    }

  arraylist_add(info->link_obj, link);
  return 1;
}

static int iec_sys_add_app(struct sys_info *info,void *app)
{
  if(arraylist_size(info->app_obj)>CFG_APP_MAX)
    { 
      rt_kprintf("IEC:APP: obj's buffer is full.\n");
      return 0;
    }

  arraylist_add(info->app_obj, app);
  return 1;
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

      break;
    case EVT_SUB_SYS_LINK_SOCKET:
      link_p=evt->sub_msg;
      net_link=net_link_create(link_p->name,link_p->link_addr,link_p->link_dir);
      if(iec_sys_add_link(info, net_link)==0)
        {
          net_link_del(net_link);
        }
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

  switch(sub_evt)
    {
    case EVT_SUB_SYS:
      break;
    case EVT_SUB_SYS_APP:
      app_start(evt->sub_msg);
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
                  rt_sem_release(iec_sys_init_sem_done);
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
  
        gSys_Info.app_obj=arraylist_create();
        gSys_Info.link_obj=arraylist_create();
        
        iec_sys_init_sem_done=rt_sem_create("init",0,RT_IPC_FLAG_FIFO);

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

/*****************TEST************************/
void iec_serial_write(char *buff,int len)
{
   rt_device_t dev=rt_device_find("serial0");
   rt_device_write(dev,0,buff,len);
}

void iec_sys_send_phy_recv(char *name,char *buff,int len)
{
   struct link_obj *link=iec_sys_find_link(&gSys_Info, name);
   link->write=iec_serial_write;
    struct link_recv_info *recv_info=rt_malloc(sizeof(struct link_recv_info));
    
    recv_info->recv_data=buff;
    recv_info->recv_len=len;

  struct iec_event *evt=iec_create_event(0, (int)&gSys_Info, EVT_LINK_RECV_DATA, 0, 0);
  iec_set_event_sub(evt, EVT_SUB_DAT_LINK_PHY,(int*)recv_info, 1);
  iec_post_event(link->mb_event, evt, 20);
}


void iec_create_all_node()
{
 struct app_info *app=iec_sys_find_app(&gSys_Info,"app");
 iec_api_create_node((unsigned int )app,0,0);
 iec_api_create_node((unsigned int )app,0x4001,0);
 iec_api_create_node((unsigned int )app,0x4002,0);
 iec_api_create_node((unsigned int )app,0x4003,0);
 iec_api_create_node((unsigned int )app,0x4004,0);
 iec_api_create_node((unsigned int )app,0x4005,0);
 iec_api_create_node((unsigned int )app,0x4006,0);
 iec_api_create_node((unsigned int )app,0x4007,0);
 iec_api_create_node((unsigned int )app,0x4008,0);
 iec_api_create_node((unsigned int )app,0x4009,0);
 iec_api_create_node((unsigned int )app,0x400A,0);
 iec_api_create_node((unsigned int )app,0x400B,0);
 
 iec_api_create_node((unsigned int )app,0x1,0);
}


void iec_update_node()
{
  
}

void iec_test_sys()
{
  rt_device_t dev=rt_device_find("serial0");
   rt_device_open(dev, RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_DMA_RX);
   rt_device_control(dev,0xEE,0);
  
 iec_sys_api_create_link("serial",EVT_SUB_SYS_LINK_SERIAL,1,1,0);
 iec_sys_api_create_app("app",1,1,1,2,0);
 rt_sem_take(iec_sys_init_sem_done,0x100000);
 iec_sys_api_app_bind_link("serial","app");
 iec_sys_api_start_app("app");
 iec_sys_api_start_link("serial");
 iec_create_all_node();
 
}

void iec_recv_data(char *buff,int len)
{
 
  rt_device_t dev=rt_device_find("serial0");
  rt_device_control(dev,0xEE,0);
  iec_sys_send_phy_recv("serial",buff,len);
}