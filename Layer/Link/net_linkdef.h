#ifndef _net_link_def_h_
#define _net_link_def_h_


#include "link.h"

struct net_link_cfg
{
	int socket;
	
	unsigned int t0;
	unsigned int t1;
	unsigned int t2;
	
	unsigned int K;
	unsigned int W;
};

struct net_link_info
{
	struct link_obj obj;
	struct net_link_cfg cfg;
	
	int appid;
};

#endif