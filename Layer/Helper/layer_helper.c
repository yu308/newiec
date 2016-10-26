#include "layer_helper.h"

/** 
 * 用于向app发送请求数据事件
 * 
 * @param info  链路信息
 * @param sub_evt 请求数据类型
 */
void link_send_req_evt_to_app(struct link_obj *link,int sub_evt)
{
	struct iec_event *evt = 0;
	evt = iec_create_event(link, link->applayer_id, EVT_APP_RECV_DATA, 0, 0);
	iec_set_event_sub(evt, sub_evt, 0, 0);
	iec_post_event(((struct app_info *)link->applayer_id)->app_event, evt, 20);
}

/** 
 *  用于向app发送asdu类用户数据
 * 
 * @param info 链路信息
 * @param asdu_data 收到的asdu数据缓存地址
 * @param asdu_len  asdu数据长度
 */
void link_send_asdu_evt_to_app(struct link_obj *link,char *asdu_data,int asdu_len)
{
  struct iec_event *evt=0;
  int *recv=(int *)rt_malloc(sizeof(int)*2);
  recv[0]=(int)asdu_data;
  recv[1]=asdu_len;
  evt=iec_create_event(link, link->applayer_id, EVT_APP_RECV_DATA, 0, 0);
  iec_set_event_sub(evt, EVT_SUB_DAT_USER, recv, 1);
  iec_post_event(((struct app_info *)link->applayer_id)->app_event, evt, 20);
}

/** 
 * 用于向link通知有信息点更新
 * 
 * @param info App信息
 * @param sub_evt  信息点数据优先类别
 */
void app_send_update_evt_to_link(struct app_info *info,struct link_obj *link,int sub_evt)
{
  struct iec_event *evt = 0;
  evt=iec_create_event((unsigned int)info,(unsigned int)link,EVT_LINK_RECV_DATA, 0, 0);
  iec_set_event_sub(evt,sub_evt,0,0);
  iec_post_event(link->mb_event, evt, 20);
}

/** 
 * 用于向link确认用户数据 确认数据系统内部有更新信息点操作,已产生任务数据
 * 此处直接调用update事件通知函数
 * 
 * @param info  App信息
 * @param link_info 链路信息
 */
void app_send_userdata_ack_evt_to_link(struct app_info *info,struct link_obj *link)
{
  app_send_update_evt_to_link(info, link, EVT_SUB_DAT_USER);
}

/** 
 * 用于APP向Link通知需发送用户数据
 * 
 * @param info 
 * @param send_asdu 
 */
void app_send_asdu_evt_to_link(struct app_info *info,struct app_send_info *send_asdu)
{
  struct iec_event *evt=0;
  evt=iec_create_event((unsigned int)info,(unsigned int)send_asdu->link_id,EVT_LINK_SEND_DATA,0,0);
  iec_set_event_sub(evt,EVT_SUB_DAT_USER,send_asdu,1);
  iec_post_event(((struct link_obj*)(send_asdu->link_id))->mb_event,evt,20);
}
