#include "iec_event.h"

/// <summary>
/// 发送一个消息.
/// </summary>
/// <param name="q_id">The q_id.</param>
/// <param name="msg">The MSG.</param>
/// <param name="millisec">The millisec.</param>
/// <returns></returns>
osStatus iec_post_event(osMessageQId q_id, struct iec_event *evt, int millisec)
{
  if(evt==0)
    return osErrorValue;

	return osMessagePut(q_id, (int)evt, millisec);
}

/// <summary>
/// 接收一个消息
/// </summary>
/// <param name="queue_id">The queue_id.</param>
/// <param name="millisec">The millisec.</param>
/// <returns></returns>
struct iec_event *iec_recv_event(osMessageQId queue_id, int millisec)
{
	struct iec_event *temp = 0;
	osEvent	event=osMessageGet(queue_id,millisec);

	if (event.status == osEventMessage)
	{
		temp = event.value.p;
	}

	return temp;
}


struct iec_event *iec_create_event(int sender, int recver, int evt_type, 
	int *main_msg,int auto_free)
{
	struct iec_event *evt = (struct iec_event *)XMALLOC(sizeof(struct iec_event));
	if (evt == 0)
	{
		XPRINTF("MSG:ERROR: no memory.\n");
		return 0;
	}
	XMEMSET(evt, 0, sizeof(struct iec_event));

	evt->sender = sender;
	evt->recver = recver;
	evt->evt_type = evt_type;
	evt->main_msg = main_msg;
	evt->main_msg_free = auto_free;
	
	return evt;
}

void iec_set_event_sub(struct iec_event *evt, int evt_sub_type,int *sub_msg, int auto_free)
{
  if(evt==0)
    return;

	evt->evt_sub_type = evt_sub_type;
	evt->sub_msg = sub_msg;
	evt->sub_msg_free = auto_free;
}

/// <summary>
/// 清除一个消息
/// </summary>
/// <param name="msg">The MSG.</param>
void iec_free_event(struct iec_event *evt)
{
  if(evt==0)
    return;

	if (evt->sub_msg_free == 1)
	{
		if (evt->sub_msg)
			XFREE(evt->sub_msg);
	}

	if (evt->main_msg_free == 1)
	{
		if (evt->main_msg)
			XFREE(evt->main_msg);
	}

		XFREE(evt);
}
