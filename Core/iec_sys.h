#ifndef _iec_sys_h_
#define _iec_sys_h_
#include "iec_cfg.h"

struct sys_info
{
	int communicate_role;

	int link_obj[];
	int app_obj[];
};

#endif