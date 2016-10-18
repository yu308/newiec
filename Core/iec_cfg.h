#ifndef _iec_cfg_h_
#define _iec_cfg_h_

#include	"cmsis_os.h"

/************************************************
��������˵��
	ȫ�ֱ���	      gXxx_Yyy					g��ͷ ����ĸ��д  ����_����
	��            MMMM						ȫ��д
	��̬����	      gxxx_yyy					g��ͷ ȫСд    ����_����
	����			 level_action_object		level ��  action ����  object ����
	����			 xxxxx						ȫСд
**************************************************/

/********************* Memory *********************/
#ifndef EXTMALLOC
#define XMALLOC  malloc
#define XFREE    free
#define XREALLOC realloc
#define XCALLOC  calloc
#define XPRINTF	 printf
#define XMEMSET	 memset
#define XMEMCPY	 memcpy
#else
extern void *XMALLOC(size_t n);
extern void *XREALLOC(void *p, size_t n);
extern void *XCALLOC(size_t n, size_t s);
extern void XFREE(void *p);
extern void XPRINTF();
#endif // !EXTMALLOC
/************************************************/


/********************���й���ģʽ����***************************/
#define SINGLE_MODE			(1)			/*������ģʽ*/
#define MUTLI_MODE			(2)			/*������ģʽ*/

#define CFG_RUNNING_MODE		MUTLI_MODE

#if(CFG_RUNNING_MODE==MUTLI_MODE)
#define SYS_TASK_PROI			osPriorityNormal
#define	APP_TASK_PROI			osPriorityAboveNormal
#define	LINK_TASK_PROI			osPriorityAboveNormal
#elif(CFG_RUNNING_MODE==SINGLE_MODE)
#define SYS_TASK_PROI			osPriorityNormal
#endif


#define SYS_ROLE_CLIENT			(1<<0)		/*��֧�ֽ�ɫΪ�ͻ��� ���緽ʽʱ��·��Ϊsocket�������� */
#define SYS_ROLE_SERVER			(1<<1)		/*��֧�ֽ�ɫΪ�����	���緽ʽ��·��Ϊsocket�ͻ���*/
#define SYS_ROLE_MIXED			(1<<2)		/*����֧��*/

#define CFG_ROLE_MODE			SYS_ROLE_CLIENT


/*
-�ͻ���ģʽ
	--������ͨ�ŷ�ʽ
		LINK ����ʵ�ʴ��нӿ�����M1����
		APP	 1
	--������ͨ�ŷ�ʽ
		LINK ����ʵ������ӿ�����M2����
		APP   1
	--˫ͨ��֧��
		LINK �ӿ�������M1+M2
		APP	 1
	��Ӧ��ϵ: 1������APPֻ��Ӧ����LINKͨ��  2������APP��ͬʱ��Ӧ���LINKͨ��

-�����ģʽ
	--������ͨ�ŷ�ʽ
		LINK ����ʵ�ʴ��нӿ�����M1����
		APP	 >=1 <=M1
	--������ͨ�ŷ�ʽ
		LINK ����ʵ������ӿ�����M2����
		APP   >=1 <=M2
	--˫ͨ��֧��
		LINK �ӿ�������M1+M2
		APP	 >=2 <=M1+M2
	��Ӧ��ϵ: 1������APPֻ��Ӧ����LINKͨ��  2������APP��ͬʱ��Ӧ���LINKͨ��

-���ģʽ
	�ͻ�����������˹��沢�н������ݣ���������ת����APP��Ϊ�������ͻ���2�֡�ÿ��LINKͨ��ֻ�ɶ�Ӧһ��APP��APP����ͬ
	��Ӧ��ϵ: 1�����ߵ���APPֻ��Ӧ����LINKͨ��  2�����ߵ���APP��ͬʱ��Ӧ���LINKͨ�� 3����ͬ��2��APP֮��ɽ������ݽ���

����,APP��LINK֮����1��2���ֹ�ϵ,����Ϊ������ת��ʱ������3


*/

