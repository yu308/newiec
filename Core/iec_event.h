
#ifndef _iec_event_h_
#define _iec_event_h_

#include "cmsis_os.h"
#include "iec_cfg.h"


/***************************系统层事件定义*****************************************/
#define		EVT_SYS_CREATE_LINK		(1)		/*创建链路链接层*/
#define		EVT_SYS_CREATE_APP		(2)		/*创建APP层*/
#define		EVT_SYS_BIND_APP_LINK	(3)		/*绑定APP层与链路层*/
#define		EVT_SYS_START			(4)		/*系统启动服务*/
#define		EVT_SYS_DEL_APP			(5)		/*删除一个APP层*/
#define		EVT_SYS_DEL_LINK		(6)		/*删除一个链路层*/
#define		EVT_SYS_EDIT_PROFILE	(7)		/*修改某个对象配置*/


/*******************************链路层事件定义***************************************/
#define		EVT_LINK_PHY_CONNECT			(0x11)		/*通信物理通道链接*/
#define		EVT_LINK_PHY_DISCONNECT			(0x12)		/*通信物理通道断开*/
#define		EVT_LINK_RECV_DATA				(0x13)		/*收到链路数据*/
#define		EVT_LINK_SEND_DATA				(0x14)		/*需发送链路数据*/

/******************************APP系统应用层事件定义*****************************************/
#define		EVT_APP_ADD_NODE				(0x21)		/*添加信息点*/
#define		EVT_APP_NODE_UPDATE				(0x22)		/*信息点变化*/
#define		EVT_APP_RECV_DATA				(0x23)		/*收到ASDU数据*/
#define		EVT_APP_SEND_DATA				(0x24)		/*需发送ASDU数据*/

/******************************APP用户应用层事件定义*************************************/
#define		EVT_APP_CTRL_OP					(0x31)		/*控制操作*/
#define		EVT_APP_SET_OP					(0x32)		/*设置修改操作*/
#define		EVT_APP_READ_OP					(0x33)		/*读取操作*/
#define		EVT_APP_FILE_OP					(0x34)		/*文件操作*/

#if(CFG_RUNNING_MODE==SINGLE_MODE)
#define		SYS_OBJ				(0x0)		/*使用单任务模式时 系统对象编号*/
#endif


struct normal_node_create_info
{
	unsigned int addr; /* 信息点地址 */
	unsigned int appid;/* 所属app实例id */
};

struct seq_node_create_info
{
	unsigned int addr; /*信息点组起始地址*/
	unsigned int appid;/*所属app实例id*/
	unsigned int count;/*序列化信息点组数量*/
};

struct normal_node_update_info
{
	unsigned int appid;
	unsigned int asdu_ident;
	unsigned int cause;

	int *val;
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

union event_msg
{
	struct link_param m_link_param;
	struct link_send_info m_link_send_info;
	struct link_recv_info m_link_recv_info;


	struct app_param	m_app_param;
	struct app_recv_info	m_app_recv_info;


	struct normal_node_create_info m_nd_info;
	struct seq_node_create_info m_snd_info;
};

/// <summary>
/// 事件
/// </summary>
struct iec_event
{
	int sender;		/*事件发送者*/
	int recver;		/*事件接收者*/
	int event_mem_auto;	/*event是否自动回收*/
	int msg_mem_auto;
	
	int evt_type;		/*事件类型*/
	union event_msg *msg;			/*事件数据*/
};


extern osStatus iec_post_event(osMessageQId q_id, struct iec_event *msg, int millisec);
extern struct iec_event *iec_recv_event(osMessageQId queue_id, int millisec);
extern void iec_init_event(struct iec_event *msg, int sender, int recver, int evt_type, int *data);
extern struct iec_event *iec_create_event(int sender, int recver, int evt_type, int *data,int data_auto);
extern void iec_free_event(struct iec_event *msg);


#endif