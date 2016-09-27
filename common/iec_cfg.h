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



#define CFG_SERIAL_CLIENT_EN		(1)	/*串行方式---客户端(下位机)*/
#define CFG_SERIAL_SERVER_EN		(0)	/*串行方式---服务器端(上位机)*/
#define CFG_NET_CLIENT_EN			(0)	/*网络方式---服务器端(下位机)*/
#define CFG_NET_SERVER_EN			(0)	/*网络方式---客户端(上位机)*/

#define CFG_SOCKET_MAX		(5)			/*允许socket 最大链接数*/


#define CFG_ASDU_NODE_MAX		(6)		/*单个ASDU支持最大信息点数*/
#define CFG_FIRST_FRAME_MAX		(10)		/*一级数据发送缓存*/
#define CFG_SECOND_FRAME_MAX	(10)		/*二级数据发送缓存*/

#endif