#define CFG_LINK_MAX			(4)		/*֧�ֵ���·���� */
#define CFG_APP_MAX				(1)		/*APP���� */
#define	CFG_SOCKET_MAX			(5)		/*����ģʽ��SOCKET���������*/


#define	CFG_ASDU_DATA_BUFF_MAX       (200)		/*ASDU������Ϣ����*/


/******************************ASDU �������******************************/

#define CFG_ASDU_M_SP_NA			(1)
#define CFG_ASDU_M_SP_NA			(1)
#define CFG_ASDU_M_SP_TA			(1)

#define CFG_ASDU_M_DP_NA			(1)
#define CFG_ASDU_M_DP_TA			(1)

#define CFG_ASDU_M_ST_NA			(1)
#define CFG_ASDU_M_ST_TA			(1)

#define CFG_ASDU_M_BO_NA			(1)
#define CFG_ASDU_M_BO_TA   (1)

#define CFG_ASDU_M_ME_NA   (1)
#define CFG_ASDU_M_ME_TA   (1)
#define CFG_ASDU_M_ME_NB   (1)
#define CFG_ASDU_M_ME_TB   (1)
#define CFG_ASDU_M_ME_NC   (1)
#define CFG_ASDU_M_ME_TC   (1)

#define CFG_ASDU_M_IT_NA   (1)
#define CFG_ASDU_M_IT_TA   (1)

#define CFG_ASDU_M_EP_TA   (1)
#define CFG_ASDU_M_EP_TB   (1)

#define CFG_ASDU_M_EP_TC   (1)
#define CFG_ASDU_M_PS_NA   (1)
#define CFG_ASDU_M_ME_ND   (1)

#define CFG_ASDU_M_SP_TB   (1)
#define CFG_ASDU_M_DP_TB   (1)
#define CFG_ASDU_M_ST_TB   (1)
#define CFG_ASDU_M_BO_TB   (1)
#define CFG_ASDU_M_ME_TD   (1)
#define CFG_ASDU_M_ME_TE   (1)
#define CFG_ASDU_M_ME_TF   (1)
#define CFG_ASDU_M_IT_TB   (1)
#define CFG_ASDU_M_EP_TD   (1)
#define CFG_ASDU_M_EP_TE   (1)
#define CFG_ASDU_M_EP_TF   (1)

#define CFG_ASDU_C_SC_NA    (1)
#define CFG_ASDU_C_DC_NA   (1)
#define CFG_ASDU_C_RC_NA   (1)
#define CFG_ASDU_C_SE_NA   (1)
#define CFG_ASDU_C_SE_NB   (1)
#define CFG_ASDU_C_SE_NC   (1)
#define CFG_ASDU_C_BO_NA   (1)

#define CFG_ASDU_M_EI_NA   (1)

#define CFG_ASDU_C_IC_NA    (1)
#define CFG_ASDU_C_CI_NA   (1)
#define CFG_ASDU_C_RD_NA   (1)
#define CFG_ASDU_C_CS_NA   (1)
#define CFG_ASDU_C_TS_NA   (1)
#define CFG_ASDU_C_RP_NA   (1)
#define CFG_ASDU_C_CD_NA   (1)

#define CFG_ASDU_P_ME_NA    (1)
#define CFG_ASDU_P_ME_NB   (1)
#define CFG_ASDU_P_ME_NC   (1)
#define CFG_ASDU_P_AC_NA   (1)

#define CFG_ASDU_F_FR_NA    (0)
#define CFG_ASDU_F_SR_NA   (0)
#define CFG_ASDU_F_SC_NA   (0)
#define CFG_ASDU_F_LS_NA   (0)
#define CFG_ASDU_F_AF_NA   (0)
#define CFG_ASDU_F_SG_NA   (0)
#define CFG_ASDU_F_DR_TA   (0)

#define CFG_ASDU_COUNT		(51+CFG_ASDU_F_FR_NA+CFG_ASDU_F_SR_NA+CFG_ASDU_F_SC_NA+CFG_ASDU_F_LS_NA+CFG_ASDU_F_AF_NA+CFG_ASDU_F_SG_NA+CFG_ASDU_F_DR_TA)

/************************************************************************/
#endif
