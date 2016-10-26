#include "../Helper/layer_helper.h"

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void app_thread_entry(void *param);
#define APP_THREAD_PROI         (14)
#define APP_THREAD_STACK_SIZE   (1024)
#define APP_THREAD_TICK		(50)
#define MAX_EVENT_COUNT			(5)
#endif

/** 
 *  初始化某个APP实例信息
 * 
 * @param info APP实例
 * @param asdu_addr ASDU地址
 * @param asdu_addr_len ASDU地址长度
 * @param cause_len 传送原因长度
 * @param node_addr_len 地址长度
 * @param sm2_enable SM2功能启用
 */
void app_init(struct app_info *info, int asdu_addr, int asdu_addr_len,int cause_len,int node_addr_len,int sm2_enable)
{
	rt_memset(info, 0, sizeof(struct app_info));
	
	info->cfg.asdu_addr = asdu_addr;
	info->cfg.asdu_addr_len = asdu_addr_len;
	info->cfg.cause_len = cause_len;
	info->cfg.node_addr_len = node_addr_len;
	info->cfg.sm2_enable = sm2_enable;


	info->n_node_list = arraylist_create();

	info->first_task = arraylist_create();
	info->second_task = arraylist_create();
	info->buffered = arraylist_create();

#if(CFG_RUNNING_MODE==MUTLI_MODE)
	info->app_event =rt_mb_create("appmb", MAX_EVENT_COUNT, RT_IPC_FLAG_FIFO); 
#endif
}

/** 
 * 生成一个APP
 * 
 * @param asdu_addr ASDU地址
 * @param asdu_addr_len ASDU地址长度
 * @param cause_len 传送原因长度
 * @param node_addr_len 信息点地址长度
 * @param sm2_enable SM2功能启用
 * 
 * @return APP
 */
struct app_info *app_create(int asdu_addr, int asdu_addr_len, int cause_len, int node_addr_len, int sm2_enable)
{
	struct app_info *info = (struct app_info *)rt_malloc(sizeof(struct app_info));
	if (info == 0)
	{
		rt_kprintf("applayer configure malloc fail.\n");
		return 0;
	}

	app_init(info, asdu_addr, asdu_addr_len, cause_len, node_addr_len, sm2_enable);

  return info;
}


/*添加信息点*/
void app_create_normal_node(struct app_info *info,int *normal_node)
{
	arraylist_add(info->n_node_list, normal_node);
}



/** 
 *  APP-接收端收到ASDU类数据处理函数
 * 
 * @param info APP
 * @param asdu_data ASDU字节数据
 */
static void app_evt_dispatch_recv_asdu(struct app_info *info,int link_id,char *asdu_data,int asdu_len)
{
  /*ASDU地址检测 传输原因检测*/
  struct app_recv_info recv_info;
  rt_memset(&recv_info,0,sizeof(struct app_recv_info));

  if(iec_get_asdu_cfg(asdu_data[0])==0)
    {
      recv_info.ack_cause=Unknowtype;
      return ;
    }

  int recv_asdu_addr=0;
  rt_memcpy(&recv_asdu_addr, &asdu_data[3], info->cfg.asdu_addr_len);

  if(recv_asdu_addr!=info->cfg.asdu_addr)
    {
      recv_info.ack_cause=Unknowasdu;
      return;
    }

  recv_info.asdu_ident=asdu_data[0];
  rt_memcpy(&recv_info.cause,&asdu_data[2],info->cfg.cause_len);
  recv_info.node_count=asdu_data[1]&0x7F;
  recv_info.seq=(asdu_data[1]>>7)&0x1;
  recv_info.asdu_sub_len=asdu_len-2-info->cfg.cause_len-info->cfg.asdu_addr_len;
  rt_memcpy(&recv_info.asdu_sub_data,&asdu_data[2+info->cfg.cause_len+info->cfg.asdu_addr_len],
            recv_info.asdu_sub_len);

  app_linkframe_convert_to_asdu(info, &recv_info);

  app_task_insert_ack_asdu(info,link_id,&recv_info);
 

}


/** 
 * APP --接收link端数据处理
 * 
 * @param info APP实例
 * @param evt 事件类型 包含 请求一类数据  请求二类数据   用户类数据
 */
