#include "../../OS/os_helper.h"
#include "../Helper/layer_helper.h"

/// <summary>
/// 设置传输模式
/// </summary>
/// <param name="dir">The dir. =0 非平衡传输模式  =1 平衡传输模式</param>
void link_set_dir(struct link_obj *link, int dir)
{
	link->double_dir = dir;
}

int link_get_dir(struct link_obj *link)
{
	return link->double_dir;
}


void link_set_write_handle(struct link_obj *link, int *handle)
{
	link->write = (write_handle)handle;
}


int link_get_active_state(struct link_obj *link)
{
  return link->active;
}

void serial_link_set_active_state(struct link_obj *link,int state)
{
  link->active=state;
}

char *link_get_name(struct link_obj *link)
{
  return link->name;
}
