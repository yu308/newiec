#include "app.h"
#include "iec_event.h"

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void app_thread_entry(void *param);
#define APP_THREAD_TICK		(50)
#define MAX_EVENT_COUNT			(5)
/// <summary>
/// ��·��������
/// </summary>
osThreadDef(app, app_thread_entry, APP_TASK_PROI, APP_THREAD_TICK, 2048);
/// <summary>
/// ��·������Ϣ����
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
	info->first_task = 0;
	info->second_task = 0;

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


#if(CFG_RUNNING_MODE==MUTLI_MODE)
void app_thread_entry(void *param)
{
	struct app_info *info = (struct app_info *)param;

	struct iec_event *event = 0;

	while (1)
	{
		event = iec_recv_msg(info->app_event, osWaitForever);

		switch (event->evt_type)
		{
		case EVT_APP_ADD_NODE:
			break;
		case EVT_APP_NODE_UPDATE:				
			break;		/*��Ϣ��仯*/
		case EVT_APP_RECV_DATA: /*�����յ�LINK��ASDU����*/
			if (event->msg->m_app_recv_info.funcode==APP_FUN_FIRST)	/*����һ������ ���һ�����񻺴�*/
			{
				/*��������,������Ϣ��Ϣ*/
			}
			else if (event->msg->m_app_recv_info.funcode == APP_FUN_SECOND) /*����������� ���������񻺴�*/
			{
				/*��������,������Ϣ��Ϣ*/
			}
			else if (event->msg->m_app_recv_info.funcode == APP_FUN_USER) /*�û�Ӧ������*/
			{
				/*���ơ��ļ�����������*/
			}
			break;		
		case EVT_APP_SEND_DATA:	/*��������ASDU����,ASDU�������û�Ӧ�ò���*/			
			break;		
		case EVT_APP_CTRL_OP:					
			break;		/*���Ʋ���*/
		case EVT_APP_SET_OP:					
			break;		/*�����޸Ĳ���*/
		case EVT_APP_READ_OP:					
			break;		/*��ȡ����*/
		case EVT_APP_FILE_OP:					
			break;		/*�ļ�����*/
		}
	}
}

void app_start(int papp_info)
{
	struct app_info *info = (struct app_info *)papp_info;
	info->app_tid = osThreadCreate(osThread(app), info);
}
#endif