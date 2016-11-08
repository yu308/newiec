#include "../../OS/os_helper.h"
#include "../layer.h"

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void net_link_thread_entry(void *param);
#define NET_THREAD_PROI      (14)
#define NET_THREAD_STACK_SIZE  (1024)
#define NET_THREAD_TICK		(50)
#define MAX_EVENT_COUNT			(5)
#endif


void net_link_time_enable(struct net_link_info *net_link,int t_idx,int enable)
{
  rt_mutex_take(net_link->cfg.time_mtx,RT_WAITING_FOREVER);
  
  if(t_idx==0)
    net_link->t0_start=enable;
  else
   if(t_idx==1)
    net_link->t1_start=enable;
   else
    if(t_idx==2)
    net_link->t2_start=enable;
    else
     if(t_idx==3)
    net_link->t3_start=enable;
  
  rt_mutex_release(net_link->cfg.time_mtx);
}


static void net_link_time0_counter(struct net_link_info *net_link)
{
  rt_mutex_take(net_link->cfg.time_mtx,RT_WAITING_FOREVER);
  
  if(net_link->t0_start==1)
    net_link->t0_delay++;
  else 
    net_link->t0_delay=0;
  
   rt_mutex_release(net_link->cfg.time_mtx);
}

static void net_link_time1_counter(struct net_link_info *net_link)
{
  rt_mutex_take(net_link->cfg.time_mtx,RT_WAITING_FOREVER);
  
  if(net_link->t1_start==1)
    net_link->t1_delay++;
  else 
    net_link->t1_delay=0;
  
   rt_mutex_release(net_link->cfg.time_mtx);
}

static void net_link_time2_counter(struct net_link_info *net_link)
{
  rt_mutex_take(net_link->cfg.time_mtx,RT_WAITING_FOREVER);
  
  if(net_link->t2_start==1)
    net_link->t2_delay++;
  else 
    net_link->t2_delay=0;
  
   rt_mutex_release(net_link->cfg.time_mtx);
}

static void net_link_time3_counter(struct net_link_info *net_link)
{
  rt_mutex_take(net_link->cfg.time_mtx,RT_WAITING_FOREVER);
  
  if(net_link->t3_start==1)
    net_link->t3_delay++;
  else 
    net_link->t3_delay=0;
  
   rt_mutex_release(net_link->cfg.time_mtx);
}


/** 
 * 递增数据帧收发计数
 * 
 * @param net_link 
 * @param counter_t 1 接收计数  2发送计数
 */
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

/** 
 * 收发计数检测 若不等返回0
 * 
 * @param net_link 
 * @param counter_t 
 * @param val 
 * 
 * @return 
 */
static int net_link_counter_check(struct net_link_info *net_link,int counter_t,int val)
{
  rt_mutex_take(net_link->cfg.counter_mtx,RT_WAITING_FOREVER);
  if(counter_t==1)
    {
       rt_mutex_release(net_link->cfg.counter_mtx);
      return (net_link->recv_counter==val);
    }
  else if(counter_t==2)
    {
       rt_mutex_release(net_link->cfg.counter_mtx);
      return (net_link->sent_counter==val);
    }

   rt_mutex_release(net_link->cfg.counter_mtx);
  return 0;
}


/** 
 * 发送超时事件
 * 
 * @param net_link 
 */
static void net_link_send_timeout(struct net_link_info *net_link,int tim)
{
  struct iec_event *evt=iec_create_event((int)net_link,(int)net_link,EVT_IEC_TIMEOUT,0,0);
  iec_set_event_sub(evt, tim,0, 0);
  iec_post_event(net_link->obj.mb_event, evt, 20);
}

static void net_link_send_cnt_full(struct net_link_info *net_link)
{
  struct iec_event *evt=iec_create_event((int)net_link, (int)net_link, EVT_IEC_CNT_FULL, 0, 0);
  iec_set_event_sub(evt, EVT_SUB_IEC_K_FULL, 0, 0);
  iec_post_event(net_link->obj.mb_event, evt, 20);
}
/** 
 * 超时计数监测任务
 * 
 * @param param 
 */
