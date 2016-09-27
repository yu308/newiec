#ifndef _iec_object_h_
#define _iec_object_h_

#include "iec_type.h"
#include "iec_cfg.h"

#define OBJECT_NAME_LEN			24


typedef struct iec_object Object;

struct iec_object
{
	int32 id;  /*ID��,�ڴ��ַ��Ϊ��ַ*/
	int8 name[OBJECT_NAME_LEN]; /*������ */

	int32 obj_type; /*��������*/
	//int32 obj_tag; /*�����ʶ ASDU����ϢԪ�ء���Ϣ���ʶ*/

#ifdef CFG_MEM_SYS
	Object *next; /*�¸�����*/
	Object *prev; /*�ϸ�����*/
#endif
	
};


extern uint8 byte_is_equal(uint32 *d1,uint32 *d2,int count);
extern void byte_set_val(uint32 *dst,uint32 val,int count);
#endif
