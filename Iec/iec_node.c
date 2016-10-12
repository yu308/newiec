#include "iec_cfg.h"
#include "iec_node.h"

struct seq_node *iec_create_seq_node(int node_start_addr)
{
	struct seq_node *node = XMALLOC(sizeof(struct seq_node));
	if (node != 0)
	{
		XMEMSET(node, 0, sizeof(struct seq_node));
		node->start_addr = node_start_addr;
	}

	return node;
}

void iec_update_seq_data(struct seq_node *node, int idx, int *value)
{
	node->val_array[idx].int_val = (*value);
}

void iec_update_seq_alldata(struct seq_node *node, int **value)
{
	int i = 0;
	for (i = 0; i < node->count; i++)
	{
		iec_update_seq_data(node, i,&value[i]);
	}
}

void iec_del_seq_node(struct seq_node *node)
{
	XFREE(node);
}

struct normal_node *iec_create_normal_node(int node_addr)
{
	struct normal_node *node = XMALLOC(sizeof(struct normal_node));
	if (node != 0)
	{
		XMEMSET(node, 0, sizeof(struct normal_node));
		node->addr = node_addr;
	}

	return node;
}

void iec_update_normal_node(struct normal_node *node, int *value)
{
	node->val.int_val = (*value);

}

void iec_del_normal_node(struct normal_node *node)
{
	XFREE(node);
}

