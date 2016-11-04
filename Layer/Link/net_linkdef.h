#ifndef _net_link_def_h_
#define _net_link_def_h_


#include "link.h"

#define HEAD_START_TAG	0x68	//启动标志

#define F104_I_FORMAT_BYTE	0x0  /* I帧格式标志 */
#define F104_I_FORMAT_BYTE_2      0x2

#define F104_S_FORMAT_BYTE	0x1  /* S帧格式标志 */
#define F104_U_FORMAT_BYTE	0x3  /* U帧格式标志 */

#define check_format(a,b)  (((a&b)==b)?1:0)

#define FRAME_COUNT_K		24 /* 发送方在未被确认连续编号的I格式帧数量后 停止发送 */
#define FRAME_COUNT_W		8  /* 接收方接收连续编号的I格式帧数量后发送确认 */

#define STARTDT_ACT_BIT   (1<<2)
#define STOPDT_ACT_BIT    (1<<4)
#define TESTFR_ACT_BIT    (1<<6)

#define EVT_IEC_TIMEOUT      (0x100)
#define EVT_SUB_IEC_T3_TIMEOUT    (1<<0)

#define EVT_IEC_CNT_FULL     (0x101)
#define EVT_SUB_IEC_K_FULL   (0x1)

/* 信息传输控制域 */
struct i_control_domain
{
  volatile unsigned flag:1;  /* 固定为0 */
  volatile unsigned sent_num:15;   /* 发送序列号 */
  volatile unsigned unused_bit1:1;  /* 固定为0 */
  volatile unsigned recv_num:15; /* 接收序列号 */
};

/* 监视功能控制域 */
struct s_control_domain
{
  unsigned short flag; /* 固定为1 */
  volatile unsigned unused_bit:1;
  volatile unsigned recv_num:15; /* 接收序列号 */
};

/* 无编号控制功能控制域 */
struct u_control_domain
{
  volatile unsigned flag:2; /* 固定为3 */

  volatile unsigned sbit_act:1; /* 数据传输启动生效位 */
  volatile unsigned sbit_confirm:1; /* 数据传输启动确认位 */
  volatile unsigned stopbit_act:1;  /* 数据传输停止生效位 */
  volatile unsigned stopbit_confirm:1; /* 数据传输停止确认位 */
  volatile unsigned tbit_act:1; /* 测试传输生效位 */
  volatile unsigned tbit_confirm:1; /* 测试传输确认位 */

  volatile unsigned unused_bit2:24; /* 固定为0 */
};

union control_domain
{
  unsigned char bytes[4];

  struct i_control_domain i_domain;

  struct s_control_domain s_domain;

  struct u_control_domain u_domain;
};

struct net_link_cfg
{
  int socket;
  
  rt_thread_t time_monitor;
  rt_mutex_t  time_mtx;
  rt_mutex_t  counter_mtx;
  
  rt_sem_t socket_close_sem;
};

struct net_link_info
{
	struct link_obj obj;
	struct net_link_cfg cfg;


  unsigned int current_k;
  unsigned int current_w;

  unsigned int t0_delay;
  unsigned int t1_delay;
  unsigned int t2_delay;
  unsigned int t3_delay;

  unsigned int recv_counter;
  unsigned int sent_counter;
};


struct net_link_info *net_link_create(char *name,int socketid,int dir);
void net_link_del(struct net_link_info *net_info);
void net_link_thread_start(struct net_link_info *net_link);

#endif
