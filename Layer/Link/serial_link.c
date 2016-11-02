#include "../../OS/os_helper.h"
#include "../layer.h"

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void serial_link_thread_entry(void *param);
#define SERIAL_THREAD_PROI      (15)
#define SERIAL_THREAD_STACK_SIZE  (1024)
#define SERIAL_THREAD_TICK		(50)
#define MAX_EVENT_COUNT			(5)
#endif


/// <summary>
/// 获取链路地址.
/// </summary>
/// <returns>链路地址</returns>
int serial_link_get_addr(struct serial_link_cfg *cfg)
{
	return cfg->link_addr;
}

/// <summary>
/// 获取链路地址长度
/// </summary>
/// <returns>链路地址长度</returns>
int serial_link_get_addr_len(struct serial_link_cfg *cfg)
{
	return cfg->link_addr_len;
}

/// <summary>
/// 串行方式下设置链路地址
/// </summary>
/// <param name="addr">The addr.</param>
void serial_link_set_addr(struct serial_link_cfg *cfg, int addr)
{
	cfg->link_addr=addr;
}


/// <summary>
/// 串行方式下设置链路地址长度.
/// </summary>
/// <param name="len">The length.</param>
void serial_link_set_addr_len(struct serial_link_cfg *cfg, int len)
{
	cfg->link_addr_len = len;
}


/// <summary>
/// 初始化已有链路
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="addr">The addr.</param>
/// <param name="addr_len">The addr_len.</param>
/// <param name="dir">The dir.</param>
void serial_link_init(struct serial_link_info *info, char *name,int addr, int addr_len, int dir)
{
	rt_memset(info, 0, sizeof(struct serial_link_info));

	info->obj.double_dir = dir;
	info->obj.link_type=EVT_SUB_SYS_LINK_SERIAL;
	info->obj.active=0;
	info->obj.recv_buff= rt_malloc(256);
	info->obj.send_buff=rt_malloc(256);
  info->obj.applayer_id=(unsigned int)info;
  rt_memcpy(info->obj.name, name, rt_strlen(name));
	
	info->cfg.link_addr=addr;
	info->cfg.link_addr_len=addr_len;
	info->cfg.prev_sent_buff = rt_malloc(256);
	info->cfg.prev_sent_len = 0;
  
	info->acd_tag = 0;
	info->app_tag = 0;
	info->fcb = 0xFF;

#if(CFG_RUNNING_MODE==MUTLI_MODE)
	info->obj.mb_event = rt_mb_create("serialmb", MAX_EVENT_COUNT, RT_IPC_FLAG_FIFO);
#endif
}

/// <summary>
/// 创建一个链路
/// </summary>
/// <param name="addr">The addr.</param>
/// <param name="addr_len">The addr_len.</param>
/// <param name="dir">The dir.</param>
/// <returns></returns>
struct serial_link_info *serial_link_create(char *name,int addr, int addr_len, int dir)
{
	struct serial_link_info *info = (struct serial_link_info *)rt_malloc(sizeof(struct serial_link_info));
	if (info == 0)
	{
		rt_kprintf("serial linklayer configure malloc fail.\n");
		return 0;
	}
	
	serial_link_init(info,name, addr, addr_len, dir);

  return info;
}

/// <summary>
/// 清除串行链路配置信息.
/// </summary>
/// <param name="cfg">The CFG.</param>
void serial_link_del(struct serial_link_info *info)
{
	rt_free(info->obj.recv_buff);
	rt_free(info->obj.send_buff);
	rt_free(info->cfg.prev_sent_buff);

#if(CFG_RUNNING_MODE==MUTLI_MODE)
  if(info->obj.mb_event!=0)
    rt_mb_delete(info->obj.mb_event);
  if(info->obj.tid!=0)
  rt_thread_delete(info->obj.tid);
#endif
	rt_free(info);
}



/// <summary>
/// 获取一段数据的校验和
/// </summary>
/// <param name="buff">The buff.</param>
/// <param name="check_len">校验的长度</param>
/// <returns></returns>
static char serial_link_get_cs(char *buff, int check_len)
{
	char cs = 0;
	int i = 0;

	for (i = 0; i < check_len; i++)
		cs += buff[i];

	return cs;
}

