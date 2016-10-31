#include "../../Core/iec_event.h"
#include "../Helper/layer_helper.h"
#include "../Iec/iec_helper.h"

#define TASK_IDENT_NAME_FORMAT		"%08X-%02X-%02X-%02X"			/*LINKID-ASDU-CAUSE-SEQ*/


int app_task_free(arraylist *task_list,struct app_task *task,int clear)
{
  struct app_task *temp=0;
  struct node_frame_info *node_temp=0;
  int i=0;

  int node_count=arraylist_size(task->node_data_list);
  if(clear==0&&node_count>0) /*任务中有信息点数据 清除标识设置为0 时 直接返回*/
    return 0;

  if(clear==1) /*清除标识为1时 强制清除 首先清除已有的信息点数据*/
    {
      arraylist_iterate(task->node_data_list, i, node_temp)
        {
          rt_free(node_temp->byte_buff);
          rt_free(node_temp);
        }
      i=0;
      arraylist_destroy(task->node_data_list);
    }
  arraylist_iterate(task_list, i, temp)
    {
      if(rt_strcmp(temp->task_name, task->task_name)==0)
        {
          arraylist_remove(task_list, i);
          break;
        }
    }
  rt_free(task);
}


/** 
 * 添加通用信息点任务
 * 
 * @param al 任务链表
 * @param link_id 任务所属链路id
 * @param asdu_ident ASDU标识
 * @param cause 传输原因
 * @param f_node 信息点数据信息
 * 
 * @return 创建状态 >=0 当前任务索引 成功  -1 已有数据 
 */
int app_task_add_normal(arraylist *al,unsigned int link_id,unsigned int asdu_ident, int cause, struct node_frame_info *f_node)
{
	char name[CFG_NAME_MAX];
	struct node_frame_info *f_node_temp = 0;
	int i = 0,j=0,seq=(f_node->vsq>>7)&0x1;
	rt_memset(name, 0, CFG_NAME_MAX);
  rt_sprintf(name,TASK_IDENT_NAME_FORMAT,link_id,asdu_ident,cause,seq);
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
                          rt_memcpy(f_node_temp->byte_buff,f_node->byte_buff,f_node->data_len);
                          f_node_temp->data_len=f_node->data_len;
                          rt_free(f_node);
                          return i;
                        }
                    }
                }
              else 
                {
                  arraylist_add(temp->node_data_list, f_node);
                  return i;
                }	
          
            }

        } 
    }

  temp = rt_malloc(sizeof(struct app_task));
  rt_memset(temp, 0, sizeof(struct app_task));
  temp->asdu_ident = asdu_ident;
  temp->cause = cause;
  temp->seq = seq;
  temp->link_id=link_id;
  rt_memcpy(temp->task_name, name, CFG_NAME_MAX);

  temp->node_data_list = arraylist_create();
  arraylist_add(temp->node_data_list, f_node);

  arraylist_add(al, temp);
  return i;
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
int app_task_pack_node(char *buff,int node_addr_len, arraylist *node_list,int *out_count)
{
	int len = 0,count=0;
	struct node_frame_info *f_node = 0;
	

  /*确保数据长度不超过协议规定长度*/
	while (len < (CFG_ASDU_DATA_BUFF_MAX - 20))
	{
		if (arraylist_size(node_list) > 0)
		{
			f_node = arraylist_remove(node_list, 0);
			if (f_node != 0)
			{
				if(((f_node->vsq>>7)&0x1)>0)
					count=f_node->vsq&0x7F;
				else 
					count++;
				iec_pack_node_addr(buff,f_node->addr,node_addr_len);
				len+=node_addr_len;
				rt_memcpy(&buff[len],f_node->byte_buff,f_node->data_len);
				len+=f_node->data_len;
				rt_free(f_node);/*清除信息点数据,信息点数据为动态创建*/
			}
		}
		else
			break;
	}
	*out_count = count;
	return len;

}


/** 
 * 获取任务
 * 
 * @param al 
 * 
 * @return 若无任务，则返回0
 */
