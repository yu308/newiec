#include "../common/iec.h"
#include "../common/iec_context.h"
#include "rtthread.h"

extern void iec_mem_init();
extern iec_task* iec_mem_get_task();
extern Asdu* iec_mem_get_asdu();
extern void iec_mem_free_task(iec_task *task);
extern void iec_mem_free_asdu(Asdu *asdu);

struct rt_mutex task_mutex;

void task_mtx_init()
{
	rt_mutex_init(&task_mutex, "taskmtx", RT_IPC_FLAG_FIFO);
}

static void task_add(iec_task *head,iec_task *etask)
{
    iec_task *task=head;
	//rt_mutex_take(&task_mutex, RT_WAITING_FOREVER);
    while(task->obj.next!=0)
    {
        task=(iec_task *)task->obj.next;
    }

    task->obj.next=(Object *)etask;
	//rt_mutex_release(&task_mutex);
}

static iec_task *task_pick(iec_task *head,int *out)
{
    iec_task *task=(iec_task *)head->obj.next;
    *out=0;
	//rt_mutex_take(&task_mutex, RT_WAITING_FOREVER);
    if(task==0)
        goto end;

    head->obj.next=task->obj.next;
    if(head->obj.next!=0)
      *out=1;

	if (head->counter > 0)
		head->counter--;

end:
	//rt_mutex_release(&task_mutex);
    return task;
}

static iec_task *task_find(iec_task *head,Type_Ident ident)
{
	if (ident == 0)
		return 0;

    iec_task *task=(iec_task *)head->obj.next;

    while(task!=0)
    {
        if((task->asdu->typ==ident)&&(task->asdu->node_count<MAX_INFO_COUNT))
        {
            return task;
        }
        task=(iec_task *)task->obj.next;
    }

    return 0;
}

static uint8 task_check(iec_task *head,Asdu *asdu,int count)
{
    iec_task *task=task_find(head,asdu->typ);
    Asdu_Config *asdu_cfg=asdu_cfg_find(asdu->typ);

    if(task!=0) //已有对应task
    {
      if(byte_is_equal(&asdu->addr,&task->asdu->addr,count)==1)// ASDU地址一致
            if(asdu->err_len==task->asdu->err_len) //错误数据长度一致
                if((*(uint8 *)(&asdu->reason))==(*(uint8 *)(&task->asdu->reason))) //传送原因一致
                    //if((asdu->node_count+task->asdu->node_count)<MAX_INFO_COUNT) // 信息点数不超最大数
                        if(asdu_cfg->single_node==0)// ASDU 支持多个信息点
                        {
                          asdu_copy_node(task->asdu,asdu,count);
                          
						  if (asdu->err_data != 0)
							  iec_free(asdu->err_data);
						  iec_free(asdu);
						  /*iec_mem_free_asdu(asdu);*/
                            return 0;
                        }
    }
	//task->asdu->node_count += asdu->node_count;
    return 1;
}

int iec_task_add(iec_task *head,Asdu *asdu,struct system_applayer *applayer)
{
  asdu->var_str_q.info_count=asdu->node_count;
  
  if(asdu==0)
    return 0;

  if (asdu->typ == 0)
	  return 0;

  if (head->counter >= MAX_INFO_COUNT)
  {
	  iec_free(asdu);
	 /* iec_mem_free_asdu(asdu);*/
	  return 0;
  }

  if(task_check(head,asdu,applayer->asdu_addr_bytes)==1)
    {
      iec_task *task=(iec_task *)iec_malloc(sizeof(iec_task));
		/*iec_task *task = iec_mem_get_task();*/

      if(task==0)
        return 0;

      iec_memset(task,0,sizeof(iec_task));

      task->asdu=asdu;

      task_add(head,task); 

	  head->counter++;

      return 1;
    }

  return 0;
 
}


#if 0
void iec_first_task_add(iec_task *head,Asdu *asdu,struct system_applayer *applayer)
{
  

}

void iec_second_task_add(Asdu *asdu)
{
    asdu->var_str_q.info_count=asdu->node_count;
    if(task_check(&gSecond_Task,asdu)==1)
    {
      iec_task *task=(iec_task *)iec_malloc(sizeof(iec_task));

        if(task==0)
            return;

        iec_memset(task,0,sizeof(iec_task));

        task->asdu=asdu;

        task_add(&gSecond_Task,task);

    }
}
#endif

iec_task *iec_task_pick(iec_task *head,int *out)
{
  iec_task *task=task_pick(head,out);
  if(task==0)
    return 0;

  return task;
}

#if 0
iec_task *iec_first_task_pick(iec_task *firs_task)
{

    iec_task *task= task_pick(firs_task);

    if(task==0)
        return 0;

    acd--;
    return task;
}

iec_task *iec_second_task_pick(iec_task *second_task)
{
    iec_task *task=task_pick(second_task);

    if(task==0)
        return 0;

    return task;
}
#endif

void iec_task_free(iec_task *task)
{
  if(task->asdu!=0)
  {
	if (task->asdu->err_data != 0)
		iec_free(task->asdu->err_data);
  iec_free(task->asdu);
	/*iec_mem_free_asdu(task->asdu);*/
  }
 
  if(task!=0)
	  iec_free(task);
	//iec_mem_free_task(task);
}


void iec_task_clear(iec_task *head)
{
  int out=0;
  iec_task *task=task_pick(head,&out);

  while(task!=0)
    {
      iec_task_free(task);
      task=task_pick(head,&out);
    }
}

