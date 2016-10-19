#ifndef _app_task_h_
#define _app_task_h_

#include "../Helper/arraylist.h"
#include "../../Iec/iec_helper.h"

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



extern int app_task_add_normal(arraylist *al,unsigned int link_id, unsigned int asdu_ident, int cause, struct node_frame_info *f_node);
extern int app_task_add_seq(arraylist *al,unsigned int link_id, unsigned int asdu_ident, int cause, struct seq_node_frame_info *f_s_node);
extern struct app_task *app_task_get(arraylist *al);
extern struct app_send_info *app_task_covert_to_asdu_frame(struct app_info *info, struct app_task *task);
extern int app_task_check_empty(arraylist *al,int link_id);
#endif