struct app_task *app_task_get(arraylist *al,unsigned int link_id)
{
  struct app_task *task=0;
  int i=0;
  for(i=0;i<al->size;i++)
  {
    task=arraylist_get(al, i);
    if(task->link_id==link_id)
      return task;
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

  
  rt_memcpy(&asdu_frame[idx], &task->cause, info->cfg.cause_len);
  idx += info->cfg.cause_len;

  rt_memcpy(&asdu_frame[idx], &info->cfg.asdu_addr, info->cfg.asdu_addr_len);
  idx += info->cfg.asdu_addr_len;


	  /*普通信息点处理*/
	idx+=app_task_pack_node(&asdu_frame[idx], info->cfg.node_addr_len,task->node_data_list,&node_count);
  


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

/*用户实现的控制命令接口*/
extern int app_frame_ctrl_cmd_user_callback(int asdu_ident,int node_addr,char *node_val,int node_data_len);
extern int app_frame_ctrl_sys_cmd_callback(int appid,int asdu_ident,char *node_val,int node_data_len);
/*{
  if(node_addr==0x4001)//单点命令
    {
      Info_E_SCO *sco=(Info_E_SCO*)node_val;
      if(sco->SE==CHOOSE)
        {
          //检测是否可以执行 可执行则返回1
        }
      else if(sco->SE==EXECUTE)
        {
          //执行动作 返回成功
        }
    }

}*/
#include "appsys.h"
extern struct system_dev gSystemDev;
int all_call_proc(int appid)
{
  struct node_frame_info *frame_info=0;
  Info_E_QDS qds;
  Info_E_SIQ siq;
  Info_E_QOI qoi;
  int state=switcher_check_status(gSystemDev.ftu_dev->swc,SWC_ST_CLOSE);
  siq.SPI=state;
  siq.BL=0;
  siq.IV=0;
  siq.NT=0;
  siq.RES=0;
  siq.SB=0;
  
  frame_info=(struct node_frame_info *)iec_api_gen_node_info(0x1,0);
  iec_api_add_element_to_node(frame_info,SIQ,&siq);
  iec_api_update_normal_node(appid,EVT_SUB_DAT_LEVEL_1,M_SP_NA,20,frame_info);
  
  frame_info=(struct node_frame_info *)iec_api_gen_node_info(0x4001,0);
  iec_api_add_element_to_node(frame_info,SVA,&gSystemDev.ftu_dev->c_realdata.Uab);
 	qds.OV=0;
	qds.RES=0;
	qds.BL=0;
	qds.SB=0;
	qds.NT=0;
	qds.IV=0;
  iec_api_add_element_to_node(frame_info,QDS,&qds);
  iec_api_update_normal_node(appid,EVT_SUB_DAT_LEVEL_1,M_ME_NB,20,frame_info);
  

  qoi=GOL_CALL;
   frame_info=(struct node_frame_info *)iec_api_gen_node_info(0x0,0);
  iec_api_add_element_to_node(frame_info,QOI,&qoi);
  iec_api_update_normal_node(appid,EVT_SUB_DAT_LEVEL_1,C_IC_NA,Actterm,frame_info);
}

int app_frame_ctrl_sys_cmd_callback(int appid,int asdu_ident,char *node_val,int node_data_len)
{
  if(asdu_ident==C_IC_NA)
  {
    Info_E_QOI *qoi=(Info_E_QOI*)node_val;
    if((*qoi)==GOL_CALL)
    {
      all_call_proc(appid);
    }
  }
  return 1;
}
/** 
 * 控制命令处理,具体过程由用户提供的接口实现
 * 
 * @param asdu_ident ASDU标识
 * @param node_addr 信息点地址
 * @param node_data 信息点数据
 * @param node_data_len 信息点数据长度 
 */
static int app_frame_ctrl_cmd_proc(int asdu_ident,int node_addr,char *node_data,int node_data_len)
{
  //return app_frame_ctrl_cmd_user_callback(asdu_ident,node_addr,node_data,node_data_len);
}

static int app_frame_ctrl_sys_cmd_proc(int appid,int asdu_ident,char *node_data,int node_data_len)
{
  return app_frame_ctrl_sys_cmd_callback(appid,asdu_ident,node_data,node_data_len);
}


/** 
 * APP对链路送至的ASDU数据转换处理,准备创建发送信息,建立任务
 * 
 * @param info APP信息
 * @param recv_info  收到的ASDU信息
 */
void app_linkframe_convert_to_asdu(struct app_info *info,struct app_recv_info *recv_info)
{
  int node_addr=0,i=0,state=0;

  if(recv_info->node_count>1)
    return;

  rt_memcpy(&node_addr,&recv_info->asdu_sub_data[0],info->cfg.node_addr_len);

  /*检测APP是否支持信息点地址,移至用户接口判断*/

   if((recv_info->asdu_ident>0)&&(recv_info->asdu_ident<41))
    {
      /*监视方向过程信息处理*/
    }
  else if((recv_info->asdu_ident>44)&&(recv_info->asdu_ident<52))
    {
      /*控制方向过程控制信息处理*/
      if(recv_info->cause==Act)
        {
          state=app_frame_ctrl_cmd_proc(recv_info->asdu_ident,node_addr,&recv_info->asdu_sub_data[info->cfg.node_addr_len],
                                        recv_info->asdu_sub_len-info->cfg.node_addr_len);

          if(state==1)
            {
              recv_info->ack_cause=Actcon;
            }
          else
            recv_info->ack_cause=Actterm;

        }
      else if(recv_info->cause==Deact)
        {
          recv_info->ack_cause=Deactcon;
        }
    }    
  else if((recv_info->asdu_ident>69)&&(recv_info->asdu_ident<98))
    {
      /*监视方向过程控制信息处理*/
    }
  else if((recv_info->asdu_ident>99)&&(recv_info->asdu_ident<107))
    {
      /*控制方向的系统命令*/
       recv_info->ack_cause=Actcon;
        app_frame_ctrl_sys_cmd_proc((int)info,recv_info->asdu_ident,&recv_info->asdu_sub_data[info->cfg.node_addr_len],
                                        recv_info->asdu_sub_len-info->cfg.node_addr_len);
    }
  else if((recv_info->asdu_ident>109)&&(recv_info->asdu_ident<114))
    {
      /*控制方向的参数命令*/
     
    }
  else if((recv_info->asdu_ident>119)&&(recv_info->asdu_ident<127))
    {
      /*文件传输*/
    }


}


void app_task_insert_ack_asdu(struct app_info *info,int link_id,struct app_recv_info *recv_info)
{
  arraylist *al=info->first_task;
  int idx=0;
  int res=0;
  char *task_name=rt_malloc(24);
  rt_sprintf(task_name, "%08X-%02X-%02X-%02X", link_id,recv_info->asdu_ident,recv_info->cause,recv_info->seq);

  struct asdu_cfg *cfg=iec_get_asdu_cfg(recv_info->asdu_ident);

  struct node_frame_info *n_node_info=rt_malloc(sizeof(struct node_frame_info));

  int node_addr=0;
  rt_memcpy(&node_addr, &recv_info->asdu_sub_data[0], info->cfg.node_addr_len);
  idx+=info->cfg.node_addr_len;

  n_node_info->addr=node_addr;
  n_node_info->buffered=0;
  n_node_info->vsq=recv_info->seq<<7|recv_info->node_count;
  n_node_info->data_len=recv_info->asdu_sub_len-info->cfg.node_addr_len;
  rt_memcpy(n_node_info->byte_buff,&recv_info->asdu_sub_data[idx],n_node_info->data_len);
  //if(cfg->tm_ident!=0)
  /* 转化为utc时间 时间标签*/
  res=app_task_add_normal(al, link_id, recv_info->asdu_ident, recv_info->ack_cause, n_node_info);


  /*将要发送的ACK ASDU任务移到首位,保证下次优先发送*/
  void* item= arraylist_remove(al, res);
  arraylist_insert(al, 0, item);
  
}
