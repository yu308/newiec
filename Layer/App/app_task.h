#ifndef _app_task_h_
#define _app_task_h_

#include "arraylist.h"
#include "iec_node.h"

struct app_task
{
	char task_name[16];
	unsigned int asdu_ident;
	unsigned int cause;
	arraylist *node_data_list;
};

struct app_node_frame_cfg
{
	unsigned int addr;
	int val;
	int qual;
	unsigned int utc_time;
};



extern int app_task_add_normal(arraylist *al, unsigned int asdu_ident, int cause, int seq, struct node_frame_info *f_node);
#endif
