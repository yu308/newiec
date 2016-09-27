
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

/// <summary>
/// 事件
/// </summary>
struct iec_msg
{
	int sender;		/*事件发送者*/
	int recver;		/*事件接收者*/
	int mem_auto;	/*是否自动回收*/
	
	int evt_type;		/*事件类型*/
	int *msg;			/*事件数据*/
};


#endif