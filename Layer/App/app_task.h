#ifndef _app_task_h_
#define _app_task_h_

#include "../../Core/core.h"
#include "../layer.h"
#include "../../Iec/iec.h"

struct app_task
{
	char task_name[24];
  unsigned int link_id;
	unsigned int asdu_ident;
	unsigned int cause;
	unsigned int seq;
	arraylist *node_data_list;
};

struct app_node_frame_cfg
{
	unsigned int addr;
	int val;
	int qual;
	unsigned int utc_time;
};

int app_task_free(arraylist *task_list,struct app_task *task,int clear);


extern int app_task_add_normal(arraylist *al,unsigned int link_id, unsigned int asdu_ident, int cause, struct node_frame_info *f_node);
extern struct app_task *app_task_get(arraylist *al,unsigned int link_id);
extern struct app_send_info *app_task_covert_to_asdu_frame(struct app_info *info, struct app_task *task);
extern int app_task_check_empty(arraylist *al,int link_id);
extern void app_linkframe_convert_to_asdu(struct app_info *info,struct app_recv_info *recv_info);


extern void app_task_insert_ack_asdu(struct app_info *info,int link_id,struct app_recv_info *recv_info);
#endif
