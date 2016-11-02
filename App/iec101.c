#include "../Core/core.h"
#include "../Iec/iec.h"
#include "../Layer/layer.h"

#define IEC101_LINK_NAME    "iec1_s"
#define IEC101_APP_NAME     "iec1_a"

#define IEC101_INIT_NODE_INFO(nd_info,node_addr,buffered,node_addr_len)  do{ if(nd_info==0){ \
nd_info = iec_api_gen_node_info(node_addr,buffered);}else{ \
    iec_api_add_nodedata_to_node(nd_info,node_addr,node_addr_len); }  \
}while(0)


#define COMPARE_VAL(a,b,range)  if((a>=b&&((a-b)>=range))||(a<b&&((b-a)<=range)))

#include "appsys.h"
#include "ftu.h"
#include "switcher.h"
#include "gpio_dev.h"
static unsigned int serial_link0=0;
static unsigned int serial_app0=0;

extern struct system_dev gSystemDev;

#define MV_ASDU         M_ME_NB   
#define MV_UAB_ADDR	  (0x4001)
#define MV_UBC_ADDR      (0x4002)
#define MV_IA_ADDR       (0x4003)
#define MV_IB_ADDR       (0x4004)
#define MV_IC_ADDR       (0x4005)
#define MV_UZ_ADDR       (0x4006)
#define MV_IZ_ADDR       (0x4007)
#define MV_P_ADDR        (0x4008)
#define MV_Q_ADDR        (0x4009)
#define MV_COS_ADDR       (0x400A)
#define MV_HZ_ADDR       (0x400B)



/**************************************************************************************************
 * <summary>	A macro that defines yx close address.合位信息 </summary>
 *
 * <remarks>	yu, 2016/7/23 星期六. </remarks>
 **************************************************************************************************/

#define YX_CLOSE_ADDR		(0x0001)  

/**************************************************************************************************
 * <summary>	A macro that defines yx cut address.分位信息 </summary>
 *
 * <remarks>	yu, 2016/7/23 星期六. </remarks>
 **************************************************************************************************/

#define YX_CUT_ADDR			(0x0002)       

/**************************************************************************************************
 * <summary>	A macro that defines yx hand address. 本地手动控制模式是否启用</summary>
 *
 * <remarks>	yu, 2016/7/23 星期六. </remarks>
 **************************************************************************************************/

#define YX_HAND_ADDR		(0x0003) 

/**************************************************************************************************
 * <summary>	A macro that defines yx remote lock address.远方通信控制启用 </summary>
 *
 * <remarks>	yu, 2016/7/23 星期六. </remarks>
 **************************************************************************************************/

#define YX_REMOTE_LOCK_ADDR (0x0004)

/**************************************************************************************************
 * <summary>	A macro that defines yx eactive address. 电池活化状态 </summary>
 *
 * <remarks>	yu, 2016/7/23 星期六. </remarks>
 **************************************************************************************************/

#define YX_EACTIVE_ADDR		(0x0005)

/**************************************************************************************************
 * <summary>	A macro that defines yx AC address.220v交流供电状态 </summary>
 *
 * <remarks>	yu, 2016/7/23 星期六. </remarks>
 **************************************************************************************************/

#define YX_AC_ADDR			(0x0006)

/**************************************************************************************************
 * <summary>	A macro that defines yx device-context address.电池欠压状态 </summary>
 *
 * <remarks>	yu, 2016/7/23 星期六. </remarks>
 **************************************************************************************************/

#define YX_DC_ADDR			(0x0007)

