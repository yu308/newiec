#include "../common/iec.h"


static int32 asdu_unpack_CCSNA(Asdu *asdu,uint8 *node_data,int node_addr_count,uint32 *custom_data)
{
    Node_Addr addr=0;
    uint32 i=node_addr_count;
    struct system_global_data *zero=(struct system_global_data *)custom_data;

    iec_memcpy(&addr,node_data,node_addr_count);

    if(addr!=0)
    {
        return -2;
    }

    iec_memcpy(&zero->gTime,&node_data[i],sizeof(Info_E_TM56));

    if(asdu->reason==Act)
    {
       asdu->reason=Actcon;
    }
    else if(asdu->reason==Deact)
    {
        asdu->reason=Deactcon;
    }
    else
    {
        return -4;
    }

    asdu->node_count=1;
    //asdu->node[0]=addr;
    node_addr_init(&asdu->node[0],addr,node_addr_count);

    return 1;
}
extern void rtc_time_set(rt_uint8_t *t);
static int32 asdu_pack_CCSNA(Asdu *asdu,uint8 *buff,int node_addr_count,uint32 *custom_data)
{
    Node_Addr addr=0;
    uint32 i=0;
	rt_uint8_t t[8];
    struct system_global_data *zero=(struct system_global_data *)custom_data;
    iec_memcpy(&addr,&asdu->node[0],node_addr_count);

    iec_memcpy(&buff[i],&addr,node_addr_count);
    i+=node_addr_count;
    iec_memcpy(&buff[i],&zero->gTime,7);
    i+=7;

    if(asdu->reason==Actcon)
    {
      /* 调用用户设置时间接口函数 */
		t[0] = zero->gTime.YEAR;
		t[1] = zero->gTime.MONTH;
		t[2] = zero->gTime.DOM;
		t[3] = zero->gTime.HOUR;
		t[4] = zero->gTime.MIN;
		t[5] = zero->gTime.M_SEC/1000;
		t[6] = t[7] = 0;
		rtc_time_set(t);
    }

    return i;

}

static int32 asdu_unpack_CDCNA(Asdu *asdu, uint8 *node_data, int node_addr_count, uint32 *custom_data)
{
	Node_Addr addr = 0;
	uint32 i = node_addr_count;
	struct system_global_data *zero = (struct system_global_data *)custom_data;
	iec_memcpy(&addr, node_data, node_addr_count);

	struct ctrl_node *cnode = iec_ctrl_node_find(&zero->gCtrl_Node, addr, node_addr_count);

	struct info_element *e = (struct info_element *)iec_node_element_find(&cnode->base, DCO);

	if (cnode == 0)
	{
		return -2;
	}

	Info_E_DCO dco;

	iec_memcpy(&dco, &node_data[i], sizeof(Info_E_DCO));

	if (e != 0 && e->val != 0)
	{
		iec_memcpy(e->val, &dco, sizeof(Info_E_DCO));
	}

	if (asdu->reason == Act)
	{
		if (dco.SE == CHOOSE)
		{
			if (cnode->check != 0)
			{
				if (cnode->check(dco.DCS-1) == 1)
				{
					asdu->reason = Actcon;
				}
				else
				{
					asdu->reason = Actterm;
				}
			}
			else
			{
				asdu->reason = Actcon;
			}
		}
		else if (dco.SE == EXECUTE)
		{
			if (cnode->action != 0)
			{
				cnode->action(dco.DCS-1);
				asdu->reason = Actcon;
			}
			else
				asdu->reason = Actterm;
		}
	}
	else if (asdu->reason == Deact)
	{
		asdu->reason = Deactcon;
	}
	else
	{
		asdu->reason = Unknowreason;
		return -4;
	}

	asdu->node_count = 1;
	node_addr_init(&asdu->node[0], addr, node_addr_count);
	//asdu->node[0]=addr;

	return 1;
}

