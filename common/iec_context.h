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

  iec_task gFirst_Task;  /* 一级数据 */
  iec_task gSecond_Task;  /* 二级数据 */

  struct ctrl_node  gCtrl_Node;   /* 控制信息点头元素 */
  struct param_node gParam_Node;   /* 参数类信息体头元素 */
  struct info_node gInfo_Node;  /* 遥信点信息体头元素 */

  /*信息元素地址 Node_Addr=0 的特殊信息元素*/

  Info_E_QOI gCall_QOI; //召唤命令信息元素
  Info_E_QCC gCall_QCC; //计数量召唤命令信息元素
  Info_E_TM56  gTime;  //时间同步命令信息元素
  Info_E_QRP gReset_task;     //复位进程命令信息元素
  Info_E_TM56  gDelay_Get; //延时获取命令信息元素v 
};


struct system_applayer
{
  int asdu_addr;    /**< 应用单元公共地址 */
  int asdu_addr_bytes; /* 应用服务数据单元地址长度 */
  int cause_src_enable;      /*启用则 传送原因地址长度 为2 */
  int info_addr_bytes;   /* 信息对象地址长度 */

  int sm2_enable;   /* SM2加密启用 */


  
  Asdu_Config *user_asdu;
};



struct system_linklayer
{
  int  link_addr;    /**< 链路地址  */
  int link_addr_bytes; /* 链路地址长度 */
};

struct iec_system_context
{
  rt_device_t dev;     /*通信设备*/
  int communication_protocol_type; /*通信协议类型 101 or 104*/
  struct system_linklayer linklayer;  /* 链路层配置 */

  struct system_applayer  applayer;   /* 应用层配置 */
  struct system_global_data global_data;  /* 全局数据 */
  rt_mutex_t worker_mtx; /*锁,防止接收处理和发送处理同时获取发送任务 101 未用*/
  uint32 *custom_data; /* /特定协议的自定义数据/ */
};


extern int32 iec_asdu_unpack(struct system_applayer *applayer,struct system_global_data *global_data,int app_code,uint8 *asdu_data,int asdu_len); 
extern uint32 iec_asdu_pack(Asdu *asdu,uint8* buff,struct system_applayer *applayer, struct system_global_data *zero,int protocol_t); 
extern int iec_task_add(iec_task *head,Asdu *asdu,struct system_applayer *applayer);



#endif
