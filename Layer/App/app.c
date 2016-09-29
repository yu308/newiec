#include "app.h"

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

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void app_thread_entry(void *param)
{
	struct app_info *info = (struct app_info *)param;

	struct iec_msg *msg = 0;

	while (1)
	{
		msg=
	}
}

void app_start(int papp_info)
{
	struct app_info *info = (struct app_info *)papp_info;
	info->app_tid = osThreadCreate(osThread(app), info);
}
#endif
