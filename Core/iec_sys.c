#include	"iec_cfg.h"
#include	"iec_event.h"
#include	"iec_sys.h"

/// <summary>
/// ȫ��ϵͳ��Ϣ
/// </summary>
struct sys_info  gSys_Info;


void iec_main_thread_entry(void *param);
#define MAIN_THREAD_TICK		(50)
#define MAX_EVENT_COUNT			(5)
/// <summary>
/// ����������
/// </summary>
osThreadDef(main, iec_main_thread_entry, SYS_TASK_PROI, MAIN_THREAD_TICK, 2048);
/// <summary>
/// ϵͳ��Ϣ����
/// </summary>
osMessageQDef(sysevent, MAX_EVENT_COUNT, 4);


/// <summary>
/// ������
/// </summary>
/// <param name="param">The parameter.</param>
void iec_main_thread_entry(void *param)
{
	struct sys_info *info = (struct sys_info*)param;
	
	XPRINTF("IEC:SYS: system is start.\n");

	struct iec_msg *msg = 0;

	while (1)
	{
		iec_recv_msg(info->sys_event, osWaitForever);
		if (msg == 0)
			continue;

		switch (msg->evt_type)
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
/// ϵͳ��ʼ��
/// </summary>
void iec_init_sysinfo()
{
	gSys_Info.communicate_role = CFG_ROLE_MODE;
	gSys_Info.sys_event = osMessageCreate(osMessageQ(sysevent), 0);
}

/// <summary>
/// ϵͳ����
/// </summary>
void iec_start_sys()
{
	osThreadCreate(osThread(main), &gSys_Info);
}