#define YX_OVER_C1          (0x0008) //过流1段
#define YX_OVER_C2	        (0x0009) // 过流2段
#define YX_OVER_C3           (0x000A) //过流3段
#define YX_OVER_LOAD		(0x000B) //过负荷
#define YX_ZERO				(0x000C) //零序
#define YX_OVER_U			(0x000D) //过压 
#define YX_LOW_U			(0x000E) //失压
#define YX_RECLOSE_FULL		(0x000F) //重合闸次数满
#define YX_RECLOSE_LOCK		(0x0010) //重合闸闭锁
#define YX_START			(0x0011) //运行启动
#define YX_HAND_CL			(0x0012) //手动合闸信号
#define YX_HAND_CUT			(0x0013)  //手动分闸信号

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


void serial_begin_recv()
{
  rt_device_t serial_dev=rt_device_find("serial0");
  rt_device_control(serial_dev,0xEE,0);
}

void iec101_link_init(int link_addr,int link_addr_len,int dir)
{
    iec_sys_api_create_link(IEC101_LINK_NAME, EVT_SUB_SYS_LINK_SERIAL,link_addr,link_addr_len,dir);
}


void iec101_app_init(int asdu_addr,int asdu_addr_len,int cause_len,int node_addr_len,int sm2)
{
    iec_sys_api_create_app(IEC101_APP_NAME,asdu_addr,asdu_addr_len,cause_len,node_addr_len,sm2);
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
      serial_link0=linkid;
      rt_device_t dev=rt_device_find("serial0");
      rt_device_open(dev, RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_DMA_RX);
      link_set_write_handle((struct link_obj*)linkid,(int *)iec101_serial_write);
      serial_begin_recv();
    }
}


void iec_sys_create_app_complete(char *name,unsigned int appid)
{
    if (rt_strcmp(name,IEC101_APP_NAME)==0)
    {
      serial_app0=appid;
        iec_sys_api_app_bind_link(IEC101_LINK_NAME, IEC101_APP_NAME);
        iec_sys_api_start_app(IEC101_APP_NAME);
        iec_sys_api_start_link(IEC101_LINK_NAME);
        iec_sys_api_app_set_cmd_cb(IEC101_APP_NAME, EVT_SUB_APP_CTRL_CMD, (int *)iec101_ctrl_cmd_callback);
        iec_sys_api_app_set_cmd_cb(IEC101_APP_NAME, EVT_SUB_APP_SYS_CMD, (int *)iec101_sys_cmd_callback);
    }
}

struct ftu_data last_realdata;

