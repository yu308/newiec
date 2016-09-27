#include "../common/iec.h"
#include "iec_def.h"
#include "iec_asdu.h"
#include "iec_node.h"
#include "iec_context.h"
#include "iec_asducfg.h"


extern iec_task* iec_mem_get_task();
extern Asdu* iec_mem_get_asdu();
extern void iec_mem_free_task(iec_task *task);
extern void iec_mem_free_asdu(Asdu *asdu);
/*
  �Ƚ��ֽ��Ƿ����
  count��ΪҪ�Ƚϵ��ֽ���
*/
uint8 byte_is_equal(uint32 *d1,uint32 *d2,int count)
{
  int i=0;
  char *t1=(char *)d1;
  char *t2=(char *)d2;

  while(i<count)
    {
      if(*t1!=*t2)
        return 0;

      t1++,t2++,i++;
    }
  return 1;
}

void byte_set_val(uint32 *dst,uint32 val,int count)
{
  int i=0;
  uint32 temp=val;
  char *t1=(char *)dst;
  char *t2=(char *)&temp;

  for(i=0;i<count;i++)
    *t1++=*t2++;
 
}





static uint8 check_asdu_node(Asdu* asdu,uint32 addr,int count)
{
  uint8 i=0;

  for(i=0;i<asdu->node_count;i++)
  {
    if(byte_is_equal(&asdu->node[i], &addr, count)==1)
      return 1;
  }

  return 0;
}

/**
 * @brief  �ϲ�2����ͬASDU��Ԫ����ϢԪ��
 * @param dst_asdu  Ŀ��ASDU
 * @param src_asdu   ԴASDU
 */

void asdu_copy_node(Asdu *dst_asdu,Asdu *src_asdu,int addr_count)
{
    uint8 i=0,idx=dst_asdu->node_count;

    for(i=0;i<src_asdu->node_count;i++)
    {
      if(check_asdu_node(dst_asdu,src_asdu->node[i],addr_count)==1)
        continue;
        dst_asdu->node[idx++]=src_asdu->node[i];
        dst_asdu->var_str_q.info_count++;
        dst_asdu->node_count++;
    }
}

/**
 * @brief ���Ʒ���Ӧ�÷������ݵ�Ԫ����
 * @param app_code  Ӧ�÷������ݵ�Ԫ��ʶ
 * @param asdu_data   ����
 * @param len ASDU�����ݳ���
 * @return  ����״̬    1  �ɹ�
 *                      0  �ڴ治��
 *                     -1  Ӧ�õ�Ԫ��ַ����
 *                     -2  ��ƥ����Ϣ�����ַ
 *                     -3  δ֪����
 *                     -4  δ֪ԭ��
 *
 *
 */

int32 iec_asdu_unpack(struct system_applayer *applayer,struct system_global_data *global_data,int app_code,uint8 *asdu_data,int asdu_len)
{
    int32 state=1;

    Asdu *asdu=iec_malloc(sizeof(Asdu));
	/*Asdu *asdu = iec_mem_get_asdu();*/

    if(asdu==0)
    {
        return 0;
    }

    iec_memcpy(&asdu->typ,&asdu_data[0],sizeof(Type_Ident));
    iec_memcpy(&asdu->var_str_q,&asdu_data[1],sizeof(Var_Struct_Qual));
    iec_memcpy(&asdu->reason,&asdu_data[2],1);
    iec_memcpy(&asdu->addr,&asdu_data[3 + applayer->cause_src_enable],applayer->asdu_addr_bytes);

    if(byte_is_equal((uint32 *)(&applayer->asdu_addr),&asdu->addr,applayer->asdu_addr_bytes)==0)
      {
        state=-1;
        goto end;
      }

    Asdu_Config *asdu_cfg=asdu_cfg_find(app_code);
    if(asdu_cfg==0)
    {
        state=-3;
        goto end;
    }

    state=asdu_cfg->unpack(asdu,&asdu_data[3+applayer->asdu_addr_bytes+ applayer->cause_src_enable],applayer->info_addr_bytes, (uint32 *)global_data);
    asdu->err_len=0;
    asdu->err_data=0;

 end:
    if(state==-1)
      byte_set_val((uint32 *)(&asdu->reason),Unknowasdu,1+applayer->cause_src_enable);
    if(state==-3)
      byte_set_val((uint32 *)(&asdu->reason),Unknowtype,1+applayer->cause_src_enable);
    if(state==-2)
      byte_set_val((uint32 *)(&asdu->reason),Unknowinfo,1+applayer->cause_src_enable);
    if(state==-4)
      byte_set_val((uint32 *)(&asdu->reason), Unknowreason, 1+applayer->cause_src_enable);

    if(state<1)
    {
        asdu->err_len=asdu_len-sizeof(Type_Ident)-sizeof(Var_Struct_Qual)-sizeof(Comm_Reason)-applayer->asdu_addr_bytes-applayer->cause_src_enable;
        asdu->err_data=iec_malloc(asdu->err_len);
        if(asdu->err_data==0)
        {
            return 0;
        }
        
        iec_memcpy(asdu->err_data,&asdu_data[3+applayer->asdu_addr_bytes],asdu->err_len);
    }

   
    state=iec_task_add(&global_data->gFirst_Task, asdu, applayer);
    return state;
}

/**
 * @brief ��װ���Ȳ��̶�֡
 * @param asdu  Ӧ�÷������ݵ�Ԫ��װ��Ϣ
 * @param buff  ���ݻ���
 * @return  ��װ�����ɵ����ݳ���
 */

uint32 iec_asdu_pack(Asdu *asdu,uint8* buff,struct system_applayer *applayer, struct system_global_data *zero,int protocol_t)
{
    uint32 len=0;

    Asdu_Config *asdu_cfg=asdu_cfg_find(asdu->typ);
    
    if(asdu_cfg==0)
    {
        return 0;
    }
    
    iec_memcpy(&buff[0],&asdu->typ,sizeof(Type_Ident));
    iec_memcpy(&buff[1],&asdu->var_str_q,sizeof(Var_Struct_Qual));
    iec_memcpy(&buff[2],&asdu->reason,1);
	if (applayer->cause_src_enable)
		buff[3] = 0;
    iec_memcpy(&buff[3+ applayer->cause_src_enable],&asdu->addr,applayer->asdu_addr_bytes);
    len=3+applayer->asdu_addr_bytes+ applayer->cause_src_enable;
    
    len+=asdu_cfg->pack(asdu,&buff[len],applayer->info_addr_bytes,(uint32 *)zero);

    if(asdu_cfg->user_cb!=0&&asdu->reason==Actcon)
      {
        int param[3];
        param[0]=(int)zero;
        param[1]=(int)applayer;
		param[3] = (int)protocol_t;
        asdu_cfg->user_cb(param);
      }

    return len;
}

/**
 * @brief ����һ��Ӧ�÷������ݵ�Ԫ��װ��Ϣ
 * @param ident  ASDU���ͱ�ʶ
 * @param reason  ����ԭ��
 * @param node_addr  �������͵���ϢԪ�ص�ַ
 */

Asdu *iec_asdu_create(ASDU_Addr addr, Type_Ident ident,uint8 reason,uint32 node_addr)
{
    Asdu *asdu=iec_malloc(sizeof(Asdu));
	/*Asdu *asdu = iec_mem_get_asdu();*/
    
    if(asdu==0)
        return 0;
    
    iec_memset(asdu,0,sizeof(Asdu));
    
    asdu->typ=ident;
    asdu->addr=addr;
    asdu->reason=reason;
    asdu->node_count=1;
    asdu->node[0]=node_addr;
    
    return asdu;
}
