
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


struct normal_node_create_info
{
	unsigned int addr; /* ��Ϣ���ַ */
	unsigned int appid;/* ����appʵ��id */
};

struct seq_node_create_info
{
	unsigned int addr; /*��Ϣ������ʼ��ַ*/
	unsigned int appid;/*����appʵ��id*/
	unsigned int count;/*���л���Ϣ��������*/
};

struct normal_node_update_info
{
	unsigned int appid;
	unsigned int asdu_ident;
	unsigned int cause;

	int *val;
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
struct link_send_info
{
	char *app_data;
	char app_data_len;
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

union event_msg
{
	struct link_param m_link_param;
	struct link_send_info m_link_send_info;
	struct link_recv_info m_link_recv_info;


	struct app_param	m_app_param;
	struct app_recv_info	m_app_recv_info;


	struct normal_node_create_info m_nd_info;
	struct seq_node_create_info m_snd_info;
};

/// <summary>
/// �¼�
/// </summary>
struct iec_event
{
	int sender;		/*�¼�������*/
	int recver;		/*�¼�������*/
	int event_mem_auto;	/*event�Ƿ��Զ�����*/
	int msg_mem_auto;
	
	int evt_type;		/*�¼�����*/
	union event_msg *msg;			/*�¼�����*/
};


extern osStatus iec_post_event(osMessageQId q_id, struct iec_event *msg, int millisec);
extern struct iec_event *iec_recv_event(osMessageQId queue_id, int millisec);
extern void iec_init_event(struct iec_event *msg, int sender, int recver, int evt_type, int *data);
extern struct iec_event *iec_create_event(int sender, int recver, int evt_type, int *data,int data_auto);
extern void iec_free_event(struct iec_event *msg);


#endif