static int32 asdu_pack_CDCNA(Asdu *asdu, uint8 *buff, int node_addr_count, uint32 *custom_data)
{
	Node_Addr addr = 0;
	uint32 i = node_addr_count;
	struct system_global_data *zero = (struct system_global_data *)custom_data;
	iec_memcpy(&addr, &asdu->node[0], node_addr_count);

	struct ctrl_node *cnode = iec_ctrl_node_find(&zero->gCtrl_Node, addr, node_addr_count);

	struct info_element *e = (struct info_element *)iec_node_element_find(&cnode->base, DCO);

	iec_memcpy(buff, &addr, node_addr_count);

	iec_memcpy(&buff[i], e->val, sizeof(Info_E_DCO));
	i += sizeof(Info_E_DCO);

	return i;

}


static int32 asdu_unpack_CSCNA(Asdu *asdu,uint8 *node_data,int node_addr_count,uint32 *custom_data)
{
    Node_Addr addr=0;
    uint32 i=node_addr_count;
    struct system_global_data *zero=(struct system_global_data *)custom_data;
    iec_memcpy(&addr,node_data,node_addr_count);

    struct ctrl_node *cnode=iec_ctrl_node_find(&zero->gCtrl_Node, addr,node_addr_count);

    struct info_element *e=(struct info_element *)iec_node_element_find(&cnode->base,SCO);

    if(cnode==0)
    {
        return -2;
    }

    Info_E_SCO sco;

    iec_memcpy(&sco,&node_data[i],sizeof(Info_E_SCO));

    if(e!=0&&e->val!=0)
    {
      iec_memcpy(e->val,&sco,sizeof(Info_E_SCO));
    }

    if(asdu->reason==Act)
    {
        if(sco.SE==CHOOSE)
        {
            if(cnode->check!=0)
            {
                if(cnode->check(sco.SCS)==1)
                {
                    asdu->reason=Actcon;
                }
                else
                {
                    asdu->reason=Actterm;
                }
            }
            else
            {
                asdu->reason=Actcon;
            }
        }
        else if(sco.SE==EXECUTE)
        {
            if(cnode->action!=0)
            {
                cnode->action(sco.SCS);
                asdu->reason=Actcon;
            }
            else
                asdu->reason=Actterm;
        }
    }
    else if(asdu->reason==Deact)
    {
        asdu->reason=Deactcon;
    }
    else
    {
        asdu->reason=Unknowreason;
        return -4;
    }

    asdu->node_count=1;
    node_addr_init(&asdu->node[0],addr,node_addr_count);
    //asdu->node[0]=addr;

    return 1;
}

static int32 asdu_pack_CSCNA(Asdu *asdu,uint8 *buff,int node_addr_count,uint32 *custom_data)
{
    Node_Addr addr=0;
    uint32 i=node_addr_count;
    struct system_global_data *zero=(struct system_global_data *)custom_data;
    iec_memcpy(&addr,&asdu->node[0],node_addr_count);

    struct ctrl_node *cnode=iec_ctrl_node_find(&zero->gCtrl_Node, addr,node_addr_count);

    struct info_element *e=(struct info_element *)iec_node_element_find(&cnode->base,SCO);

    iec_memcpy(buff,&addr,node_addr_count);

    iec_memcpy(&buff[i],e->val,sizeof(Info_E_SCO));
    i+=sizeof(Info_E_SCO);

    return i;

}

static int32 asdu_unpack_CICNA(Asdu *asdu,uint8 *node_data,int node_addr_count,uint32 *custom_data)
{
    Node_Addr addr=0;
    uint32 i=node_addr_count;
    struct system_global_data *zero=(struct system_global_data *)custom_data;
    iec_memcpy(&addr,node_data,node_addr_count);

    if(addr!=0)
    {
        return -2;
    }

    iec_memcpy(&zero->gCall_QOI,&node_data[i],sizeof(Info_E_QOI));

    if(asdu->reason==Act)
    {
       asdu->reason=Actcon;
    }
    else if(asdu->reason==Deact)
    {
        asdu->reason=Deactcon;
    }
    else
    {
        return -4;
    }

    asdu->node_count=1;
    //asdu->node[0]=addr;
    node_addr_init(&asdu->node[0],addr,node_addr_count);

    return 1;
}


