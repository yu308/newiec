#include "../../OS/os_helper.h"
#include "../Helper/layer_helper.h"

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void net_link_thread_entry(void *param);
#define NET_THREAD_PROI      (14)
#define NET_THREAD_STACK_SIZE  (1024)
#define NET_THREAD_TICK		(50)
#define MAX_EVENT_COUNT			(5)
#endif

static void net_link_time_counter_add(struct net_link_info *net_link)
{
  rt_mutex_take(net_link->cfg.time_mtx,RT_WAITING_FOREVER);
  net_link->t0_delay++;
  net_link->t1_delay++;
  net_link->t2_delay++;
  net_link->t3_delay++;
  rt_mutex_release(net_link->cfg.time_mtx);
}

static void net_link_time_counter_dec(struct net_link_info *net_link)
{
  rt_mutex_take(net_link->cfg.time_mtx,RT_WAITING_FOREVER);
  net_link->t0_delay=0;
  net_link->t1_delay=0;
  net_link->t2_delay=0;
  net_link->t3_delay=0;
  rt_mutex_release(net_link->cfg.time_mtx);

}

static void net_link_counter_add(struct net_link_info *net_link,int counter_t)
{
  rt_mutex_take(net_link->cfg.counter_mtx,RT_WAITING_FOREVER);
  if(counter_t==1)//recv
    {
      net_link->recv_counter++;
    }
  else if(counter_t==2)
    {
      net_link->sent_counter++;
    }
  rt_mutex_release(net_link->cfg.counter_mtx);
}

static int net_link_counter_check(struct net_link_info *net_link,int counter_t,int val)
{
  rt_mutex_take(net_link->cfg.counter_mtx,RT_WAITING_FOREVER);
  if(counter_t==1)
    {
      return (net_link->recv_counter==val);
    }
  else if(counter_t==2)
    {
      return (net_link->sent_counter==val);
    }

  return 0;
}

static void net_link_send_timeout(struct net_link_info *net_link)
{
  struct iec_event *evt=iec_create_event(net_link,net_link,EVT_IEC_TIMEOUT,0,0);
  iec_set_event_sub(evt, EVT_SUB_IEC_T3_TIMEOUT,0, 0);
  iec_post_event(net_link->obj.mb_event, evt, 20);
}

static void net_link_time_monitor(void *param)
{
  struct net_link_info *net_link=(struct net_link_info *)param;
  rt_err_t err;
  while(1)
    {
      rt_thread_delay(RT_TICK_PER_SECOND);
      net_link_time_counter_add(net_link);

      if(net_link->t3_delay>=CFG_IEC104_T3)
        {
          net_link_send_timeout(net_link);
        }
    }
}

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
  net_info->cfg.time_monitor=rt_thread_create("tmonitor",net_link_time_monitor,net_info,NET_THREAD_STACK_SIZE,
                                              NET_THREAD_PROI,NET_THREAD_PROI);
  net_info->cfg.time_mtx=rt_mutex_create("tmtx",RT_IPC_FLAG_FIFO);
#if(CFG_RUNNING_MODE==MUTLI_MODE)
  net_info->obj.mb_event=rt_mb_create("netmb", 4, RT_IPC_FLAG_FIFO);
#endif
  return net_info;
}

void net_link_del(struct net_link_info *net_info)
{
  rt_free(net_info->obj.send_buff);
  rt_free(net_info->obj.recv_buff);

  rt_thread_delete(net_info->cfg.time_monitor);
  rt_mutex_delete(net_info->cfg.time_mtx);
#if(CFG_RUNNING_MODE==MUTLI_MODE)
  if(net_info->obj.mb_event!=0)
    rt_mb_delete(net_info->obj.mb_event);
  if(net_info->obj.tid!=0)
    rt_thread_delete(net_info->obj.tid);
#endif
}

static int net_link_base_check(char *buff,unsigned int len)
{
  if(buff[0]!=HEAD_START_TAG)
    {
      rt_kprintf("NET_LINK: not IEC104 frame\n");
      return -1;
    }

  if(buff[1]!=(len-2))
    {
      rt_kprintf("NET_LINK: frame's length is wrong\n");
      return -2;
    }

  return 0;
}


