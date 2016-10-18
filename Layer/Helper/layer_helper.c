#include "layer_helper.h"

/** 
 * 用于向app发送请求数据事件
 * 
 * @param info  链路信息
 * @param sub_evt 请求数据类型
 */
void serial_link_send_req_evt_to_app(struct serial_link_info *info,int sub_evt)
{
	struct iec_event *evt = 0;
	evt = iec_create_event(info, info->applayer_id, EVT_APP_RECV_DATA, 0, 0);
	iec_set_event_sub(evt, sub_evt, 0, 0);
	iec_post_event(((struct app_info *)info->applayer_id)->app_event, evt, 20);
}

/** 
 *  用于向app发送asdu类用户数据
 * 
 * @param info 链路信息
 * @param asdu_data 收到的asdu数据缓存地址
 */
void serial_link_send_asdu_evt_to_app(struct serial_link_info *info,int *asdu_data)
{
  struct iec_event *evt=0;
  evt=iec_create_event(info, info->applayer_id, EVT_APP_RECV_DATA, 0, 0);
  iec_set_event_sub(evt, EVT_SUB_DAT_USER, asdu_data, 0);
  iec_post_event(((struct app_info *)info->applayer_id)->app_event, evt, 20);
}

/** 
 * 用于向link通知有信息点更新
 * 
 * @param info App信息
 * @param sub_evt  信息点数据优先类别
 */
void app_send_update_evt_to_link(struct app_info *info,struct serial_link_info *link_info,int sub_evt)
{
  struct iec_event *evt = 0;
  evt=iec_create_event((unsigned int)info,(unsigned int)link_info, EVT_LINK_RECV_DATA, 0, 0);
  iec_set_event_sub(evt,sub_evt,0,0);
  iec_post_event(link_info->serial_event, evt, 20);
}

void app_send_asdu_evt_to_link(struct app_info *info,struct app_send_info *send_asdu)
{
  struct iec_event *evt=0;
}