static int32 asdu_pack_CICNA(Asdu *asdu,uint8 *buff,int node_addr_count,uint32 *custom_data)
{
    Node_Addr addr=0;
    uint32 i=0;
    struct system_global_data *zero=(struct system_global_data *)custom_data;
    iec_memcpy(&addr,&asdu->node[0],node_addr_count);

    iec_memcpy(&buff[i],&addr,node_addr_count);
    i+=node_addr_count;
    iec_memcpy(&buff[i],&zero->gCall_QOI,sizeof(Info_E_QOI));
    i+=sizeof(Info_E_QOI);

    if(zero->gCall_QOI==GOL_CALL&&asdu->reason==Actcon)
    {

        /*总召用户接口*/
        /* Asdu *fasdu=0; 放入用户接口中调用 传入原因，*/
        /* fasdu=iec_asdu_create(C_IC_NA,Actterm,0); */
        /* if(fasdu!=0) */
        /*     iec_first_task_add(fasdu); */
    }
    if(zero->gCall_QOI==GOL_CALL&&asdu->reason==Actterm) 
     {
       zero->global_tag=1;  
     } 

    return i;
}


static int32 asdu_unpack_CRCNA(Asdu *asdu,uint8 *node_data,int node_addr_count,uint32 *custom_data)
{
    Node_Addr addr=0;
    uint32 i=node_addr_count;
    struct system_global_data *zero=(struct system_global_data *)custom_data;
    iec_memcpy(&addr,node_data,node_addr_count);

    struct ctrl_node *cnode=iec_ctrl_node_find(&zero->gCtrl_Node,addr,node_addr_count);

    struct info_element *e=(struct info_element *)iec_node_element_find(&cnode->base,RCO);

    if(cnode==0)
    {
        return -2;
    }

    Info_E_RCO rco;

    iec_memcpy(&rco,&node_data[i],sizeof(Info_E_RCO));

    if(e!=0&&e->val!=0)
    {
      iec_memcpy(e->val,&rco,sizeof(Info_E_RCO));
    }

    if(asdu->reason==Act)
    {
        if(rco.SE==CHOOSE)
        {
            if(cnode->check!=0)
            {
                if(cnode->check(rco.RCS)==1)
                {
                    asdu->reason=Actcon;
                }
                else
                {
                    asdu->reason=Actterm;
                }
            }
            else
            {
                asdu->reason=Actcon;
            }
        }
        else if(rco.SE==EXECUTE)
        {
            if(cnode->action!=0)
            {
                cnode->action(rco.RCS);
                asdu->reason=Actcon;
            }
            else
                asdu->reason=Actterm;
        }
    }
    else if(asdu->reason==Deact)
    {
        asdu->reason=Deactcon;
    }
    else
    {
        return -4;
    }

    asdu->node_count=1;
    node_addr_init(&asdu->node[0],addr,node_addr_count);
    //asdu->node[0]=addr;

    return 1;
}

static int32 asdu_pack_CRCNA(Asdu *asdu,uint8 *buff,int node_addr_count,uint32 *custom_data)
{
    Node_Addr addr=0;
    uint32 i=node_addr_count;
    struct system_global_data *zero=(struct system_global_data *)custom_data;
    iec_memcpy(&addr,&asdu->node[0],node_addr_count);

    struct ctrl_node *cnode=iec_ctrl_node_find(&zero->gCtrl_Node,addr,node_addr_count);

    struct info_element *e=(struct info_element *)iec_node_element_find(&cnode->base,RCO);

    iec_memcpy(buff,&addr,node_addr_count);

    iec_memcpy(&buff[i],e->val,sizeof(Info_E_RCO));
    i+=sizeof(Info_E_RCO);

    return i;
}

