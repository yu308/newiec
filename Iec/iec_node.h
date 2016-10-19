#ifndef _iec_node_h_
#define _iec_node_h_

union node_val
{
	int int_val;		/*����*/
	float float_val;	/*����*/
	char iarray_val[4];		/*�ַ�*/
	volatile unsigned bit_val : 32;	/*λ��*/
};


/// <summary>
/// ��Ϣ�� ����APPģ����֧�ֵ���Ϣ��
/// </summary>
struct normal_node
{
	int addr;	/* ��Ϣ���ַ */
	//union node_val val; /*��Ϣ��ֵ*/

	//int buffered;		/*�Ƿ񻺴�����*/
	//int asdu_ident;		/*��������ʱ����ASDU����*/
};

/// <summary>
/// ���л���Ϣ���� ����APPģ����֧�ֵ���Ϣ��
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

//struct file_node �ļ�����

/// <summary>
/// EVENT����Ϣ��ı�����ʽ
/// </summary>
struct node_frame_info
{
	unsigned int addr;
	int val;
	int qual;
	unsigned int utc_time;
	int millsecond;
	int buffered;

};

struct seq_node_frame_info
{
	unsigned int addr;
	int count;
	int *val;
	int *qual;
	int buffered;
};

extern struct seq_node *iec_create_seq_node(int node_start_addr,int count);
extern void iec_del_seq_node(struct seq_node *node);
extern unsigned int iec_pack_node_addr(char *buff, unsigned int node_addr, int addr_len);
extern unsigned int iec_pack_node_element(char *buff, int element_val, int element_ident);
extern struct normal_node *iec_create_normal_node(int node_addr);
void iec_api_update_normal_node(int appid,int level, unsigned int asdu_ident,
                                unsigned int cause,struct node_frame_info *f_node);

extern unsigned int iec_pack_tm_node_element(char *buff, int utc_time, int millsecond, int tm_ident);

#endif
