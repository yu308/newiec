#include "iec_event.h"
#include "app_task.h"
#include "iec_node.h"
#include "../Iec/iec_asdu_table.h"
#include "../Helper/layer_helper.h"
#include "../Iec/iec_asdu.h"
#include <stdio.h>
#include <string.h>

#define TASK_IDENT_NAME_FORMAT		"%02X-%02X-%02X"			/*ASDU-CAUSE-SEQ*/

int app_task_add_normal(arraylist *al,unsigned int asdu_ident, int cause, struct node_frame_info *f_node)
{
	char name[16];
	struct node_frame_info *f_node_temp = 0;
	int i = 0,j=0;
	XMEMSET(name, 0, 16);
  sprintf(name,TASK_IDENT_NAME_FORMAT,asdu_ident,cause,0);
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
  temp->seq = 0;
  XMEMCPY(temp->task_name, name, 16);

  temp->node_data_list = arraylist_create();
  arraylist_add(temp->node_data_list, f_node);

  arraylist_add(al, temp);
  return 0;
}

int app_task_add_seq(arraylist *al, unsigned int asdu_ident, int cause, struct seq_node_frame_info *f_s_node)
{
	char name[16];
	struct seq_node_frame_info *f_s_node_temp = 0;
	int i = 0, j = 0;
	XMEMSET(name, 0, 16);
	sprintf(name, TASK_IDENT_NAME_FORMAT, asdu_ident, cause, 1);
	struct app_task *temp = 0;

	arraylist_iterate(al, i, temp)
	{
		if (strcmp(name, temp->task_name) == 0)
		{
			if (temp->node_data_list != 0)
			{
				if (f_s_node->buffered == 0) /*不缓存数据时,查找是否已有相同信息点数据*/
				{
					arraylist_iterate(temp->node_data_list, j, f_s_node_temp)
					{
						if (f_s_node_temp->addr == f_s_node->addr)
						{
							XFREE(f_s_node_temp->qual); /*序列化信息点 qual val必须为动态分配的内存地址*/
							XFREE(f_s_node_temp->val);

							f_s_node_temp->qual = f_s_node->qual;
							f_s_node_temp->val = f_s_node->val;
							f_s_node_temp->count = f_s_node->count;

							return -1;
						}
					}
				}
				arraylist_add(temp->node_data_list, f_s_node);
				return 0;
			}

		}
	}

	temp = XMALLOC(sizeof(struct app_task));
	XMEMSET(temp, 0, sizeof(struct app_task));
	temp->asdu_ident = asdu_ident;
	temp->cause = cause;
	temp->seq = 1;
	XMEMCPY(temp->task_name, name, 16);

	temp->node_data_list = arraylist_create();
	arraylist_add(temp->node_data_list, f_s_node);

	arraylist_add(al, temp);
	return 0;
}

int app_task_pack_normal_node(char *buff,unsigned int asdu_ident, int node_addr_len, arraylist *node_list,int *out_count)
{
	int len = 0,count=0;
	struct asdu_cfg *acfg = iec_get_asdu_cfg(asdu_ident);
	struct node_frame_info *f_node = 0;
	

	while (len < (CFG_ASDU_DATA_BUFF_MAX - 20))
	{
		if (arraylist_size(node_list) > 0)
		{
			f_node = arraylist_remove(node_list, 0);
			if (f_node != 0)
			{
				count++;
				len += iec_asdu_pack_node(buff, acfg, f_node->addr, node_addr_len, f_node->val, f_node->qual, f_node->utc_time, f_node->millsecond);
				XFREE(f_node);/*清除信息点数据,信息点数据为动态创建*/
			}
		}
		else
			break;
	}
	*out_count = count;
	return len;

}

int app_task_pack_seq_node(char *buff, unsigned int asdu_ident, int node_addr_len, arraylist *node_list,int *out_count)
{
	int len = 0;
	struct asdu_cfg *acfg = iec_get_asdu_cfg(asdu_ident);
	struct seq_node_frame_info *f_s_node = 0;

	if (arraylist_size(node_list) > 0)
	{
		f_s_node = arraylist_remove(node_list, 0);
		if (f_s_node != 0)
		{
			len += iec_asdu_pack_seq_node(buff, acfg, f_s_node->addr, node_addr_len,f_s_node->count,f_s_node->val, f_s_node->qual);
			
			*out_count = f_s_node->count;

			if (f_s_node->val)
				XFREE(f_s_node->val);/*清除序列化信息点数据*/
			if (f_s_node->qual)
				XFREE(f_s_node->qual);
			XFREE(f_s_node);/*清除序列化信息点数据,信息点数据为动态创建*/

		}
	}

	return len;
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

struct app_send_info *app_task_covert_to_asdu_frame(struct app_info *info,struct app_task *task)
{
	int idx = 0, len = 0,node_count=0;
  char *asdu_frame=XMALLOC(CFG_ASDU_DATA_BUFF_MAX);
  struct app_send_info *send_info = XMALLOC(sizeof(struct app_send_info));
  if(asdu_frame==0)
    return;

  XMEMSET(asdu_frame, 0, CFG_ASDU_DATA_BUFF_MAX);

  asdu_frame[idx++] = task->asdu_ident;
  asdu_frame[idx++] = (task->seq << 7);
  asdu_frame[idx++] = (task->cause&0xFF);
  
  XMEMCPY(&asdu_frame[idx], &task->cause, info->cfg->cause_len);
  idx += info->cfg->cause_len;

  XMEMCPY(&asdu_frame[idx], info->cfg->asdu_addr, info->cfg->asdu_addr_len);
  idx += info->cfg->asdu_addr_len;

  if (task->seq == 0)
  {
	  /*普通信息点处理*/
	  idx+=app_task_pack_normal_node(&asdu_frame[idx], task->asdu_ident, info->cfg->node_addr_len, task->node_data_list,&node_count);
  }
  else if (task->seq == 1)
  {
	  /*序列化信息点处理*/
	  idx += app_task_pack_seq_node(&asdu_frame[idx], task->asdu_ident, info->cfg->node_addr_len, task->node_data_list,&node_count);
  }

  asdu_frame[1] |= node_count;

  send_info->app_data = asdu_frame;
  send_info->app_data_len = idx;
}