static int32 asdu_unpack_MMENB(Asdu *asdu,uint8 *node_data,int node_addr_count,uint32 *custom_data)
{
  return 0;
}

static int32 asdu_pack_MMENB(Asdu *asdu,uint8 *buff,int node_addr_count,uint32 *custom_data)
{
    Node_Addr addr=0;
    uint32 idx=0;
    struct system_global_data *zero=(struct system_global_data *)custom_data;
    uint32 ncout=asdu->node_count;
    uint32 i=0;

    struct info_node *inode=0;
     struct info_element *e=0;

    for(i=0;i<ncout;i++)
    {
      iec_memcpy(&addr,&asdu->node[i],node_addr_count);
       inode=iec_info_node_find(&zero->gInfo_Node,addr,node_addr_count);
       e=(struct info_element *)iec_node_element_find(&inode->base,SVA);
       iec_memcpy(&buff[idx],&addr,node_addr_count);
       idx+=node_addr_count;

       iec_memcpy(&buff[idx],e->val,sizeof(Info_E_SVA));
       idx+=sizeof(Info_E_SVA);

       e=(struct info_element *)iec_node_element_find(&inode->base,QDS);
       iec_memcpy(&buff[idx],e->val,sizeof(Info_E_QDS));
       idx+=sizeof(Info_E_QDS);
       inode->updated_lock=0;

    }

    return idx;
}

static int32 asdu_unpack_MSPNA(Asdu *asdu,uint8 *node_data,int node_addr_count,uint32 *custom_data)
{
  return 0;
}

static int32 asdu_pack_MSPNA(Asdu *asdu,uint8 *buff,int node_addr_count,uint32 *custom_data)
{
    Node_Addr addr=0;
    uint32 idx=0;
    struct system_global_data *zero=(struct system_global_data *)custom_data;
    uint32 ncout=asdu->node_count;
    uint32 i=0;

    struct info_node *inode=0;
     struct info_element *e=0;

    for(i=0;i<ncout;i++)
    {
      iec_memcpy(&addr,&asdu->node[i],node_addr_count);
       inode=iec_info_node_find(&zero->gInfo_Node,addr,node_addr_count);
       e=(struct info_element *)iec_node_element_find(&inode->base,SIQ);
       iec_memcpy(&buff[idx],&addr,node_addr_count);
       idx+=node_addr_count;

       iec_memcpy(&buff[idx],e->val,sizeof(Info_E_SIQ));
       idx+=sizeof(Info_E_SIQ);

	   inode->updated_lock = 0;
    }

    return idx;
}


static int32 asdu_unpack_MSPTA(Asdu *asdu, uint8 *node_data, int node_addr_count, uint32 *custom_data)
{
	return 0;
}

static int32 asdu_pack_MSPTA(Asdu *asdu, uint8 *buff, int node_addr_count, uint32 *custom_data)
{
	Node_Addr addr = 0;
	uint32 idx = 0;
	struct system_global_data *zero = (struct system_global_data *)custom_data;
	uint32 ncout = asdu->node_count;
	uint32 i = 0;

	struct info_node *inode = 0;
	struct info_element *e = 0;

	for (i = 0; i<ncout; i++)
	{
		iec_memcpy(&addr, &asdu->node[i], node_addr_count);
		inode = iec_info_node_find(&zero->gInfo_Node, addr, node_addr_count);
		
		iec_memcpy(&buff[idx], &addr, node_addr_count);
		idx += node_addr_count;

		e = (struct info_element *)iec_node_element_find(&inode->base, SIQ);
		iec_memcpy(&buff[idx], e->val, 1);
		idx += 1;

		e = (struct info_element *)iec_node_element_find(&inode->base, TM24);
		iec_memcpy(&buff[idx], e->val, 3);
		idx += 3;
                
                 inode->updated_lock = 0;
	}

	return idx;
}

