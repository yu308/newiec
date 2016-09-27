#ifndef _iec_cfg_h_
#define _iec_cfg_h_


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



#define CFG_SERIAL_CLIENT_EN		(1)	/*���з�ʽ---�ͻ���(��λ��)*/
#define CFG_SERIAL_SERVER_EN		(0)	/*���з�ʽ---��������(��λ��)*/
#define CFG_NET_CLIENT_EN			(0)	/*���緽ʽ---��������(��λ��)*/
#define CFG_NET_SERVER_EN			(0)	/*���緽ʽ---�ͻ���(��λ��)*/

#define CFG_SOCKET_MAX		(5)			/*����socket ���������*/


#define CFG_ASDU_NODE_MAX		(6)		/*����ASDU֧�������Ϣ����*/
#define CFG_FIRST_FRAME_MAX		(10)		/*һ�����ݷ��ͻ���*/
#define CFG_SECOND_FRAME_MAX	(10)		/*�������ݷ��ͻ���*/

#endif
