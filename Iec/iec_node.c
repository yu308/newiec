#include "../../Core/iec_cfg.h"
#include "iec_node.h"
#include "iec_element.h"
#include <time.h>

/** 
 *  创建序列化信息点
 * 
 * @param node_start_addr 信息点组开始地址 
 * @param count 信息点组数据数量
 * 
 * @return 序列化信息点
 */
struct seq_node *iec_create_seq_node(int node_start_addr,int count)
{
	struct seq_node *node =rt_malloc(sizeof(struct seq_node));
	if (node != 0)
	{
		rt_memset(node, 0, sizeof(struct seq_node));
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
	rt_free(node);
}

/** 
 * 封装信息点地址
 * 
 * @param buff 数据缓存 
 * @param node_addr 信息点地址
 * @param addr_len 地址长度
 * 
 * @return 数据长度
 */
unsigned int iec_pack_node_addr(char *buff, unsigned int node_addr, int addr_len)
{
	rt_memcpy(&buff[0], &node_addr, addr_len);
	return addr_len;
}

/** 
 * 封装信息点元素数据
 * 
 * @param buff  数据缓存
 * @param element_val  数据元素值
 * @param element_ident 数据元素标识
 * 
 * @return 数据长度
 */
unsigned int iec_pack_node_element(char *buff, int element_val, int element_ident)
{
	rt_memcpy(&buff[0], &element_val, element_len_cfg[element_ident]);
	return element_len_cfg[element_ident];
}

/** 
 * 封装时间类元素数据
 * 
 * @param buff 数据缓存
 * @param utc_time UTC时间
 * @param millsecond 毫秒数
 * @param tm_ident 时间标识
 * 
 * @return 数据长度
 */
unsigned int iec_pack_tm_node_element(char *buff, int utc_time, int millsecond,int tm_ident)
{
	struct tm *t = gmtime(&utc_time);
	Info_E_TM56 info_tm56;
	Info_E_TM24 info_tm24;
	Info_E_TM16 info_tm16;

	rt_memset(&info_tm56, 0, 7);
	rt_memset(&info_tm24, 0, 3);
	rt_memset(&info_tm16, 0, 2);

	if (tm_ident == TM56)
	{
		info_tm56.YEAR = t->tm_year + 1900 - 2000;
		info_tm56.MONTH = t->tm_mon + 1;
		info_tm56.DOM = t->tm_mday;
		info_tm56.DOW = t->tm_wday + 1;
		info_tm56.HOUR = t->tm_hour;
		info_tm56.MIN = t->tm_min;
		info_tm56.M_SEC = t->tm_sec * 1000+millsecond;

		rt_memcpy(&buff[0], &info_tm56, element_len_cfg[tm_ident]);
	}
	else if (tm_ident == TM24)
	{
		info_tm24.MIN = t->tm_min;
		info_tm24.M_SEC = t->tm_sec * 1000 + millsecond;
		rt_memcpy(&buff[0], &info_tm24, element_len_cfg[tm_ident]);

	}
	else if (tm_ident == TM16)
	{
		info_tm16.M_SEC = t->tm_sec * 1000 + millsecond;
		rt_memcpy(&buff[0], &info_tm16, element_len_cfg[tm_ident]);
	}

	
	return element_len_cfg[tm_ident];
}

/** 
 * 创建信息点
 * 
 * @param node_addr 信息点地址
 * 
 * @return 信息点
 */
struct normal_node *iec_create_normal_node(int node_addr)
{
	struct normal_node *node = rt_malloc(sizeof(struct normal_node));
	if (node != 0)
	{
		rt_memset(node, 0, sizeof(struct normal_node));
		node->addr = node_addr;
	}

