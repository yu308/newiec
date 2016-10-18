#ifndef _link_h_
#define _link_h_

#include "iec_cfg.h"

typedef void(*serial_write_handle)(char *buff, int len);
/// <summary>
/// 串行方式--链路配置
/// </summary>
struct serial_link_cfg
{
	int link_addr;			/*链路地址*/
	int link_addr_len;		/*链路地址长度*/
  int active;           /*是否激活*/
	int double_dir;			/*平衡或非平衡*/
		
	char *recv_buff;		/*接收缓存*/
	char *send_buff;		/*发送缓存*/
	char *prev_sent_buff;	/*上一此发送数据缓存*/
	int	prev_sent_len;		/*发送长度*/

	serial_write_handle	serial_write;	/*发送接口*/
};

/// <summary>
/// 串行方式--链路运行信息
/// </summary>
struct serial_link_info
{
	struct serial_link_cfg *cfg;

	int acd_tag;
	int app_tag;
	int fcb;
	int applayer_id;

#if(CFG_RUNNING_MODE==MUTLI_MODE)
	osThreadId	serial_tid;
	osMessageQId serial_event;
#endif
};


/**************链路分发处理判断结果******************/
#define NO_AWS				(0)			/*无回答*/
#define INVAILD_FCB			(1)			/*无效FCB计数*/
#define TO_LINK				(2)			/*至LINK*/
#define TO_LINK_REQ				(3)			/*至LINK链路状态*/
#define TO_APP_FIRST				(4)			/*至APP*/
#define TO_APP_SECOND		(5)
#define TO_APP_USER			(6)

/// <summary>
/// 网络方式--链路配置
/// </summary>
struct net_link_cfg
{
	int ip[4];
	int role;
	
	int link_socket[CFG_SOCKET_MAX];
};

#endif