static void app_evt_recv_data_handle(struct app_info *info,struct iec_event *evt)
{
  int sub_evt=evt->evt_sub_type;

  struct app_task *task_temp=0;
  struct app_send_info *send_info = 0;
  int *recv=0;

  switch (sub_evt)
  {
  case EVT_SUB_DAT_LEVEL_1:
	  task_temp = app_task_get(info->first_task);
    if(task_temp!=0)
      {
        send_info = app_task_covert_to_asdu_frame(info, task_temp);
        app_send_asdu_evt_to_link(info,send_info);
      }
    break;
  case EVT_SUB_DAT_LEVEL_2:
	  task_temp = app_task_get(info->second_task);
    if(task_temp!=0)
      {
        send_info = app_task_covert_to_asdu_frame(info, task_temp);
        app_send_asdu_evt_to_link(info, send_info);
      }
    break;
  case EVT_SUB_DAT_USER:
    recv=evt->sub_msg;
	  app_evt_dispatch_recv_asdu(info, evt->sender, (char *)recv[0],recv[1]);
    /*发送事件至Link 确认用户数据*/
    app_send_userdata_ack_evt_to_link(info,(struct serial_link_info*)evt->sender);
    break;
  }

}


/** 
 * APP-- 更新信息点事件处理
 * 
 * @param info APP实例
 * @param evt 事件 信息点更新信息及具体信息点数据
 */
static void app_evt_update_node_handle(struct app_info *info, struct iec_event *evt)
{
	int res = 0,i=0;
	struct node_update_info *nd_info = (struct node_update_info *)evt->main_msg;

  struct node_frame_info  *nd_frame_info=0;
  struct seq_node_frame_info *s_nd_frame_info=0;
  struct link_obj *link_info=0;
  arraylist *task_list = 0;

  int *temp=0;

	if (nd_info->level == EVT_SUB_DAT_LEVEL_1)
	{
		task_list = info->first_task;
	}
	else if (nd_info->level == EVT_SUB_DAT_LEVEL_2)
	{
		task_list = info->second_task;
	}

  /**
   * 1、检测APP缓存的链路是否有效
   * 2、获取link的激活状态,默认为总召后激活
   * 3、注册任务
   */

  for(i=0;i<CFG_LINK_MAX;i++) 
    {
      if(info->linklayer_id[i]==0)
        continue;

      link_info=(struct link_obj *)info->linklayer_id[i];

      if(link_get_active_state(link_info)==0)
        continue;

      if (evt->evt_sub_type == EVT_SUB_NORMAL_NODE)
        {
          nd_frame_info=rt_malloc(sizeof(struct node_frame_info));
          rt_memcpy(nd_frame_info,evt->sub_msg,sizeof(struct node_frame_info));
          res = app_task_add_normal(task_list,info->linklayer_id[i], nd_info->asdu_ident, nd_info->cause,
                                    nd_frame_info);
        }

      if(res>=0)
        {
          app_send_update_evt_to_link(info, link_info,nd_info->level);
        }
    }

}



#if(CFG_RUNNING_MODE==MUTLI_MODE)
void app_thread_entry(void *param)
{
	struct app_info *info = (struct app_info *)param;

	struct iec_event *evt = 0;

  int res=0;

	while (1)
	{
		evt = iec_recv_event(info->app_event, RT_WAITING_FOREVER);

    if(evt==0)
      continue;

		switch (evt->evt_type)
		{
		case EVT_APP_ADD_NODE:
			if (evt->evt_sub_type == EVT_SUB_NORMAL_NODE)
			{
				app_create_normal_node(info, evt->sub_msg);
			}
			break;
		case EVT_APP_NODE_UPDATE:	
			app_evt_update_node_handle(info, evt);
			break;		/*ÐÅÏ¢µã±ä»¯*/
		case EVT_APP_RECV_DATA: /*±»¶¯ÊÕµ½LINKÖÁASDUÊý¾Ý*/
			app_evt_recv_data_handle(info, evt);
			break;		
		case EVT_APP_SEND_DATA:	/*Ö÷¶¯·¢ËÍASDUÊý¾Ý,ASDUÊý¾ÝÓÉÓÃ»§Ó¦ÓÃ²úÉú*/			
			break;		
		case EVT_APP_CTRL_OP:					
			break;		/*¿ØÖÆ²Ù×÷*/
		case EVT_APP_SET_OP:					
			break;		/*ÉèÖÃÐÞ¸Ä²Ù×÷*/
		case EVT_APP_READ_OP:					
			break;		/*¶ÁÈ¡²Ù×÷*/
		case EVT_APP_FILE_OP:					
			break;		/*ÎÄ¼þ²Ù×÷*/
		}

		iec_free_event(evt);
	}
}

void app_start(int papp_info)
{
	struct app_info *info = (struct app_info *)papp_info;
	info->app_tid =rt_thread_create("app", app_thread_entry, info, APP_THREAD_STACK_SIZE, APP_THREAD_PROI, APP_THREAD_TICK);
  rt_thread_startup(info->app_tid);
}
#endif
