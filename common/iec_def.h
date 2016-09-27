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
* 类型标识

typedef struct type_ident
{
	unsigned char ident;
}Type_Ident;*/
typedef unsigned char Type_Ident;

#pragma pack(1)
/*
* 可变结构限定词
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
* 传送原因
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
typedef char Src_Addr; /* 原发地址 */
/*
* 应用服务数据单元公共地址ASDU ADDR
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
* 信息对象地址
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

/***************END 帧类型、限定词、地址 END*****************/



/*******************信息元素**********************/

#define CLOSE	1  		//合
#define CUT	0		//分

#define LOCK	1		//闭锁
#define UNLOCK	0		//未闭锁

#define REPLACE	1		//取代
#define NOT_REPLACE	0	//未取代

#define CURRENT	0		//当前值
#define NOT_CURRENT	1	//非当前值

#define VALID	0		//有效
#define INVALID	1		//无效

/*
* 带品质描述词的单点信息
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


#define NOT_CONFIRM	3	//不确定
#define CONFIRM_CLOSE	2	//确定状态合
#define CONFIRM_CUT	1	//确定状态开
#define MIDDLE		0	//中间状态
/*
* 带品质描述词的双点信息
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



#define OVER	1	//溢出
#define NOT_OVER 0	//未溢出

/*
* 品质描述词
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


#define TM_VALID	0	//动作时间有效
#define TM_INVALID	1	//动作时间无效
/*
* 继电保护设备事件的品质描述词
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


#define DEV_NOT_CHANGE	0	//设备未在瞬变状态
#define DEV_CHANGE	1	//设备处于瞬变状态
/*
* 带瞬变状态指示的值
*/
typedef struct info_element_VTI
{
	volatile unsigned char VAL : 7;
	volatile unsigned char TI : 1;
}Info_E_VTI;


/*
* 归一化值
* 转化为不大于1的数  一般为 测量值/取值范围最大值
*/
typedef struct info_element_NVA
{
	short VAL;
}Info_E_NVA;

/*
* 标度化值
*/
typedef struct info_element_SVA
{
	short VAL;
}Info_E_SVA;

/*
* 754格式的短浮点数
*/
typedef float Info_E_STD;


#define C_OVER		1	//累计周期内计数溢出
#define C_NOT_OVER	0

#define C_JUDGE		1	//计数调整
#define C_NOT_JUDGE	0
/*
* 二进制计数器读数
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
* 继电保护设备单个事件
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


#define T_START		1	//总启动
#define T_NOT_START	0

#define A_PROTECT	1	//A 相 保护
#define A_NOT_PROTECT	0

#define B_PROTECT	1	//B 相 保护
#define B_NOT_PROTECT	0

#define C_PROTECT	1	//C 相 保护
#define C_NOT_PROTECT	0

#define GROUND_PROTECT	1	//接地 保护
#define GROUND_NOT_PROTECT	0

#define BACK_PROTECT	1	//反向 保护
#define BACK_NOT_PROTECT	0
/*
* 继电保护设备启动事件
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

#define T_CMDOUT	1  //命令输出至输出电路
#define T_NOT_CMDOUT	0

#define A_CMDOUT	1  //命令输出至A输出电路
#define A_NOT_CMDOUT	0

#define B_CMDOUT	1  //命令输出至B输出电路
#define B_NOT_CMDOUT	0

#define C_CMDOUT	1  //命令输出至C输出电路
#define C_NOT_CMDOUT	0
/*
* 继电保护设备输出电路信息
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
* 二进制状态信息
*/
typedef struct info_element_BSI
{
	unsigned int VAL;
}Info_E_BSI;

/*
* 固定测试字 固定值 为0x55AA
*/
typedef struct info_element_FBP
{
	unsigned short VAL;
}Info_E_FBP;

#define NOT_OTHER_DEF	0	//无其他定义
#define SHORT_SIGNLE	1	//短脉冲
#define LONG_SIGNLE	2	//长脉冲
#define CON_SIGNLE	3	//持续脉冲

