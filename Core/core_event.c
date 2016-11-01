#include "core.h"

/// <summary>
/// 发送一个消息.
/// </summary>
/// <param name="q_id">The q_id.</param>
/// <param name="msg">The MSG.</param>
/// <param name="millisec">The millisec.</param>
/// <returns></returns>
rt_err_t iec_post_event(rt_mailbox_t q_id, struct iec_event *evt, int millisec)
{
  if(evt==0)
    return RT_ERROR;

  rt_mb_send(q_id, (int)evt);

  return RT_EOK;
}

/// <summary>
/// 接收一个消息
/// </summary>
/// <param name="queue_id">The queue_id.</param>
/// <param name="millisec">The millisec.</param>
/// <returns></returns>
struct iec_event *iec_recv_event(rt_mailbox_t queue_id, unsigned int millisec)
{
	struct iec_event *temp = 0;
  rt_err_t  res=0;

  res=rt_mb_recv(queue_id, (rt_uint32_t *)&temp, rt_tick_from_millisecond(millisec));

	if (res == RT_EOK)
	{
		return temp;
	}

	return 0;
}

/**
 * 生成一个事件
 *
 * @param sender  发送者
 * @param recver  接收者
 * @param evt_type  事件类型
 * @param main_msg  事件消息
 * @param auto_free 是否自动释放
 *
 * @return 事件
 */
struct iec_event *iec_create_event(int sender, int recver, int evt_type,
	int *main_msg,int auto_free)
{
	struct iec_event *evt = (struct iec_event *)rt_malloc(sizeof(struct iec_event));
	if (evt == 0)
	{
		rt_kprintf("MSG:ERROR: no memory.\n");
		return 0;
	}
	rt_memset(evt, 0, sizeof(struct iec_event));

	evt->sender = sender;
	evt->recver = recver;
	evt->evt_type = evt_type;
	evt->main_msg = main_msg;
	evt->main_msg_free = auto_free;

	return evt;
}

/**
 * 设置某个事件的子事件信息
 *
 * @param evt 事件
 * @param evt_sub_type 子事件类型
 * @param sub_msg 子事件消息
 * @param auto_free 是否自动释放
 */
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
			rt_free(evt->sub_msg);
	}

	if (evt->main_msg_free == 1)
	{
		if (evt->main_msg)
			rt_free(evt->main_msg);
	}

		rt_free(evt);
}
