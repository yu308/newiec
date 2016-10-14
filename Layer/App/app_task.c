#include "iec_event.h"
#include "app_task.h"
#include "iec_node.h"
#include <stdio.h>

#define TASK_IDENT_NAME_FORMAT		"%02X-%02X-%02X"			/*ASDU-CAUSE-SEQ*/

void app_task_add_normal(hashtable *ht,unsigned int asdu_ident, int cause,int seq, struct node_frame_info *f_node)
{
	char name[16];
	struct node_frame_info *f_node_temp = 0;
	int i = 0;
	XMEMSET(name, 0, 16);
	vsprintf(name, TASK_IDENT_NAME_FORMAT, asdu_ident, cause,seq);

	struct app_task *temp= (struct app_task *)hashtable_get(ht, name);
	if (temp != 0)
	{
		if (temp->node_data_list != 0)
		{
			if (f_node->buffered == 0)
			{
				arraylist_iterate(temp->node_data_list, i, f_node_temp)
				{
					if (f_node_temp->addr == f_node->addr)
					{
						f_node_temp->qual = f_node->qual;
						f_node_temp->utc_time = f_node->utc_time;
						f_node_temp->val = f_node->val;
						return;
					}
				}
			}
			arraylist_add(temp->node_data_list, f_node);
		}
	}
	else
	{
		temp = XMALLOC(sizeof(struct app_task));
		XMEMSET(temp, 0, sizeof(struct app_task));
		temp->asdu_ident = asdu_ident;
		temp->cause = cause;
		XMEMCPY(temp->task_name, name, 16);

		temp->node_data_list = arraylist_create();
		arraylist_add(temp->node_data_list, f_node);

		hashtable_set(ht, name, temp);
	}
}