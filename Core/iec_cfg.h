#ifndef _iec_cfg_h_
#define _iec_cfg_h_

#include	"cmsis_os.h"

/************************************************
命名规则说明
	全局变量	      gXxx_Yyy					g开头 首字母大写  单词_连接
	宏            MMMM						全大写
	静态变量	      gxxx_yyy					g开头 全小写    单词_连接
	函数			 level_action_object		level 层  action 操作  object 对象
	声明			 xxxxx						全小写
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


#define SYS_ROLE_CLIENT			(1<<0)		/*仅支持角色为客户端 网络方式时链路层为socket服务器端 */
#define SYS_ROLE_SERVER			(1<<1)		/*仅支持角色为服务端	网络方式链路层为socket客户端*/
#define SYS_ROLE_MIXED			(1<<2)		/*两者支持*/

#define CFG_ROLE_MODE			SYS_ROLE_CLIENT


/*
-客户端模式
	--仅串行通信方式
		LINK 根据实际串行接口数量M1设置
		APP	 1
	--仅网络通信方式
		LINK 根据实际网络接口数量M2设置
		APP   1
	--双通道支持
		LINK 接口数量和M1+M2
		APP	 1
	对应关系: 1、单个APP只对应单个LINK通道  2、单个APP可同时对应多个LINK通道

-服务端模式
	--仅串行通信方式
		LINK 根据实际串行接口数量M1设置
		APP	 >=1 <=M1
	--仅网络通信方式
		LINK 根据实际网络接口数量M2设置
		APP   >=1 <=M2
	--双通道支持
		LINK 接口数量和M1+M2
		APP	 >=2 <=M1+M2
	对应关系: 1、单个APP只对应单个LINK通道  2、单个APP可同时对应多个LINK通道

-混合模式
	客户端与服务器端共存并有交互数据，即数据中转机。APP分为服务端与客户端2种。每个LINK通道只可对应一个APP，APP可相同
	对应关系: 1、单边单个APP只对应单个LINK通道  2、单边单个APP可同时对应多个LINK通道 3、不同边2个APP之间可进行数据交互

综上,APP与LINK之间有1、2两种关系,在做为数据中转机时，具有3


*/

#define CFG_LINK_MAX			(4)		/*支持的链路数量 */
#define CFG_APP_MAX				(1)		/*APP数量 */
#define	CFG_SOCKET_MAX			(5)		/*网络模式下SOCKET最大连接数*/


#define	CFG_ASDU_NODE_MAX       (10)		/*ASDU包含信息点数*/



#endif
