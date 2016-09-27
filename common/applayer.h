#ifndef _applayer_h_
#define _applayer_h_

#include "iec_cfg.h"


union node_val
{
	int int_val;		/*����*/
	float float_val;	/*����*/
	char iarray_val[4];		/*�ַ�*/
	volatile unsigned bit_val : 32;	/*λ��*/
};

/// <summary>
/// ��Ϣ����Ϣ
/// </summary>
struct node
{
	int addr;	/* ��Ϣ���ַ */
	union node_val val; /*��Ϣ��ֵ*/
	int changed;		/*�Ƿ��иı�*/
	int serial_cmp;		/*���ڷ�ʽ������ɱ�־ ��δ������Ϊ-1 */
	int	net_cmp;		/*���緽ʽ������ɱ�־ δ����Ϊ-1 */
	
	int buffered;		/*�Ƿ񻺴�����*/
	int asdu_ident;		/*��������ʱ����ASDU����*/
};

/// <summary>
/// ASDU������Ϣ ���ڽ������װ��Ϣ������
/// </summary>
struct asdu_cfg
{	
	int val_ident;	/* ��Ϣ��ֵ���ͱ�ʶ*/
	int qual_ident;	/* ��Ϣ��Ʒ���������ͱ�ʶ*/
	int tm_ident;	/* ��Ϣ��ʱ���ǩ��ʶ*/
	int ext_tm_ident; /*��Ϣ����չʱ���ǩ��ʶ ���ڼ̵籣���¼���Ϣ��*/
};

/// <summary>
/// �������� 
/// </summary>
struct buffered_data
{
	int node_addr;		/*��Ϣ���ַ*/
	int asdu_ident;		/*ASDU��ʶ*/
	union node_val val;	/*��Ϣ��ֵ*/
	int time[7];		/*����ʱ��*/
};

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

extern struct asdu_task First_task[CFG_FIRST_FRAME_MAX];
extern struct asdu_task Second_task[CFG_SECOND_FRAME_MAX];

#endif
