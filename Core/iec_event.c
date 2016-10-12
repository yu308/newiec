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
	return osMessagePut(q_id, (int)evt, millisec);
}

/// <summary>
/// 接收一个消息
/// </summary>
/// <param name="queue_id">The queue_id.</param>
/// <param name="millisec">The millisec.</param>
/// <returns></returns>
struct iec_msg *iec_recv_event(osMessageQId queue_id, int millisec)
{
	struct iec_msg *temp = 0;
	osEvent	event=osMessageGet(queue_id,millisec);

	if (event.status == osEventMessage)
	{
		temp = event.value.p;
	}

	return temp;
}

/// <summary>
/// 初始化已有的msg.
/// </summary>
/// <param name="msg">The MSG.</param>
/// <param name="sender">The sender.</param>
/// <param name="recver">The recver.</param>
/// <param name="evt_type">The evt_type.</param>
/// <param name="data">The data.</param>
void iec_init_event(struct iec_event *evt, int sender, int recver, int evt_type, int *data,int data_auto)
{
	evt->sender = sender;
	evt->recver = recver;
	evt->evt_type = evt_type;
	evt->msg = data;
}


/// <summary>
/// 创建一个新消息.
/// </summary>
/// <param name="sender">The sender.</param>
/// <param name="recver">The recver.</param>
/// <param name="evt_type">The evt_type.</param>
/// <param name="data">The data.</param>
/// <returns></returns>
struct iec_event *iec_create_event(int sender, int recver, int evt_type, int *data,int data_auto)
{
	struct iec_event *evt = (struct iec_event *)XMALLOC(sizeof(struct iec_event));
	if (evt == 0)
	{
		XPRINTF("MSG:ERROR: no memory.\n");
		return 0;
	}
	XMEMSET(evt, 0, sizeof(struct iec_event));

	evt->event_mem_auto = 1;
	iec_init_msg(evt, sender, recver, evt_type, data,data_auto);

	return evt;
}

/// <summary>
/// 清除一个消息
/// </summary>
/// <param name="msg">The MSG.</param>
void iec_free_event(struct iec_event *evt)
{
	if (evt->msg_mem_auto == 1)
	{
		XFREE(evt->msg);
	}

	if (evt->event_mem_auto == 1)
	{
		XFREE(evt);
	}
}