/// <summary>
/// 固定帧校验
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="recv_buff">The recv_buff.</param>
/// <returns></returns>
static int serial_link_fixed_check(struct serial_link_cfg *cfg, char *recv_buff)
{
	int link_addr = 0;

	if (recv_buff[0] != FIXED_HEAD_TAG)
		return 0;

	if (recv_buff[cfg->link_addr_len+3] != FRAME_END_TAG)
		return 0;
	
	if (recv_buff[cfg->link_addr_len + 2] != serial_link_get_cs(&recv_buff[1], cfg->link_addr_len + 1))
		return 0;

	rt_memcpy(&link_addr, &recv_buff[2], cfg->link_addr_len);

	if (link_addr != cfg->link_addr)
		return 0;

	return 1;

}

/// <summary>
/// 非固定帧校验
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="recv_buff">The recv_buff.</param>
/// <returns></returns>
static int serial_link_unfixed_check(struct serial_link_cfg *cfg, char *recv_buff)
{
	int link_addr = 0;

	if (recv_buff[3] != UNFIXED_HEAD_TAG|| recv_buff[0] != UNFIXED_HEAD_TAG)
		return 0;

	if (recv_buff[1] != recv_buff[2])
		return 0;

  if(recv_buff[5+recv_buff[1]]!=FRAME_END_TAG)
    return 0;

	if (recv_buff[recv_buff[1]+4] != serial_link_get_cs(&recv_buff[4], recv_buff[1]))
		return 0;

	rt_memcpy(&link_addr, &recv_buff[5], cfg->link_addr_len);

	if (link_addr != cfg->link_addr)
		return 0;
	
	return 1;
	
}



/// <summary>
/// 校验数据
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="recv_size">The recv_size.</param>
/// <returns>返回控制域字节 若为0x0 则数据校验出错 </returns>
static char serial_link_recv_check_data(struct serial_link_info *info, int recv_size)
{
	char *recv_buff = info->obj.recv_buff;

  struct serial_link_cfg *cfg=&info->cfg;

	if (recv_size >= (4 + cfg->link_addr_len))
	{
		if (serial_link_fixed_check(cfg, recv_buff))
		{
			return  recv_buff[1];
		}
		else if (serial_link_unfixed_check(cfg,recv_buff))
		{
			return recv_buff[4];
		}
	}
	
	return 0x0;
}


/// <summary>
/// 链路数据处理，根据功能码分发处理.
/// </summary>
/// <param name="info">The information.</param>
/// <param name="cfg">The CFG.</param>
/// <param name="domain">The domain.</param>
/// <returns></returns>
static int serial_link_dispatch(struct serial_link_info *info, struct serial_link_cfg *cfg, char domain)
{
	union ctrl_domain dw_domain;
	dw_domain.domain = domain;

	if (dw_domain.dw_ctrl_domain.FCV)
	{
		if (info->fcb != dw_domain.dw_ctrl_domain.FCB)
		{
                  info->fcb = dw_domain.dw_ctrl_domain.FCB;
			
		}
                else 
                  return INVAILD_FCB;

		
	}

	switch (dw_domain.dw_ctrl_domain.FunCoed)
	{
	case FC_DW_LINK:
		return TO_LINK_REQ;
	case FC_DW_REQ:
		return TO_LINK; // 链路层--固定帧格式直接处理
		break;
	case FC_DW_DATA_NO:
		return NO_AWS; //无需回答
		break;
	case FC_DW_RST_REMOTE:
	case FC_DW_RST_USER:
		info->fcb=0xFF;
                link_set_active_state(&info->obj,0);
                info->obj.data_trans_active=0;
		return TO_LINK;
		break;
	case FC_DW_DATA_YES:
		return TO_APP_USER;
	case FC_DW_FIRST_DATA:	/**< 一级数据 */
		return TO_APP_FIRST;
	case FC_DW_SEC_DATA:
		return TO_APP_SECOND; //应用层---处理
		break;
	default:
		break;
	}

	return 0;

}

