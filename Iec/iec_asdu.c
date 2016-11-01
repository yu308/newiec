#include "iec.h"


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


