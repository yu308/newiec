#include "app.h"
#include "../../Core/iec_event.h"
#include "app_task.h"
#include "../Link/serial_link.h"


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


void app_send_evt_to_link(struct app_info *info,int level)
{
  int i=0;
  struct iec_event *evt = 0;
  for(i=0;i<CFG_LINK_MAX;i++)
    {
      evt=iec_create_event(info, info->linklayer_id[i],EVT_LINK_RECV_DATA, 0, 0);
      iec_set_event_sub(evt,level,0,0);
      iec_post_event(((struct serial_link_info *)info->linklayer_id[i])->serial_event, evt, 20);
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
			if (evt->evt_sub_type == EVT_SUB_NORMAL_NODE)
			{
				struct normal_node_update_info *nd_info = (struct normal_node_update_info *)evt->main_msg;
				arraylist *task_list = 0;
				if (nd_info->level == EVT_SUB_DAT_LEVEL_1)
				{
					task_list = info->first_task;
				}
				else if (nd_info->level == EVT_SUB_DAT_LEVEL_2)
				{
					task_list = info->second_task;
				}
				res = app_task_add_norml(task_list, nd_info->asdu_ident, nd_info->cause, nd_info->seq,
					evt->sub_msg);
				if (res == -1)
					XFREE(evt->sub_msg);
				else
				{
					app_send_evt_to_link(info, nd_info->level);
				}
      }
			break;		/*信息点变化*/
		case EVT_APP_RECV_DATA: /*被动收到LINK至ASDU数据*/
			if (evt->msg->m_app_recv_info.funcode==APP_FUN_FIRST)	/*请求一类数据 检测一类任务缓存*/
			{
				/*若有任务,配置消息信息*/
			}
			else if (evt->msg->m_app_recv_info.funcode == APP_FUN_SECOND) /*请求二类数据 检测二类任务缓存*/
			{
				/*若有任务,配置消息信息*/
			}
			else if (evt->msg->m_app_recv_info.funcode == APP_FUN_USER) /*用户应用数据*/
			{
				/*控制、文件、参数设置*/
			}
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
