#include "../Helper/layer_helper.h"

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void app_thread_entry(void *param);
#define APP_THREAD_TICK		(50)
#define MAX_EVENT_COUNT			(5)
/// <summary>
/// 链路任务配置
/// </summary>
osThreadDef(app, app_thread_entry, APP_TASK_PROI, APP_THREAD_TICK, 2048);
/// <summary>
/// 链路任务消息队列
/// </summary>
osMessageQDef(appevent, MAX_EVENT_COUNT, 4);
#endif


void app_init(struct app_info *info, int asdu_addr, int asdu_addr_len,int cause_len,int node_addr_len,int sm2_enable)
{
	XMEMSET(info, 0, sizeof(struct app_info));
	struct app_cfg *cfg = (struct app_cfg *)XMALLOC(sizeof(struct app_cfg));

	cfg->asdu_addr = asdu_addr;
	cfg->asdu_addr_len = asdu_addr_len;
	cfg->cause_len = cause_len;
	cfg->node_addr_len = node_addr_len;
	cfg->sm2_enable = sm2_enable;

	info->cfg = cfg;


	info->n_node_list = arraylist_create();
	info->s_node_list = arraylist_create();

	info->first_task = arraylist_create();
	info->second_task = arraylist_create();
	info->buffered = arraylist_create();

#if(CFG_RUNNING_MODE==MUTLI_MODE)
	info->app_event = osMessageCreate(osMessageQ(appevent), 0);
#endif
}

struct app_info *app_create(int asdu_addr, int asdu_addr_len, int cause_len, int node_addr_len, int sm2_enable)
{
	struct app_info *info = (struct app_info *)XMALLOC(sizeof(struct app_info));
	if (info == 0)
	{
		XPRINTF("applayer configure malloc fail.\n");
		return 0;
	}

	app_init(info, asdu_addr, asdu_addr_len, cause_len, node_addr_len, sm2_enable);
}

int app_check_data()
{
  
}


void app_create_normal_node(struct app_info *info,int *normal_node)
{
	arraylist_add(info->n_node_list, normal_node);
}

void app_create_seq_node(struct app_info *info, int *seq_node)
{
	arraylist_add(info->s_node_list, seq_node);
}


static void app_evt_dispatch_recv_asdu(struct app_info *info,int *asdu_data)
{
  
}


static void app_evt_recv_data_handle(struct app_info *info,struct iec_event *evt)
{
  int sub_evt=evt->evt_sub_type;

  struct app_task *task_temp=0;
  struct app_send_info *send_info = 0;

  switch (sub_evt)
  {
  case EVT_SUB_DAT_LEVEL_1:
	  task_temp = app_task_get(info->first_task);
	  send_info = app_task_covert_to_asdu_frame(info, task_temp);
	  break;
  case EVT_SUB_DAT_LEVEL_2:
	  task_temp = app_task_get(info->second_task);
	  send_info = app_task_covert_to_asdu_frame(info, task_temp);
	  break;
  case EVT_SUB_DAT_USER:
	  app_evt_dispatch_recv_asdu(info, evt->sub_msg);
	  break;
  }

}



static void app_evt_update_node_handle(struct app_info *info, struct iec_event *evt)
{
	int res = 0,i=0;
	struct node_update_info *nd_info = (struct node_update_info *)evt->main_msg;

  struct serial_link_info *link_info=0;
  arraylist *task_list = 0;

	if (nd_info->level == EVT_SUB_DAT_LEVEL_1)
	{
		task_list = info->first_task;
	}
	else if (nd_info->level == EVT_SUB_DAT_LEVEL_2)
	{
		task_list = info->second_task;
	}

  for(i=0;i<CFG_LINK_MAX;i++)
    {
      if(info->linklayer_id[i]==0)
        continue;

      link_info=(struct serial_link_info *)info->linklayer_id[i];

      if(serial_link_get_active_state(link_info)==0)
        continue;

      if (evt->evt_sub_type == EVT_SUB_NORMAL_NODE)
        {
          res = app_task_add_normal(task_list,info->linklayer_id[i], nd_info->asdu_ident, nd_info->cause,
                                    evt->sub_msg);
        }
      else if (evt->evt_sub_type == EVT_SUB_SEQ_NODE)
        {
          res = app_task_add_seq(task_list,info->linklayer_id[i], nd_info->asdu_ident, nd_info->cause, evt->sub_msg);
        }

      if (res == -1)
        XFREE(evt->sub_msg);
      else
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
		evt = iec_recv_event(info->app_event, osWaitForever);

		if (evt == 0)
			continue;

		switch (evt->evt_type)
		{
		case EVT_APP_ADD_NODE:
			if (evt->evt_sub_type == EVT_SUB_NORMAL_NODE)
			{
				app_create_normal_node(info, evt->sub_msg);
			}
			else if (evt->evt_sub_type == EVT_SUB_SEQ_NODE)
			{
				app_create_seq_node(info, evt->sub_msg);
			}
			break;
		case EVT_APP_NODE_UPDATE:	
			app_evt_update_node_handle(info, evt);
			break;		/*信息点变化*/
		case EVT_APP_RECV_DATA: /*被动收到LINK至ASDU数据*/
			app_evt_recv_data_handle(info, evt);
			break;		
		case EVT_APP_SEND_DATA:	/*主动发送ASDU数据,ASDU数据由用户应用产生*/			
			break;		
		case EVT_APP_CTRL_OP:					
			break;		/*控制操作*/
		case EVT_APP_SET_OP:					
			break;		/*设置修改操作*/
		case EVT_APP_READ_OP:					
			break;		/*读取操作*/
		case EVT_APP_FILE_OP:					
			break;		/*文件操作*/
		}

		iec_free_event(evt);
	}
}

void app_start(int papp_info)
{
	struct app_info *info = (struct app_info *)papp_info;
	info->app_tid = osThreadCreate(osThread(app), info);
}
#endif
