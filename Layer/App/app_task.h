#ifndef _app_task_h_
#define _app_task_h_

#include "arraylist.h"

struct app_task
{
	unsigned int asdu_ident;
	unsigned int seq;
	unsigned int cause;
	arraylist *node;
};

#endif