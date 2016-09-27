#ifndef _iec_sys_h_
#define _iec_sys_h_
#include "iec_cfg.h"

/// <summary>
/// 系统信息
/// </summary>
struct sys_info
{
	int communicate_role;	/*系统通信角色*/

	int link_obj[CFG_LINK_MAX];	 /*当前链路*/
	int app_obj[CFG_APP_MAX];	/*当前APP*/

	osMessageQId sys_event;	/*系统事件*/
};


extern struct sys_info  gSys_Info;

#endif