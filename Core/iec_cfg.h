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


#define	CFG_ASDU_NODE_MAX       (10)		/*ASDU������Ϣ����*/



#endif
