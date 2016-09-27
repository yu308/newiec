#ifndef _iec_cfg_h_
#define _iec_cfg_h_

#include	"cmsis_os.h"


/********************运行工作模式配置***************************/
#define SINGLE_MODE			(1)			/*单任务模式*/
#define MUTLI_MODE			(2)			/*多任务模式*/

#define CFG_RUNNING_MODE		MUTLI_MODE

#if(CFG_RUNNING_MODE==MUTLI_MODE)
#define SYS_TASK_PROI			osPriorityNormal
#define	APP_TASK_PROI			osPriorityAboveNormal
#define	LINK_TASK_PROI			osPriorityAboveNormal
#elif(CFG_RUNNING_MODE==SINGLE_MODE)
#define SYS_TASK_PROI			osPriorityNormal
#endif


#define SYS_ROLE_CLIENT			(1)		/*仅支持角色为客户端 网络方式时链路层为socket服务器端 */
#define SYS_ROLE_SERVER			(2)		/*仅支持角色为服务端	网络方式链路层为socket客户端*/
#define SYS_ROLE_MIXED			(3)		/*两者支持*/

#define CFG_ROLE_MODE			SYS_ROLE_CLIENT


/*
-客户端模式
	--仅串行通信方式
		LINK 根据实际串行接口数量设置
		APP	 1
	--仅网络通信方式
		LINK 根据实际网络接口数量设置
		APP   1
	--双通道支持
		LINK 接口数量和
		APP	 1

-服务端模式
	--仅串行通信方式
		LINK 根据实际串行接口数量设置
		APP	 1
	--仅网络通信方式
		LINK 根据实际网络接口数量设置
		APP   1
	--双通道支持
		LINK 接口数量和
		APP	 1


*/



#define CFG_LINK_MAX			(5)		/*支持的链路数量 */
#define CFG_APP_MAX				(1)		/*APP数量 */

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