	return node;
}




/**************** API ******************/
#include "../Core/iec_event.h"
#include "../Layer/Helper/layer_helper.h"

/// <summary>
/// 外部接口 创建通用信息点
/// </summary>
/// <param name="appid">The appid.</param>
/// <param name="node_addr">The node_addr.</param>
void iec_api_create_normal_node(int appid, int node_addr)
{
	struct normal_node *node=iec_create_normal_node(node_addr);

	struct iec_event *evt=iec_create_event(0, appid, EVT_APP_ADD_NODE, 0,1);
	iec_set_event_sub(evt, EVT_SUB_NORMAL_NODE, (int *)node, 0);
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
	iec_set_event_sub(evt, EVT_SUB_SEQ_NODE, (int *)node, 0);
	iec_post_event(((struct app_info *)appid)->app_event, evt, 20);

}

/** 
 * 生成一个信息点的传输数据信息
 * 
 * @param node_addr 地址
 * @param val 值
 * @param qual 描述符
 * @param utc_time UTC时间
 * @param millsecond 毫秒数
 * @param buffered 是否缓存
 * 
 * @return 传输数据信息
 */
struct node_frame_info * iec_api_gen_normal_node_data(unsigned int node_addr,int val, unsigned int qual, unsigned int utc_time,int millsecond,int buffered)
{
	struct node_frame_info *f_node = rt_malloc(sizeof(struct node_frame_info));
	if (f_node == 0)
		return 0;

	rt_memset(f_node, 0, sizeof(struct node_frame_info));

	f_node->val = val;
	f_node->qual = qual;
	f_node->utc_time = utc_time;
	f_node->addr = node_addr;
	f_node->buffered = buffered;
	f_node->millsecond = millsecond;

	return f_node;
}

/** 
 * 序列化信息点的传输数据信息
 * 
 * @param node_addr 开始地址
 * @param count 数量
 * @param val 值组
 * @param qual 描述符组
 * @param buffered 是否缓存
 * 
 * @return 信息点数据信息
 */
struct seq_node_frame_info *iec_api_gen_seq_node_data(unsigned int node_addr, int count, int *val, int *qual, int buffered)
{
	struct seq_node_frame_info *f_s_node = rt_malloc(sizeof(struct seq_node_frame_info));
	if (f_s_node == 0)
		return 0;

	rt_memset(f_s_node, 0, sizeof(struct seq_node_frame_info));

	f_s_node->val = val;
	f_s_node->qual = qual;
	f_s_node->addr = node_addr;
	f_s_node->buffered = buffered;
	f_s_node->count = count;


	return f_s_node;
}


/** 
 * 更新信息点--- 用户接口
 * 
 * @param appid app实例
 * @param level 数据优先级 对应一级数据 二级数据
 * @param asdu_ident 使用的ASDU标识
 * @param cause 传送原因
 * @param f_node 信息点数据传输信息
 */
void iec_api_update_normal_node(int appid,int level, unsigned int asdu_ident,
	unsigned int cause,struct node_frame_info *f_node)
{
	struct node_update_info *info = rt_malloc(sizeof(struct node_update_info));

	if (info == 0)
		return;

	rt_memset(info, 0, sizeof(struct node_update_info));

	info->appid = appid;
	info->asdu_ident = asdu_ident;
	info->cause = cause;
	info->level = level;

	struct iec_event *evt = iec_create_event(0, appid, EVT_APP_NODE_UPDATE, (int*)info, 1);
	iec_set_event_sub(evt, EVT_SUB_NORMAL_NODE, (int *)f_node, 1);
	iec_post_event(((struct app_info *)appid)->app_event, evt, 20);

}

/*
* 更新序列化信息点数据  --- 用户数据
*
*/
void iec_api_update_seq_node(int appid, int level, unsigned int asdu_ident, unsigned int cause, struct seq_node_frame_info *f_s_node)
{
	struct node_update_info *info = rt_malloc(sizeof(struct node_update_info));

	if (info == 0)
		return;

	rt_memset(info, 0, sizeof(struct node_update_info));

	info->appid = appid;
	info->asdu_ident = asdu_ident;
	info->cause = cause;
	info->level = level;

	struct iec_event *evt = iec_create_event(0, appid, EVT_APP_NODE_UPDATE, (int*)info, 1);
	iec_set_event_sub(evt, EVT_SUB_SEQ_NODE, (int *)f_s_node, 1);
	iec_post_event(((struct app_info *)appid)->app_event, evt, 20);
}
