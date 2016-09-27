#ifndef _iec_def_h_
#define _iec_def_h_

#include "iec_cfg.h"

enum Cause
{
	Defreason = 0,
	Cyc = 1,
	Back = 2,
	Spont = 3,
	Init = 4,
	Req = 5,
	Act = 6,
	Actcon = 7,
	Deact = 8,
	Deactcon = 9,
	Actterm = 10,
	Retrem = 11,
	Retloc = 12,
	CFile = 13,

	Introgen = 20,
	Inro1 = 21,
	Inro2 = 22,
	Inro3 = 23,
	Inro4 = 24,
	Inro5 = 25,
	Inro6 = 26,
	Inro7 = 27,
	Inro8 = 28,
	Inro9 = 29,
	Inro10 = 30,
	Inro11 = 31,
	Inro12 = 32,
	Inro13 = 33,
	Inro14 = 34,
	Inro15 = 35,
	Inro16 = 36,
	Reqcogen = 37,
	Reqco1 = 38,
	Reqco2 = 39,
	Reqco3 = 40,
	Reqco4 = 41,

	Unknowtype = 44,
	Unknowreason = 45,
	Unknowasdu = 46,
	Unknowinfo = 47,

};

/*
* ���ͱ�ʶ

typedef struct type_ident
{
	unsigned char ident;
}Type_Ident;*/
typedef unsigned char Type_Ident;

#pragma pack(1)
/*
* �ɱ�ṹ�޶���
*/
typedef struct var_struct_qual
{
	volatile unsigned char info_count : 7;
	volatile unsigned char SQ : 1;
}Var_Struct_Qual;
#pragma pack()

#define REASON_PN_DIS          0
#define REASON_T_DIS           0

#define REASON_PN_EN
#define REASON_T_EN
/*
* ����ԭ��
*/
/* typedef struct comm_reason */
/* { */
/* 	volatile unsigned char cause : 6; */
/* 	volatile unsigned char PN : 1; */
/* 	volatile unsigned char T : 1; */
/* #if CFG_REASON_SRC_ADDR */
/* 	unsigned char src_addr; */
/* #endif */
/* }Comm_Reason; */

typedef char Comm_Reason;
typedef char Src_Addr; /* ԭ����ַ */
/*
* Ӧ�÷������ݵ�Ԫ������ַASDU ADDR
*/
/* typedef struct asdu_addr */
/* { */
/* 	unsigned char addr1; */
/* #ifdef CFG_ASDU_ADDR_2 */
/* 	unsigned char addr2; */
/* #endif */
/* #ifdef CFG_ASDU_ADDR_3 */
/* 	unsigned char addr3; */
/* #endif */
/* }ASDU_Addr; */
typedef unsigned int ASDU_Addr;
/*
* ��Ϣ�����ַ
*/
/* typedef struct node_addr */
/* { */
/* 	unsigned char addr1; */
/* #ifdef CFG_NODE_ADDR_2 */
/* 	unsigned char addr2; */
/* #endif */
/* #ifdef CFG_NODE_ADDR_3 */
/* 	unsigned char addr3; */
/* #endif */

/* }Node_Addr; */

typedef unsigned int Node_Addr;

/***************END ֡���͡��޶��ʡ���ַ END*****************/



/*******************��ϢԪ��**********************/

#define CLOSE	1  		//��
#define CUT	0		//��

#define LOCK	1		//����
#define UNLOCK	0		//δ����

#define REPLACE	1		//ȡ��
#define NOT_REPLACE	0	//δȡ��

#define CURRENT	0		//��ǰֵ
#define NOT_CURRENT	1	//�ǵ�ǰֵ

#define VALID	0		//��Ч
#define INVALID	1		//��Ч

/*
* ��Ʒ�������ʵĵ�����Ϣ
*/
typedef struct info_element_SIQ
{
	volatile unsigned char SPI : 1;
	volatile unsigned char RES : 3;
	volatile unsigned char BL : 1;
	volatile unsigned char SB : 1;
	volatile unsigned char NT : 1;
	volatile unsigned char IV : 1;
}Info_E_SIQ;


