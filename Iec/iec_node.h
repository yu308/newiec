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
/// ��Ϣ��
/// </summary>
struct normal_node
{
	int addr;	/* ��Ϣ���ַ */
	union node_val val; /*��Ϣ��ֵ*/

	int buffered;		/*�Ƿ񻺴�����*/
	int asdu_ident;		/*��������ʱ����ASDU����*/
};

/// <summary>
/// ���л���Ϣ����
/// </summary>
struct seq_node
{
	int start_addr;
	int count;
	
	union node_val val_array[24];

	int buffered;
	int asdu_ident;
};

//struct file_node �ļ�����


#endif