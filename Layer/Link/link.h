#ifndef _link_h_
#define _link_h_

#include "../../Core/core.h"

typedef void(*write_handle)(unsigned int dev,char *buff, int len);


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


void link_set_dir(struct link_obj *link, int dir);

int link_get_dir(struct link_obj *link);


void link_set_write_handle(struct link_obj *link, int *handle);


int link_get_active_state(struct link_obj *link);

void link_set_active_state(struct link_obj *link,int state);

char *link_get_name(struct link_obj *link);


#endif
