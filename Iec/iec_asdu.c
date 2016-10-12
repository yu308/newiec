#include "iec_event.h"
#include "iec_element.h"
#include "iec_asdu.h"
#include "iec_asdu_table.h"


struct asdu_cfg* iec_get_asdu_cfg(int ident)
{
	int i = 0;
	int asdu_count = sizeof(g_SysASDU_Table) / sizeof(struct asdu_cfg);

	for (i = 0; i < asdu_count; i++)
	{
		if (g_SysASDU_Table[i].asdu_ident == ident)
			return &g_SysASDU_Table[i];
	}

	return 0;
}

static unsigned int asdu_pack_node(char *buff, struct asdu_cfg *cfg, int node_addr,int node_addr_len,int seq_flag)
{
	int idx = 0;

	if (seq_flag == 0)
	{
		XMEMCPY(&buff[idx], &node_addr, node_addr_len);//信息点地址
		idx += node_addr_len;
	}

	if (cfg->val_ident != 0)
	{
		
	}

}

/// <summary>
/// 封装ASDU数据
/// </summary>
/// <param name="buff">缓存区</param>
/// <param name="asdu_ident">ASDU标识</param>
/// <param name="node_array">信息点地址组</param>
/// <param name="node_count">信息点组中的数量</param>
/// <returns>封装的数据长度</returns>
unsigned int iec_asdu_pack_node_data(char *buff, int asdu_ident, int node_array, int node_count)
{
	struct asdu_cfg *cfg = iec_get_asdu_cfg(asdu_ident);
	int i = 0;

	for (i = 0; i < node_count; i++)
	{

	}
}