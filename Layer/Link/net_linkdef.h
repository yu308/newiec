#ifndef _net_link_def_h_
#define _net_link_def_h_


#include "link.h"

struct net_link_cfg
{
	int socket;

};

struct net_link_info
{
	struct link_obj obj;
	struct net_link_cfg cfg;
	
	int appid;
};

#endif
