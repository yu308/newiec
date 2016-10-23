#include "../../Core/iec_event.h"
#include "../Helper/layer_helper.h"
#include "../Iec/iec_helper.h"

#define TASK_IDENT_NAME_FORMAT		"%08X-%02X-%02X-%02X"			/*LINKID-ASDU-CAUSE-SEQ*/

/** 
 * 添加通用信息点任务
 * 
 * @param al 任务链表
 * @param link_id 任务所属链路id
 * @param asdu_ident ASDU标识
 * @param cause 传输原因
 * @param f_node 信息点数据信息
 * 
 * @return 创建状态 0 成功  -1 已有数据 
 */
int app_task_add_normal(arraylist *al,unsigned int link_id,unsigned int asdu_ident, int cause, struct node_frame_info *f_node)
{
	char name[24];
	struct node_frame_info *f_node_temp = 0;
	int i = 0,j=0;
	rt_memset(name, 0, 16);
  rt_sprintf(name,TASK_IDENT_NAME_FORMAT,link_id,asdu_ident,cause,0);
  struct app_task *temp=0;

  /*查找是否已有同类任务*/
  arraylist_iterate(al,i,temp)
    {
      if(rt_strcmp(name, temp->task_name)==0)
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

  temp = rt_malloc(sizeof(struct app_task));
  rt_memset(temp, 0, sizeof(struct app_task));
  temp->asdu_ident = asdu_ident;
  temp->cause = cause;
  temp->seq = 0;
  temp->link_id=link_id;
  rt_memcpy(temp->task_name, name, 16);

  temp->node_data_list = arraylist_create();
  arraylist_add(temp->node_data_list, f_node);

  arraylist_add(al, temp);
  return 0;
}


int app_task_add_seq(arraylist *al,unsigned int link_id, unsigned int asdu_ident, int cause, struct seq_node_frame_info *f_s_node)
{
	char name[16];
	struct seq_node_frame_info *f_s_node_temp = 0;
	int i = 0, j = 0;
	rt_memset(name, 0, 16);
	rt_sprintf(name, TASK_IDENT_NAME_FORMAT,link_id, asdu_ident, cause, 1);
	struct app_task *temp = 0;

	arraylist_iterate(al, i, temp)
	{
		if (rt_strcmp(name, temp->task_name) == 0)
		{
			if (temp->node_data_list != 0)
			{
				if (f_s_node->buffered == 0) /*不缓存数据时,查找是否已有相同信息点数据*/
				{
					arraylist_iterate(temp->node_data_list, j, f_s_node_temp)
					{
						if (f_s_node_temp->addr == f_s_node->addr)
						{
							rt_free(f_s_node_temp->qual); /*序列化信息点 qual val必须为动态分配的内存地址*/
							rt_free(f_s_node_temp->val);

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

	temp =rt_malloc(sizeof(struct app_task));
  rt_memset(temp, 0, sizeof(struct app_task));
	temp->asdu_ident = asdu_ident;
	temp->cause = cause;
	temp->seq = 1;
  temp->link_id=link_id;
	rt_memcpy(temp->task_name, name, 16);

	temp->node_data_list = arraylist_create();
	arraylist_add(temp->node_data_list, f_s_node);

	arraylist_add(al, temp);
	return 0;
}


/** 
 * ASDU封装信息点数据
 * 
 * @param buff 数据缓存
 * @param asdu_ident ASDU标识
 * @param node_addr_len 地址长度
 * @param node_list ASDU信息点链表
 * @param out_count 封装的信息点数量
 * 
 * @return ASDU字节数据
 */
int app_task_pack_normal_node(char *buff,unsigned int asdu_ident, int node_addr_len, arraylist *node_list,int *out_count)
{
	int len = 0,count=0;
	struct asdu_cfg *acfg = iec_get_asdu_cfg(asdu_ident);
	struct node_frame_info *f_node = 0;
	

  /*确保数据长度不超过协议规定长度*/
	while (len < (CFG_ASDU_DATA_BUFF_MAX - 20))
	{
		if (arraylist_size(node_list) > 0)
		{
			f_node = arraylist_remove(node_list, 0);
			if (f_node != 0)
			{
				count++;
				len += iec_asdu_pack_node(buff, acfg, f_node->addr, node_addr_len, f_node->val, f_node->qual, f_node->utc_time, f_node->millsecond);
				rt_free(f_node);/*清除信息点数据,信息点数据为动态创建*/
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
				rt_free(f_s_node->val);/*清除序列化信息点数据*/
			if (f_s_node->qual)
				rt_free(f_s_node->qual);
			rt_free(f_s_node);/*清除序列化信息点数据,信息点数据为动态创建*/

		}
	}

	return len;
}


/** 
 * 获取任务
 * 
 * @param al 
 * 
 * @return 若无任务，则返回0
 */
struct app_task *app_task_get(arraylist *al)
{
  struct app_task *task=0;

  if(al->size>0)
    {
      task=arraylist_get(al, 0);
    }

  return task;

}

/** 
 * 任务转为ASDU字节数据帧
 * 
 * @param info APP应用信息
 * @param task 任务
 * 
 * @return ASDU字节数据帧
 */
struct app_send_info *app_task_covert_to_asdu_frame(struct app_info *info,struct app_task *task)
{
	int idx = 0, len = 0,node_count=0;
  char *asdu_frame=rt_malloc(CFG_ASDU_DATA_BUFF_MAX);
  struct app_send_info *send_info = rt_malloc(sizeof(struct app_send_info));
  if(asdu_frame==0)
    return 0;

  rt_memset(asdu_frame, 0, CFG_ASDU_DATA_BUFF_MAX);

  asdu_frame[idx++] = task->asdu_ident;
  asdu_frame[idx++] = (task->seq << 7);
  asdu_frame[idx++] = (task->cause&0xFF);
  
  rt_memcpy(&asdu_frame[idx], &task->cause, info->cfg->cause_len);
  idx += info->cfg->cause_len;

  rt_memcpy(&asdu_frame[idx], &info->cfg->asdu_addr, info->cfg->asdu_addr_len);
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

  send_info->link_id=task->link_id;
  send_info->app_data = asdu_frame;
  send_info->app_data_len = idx;
  return send_info;
}

/** 
 * 检测当前链路是否有数据传输任务
 * 
 * @param al 
 * @param link_id 
 * 
 * @return 有返回1 无则返回0
 */
int app_task_check_empty(arraylist *al,int link_id)
{
  int i=0;
  struct app_task *task_temp=0;

  if(arraylist_size(al)>0)
    {
      arraylist_iterate(al, i, task_temp)
        {
          if(task_temp->link_id==link_id)
            {
              if(arraylist_size(task_temp->node_data_list)>0)
                return 1;
            }
        }
    }

  return 0;
}
