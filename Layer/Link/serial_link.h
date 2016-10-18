#ifndef _link_h_
#define _link_h_

#include "iec_cfg.h"

typedef void(*serial_write_handle)(char *buff, int len);
/// <summary>
/// ���з�ʽ--��·����
/// </summary>
struct serial_link_cfg
{
	int link_addr;			/*��·��ַ*/
	int link_addr_len;		/*��·��ַ����*/
  int active;           /*�Ƿ񼤻�*/
	int double_dir;			/*ƽ����ƽ��*/
		
	char *recv_buff;		/*���ջ���*/
	char *send_buff;		/*���ͻ���*/
	char *prev_sent_buff;	/*��һ�˷������ݻ���*/
	int	prev_sent_len;		/*���ͳ���*/

	serial_write_handle	serial_write;	/*���ͽӿ�*/
};

/// <summary>
/// ���з�ʽ--��·������Ϣ
/// </summary>
struct serial_link_info
{
	struct serial_link_cfg *cfg;

	int acd_tag;
	int app_tag;
	int fcb;
	int applayer_id;

#if(CFG_RUNNING_MODE==MUTLI_MODE)
	osThreadId	serial_tid;
	osMessageQId serial_event;
#endif
};


/**************��·�ַ������жϽ��******************/
#define NO_AWS				(0)			/*�޻ش�*/
#define INVAILD_FCB			(1)			/*��ЧFCB����*/
#define TO_LINK				(2)			/*��LINK*/
#define TO_LINK_REQ				(3)			/*��LINK��·״̬*/
#define TO_APP_FIRST				(4)			/*��APP*/
#define TO_APP_SECOND		(5)
#define TO_APP_USER			(6)

/// <summary>
/// ���緽ʽ--��·����
/// </summary>
struct net_link_cfg
{
	int ip[4];
	int role;
	
	int link_socket[CFG_SOCKET_MAX];
};

#endif
