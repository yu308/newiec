#ifndef _iec_sys_h_
#define _iec_sys_h_

#include "core.h"
/// <summary>
/// 系统信息
/// </summary>
struct sys_info
{
	int communicate_role;	/*系统通信角色*/

	arraylist *sys_link_obj;	 /*当前链路*/
	arraylist *sys_app_obj;	/*当前APP*/

	rt_mailbox_t sys_event;	/*系统事件*/
};


extern struct sys_info  gSys_Info;

WEAK void iec_sys_create_app_complete(char *name,unsigned int appid);
WEAK void iec_sys_create_link_complete(char *name,unsigned int linkid);


extern void iec_sys_api_start_sys();
extern void iec_sys_api_create_link(char *link_name,int link_type,unsigned int link_addr,int link_addr_len,int dir);
extern void iec_sys_api_start_link(char *link_name);
extern void iec_sys_api_create_app(char *name,unsigned int asdu_addr,unsigned int asdu_addr_len,unsigned int cause_len,
                                   unsigned int node_addr_len,unsigned int sm2);
extern void iec_sys_api_start_app(char *app_name);
extern void iec_sys_api_app_bind_link(char *link_name,char *app_name);
extern void iec_sys_api_app_set_cmd_cb(char *app_name,int cb_idx,void *cb);
extern void iec_sys_api_send_phy_recv(char *name,char *buff,int len);
#endif