#define NOT_CONFIRM	3	//��ȷ��
#define CONFIRM_CLOSE	2	//ȷ��״̬��
#define CONFIRM_CUT	1	//ȷ��״̬��
#define MIDDLE		0	//�м�״̬
/*
* ��Ʒ�������ʵ�˫����Ϣ
*/
typedef struct info_element_DIQ
{
	volatile unsigned char DPI : 2;
	volatile unsigned char RES : 2;
	volatile unsigned char BL : 1;
	volatile unsigned char SB : 1;
	volatile unsigned char NT : 1;
	volatile unsigned char IV : 1;
}Info_E_DIQ;



#define OVER	1	//���
#define NOT_OVER 0	//δ���

/*
* Ʒ��������
*/

#define YES	1
#define NO	0

#define QDS_Bit_Val(val)	(val?YES:NO)

typedef struct info_element_QDS
{
	volatile unsigned char OV : 1;
	volatile unsigned char RES : 3;
	volatile unsigned char BL : 1;
	volatile unsigned char SB : 1;
	volatile unsigned char NT : 1;
	volatile unsigned char IV : 1;
}Info_E_QDS;


#define TM_VALID	0	//����ʱ����Ч
#define TM_INVALID	1	//����ʱ����Ч
/*
* �̵籣���豸�¼���Ʒ��������
*/
typedef struct info_element_QDP
{
	volatile unsigned char RES : 3;
	volatile unsigned char EI : 1;
	volatile unsigned char BL : 1;
	volatile unsigned char SB : 1;
	volatile unsigned char NT : 1;
	volatile unsigned char IV : 1;
}Info_E_QDP;


#define DEV_NOT_CHANGE	0	//�豸δ��˲��״̬
#define DEV_CHANGE	1	//�豸����˲��״̬
/*
* ��˲��״ָ̬ʾ��ֵ
*/
typedef struct info_element_VTI
{
	volatile unsigned char VAL : 7;
	volatile unsigned char TI : 1;
}Info_E_VTI;


/*
* ��һ��ֵ
* ת��Ϊ������1����  һ��Ϊ ����ֵ/ȡֵ��Χ���ֵ
*/
typedef struct info_element_NVA
{
	short VAL;
}Info_E_NVA;

/*
* ��Ȼ�ֵ
*/
typedef struct info_element_SVA
{
	short VAL;
}Info_E_SVA;

/*
* 754��ʽ�Ķ̸�����
*/
typedef float Info_E_STD;


#define C_OVER		1	//�ۼ������ڼ������
#define C_NOT_OVER	0

#define C_JUDGE		1	//��������
#define C_NOT_JUDGE	0
/*
* �����Ƽ���������
*/
typedef struct info_element_BCR
{
	int Counter;
	volatile unsigned char SQ : 5;
	volatile unsigned char CY : 1;
	volatile unsigned char CA : 1;
	volatile unsigned char IV : 1;
}Info_E_BCR;

/*
* �̵籣���豸�����¼�
*/
typedef struct info_element_SEP
{
	volatile unsigned char ES : 2;
	volatile unsigned char RES : 1;
	volatile unsigned char EI : 1;
	volatile unsigned char BL : 1;
	volatile unsigned char SB : 1;
	volatile unsigned char NT : 1;
	volatile unsigned char IV : 1;

}Info_E_SEP;


#define T_START		1	//������
#define T_NOT_START	0

#define A_PROTECT	1	//A �� ����
#define A_NOT_PROTECT	0

#define B_PROTECT	1	//B �� ����
#define B_NOT_PROTECT	0

#define C_PROTECT	1	//C �� ����
#define C_NOT_PROTECT	0

#define GROUND_PROTECT	1	//�ӵ� ����
#define GROUND_NOT_PROTECT	0

