#ifndef _iec_context_h_
#define _iec_context_h_

#include "iec_type.h"
#include "iec_object.h"
#include "iec_def.h"
#include "iec_node.h"
#include "iec_task.h"



struct system_global_data
{

  int global_tag;
  int *export_context;

  iec_task gFirst_Task;  /* һ������ */
  iec_task gSecond_Task;  /* �������� */

  struct ctrl_node  gCtrl_Node;   /* ������Ϣ��ͷԪ�� */
  struct param_node gParam_Node;   /* ��������Ϣ��ͷԪ�� */
  struct info_node gInfo_Node;  /* ң�ŵ���Ϣ��ͷԪ�� */

  /*��ϢԪ�ص�ַ Node_Addr=0 ��������ϢԪ��*/

  Info_E_QOI gCall_QOI; //�ٻ�������ϢԪ��
  Info_E_QCC gCall_QCC; //�������ٻ�������ϢԪ��
  Info_E_TM56  gTime;  //ʱ��ͬ��������ϢԪ��
  Info_E_QRP gReset_task;     //��λ����������ϢԪ��
  Info_E_TM56  gDelay_Get; //��ʱ��ȡ������ϢԪ��v 
};


struct system_applayer
{
  int asdu_addr;    /**< Ӧ�õ�Ԫ������ַ */
  int asdu_addr_bytes; /* Ӧ�÷������ݵ�Ԫ��ַ���� */
  int cause_src_enable;      /*������ ����ԭ���ַ���� Ϊ2 */
  int info_addr_bytes;   /* ��Ϣ�����ַ���� */

  int sm2_enable;   /* SM2�������� */


  
  Asdu_Config *user_asdu;
};



struct system_linklayer
{
  int  link_addr;    /**< ��·��ַ  */
  int link_addr_bytes; /* ��·��ַ���� */
};

struct iec_system_context
{
  rt_device_t dev;     /*ͨ���豸*/
  int communication_protocol_type; /*ͨ��Э������ 101 or 104*/
  struct system_linklayer linklayer;  /* ��·������ */

  struct system_applayer  applayer;   /* Ӧ�ò����� */
  struct system_global_data global_data;  /* ȫ������ */
  rt_mutex_t worker_mtx; /*��,��ֹ���մ���ͷ��ʹ���ͬʱ��ȡ�������� 101 δ��*/
  uint32 *custom_data; /* /�ض�Э����Զ�������/ */
};


extern int32 iec_asdu_unpack(struct system_applayer *applayer,struct system_global_data *global_data,int app_code,uint8 *asdu_data,int asdu_len); 
extern uint32 iec_asdu_pack(Asdu *asdu,uint8* buff,struct system_applayer *applayer, struct system_global_data *zero,int protocol_t); 
extern int iec_task_add(iec_task *head,Asdu *asdu,struct system_applayer *applayer);



#endif
