#include "../../OS/os_helper.h"
#include "../Helper/layer_helper.h"

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void serial_link_thread_entry(void *param);
#define SERIAL_THREAD_PROI      (14)
#define SERIAL_THREAD_STACK_SIZE  (1024)
#define SERIAL_THREAD_TICK		(50)
#define MAX_EVENT_COUNT			(5)
#endif

/// <summary>
/// 初始化已有链路
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="addr">The addr.</param>
/// <param name="addr_len">The addr_len.</param>
/// <param name="dir">The dir.</param>
void serial_link_init(struct serial_link_info *info, int addr, int addr_len, int dir)
{
	XMEMSET(info, 0, sizeof(struct serial_link_info));
	struct serial_link_cfg *cfg= (struct serial_link_cfg *)rt_malloc(sizeof(struct serial_link_cfg));

	cfg->double_dir = dir;
	cfg->link_addr_len = addr_len;
	cfg->link_addr = addr;
	cfg->recv_buff = rt_malloc(256);
	cfg->send_buff = rt_malloc(256);
	cfg->prev_sent_buff = rt_malloc(256);
	cfg->prev_sent_len = 0;

	info->cfg = cfg;
	info->acd_tag = 0;
	info->app_tag = 0;
	info->fcb = 0;

#if(CFG_RUNNING_MODE==MUTLI_MODE)
	info->serial_event = rt_mb_create("serialmb", MAX_EVENT_COUNT, RT_IPC_FLAG_FIFO);
#endif
}

/// <summary>
/// 创建一个链路
/// </summary>
/// <param name="addr">The addr.</param>
/// <param name="addr_len">The addr_len.</param>
/// <param name="dir">The dir.</param>
/// <returns></returns>
struct serial_link_info *serial_link_create(int addr, int addr_len, int dir)
{
	struct serial_link_info *info = (struct serial_link_info *)rt_malloc(sizeof(struct serial_link_info));
	if (info == 0)
	{
		rt_kprintf("serial linklayer configure malloc fail.\n");
		return 0;
	}
	
	serial_link_init(info, addr, addr_len, dir);

}

/// <summary>
/// 清除链路配置信息.
/// </summary>
/// <param name="cfg">The CFG.</param>
void serial_link_del(struct serial_link_info *info)
{
	rt_free(info->cfg->recv_buff);
	rt_free(info->cfg->send_buff);
	rt_free(info->cfg->prev_sent_buff);
	rt_free(info->cfg);
}


/// <summary>
/// 重新初始化,恢复初始状态
/// </summary>
/// <param name="cfg">The CFG.</param>
void serial_link_reinit(struct serial_link_info *info)
{
	info->acd_tag = 0;
	info->app_tag = 0;
	info->fcb = 0;
}

