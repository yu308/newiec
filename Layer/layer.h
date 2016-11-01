
#ifndef _layer_helper_h_
#define _layer_helper_h_

#include    "../Core/core.h"

#include "App/app.h"
#include "App/app_task.h"

#include "Link/link.h"
#include "Link/serial_linkdef.h"
#include "Link/net_linkdef.h"

extern void link_send_req_evt_to_app(struct link_obj *info,int sub_evt);

extern void link_send_asdu_evt_to_app(struct link_obj *link,char *asdu_data,int asdu_len);

extern void app_send_update_evt_to_link(struct app_info *info,struct link_obj *link, int sub_evt);

extern void app_send_asdu_evt_to_link(struct app_info *info,struct app_send_info *send_asdu);
extern void app_send_userdata_ack_evt_to_link(struct app_info *info,struct link_obj *link);
#endif