static int32 asdu_unpack_USER(Asdu *asdu,uint8 *node_data,int node_addr_count,uint32 *custom_data)
{
   Node_Addr addr=0;
   iec_memcpy(&addr,node_data,node_addr_count);
   asdu->node_count=1;
   node_addr_init(&asdu->node[0],addr,node_addr_count);

   return 1;
}

static int32 asdu_pack_CHOUR(Asdu *asdu,uint8 *buff,int node_addr_count,struct system_global_data *zero)
{
  #if 0
  uint32 idx=0;
  
  uint32 t_day=(asdu->node[0]&0xFF)/4+1;

  time_t sec=t_day*86400;
  time_t tol_sec=0;
  uint32 i=0,m=0;

  struct tm local_tm;
  /* tol_sec=mymktime(Dev_Param.sys_time.year+2000,Dev_Param.sys_time.month,Dev_Param.sys_time.day,Dev_Param.sys_time.hour,Dev_Param.sys_time.min,Dev_Param.sys_time.sec); */
  tol_sec=tol_sec-sec;

  /*
   *得出的时间月份从0-11 需加1处理
   */
  time_to_tm(tol_sec,0,&local_tm);
  
  iec_memcpy(&buff[idx],&asdu->node[0],sizeof(Node_Addr));
  idx+=sizeof(Node_Addr);
  
   for(i=0;i<6;i++)
    {
      if(Read_Hour_Data(local_tm.tm_mon+1,local_tm.tm_mday,((asdu->node[0]&0xFF)%4)*6+i,&buff[idx])==0)
	break;
      
      idx+=32;
    }
   
   return idx;
#endif

   return 0;
}


static int32 asdu_pack_CEVT(Asdu *asdu,uint8 *buff,int node_addr_count,struct system_global_data *zero)
{
  #if 0
  uint32 idx=0;
  
  uint32 e_count=(asdu->node[0]&0xFF)/4+1;
  
   
  iec_memcpy(&buff[idx],&asdu->node[0],sizeof(Node_Addr));
  idx+=sizeof(Node_Addr);
  
  if(Read_Event(6,(asdu->node[0]&0xFF),&buff[idx])==0)
    idx+=192;
   
   return idx;
#endif
   return 0;
}

#define  C_HOUR_DA 141
#define  C_EVT_DA  142
extern void allgroup_create(int param[]);

//应用服务单元配置信息表
Asdu_Config gAsdu_Config[]=
{
  {C_SC_NA,1,asdu_unpack_CSCNA,asdu_pack_CSCNA,0},
  {C_IC_NA,1,asdu_unpack_CICNA,asdu_pack_CICNA,allgroup_create},
  {C_RC_NA,1,asdu_unpack_CRCNA,asdu_pack_CRCNA,0},
  {M_ME_NB,0,asdu_unpack_MMENB,asdu_pack_MMENB,0},
  {M_SP_NA,0,asdu_unpack_MSPNA,asdu_pack_MSPNA,0},
  {C_CS_NA,1,asdu_unpack_CCSNA,asdu_pack_CCSNA,0},
  { M_SP_TA,1,asdu_unpack_MSPTA,asdu_pack_MSPTA,0 },
  {C_DC_NA,1,asdu_unpack_CDCNA,asdu_pack_CDCNA,0 },

    /* {C_HOUR_DA,1,asdu_unpack_USER,asdu_pack_CHOUR}, */
    /* {C_EVT_DA,1,asdu_unpack_USER,asdu_pack_CEVT}, */
};


/**
 * @brief 查找某个ASDU配置信息
 * @param asdu_ident  ASDU的标识
 * @return  >0 ASDU配置信息  =0 未找到
 */

Asdu_Config *asdu_cfg_find(uint32 asdu_ident)
{
    uint32 cfg_sum=sizeof(gAsdu_Config)/sizeof(Asdu_Config);
    uint32 i=0;

    for(i=0; i<cfg_sum; i++)
    {
        if(gAsdu_Config[i].asdu_ident==asdu_ident)
        {
            return &gAsdu_Config[i];
        }
    }

    return 0;

}
