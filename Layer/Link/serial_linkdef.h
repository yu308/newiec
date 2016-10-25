#ifndef _iec_def_h_
#define _iec_def_h_

/****************************************LINK Layer***********************************/

#define FIXED_FRAME		(1)			/*Ö¡ÀàÐÍ ¹Ì¶¨³¤¶ÈÖ¡*/		
#define UNFIXED_FRAME	(2)			/*Ö¡ÀàÐÍ ·Ç¹Ì¶¨³¤¶ÈÖ¡*/

#define UNFIXED_HEAD_TAG	(0x68)		/*·Ç¹Ì¶¨Ö¡Í·*/
#define FIXED_HEAD_TAG		(0x10)		/*¹Ì¶¨Ö¡Í·*/
#define FRAME_END_TAG		0x16		/*Ö¡½áÎ²±êÊ¶*/

#define CTRL_DOMAIN_LEN		(1)			/*¿ØÖÆÓò³¤¶È*/

/**********************¿ØÖÆÓò****************/
//ÉÏ´«¹¦ÄÜÂë
#define FC_UP_YES	0	/*¿Ï¶¨ÈÏ¿É*/
#define FC_UP_NO	1	/*·ñ¶¨ÈÏ¿É*/
#define FC_UP_USER_DATA	8	/*ÓÃ»§Êý¾Ý*/
#define FC_UP_NO_DATA	9	/*ÎÞÊý¾Ý*/
#define FC_UP_LINK	11	/*ÏìÓ¦Á´Â·×´Ì¬*/

//ÏÂËÍ¹¦ÄÜÂë
#define FC_DW_RST_REMOTE	0	/**< Ô¶·½¸´Î» */
#define FC_DW_RST_USER		1	/**< ¸´Î»ÓÃ»§ */
#define FC_DW_DATA_YES		3	/**< È·ÈÏ/·¢ËÍÓÃ»§Êý¾Ý */
#define FC_DW_DATA_NO		4	/**< ·ñ¶¨ÓÃ»§Êý¾Ý */
#define FC_DW_REQ		8	/**< ·ÃÎÊÇëÇó */
#define FC_DW_LINK		9   /**< ÇëÇóÁ´Â·×´Ì¬ */
#define FC_DW_FIRST_DATA	10	/**< Ò»¼¶Êý¾Ý */
#define FC_DW_SEC_DATA		11	/**< ¶þ¼¶Êý¾Ý */

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


/// <summary>
/// 串行方式--链路配置
/// </summary>
struct serial_link_cfg
{
	int link_addr;			/*链路地址*/
	int link_addr_len;		/*链路地址长度*/        
		
	char *prev_sent_buff;	/*上一此发送数据缓存*/
	int	prev_sent_len;		/*发送长度*/

};

/// <summary>
/// 串行方式--链路运行信息
/// </summary>
struct serial_link_info
{
	struct link_obj  obj;
	struct serial_link_cfg cfg;

	int acd_tag;
	int app_tag;
	int fcb;
	int applayer_id;


};


/**************链路分发处理判断结果******************/
#define NO_AWS				(0)			/*无回答*/
#define INVAILD_FCB			(1)			/*无效FCB计数*/
#define TO_LINK				(2)			/*至LINK*/
#define TO_LINK_REQ				(3)			/*至LINK链路状态*/
#define TO_APP_FIRST				(4)			/*至APP*/
#define TO_APP_SECOND		(5)
#define TO_APP_USER			(6)



extern struct serial_link_info *serial_link_create(char *name,int addr,int addr_len,int dir);
extern void serial_link_del(struct serial_link_info *info);
extern void serial_link_thread_start(struct serial_link_info *info);



#endif
