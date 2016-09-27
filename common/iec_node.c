#include "../common/iec.h"

/**
 * @brief �ȶ�2����ϢԪ�ص�ַ
 * @param addr1  ��ַ1
 * @param addr2  ��ַ2
 * @return  1 ��ͬ  0 ��ͬ
 */

static uint8 node_compare_addr(Node_Addr addr1,Node_Addr addr2,int count)
{
 return byte_is_equal(&addr1, &addr2, count);
}

void node_addr_init(Node_Addr *n_addr,uint32 addr,int count)
{
  byte_set_val(n_addr, addr, count);
}

/**
 * @brief ������ϢԪ�ص�ַ
 * @param node  ��ϢԪ��
 * @param addr  ��ַ
 */

static void node_set_addr(struct _node *node,uint32 addr,int count)
{
  node_addr_init(&node->info_addr,addr,count);
}

/**
 * @brief ���һ����ϢԪ��
 * @param head  ��Ϣͷ
 * @param enode Ԫ����
 */

static void node_add(struct _node *head,struct _node *enode)
{
    struct _node *node=head;

    while(node->obj.next!=0)
    {
        node=(struct _node *)node->obj.next;
    }

    node->obj.next=(Object *)enode;
}

/**
 * @brief ����ĳ����ϢԪ��
 * @param head  ͷ
 * @param addr ��ϢԪ�ص�ַ
 * @return ��ϢԪ��  0 δ�ҵ�
 */

static struct _node* node_find(struct _node *head,Node_Addr addr,int count)
{
    struct _node *node=(struct _node *)head->obj.next;

    while(node!=0)
    {
      if(node_compare_addr(node->info_addr,addr,count))
            return node;

        node=(struct _node *)node->obj.next;
    }

    return 0;
}

/**
 * @brief ��ʼ��ĳ����ϢԪ��
 * @param node  ��ϢԪ��
 * @param name  ����
 * @param type  ��ϢԪ������
 * @param addr  ��ϢԪ�ص�ַ
 */

static void node_base_init(struct _node *node,uint8 *name,uint8 type,uint32 addr,int count)
{
    node->obj.id=(uint32)node;

    iec_memset(&node->obj.name,0,sizeof(node->obj.name));
    iec_memcpy(&node->obj.name,name,iec_strlen(name));

    node->obj.obj_type=type;
    node->obj.next=0;
    node->obj.prev=0;
    node->val_count=0;
    iec_memset(&node->node_val_head,0,sizeof(Object));

    node_set_addr(node,addr,count);
}

/**
 * @brief ���һ������ϢԪ��
 * @param node  ����ϢԪ����
 * @param ident  ��Ϣ���ͱ�ʶ
 */

static void node_add_child(struct _node *node,enum Element_Ident ident)
{
    Object *eobj=&node->node_val_head;

    Object *eobj1=(Object *)iec_element_create(ident);

    if(eobj1==0)
        return ;

    while(eobj->next!=0)
    {
        eobj=eobj->next;
    }

    node->val_count++;
    eobj->next=eobj1;

}

/**
 * @brief �����ض�������ϢԪ��
 * @param node  ����ϢԪ��
 * @param ident ����ϢԪ�ر�ʶ
 * @return  >0 ��Ԫ��  =0 δ�ҵ�
 */

Object *iec_node_element_find(struct _node *node,enum Element_Ident ident)
{
    uint32 i=0;

    Object *obj=node->node_val_head.next;

    while(obj!=0&&(i<node->val_count))
    {
        if(obj->obj_type==ident)
            return obj;

        obj=obj->next;
        i++;
    }

    return 0;
}




/**
 * @brief ������ϢԪ�س�ʼ��
 * @param name  ����
 * @param addr  ��Ϣ��ַ
 * @return  >0 ������ϢԪ��  =0 δ����
 */

struct ctrl_node *iec_ctrl_node_create(struct ctrl_node *cnode_head,uint8 *name,uint32 addr,int count)
{
  struct ctrl_node *node=iec_malloc(sizeof(struct ctrl_node));

    if(node==0)
        return 0;

    node_base_init(&node->base,name,NT_Ctrl,addr,count);
    node_add(&cnode_head->base,&node->base);