void iec101_update_mv_node()
{
  
  
  if(link_get_active_state((struct link_obj*)serial_link0)==0)
    return;
  
  if(((struct link_obj*)serial_link0)->data_trans_active==0)
    return;
  
  struct node_frame_info *nd_info=0;

  Info_E_QDS qds;
  Info_E_QOI qoi;

 int node_addr_len=((struct app_info*)serial_app0)->cfg.node_addr_len;
      qds.BL = 0;
      qds.IV = 0;
      qds.NT = 0;
      qds.OV = 0;
      qds.RES = 0;
      qds.SB = 0;
      
     COMPARE_VAL(gSystemDev.ftu_dev->c_realdata.Uab,last_realdata.Uab,50)
     {
       last_realdata.Uab=gSystemDev.ftu_dev->c_realdata.Uab;
      IEC101_INIT_NODE_INFO(nd_info,MV_UAB_ADDR,0,node_addr_len);
      iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Uab);
      iec_api_add_element_to_node(nd_info,QDS,&qds);
     }
    
      COMPARE_VAL(gSystemDev.ftu_dev->c_realdata.Ubc,last_realdata.Ubc,50)
      {
        last_realdata.Ubc=gSystemDev.ftu_dev->c_realdata.Ubc;
        IEC101_INIT_NODE_INFO(nd_info,MV_UBC_ADDR,0,node_addr_len);
        iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Ubc);
        iec_api_add_element_to_node(nd_info,QDS,&qds);
      }
    
      COMPARE_VAL(gSystemDev.ftu_dev->c_realdata.Ia,last_realdata.Ia,250)
      {
        last_realdata.Ia=gSystemDev.ftu_dev->c_realdata.Ia;
        IEC101_INIT_NODE_INFO(nd_info,MV_IA_ADDR,0,node_addr_len);
        iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Ia);
        iec_api_add_element_to_node(nd_info,QDS,&qds);
      }
    
      COMPARE_VAL(gSystemDev.ftu_dev->c_realdata.Ib,last_realdata.Ib,250)
      {
        last_realdata.Ib=gSystemDev.ftu_dev->c_realdata.Ib;
        IEC101_INIT_NODE_INFO(nd_info,MV_IB_ADDR,0,node_addr_len);
        iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Ib);
        iec_api_add_element_to_node(nd_info,QDS,&qds);
      }
    
      COMPARE_VAL(gSystemDev.ftu_dev->c_realdata.Ic,last_realdata.Ic,250)
      {
        last_realdata.Ic=gSystemDev.ftu_dev->c_realdata.Ic;
        IEC101_INIT_NODE_INFO(nd_info,MV_IC_ADDR,0,node_addr_len);
        iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Ic);
        iec_api_add_element_to_node(nd_info,QDS,&qds);
      }
    
      COMPARE_VAL(gSystemDev.ftu_dev->c_realdata.Uzero,last_realdata.Uzero,20)
      {
        last_realdata.Uzero=gSystemDev.ftu_dev->c_realdata.Uzero;
        IEC101_INIT_NODE_INFO(nd_info,MV_UZ_ADDR,0,node_addr_len);
        iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Uzero);
        iec_api_add_element_to_node(nd_info,QDS,&qds);
      }
     
     COMPARE_VAL(gSystemDev.ftu_dev->c_realdata.Izero,last_realdata.Izero,100)
      {
        last_realdata.Izero=gSystemDev.ftu_dev->c_realdata.Izero;
        IEC101_INIT_NODE_INFO(nd_info,MV_IZ_ADDR,0,node_addr_len);
        iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Izero);
        iec_api_add_element_to_node(nd_info,QDS,&qds);
      }
    
      COMPARE_VAL(gSystemDev.ftu_dev->c_realdata.P,last_realdata.P,5000)
      {
        last_realdata.P=gSystemDev.ftu_dev->c_realdata.P;
        IEC101_INIT_NODE_INFO(nd_info,MV_P_ADDR,0,node_addr_len);
        iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.P);
        iec_api_add_element_to_node(nd_info,QDS,&qds);
      }
    
      COMPARE_VAL(gSystemDev.ftu_dev->c_realdata.Q,last_realdata.Q,5000)
      {
        last_realdata.Q=gSystemDev.ftu_dev->c_realdata.Q;
        IEC101_INIT_NODE_INFO(nd_info,MV_Q_ADDR,0,node_addr_len);
        iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Q);
        iec_api_add_element_to_node(nd_info,QDS,&qds);
      }
     COMPARE_VAL(gSystemDev.ftu_dev->c_realdata.Cos,last_realdata.Cos,10)
      {
        last_realdata.Cos=gSystemDev.ftu_dev->c_realdata.Cos;
        IEC101_INIT_NODE_INFO(nd_info,MV_COS_ADDR,0,node_addr_len);
        iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Cos);
        iec_api_add_element_to_node(nd_info,QDS,&qds);
      }
    
      COMPARE_VAL(gSystemDev.ftu_dev->c_realdata.Hz,last_realdata.Hz,20)
      {
        last_realdata.Hz=gSystemDev.ftu_dev->c_realdata.Hz;
         IEC101_INIT_NODE_INFO(nd_info,MV_HZ_ADDR,0,node_addr_len);
        iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Hz);
        iec_api_add_element_to_node(nd_info,QDS,&qds);
      }
     
     if(nd_info!=0)
    iec_api_update_node(serial_app0,EVT_SUB_DAT_LEVEL_2,MV_ASDU,Spont,nd_info);
    

}


extern void rtc_time_get(rt_uint8_t *t);

