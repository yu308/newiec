#include "../Core/core.h"
#include "iec.h"




struct node_obj *iec_create_node(unsigned int node_addr,int seq_count)
{
	struct node_obj *nd=rt_malloc(sizeof(struct node_obj));
	if(nd==0)
	{
		rt_kprintf("IEC:NODE: Created fail\n");
		return 0;
	}

	nd->addr=node_addr;
	nd->seq=seq_count;

	return nd;
}

/// <summary>
/// 删除某个信息点
/// </summary>
/// <param name="node">The node.</param>
void iec_del_node(struct node_obj *node)
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
unsigned int iec_pack_node_element(char *buff, void *element_val, int element_ident)
{
	rt_memcpy(&buff[0], element_val, element_len_cfg[element_ident]);
	return element_len_cfg[element_ident];
}




/**************** API ******************/
#include "../Core/core.h"
#include "../Layer/layer.h"

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
struct node_frame_info * iec_api_gen_node_info(unsigned int node_addr,int buffered)
{
	struct node_frame_info *f_node = rt_malloc(sizeof(struct node_frame_info));
	if (f_node == 0)
		return 0;

	rt_memset(f_node, 0, sizeof(struct node_frame_info));

	f_node->addr = node_addr;
	f_node->buffered = buffered;
    f_node->single=1;

	return f_node;
}

void iec_api_add_nodedata_to_node(struct node_frame_info *head_node,int node_addr,int node_addr_len)
{
    if (head_node!=0)
    {
        rt_memcpy(&head_node->byte_buff[head_node->data_len], &node_addr, node_addr_len);
        head_node->data_len+=node_addr_len;
        head_node->single+=1;
    }
}

void iec_api_add_element_to_node(struct node_frame_info *nd_info,int element_tag,void *el_val)
{
	nd_info->data_len+=iec_pack_node_element(&nd_info->byte_buff[nd_info->data_len],el_val,element_tag);
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
void iec_api_update_node(int appid,int level, unsigned int asdu_ident,
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


