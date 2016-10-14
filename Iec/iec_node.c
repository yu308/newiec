#include "iec_cfg.h"
#include "iec_node.h"



struct seq_node *iec_create_seq_node(int node_start_addr,int count)
{
	struct seq_node *node = XMALLOC(sizeof(struct seq_node));
	if (node != 0)
	{
		XMEMSET(node, 0, sizeof(struct seq_node));
		node->start_addr = node_start_addr;
		node->count = count;
	}

	return node;
}

/// <summary>
/// 删除某个通用信息点
/// </summary>
/// <param name="node">The node.</param>
void iec_del_seq_node(struct seq_node *node)
{
	XFREE(node);
}


unsigned int iec_pack_node_addr(char *buff, unsigned int node_addr, int addr_len)
{
	XMEMCPY(&buff[0], &node_addr, addr_len);
	return addr_len;
}

unsigned int iec_pack_node_element(char *buff, int element_val, int element_ident)
{
	XMEMCPY(&buff[0], &element_val, element_len_cfg[element_ident]);
	return element_len_cfg[element_ident];
}


struct normal_node *iec_create_normal_node(int node_addr)
{
	struct normal_node *node = XMALLOC(sizeof(struct normal_node));
	if (node != 0)
	{
		XMEMSET(node, 0, sizeof(struct normal_node));
		node->addr = node_addr;
	}

	return node;
}




/**************** API ******************/
#include "iec_event.h"
#include "app.h"

/// <summary>
/// 外部接口 创建通用信息点
/// </summary>
/// <param name="appid">The appid.</param>
/// <param name="node_addr">The node_addr.</param>
void iec_api_create_normal_node(int appid, int node_addr)
{
	struct normal_node *node=iec_create_normal_node(node_addr);

	struct iec_event *evt=iec_create_event(0, appid, EVT_APP_ADD_NODE, 0,1);
	iec_set_event_sub(evt, EVT_APP_SUB_NORMAL_NODE, (int *)node, 0);
	iec_post_event(((struct app_info *)appid)->app_event, evt, 20);

}

/// <summary>
/// 外部接口  创建序列化信息点
/// </summary>
/// <param name="appid">The appid.</param>
/// <param name="node_addr">The node_addr.</param>
/// <param name="count">The count.</param>
void iec_api_create_seq_node(int appid, int node_addr, int count)
{
	struct seq_node *node = iec_create_seq_node(node_addr, count);

	struct iec_event *evt = iec_create_event(0, appid, EVT_APP_ADD_NODE, 0, 1);
	iec_set_event_sub(evt, EVT_APP_SUB_SEQ_NODE, (int *)node, 0);
	iec_post_event(((struct app_info *)appid)->app_event, evt, 20);

}


struct node_frame_info * iec_api_gen_node_data(unsigned int node_addr,int val, unsigned int qual, unsigned int utc_time,int buffered)
{
	struct node_frame_info *f_node = XMALLOC(sizeof(struct node_frame_info));
	if (f_node == 0)
		return 0;

	XMEMSET(f_node, 0, sizeof(struct node_frame_info));

	f_node->val = val;
	f_node->qual = qual;
	f_node->utc_time = utc_time;
	f_node->addr = node_addr;
	f_node->buffered = buffered;


	return f_node;
}


void iec_api_update_normal_node(int appid,int level, unsigned int asdu_ident,
	unsigned int cause,int seq,struct node_frame_info *f_node)
{
	struct normal_node_update_info *info = XMALLOC(sizeof(struct normal_node_update_info));

	if (info == 0)
		return;

	XMEMSET(info, 0, sizeof(struct normal_node_update_info));

	info->appid = appid;
	info->asdu_ident = asdu_ident;
	info->cause = cause;
	info->seq = seq;
	info->level = level;

	struct iec_event *evt = iec_create_event(0, appid, EVT_APP_NODE_UPDATE, (int*)info, 1);
	iec_set_event_sub(evt, EVT_APP_SUB_NORMAL_NODE, (int *)f_node, 0);
	iec_post_event(((struct app_info *)appid)->app_event, evt, 20);

}


