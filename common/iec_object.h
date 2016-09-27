#ifndef _iec_object_h_
#define _iec_object_h_

#include "iec_type.h"
#include "iec_cfg.h"

#define OBJECT_NAME_LEN			24


typedef struct iec_object Object;

struct iec_object
{
	int32 id;  /*ID号,内存地址作为地址*/
	int8 name[OBJECT_NAME_LEN]; /*对象名 */

	int32 obj_type; /*对象类型*/
	//int32 obj_tag; /*对象标识 ASDU、信息元素、信息体标识*/

#ifdef CFG_MEM_SYS
	Object *next; /*下个对象*/
	Object *prev; /*上个对象*/
#endif
	
};


extern uint8 byte_is_equal(uint32 *d1,uint32 *d2,int count);
extern void byte_set_val(uint32 *dst,uint32 val,int count);
#endif
