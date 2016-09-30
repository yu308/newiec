#ifndef _app_h_
#define _app_h_

#include "iec_cfg.h"
#include "element.h"
#include "asdu.h"


/// <summary>
/// ASDU������������
/// </summary>
struct asdu_task
{
	int asdu_ident;		/*ASDU��ʶ*/
	int seq_en;			/*����˳�����*/
	int node_count;		/*������Ϣ�����*/
	int cause;			/* ����ԭ��*/
	int node_addr[CFG_ASDU_NODE_MAX];		/*��Ϣ����*/
};

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

	struct asdu_task *first_task;	/*һ������ ���������ȼ���ߵ�����*/
	struct asdu_task *second_task;	/*�������� ��֮*/

	struct buffered_data *buffered;	

#if(CFG_RUNNING_MODE==MUTLI_MODE)
	osMessageQId app_event;
	osThreadId app_tid;
#endif
};

#define APP_FUN_FIRST		(4)
#define APP_FUN_SECOND		(5)
#define APP_FUN_USER		(6)
#endif
