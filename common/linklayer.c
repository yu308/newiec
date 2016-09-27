#include "linklayer.h"




#if(CFG_SERIAL_CLIENT_EN||CFG_SERIAL_SERVER_EN)

/// <summary>
/// 初始化已有链路配置结构
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="addr">The addr.</param>
/// <param name="addr_len">The addr_len.</param>
/// <param name="dir">The dir.</param>
void serial_linklayer_init(struct serial_linklayer_cfg *cfg, int addr, int addr_len, int dir)
{
	XMEMSET(cfg, 0, sizeof(struct serial_linklayer_cfg));

	cfg->double_dir = dir;
	cfg->link_addr_bytes = addr_len;
	cfg->link_addr = addr;

	cfg->info.acd_tag = 0;
	cfg->info.app_tag = 0;
	cfg->info.fcb = 0;
}

/// <summary>
/// 创建一个链路配置
/// </summary>
/// <param name="addr">The addr.</param>
/// <param name="addr_len">The addr_len.</param>
/// <param name="dir">The dir.</param>
/// <returns></returns>
struct serial_linklayer_cfg *serial_linklayer_create(int addr, int addr_len, int dir)
{
	struct serial_linklayer_cfg *cfg = (struct serial_linklayer_cfg *)XMALLOC(sizeof(struct serial_linklayer_cfg));
	if (cfg == 0)
	{
		XPRINTF("serial linklayer configure malloc fail.\n");
		return 0;
	}
	
	serial_linklayer_init(cfg, addr, addr_len, dir);

	cfg->recv_buff = XMALLOC(256);
	cfg->send_buff = XMALLOC(256);
}

/// <summary>
/// 清除链路配置信息.
/// </summary>
/// <param name="cfg">The CFG.</param>
void serial_linklayer_del(struct serial_linklayer_cfg *cfg)
{
	XFREE(cfg->recv_buff);
	XFREE(cfg->send_buff);
	XFREE(cfg);
}


/// <summary>
/// 重新初始化,恢复初始状态
/// </summary>
/// <param name="cfg">The CFG.</param>
void serial_linklayer_reinit(struct serial_linklayer_cfg *cfg)
{
	cfg->info.acd_tag = 0;
	cfg->info.app_tag = 0;
	cfg->info.fcb = 0;
}

/// <summary>
/// 设置链路层缓存,使用静态内存时使用
/// </summary>
/// <param name="recv_handle">The recv_handle. 接收接口缓存</param>
/// <param name="send_handle">The send_handle. 发送接口缓存 </param>
void serial_linklayer_set_communicate_buff(struct serial_linklayer_cfg *cfg, char *recv_buff, char *send_buff)
{
	if (recv_buff != 0)
		cfg->recv_buff = recv_buff;

	if (send_buff != 0)
		cfg->send_buff = send_buff;
}
/// <summary>
/// 串行方式下设置链路地址
/// </summary>
/// <param name="addr">The addr.</param>
void serial_linklayer_set_addr(struct serial_linklayer_cfg *cfg, int addr)
{
	cfg->link_addr = addr;
}


/// <summary>
/// 串行方式下设置链路地址长度.
/// </summary>
/// <param name="len">The length.</param>
void serial_linklayer_set_addr_len(struct serial_linklayer_cfg *cfg, int len)
{
	cfg->link_addr_bytes = len;
}

/// <summary>
/// 设置传输模式
/// </summary>
/// <param name="dir">The dir. =0 非平衡传输模式  =1 平衡传输模式</param>
void serial_linklayer_set_dir(struct serial_linklayer_cfg *cfg, int dir)
{
	cfg->double_dir = dir;
}

/// <summary>
/// 获取链路地址.
/// </summary>
/// <returns>链路地址</returns>
int serial_linklayer_get_addr(struct serial_linklayer_cfg *cfg)
{
	return cfg->link_addr;
}

/// <summary>
/// 获取链路地址长度
/// </summary>
/// <returns>链路地址长度</returns>
int serial_linklayer_get_addr_len(struct serial_linklayer_cfg *cfg)
{
	return cfg->link_addr_bytes;
}


