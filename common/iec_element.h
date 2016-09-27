#ifndef _iec_element_h_
#define _iec_element_h_

#include "iec_type.h"
#include "iec_object.h"
#include "iec_def.h" 

#define SIZE_OF_ELEMENT(t)	(sizeof(Info_E_##t))
#define ALLOC_ELEMENT(t)	((void *)calloc(1,SIZE_OF_ELEMENT(t)))
#define INIT_ELEMENT(p,t)         (memset(p,0,SIZE_OF_ELEMENT(t)))
#define ELEMENT_TO_BUFF(e,b,t)	(memcpy(b,e,SIZE_OF_ELEMENT(t)))
#define Deactcon(e,b,t)  (memcpy(e,b,SIZE_OF_ELEMENT(t)))


struct info_element
{
    Object obj;
    
    uint32 *val;
};

extern struct info_element *iec_element_create(enum Element_Ident ident);

#endif