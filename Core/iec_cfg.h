#ifndef _iec_cfg_h_
#define _iec_cfg_h_

#include	"cmsis_os.h"


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


#define SYS_ROLE_CLIENT			(1)		/*��֧�ֽ�ɫΪ�ͻ��� ���緽ʽʱ��·��Ϊsocket�������� */
#define SYS_ROLE_SERVER			(2)		/*��֧�ֽ�ɫΪ�����	���緽ʽ��·��Ϊsocket�ͻ���*/
#define SYS_ROLE_MIXED			(3)		/*����֧��*/

#define CFG_ROLE_MODE			SYS_ROLE_CLIENT


/*
-�ͻ���ģʽ
	--������ͨ�ŷ�ʽ
		LINK ����ʵ�ʴ��нӿ���������
		APP	 1
	--������ͨ�ŷ�ʽ
		LINK ����ʵ������ӿ���������
		APP   1
	--˫ͨ��֧��
		LINK �ӿ�������
		APP	 1

-�����ģʽ
	--������ͨ�ŷ�ʽ
		LINK ����ʵ�ʴ��нӿ���������
		APP	 1
	--������ͨ�ŷ�ʽ
		LINK ����ʵ������ӿ���������
		APP   1
	--˫ͨ��֧��
		LINK �ӿ�������
		APP	 1


*/



#define CFG_LINK_MAX			(5)		/*֧�ֵ���·���� */
#define CFG_APP_MAX				(1)		/*APP���� */

/********** Memory ************/
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
/**************************************/

#endif