#define CHOOSE         1     //选择
#define EXECUTE        0     //执行
/*
* 命令限定词
*/
typedef struct info_element_QOC
{
	volatile unsigned char QU : 5;
	volatile unsigned char SE : 1;
}Info_E_QOC;

#define NOT_ALLOW	0	//不允许
/*
* 单点命令
*/
typedef struct info_element_SCO
{
	volatile unsigned char SCS : 1;
	volatile unsigned char RES : 1;
	volatile unsigned char QU : 5;
	volatile unsigned char SE : 1;
}Info_E_SCO;

/*
* 双命令
*/
typedef struct info_element_DCO
{
	volatile unsigned char DCS : 2;
	volatile unsigned char QU : 5;
	volatile unsigned char SE : 1;

}Info_E_DCO;

#define UP_STEP		2	//升一步
#define DOWN_STEP	1	//降一步
/*
* 步调节命令
*/
typedef struct info_element_RCO
{
	volatile unsigned char RCS : 2;
	volatile unsigned char QU : 5;
	volatile unsigned char SE : 1;
}Info_E_RCO;

/*
* 二进制时间-7字节长
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
* 二进制时间-3字节长
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
* 二进制时间-3字节长
*/
typedef struct info_element_CP16Tm2a
{
	unsigned short M_SEC;
}Info_E_TM16;


#define CURRENT_CLOSE_POWER	0	//本地电源合上
#define CURRENT_HAND_RESET	1	//本地手动复位
#define REMOTE_RESET		2	//远方复位

#define PARAM_CHANGE	1		//参数改变的初始化
#define PARAM_NOT_CHANGE	0

/*
* 初始化原因
*/
typedef struct info_element_COI
{
	volatile unsigned char REASON : 7;
	volatile unsigned char CHANGED : 1;
}Info_E_COI;

#define GOL_CALL	20 //全局召唤

/*
* 召唤限定词
*/
typedef unsigned char Info_E_QOI;

#define GROUP_REQ_1	1	//请求计数量第1组
#define GROUP_REQ_2	2
#define GROUP_REQ_3	3
#define GROUP_REQ_4	4
#define GROUP_REQ_G	5	//总的请求计数量

#define NOT_RESET	0
#define REQ_FREEZE	1	//冻结计数 不复位
#define REQ_Z_RESET	2	//冻结带复位
#define REQ_RESET	3	//计数量复位


/*
* 计数量召唤命令限定词
*/
typedef struct info_element_QCC
{
	volatile unsigned char RQT : 6;
	volatile unsigned char FRZ : 2;
}Info_E_QCC;

#define LIMIT		1	//门限值
#define FILTER_FACTOR	2	//平滑系数 滤波系数
#define MV_UP_LIMIT	4	//测量值上限
#define MV_DW_LIMIT	3	//测量值下限

#define MV_PARAM_CHANGE	1	//当地参数改变
#define MV_PARAM_NOT_CHANGE	0

#define MV_PARAM_RUN	0	//参数运行
#define MV_PARAM_NOT_RUN	1
/*
* 测量值参数限定词
*/
typedef struct info_element_QPM
{
	volatile unsigned char KPA : 6;
	volatile unsigned char LPC : 1;
	volatile unsigned char POP : 1;
}Info_E_QPM;

#define ENABLE_LAST_LOAD	1	//激活或停止之前装载的参数
#define ENABLE_INFO_PARAM	2	//激活或停止寻址的信息对象的参数
#define ENABLE_LOOP_PARAM	3	//激活或停止寻址的持续循环或周期传输的信息对象
/*参数激活限定词*/
typedef unsigned char Info_E_QPA;


#define TASK_RESET	1	//进程的总复位
#define RESET_EVENT_INFO_TM	2	//复位事件缓冲区待处理的带时标信息
/*复位进程命令限定词*/
typedef unsigned char Info_E_QRP;

