#ifndef _net_link_def_h_
#define _net_link_def_h_


#include "link.h"

#define HEAD_START_TAG	0x68	//������־

#define F104_I_FORMAT_BYTE	0x0  /* I֡��ʽ��־ */
#define F104_I_FORMAT_BYTE_2      0x2

#define F104_S_FORMAT_BYTE	0x1  /* S֡��ʽ��־ */
#define F104_U_FORMAT_BYTE	0x3  /* U֡��ʽ��־ */

#define check_format(a,b)  (((a&b)==b)?1:0)

#define FRAME_COUNT_K		24 /* ���ͷ���δ��ȷ��������ŵ�I��ʽ֡������ ֹͣ���� */
#define FRAME_COUNT_W		8  /* ���շ�����������ŵ�I��ʽ֡��������ȷ�� */

#define STARTDT_ACT_BIT   (1<<2)
#define STOPDT_ACT_BIT    (1<<4)
#define TESTFR_ACT_BIT    (1<<6)

#define EVT_IEC_TIMEOUT      (0x100)
#define EVT_SUB_IEC_T3_TIMEOUT    (1<<0)

#define EVT_IEC_CNT_FULL     (0x101)
#define EVT_SUB_IEC_K_FULL   (0x1)

/* ��Ϣ��������� */
struct i_control_domain
{
  volatile unsigned flag:1;  /* �̶�Ϊ0 */
  volatile unsigned sent_num:15;   /* �������к� */
  volatile unsigned unused_bit1:1;  /* �̶�Ϊ0 */
  volatile unsigned recv_num:15; /* �������к� */
};

/* ���ӹ��ܿ����� */
struct s_control_domain
{
  unsigned short flag; /* �̶�Ϊ1 */
  volatile unsigned unused_bit:1;
  volatile unsigned recv_num:15; /* �������к� */
};

/* �ޱ�ſ��ƹ��ܿ����� */
struct u_control_domain
{
  volatile unsigned flag:2; /* �̶�Ϊ3 */

  volatile unsigned sbit_act:1; /* ���ݴ���������Чλ */
  volatile unsigned sbit_confirm:1; /* ���ݴ�������ȷ��λ */
  volatile unsigned stopbit_act:1;  /* ���ݴ���ֹͣ��Чλ */
  volatile unsigned stopbit_confirm:1; /* ���ݴ���ֹͣȷ��λ */
  volatile unsigned tbit_act:1; /* ���Դ�����Чλ */
  volatile unsigned tbit_confirm:1; /* ���Դ���ȷ��λ */

  volatile unsigned unused_bit2:24; /* �̶�Ϊ0 */
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
