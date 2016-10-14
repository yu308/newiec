
#ifndef _iec_event_h_
#define _iec_event_h_

#include "cmsis_os.h"
#include "iec_cfg.h"
#include "iec_node.h"


/***************************系统层事件定义*****************************************/
#define		EVT_SYS_CREATE_LINK		(1)		/*创建链路链接层  MSG--链路地址,链路地址长度*/
#define		EVT_SYS_CREATE_APP		(2)		/*创建APP层 MSG--ASDU地址,ASDU地址长度,传输原因长度,启用SM2,信息点地址长度*/
#define		EVT_SYS_BIND_APP_LINK	(3)		/*绑定APP层与链路层 MSG---链路模块ID,APP模块ID*/
#define		EVT_SYS_START			(4)		/*系统启动服务 MSG--启动*/
#define		EVT_SYS_DEL_APP			(5)		/*删除一个APP层 MSG---APP模块ID*/
#define		EVT_SYS_DEL_LINK		(6)		/*删除一个链路层 MSG---LINK模块ID*/
#define		EVT_SYS_EDIT_PROFILE	(7)		/*修改某个对象配置*/


/*******************************链路层事件定义***************************************/
#define		EVT_LINK_PHY_CONNECT			(0x11)		/*通信物理通道链接*/
#define		EVT_LINK_PHY_DISCONNECT			(0x12)		/*通信物理通道断开*/
#define		EVT_LINK_RECV_DATA				(0x13)		/*收到链路数据*/
#define		EVT_LINK_SEND_DATA				(0x14)		/*需发送链路数据*/

/******************************APP系统应用层事件定义*****************************************/
#define		EVT_APP_ADD_NODE				(0x21)		/*添加信息点 MSG--NODE地址,NODE类型 SUBMSG--SEQ数量*/
#define		EVT_APP_NODE_UPDATE				(0x22)		/*信息点变化 MSG--ASDU IDENT,传输原因 SUB MSG--NODE地址,NODE值,品质描述,UTC时间,是否缓存*/
#define		EVT_APP_RECV_DATA				(0x23)		/*收到ASDU数据  MSG--ASDU 数据串、数据串长度*/
#define		EVT_APP_SEND_DATA				(0x24)		/*需发送ASDU数据 MSG---ASDU 数据串、数据串长度*/


#define		EVT_APP_SUB_NORMAL_NODE		(1) 
#define		EVT_APP_SUB_SEQ_NODE		(2)

/******************************APP用户应用层事件定义*************************************/
#define		EVT_APP_CTRL_OP					(0x31)		/*控制操作*/
#define		EVT_APP_SET_OP					(0x32)		/*设置修改操作*/
#define		EVT_APP_READ_OP					(0x33)		/*读取操作*/
#define		EVT_APP_FILE_OP					(0x34)		/*文件操作*/

#if(CFG_RUNNING_MODE==SINGLE_MODE)
#define		SYS_OBJ				(0x0)		/*使用单任务模式时 系统对象编号*/
#endif



/// <summary>
/// 更新通用信息点时信息
/// </summary>
struct normal_node_update_info
{
	unsigned int appid; /*所属APP*/
	unsigned int level; /*刷新优先级*/
	unsigned int asdu_ident;/*asdu标识*/
	unsigned int cause; /*传送原因*/
	unsigned int seq; /*序列化传输*/
};

/// <summary>
/// 序列化信息点更新时信息
/// </summary>
struct seq_node_update_info
{
	unsigned int appid;
	unsigned int node_start_addr;
	unsigned int count;
	unsigned int asdu_ident;
	unsigned int cause;

	struct node_frame_info *f_seq_info;/*对应count数量的 node 帧格式数据,*/
};

/// <summary>
/// 创建Link时参数
/// </summary>
struct link_param
{
	unsigned char link_addr;
	unsigned char link_addr_len;
	unsigned char link_dir;
};

/// <summary>
/// Link实例收到的数据信息
/// </summary>
struct link_recv_info
{
	char *recv_data;
	int recv_len;
};

/// <summary>
/// LINK实例收到APP实例需发送数据信息
/// </summary>
struct link_send_info
{
	char *app_data;
	char app_data_len;
};

/// <summary>
/// 创建APP实例时的参数
/// </summary>
struct app_param
{
	unsigned char asdu_addr;
	unsigned char asdu_addr_len;
	unsigned char cause_len;
	unsigned char node_addr_len;
	unsigned char sm2_enable;
};


/// <summary>
/// APP实例收到LINK实例的数据信息
/// </summary>
struct app_recv_info
{
	char funcode;
	char *app_data;
};



/// <summary>
/// 事件
/// </summary>
struct iec_event
{
	int sender;		/*事件发送者*/
	int recver;		/*事件接收者*/
	int main_msg_free;	/*事件主消息是否自动回收*/
	int sub_msg_free;		/*事件子消息是否自动回收*/
	
	int evt_type;		/*事件类型*/
	int evt_sub_type;   /*事件细分*/
	void *main_msg;			/*主事件数据*/
	void *sub_msg			/*子事件数据*/
};


extern osStatus iec_post_event(osMessageQId q_id, struct iec_event *msg, int millisec);
extern struct iec_event *iec_recv_event(osMessageQId queue_id, int millisec);
extern struct iec_event *iec_create_event(int sender, int recver, int evt_type,
	int *main_msg, int auto_free);
extern void iec_free_event(struct iec_event *evt);

void iec_set_event_sub(struct iec_event *evt, int evt_sub_type, int *sub_msg, int auto_free);


#endif