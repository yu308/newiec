#include "../../Core/iec_cfg.h"
#include "iec_node.h"
#include "iec_element.h"
#include <time.h>

/** 
 *  �������л���Ϣ��
 * 
 * @param node_start_addr ��Ϣ���鿪ʼ��ַ 
 * @param count ��Ϣ������������
 * 
 * @return ���л���Ϣ��
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
/// ɾ��ĳ��ͨ����Ϣ��
/// </summary>
/// <param name="node">The node.</param>
void iec_del_seq_node(struct seq_node *node)
{
	rt_free(node);
}

/** 
 * ��װ��Ϣ���ַ
 * 
 * @param buff ���ݻ��� 
 * @param node_addr ��Ϣ���ַ
 * @param addr_len ��ַ����
 * 
 * @return ���ݳ���
 */
unsigned int iec_pack_node_addr(char *buff, unsigned int node_addr, int addr_len)
{
	rt_memcpy(&buff[0], &node_addr, addr_len);
	return addr_len;
}

/** 
 * ��װ��Ϣ��Ԫ������
 * 
 * @param buff  ���ݻ���
 * @param element_val  ����Ԫ��ֵ
 * @param element_ident ����Ԫ�ر�ʶ
 * 
 * @return ���ݳ���
 */
unsigned int iec_pack_node_element(char *buff, int element_val, int element_ident)
{
	rt_memcpy(&buff[0], &element_val, element_len_cfg[element_ident]);
	return element_len_cfg[element_ident];
}

/** 
 * ��װʱ����Ԫ������
 * 
 * @param buff ���ݻ���
 * @param utc_time UTCʱ��
 * @param millsecond ������
 * @param tm_ident ʱ���ʶ
 * 
 * @return ���ݳ���
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
 * ������Ϣ��
 * 
 * @param node_addr ��Ϣ���ַ
 * 
 * @return ��Ϣ��
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
/// �ⲿ�ӿ� ����ͨ����Ϣ��
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
/// �ⲿ�ӿ�  �������л���Ϣ��
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
 * ����һ����Ϣ��Ĵ���������Ϣ
 * 
 * @param node_addr ��ַ
 * @param val ֵ
 * @param qual ������
 * @param utc_time UTCʱ��
 * @param millsecond ������
 * @param buffered �Ƿ񻺴�
 * 
 * @return ����������Ϣ
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
 * ���л���Ϣ��Ĵ���������Ϣ
 * 
 * @param node_addr ��ʼ��ַ
 * @param count ����
 * @param val ֵ��
 * @param qual ��������
 * @param buffered �Ƿ񻺴�
 * 
 * @return ��Ϣ��������Ϣ
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
 * ������Ϣ��--- �û��ӿ�
 * 
 * @param appid appʵ��
 * @param level �������ȼ� ��Ӧһ������ ��������
 * @param asdu_ident ʹ�õ�ASDU��ʶ
 * @param cause ����ԭ��
 * @param f_node ��Ϣ�����ݴ�����Ϣ
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
* �������л���Ϣ������  --- �û�����
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