static  int get_close_pos()
{
  struct ftu *t=(struct ftu *)(gSystemDev.ftu_dev);
  return switcher_check_status(t->swc,SWC_ST_CLOSE);
}

static  int get_cut_pos()
{
	return switcher_check_status(gSystemDev.ftu_dev->swc, SWC_ST_CUT);
}

static  int get_hand_pos()
{
	rt_uint32_t temp = 0;
	rt_device_t dev = rt_device_find("io");
	if (dev != 0)
	{
		rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_read(dev, IN_POS_HAND_MODE, &temp, 1);
		rt_device_close(dev);
	}
	return (temp==IN_NORMAL);
}

static  int get_lock_pos()
{
	rt_uint32_t temp = 0;
	rt_device_t dev = rt_device_find("io");
	if (dev != 0)
	{
		rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_read(dev, IN_POS_ST_LOCAL_CTRL, &temp, 1);
		rt_device_close(dev);
	}
	return (temp == IN_NORMAL);
}

static  int get_eactive_pos()
{
	rt_uint32_t temp = 0;
	rt_device_t dev = rt_device_find("io");
	if (dev != 0)
	{
		rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_read(dev, IN_POS_ST_VOL_ACTIVE, &temp, 1);
		rt_device_close(dev);
	}
	return (temp == IN_NORMAL);
}

static  int get_ac_pos()
{
	rt_uint32_t temp = 0;
	rt_device_t dev = rt_device_find("io");
	if (dev != 0)
	{
		rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_read(dev, IN_POS_ST_VOL_220, &temp, 1);
		rt_device_close(dev);
	}
	return (temp == IN_NORMAL);
}
static  int get_dc_pos()
{
	rt_uint32_t temp = 0;
	rt_device_t dev = rt_device_find("io");
	if (dev != 0)
	{
		rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_read(dev, IN_POS_ST_VOL_LACK, &temp, 1);
		rt_device_close(dev);
	}
	return (temp == IN_NORMAL);
}
static int get_over_c1_pos()
{
	return gSystemDev.ftu_dev->out_status.overcurrent1_alarm;
}

static int get_over_c2_pos()
{
	return gSystemDev.ftu_dev->out_status.overcurrent2_alarm;
}

static int get_over_c3_pos()
{
	return gSystemDev.ftu_dev->out_status.overcurrent3_alarm;
}


static int get_over_load_pos()
{
	return gSystemDev.ftu_dev->out_status.overload_alarm;
}
static int get_low_u_pos()
{
	return gSystemDev.ftu_dev->out_status.lowvoltage_alarm;
}
static int get_over_u_pos()
{
	return gSystemDev.ftu_dev->out_status.overvoltage_alarm;
}
static int get_reclose_sflock_pos()
{
	return gSystemDev.ftu_dev->out_status.recloselocked_alarm;
}
static int get_reclose_hwlock_pos()
{
	return gSystemDev.ftu_dev->out_status.reclosehwlocked_alarm;
}

static int get_hcls_pos()
{
	rt_uint32_t temp = 0;
	rt_device_t dev = rt_device_find("io");
	if (dev != 0)
	{
		rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_read(dev, IN_POS_HAND_CLOSE, &temp, 1);
		rt_device_close(dev);
	}
	return (temp == IN_NORMAL);
}

