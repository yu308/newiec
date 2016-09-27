
#ifndef _iec_event_h_
#define _iec_event_h_

#include "cmsis_os.h"
#include "iec_cfg.h"


/***************************ϵͳ���¼�����*****************************************/
#define		EVT_SYS_CREATE_LINK		(1)		/*������·���Ӳ�*/
#define		EVT_SYS_CREATE_APP		(2)		/*����APP��*/
#define		EVT_SYS_BIND_APP_LINK	(3)		/*��APP������·��*/
#define		EVT_SYS_START			(4)		/*ϵͳ��������*/
#define		EVT_SYS_DEL_APP			(5)		/*ɾ��һ��APP��*/
#define		EVT_SYS_DEL_LINK		(6)		/*ɾ��һ����·��*/
#define		EVT_SYS_EDIT_PROFILE	(7)		/*�޸�ĳ����������*/


/*******************************��·���¼�����***************************************/
#define		EVT_LINK_PHY_CONNECT			(0x11)		/*ͨ������ͨ������*/
#define		EVT_LINK_PHY_DISCONNECT			(0x12)		/*ͨ������ͨ���Ͽ�*/
#define		EVT_LINK_RECV_DATA				(0x13)		/*�յ���·����*/
#define		EVT_LINK_SEND_DATA				(0x14)		/*�跢����·����*/

/******************************APPϵͳӦ�ò��¼�����*****************************************/
#define		EVT_APP_ADD_NODE				(0x21)		/*�����Ϣ��*/
#define		EVT_APP_NODE_UPDATE				(0x22)		/*��Ϣ��仯*/
#define		EVT_APP_RECV_DATA				(0x23)		/*�յ�ASDU����*/
#define		EVT_APP_SEND_DATA				(0x24)		/*�跢��ASDU����*/

/******************************APP�û�Ӧ�ò��¼�����*************************************/
#define		EVT_APP_CTRL_OP					(0x31)		/*���Ʋ���*/
#define		EVT_APP_SET_OP					(0x32)		/*�����޸Ĳ���*/
#define		EVT_APP_READ_OP					(0x33)		/*��ȡ����*/
#define		EVT_APP_FILE_OP					(0x34)		/*�ļ�����*/

#if(CFG_RUNNING_MODE==SINGLE_MODE)
#define		SYS_OBJ				(0x0)		/*ʹ�õ�����ģʽʱ ϵͳ������*/
#endif

/// <summary>
/// �¼�
/// </summary>
struct iec_msg
{
	int sender;		/*�¼�������*/
	int recver;		/*�¼�������*/
	int mem_auto;	/*�Ƿ��Զ�����*/
	
	int evt_type;		/*�¼�����*/
	int *msg;			/*�¼�����*/
};


#endif