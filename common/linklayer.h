#ifndef _linklayer_h_
#define _linkelayer_h_

#include "iec_cfg.h"


#define	SINGLE_DIR		(0)		/*非平衡传输*/
#define DOUBLE_DIR		(1)		/*平衡传输*/


struct serial_linklayer_info
{
	int acd_tag;  /*有变化数据需要传输,一级数据*/
	int app_tag; /*有变化数据需要传输,二级数据*/
	int fcb;	/*数据变化有效位缓存*/
};

/*
	串行方式 链路层配置信息
*/
struct serial_linklayer_cfg
{
	int  link_addr;				/**< 链路地址  */
	int link_addr_bytes;		/* 链路地址长度 */
	int double_dir;				/*传输模式 平衡模式或非平衡模式*/

	char *recv_buff;   /*接收缓存*/
	char *send_buff;	 /*发送缓存*/

	struct serial_linklayer_info info;
};

/*
	网络方式 链路层配置信息
*/
struct net_linklayer_cfg
{
	int double_dir;

	int server_ip[4];

	int socket[CFG_SOCKET_MAX];

	char *recv_buff[CFG_SOCKET_MAX];
	char  *send_buff[CFG_SOCKET_MAX];
};



#define FIXED_FRAME		(1)			/*帧类型 固定长度帧*/		
#define UNFIXED_FRAME	(2)			/*帧类型 非固定长度帧*/

#define UNFIXED_HEAD_TAG	(0x68)		/*非固定帧头*/
#define FIXED_HEAD_TAG		(0x10)		/*固定帧头*/

#define FRAME_END_TAG		0x16		/*帧结尾标识*/

#define CTRL_DOMAIN_LEN		(1)			/*控制域长度*/

/**********************控制域****************/
//上传功能码
#define FC_UP_YES	0	/*肯定认可*/
#define FC_UP_NO	1	/*否定认可*/
#define FC_UP_USER_DATA	8	/*用户数据*/
#define FC_UP_NO_DATA	9	/*无数据*/
#define FC_UP_LINK	11	/*响应链路状态*/

struct up_control_domain
{
	volatile unsigned char FunCoed : 4;
	volatile unsigned char DFC : 1;
	volatile unsigned char ACD : 1;
	volatile unsigned char PRM : 1;
	volatile unsigned char RES : 1;
};

//下送功能码
#define FC_DW_RST_REMOTE	0	/**< 远方复位 */
#define FC_DW_RST_USER		1	/**< 复位用户 */
#define FC_DW_DATA_YES		3	/**< 确认/发送用户数据 */
#define FC_DW_DATA_NO		4	/**< 否定用户数据 */
#define FC_DW_REQ		8	/**< 访问请求 */
#define FC_DW_LINK		9   /**< 请求链路状态 */
#define FC_DW_FIRST_DATA	10	/**< 一级数据 */
#define FC_DW_SEC_DATA		11	/**< 二级数据 */

struct down_control_domain
{
	volatile unsigned char FunCoed : 4;
	volatile unsigned char FCV : 1;
	volatile unsigned char FCB : 1;
	volatile unsigned char PRM : 1;
	volatile unsigned char RES : 1;
};

union ctrl_domain
{
	struct up_control_domain up_ctrl_domain;
	struct down_control_domain dw_ctrl_domain;
	char domain;
};







#endif