/// <summary>
/// 链路层封装固定数据包
/// </summary>
/// <param name="info">The information.</param>
/// <param name="funcode">The funcode. 功能码</param>
/// <returns>数据包长度</returns>
int serial_link_pack_fixed_frame(struct serial_link_info *info, char funcode)
{
	union ctrl_domain up_domain;
	int count=0;

	if (info->acd_tag > 0)
	{
		up_domain.up_ctrl_domain.ACD = 1;
	}

	up_domain.up_ctrl_domain.FunCoed = funcode;
        up_domain.up_ctrl_domain.DFC=0;
        up_domain.up_ctrl_domain.PRM=0;
        up_domain.up_ctrl_domain.RES=0;

	{
		info->obj.send_buff[count++] = FIXED_HEAD_TAG;
		info->obj.send_buff[count++] = up_domain.domain;
		rt_memcpy(&info->obj.send_buff[count], &info->cfg.link_addr, info->cfg.link_addr_len);
    count+=info->cfg.link_addr_len;
		info->obj.send_buff[count++] = serial_link_get_cs(&info->obj.send_buff[1], 1 + info->cfg.link_addr_len);
		info->obj.send_buff[count++] = FRAME_END_TAG;

	}
	return count;
}

/// <summary>
/// 链路层封装非固定数据包
/// </summary>
/// <param name="info">The information.</param>
/// <param name="funcode">The funcode.</param>
/// <param name="app_data">The app_data.</param>
/// <param name="app_data_len">The app_data_len.</param>
/// <returns>数据包长度</returns>
int serial_link_pack_unfixed_frame(struct serial_link_info *info, char funcode, char *app_data, int app_data_len)
{
	union ctrl_domain up_domain;
	int count=0;

       up_domain.domain=0;
	if (info->acd_tag > 0)
	{
		up_domain.up_ctrl_domain.ACD = 1;
                info->acd_tag=0;
	}

        
	up_domain.up_ctrl_domain.FunCoed = funcode;

	{
		if (app_data != 0)
		{
			info->obj.send_buff[0] = info->obj.send_buff[3] = UNFIXED_HEAD_TAG;
			info->obj.send_buff[1] = info->obj.send_buff[2] = 1 + info->cfg.link_addr_len + app_data_len;
			info->obj.send_buff[4] = up_domain.domain;
      count=5;
      rt_memcpy(&info->obj.send_buff[count], &info->cfg.link_addr, info->cfg.link_addr_len);
      count+=info->cfg.link_addr_len;
			rt_memcpy(&info->obj.send_buff[count], app_data, app_data_len);
      count+=app_data_len;
      info->obj.send_buff[count++] = serial_link_get_cs(&info->obj.send_buff[4], 1 + info->cfg.link_addr_len + app_data_len);
			info->obj.send_buff[count++] = FRAME_END_TAG;

		}

	}

	return count;
}




/** 
 * 链路物理接收事件处理
 * 
 * @param info 
 * @param evt 
 */
static void serial_link_phy_recv_handle(struct serial_link_info *info,struct iec_event *evt)
{
	char *data = 0;
	int data_len = 0;
	char domain_byte = 0;
	int dispatch_res = 0;
	struct link_recv_info *phy_recv_info = (struct link_recv_info *)evt->sub_msg;

	data_len = phy_recv_info->recv_len;
	data = phy_recv_info->recv_data;

	rt_memset(info->obj.recv_buff, 0, data_len);
	rt_memcpy(info->obj.recv_buff, data, data_len);

	domain_byte = serial_link_recv_check_data(info, data_len);
	if (domain_byte == 0)
		return;

	dispatch_res = serial_link_dispatch(info, &info->cfg, domain_byte);

	if (dispatch_res == NO_AWS)
		return;

	if (dispatch_res == INVAILD_FCB)
		info->obj.write(0,info->cfg.prev_sent_buff, info->cfg.prev_sent_len);
	else if (dispatch_res == TO_LINK)
	{
		data_len = serial_link_pack_fixed_frame(info, FC_UP_YES);
		info->obj.write(0,info->obj.send_buff, data_len);
		rt_memcpy(info->cfg.prev_sent_buff, info->obj.send_buff, data_len);
	}
	else if (dispatch_res == TO_LINK_REQ)
	{
		data_len = serial_link_pack_fixed_frame(info, FC_UP_LINK);
		info->obj.write(0,info->obj.send_buff, data_len);
		rt_memcpy(info->cfg.prev_sent_buff, info->obj.send_buff, data_len);
	}
	else if (dispatch_res == TO_APP_FIRST)
	{
		/*发送APP部分数据至APP层*/
		link_send_req_evt_to_app((struct link_obj*)info, EVT_SUB_DAT_LEVEL_1);
	}
	else if (dispatch_res == TO_APP_SECOND)
	{
		link_send_req_evt_to_app((struct link_obj*)info, EVT_SUB_DAT_LEVEL_2);
	}
	else if (dispatch_res == TO_APP_USER)
	{
		/*控制类帧解析*/
              link_set_active_state(&info->obj,1);
	      link_send_asdu_evt_to_app((struct link_obj*)info,&info->obj.recv_buff[5+info->cfg.link_addr_len],data_len-7-info->cfg.link_addr_len);
	}
}

