#ifndef _iec_node_h_
#define _iec_node_h_

union node_val
{
	int int_val;		/*整形*/
	float float_val;	/*浮点*/
	char iarray_val[4];		/*字符*/
	volatile unsigned bit_val : 32;	/*位串*/
};


/// <summary>
/// 信息点
/// </summary>
struct normal_node
{
	int addr;	/* 信息点地址 */
	union node_val val; /*信息点值*/

	int buffered;		/*是否缓存数据*/
	int asdu_ident;		/*发送数据时采用ASDU类型*/
};

/// <summary>
/// 序列化信息点组
/// </summary>
struct seq_node
{
	int start_addr;
	int count;
	
	union node_val val_array[24];

	int buffered;
	int asdu_ident;
};

//struct file_node 文件操作


#endif