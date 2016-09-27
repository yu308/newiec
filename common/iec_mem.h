#ifndef _iec_mem_h_
#define _iec_mem_h_

#include "iec_cfg.h"
#include "iec_type.h"

#if(CFG_MEM_SYS==1) /*使用编译器自带malloc*/
#include <stdlib.h>
#endif

#if(CFG_MEM_SYS==2) /*使用OS支持的malloc*/
#define iec_malloc    rt_malloc
#define iec_memset    rt_memset
#define iec_memcpy    rt_memcpy
#define iec_calloc    rt_calloc
#define iec_free      rt_free
#define iec_strlen    rt_strlen


#endif



#endif
