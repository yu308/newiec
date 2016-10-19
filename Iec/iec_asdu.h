#ifndef _asdu_h_
#define _asdu_h_

#define NO_SEQ		(0)		/*无SEQ*/
#define SEQ			(1)		/*支持SEQ*/

#define NO_QUAL_WORD	(0)

#define NO_TIME		(0)

#define NO_EXT_TIME	(0)
/// <summary>
/// 应用单元标识
/// </summary>
enum Asdu_Ident
{
	M_SP_NA = 1,
	M_SP_TA,

	M_DP_NA,
	M_DP_TA,

	M_ST_NA,
	M_ST_TA,

	M_BO_NA,
	M_BO_TA,

	M_ME_NA,
	M_ME_TA,
	M_ME_NB,
	M_ME_TB,
	M_ME_NC,
	M_ME_TC,

	M_IT_NA,
	M_IT_TA,

	M_EP_TA,
	M_EP_TB,

	M_EP_TC,
	M_PS_NA,
	M_ME_ND,

	M_SP_TB = 30,
	M_DP_TB,
	M_ST_TB,
	M_BO_TB,
	M_ME_TD,
	M_ME_TE,
	M_ME_TF,
	M_IT_TB,
	M_EP_TD,
	M_EP_TE,
	M_EP_TF,

	C_SC_NA = 45,
	C_DC_NA,
	C_RC_NA,
	C_SE_NA,
	C_SE_NB,
	C_SE_NC,
	C_BO_NA,

	M_EI_NA = 70,

	C_IC_NA = 100,
	C_CI_NA,
	C_RD_NA,
	C_CS_NA,
	C_TS_NA,
	C_RP_NA,
	C_CD_NA,

	P_ME_NA = 110,
	P_ME_NB,
	P_ME_NC,
	P_AC_NA,

	F_FR_NA = 120,
	F_SR_NA,
	F_SC_NA,
	F_LS_NA,
	F_AF_NA,
	F_SG_NA,
	F_DR_TA,
};

/// <summary>
/// 传送原因
/// </summary>
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





/// <summary>
/// ASDU配置信息 用于解析或封装信息点数据
/// </summary>
struct asdu_cfg
{	
	int asdu_ident;
	/*int seq_tag;*/ /*运行时决定*/
	int val_ident;	/* 信息点值类型标识*/
	int qual_ident;	/* 信息点品质描述类型标识*/
	int tm_ident;	/* 信息点时间标签标识*/
	int ext_tm_ident; /*信息点扩展时间标签标识 用于继电保护事件信息点*/
};


extern unsigned int iec_asdu_pack_node(char *buff, struct asdu_cfg *cfg, int node_addr, int node_addr_len, int val, int qual, int utc_time,
	int millsecond);
extern struct asdu_cfg* iec_get_asdu_cfg(int ident);
#endif