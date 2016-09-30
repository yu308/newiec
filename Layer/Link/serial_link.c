#include	"serial_link.h"
#include	"serial_linkdef.h"
#include	"iec_event.h"

#if(CFG_RUNNING_MODE==MUTLI_MODE)
void serial_link_thread_entry(void *param);
#define SERIAL_THREAD_TICK		(50)
#define MAX_EVENT_COUNT			(5)
/// <summary>
/// ��·��������
/// </summary>
osThreadDef(serial, serial_link_thread_entry, LINK_TASK_PROI, SERIAL_THREAD_TICK, 2048);
/// <summary>
/// ��·������Ϣ����
/// </summary>
osMessageQDef(serialevent, MAX_EVENT_COUNT, 4);
#endif

/// <summary>
/// ��ʼ��������·
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="addr">The addr.</param>
/// <param name="addr_len">The addr_len.</param>
/// <param name="dir">The dir.</param>
void serial_link_init(struct serial_link_info *info, int addr, int addr_len, int dir)
{
	XMEMSET(info, 0, sizeof(struct serial_link_info));
	struct serial_link_cfg *cfg= (struct serial_link_cfg *)XMALLOC(sizeof(struct serial_link_cfg));

	cfg->double_dir = dir;
	cfg->link_addr_len = addr_len;
	cfg->link_addr = addr;
	cfg->recv_buff = XMALLOC(256);
	cfg->send_buff = XMALLOC(256);
	cfg->prev_sent_buff = XMALLOC(256);
	cfg->prev_sent_len = 0;

	info->cfg = cfg;
	info->acd_tag = 0;
	info->app_tag = 0;
	info->fcb = 0;

#if(CFG_RUNNING_MODE==MUTLI_MODE)
	info->serial_event = osMessageCreate(osMessageQ(serialevent), 0);
#endif
}

/// <summary>
/// ����һ����·
/// </summary>
/// <param name="addr">The addr.</param>
/// <param name="addr_len">The addr_len.</param>
/// <param name="dir">The dir.</param>
/// <returns></returns>
struct serial_link_info *serial_link_create(int addr, int addr_len, int dir)
{
	struct serial_link_info *info = (struct serial_link_info *)XMALLOC(sizeof(struct serial_link_info));
	if (info == 0)
	{
		XPRINTF("serial linklayer configure malloc fail.\n");
		return 0;
	}
	
	serial_link_init(info, addr, addr_len, dir);

}

/// <summary>
/// �����·������Ϣ.
/// </summary>
/// <param name="cfg">The CFG.</param>
void serial_link_del(struct serial_link_info *info)
{
	XFREE(info->cfg->recv_buff);
	XFREE(info->cfg->send_buff);
	XFREE(info->cfg->prev_sent_buff);
	XFREE(info->cfg);
}


/// <summary>
/// ���³�ʼ��,�ָ���ʼ״̬
/// </summary>
/// <param name="cfg">The CFG.</param>
void serial_link_reinit(struct serial_link_info *info)
{
	info->acd_tag = 0;
	info->app_tag = 0;
	info->fcb = 0;
}

/// <summary>
/// ���з�ʽ��������·��ַ
/// </summary>
/// <param name="addr">The addr.</param>
void serial_link_set_addr(struct serial_link_cfg *cfg, int addr)
{
	cfg->link_addr = addr;
}


/// <summary>
/// ���з�ʽ��������·��ַ����.
/// </summary>
/// <param name="len">The length.</param>
void serial_link_set_addr_len(struct serial_link_cfg *cfg, int len)
{
	cfg->link_addr_len = len;
}

/// <summary>
/// ���ô���ģʽ
/// </summary>
/// <param name="dir">The dir. =0 ��ƽ�⴫��ģʽ  =1 ƽ�⴫��ģʽ</param>
void serial_link_set_dir(struct serial_link_cfg *cfg, int dir)
{
	cfg->double_dir = dir;
}

/// <summary>
/// ��ȡ��·��ַ.
/// </summary>
/// <returns>��·��ַ</returns>
int serial_link_get_addr(struct serial_link_cfg *cfg)
{
	return cfg->link_addr;
}

/// <summary>
/// ��ȡ��·��ַ����
/// </summary>
/// <returns>��·��ַ����</returns>
int serial_link_get_addr_len(struct serial_link_cfg *cfg)
{
	return cfg->link_addr_len;
}


void serial_link_set_write_handle(struct serial_link_cfg *cfg, int *handle)
{
	cfg->serial_write = (serial_write_handle)handle;
}