static void net_link_time_monitor(void *param)
{
  struct net_link_info *net_link=(struct net_link_info *)param;
  rt_err_t err;
  while(1)
    {
      rt_thread_delay(RT_TICK_PER_SECOND);
      net_link_time0_counter(net_link);
      net_link_time1_counter(net_link);
      net_link_time2_counter(net_link);
      net_link_time3_counter(net_link);
      
 
        if(net_link->t0_delay>=CFG_IEC104_T0)
        {
          net_link_send_timeout(net_link,EVT_SUB_IEC_T0_TIMEOUT);
        }
        else if(net_link->t1_delay>=CFG_IEC104_T1)
        {
          net_link_send_timeout(net_link,EVT_SUB_IEC_T1_TIMEOUT);
        }
        else if(net_link->t2_delay>=CFG_IEC104_T2)
        {
          net_link_send_timeout(net_link,EVT_SUB_IEC_T2_TIMEOUT);
        }
        else if(net_link->t3_delay>=CFG_IEC104_T3)
        {
          net_link_send_timeout(net_link,EVT_SUB_IEC_T3_TIMEOUT);
        }
    }
}

/** 
 * 创建net链路
 * 
 * @param name 
 * @param socketid 
 * @param dir 
 * 
 * @return 
 */
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
  net_info->cfg.counter_mtx=rt_mutex_create("cntr",RT_IPC_FLAG_FIFO);
#if(CFG_RUNNING_MODE==MUTLI_MODE)
  net_info->obj.mb_event=rt_mb_create("netmb", 4, RT_IPC_FLAG_FIFO);
#endif
  return net_info;
}


/** 
 * 删除net链路
 * 
 * @param net_info 
 */
void net_link_del(struct net_link_info *net_info)
{
   net_info->obj.active=0;
   app_remove_link((struct app_info*)(net_info->obj.applayer_id),(unsigned int)net_info);
  rt_free(net_info->obj.send_buff);
  rt_free(net_info->obj.recv_buff);

  rt_thread_suspend(net_info->cfg.time_monitor);
  rt_thread_delete(net_info->cfg.time_monitor);
  rt_mutex_delete(net_info->cfg.time_mtx);
  rt_mutex_delete(net_info->cfg.counter_mtx);
#if(CFG_RUNNING_MODE==MUTLI_MODE)
  if(net_info->obj.mb_event!=0)
    rt_mb_delete(net_info->obj.mb_event);
  if(net_info->obj.tid!=0)
    rt_thread_delete(net_info->obj.tid);
#endif
  
  iec_sys_api_link_notify_del(net_info,net_info->obj.applayer_id);
}

/** 
 * 链路接收端数据基本特征检测
 * 
 * @param buff 
 * @param len 
 * 
 * @return 
 */
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

/** 
 * 检测链路接收端接收的数据
 *  帧格式、长度、计数检测
 * @param net_link 
 * @param buff 
 * @param len 
 * 
 * @return 接收的数据帧类型
 */
static int net_link_check_phy_data(struct net_link_info *net_link,char *buff,unsigned int len)
{
  int res=0,format=0xFF;
  union control_domain ctrl;

  res=net_link_base_check(buff,len);

  if(res<0)
    return 0;

  //net_link_time_counter_dec(net_link);

  rt_memcpy(ctrl.bytes,&buff[2],4);

  format=ctrl.bytes[0]&0x3;

 

  if(format==F104_I_FORMAT_BYTE||format==F104_I_FORMAT_BYTE_2)
    {
      if(net_link_counter_check(net_link,1,ctrl.i_domain.sent_num)==1)
        {
          net_link_counter_add(net_link,1);
          return F104_I_FORMAT_BYTE_2;
        }
      else
        return -1;
    }
  else if(format==F104_S_FORMAT_BYTE)
    {
      if(net_link_counter_check(net_link,2,ctrl.s_domain.recv_num)==0)
        return -1;

      net_link->current_k=0;
      return F104_S_FORMAT_BYTE;
    }
  else if(format==F104_U_FORMAT_BYTE)
    {
      if(ctrl.u_domain.sbit_act==1)
        {
          net_link->obj.active=1;
        }
      else if(ctrl.u_domain.stopbit_act==1)
        {
          net_link->obj.active=0;
        }

      return F104_U_FORMAT_BYTE;
    }

  return 0;
}

/** 
 *  生成U帧数据
 * 
 * @param net_link 
 * @param ctrl 
 * 
 * @return 数据长度
 */
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
  
  ack_ctrl.u_domain.flag=F104_U_FORMAT_BYTE;
  rt_memcpy(&net_link->obj.send_buff[2],ack_ctrl.bytes,4);
  net_link->obj.send_buff[1]=4;

  return 6;
}

/** 
 *  生成U帧数据
 * 
 * @param net_link 
 * @param ctrl 
 * 
 * @return 数据长度
 */