#define FILE_OP_CONFIRM_Y	0	//文件相关操作的肯定确认
#define FILE_OP_CONFIRM_N	1	//否定确认
/*
* 文件准备就绪限定词
*/
typedef struct info_element__FRQ
{
	volatile unsigned char IDENT : 7;
	volatile unsigned char OPERATE : 1;

}Info_E_FRQ;

#define SEC_READY	0	//节准备就绪
#define SEC_NOT_READY	1
/*
* 节准备就绪限定词
*/
typedef struct info_element_SRQ
{
	volatile unsigned char IDENT : 7;
	volatile unsigned char READY : 1;
}Info_E_SRQ;

#define OP_FILE_SEL	1			//选择文件
#define OP_FILE_REQ	2			//请求文件
#define OP_FILE_ACT_STOP	3		//停止文件操作
#define OP_FILE_DEL	4			//删除文件
#define OP_SEC_SEL	5			//选择节
#define OP_SEC_REQ	6			//请求节
#define OP_SEC_ACT_STOP		7		//停止节操作

#define ST_NOT_REQ_MEM	1			//无请求的存储空间
#define ST_CHECK_ERR	2			//校验和错
#define ST_NOT_SERVICE	3			//非期望的通信服务
#define ST_NOT_FILE_NAME	4		//非期望的文件名
#define ST_NOT_SEC_NAME	5			//非期望的节名
/*
* 选择和召唤限定词
*/
typedef struct info_element_SCQ
{
	volatile unsigned char SELECT : 4;
	volatile unsigned char STAT : 4;
}Info_E_SCQ;


#define TRS_F_NOT_STOP_CONFIRM		1	//不带停止确认的文件传输
#define TRS_F_STOP_CONFIRM		2	//带停止确认的文件传输
#define TRS_S_NOT_STOP_CONFIRM		3	//不带停止确认的节传输
#define TRS_S_STOP_CONFIRM		4	//带停止确认的节传输
/*最后的字和段的限定词*/
typedef unsigned char Info_E_LSQ;



#define TRS_F_CONFIRM_Y	1	//文件传输的肯定认可
#define TRS_F_CONFIRM_N	2	//文件传输的否定认可
#define TRS_S_CONFIRM_Y	3	//节传输的肯定认可
#define TRS_S_CONFIRM_N	4	//节传输的否定认可
/*
* 文件认可或节认可限定词
*/
typedef struct info_element_AFQ
{
	volatile unsigned char ACCEPT : 4;
	volatile unsigned char STAT : 4;
}Info_E_AFQ;

/*文件名称*/
typedef unsigned short Info_E_NOF;

/*节点名称*/
typedef unsigned char Info_E_NOS;

/*文件或节点的长度*/
typedef struct info_element_LOF
{
	unsigned char VAL[3];
}Info_E_LOF;

/*段的长度*/
typedef unsigned char Info_E_LOS;

/*校验和*/
typedef unsigned char Info_E_CHS;

#define LAST_DIR	1	//最后目录文件
#define DIR		0	//后面还有目录文件

#define FILE_DEF	0 	//定义文件名
#define CHILD_DIR_DEF	1	//定义子目录名

#define TRS_WAIT	0	//文件等待传输
#define TRS_ACT		1	//文件传输已经激活
/*
* 文件的状态
*/
typedef struct info_element_SOF
{
	volatile unsigned char STATUS : 5;
	volatile unsigned char LFD : 1;
	volatile unsigned char FOR : 1;
	volatile unsigned char FA : 1;
}Info_E_SOF;

#define SELECT	1	//选择
#define ACT	0	//执行
/*
* 设定命令限定词
*/
typedef struct info_element_QOS
{
	volatile unsigned char QL : 7;
	volatile unsigned char SE : 1;
}Info_E_QOS;

/*
* 状态和状态变位检出
*/
typedef struct info_element_SCD
{
	unsigned short ST;
	unsigned short CD;
}Info_E_SCD;


/*信息元素类型*/
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

/************** END 信息元素  END*********************/


#endif
