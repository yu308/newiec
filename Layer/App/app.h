#ifndef _app_h_
#define _app_h_

#include "../../OS/os_helper.h"
#include "../Helper/layer_helper.h"

typedef int (*sys_cmd_proc)(unsigned int asdu_ident,char* node_data,unsigned int len);
typedef int (*ctrl_cmd_proc)(unsigned int asdu_ident,unsigned int node_addr,char* node_data,unsigned int len);
typedef int (*file_cmd_proc)();
typedef int (*param_cmd_proc)();


struct buffered_data
{
	int node_addr;		/*ÐÅÏ¢µãµØÖ·*/
	int asdu_ident;		/*ASDU±êÊ¶*/
	int utc_node_time;	/*ÐÅÏ¢µãÀ©Õ¹Ê±¼ä*/
	int utc_time;		/*·¢ÉúÊ±¼ä*/
};

/// <summary>
/// Ó¦ÓÃÅäÖÃÐÅÏ¢
/// </summary>
struct app_cfg
{       
  char name[CFG_NAME_MAX];
	int asdu_addr; /*ASDUµØÖ·*/	
	int asdu_addr_len;	/*ASDUµØÖ·³¤¶È*/
	int cause_len;	/*´«ËÍÔ­Òò³¤¶È*/
	int node_addr_len;	/*ÐÅÏ¢µãµØÖ·³¤¶È*/
	int sm2_enable;		/*SM2¼ÓÃÜ¹¦ÄÜ*/
};

/// <summary>
/// Ó¦ÓÃÐÅÏ¢
/// </summary>
struct app_info
{
	struct app_cfg cfg;

	unsigned int linklayer_id[CFG_LINK_MAX];		/*°ó¶¨µÄlinkÁ´Â·*/
 
#if(CFG_ROLE_MODE==SYS_ROLE_MIXED)
	int bro_applayer_id[CFG_APP_MAX];	/*ÖÐ×ª»ú»ìºÏÄ£Ê½ÏÂ ¹ØÁªµÄAPP*/
#endif

	arraylist *first_task;	/*Ò»¼¶Êý¾Ý ¼´´«ÊäÓÅÏÈ¼¶×î¸ßµÄÊý¾Ý*/
	arraylist *second_task;	/*¶þ¼¶Êý¾Ý ´ÎÖ®*/

	arraylist *buffered;

  sys_cmd_proc  sys_cmd_cb;
  ctrl_cmd_proc ctrl_cmd_cb;
  file_cmd_proc  file_cmd_cb;
  param_cmd_proc  param_cmd_cb;

#if(CFG_RUNNING_MODE==MUTLI_MODE)
	rt_mailbox_t app_event;
	rt_thread_t app_tid;
#endif
};

#define APP_FUN_FIRST		(4)
#define APP_FUN_SECOND		(5)
#define APP_FUN_USER		(6)
#endif
