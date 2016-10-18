
#ifndef _iec_event_h_
#define _iec_event_h_

#include "../OS/cmsis_os.h"
#include "iec_cfg.h"
#include "iec_node.h"


/***************************ϵͳ���¼�����*****************************************/
#define		EVT_SYS_CREATE_LINK		(1)		/*������·���Ӳ�  MSG--��·��ַ,��·��ַ����*/
#define		EVT_SYS_CREATE_APP		(2)		/*����APP�� MSG--ASDU��ַ,ASDU��ַ����,����ԭ�򳤶�,����SM2,��Ϣ���ַ����*/
#define		EVT_SYS_BIND_APP_LINK	(3)		/*��APP������·�� MSG---��·ģ��ID,APPģ��ID*/
#define		EVT_SYS_START			(4)		/*ϵͳ�������� MSG--����*/
#define		EVT_SYS_DEL_APP			(5)		/*ɾ��һ��APP�� MSG---APPģ��ID*/
#define		EVT_SYS_DEL_LINK		(6)		/*ɾ��һ����·�� MSG---LINKģ��ID*/
#define		EVT_SYS_EDIT_PROFILE	(7)		/*�޸�ĳ����������*/


/*******************************��·���¼�����***************************************/
#define		EVT_LINK_PHY_CONNECT			(0x11)		/*ͨ������ͨ������*/
#define		EVT_LINK_PHY_DISCONNECT			(0x12)		/*ͨ������ͨ���Ͽ�*/
#define		EVT_LINK_RECV_DATA				(0x13)		/*��·�յ�����*/
#define		EVT_LINK_SEND_DATA				(0x14)		/*��·�跢������*/


#define		EVT_SUB_DAT_LINK_PHY			(0) /*�ӿ��շ�����*/
#define		EVT_SUB_DAT_LEVEL_1		(1) /*һ������*/
#define		EVT_SUB_DAT_LEVEL_2		(2) /*��������*/
#define		EVT_SUB_DAT_USER		(3) /*�û�����*/

/******************************APPϵͳӦ�ò��¼�����*****************************************/
#define		EVT_APP_ADD_NODE				(0x21)		/*�����Ϣ�� MSG--NODE��ַ,NODE���� SUBMSG--SEQ����*/
#define		EVT_APP_NODE_UPDATE				(0x22)		/*��Ϣ��仯 MSG--ASDU IDENT,����ԭ�� SUB MSG--NODE��ַ,NODEֵ,Ʒ������,UTCʱ��,�Ƿ񻺴�*/
#define		EVT_APP_RECV_DATA				(0x23)		/*APP�յ�����  MSG--ASDU ���ݴ������ݴ�����*/
#define		EVT_APP_SEND_DATA				(0x24)		/*�跢������ MSG---ASDU ���ݴ������ݴ�����*/


#define		EVT_SUB_NORMAL_NODE		(1) /*ͨ����Ϣ���¼�*/ 
#define		EVT_SUB_SEQ_NODE		(2) /*���л���Ϣ���¼�*/




/******************************APP�û�Ӧ�ò��¼�����*************************************/
#define		EVT_APP_CTRL_OP					(0x31)		/*���Ʋ���*/
#define		EVT_APP_SET_OP					(0x32)		/*�����޸Ĳ���*/
#define		EVT_APP_READ_OP					(0x33)		/*��ȡ����*/
#define		EVT_APP_FILE_OP					(0x34)		/*�ļ�����*/

#if(CFG_RUNNING_MODE==SINGLE_MODE)
#define		SYS_OBJ				(0x0)		/*ʹ�õ�����ģʽʱ ϵͳ������*/
#endif



/// <summary>
/// ����ͨ����Ϣ��ʱ��Ϣ
/// </summary>
struct node_update_info
{
	unsigned int appid; /*����APP*/
	unsigned int level; /*ˢ�����ȼ�*/
	unsigned int asdu_ident;/*asdu��ʶ*/
	unsigned int cause; /*����ԭ��*/
};


/// <summary>
/// ����Linkʱ����
/// </summary>
struct link_param
{
	unsigned char link_addr;
	unsigned char link_addr_len;
	unsigned char link_dir;
};

/// <summary>
/// Linkʵ���յ���������Ϣ
/// </summary>
struct link_recv_info
{
	char *recv_data;
	int recv_len;
};

/// <summary>
/// LINKʵ���յ�APPʵ���跢��������Ϣ
/// </summary>
struct app_send_info
{
	char *app_data;
	int app_data_len;
};

/// <summary>
/// ����APPʵ��ʱ�Ĳ���
/// </summary>
struct app_param
{
	unsigned char asdu_addr;
	unsigned char asdu_addr_len;
	unsigned char cause_len;
	unsigned char node_addr_len;
	unsigned char sm2_enable;
};


/// <summary>
/// APPʵ���յ�LINKʵ����������Ϣ
/// </summary>
struct app_recv_info
{
	char funcode;
	char *app_data;
};



/// <summary>
/// �¼�
/// </summary>
struct iec_event
{
	int sender;		/*�¼�������*/
	int recver;		/*�¼�������*/
	int main_msg_free;	/*�¼�����Ϣ�Ƿ��Զ�����*/
	int sub_msg_free;		/*�¼�����Ϣ�Ƿ��Զ�����*/
	
	int evt_type;		/*�¼�����*/
	int evt_sub_type;   /*�¼�ϸ��*/
	void *main_msg;			/*���¼�����*/
	void *sub_msg			/*���¼�����*/
};


extern osStatus iec_post_event(osMessageQId q_id, struct iec_event *msg, int millisec);
extern struct iec_event *iec_recv_event(osMessageQId queue_id, int millisec);
extern struct iec_event *iec_create_event(int sender, int recver, int evt_type,int *main_msg, int auto_free);
extern void iec_free_event(struct iec_event *evt);

void iec_set_event_sub(struct iec_event *evt, int evt_sub_type, int *sub_msg, int auto_free);


#endif
