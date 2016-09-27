#include "iec_event.h"

/// <summary>
/// 发送一个消息.
/// </summary>
/// <param name="q_id">The q_id.</param>
/// <param name="msg">The MSG.</param>
/// <param name="millisec">The millisec.</param>
/// <returns></returns>
osStatus iec_post_msg(osMessageQId q_id, struct iec_msg *msg, int millisec)
{
	return osMessagePut(q_id, (int)msg, millisec);
}

/// <summary>
/// 接收一个消息
/// </summary>
/// <param name="queue_id">The queue_id.</param>
/// <param name="millisec">The millisec.</param>
/// <returns></returns>
struct iec_msg *iec_recv_msg(osMessageQId queue_id, int millisec)
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
void iec_init_msg(struct iec_msg *msg, int sender, int recver, int evt_type, int *data)
{
	msg->sender = sender;
	msg->recver = recver;
	msg->evt_type = evt_type;
	msg->msg = data;
}


/// <summary>
/// 创建一个新消息.
/// </summary>
/// <param name="sender">The sender.</param>
/// <param name="recver">The recver.</param>
/// <param name="evt_type">The evt_type.</param>
/// <param name="data">The data.</param>
/// <returns></returns>
struct iec_msg *iec_create_msg(int sender, int recver, int evt_type, int *data)
{
	struct iec_msg *msg = (struct iec_msg *)XMALLOC(sizeof(struct iec_msg));
	if (msg == 0)
	{
		XPRINTF("MSG:ERROR: no memory.\n");
		return 0;
	}
	msg->mem_auto = 1;
	iec_init_msg(msg, sender, recver, evt_type, data);

	return msg;
}

/// <summary>
/// 清除一个消息
/// </summary>
/// <param name="msg">The MSG.</param>
void iec_free_msg(struct iec_msg *msg)
{
	if (msg->mem_auto == 1)
	{
		XFREE(msg);
	}
}