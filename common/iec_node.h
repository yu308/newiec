#ifndef _iec_node_h_
#define _iec_node_h_

#include "iec_type.h"
#include "iec_object.h"
#include "iec_def.h"

typedef struct _node  Node;

/****************** Base Def************************/
enum Node_Type
{
    NT_Ctrl,
    NT_Param,
    NT_Info,
    NT_System,
};


struct _node
{
	Object obj;

    Node_Addr  info_addr;

    uint32 val_count;

  Object node_val_head;

};
/*******************************************/



/******************Control Node************************/
typedef void (*ctrl_action)(int8);
typedef int8 (*ctrl_check)(int8);

struct ctrl_node
{
    struct _node base;


    ctrl_action action;
    ctrl_check  check;

};
/*******************************************/



/*********************Param Node ******************/
struct param_configure
{
    uint32 param_val;
};

typedef void (*param_set)(struct param_configure);
typedef struct param_configure (*param_get)();

struct param_node
{
    struct _node base;

    param_set set;
    param_get get;
};
/************************************************/


/********************Information Node***************************/
union info_configure
{
    uint32 ival;
    float fval;
};

typedef int32 (*get_val)();
typedef void (*upload_info_node)(int *param);
typedef void (*update_info_node)();


#define SINGLE_INFO     0           //遥信点
#define MS_SVA_INFO     1           //标度化 遥测点
#define MS_NVA_INFO     2           //归一化 遥测点
#define MS_STD_INFO     3           //短浮点 遥测点

struct info_node
{
    struct _node base;

    struct param_node *limit_config;

    union info_configure last_val;
    
    uint8  sub_type;

    get_val get;
    
    update_info_node update;
    upload_info_node upload;

	rt_uint32_t updated_lock;
};

/*********************************************/
extern void node_addr_init(Node_Addr *n_addr,uint32 addr,int count);

extern void info_node_init(struct info_node *inode_head, int *param);
extern void info_node_update_monitor(struct info_node *inode_head,int *param);

extern struct ctrl_node *iec_ctrl_node_create(struct ctrl_node *cnode_head,uint8 *name,uint32 addr,int count);
extern struct ctrl_node * iec_ctrl_node_find(struct ctrl_node *cnode_head,Node_Addr addr,int count);
extern void iec_ctrl_node_add_child(struct ctrl_node *node, enum Element_Ident ident);
extern void iec_ctrl_node_set_check_callback(struct ctrl_node *node,ctrl_check cb_check);
extern void iec_ctrl_node_set_action_callback(struct ctrl_node *node,ctrl_action cb_action);

extern Object *iec_node_element_find(struct _node *node,enum Element_Ident ident);


extern struct info_node *iec_info_node_create(struct info_node *inode_head,uint8 *name,uint32 addr,uint8 subtype,int count);
extern void iec_info_node_set_get_callback(struct info_node *node,get_val cb_get);
extern void iec_info_node_set_update_callback(struct info_node *node,update_info_node cb_update);
extern void iec_info_node_set_upload_callback(struct info_node *node,upload_info_node cb_upload);
extern void iec_info_node_set_limit(struct info_node *node,struct param_node *limit_param);
extern struct info_node * iec_info_node_find(struct info_node *inode_head,Node_Addr addr,int count);
extern void iec_info_node_add_child(struct info_node *node,enum Element_Ident ident);


extern struct param_node *iec_param_node_create(struct param_node *pnode_head, uint8 *name, uint32 addr, int count);
extern void iec_param_node_set_callback(struct param_node *node, param_set set);
extern void iec_param_node_get_callback(struct param_node *node, param_get get);
extern  void iec_param_node_add_child(struct param_node *node, enum Element_Ident ident);

#endif
