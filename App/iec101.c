#include "../Core/core.h"
#include "../Iec/iec.h"
#include "../Layer/layer.h"

#define IEC101_LINK_NAME    "iec1_s"
#define IEC101_APP_NAME     "iec1_a"

#define IEC101_INIT_NODE_INFO(nd_info,node_addr,buffered,node_addr_len)  do{ if(nd_info==0){ \
nd_info = iec_api_gen_node_info(node_addr,buffered);}else{ \
    iec_api_add_nodedata_to_node(nd_info,node_addr,node_addr_len); }  \
}while(0)

/**
 * 系统初始化流程
 * 1、iec_sys_api_init_sysinfo() 初始化变量
 * 2、iec_sys_api_start_sys() 启动系统主任务
 * 3、iec101_link_init(..)  创建链路实例
 * 4、iec101_app_init(..) 创建APP实例
 * 5、link创建完成后设置相关参数 iec_sys_create_link_complete
 * 6、app创建完成后绑定Link,设置APP应用操作函数 iec_sys_create_app_complete
 */

int iec101_sys_cmd_callback(unsigned int asdu_ident,char *node_data,unsigned int node_len);
int iec101_ctrl_cmd_callback(unsigned int asdu_ident,unsigned int node_addr,char *node_data,unsigned int node_len);
void iec101_serial_write(unsigned int dev,unsigned char *data,unsigned int len);


void iec101_link_init(char *name,int link_addr,int link_addr_len,int dir)
{
    iec_sys_api_create_link(name, EVT_SUB_SYS_LINK_SERIAL,link_addr,link_addr_len,dir);
}


void iec101_app_init(char *name,int asdu_addr,int asdu_addr_len,int cause_len,int node_addr_len,int sm2)
{
    iec_sys_api_create_app(name,asdu_addr,asdu_addr_len,cause_len,node_addr_len,sm2);
}


void iec101_serial_write(unsigned int dev,unsigned char *data,unsigned int len)
{
  rt_device_t serial_dev=rt_device_find("serial0");
  if(serial_dev!=0)
    rt_device_write(serial_dev,0,data,len);
}



void iec_sys_create_link_complete(char *name,unsigned int linkid)
{
  if(rt_strcmp(name, IEC101_LINK_NAME)==0)
    {
      link_set_write_handle((struct link_obj*)linkid,iec101_serial_write);
    }
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
        cmd_res=iec101_all_call_proc();
    case C_CS_NA:
        cmd_res = iec101_sync_time_proc(node_data);
        break;
    case C_TS_NA:
        cmd_res = iec101_test_proc(node_data);
        break;
    case C_CD_NA:
        cmd_res = iec101_delay_time_proc(node_data);
        break;
    case C_RP_NA:
        cmd_res = iec101_reset_proc(node_data);
        break;
    default:
        cmd_res = CMD_RES_ERR_TYPE;
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