/// <summary>
/// 串行方式下设置链路地址
/// </summary>
/// <param name="addr">The addr.</param>
void serial_link_set_addr(struct serial_link_cfg *cfg, int addr)
{
	cfg->link_addr = addr;
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
/// 设置传输模式
/// </summary>
/// <param name="dir">The dir. =0 非平衡传输模式  =1 平衡传输模式</param>
void serial_link_set_dir(struct serial_link_cfg *cfg, int dir)
{
	cfg->double_dir = dir;
}

int serial_link_get_dir(struct serial_link_cfg *cfg)
{
	return cfg->double_dir;
}

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


void serial_link_set_write_handle(struct serial_link_cfg *cfg, int *handle)
{
	cfg->serial_write = (serial_write_handle)handle;
}


int serial_link_get_active_state(struct serial_link_cfg *cfg)
{
  return cfg->active;
}

int serial_link_set_active_state(struct serial_link_cfg *cfg,int state)
{
  cfg->active=state;
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

	rt_memcpy(&link_addr, &recv_buff[5], cfg->link_addr_len);

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

	if (recv_buff[cfg->link_addr_len + 2] != serial_linklayer_get_cs(&recv_buff[4], recv_buff[1]))
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
static char serial_link_recv_check_data(struct serial_link_cfg *cfg, int recv_size)
{
	char *recv_buff = cfg->recv_buff;

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
			return INVAILD_FCB;
		}

		info->fcb = dw_domain.dw_ctrl_domain.FCB;
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
		serial_link_reinit(info);
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

	{
		info->cfg->send_buff[0] = FIXED_HEAD_TAG;
		info->cfg->send_buff[1] = up_domain.domain;
		rt_memcpy(&info->cfg->send_buff[2], &info->cfg->link_addr, info->cfg->link_addr_len);
		info->cfg->send_buff[1 + info->cfg->link_addr_len] = serial_linklayer_get_cs(&info->cfg->send_buff[1], 1 + info->cfg->link_addr_len);
		info->cfg->send_buff[2 + info->cfg->link_addr_len] = FRAME_END_TAG;

		count = 3 + info->cfg->link_addr_len;
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

	if (info->acd_tag > 0)
	{
		up_domain.up_ctrl_domain.ACD = 1;
	}

	up_domain.up_ctrl_domain.FunCoed = funcode;

	{
		if (app_data != 0)
		{
			info->cfg->send_buff[0] = info->cfg->send_buff[3] = UNFIXED_HEAD_TAG;
			info->cfg->send_buff[4] = up_domain.domain;
			info->cfg->send_buff[1] = info->cfg->send_buff[2] = 1 + info->cfg->link_addr_len + app_data_len;
			XMEMCPY(&info->cfg->send_buff[5], app_data, app_data_len);
			info->cfg->send_buff[4 + info->cfg->link_addr_len + app_data_len] = serial_link_get_cs(&info->cfg->send_buff[4], 1 + info->cfg->link_addr_len + app_data_len);
			info->cfg->send_buff[5 + info->cfg->link_addr_len + app_data_len] = FRAME_END_TAG;

			count = info->cfg->send_buff[2] + 6;
		}

	}

	return count;
}





static void serial_link_phy_recv_handle(struct serial_link_info *info,struct iec_event *evt)
{
	char *data = 0;
	int data_len = 0;
	char domain_byte = 0;
	int dispatch_res = 0;
	struct link_recv_info *phy_recv_info = (struct link_recv_info *)evt->main_msg;

	data_len = phy_recv_info->recv_len;
	data = phy_recv_info->recv_data;

	rt_memset(info->cfg->recv_buff, 0, data_len);
	rt_memcpy(info->cfg->recv_buff, data, data_len);

	domain_byte = serial_link_recv_check_data(info->cfg, data_len);
	if (domain_byte == 0)
		return;

	dispatch_res = serial_link_dispatch(info, info->cfg, domain_byte);

	if (dispatch_res == NO_AWS)
		return;

	if (dispatch_res == INVAILD_FCB)
		info->cfg->serial_write(info->cfg->prev_sent_buff, info->cfg->prev_sent_len);
	else if (dispatch_res == TO_LINK)
	{
		data_len = serial_link_pack_fixed_frame(info, FC_UP_YES);
		info->cfg->serial_write(info->cfg->send_buff, data_len);
		rt_memcpy(info->cfg->prev_sent_buff, info->cfg->send_buff, data_len);
	}
	else if (dispatch_res == TO_LINK_REQ)
	{
		data_len = serial_link_pack_fixed_frame(info, FC_UP_LINK);
		info->cfg->serial_write(info->cfg->send_buff, data_len);
		rt_memcpy(info->cfg->prev_sent_buff, info->cfg->send_buff, data_len);
	}
	else if (dispatch_res == TO_APP_FIRST)
	{
		/*发送APP部分数据至APP层*/
		serial_link_send_req_evt_to_app(info, EVT_SUB_DAT_LEVEL_1);
	}
	else if (dispatch_res == TO_APP_SECOND)
	{
		serial_link_send_req_evt_to_app(info, EVT_SUB_DAT_LEVEL_2);
	}
	else if (dispatch_res == TO_APP_USER)
	{
		/*控制类帧解析*/
		serial_link_send_asdu_evt_to_app(info,&info->cfg->recv_buff[5+info->cfg->link_addr_len]);
	}
}

static void serial_link_app_recv_handle(struct serial_link_info *info, struct iec_event *evt)
{
	if (serial_link_get_dir(info->cfg) == 1)/*平衡模式*/
	{
		serial_link_send_req_evt_to_app(info, evt->evt_type);
	}
	else
	{
		if (evt->evt_sub_type == EVT_SUB_DAT_LEVEL_1)
			info->acd_tag++;
		else
			info->app_tag++;
	}
}

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
		serial_link_app_recv_handle(info, evt);
		break;
	}
}

static void serial_link_asdu_send_evt_handle(struct serial_link_info *info ,struct iec_event *evt)
{
  int sub_evt=evt->evt_sub_type;
  struct app_send_info *send_info=evt->sub_msg;
  int count =0;
  switch(sub_evt)
    {
    case EVT_SUB_DAT_USER:
      if(app_task_check_empty(((struct app_info*)(info->applayer_id))->first_task, (int)info)>0)
        {
          info->acd_tag=1;
        }
      count=serial_link_pack_unfixed_frame(info,FC_UP_USER_DATA ,send_info->app_data, send_info->app_data_len);
      rt_free(send_info->app_data);
      break;
    }

  info->cfg->serial_write(info->cfg->send_buff,count);
}

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void serial_link_thread_entry(void *param)
{
	struct serial_link_info *info = (struct serial_link_info*)param;

	struct iec_event *evt = 0;
	rt_kprintf("IEC:SERIAL LINK: %d link is running.\n", info->serial_tid);

	while (1)
	{
		evt =iec_recv_event(info->serial_event, RT_WAITING_FOREVER);
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

void serial_link_thread_start(int plink_info)
{
	struct serial_link_info *info = (struct serial_link_info *)plink_info;
	info->serial_tid=rt_thread_create("serial", serial_link_thread_entry, info, SERIAL_THREAD_STACK_SIZE, SERIAL_THREAD_PROI, SERIAL_THREAD_TICK);
  rt_thread_startup(info->serial_tid);
}
#endif