#define BACK_PROTECT	1	//���� ����
#define BACK_NOT_PROTECT	0
/*
* �̵籣���豸�����¼�
*/
typedef struct info_element_SPE
{
	volatile unsigned char GS : 1;
	volatile unsigned char SL1 : 1;
	volatile unsigned char SL2 : 1;
	volatile unsigned char SL3 : 1;
	volatile unsigned char SIE : 1;
	volatile unsigned char SRD : 1;
	volatile unsigned char RES : 2;
}Info_E_SPE;

#define T_CMDOUT	1  //��������������·
#define T_NOT_CMDOUT	0

#define A_CMDOUT	1  //���������A�����·
#define A_NOT_CMDOUT	0

#define B_CMDOUT	1  //���������B�����·
#define B_NOT_CMDOUT	0

#define C_CMDOUT	1  //���������C�����·
#define C_NOT_CMDOUT	0
/*
* �̵籣���豸�����·��Ϣ
*/
typedef struct info_element_OCI
{
	volatile unsigned char GC : 1;
	volatile unsigned char CL1 : 1;
	volatile unsigned char CL2 : 1;
	volatile unsigned char CL3 : 1;
	volatile unsigned char RES : 4;
}Info_E_OCI;

/*
* ������״̬��Ϣ
*/
typedef struct info_element_BSI
{
	unsigned int VAL;
}Info_E_BSI;

/*
* �̶������� �̶�ֵ Ϊ0x55AA
*/
typedef struct info_element_FBP
{
	unsigned short VAL;
}Info_E_FBP;

#define NOT_OTHER_DEF	0	//����������
#define SHORT_SIGNLE	1	//������
#define LONG_SIGNLE	2	//������
#define CON_SIGNLE	3	//��������

#define CHOOSE         1     //ѡ��
#define EXECUTE        0     //ִ��
/*
* �����޶���
*/
typedef struct info_element_QOC
{
	volatile unsigned char QU : 5;
	volatile unsigned char SE : 1;
}Info_E_QOC;

#define NOT_ALLOW	0	//������
/*
* ��������
*/
typedef struct info_element_SCO
{
	volatile unsigned char SCS : 1;
	volatile unsigned char RES : 1;
	volatile unsigned char QU : 5;
	volatile unsigned char SE : 1;
}Info_E_SCO;

/*
* ˫����
*/
typedef struct info_element_DCO
{
	volatile unsigned char DCS : 2;
	volatile unsigned char QU : 5;
	volatile unsigned char SE : 1;

}Info_E_DCO;

#define UP_STEP		2	//��һ��
#define DOWN_STEP	1	//��һ��
/*
* ����������
*/
typedef struct info_element_RCO
{
	volatile unsigned char RCS : 2;
	volatile unsigned char QU : 5;
	volatile unsigned char SE : 1;
}Info_E_RCO;

/*
* ������ʱ��-7�ֽڳ�
*/
typedef struct info_element_CP56Tm2a
{
	unsigned short M_SEC;
	volatile unsigned char MIN : 6;
#if CFG_INFO_TM_REPL
	volatile unsigned char GEN : 1;
#else
	volatile unsigned char RES1 : 1;
#endif
	volatile unsigned char IV : 1;
	volatile unsigned char HOUR : 5;
	volatile unsigned char RES2 : 1;
	volatile unsigned char SU : 1;
	volatile unsigned char DOM : 5;
	volatile unsigned char DOW : 3;
	volatile unsigned char MONTH : 4;
	volatile unsigned char RES3 : 4;
	volatile unsigned char YEAR : 7;
	volatile unsigned char RES4 : 1;

}Info_E_TM56;

/*
* ������ʱ��-3�ֽڳ�
*/
typedef struct info_element_CP24Tm2a
{
	unsigned short M_SEC;
	volatile unsigned char MIN : 6;
#if CFG_INFO_TM_REPL
	volatile unsigned char GEN : 1;
#else
	volatile unsigned char RES1 : 1;
#endif
	volatile unsigned char IV : 1;
}Info_E_TM24;

