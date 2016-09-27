#ifndef _iec_asdu_h_
#define _iec_asdu_h_

#include "iec_type.h"
#include "iec_object.h"
#include "iec_def.h"

#define SQ_EN 1
#define SQ_DIS 0

#define UNFIXED_NODE 0x3F //��Ϣ����Ŀ����

#define TIME_FLAG_EN 1
#define TIME_FLAG_DIS 0

#define MAX_INFO_COUNT      10

typedef struct _asdu_info
{
    Object object;
    
    Type_Ident  typ;   //���ͱ�ʶ
    Var_Struct_Qual var_str_q; //�ɱ�ṹ�޶���
    Comm_Reason reason; //����ԭ��
    ASDU_Addr addr;  //����Ԫ������ַ
    
    uint8 node_count; //��ϢԪ����Ŀ
    uint32 node[MAX_INFO_COUNT]; //��ϢԪ�ص�ַ
    
    uint8 err_len; //�������ݳ���
    uint8 *err_data; //����Ĵ�������

} Asdu;

/*
 * ���Ʒ���Ӧ�÷������ݵ�Ԫ�Ľ���
 *  Asdu  ASDU������Ϣ
 *  node_data ���Ʒ�����ϢԪ�صĻ���
 *  
 * �����Ʒ����Ӧ�÷������ݵ�Ԫͨ��֡����,�����ϵͳ��Ӧ����ϢԪ�ص�
 */
typedef int32 (*asdu_unpack_cb)(Asdu *asdu,uint8 *node_data,int count,uint32 *custom_data);

/*
 * �������ڼ��ӷ����Ӧ�÷������ݵ�Ԫ
 * asdu  ASDU������Ϣ
 * buff ���ݻ���
 * 
 * ��ȡϵͳ�ж�Ӧ��ϢԪ�ص�,���ɼ��ӷ��������
 */ 
typedef int32 (*asdu_pack_cb)(Asdu *asdu,uint8 *buff,int count ,uint32 *custom_data);

typedef void (*asdu_custom_cb)(int *param);

typedef struct _asdu_config
{  
  uint8 asdu_ident; //���ͱ�ʶ
  uint8 single_node; //�Ƿ�֧�ֶ����ϢԪ��  1 ֧�� 0 ��֧��
  asdu_unpack_cb unpack; // ���Ʒ���Ľ�������
  asdu_pack_cb pack; // ���ӷ���ķ�װ����
  asdu_custom_cb  user_cb;//������ϵͳ�������ɻ�Ӧ����֮֡����������ɺ���
}Asdu_Config;

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

extern void asdu_copy_node(Asdu *dst_asdu,Asdu *src_asdu,int addr_count);
extern Asdu *iec_asdu_create(ASDU_Addr addr,Type_Ident ident,uint8 reason,uint32 node_addr);
#endif
