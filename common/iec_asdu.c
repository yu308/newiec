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
  比较字节是否相等
  count：为要比较的字节数
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
 * @brief  合并2个相同ASDU单元的信息元素
 * @param dst_asdu  目的ASDU
 * @param src_asdu   源ASDU
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
 * @brief 控制方向应用服务数据单元解析
 * @param app_code  应用服务数据单元标识
 * @param asdu_data   数据
 * @param len ASDU的数据长度
 * @return  解析状态    1  成功
 *                      0  内存不足
 *                     -1  应用单元地址不符
 *                     -2  无匹配信息对象地址
 *                     -3  未知类型
 *                     -4  未知原因
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
 * @brief 封装长度不固定帧
 * @param asdu  应用服务数据单元封装信息
 * @param buff  数据缓存
 * @return  封装后生成的数据长度
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
 * @brief 生成一个应用服务数据单元封装信息
 * @param ident  ASDU类型标识
 * @param reason  传送原因
 * @param node_addr  引发传送的信息元素地址
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