static int net_link_pack_S_frame(struct net_link_info *net_link)
{
  union control_domain ack_ctrl;

  net_link->obj.send_buff[0]=HEAD_START_TAG;
  ack_ctrl.s_domain.recv_num=net_link->recv_counter;
  ack_ctrl.s_domain.unused_bit=0;
  ack_ctrl.s_domain.flag=F104_S_FORMAT_BYTE;
  rt_memcpy(&net_link->obj.send_buff[2],ack_ctrl.bytes,4);
  net_link->obj.send_buff[1]=4;

  return 6;
}

/** 
 * 链路接收端数据分发处理
 * 
 * @param net_link 
 * @param format 
 * @param apci_data 
 * @param len 
 */
static void net_link_phy_recv_dispatch(struct net_link_info *net_link,int format,char *apci_data,int len)
{
  union control_domain ctrl;
  rt_memcpy(ctrl.bytes,apci_data,4);
  char *asdu_buff=0;
  int ack_len=0;
  
   /*  */
  net_link_time_enable(net_link,3,0);//首先清0
  net_link_time3_counter(net_link);
  
  if(format==F104_I_FORMAT_BYTE_2)
    {
      if(net_link->obj.active==1)/*链路启用*/
        {
          /* W 接收计数确认*/
          net_link->current_w++;
          net_link_time_enable(net_link,2,0);//首先清0
          net_link_time2_counter(net_link);
          if(net_link->current_w==FRAME_COUNT_W)
          {
            ack_len=net_link_pack_S_frame(net_link);
            net_link->obj.write(net_link->cfg.socket,net_link->obj.send_buff,ack_len);
            net_link->current_w=0;
          }
          else
          {
            net_link_time_enable(net_link,2,1);//重新计数
          }
          
          asdu_buff=rt_malloc(len-4);
          rt_memcpy(asdu_buff,&apci_data[4],len-4);
          link_send_asdu_evt_to_app((struct link_obj*)net_link,asdu_buff,len-4);
        }
    }
  else if(format==F104_U_FORMAT_BYTE)
    {
      net_link_time_enable(net_link,1,0);
       net_link_time1_counter(net_link);
      ack_len=net_link_pack_U_frame(net_link,ctrl);
      net_link->obj.write(net_link->cfg.socket,net_link->obj.send_buff,ack_len);
      
      //net_link_counter_add(net_link,2);//增加发送计数
    }
  else if(format==F104_S_FORMAT_BYTE)
  {
    net_link_time_enable(net_link,1,0);
     net_link_time1_counter(net_link);
  }
}

/** 
 * net链路接收端APP事件处理
 * 
 * @param net_link 
 * @param evt 
 */
static void net_link_app_recv_dispatch(struct net_link_info *net_link,struct iec_event *evt)
{
  if(evt->evt_sub_type==EVT_SUB_DAT_USER)/* 控制方向回应 */
    {
      if(net_link->obj.active==1)
      {
        evt->evt_sub_type=EVT_SUB_DAT_LEVEL_1;
        link_send_req_evt_to_app((struct link_obj*)net_link,evt->evt_sub_type);
      }
    }
  else
    {
      if(net_link->obj.data_trans_active==1)/*总招完成?*/
        {
          if(link_get_dir(&net_link->obj)==1)/*平衡模式*/
            {
              link_send_req_evt_to_app((struct link_obj*)net_link,evt->evt_sub_type);
            }
          else
            {
              rt_kprintf("NET_LINK:iec104's dir=0?\n");
            }
        }
    }
}

/** 
 * net链路接收端事件处理
 * 
 * @param net_link 
 * @param evt 
 */
static void net_link_recv_evt_handle(struct net_link_info *net_link,struct iec_event *evt)
{
    int sub_evt=evt->evt_sub_type;
    int format=0;
    struct link_recv_info *recv_info=evt->sub_msg;
    
    switch(sub_evt)
    {
    case EVT_SUB_DAT_LINK_PHY:
      format=net_link_check_phy_data(net_link,recv_info->recv_data,recv_info->recv_len);
      if(format<0)
        {
          //net_link_del(net_link);
          return;
        }
      net_link_phy_recv_dispatch(net_link,format,&recv_info->recv_data[2],recv_info->recv_len-2);
        break;
    case EVT_SUB_DAT_LEVEL_1:
    case EVT_SUB_DAT_LEVEL_2:
    case EVT_SUB_DAT_USER:
      net_link_app_recv_dispatch(net_link,evt);
      break;
      break;
    }
}

/** 
 * 生成I帧数据帧
 * 
 * @param net_link 
 * @param asdu_data 
 * @param len 
 * 
 * @return 
 */
