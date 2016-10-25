
#ifndef _layer_helper_h_
#define _layer_helper_h_

#include "../../Core/iec_cfg.h"
#include "../../Core/iec_event.h"

#include "../Helper/arraylist.h"

#include "../App/app.h"
#include "../App/app_task.h"

#include "../Link/link.h"
#include "../Link/serial_linkdef.h"

extern void serial_link_send_req_evt_to_app(struct serial_link_info *info,int sub_evt);

extern void serial_link_send_asdu_evt_to_app(struct serial_link_info *info,char *asdu_data);

extern void app_send_update_evt_to_link(struct app_info *info,struct serial_link_info *link_info, int sub_evt);
  
extern void app_send_asdu_evt_to_link(struct app_info *info,struct app_send_info *send_asdu);
extern void app_send_userdata_ack_evt_to_link(struct app_info *info,struct serial_link_info *link_info);
#endif
