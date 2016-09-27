#include "linklayer.h"




#if(CFG_SERIAL_CLIENT_EN||CFG_SERIAL_SERVER_EN)

/// <summary>
/// ��ʼ��������·���ýṹ
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="addr">The addr.</param>
/// <param name="addr_len">The addr_len.</param>
/// <param name="dir">The dir.</param>
void serial_linklayer_init(struct serial_linklayer_cfg *cfg, int addr, int addr_len, int dir)
{
	XMEMSET(cfg, 0, sizeof(struct serial_linklayer_cfg));

	cfg->double_dir = dir;
	cfg->link_addr_bytes = addr_len;
	cfg->link_addr = addr;

	cfg->info.acd_tag = 0;
	cfg->info.app_tag = 0;
	cfg->info.fcb = 0;
}

/// <summary>
/// ����һ����·����
/// </summary>
/// <param name="addr">The addr.</param>
/// <param name="addr_len">The addr_len.</param>
/// <param name="dir">The dir.</param>
/// <returns></returns>
struct serial_linklayer_cfg *serial_linklayer_create(int addr, int addr_len, int dir)
{
	struct serial_linklayer_cfg *cfg = (struct serial_linklayer_cfg *)XMALLOC(sizeof(struct serial_linklayer_cfg));
	if (cfg == 0)
	{
		XPRINTF("serial linklayer configure malloc fail.\n");
		return 0;
	}
	
	serial_linklayer_init(cfg, addr, addr_len, dir);

	cfg->recv_buff = XMALLOC(256);
	cfg->send_buff = XMALLOC(256);
}

/// <summary>
/// �����·������Ϣ.
/// </summary>
/// <param name="cfg">The CFG.</param>
void serial_linklayer_del(struct serial_linklayer_cfg *cfg)
{
	XFREE(cfg->recv_buff);
	XFREE(cfg->send_buff);
	XFREE(cfg);
}


/// <summary>
/// ���³�ʼ��,�ָ���ʼ״̬
/// </summary>
/// <param name="cfg">The CFG.</param>
void serial_linklayer_reinit(struct serial_linklayer_cfg *cfg)
{
	cfg->info.acd_tag = 0;
	cfg->info.app_tag = 0;
	cfg->info.fcb = 0;
}

/// <summary>
/// ������·�㻺��,ʹ�þ�̬�ڴ�ʱʹ��
/// </summary>
/// <param name="recv_handle">The recv_handle. ���սӿڻ���</param>
/// <param name="send_handle">The send_handle. ���ͽӿڻ��� </param>
void serial_linklayer_set_communicate_buff(struct serial_linklayer_cfg *cfg, char *recv_buff, char *send_buff)
{
	if (recv_buff != 0)
		cfg->recv_buff = recv_buff;

	if (send_buff != 0)
		cfg->send_buff = send_buff;
}
/// <summary>
/// ���з�ʽ��������·��ַ
/// </summary>
/// <param name="addr">The addr.</param>
void serial_linklayer_set_addr(struct serial_linklayer_cfg *cfg, int addr)
{
	cfg->link_addr = addr;
}


/// <summary>
/// ���з�ʽ��������·��ַ����.
/// </summary>
/// <param name="len">The length.</param>
void serial_linklayer_set_addr_len(struct serial_linklayer_cfg *cfg, int len)
{
	cfg->link_addr_bytes = len;
}

/// <summary>
/// ���ô���ģʽ
/// </summary>
/// <param name="dir">The dir. =0 ��ƽ�⴫��ģʽ  =1 ƽ�⴫��ģʽ</param>
void serial_linklayer_set_dir(struct serial_linklayer_cfg *cfg, int dir)
{
	cfg->double_dir = dir;
}

/// <summary>
/// ��ȡ��·��ַ.
/// </summary>
/// <returns>��·��ַ</returns>
int serial_linklayer_get_addr(struct serial_linklayer_cfg *cfg)
{
	return cfg->link_addr;
}

/// <summary>
/// ��ȡ��·��ַ����
/// </summary>
/// <returns>��·��ַ����</returns>
int serial_linklayer_get_addr_len(struct serial_linklayer_cfg *cfg)
{
	return cfg->link_addr_bytes;
}