/// <summary>
/// ��ȡһ�����ݵ�У���
/// </summary>
/// <param name="buff">The buff.</param>
/// <param name="check_len">У��ĳ���</param>
/// <returns></returns>
static char serial_link_get_cs(char *buff, int check_len)
{
	char cs = 0;
	int i = 0;

	for (i = 0; i < check_len; i++)
		cs += buff[i];

	return cs;
}

/// <summary>
/// �̶�֡У��
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="recv_buff">The recv_buff.</param>
/// <returns></returns>
static int serial_link_fixed_check(struct serial_link_cfg *cfg, char *recv_buff)
{
	int link_addr = 0;

	if (recv_buff[0] != FIXED_HEAD_TAG)
		return 0;

	if (recv_buff[cfg->link_addr_len+3] != FRAME_END_TAG)
		return 0;
	
	if (recv_buff[cfg->link_addr_len + 2] != serial_link_get_cs(&recv_buff[1], cfg->link_addr_len + 1))
		return 0;

	XMEMCPY(link_addr, &recv_buff[5], cfg->link_addr_len);

	if (link_addr != cfg->link_addr)
		return 0;

	return 1;

}

/// <summary>
/// �ǹ̶�֡У��
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="recv_buff">The recv_buff.</param>
/// <returns></returns>
static int serial_link_unfixed_check(struct serial_link_cfg *cfg, char *recv_buff)
{
	int link_addr = 0;

	if (recv_buff[3] != UNFIXED_HEAD_TAG|| recv_buff[0] != UNFIXED_HEAD_TAG)
		return 0;

	if (recv_buff[1] != recv_buff[2])
		return 0;

	if (recv_buff[cfg->link_addr_len + 2] != serial_linklayer_get_cs(&recv_buff[4], recv_buff[1]))
		return 0;

	XMEMCPY(link_addr, &recv_buff[5], cfg->link_addr_len);

	if (link_addr != cfg->link_addr)
		return 0;
	
	return 1;
	
}



/// <summary>
/// У������
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="recv_size">The recv_size.</param>
/// <returns>���ؿ������ֽ� ��Ϊ0x0 ������У����� </returns>
char serial_link_recv_check_data(struct serial_link_cfg *cfg, int recv_size)
{
	char *recv_buff = cfg->recv_buff;

	if (recv_size >= (4 + cfg->link_addr_len))
	{
		if (serial_link_fixed_check(cfg, recv_buff))
		{
			return  recv_buff[1];
		}
		else if (serial_link_unfixed_check(cfg,recv_buff))
		{
			return recv_buff[4];
		}
	}
	
	return 0x0;
}


/// <summary>
/// ��·���ݴ������ݹ�����ַ�����.
/// </summary>
/// <param name="info">The information.</param>
/// <param name="cfg">The CFG.</param>
/// <param name="domain">The domain.</param>
/// <returns></returns>
int serial_link_dispatch(struct serial_link_info *info, struct serial_link_cfg *cfg, char domain)
{
	union ctrl_domain dw_domain;
	dw_domain.domain = domain;

	if (dw_domain.dw_ctrl_domain.FCV)
	{
		if (info->fcb != dw_domain.dw_ctrl_domain.FCB)
		{
			return INVAILD_FCB;
		}

		info->fcb = dw_domain.dw_ctrl_domain.FCB;
	}

	switch (dw_domain.dw_ctrl_domain.FunCoed)
	{
	case FC_DW_LINK:
		return TO_LINK_REQ;
	case FC_DW_REQ:
		return TO_LINK; // ��·��--�̶�֡��ʽֱ�Ӵ���
		break;
	case FC_DW_DATA_NO:
		return NO_AWS; //����ش�
		break;
	case FC_DW_RST_REMOTE:
	case FC_DW_RST_USER:
		serial_link_reinit(cfg);
		return TO_LINK;
		break;
	case FC_DW_DATA_YES:
		return TO_APP_USER;
	case FC_DW_FIRST_DATA:	/**< һ������ */
		return TO_APP_FIRST;
	case FC_DW_SEC_DATA:
		return TO_APP_SECOND; //Ӧ�ò�---����
		break;
	default:
		break;
	}

	return 0;

}

