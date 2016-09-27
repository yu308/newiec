#ifndef _applayer_h_
#define _applayer_h_

#include "iec_cfg.h"


union node_val
{
	int int_val;		/*整形*/
	float float_val;	/*浮点*/
	char iarray_val[4];		/*字符*/
	volatile unsigned bit_val : 32;	/*位串*/
};

/// <summary>
/// 信息点信息
/// </summary>
struct node
{
	int addr;	/* 信息点地址 */
	union node_val val; /*信息点值*/
	int changed;		/*是否有改变*/
	int serial_cmp;		/*串口方式传输完成标志 若未启用则为-1 */
	int	net_cmp;		/*网络方式传输完成标志 未启用为-1 */
	
	int buffered;		/*是否缓存数据*/
	int asdu_ident;		/*发送数据时采用ASDU类型*/
};

/// <summary>
/// ASDU配置信息 用于解析或封装信息点数据
/// </summary>
struct asdu_cfg
{	
	int val_ident;	/* 信息点值类型标识*/
	int qual_ident;	/* 信息点品质描述类型标识*/
	int tm_ident;	/* 信息点时间标签标识*/
	int ext_tm_ident; /*信息点扩展时间标签标识 用于继电保护事件信息点*/
};

/// <summary>
/// 缓存数据 
/// </summary>
struct buffered_data
{
	int node_addr;		/*信息点地址*/
	int asdu_ident;		/*ASDU标识*/
	union node_val val;	/*信息点值*/
	int time[7];		/*发生时间*/
};

/// <summary>
/// ASDU传输数据任务
/// </summary>
struct asdu_task
{
	int asdu_ident;		/*ASDU标识*/
	int seq_en;			/*启用顺序队列*/
	int node_count;		/*包含信息点个数*/
	int cause;			/* 传送原因*/
	int node_addr[CFG_ASDU_NODE_MAX];		/*信息点组*/
};

extern struct asdu_task First_task[CFG_FIRST_FRAME_MAX];
extern struct asdu_task Second_task[CFG_SECOND_FRAME_MAX];

#endif
