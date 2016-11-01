#ifndef _iec_sys_h_
#define _iec_sys_h_

#include "core.h"
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

WEAK void iec_sys_create_app_complete(char *name,unsigned int appid);
WEAK void iec_sys_create_link_complete(char *name,unsigned int linkid);

extern void iec_sys_api_create_link(char *link_name,int link_type,unsigned int link_addr,int link_addr_len,int dir);
extern void iec_sys_api_start_link(char *link_name);

#endif
