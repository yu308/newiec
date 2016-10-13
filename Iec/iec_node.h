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
	//union node_val val; /*信息点值*/

	//int buffered;		/*是否缓存数据*/
	//int asdu_ident;		/*发送数据时采用ASDU类型*/
	struct normal_node *next;
};

/// <summary>
/// 序列化信息点组
/// </summary>
struct seq_node
{
	int start_addr;
	int count;
	//
	//union node_val val_array[24];

	//int buffered;
	//int asdu_ident;
};

//struct file_node 文件操作

struct node_frame_info
{
	int val;
	int qual;
	unsigned int utc_time;

};


extern struct seq_node *iec_create_seq_node(int node_start_addr);
extern void iec_del_seq_node(struct seq_node *node);
extern unsigned int iec_pack_node_addr(char *buff, unsigned int node_addr, int addr_len);
extern unsigned int iec_pack_node_element(char *buff, int element_val, int element_ident);
extern struct normal_node *iec_create_normal_node(int node_addr);
extern void iec_del_normal_node(struct normal_node *node);


#endif