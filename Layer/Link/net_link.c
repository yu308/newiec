#include "../../OS/os_helper.h"
#include "../Helper/layer_helper.h"

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void net_link_thread_entry(void *param);
#define NET_THREAD_PROI      (14)
#define NET_THREAD_STACK_SIZE  (1024)
#define NET_THREAD_TICK		(50)
#define MAX_EVENT_COUNT			(5)
#endif


#if(CFG_RUNNING_MODE==MUTLI_MODE)
void net_link_thread_entry(void *param)
{
	struct net_link_info *info = (struct net_link_info*)param;

	struct iec_event *evt = 0;
	rt_kprintf("IEC:NET LINK: %s link is running.\n", info->obj.name);

	while (1)
	{
		evt =iec_recv_event(info->mb_event, RT_WAITING_FOREVER);
		if (evt == 0)
			continue;

		switch (evt->evt_type)
		{
		case EVT_LINK_PHY_CONNECT:
			break;
		case EVT_LINK_PHY_DISCONNECT:
			break;
		case EVT_LINK_RECV_DATA:
			serial_link_recv_event_handle(info, evt);
			break;
		case EVT_LINK_SEND_DATA:
      serial_link_asdu_send_evt_handle(info, evt);
			break;
		default:
			break;
		}

		iec_free_event(evt);
	}
}

void net_link_thread_start(int plink_info)
{
	struct net_link_info *info = (struct net_link_info *)plink_info;
	info->obj.tid=rt_thread_create("sck", net_link_thread_entry, info, NET_THREAD_STACK_SIZE, NET_THREAD_PROI, NET_THREAD_TICK);
	rt_thread_startup(info->obj.tid);	
}
#endif