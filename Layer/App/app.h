#ifndef _app_h_
#define _app_h_


#include "../Helper/layer_helper.h"




struct buffered_data
{
	int node_addr;		/*信息点地址*/
	int asdu_ident;		/*ASDU标识*/
	union node_val val;	/*信息点值*/
	int utc_node_time;	/*信息点扩展时间*/
	int utc_time;		/*发生时间*/
};

/// <summary>
/// 应用配置信息
/// </summary>
struct app_cfg
{
	int asdu_addr; /*ASDU地址*/	
	int asdu_addr_len;	/*ASDU地址长度*/
	int cause_len;	/*传送原因长度*/
	int node_addr_len;	/*信息点地址长度*/
	int sm2_enable;		/*SM2加密功能*/
};

/// <summary>
/// 应用信息
/// </summary>
struct app_info
{
	struct app_cfg *cfg;

	int linklayer_id[CFG_LINK_MAX];		/*绑定的link链路*/
 
#if(CFG_ROLE_MODE==SYS_ROLE_MIXED)
	int bro_applayer_id[CFG_APP_MAX];	/*中转机混合模式下 关联的APP*/
#endif

	arraylist *n_node_list;	/*普通信息点组记录*/
	arraylist *s_node_list;/*序列化信息点组记录*/
	

	arraylist *first_task;	/*一级数据 即传输优先级最高的数据*/
	arraylist *second_task;	/*二级数据 次之*/

	arraylist *buffered;

#if(CFG_RUNNING_MODE==MUTLI_MODE)
	osMessageQId app_event;
	osThreadId app_tid;
#endif
};

#define APP_FUN_FIRST		(4)
#define APP_FUN_SECOND		(5)
#define APP_FUN_USER		(6)
#endif