static int get_hcut_pos()
{
	rt_uint32_t temp = 0;
	rt_device_t dev = rt_device_find("io");
	if (dev != 0)
	{
		rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_read(dev, IN_POS_HAND_CUT, &temp, 1);
		rt_device_close(dev);
	}
	return (temp == IN_NORMAL);
}
static int get_start_pos()
{
	return gSystemDev.ftu_dev->out_status.start_signal;
}
static int get_zero_pos()
{
	return gSystemDev.ftu_dev->out_status.zero_alarm;
}
static void yx_all_cll()
{
    struct node_frame_info *nd_info=0;
   Info_E_SIQ  siq;
   int node_addr_len=((struct app_info*)serial_app0)->cfg.node_addr_len;
    siq.BL = 0;
    siq.NT = 0;
    siq.IV = 0;
    siq.RES = 0;
    siq.SB = 0; 
 
  siq.SPI=get_close_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_CLOSE_ADDR,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
   siq.SPI=get_cut_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_CUT_ADDR,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
  siq.SPI=get_hand_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_HAND_ADDR,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
   siq.SPI=get_lock_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_REMOTE_LOCK_ADDR,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
  siq.SPI=get_eactive_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_EACTIVE_ADDR,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
  siq.SPI=get_ac_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_AC_ADDR,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
  siq.SPI=get_dc_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_DC_ADDR,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
  siq.SPI=get_over_c1_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_OVER_C1,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
  siq.SPI=get_over_c2_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_OVER_C2,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
    siq.SPI=get_over_c3_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_OVER_C3,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
  siq.SPI=get_over_load_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_OVER_LOAD,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
  siq.SPI=get_zero_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_ZERO,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
   siq.SPI=get_over_u_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_OVER_U,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
    
       siq.SPI=get_low_u_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_LOW_U,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
    siq.SPI=get_reclose_sflock_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_RECLOSE_FULL,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
   siq.SPI=get_reclose_hwlock_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_RECLOSE_LOCK,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
   siq.SPI=get_hcls_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_HAND_CL,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
   siq.SPI=get_hcut_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_HAND_CUT,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
  siq.SPI=get_start_pos();
  IEC101_INIT_NODE_INFO(nd_info,YX_START,0,node_addr_len);
  iec_api_add_element_to_node(nd_info,SIQ,&siq);
  
  iec_api_update_node(serial_app0,EVT_SUB_DAT_LEVEL_1,M_SP_NA,Introgen,nd_info);

}

static int iec101_all_call_proc()
{
  
  yx_all_cll();
  
  
  struct node_frame_info *nd_info=0;

    Info_E_QDS qds;
    Info_E_QOI qoi;

    int node_addr_len=((struct app_info*)serial_app0)->cfg.node_addr_len;
      qds.BL = 0;
      qds.IV = 0;
      qds.NT = 0;
      qds.OV = 0;
      qds.RES = 0;
      qds.SB = 0;
      


    IEC101_INIT_NODE_INFO(nd_info,MV_UAB_ADDR,0,node_addr_len);
    iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Uab);
    iec_api_add_element_to_node(nd_info,QDS,&qds);
    
    IEC101_INIT_NODE_INFO(nd_info,MV_UBC_ADDR,0,node_addr_len);
    iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Ubc);
    iec_api_add_element_to_node(nd_info,QDS,&qds);
    
    IEC101_INIT_NODE_INFO(nd_info,MV_IA_ADDR,0,node_addr_len);
    iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Ia);
    iec_api_add_element_to_node(nd_info,QDS,&qds);
    
    IEC101_INIT_NODE_INFO(nd_info,MV_IB_ADDR,0,node_addr_len);
    iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Ib);
    iec_api_add_element_to_node(nd_info,QDS,&qds);
    
    IEC101_INIT_NODE_INFO(nd_info,MV_IC_ADDR,0,node_addr_len);
    iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Ic);
    iec_api_add_element_to_node(nd_info,QDS,&qds);
    
    IEC101_INIT_NODE_INFO(nd_info,MV_UZ_ADDR,0,node_addr_len);
    iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Uzero);
    iec_api_add_element_to_node(nd_info,QDS,&qds);
    
    IEC101_INIT_NODE_INFO(nd_info,MV_IZ_ADDR,0,node_addr_len);
    iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Izero);
    iec_api_add_element_to_node(nd_info,QDS,&qds);
    
    IEC101_INIT_NODE_INFO(nd_info,MV_P_ADDR,0,node_addr_len);
    iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.P);
    iec_api_add_element_to_node(nd_info,QDS,&qds);
    
    IEC101_INIT_NODE_INFO(nd_info,MV_Q_ADDR,0,node_addr_len);
    iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Q);
    iec_api_add_element_to_node(nd_info,QDS,&qds);
    
    IEC101_INIT_NODE_INFO(nd_info,MV_COS_ADDR,0,node_addr_len);
    iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Cos);
    iec_api_add_element_to_node(nd_info,QDS,&qds);
    
     IEC101_INIT_NODE_INFO(nd_info,MV_HZ_ADDR,0,node_addr_len);
    iec_api_add_element_to_node(nd_info,SVA,&gSystemDev.ftu_dev->c_realdata.Hz);
    iec_api_add_element_to_node(nd_info,QDS,&qds);
    
    iec_api_update_node(serial_app0,EVT_SUB_DAT_LEVEL_1,MV_ASDU,Introgen,nd_info);
    nd_info=0;
    
    IEC101_INIT_NODE_INFO(nd_info,0,0,node_addr_len);
    qoi=GOL_CALL;
    iec_api_add_element_to_node(nd_info,QOI,&qoi);
    iec_api_update_node(serial_app0,EVT_SUB_DAT_LEVEL_1,C_IC_NA,Actterm,nd_info);  

     return CMD_RES_OK;
}

