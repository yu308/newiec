#ifndef _iec_def_h_
#define _iec_def_h_

/****************************************LINK Layer***********************************/

#define FIXED_FRAME		(1)			/*֡���� �̶�����֡*/		
#define UNFIXED_FRAME	(2)			/*֡���� �ǹ̶�����֡*/

#define UNFIXED_HEAD_TAG	(0x68)		/*�ǹ̶�֡ͷ*/
#define FIXED_HEAD_TAG		(0x10)		/*�̶�֡ͷ*/
#define FRAME_END_TAG		0x16		/*֡��β��ʶ*/

#define CTRL_DOMAIN_LEN		(1)			/*�����򳤶�*/

/**********************������****************/
//�ϴ�������
#define FC_UP_YES	0	/*�϶��Ͽ�*/
#define FC_UP_NO	1	/*���Ͽ�*/
#define FC_UP_USER_DATA	8	/*�û�����*/
#define FC_UP_NO_DATA	9	/*������*/
#define FC_UP_LINK	11	/*��Ӧ��·״̬*/

//���͹�����
#define FC_DW_RST_REMOTE	0	/**< Զ����λ */
#define FC_DW_RST_USER		1	/**< ��λ�û� */
#define FC_DW_DATA_YES		3	/**< ȷ��/�����û����� */
#define FC_DW_DATA_NO		4	/**< ���û����� */
#define FC_DW_REQ		8	/**< �������� */
#define FC_DW_LINK		9   /**< ������·״̬ */
#define FC_DW_FIRST_DATA	10	/**< һ������ */
#define FC_DW_SEC_DATA		11	/**< �������� */

struct up_control_domain
{
	volatile unsigned char FunCoed : 4;
	volatile unsigned char DFC : 1;
	volatile unsigned char ACD : 1;
	volatile unsigned char PRM : 1;
	volatile unsigned char RES : 1;
};

struct down_control_domain
{
	volatile unsigned char FunCoed : 4;
	volatile unsigned char FCV : 1;
	volatile unsigned char FCB : 1;
	volatile unsigned char PRM : 1;
	volatile unsigned char RES : 1;
};

union ctrl_domain
{
	struct up_control_domain up_ctrl_domain;
	struct down_control_domain dw_ctrl_domain;
	char domain;
};

/*********************************************************************/



#endif