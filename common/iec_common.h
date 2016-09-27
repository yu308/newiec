
#ifndef _iec_common_h_
#define _iec_common_h_

#include "../common/iec.h"

extern struct iec_system_context *iec_system_init();
extern void iec_system_config_init(struct iec_system_context *context,int link_addr_bytes,
                                   int asdu_addr_bytes,int info_addr_bytes,int cause_src_enable,int sm2_enable);
extern void iec_system_asdu_addr_set(struct iec_system_context *context,uint32 addr);
extern void iec_system_iodev_set(struct iec_system_context *context,rt_device_t dev);


#endif
