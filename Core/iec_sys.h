#ifndef _iec_sys_h_
#define _iec_sys_h_
#include "iec_cfg.h"

/// <summary>
/// ϵͳ��Ϣ
/// </summary>
struct sys_info
{
	int communicate_role;	/*ϵͳͨ�Ž�ɫ*/

	int link_obj[CFG_LINK_MAX];	 /*��ǰ��·*/
	int app_obj[CFG_APP_MAX];	/*��ǰAPP*/

	osMessageQId sys_event;	/*ϵͳ�¼�*/
};


extern struct sys_info  gSys_Info;

#endif