/*
* ������ʱ��-3�ֽڳ�
*/
typedef struct info_element_CP16Tm2a
{
	unsigned short M_SEC;
}Info_E_TM16;


#define CURRENT_CLOSE_POWER	0	//���ص�Դ����
#define CURRENT_HAND_RESET	1	//�����ֶ���λ
#define REMOTE_RESET		2	//Զ����λ

#define PARAM_CHANGE	1		//�����ı�ĳ�ʼ��
#define PARAM_NOT_CHANGE	0

/*
* ��ʼ��ԭ��
*/
typedef struct info_element_COI
{
	volatile unsigned char REASON : 7;
	volatile unsigned char CHANGED : 1;
}Info_E_COI;

#define GOL_CALL	20 //ȫ���ٻ�

/*
* �ٻ��޶���
*/
typedef unsigned char Info_E_QOI;

#define GROUP_REQ_1	1	//�����������1��
#define GROUP_REQ_2	2
#define GROUP_REQ_3	3
#define GROUP_REQ_4	4
#define GROUP_REQ_G	5	//�ܵ����������

#define NOT_RESET	0
#define REQ_FREEZE	1	//������� ����λ
#define REQ_Z_RESET	2	//�������λ
#define REQ_RESET	3	//��������λ


/*
* �������ٻ������޶���
*/
typedef struct info_element_QCC
{
	volatile unsigned char RQT : 6;
	volatile unsigned char FRZ : 2;
}Info_E_QCC;

#define LIMIT		1	//����ֵ
#define FILTER_FACTOR	2	//ƽ��ϵ�� �˲�ϵ��
#define MV_UP_LIMIT	4	//����ֵ����
#define MV_DW_LIMIT	3	//����ֵ����

#define MV_PARAM_CHANGE	1	//���ز����ı�
#define MV_PARAM_NOT_CHANGE	0

#define MV_PARAM_RUN	0	//��������
#define MV_PARAM_NOT_RUN	1
/*
* ����ֵ�����޶���
*/
typedef struct info_element_QPM
{
	volatile unsigned char KPA : 6;
	volatile unsigned char LPC : 1;
	volatile unsigned char POP : 1;
}Info_E_QPM;

#define ENABLE_LAST_LOAD	1	//�����ֹ֮ͣǰװ�صĲ���
#define ENABLE_INFO_PARAM	2	//�����ֹͣѰַ����Ϣ����Ĳ���
#define ENABLE_LOOP_PARAM	3	//�����ֹͣѰַ�ĳ���ѭ�������ڴ������Ϣ����
/*���������޶���*/
typedef unsigned char Info_E_QPA;


#define TASK_RESET	1	//���̵��ܸ�λ
#define RESET_EVENT_INFO_TM	2	//��λ�¼�������������Ĵ�ʱ����Ϣ
/*��λ���������޶���*/
typedef unsigned char Info_E_QRP;

#define FILE_OP_CONFIRM_Y	0	//�ļ���ز����Ŀ϶�ȷ��
#define FILE_OP_CONFIRM_N	1	//��ȷ��
/*
* �ļ�׼�������޶���
*/
typedef struct info_element__FRQ
{
	volatile unsigned char IDENT : 7;
	volatile unsigned char OPERATE : 1;

}Info_E_FRQ;

#define SEC_READY	0	//��׼������
#define SEC_NOT_READY	1
/*
* ��׼�������޶���
*/
typedef struct info_element_SRQ
{
	volatile unsigned char IDENT : 7;
	volatile unsigned char READY : 1;
}Info_E_SRQ;

#define OP_FILE_SEL	1			//ѡ���ļ�
#define OP_FILE_REQ	2			//�����ļ�
#define OP_FILE_ACT_STOP	3		//ֹͣ�ļ�����
#define OP_FILE_DEL	4			//ɾ���ļ�
#define OP_SEC_SEL	5			//ѡ���
#define OP_SEC_REQ	6			//�����
#define OP_SEC_ACT_STOP		7		//ֹͣ�ڲ���

