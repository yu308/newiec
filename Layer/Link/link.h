#ifndef _link_h_
#define _link_h_

#include "../../Core/iec_cfg.h"

typedef void(*write_handle)(char *buff, int len);


struct link_obj
{
	char name[CFG_NAME_MAX];  /*名称 放在首位*/
	char link_type; 	/*链路类型*/

  unsigned int applayer_id;
	char active;	 /*是否激活 可进行数据交互*/
        char data_trans_active; /*数据传输激活*/
	char double_dir; /*平衡或非平衡*/
	
	char *recv_buff; /*接收缓存*/
	char *send_buff; /*发送缓存*/
	
	write_handle write; /*发送接口*/
	
#if(CFG_RUNNING_MODE==MUTLI_MODE)
	rt_thread_t tid;
	rt_mailbox_t mb_event;
#endif
};


#endif