static int iec101_sync_time_proc(Info_E_TM56 *tm56)
{
  unsigned char t[8];
  t[0] = tm56->YEAR;
  t[1] = tm56->MONTH;
  t[2] = tm56->DOM;
  t[3] = tm56->HOUR;
  t[4] = tm56->MIN;
  t[5] = tm56->M_SEC/1000;
  t[6] = t[7] = 0;
  rtc_time_set(t);
  return CMD_RES_OK;
}

static int iec101_delay_time_proc(Info_E_TM16 *tm16)
{
  return CMD_RES_OK;
}

static int iec101_reset_proc(Info_E_QRP *qrp)
{
  return CMD_RES_OK;
}

static int iec101_test_proc(Info_E_FBP *fbp)
{
  return CMD_RES_OK;
}

int iec101_sys_cmd_callback(unsigned int asdu_ident,char *node_data,unsigned int node_len)
{
    int cmd_res=0;
    switch (asdu_ident)
    {
    case C_IC_NA:
        cmd_res=iec101_all_call_proc();
        ((struct link_obj*)serial_link0)->data_trans_active=1;
    case C_CS_NA:
        cmd_res = iec101_sync_time_proc((Info_E_TM56 *)node_data);
        break;
    case C_TS_NA:
        cmd_res = iec101_test_proc((Info_E_FBP*)node_data);
        break;
    case C_CD_NA:
        cmd_res = iec101_delay_time_proc((Info_E_TM16*)node_data);
        break;
    case C_RP_NA:
        cmd_res = iec101_reset_proc((Info_E_QRP*)node_data);
        break;
    default:
        cmd_res = CMD_RES_ERR_TYPE;
        break;
    }

    return cmd_res;
}


static int swc_check(rt_int8_t act)
{
	rt_int32_t res = 0;
	rt_uint32_t state_temp;
	rt_device_t dev = rt_device_find("io");
	if (dev != 0)
	{
		rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_read(dev, IN_POS_ST_LOCAL_CTRL, &state_temp, 1);
		if (state_temp == IN_ERR)
		{
			rt_device_read(dev, IN_POS_HAND_MODE, &state_temp, 1);
			if (state_temp == IN_NORMAL) /*远方遥控*/
			{
				state_temp = IN_ERR;

				if(act==1)
					rt_device_read(dev, IN_POS_ST_CUT, &state_temp, 1);
				else if(act==0)
					rt_device_read(dev, IN_POS_ST_CLOSE, &state_temp, 1);

				if (state_temp == IN_NORMAL)
					res = 1;
			}
		}
		rt_device_close(dev);
	}
	return res;
}


