#include "rtconfig.h"
#include "rtthread.h"
#include "iec_common.h"

#define TASK_BUFF_COUNT		 (50)
#define ASDU_BUFF_COUNT		(50)

static iec_task *task_buff = 0;
static Asdu *asdu_buff = 0;

void iec_mem_init()
{
	task_buff = rt_malloc(sizeof(iec_task)*TASK_BUFF_COUNT);
	asdu_buff = rt_malloc(sizeof(Asdu)*ASDU_BUFF_COUNT);

	if(task_buff!=0)
		rt_memset(task_buff, 0, sizeof(iec_task)*TASK_BUFF_COUNT);

	if (asdu_buff != 0)
		rt_memset(task_buff, 0, sizeof(Asdu)*TASK_BUFF_COUNT);
}


iec_task* iec_mem_get_task()
{
	int32 i = 0;
	for (i = 0; i < TASK_BUFF_COUNT; i++)
	{
		if (task_buff[i].obj.id == 0)
		{
			task_buff[i].obj.id = (int32)&task_buff[i];
			return &task_buff[i];
		}
	}
	return 0;
}

Asdu* iec_mem_get_asdu()
{
	int32 i = 0;
	for (i = 0; i < TASK_BUFF_COUNT; i++)
	{
		if (asdu_buff[i].object.id == 0)
		{
			asdu_buff[i].object.id = (int32)&asdu_buff[i];
			return &asdu_buff[i];
		}
	}

	return 0;
}


void iec_mem_free_task(iec_task *task)
{
	rt_memset(task, 0, sizeof(iec_task));
}

void iec_mem_free_asdu(Asdu *asdu)
{
	rt_memset(asdu, 0, sizeof(Asdu));
}