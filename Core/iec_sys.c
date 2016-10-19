#include	"iec_cfg.h"
#include	"iec_event.h"
#include	"iec_sys.h"

/// <summary>
/// 全局系统信息
/// </summary>
struct sys_info  gSys_Info;


void iec_main_thread_entry(void *param);
#define MAIN_THREAD_PROI    (15)
#define MAIN_THREAD_TICK		(50)
#define MAIN_THREAD_STACK_SIZE   (512)
#define MAX_EVENT_COUNT			(5)

/// <summary>
/// 主任务
/// </summary>
/// <param name="param">The parameter.</param>
void iec_main_thread_entry(void *param)
{
	struct sys_info *info = (struct sys_info*)param;
	
	rt_kprintf("IEC:SYS: system startup.\n");

	struct iec_event *evt = 0;

	while (1)
	{
		iec_recv_event(info->sys_event, RT_WAITING_FOREVER);

		switch (evt->evt_type)
		{
		case EVT_SYS_CREATE_LINK:
			break;
		case EVT_SYS_CREATE_APP:
			break;
		case EVT_SYS_DEL_LINK:
			break;
		case EVT_SYS_DEL_APP:
			break;
		default:
			break;
		}
	}

}



/// <summary>
/// 系统初始化
/// </summary>
void iec_init_sysinfo()
{
	gSys_Info.communicate_role = CFG_ROLE_MODE;
	gSys_Info.sys_event = rt_mb_create("sysevt", MAX_EVENT_COUNT, RT_IPC_FLAG_FIFO);
}

/// <summary>
/// 系统启动
/// </summary>
void iec_start_sys()
{
  rt_thread_t tid=rt_thread_create("sys", iec_main_thread_entry,&gSys_Info,MAIN_THREAD_STACK_SIZE,MAIN_THREAD_PROI,MAIN_THREAD_TICK);
  rt_thread_startup(tid);
}


