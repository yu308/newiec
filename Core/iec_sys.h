#ifndef _iec_sys_h_
#define _iec_sys_h_
#include "iec_cfg.h"
#include "../Layer/Helper/layer_helper.h"

/// <summary>
/// 系统信息
/// </summary>
struct sys_info
{
	int communicate_role;	/*系统通信角色*/

	arraylist *link_obj;	 /*当前链路*/
	arraylist *app_obj;	/*当前APP*/

	rt_mailbox_t sys_event;	/*系统事件*/
};


extern struct sys_info  gSys_Info;

#endif
