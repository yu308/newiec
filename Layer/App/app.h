#ifndef _app_h_
#define _app_h_


#include "../Helper/layer_helper.h"




struct buffered_data
{
	int node_addr;		/*��Ϣ���ַ*/
	int asdu_ident;		/*ASDU��ʶ*/
	union node_val val;	/*��Ϣ��ֵ*/
	int utc_node_time;	/*��Ϣ����չʱ��*/
	int utc_time;		/*����ʱ��*/
};

/// <summary>
/// Ӧ��������Ϣ
/// </summary>
struct app_cfg
{
	int asdu_addr; /*ASDU��ַ*/	
	int asdu_addr_len;	/*ASDU��ַ����*/
	int cause_len;	/*����ԭ�򳤶�*/
	int node_addr_len;	/*��Ϣ���ַ����*/
	int sm2_enable;		/*SM2���ܹ���*/
};

/// <summary>
/// Ӧ����Ϣ
/// </summary>
struct app_info
{
	struct app_cfg *cfg;

	int linklayer_id[CFG_LINK_MAX];		/*�󶨵�link��·*/
 
#if(CFG_ROLE_MODE==SYS_ROLE_MIXED)
	int bro_applayer_id[CFG_APP_MAX];	/*��ת�����ģʽ�� ������APP*/
#endif

	arraylist *n_node_list;	/*��ͨ��Ϣ�����¼*/
	arraylist *s_node_list;/*���л���Ϣ�����¼*/
	

	arraylist *first_task;	/*һ������ ���������ȼ���ߵ�����*/
	arraylist *second_task;	/*�������� ��֮*/

	arraylist *buffered;

#if(CFG_RUNNING_MODE==MUTLI_MODE)
	osMessageQId app_event;
	osThreadId app_tid;
#endif
};

#define APP_FUN_FIRST		(4)
#define APP_FUN_SECOND		(5)
#define APP_FUN_USER		(6)
#endif