    node->action=0;
    node->check=0;
    
    return node;
}

/**
 * @brief ���ÿ��Ƶ�Ԥ���ƻص�
 * @param node  ���Ƶ�
 * @param cb_check  �ص�����
 */

void iec_ctrl_node_set_check_callback(struct ctrl_node *node,ctrl_check cb_check)
{
    node->check=cb_check;
}

/**
 * @brief ���ÿ��Ƶ㶯���ص�
 * @param node
 * @param cb_action
 */

void iec_ctrl_node_set_action_callback(struct ctrl_node *node,ctrl_action cb_action)
{
    node->action=cb_action;
}

/**
 * @brief ���һ������Ϣ��
 * @param node
 * @param ident
 */

void iec_ctrl_node_add_child(struct ctrl_node *node,enum Element_Ident ident)
{
    node_add_child(&node->base,ident);
}

struct ctrl_node * iec_ctrl_node_find(struct ctrl_node *cnode_head,Node_Addr addr,int count)
{
  return (struct ctrl_node *)node_find(&cnode_head->base,addr,count);
}



/**
 * @brief ����һ����������Ϣ��Ԫ��
 * @param name
 * @return 
 */

struct param_node *iec_param_node_create(struct param_node *pnode_head,uint8 *name,uint32 addr,int count)
{
  struct param_node *node=iec_malloc(sizeof(struct param_node));

    if(node==0)
        return 0;

    node_base_init(&node->base,name,NT_Param,addr,count);
    node_add(&pnode_head->base,&node->base);

    node->get=0;
    node->set=0;

    return node;
}

void iec_param_node_set_callback(struct param_node *node,param_set set)
{
    node->set=set;
}

void iec_param_node_get_callback(struct param_node *node,param_get get)
{
    node->get=get;
}
void iec_param_node_add_child(struct param_node *node, enum Element_Ident ident)
{
	node_add_child(&node->base, ident);
}


/**
 * @brief ����ң��,ң������ϢԪ��
 * @param name
 * @param addr
 * @param subtype  �������
 * @return 
 */

struct info_node *iec_info_node_create(struct info_node *inode_head,uint8 *name,uint32 addr,uint8 subtype,int count)
{
  struct info_node *node=iec_malloc(sizeof(struct info_node));

    if(node==0)
        return 0;

	iec_memset((rt_uint8_t *)node, 0, sizeof(struct info_node));
    node_base_init(&node->base,name,NT_Info,addr,count);
    node_add(&inode_head->base,&node->base);

    node->update=0;
    node->limit_config=0;
    node->last_val.ival=0;
    node->sub_type=subtype;
    node->upload=0;
  

    return node;
}

void iec_info_node_set_get_callback(struct info_node *node,get_val cb_get)
{
    node->get=cb_get;
}

void iec_info_node_set_update_callback(struct info_node *node,update_info_node cb_update)
{
    node->update=cb_update;
}

void iec_info_node_set_upload_callback(struct info_node *node,upload_info_node cb_upload)
{
    node->upload=cb_upload;
}

void iec_info_node_set_limit(struct info_node *node,struct param_node *limit_param)
{
    node->limit_config=limit_param;
}


void iec_info_node_add_child(struct info_node *node,enum Element_Ident ident)
{
    node_add_child(&node->base,ident);
}


struct info_node * iec_info_node_find(struct info_node *inode_head,Node_Addr addr,int count)
{
  return (struct info_node *)node_find(&inode_head->base,addr,count);
}
/**
 * @brief ����ϵͳ��ϢԪ�ص�����,�����û��Զ�����»ص�����
 * @param node
 */

