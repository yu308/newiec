#include "iec_event.h"
#include "app_task.h"
#include "iec_node.h"
#include "../../Iec/arraylist.h"
#include <stdio.h>
#include <string.h>

#define TASK_IDENT_NAME_FORMAT		"%02X-%02X-%02X"			/*ASDU-CAUSE-SEQ*/

int app_task_add_normal(arraylist *al,unsigned int asdu_ident, int cause,int seq, struct node_frame_info *f_node)
{
	char name[16];
	struct node_frame_info *f_node_temp = 0;
	int i = 0,j=0;
	XMEMSET(name, 0, 16);
  sprintf(name,TASK_IDENT_NAME_FORMAT,asdu_ident,cause,seq);
  struct app_task *temp=0;

  /*查找是否已有同类任务*/
  arraylist_iterate(al,i,temp)
    {
      if(strcmp(name, temp->task_name)==0)
        {
          if (temp->node_data_list != 0)
            {
              if (f_node->buffered == 0) /*不缓存数据时,查找是否已有相同信息点数据*/
                {
                  arraylist_iterate(temp->node_data_list, j, f_node_temp)
                    {
                      if (f_node_temp->addr == f_node->addr)
                        {
                          f_node_temp->qual = f_node->qual;
                          f_node_temp->utc_time = f_node->utc_time;
                          f_node_temp->val = f_node->val;
                          return -1; 
                        }
                    }
                }
              arraylist_add(temp->node_data_list, f_node);
              return 0;
            }

        } 
    }

		temp = XMALLOC(sizeof(struct app_task));
		XMEMSET(temp, 0, sizeof(struct app_task));
		temp->asdu_ident = asdu_ident;
		temp->cause = cause;
		XMEMCPY(temp->task_name, name, 16);

		temp->node_data_list = arraylist_create();
		arraylist_add(temp->node_data_list, f_node);

		arraylist_add(al, temp);
    return 0;
}

struct app_task *app_task_get(arraylist *al)
{
  struct app_task *task=0;

  if(al->size>0)
    {
      task=arraylist_get(al, 0);
    }

  return task;

}

void app_task_covert_asdu_frame(struct app_task *task)
{
  char *asdu_frame=XMALLOC(200);
  if(asdu_frame==0)
    return;


}