/** 
 *  app至Link的事件处理
 * 
 * @param info 
 * @param evt 
 */
static void serial_link_app_recv_handle(struct serial_link_info *info, struct iec_event *evt)
{
  if(evt->evt_sub_type!=EVT_SUB_DAT_USER)
    {
      if (link_get_dir(&info->obj) == 1)/*平衡模式*/
        {
          link_send_req_evt_to_app((struct link_obj*)info, evt->evt_type);
        }
      else
        {
          if (evt->evt_sub_type == EVT_SUB_DAT_LEVEL_1)
            info->acd_tag=1;
          else
            info->app_tag++;
        }
    }
  else
    {
      info->acd_tag=1;
      int data_len = serial_link_pack_fixed_frame(info, FC_UP_YES);
      info->obj.write(0,info->obj.send_buff, data_len);
      rt_memcpy(info->cfg.prev_sent_buff, info->obj.send_buff, data_len);
    }
}


/** 
 * 链路处理接收数据事件
 * 
 * @param info 链路信息
 * @param evt 事件  1 物理链路收到数据 2 app产生的信息点变化事件 
 */
static void serial_link_recv_event_handle(struct serial_link_info *info,struct iec_event *evt)
{
	int sub_evt = evt->evt_sub_type;

	switch (sub_evt)
	{
	case EVT_SUB_DAT_LINK_PHY:
		serial_link_phy_recv_handle(info, evt);
		break;
	case EVT_SUB_DAT_LEVEL_1:
	case EVT_SUB_DAT_LEVEL_2: 
  case EVT_SUB_DAT_USER:
                 
		serial_link_app_recv_handle(info, evt);
		break;
	}
}
/** 
 * 链路处理发送的用户数据ASDU事件
 * 
 * @param info 链路信息
 * @param evt 事件 ASDU数据信息
 */
static void serial_link_asdu_send_evt_handle(struct serial_link_info *info ,struct iec_event *evt)
{
  int sub_evt=evt->evt_sub_type;
  struct app_send_info *send_info=evt->sub_msg;
  int count =0;
  
  if(send_info==0)
  {
    count = serial_link_pack_fixed_frame(info, FC_UP_NO_DATA);
    info->obj.write(0,info->obj.send_buff, count);
    rt_memcpy(info->cfg.prev_sent_buff, info->obj.send_buff, count);
    return;
  }
  
  switch(sub_evt)
    {
    case EVT_SUB_DAT_USER:
        if(app_task_check_empty(((struct app_info*)(info->obj.applayer_id))->first_task, (int)info)>0)
        {
          info->acd_tag=1;
        }
        else 
          info->acd_tag=0;
      count=serial_link_pack_unfixed_frame(info,FC_UP_USER_DATA ,send_info->app_data, send_info->app_data_len);
      rt_free(send_info->app_data);

      break;
    }

  info->obj.write(0,info->obj.send_buff,count);
  rt_memcpy(info->cfg.prev_sent_buff, info->obj.send_buff, count);
}

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void serial_link_thread_entry(void *param)
{
	struct serial_link_info *info = (struct serial_link_info*)param;

	struct iec_event *evt = 0;
	rt_kprintf("IEC:SERIAL LINK: %s link is running.\n", info->obj.name);

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

void serial_link_thread_start(struct serial_link_info *serial_link)
{
	serial_link->obj.tid=rt_thread_create("serial", serial_link_thread_entry, serial_link, SERIAL_THREAD_STACK_SIZE, SERIAL_THREAD_PROI, SERIAL_THREAD_TICK);
  rt_thread_startup(serial_link->obj.tid);
}
#endif







