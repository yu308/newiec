#include "iec_event.h"
#include "iec_element.h"
#include "iec_asdu.h"
#include "iec_asdu_table.h"

/** 
 *  查找对应标识的ASDU配置信息 
 * 
 * @param ident  ASDU标识
 * 
 * @return ASDU配置信息
 */
struct asdu_cfg* iec_get_asdu_cfg(int ident)
{
	int i = 0;
	int asdu_count = sizeof(g_SysASDU_Table) / sizeof(struct asdu_cfg);

	for (i = 0; i < asdu_count; i++)
	{
		if (g_SysASDU_Table[i].asdu_ident == ident)
			return (struct asdu_cfg *)&g_SysASDU_Table[i];
	}

	return 0;
}

/** 
 * 封装信息点数据
 * 
 * @param buff 数据缓存 
 * @param cfg ASDU配置信息
 * @param node_addr 信息点地址
 * @param node_addr_len 信息点地址长度
 * @param val 信息点的值
 * @param qual 信息点值描述符
 * @param utc_time  utc时间
 * @param millsecond 毫秒计数 0-999
 * 
 * @return 信息点数据的长度
 */
unsigned int iec_asdu_pack_node(char *buff, struct asdu_cfg *cfg,int node_addr,int node_addr_len,int val,int qual,int utc_time,
	int millsecond)
{
	int idx = 0;

	idx += iec_pack_node_addr(buff, node_addr, node_addr_len);

	

	return idx;
}

/** 
 * 封装序列化信息点数据
 * 
 * @param buff 数据缓存
 * @param cfg ASDU配置信息
 * @param node_addr 信息点地址
 * @param node_addr_len 信息点地址长度
 * @param count 当前序列化信息点包含数据数量
 * @param val 数据数组
 * @param qual 描述符数据组
 * 
 * @return 封装的数据长度
 */
unsigned int iec_asdu_pack_seq_node(char *buff, struct asdu_cfg *cfg, int node_addr, int node_addr_len,int count,int *val, int *qual)
{
  /*
	int idx = 0,i=0;

	idx += iec_pack_node_addr(buff, node_addr, node_addr_len);

	while (count--)
	{
		if (cfg->val_ident != 0)
		{
			idx += iec_pack_node_element(&buff[idx], val[i], cfg->val_ident);
		}
		if (cfg->qual_ident != 0)
		{
			idx += iec_pack_node_element(&buff[idx], qual[i], cfg->qual_ident);
		}

		i++;
	}

	return idx;
*/
}