#define ST_NOT_REQ_MEM	1			//������Ĵ洢�ռ�
#define ST_CHECK_ERR	2			//У��ʹ�
#define ST_NOT_SERVICE	3			//��������ͨ�ŷ���
#define ST_NOT_FILE_NAME	4		//���������ļ���
#define ST_NOT_SEC_NAME	5			//�������Ľ���
/*
* ѡ����ٻ��޶���
*/
typedef struct info_element_SCQ
{
	volatile unsigned char SELECT : 4;
	volatile unsigned char STAT : 4;
}Info_E_SCQ;


#define TRS_F_NOT_STOP_CONFIRM		1	//����ֹͣȷ�ϵ��ļ�����
#define TRS_F_STOP_CONFIRM		2	//��ֹͣȷ�ϵ��ļ�����
#define TRS_S_NOT_STOP_CONFIRM		3	//����ֹͣȷ�ϵĽڴ���
#define TRS_S_STOP_CONFIRM		4	//��ֹͣȷ�ϵĽڴ���
/*�����ֺͶε��޶���*/
typedef unsigned char Info_E_LSQ;



#define TRS_F_CONFIRM_Y	1	//�ļ�����Ŀ϶��Ͽ�
#define TRS_F_CONFIRM_N	2	//�ļ�����ķ��Ͽ�
#define TRS_S_CONFIRM_Y	3	//�ڴ���Ŀ϶��Ͽ�
#define TRS_S_CONFIRM_N	4	//�ڴ���ķ��Ͽ�
/*
* �ļ��Ͽɻ���Ͽ��޶���
*/
typedef struct info_element_AFQ
{
	volatile unsigned char ACCEPT : 4;
	volatile unsigned char STAT : 4;
}Info_E_AFQ;

/*�ļ�����*/
typedef unsigned short Info_E_NOF;

/*�ڵ�����*/
typedef unsigned char Info_E_NOS;

/*�ļ���ڵ�ĳ���*/
typedef struct info_element_LOF
{
	unsigned char VAL[3];
}Info_E_LOF;

/*�εĳ���*/
typedef unsigned char Info_E_LOS;

/*У���*/
typedef unsigned char Info_E_CHS;

#define LAST_DIR	1	//���Ŀ¼�ļ�
#define DIR		0	//���滹��Ŀ¼�ļ�

#define FILE_DEF	0 	//�����ļ���
#define CHILD_DIR_DEF	1	//������Ŀ¼��

#define TRS_WAIT	0	//�ļ��ȴ�����
#define TRS_ACT		1	//�ļ������Ѿ�����
/*
* �ļ���״̬
*/
typedef struct info_element_SOF
{
	volatile unsigned char STATUS : 5;
	volatile unsigned char LFD : 1;
	volatile unsigned char FOR : 1;
	volatile unsigned char FA : 1;
}Info_E_SOF;

#define SELECT	1	//ѡ��
#define ACT	0	//ִ��
/*
* �趨�����޶���
*/
typedef struct info_element_QOS
{
	volatile unsigned char QL : 7;
	volatile unsigned char SE : 1;
}Info_E_QOS;

/*
* ״̬��״̬��λ���
*/
typedef struct info_element_SCD
{
	unsigned short ST;
	unsigned short CD;
}Info_E_SCD;


/*��ϢԪ������*/
enum Element_Ident
{
	SIQ = 1,
	DIQ,
	QDS,
	QDP,
	VTI,
	NVA,
	SVA,
	STD,
	BCR,
	SEP,
	SPE,
	OCI,
	BSI,
	FBP,
	SCO,
	DCO,
	RCO,
	TM56,
	TM24,
	TM16,
	COI,
	QOI,
	QCC,
	QPM,
	QPA,
	QOC,
	QRP,
	FRQ,
	SRQ,
	SCQ,
	LSQ,
	AFQ,
	NOF,
	NOS,
	LOF,
	LOS,
	CHS,
	SOF,
	QOS,
	SCD,
};

/************** END ��ϢԪ��  END*********************/


#endif
