#ifndef _iec_helper_h_
#define _iec_helper_h_

#include "iec_asdu.h"
#include "iec_asdu_table.h"
#include "iec_element.h"
#include "iec_node.h"

unsigned int iec_pack_node_addr(char *buff, unsigned int node_addr, int addr_len);
unsigned int iec_pack_node_element(char *buff, void *element_val, int element_ident);



extern struct node_frame_info * iec_api_gen_node_info(unsigned int node_addr,int buffered);

extern void iec_api_add_nodedata_to_node(struct node_frame_info *head_node,int node_addr,int node_addr_len);

extern void iec_api_add_element_to_node(struct node_frame_info *nd_info,int element_tag,void *el_val);

extern void iec_api_update_node(unsigned int appid,int level, unsigned int asdu_ident,
                         unsigned int cause,struct node_frame_info *f_node);
#endif
