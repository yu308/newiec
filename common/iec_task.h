#ifndef _iec_task_h_
#define _iec_task_h_

#include "iec_object.h"
#include "iec_asdu.h"

typedef struct _iec_task  iec_task;


struct _iec_task
{
    Object obj;
    Asdu *asdu;
	int counter;
};

extern void iec_task_free(iec_task *task);
extern iec_task *iec_task_pick(iec_task *head,int *out);
extern void iec_task_clear(iec_task *head);
#endif
