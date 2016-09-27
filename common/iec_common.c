#include "../common/iec_common.h"



void iec_system_asdu_addr_set(struct iec_system_context *context,uint32 addr)
{
  byte_set_val((uint32*)(&context->applayer.asdu_addr),addr,context->applayer.asdu_addr_bytes);
}


void iec_system_iodev_set(struct iec_system_context *context,rt_device_t dev)
{
  if(context->dev!=RT_NULL)
    {
      rt_device_close(context->dev);
    }

  context->dev=dev;
}


void iec_system_config_init(struct iec_system_context *context,int link_addr_bytes,
                             int asdu_addr_bytes,int info_addr_bytes,int cause_src_enable,int sm2_enable)
{
  context->linklayer.link_addr_bytes=link_addr_bytes;
  context->applayer.asdu_addr_bytes=asdu_addr_bytes;
  context->applayer.info_addr_bytes=info_addr_bytes;
  context->applayer.cause_src_enable=cause_src_enable;
  context->applayer.sm2_enable=sm2_enable;
}

/**
 * @brief 系统上下文初始化 
 */

struct iec_system_context *iec_system_init()
{
  struct iec_system_context *context=iec_malloc(sizeof(struct iec_system_context));
  iec_memset(context,0,sizeof(struct iec_system_context));
  return context;
}
