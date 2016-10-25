#include "../../OS/os_helper.h"
#include "../Helper/layer_helper.h"

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void net_link_thread_entry(void *param);
#define NET_THREAD_PROI      (14)
#define NET_THREAD_STACK_SIZE  (1024)
#define NET_THREAD_TICK		(50)
#define MAX_EVENT_COUNT			(5)
#endif

struct net_link_info *net_link_create(char *name,int socketid,int dir)
{
  struct net_link_info *net_info=rt_malloc(sizeof(struct net_link_info));

  if(net_info==0)
    {
      rt_kprintf("net_link can't created \n");
      return 0;
    }

  rt_memset(net_info, 0, sizeof(struct net_link_info));

  net_info->obj.double_dir=dir;
  net_info->obj.link_type=EVT_SUB_SYS_LINK_SOCKET;
  net_info->obj.recv_buff=rt_malloc(256);
  net_info->obj.send_buff=rt_malloc(256);
  net_info->obj.active=0;
  rt_memcpy(net_info->obj.name, name, rt_strlen(name));

  net_info->cfg.socket=socketid;

#if(CFG_RUNNING_MODE==MUTLI_MODE)
  net_info->obj.mb_event=rt_mb_create("netmb", 4, RT_IPC_FLAG_FIFO);
#endif
  return net_info;
}

void net_link_del(struct net_link_info *net_info)
{
  rt_free(net_info->obj.send_buff);
  rt_free(net_info->obj.recv_buff);

#if(CFG_RUNNING_MODE==MUTLI_MODE)
  if(net_info->obj.mb_event!=0)
    rt_mb_delete(net_info->obj.mb_event);
  if(net_info->obj.tid!=0)
    rt_thread_delete(net_info->obj.tid);
#endif
}

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void net_link_thread_entry(void *param)
{
	struct net_link_info *info = (struct net_link_info*)param;

	struct iec_event *evt = 0;
	rt_kprintf("IEC:NET LINK: %s link is running.\n", info->obj.name);

	while (1)
	{
		evt =iec_recv_event(info->obj.mb_event, RT_WAITING_FOREVER);
		if (evt == 0)
			continue;

		switch (evt->evt_type)
		{
		case EVT_LINK_PHY_CONNECT:
			break;
		case EVT_LINK_PHY_DISCONNECT:
			break;
		case EVT_LINK_RECV_DATA:
			break;
		case EVT_LINK_SEND_DATA:
			break;
		default:
			break;
		}

		iec_free_event(evt);
	}
}

void net_link_thread_start(struct net_link_info *net_link)
{
	net_link->obj.tid=rt_thread_create("sck", net_link_thread_entry, net_link, NET_THREAD_STACK_SIZE, NET_THREAD_PROI, NET_THREAD_TICK);
	rt_thread_startup(net_link->obj.tid);	
}
#endif