/// <summary>
/// ��ȡһ�����ݵ�У���
/// </summary>
/// <param name="buff">The buff.</param>
/// <param name="check_len">У��ĳ���</param>
/// <returns></returns>
static char serial_linklayer_get_cs(char *buff, int check_len)
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
static int serial_linklayer_fixed_check(struct serial_linklayer_cfg *cfg, char *recv_buff)
{
	int link_addr = 0;

	if (recv_buff[0] != FIXED_HEAD_TAG)
		return 0;

	if (recv_buff[cfg->link_addr_bytes+3] != FRAME_END_TAG)
		return 0;
	
	if (recv_buff[cfg->link_addr_bytes + 2] != serial_linklayer_get_cs(&recv_buff[1], cfg->link_addr_bytes + 1))
		return 0;

	XMEMCPY(link_addr, &recv_buff[5], cfg->link_addr_bytes);

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
static int serial_linklayer_unfixed_check(struct serial_linklayer_cfg *cfg, char *recv_buff)
{
	int link_addr = 0;

	if (recv_buff[3] != UNFIXED_HEAD_TAG|| recv_buff[0] != UNFIXED_HEAD_TAG)
		return 0;

	if (recv_buff[1] != recv_buff[2])
		return 0;

	if (recv_buff[cfg->link_addr_bytes + 2] != serial_linklayer_get_cs(&recv_buff[4], recv_buff[1]))
		return 0;

	XMEMCPY(link_addr, &recv_buff[5], cfg->link_addr_bytes);

	if (link_addr != cfg->link_addr)
		return 0;
	
	return 1;
	
}



/// <summary>
/// У������
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="recv_size">The recv_size.</param>
/// <returns>���ؿ������ֽ� ��Ϊ0xFF ������У����� </returns>
char serial_linklayer_recv_check_data(struct serial_linklayer_cfg *cfg, int recv_size)
{
	char *recv_buff = cfg->recv_buff;

	if (recv_size >= (4 + cfg->link_addr_bytes))
	{
		if (serial_linklayer_fixed_check(cfg, recv_buff))
		{
			return  recv_buff[1];
		}
		else if (serial_linklayer_unfixed_check(cfg,recv_buff))
		{
			return recv_buff[4];
		}
	}
	
	return 0xFF;
}

/// <summary>
/// ��������Ϣ�ַ�����ͬ��ȥ����.
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="domain">The domain.</param>
/// <returns>0 ���账��, 1 ��·�㴦��  2 Ӧ�ò㴦��</returns>
int serial_linklayer_dispatch(struct serial_linklayer_cfg *cfg, char domain)
{
	union ctrl_domain dw_domain;
	dw_domain.domain = domain;

	if (dw_domain.dw_ctrl_domain.FCV)
	{
		if (cfg->info.fcb != dw_domain.dw_ctrl_domain.FCB)
		{
			return 0;
		}

		cfg->info.fcb = dw_domain.dw_ctrl_domain.FCB;
	}

	switch (dw_domain.dw_ctrl_domain.FunCoed)
	{
	case FC_DW_LINK:
	case FC_DW_REQ:
		return 1; // ��·��--�̶�֡��ʽֱ�Ӵ���
		break;
	case FC_DW_DATA_NO:
		return 0; //����ش�
		break;
	case FC_DW_RST_REMOTE:
	case FC_DW_RST_USER:
		serial_linklayer_reinit(cfg);
		return 2;
		break;
	case FC_DW_DATA_YES:
	case FC_DW_FIRST_DATA:	/**< һ������ */
	case FC_DW_SEC_DATA:
		return 2; //Ӧ�ò�---����
		break;		
	default:
		break;
	}
	
	return 0;

}

/// <summary>
/// ��·���װ���ݰ�
/// </summary>
/// <param name="frame_type">The frame_type. ֡���� </param>
/// <param name="cfg">The CFG. ��·��������Ϣ</param>
/// <param name="funcode">The funcode. ������</param>
/// <param name="app_data">The app_data.Ӧ������</param>
/// <param name="app_data_len">The app_data_len. Ӧ�����ݳ���</param>
/// <returns></returns>
int serial_linklayer_pack_frame(int frame_type, struct serial_linklayer_cfg *cfg, char funcode, char *app_data, int app_data_len)
{
	union ctrl_domain up_domain;
	int count;

	if (cfg->info.acd_tag > 0)
	{
		up_domain.up_ctrl_domain.ACD = 1;
	}

	up_domain.up_ctrl_domain.FunCoed = funcode;

	if (frame_type == FIXED_FRAME||app_data==0)
	{
		cfg->send_buff[0] = FIXED_HEAD_TAG;
		cfg->send_buff[1] = up_domain.domain;
		XMEMCPY(&cfg->send_buff[2], &cfg->link_addr, cfg->link_addr_bytes);
		cfg->send_buff[1 + cfg->link_addr_bytes] = serial_linklayer_get_cs(&cfg->send_buff[1], 1 + cfg->link_addr_bytes);
		cfg->send_buff[2 + cfg->link_addr_bytes] = FRAME_END_TAG;

		count = 3 + cfg->link_addr_bytes;
	}
	else if (frame_type == UNFIXED_FRAME)
	{
		if (app_data != 0)
		{
			cfg->send_buff[0] = cfg->send_buff[3] = UNFIXED_HEAD_TAG;
			cfg->send_buff[4] = up_domain.domain;
			cfg->send_buff[1] = cfg->send_buff[2] = 1 + cfg->link_addr_bytes + app_data_len;
			XMEMCPY(&cfg->send_buff[5], app_data, app_data_len);
			cfg->send_buff[4 + cfg->link_addr_bytes + app_data_len] = serial_linklayer_get_cs(&cfg->send_buff[4], 1 + cfg->link_addr_bytes + app_data_len);
			cfg->send_buff[5 + cfg->link_addr_bytes + app_data_len] = FRAME_END_TAG;

			count = cfg->send_buff[2] + 6;
		}

	}

	return count;
}

/// <summary>
/// ��·����Ϣ������.
/// </summary>
/// <param name="cfg">The CFG.</param>
/// <param name="domain">The domain.</param>
/// <returns></returns>
int serial_linklayer_worker(struct serial_linklayer_cfg *cfg, char domain)
{
	union ctrl_domain dw_domain;
	dw_domain.domain = domain;


	switch (dw_domain.dw_ctrl_domain.FunCoed)
	{
	case FC_DW_LINK:
	case FC_DW_REQ:
		return serial_linklayer_pack_frame(FIXED_FRAME,cfg,FC_UP_YES,0,0); // ��·��--�̶�֡��ʽֱ�Ӵ���
		break;
	}
}
#endif


