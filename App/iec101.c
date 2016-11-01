#include "../Core/core.h"
#include "../Iec/iec.h"

#define IEC101_LINK_NAME    "iec1_s"
#define IEC101_APP_NAME     "iec1_a"

#define IEC101_INIT_NODE_INFO(nd_info,node_addr,buffered,node_addr_len)  do{ if(nd_info==0){ \
nd_info = iec_api_gen_node_info(node_addr,buffered);}else{ \
    iec_api_add_nodedata_to_node(nd_info,node_addr,node_addr_len); }  \
}while(0)

int iec101_sys_cmd_callback(unsigned int asdu_ident,char *node_data,unsigned int node_len);
int iec101_ctrl_cmd_callback(unsigned int asdu_ident,unsigned int node_addr,char *node_data,unsigned int node_len);

void iec101_link_init(char *name,int link_addr,int link_addr_len,int dir)
{
    iec_sys_api_create_link(name, EVT_SUB_SYS_LINK_SERIAL,link_addr,link_addr_len,dir);
}

void iec101_app_init(char *name,int asdu_addr,int asdu_addr_len,int cause_len,int node_addr_len,int sm2)
{
    iec_sys_api_create_app(name,asdu_addr,asdu_addr_len,cause_len,node_addr_len,sm2);
}

void iec_sys_create_app_complete(char *name,unsigned int appid)
{
    if (rt_strcmp(name,IEC101_APP_NAME)==0)
    {
        iec_sys_api_app_bind_link(IEC101_LINK_NAME, IEC101_APP_NAME);
        iec_sys_api_start_app(IEC101_APP_NAME);
        iec_sys_api_start_link(IEC101_LINK_NAME);
        iec_sys_api_app_set_cmd_cb(IEC101_APP_NAME, EVT_SUB_APP_CTRL_CMD, iec101_ctrl_cmd_callback);
        iec_sys_api_app_set_cmd_cb(IEC101_APP_NAME, EVT_SUB_APP_SYS_CMD, iec101_sys_cmd_callback);
    }
}

void iec101_update_mv_node()
{
    struct node_frame_info *nd_info=0;

    IEC101_INIT_NODE_INFO(nd_info,0x4001,0,1);
}

static int iec101_all_call_proc()
{
     return CMD_RES_OK;
}

static int iec101_sync_time_proc(Info_E_TM56 *tm56)
{
}

static int iec101_delay_time_proc(Info_E_TM16 *tm16)
{
}

static int iec101_reset_proc(Info_E_QRP *qrp)
{
}

static int iec101_test_proc(Info_E_FBP *fbp)
{
}

int iec101_sys_cmd_callback(unsigned int asdu_ident,char *node_data,unsigned int node_len)
{
    int cmd_res=0;
    switch (asdu_ident)
    {
    case C_IC_NA:
        state=iec101_all_call_proc();
    case C_CS_NA:
        state = iec101_sync_time_proc(node_data);
        break;
    case C_TS_NA:
        state = iec101_test_proc(node_data);
        break;
    case C_CD_NA:
        state = iec101_delay_time_proc(node_data);
        break;
    case C_RP_NA:
        state = iec101_reset_proc(node_data);
        break;
    default:
        state = CMD_RES_ERR_TYPE;
        break;
    }

    return cmd_res;
}



int iec101_ctrl_cmd_callback(unsigned int asdu_ident,unsigned int node_addr,char *node_data,unsigned int node_len)
{
    int cmd_res=0;
    switch (asdu_ident)
    {
    case C_DC_NA:
        break;
    default:
        return CMD_RES_ERR_TYPE;
    }

    return cmd_res;
}
