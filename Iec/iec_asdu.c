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

unsigned int iec_asdu_pack_node(char *buff, struct asdu_cfg *cfg,int node_addr,int node_addr_len,int val,int qual,int utc_time,
	int millsecond)
{
	int idx = 0;

	idx += iec_pack_node_addr(buff, node_addr, node_addr_len);

	if (cfg->val_ident != 0)
	{
		idx+=iec_pack_node_element(&buff[idx], val, cfg->val_ident);
	}
	if (cfg->qual_ident != 0)
	{
		idx += iec_pack_node_element(&buff[idx], qual, cfg->qual_ident);
	}
	if (cfg->tm_ident != 0)
	{
		idx += iec_pack_tm_node_element(&buff[idx], utc_time, millsecond, cfg->tm_ident);
	}
	if (cfg->ext_tm_ident != 0)
	{
		idx += iec_pack_tm_node_element(&buff[idx], utc_time, millsecond, cfg->ext_tm_ident);
	}

	return idx;
}

unsigned int iec_asdu_pack_seq_node(char *buff, struct asdu_cfg *cfg, int node_addr, int node_addr_len,int count,int *val, int *qual)
{
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
}