static int net_link_check_phy_data(struct net_link_info *net_link,char *buff,unsigned int len)
{
  int res=0,format=0xFF;
  union control_domain ctrl;

  res=net_link_base_check(buff,len);

  if(res<0)
    return 0;

  net_link_time_counter_dec(net_link);

  rt_memcpy(ctrl.bytes,&buff[2],4);

  format=ctrl.bytes[0]&0x3;

 

  if(format==F104_I_FORMAT_BYTE||format==F104_I_FORMAT_BYTE_2)
    {
      if(net_link_counter_check(net_link,1,ctrl.i_control_domain.sent_num)==1)
        {
          net_link_counter_add(net_link,1);
          return F104_I_FORMAT_BYTE_2;
        }
      else
        return -1;
    }
  else if(format==F104_S_FORMAT_BYTE)
    {
      if(net_link_counter_check(net_link,2,ctrl.s_control_domain.recv_num)==0)
        return -1;

      return F104_S_FORMAT_BYTE;
    }
  else if(format==F104_U_FORMAT_BYTE)
    {
      if(ctrl.u_control_domain.sbit_act==1)
        {
          net_link->start=1;
        }
      else if(ctrl.u_control_domain.stopbit_act==1)
        {
          net_link->start=0;
        }

      return F104_U_FORMAT_BYTE;
    }
}

static int net_link_pack_U_frame(struct net_link_info *net_link,union control_domain ctrl)
{
  union control_domain ack_ctrl;
  rt_memset(ack_ctrl.bytes,0,4);
  net_link->obj.send_buff[0]=HEAD_START_TAG;

  if(ctrl.u_domain.sbit_act==1)
    ack_ctrl.u_domain.sbit_confirm=1;
  if(ctrl.u_domain.stopbit_act==1)
    ack_ctrl.u_domain.stopbit_confirm=1;
  if(ctrl.u_domain.tbit_act==1)
    ack_ctrl.u_domain.tbit_confirm=1;

  rt_memcpy(&net_link->obj.send_buff[2],ack_ctrl.bytes,4);
  net_link->obj.send_buff[1]=4;

  return 6;
}

static void net_link_phy_recv_dispatch(struct net_link_info *net_link,int format,char *apci_data,int len)
{
  union control_domain ctrl;
  rt_memcpy(ctrl.bytes,apci_data,4);
  char *asdu_buff=0;
  int ack_len=0;

  if(format==F104_I_FORMAT_BYTE_2)
    {
      if(net_link->cfg.start==1)/*链路启用*/
        {
          asdu_buff=rt_malloc(len-4);
          rt_memcpy(asdu_buff,&apci_data[4],len-4);
          link_send_asdu_evt_to_app(net_link,asdu_buff,len-4);
        }
    }
  else if(format==F104_U_FORMAT_BYTE)
    {
      ack_len=net_link_pack_U_frame(net_link,ctrl);
      net_link->obj.write(net_link->obj.send_buff,ack_len);
      //net_link_counter_add(net_link,2);//增加发送计数
    }
}

static void net_link_app_recv_dispatch(struct net_link_info *net_link,struct iec_event *evt)
{
  if(evt->sub_evt==EVT_SUB_DAT_USER)/* 控制方向回应 */
    {
      if(net_link->start==1)
        link_send_req_evt_to_app(net_link,evt->sub_evt);
    }
  else
    {
      if(net_link->obj.active==1)/*总招完成?*/
        {
          if(link_get_dir(net_link)==1)/*平衡模式*/
            {
              link_send_req_evt_to_app(net_link,evt->sub_evt);
            }
          else
            {
              rt_kprintf("NET_LINK:iec104's dir=0?\n");
            }
        }
    }
}


static void net_link_recv_evt_handle(struct net_link_info *net_link,struct iec_event *evt)
{
    int sub_evt=evt->evt_sub_type;
    int format=0;
    struct link_recv_info *recv_info=evt->sub_msg;
    
    switch(sub_evt)
    {
    case EVT_SUB_DAT_LINK_PHY:
      format=net_link_check_phy_data(net_link,recv_info->recv_buff,recv_info->recv_len);
      if(format<0)
        {
          net_link_del(net_link);
          return;
        }
      net_link_phy_recv_dispatch(net_link,format,&recv_info->recv_buff[2],recv_info->recv_len-2);
        break;
    case EVT_SUB_DAT_LEVEL_1:
    case EVT_SUB_DAT_LEVEL_2:
    case EVT_SUB_DAT_USER:
      break;
      break;
    }
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
    case EVT_IEC_TIMEOUT:
      /*关闭socket*/
      net_link_del(net_link);
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