/// <summary>
/// ��·���װ�̶����ݰ�
/// </summary>
/// <param name="info">The information.</param>
/// <param name="funcode">The funcode. ������</param>
/// <returns>���ݰ�����</returns>
int serial_link_pack_fixed_frame(struct serial_link_info *info, char funcode)
{
	union ctrl_domain up_domain;
	int count=0;

	if (info->acd_tag > 0)
	{
		up_domain.up_ctrl_domain.ACD = 1;
	}

	up_domain.up_ctrl_domain.FunCoed = funcode;

	{
		info->cfg->send_buff[0] = FIXED_HEAD_TAG;
		info->cfg->send_buff[1] = up_domain.domain;
		XMEMCPY(&info->cfg->send_buff[2], &info->cfg->link_addr, info->cfg->link_addr_len);
		info->cfg->send_buff[1 + info->cfg->link_addr_len] = serial_linklayer_get_cs(&info->cfg->send_buff[1], 1 + info->cfg->link_addr_len);
		info->cfg->send_buff[2 + info->cfg->link_addr_len] = FRAME_END_TAG;

		count = 3 + info->cfg->link_addr_len;
	}
	return count;
}

/// <summary>
/// ��·���װ�ǹ̶����ݰ�
/// </summary>
/// <param name="info">The information.</param>
/// <param name="funcode">The funcode.</param>
/// <param name="app_data">The app_data.</param>
/// <param name="app_data_len">The app_data_len.</param>
/// <returns>���ݰ�����</returns>
int serial_link_pack_unfixed_frame(struct serial_link_info *info, char funcode, char *app_data, int app_data_len)
{
	union ctrl_domain up_domain;
	int count=0;

	if (info->acd_tag > 0)
	{
		up_domain.up_ctrl_domain.ACD = 1;
	}

	up_domain.up_ctrl_domain.FunCoed = funcode;

	{
		if (app_data != 0)
		{
			info->cfg->send_buff[0] = info->cfg->send_buff[3] = UNFIXED_HEAD_TAG;
			info->cfg->send_buff[4] = up_domain.domain;
			info->cfg->send_buff[1] = info->cfg->send_buff[2] = 1 + info->cfg->link_addr_len + app_data_len;
			XMEMCPY(&info->cfg->send_buff[5], app_data, app_data_len);
			info->cfg->send_buff[4 + info->cfg->link_addr_len + app_data_len] = serial_link_get_cs(&info->cfg->send_buff[4], 1 + info->cfg->link_addr_len + app_data_len);
			info->cfg->send_buff[5 + info->cfg->link_addr_len + app_data_len] = FRAME_END_TAG;

			count = info->cfg->send_buff[2] + 6;
		}

	}

	return count;
}


#if(CFG_RUNNING_MODE==MUTLI_MODE)
void serial_link_thread_entry(void *param)
{
	struct serial_link_info *info = (struct serial_link_info*)param;

	struct iec_event *event = 0;
	char *data = 0;
	int data_len = 0;
	char domain_byte = 0;
	int dispatch_res = 0;

	XPRINTF("IEC:SERIAL LINK: %d link is running.\n", info->serial_tid);

	while (1)
	{
		event =iec_recv_msg(info->serial_event, osWaitForever);
		if (event == 0)
			continue;

		switch (event->evt_type)
		{
		case EVT_LINK_PHY_CONNECT:
			break;
		case EVT_LINK_PHY_DISCONNECT:
			break;
		case EVT_LINK_RECV_DATA:
			data = event->msg->m_link_recv_info.recv_data;
			data_len = event->msg->m_link_recv_info.recv_len;
			XMEMSET(info->cfg->recv_buff, 0, data_len);
			XMEMCPY(info->cfg->recv_buff, data, data_len);

			domain_byte = serial_link_recv_check_data(info->cfg, data_len);
			if (domain_byte == 0)
				continue;

			dispatch_res=serial_link_dispatch(info, info->cfg, domain_byte);
			if (dispatch_res == NO_AWS)
				continue;
			if (dispatch_res == INVAILD_FCB)
				info->cfg->serial_write(info->cfg->prev_sent_buff, info->cfg->prev_sent_len);
			else if (dispatch_res == TO_LINK)
			{
				data_len = serial_link_pack_fixed_frame(info, FC_UP_YES);
				info->cfg->serial_write(info->cfg->send_buff, data_len);
				XMEMCPY(info->cfg->prev_sent_buff, info->cfg->send_buff, data_len);
			}
			else if (dispatch_res == TO_LINK_REQ)
			{
				data_len = serial_link_pack_fixed_frame(info, FC_UP_LINK);
				info->cfg->serial_write(info->cfg->send_buff, data_len);
				XMEMCPY(info->cfg->prev_sent_buff, info->cfg->send_buff, data_len);
			}
			else 
			{
				/*����APP����������APP��*/
			}
			break;
		case EVT_LINK_SEND_DATA:
			break;
		default:
			break;
		}
	}
}

void serial_link_thread_start(int plink_info)
{
	struct serial_link_info *info = (struct serial_link_info *)plink_info;
	info->serial_tid=osThreadCreate(osThread(serial), info);
}
#endif