static void info_node_high_update(struct info_node *node)
{
    struct info_element *e=0;
    if(node->sub_type==MS_NVA_INFO)
    {
        e=(struct info_element *)iec_node_element_find(&node->base,NVA);
        iec_memcpy(e->val,&node->last_val.ival,sizeof(Info_E_NVA));

    }
    else if(node->sub_type==MS_SVA_INFO)
    {
        e=(struct info_element *)iec_node_element_find(&node->base,SVA);
        iec_memcpy(e->val,&node->last_val.ival,sizeof(Info_E_SVA));
    }
    else if(node->sub_type==MS_STD_INFO)
    {
        e=(struct info_element *)iec_node_element_find(&node->base,STD);
        iec_memcpy(e->val,&node->last_val.fval,sizeof(Info_E_STD));
    }
    else if(node->sub_type==SINGLE_INFO)
    {
       e=(struct info_element *)iec_node_element_find(&node->base,SIQ);
       iec_memcpy(e->val,&node->last_val.fval,sizeof(Info_E_SIQ));
    }
    
    if(node->update!=0)
    node->update();
}

/**
 * @brief ϵͳ������
 * @param node
 */

static uint8 info_node_low_update(struct info_node *node)
{
    uint8 state=0;
    union info_configure  val;
    union info_configure  c_val;
    struct param_node *pnode=0;
	struct param_configure param_val;
    struct info_element *e=0;
    Info_E_QPM qpm;

    val.ival=node->get();

    if(node->limit_config!=0) //ң��
    {
        pnode=node->limit_config;
		

        e=(struct info_element *)iec_node_element_find(&pnode->base,QPM);
        iec_memcpy(&qpm,e->val,sizeof(Info_E_QPM));

        if(node->sub_type==MS_NVA_INFO)
        {
            e=(struct info_element *)iec_node_element_find(&pnode->base,NVA);
            iec_memcpy(&c_val.ival,e->val,sizeof(Info_E_NVA));

        }
        else if(node->sub_type==MS_SVA_INFO)
        {
            e=(struct info_element *)iec_node_element_find(&pnode->base,SVA);
            iec_memcpy(&c_val.ival,e->val,sizeof(Info_E_SVA));
        }
        else if(node->sub_type==MS_STD_INFO)
        {
            e=(struct info_element *)iec_node_element_find(&pnode->base,STD);
            iec_memcpy(&c_val.fval,e->val,sizeof(Info_E_STD));
        }

        if(qpm.KPA==LIMIT)
        {
			if (pnode->get != 0)
			{
				param_val.param_val = pnode->get().param_val;
				c_val.ival = param_val.param_val;
			}
			else
			{
				c_val.ival = 5;
			}

            if(node->sub_type==MS_STD_INFO&&(abs(val.fval-node->last_val.fval)>=c_val.fval))
                state=1;
            else if(abs(val.ival-node->last_val.ival)>=c_val.ival)
                state=1;
        }
        else if(qpm.KPA==FILTER_FACTOR)
        {
            state=1;
        }
        else if(qpm.KPA==MV_UP_LIMIT)
        {
            if(node->sub_type==MS_STD_INFO&&val.fval>c_val.fval)
                state=1;
            else if(val.ival>c_val.ival)
                state=1;
        }
        else if(qpm.KPA==MV_DW_LIMIT)
        {
            if(node->sub_type==MS_STD_INFO&&val.fval<c_val.fval)
                state=1;
            else if(val.ival<c_val.ival)
                state=1;
        }

    }
    else //ң��
    {
		
        if(val.ival!=node->last_val.ival)
        {
			if (node->updated_lock == 0)
			{
				node->updated_lock = 1;
				state = 1;
			}
        }
    }

    if(state==1)
    {
        node->last_val.ival=val.ival;
    }

    return state;
}

/**
 * @brief ϵͳ��ϢԪ�ص����,��������,�ϴ�����
 */

void info_node_update_monitor(struct info_node *inode_head,int *param)
{
    struct info_node *node=(struct info_node *)(inode_head->base.obj.next);

    while(node!=0)
    {
        if(info_node_low_update(node))
        {
            info_node_high_update(node);
            if(node->upload!=0)
            {
              node->upload(param);
            }
        }
        node=(struct info_node *)node->base.obj.next;
    }

}

/*
	��Ϣ���ʼ�� ����ֵ
*/
void info_node_init(struct info_node *inode_head,int *param)
{
   struct info_node *node=(struct info_node *)(inode_head->base.obj.next);

    while(node!=0)
    {
        info_node_low_update(node);
        info_node_high_update(node);
        node=(struct info_node *)node->base.obj.next;
    }
}