static int net_link_pack_I_frame(struct net_link_info *net_link,char *asdu_data,unsigned int len)
{
  int idx=0;
  union control_domain ctrl;
  char *send_buff=net_link->obj.send_buff;

  send_buff[idx++]=HEAD_START_TAG;
  send_buff[idx++]=4+len;

  ctrl.i_domain.flag=F104_I_FORMAT_BYTE;
  ctrl.i_domain.recv_num=net_link->recv_counter;
  ctrl.i_domain.sent_num=net_link->sent_counter;
  ctrl.i_domain.unused_bit1=0;

  rt_memcpy(&send_buff[idx], ctrl.bytes, 4);
  idx+=4;

  rt_memcpy(&send_buff[idx], asdu_data, len);
  idx+=len;

  return idx;
}

/** 
 * 链路发送数据事件处理
 * 
 * @param net_link 
 * @param evt 
 */
static void net_link_send_evt_handle(struct net_link_info *net_link,struct iec_event *evt)
{
  int sub_evt=evt->evt_sub_type;
  struct app_send_info *send_info=evt->sub_msg;
  int count =0;
  switch(sub_evt)
    {
    case EVT_SUB_DAT_USER:
      if(app_task_check_empty(((struct app_info*)(net_link->obj.applayer_id))->first_task, (int)net_link)>0)
        {
          link_send_req_evt_to_app((struct link_obj*)net_link,evt->evt_sub_type);
        }
      count=net_link_pack_I_frame(net_link,send_info->app_data, send_info->app_data_len);
      rt_free(send_info->app_data);
      
      net_link_time_enable(net_link,1,0);
      net_link_time1_counter(net_link);
      break;
    }

  if(net_link->current_k<CFG_IEC104_K)
    {

      net_link->obj.write(net_link->cfg.socket, net_link->obj.send_buff,count);
      net_link_time_enable(net_link,1,1);
      net_link_counter_add(net_link,2);
      net_link->current_k++;
    }
  else
    net_link_send_cnt_full(net_link);
}

/** 
 * 通信 计数错误或超时错误时 链路通知socket接收任务关闭socket链接 
 * socket接收任务关闭socket后,再产生链接断开事件信息
 * @param net_link 
 */
void net_link_notify_close(struct net_link_info *net_link)
{
  rt_sem_release(net_link->cfg.socket_close_sem); 
}

static void net_link_timeout_evt_handle(struct net_link_info *net_link,struct iec_event *evt)
{
  int sub_evt=evt->evt_sub_type;
  int ack_len=0;
  union control_domain ctrl;
  switch(sub_evt)
  {
    case EVT_SUB_IEC_T0_TIMEOUT:
      break;
      case EVT_SUB_IEC_T1_TIMEOUT:
       rt_kprintf("monitor timer timeout.\n");
      net_link_notify_close(net_link);
    break;
      case EVT_SUB_IEC_T2_TIMEOUT:
        net_link_time_enable(net_link,2,0);
        net_link_time2_counter(net_link);
       ack_len=net_link_pack_S_frame(net_link);
       net_link->obj.write(net_link->cfg.socket,net_link->obj.send_buff,ack_len);
       net_link->current_w=0;
    break;
      case EVT_SUB_IEC_T3_TIMEOUT:
        rt_memset(ctrl.bytes,0,4);
        ctrl.u_domain.tbit_act=1;
        ack_len=net_link_pack_U_frame(net_link,ctrl);
        net_link->obj.write(net_link->cfg.socket,net_link->obj.send_buff,ack_len);
    break;
  }
}



#if(CFG_RUNNING_MODE==MUTLI_MODE)
void net_link_thread_entry(void *param)
{
	struct net_link_info *info = (struct net_link_info*)param;

	struct iec_event *evt = 0;
	rt_kprintf("IEC:NET LINK: %s link is running.\n", info->obj.name);

  if(info->cfg.time_monitor!=0)
    rt_thread_startup(info->cfg.time_monitor);

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
      net_link_del(info);
			break;
		case EVT_LINK_RECV_DATA:
      net_link_recv_evt_handle(info, evt);
			break;
		case EVT_LINK_SEND_DATA:
      net_link_send_evt_handle(info, evt);
			break;
    case EVT_IEC_TIMEOUT:
      net_link_timeout_evt_handle(info,evt);
      break;
    case EVT_IEC_CNT_FULL:
      rt_kprintf("K or W counter wrong.\n");
      net_link_notify_close(info);
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