/// <summary>
/// 获取一段数据的校验和
/// </summary>
/// <param name="buff">The buff.</param>
/// <param name="check_len">校验的长度</param>
/// <returns></returns>
static char serial_linklayer_get_cs(char *buff, int check_len)
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
static int serial_linklayer_fixed_check(struct serial_linklayer_cfg *cfg, char *recv_buff)
{
	int link_addr = 0;

	if (recv_buff[0] != FIXED_HEAD_TAG)
		return 0;

	if (recv_buff[cfg->link_addr_bytes+3] != FRAME_END_TAG)
		return 0;
	
	if (recv_buff[cfg->link_addr_bytes + 2] != serial_linklayer_get_cs(&recv_buff[1], cfg->link_addr_bytes + 1))
		return 0;

	XMEMCPY(link_addr, &recv_buff[5], cfg->link_addr_bytes);

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
static int serial_linklayer_unfixed_check(struct serial_linklayer_cfg *cfg, char *recv_buff)
{
	int link_addr = 0;

	if (recv_buff[3] != UNFIXED_HEAD_TAG|| recv_buff[0] != UNFIXED_HEAD_TAG)
		return 0;

	if (recv_buff[1] != recv_buff[2])
		return 0;

	if (recv_buff[cfg->link_addr_bytes + 2] != serial_linklayer_get_cs(&recv_buff[4], recv_buff[1]))
		return 0;

	XMEMCPY(link_addr, &recv_buff[5], cfg->link_addr_bytes);

	if (link_addr != cfg->link_addr)
		return 0;
	
	return 1;
	
}



/// <summary>
/// 校验数据
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="recv_size">The recv_size.</param>
/// <returns>返回控制域字节 若为0xFF 则数据校验出错 </returns>
char serial_linklayer_recv_check_data(struct serial_linklayer_cfg *cfg, int recv_size)
{
	char *recv_buff = cfg->recv_buff;

	if (recv_size >= (4 + cfg->link_addr_bytes))
	{
		if (serial_linklayer_fixed_check(cfg, recv_buff))
		{
			return  recv_buff[1];
		}
		else if (serial_linklayer_unfixed_check(cfg,recv_buff))
		{
			return recv_buff[4];
		}
	}
	
	return 0xFF;
}

/// <summary>
/// 将数据信息分发至不同层去处理.
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="domain">The domain.</param>
/// <returns>0 无需处理, 1 链路层处理  2 应用层处理</returns>
int serial_linklayer_dispatch(struct serial_linklayer_cfg *cfg, char domain)
{
	union ctrl_domain dw_domain;
	dw_domain.domain = domain;

	if (dw_domain.dw_ctrl_domain.FCV)
	{
		if (cfg->info.fcb != dw_domain.dw_ctrl_domain.FCB)
		{
			return 0;
		}

		cfg->info.fcb = dw_domain.dw_ctrl_domain.FCB;
	}

	switch (dw_domain.dw_ctrl_domain.FunCoed)
	{
	case FC_DW_LINK:
	case FC_DW_REQ:
		return 1; // 链路层--固定帧格式直接处理
		break;
	case FC_DW_DATA_NO:
		return 0; //无需回答
		break;
	case FC_DW_RST_REMOTE:
	case FC_DW_RST_USER:
		serial_linklayer_reinit(cfg);
		return 2;
		break;
	case FC_DW_DATA_YES:
	case FC_DW_FIRST_DATA:	/**< 一级数据 */
	case FC_DW_SEC_DATA:
		return 2; //应用层---处理
		break;		
	default:
		break;
	}
	
	return 0;

}

/// <summary>
/// 链路层封装数据包
/// </summary>
/// <param name="frame_type">The frame_type. 帧类型 </param>
/// <param name="cfg">The CFG. 链路层配置信息</param>
/// <param name="funcode">The funcode. 功能码</param>
/// <param name="app_data">The app_data.应用数据</param>
/// <param name="app_data_len">The app_data_len. 应用数据长度</param>
/// <returns></returns>
int serial_linklayer_pack_frame(int frame_type, struct serial_linklayer_cfg *cfg, char funcode, char *app_data, int app_data_len)
{
	union ctrl_domain up_domain;
	int count;

	if (cfg->info.acd_tag > 0)
	{
		up_domain.up_ctrl_domain.ACD = 1;
	}

	up_domain.up_ctrl_domain.FunCoed = funcode;

	if (frame_type == FIXED_FRAME||app_data==0)
	{
		cfg->send_buff[0] = FIXED_HEAD_TAG;
		cfg->send_buff[1] = up_domain.domain;
		XMEMCPY(&cfg->send_buff[2], &cfg->link_addr, cfg->link_addr_bytes);
		cfg->send_buff[1 + cfg->link_addr_bytes] = serial_linklayer_get_cs(&cfg->send_buff[1], 1 + cfg->link_addr_bytes);
		cfg->send_buff[2 + cfg->link_addr_bytes] = FRAME_END_TAG;

		count = 3 + cfg->link_addr_bytes;
	}
	else if (frame_type == UNFIXED_FRAME)
	{
		if (app_data != 0)
		{
			cfg->send_buff[0] = cfg->send_buff[3] = UNFIXED_HEAD_TAG;
			cfg->send_buff[4] = up_domain.domain;
			cfg->send_buff[1] = cfg->send_buff[2] = 1 + cfg->link_addr_bytes + app_data_len;
			XMEMCPY(&cfg->send_buff[5], app_data, app_data_len);
			cfg->send_buff[4 + cfg->link_addr_bytes + app_data_len] = serial_linklayer_get_cs(&cfg->send_buff[4], 1 + cfg->link_addr_bytes + app_data_len);
			cfg->send_buff[5 + cfg->link_addr_bytes + app_data_len] = FRAME_END_TAG;

			count = cfg->send_buff[2] + 6;
		}

	}

	return count;
}

/// <summary>
/// 链路层信息处理函数.
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="domain">The domain.</param>
/// <returns></returns>
int serial_linklayer_worker(struct serial_linklayer_cfg *cfg, char domain)
{
	union ctrl_domain dw_domain;
	dw_domain.domain = domain;


	switch (dw_domain.dw_ctrl_domain.FunCoed)
	{
	case FC_DW_LINK:
	case FC_DW_REQ:
		return serial_linklayer_pack_frame(FIXED_FRAME,cfg,FC_UP_YES,0,0); // 链路层--固定帧格式直接处理
		break;
	}
}
#endif


