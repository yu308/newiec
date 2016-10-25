#include	"iec_cfg.h"
#include	"iec_event.h"
#include	"iec_sys.h"
#include "../Layer/Helper/layer_helper.h"
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
static int iec_sys_find_link(struct sys_info *info,char *name)
{
  int i=0;
  char link_name[16];

  for(i=0;i<CFG_LINK_MAX;i++)
    {
      rt_memcpy(&link_name,(char *)info->link_obj[i],16);
      if(rt_strcmp(name, link_name)==0)
        return info->link_obj[i];
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
static int iec_sys_add_link(struct sys_info *info,void *link_id)
{
  int i=0;

  for(i=0;i<CFG_LINK_MAX;i++)
    {
      if(info->link_obj[i]==0)
        {
          info->link_obj[i]=(int)link_id;
          return 1;
        }
    }

  rt_kprintf("IEC:LINK: obj's buffer is full.\n");
  return 0;
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
      break;
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
  int link_id=*(int *)(evt->sub_msg);

  switch(sub_evt)
    {
    case EVT_SUB_SYS:
      break;
    case EVT_SUB_SYS_APP:
      break;
    case EVT_SUB_SYS_LINK_SERIAL:
      serial_link_thread_start(link_id);
      break;
    case EVT_SUB_SYS_LINK_SOCKET:
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
			break;
		case EVT_SYS_DEL_LINK:
			break;
		case EVT_SYS_DEL_APP:
			break;
    case EVT_SYS_START:
      iec_sys_evt_start_handle(info, evt);
      break;
		default:
			break;
		}
	}

}



/// <summary>
/// 系统初始化
/// </summary>
void iec_sys_api_init_sysinfo()
{
	gSys_Info.communicate_role = CFG_ROLE_MODE;
	gSys_Info.sys_event = rt_mb_create("sysevt", MAX_EVENT_COUNT, RT_IPC_FLAG_FIFO);

  rt_memset(&gSys_Info.app_obj,0,CFG_APP_MAX);
  rt_memset(&gSys_Info.link_obj,0,CFG_LINK_MAX);
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
void iec_sys_api_start_link(int type,char *link_name)
{
  int *link_id=rt_malloc(sizeof(int));

  *link_id=iec_sys_find_link(&gSys_Info, link_name);

  int sub_evt=0;

  if(type==1) sub_evt=EVT_SUB_SYS_LINK_SERIAL;
  if(type==2) sub_evt=EVT_SUB_SYS_LINK_SOCKET;

  if(*link_id!=0)
    {
      struct iec_event *evt=iec_create_event(0, (int)&gSys_Info, EVT_SYS_START, 0, 0);
      iec_set_event_sub(evt, sub_evt, link_id, 1);
      iec_post_event(gSys_Info.sys_event, evt, 20);
    }
  else
    rt_free(link_id);
}
