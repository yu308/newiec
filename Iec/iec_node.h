#ifndef _iec_node_h_
#define _iec_node_h_

#include "../Core/core.h"

//struct file_node 文件操作

/// <summary>
/// EVENT中信息点的表现形式
/// </summary>
struct node_frame_info
{
	unsigned int addr;
	unsigned int vsq;
    unsigned int single;
	int buffered;
	char byte_buff[CFG_ASDU_DATA_BUFF_MAX];
	int data_len;
};



#endif