static void swc_action(rt_int8_t act)
{
	rt_uint32_t state_temp;
	rt_device_t dev = rt_device_find("io");
	//register rt_base_t level;

	if (dev != 0)
	{
		rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_read(dev, IN_POS_ST_LOCAL_CTRL, &state_temp, 1);
		if (state_temp == IN_ERR)
		{
			rt_device_read(dev, IN_POS_HAND_MODE, &state_temp, 1);
			if (state_temp == IN_NORMAL) /*远方遥控*/
			{
				if (act == 1)
				{
					if (gSystemDev.ftu_dev->swc->get() == SWC_ST_CUT)
						rt_swc_remote_action_start(gSystemDev.ftu_dev->swc, 1);
				}
				else if (act == 0)
				{
					if (gSystemDev.ftu_dev->swc->get() == SWC_ST_CLOSE)
						rt_swc_remote_action_start(gSystemDev.ftu_dev->swc, 0);
				}

			}
		}
		rt_device_close(dev);
	}
	
}

static int ecact_check(rt_int8_t act)
{
	rt_int32_t res = 0;
	rt_uint32_t state_temp;
	rt_device_t dev = rt_device_find("io");
	if (dev != 0)
	{
		rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_read(dev, IN_POS_ST_VOL_ACTIVE, &state_temp, 1);
		if (state_temp == IN_ERR)
		{
			res = 1;

		}
		rt_device_close(dev);
	}
	return res;

}

static void ecact_action(rt_int8_t act)
{
	rt_uint32_t state_temp;
	rt_device_t dev = rt_device_find("io");

	if (dev != 0)
	{
		rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
		rt_device_read(dev, IN_POS_ST_VOL_ACTIVE, &state_temp, 1);
		if (state_temp == IN_ERR)
		{
				if (act == 1)
				{
					state_temp = OUT_SET;
					rt_device_write(dev, OUT_POS_VOL_ACTIVE, &state_temp, 1);
					rt_thread_delay(rt_tick_from_millisecond(200));
					state_temp = OUT_RESET;
					rt_device_write(dev, OUT_POS_VOL_ACTIVE, &state_temp, 1);
				}
		}
		rt_device_close(dev);
	}

}


int dc_node_ctrl_proc(unsigned int node_addr,char *node_data,unsigned int node_len)
{
  Info_E_DCO *dco=(Info_E_DCO*)node_data;
  if(node_addr==0x6001)
  {
    if (dco->SE == CHOOSE)
    {
      if (swc_check(dco->DCS-1) == 1)
      {
              return CMD_RES_OK;
      }
      else
      {
              return CMD_RES_FAIL;
      }

    }
    else if (dco->SE == EXECUTE)
    {
      swc_action(dco->DCS-1);
      return CMD_RES_OK;
    }
  }
  
  if(node_addr==0x6002)
  {
    if (dco->SE == CHOOSE)
    {
      if (ecact_check(dco->DCS-1) == 1)
      {
              return CMD_RES_OK;
      }
      else
      {
              return CMD_RES_FAIL;
      }

    }
    else if (dco->SE == EXECUTE)
    {
      ecact_action(dco->DCS-1);
      return CMD_RES_OK;
    }
  }
  
  return CMD_RES_ERR_NODE;
}

int iec101_ctrl_cmd_callback(unsigned int asdu_ident,unsigned int node_addr,char *node_data,unsigned int node_len)
{
    int cmd_res=0;
    switch (asdu_ident)
    {
    case C_DC_NA:
      return dc_node_ctrl_proc(node_addr,node_data,node_len);
        break;
    default:
        return CMD_RES_ERR_TYPE;
    }

    return cmd_res;
}


void iec_recv_data(unsigned char *buff,unsigned int len)
{
  iec_sys_api_send_phy_recv(IEC101_LINK_NAME,buff,len);
  serial_begin_recv();
}