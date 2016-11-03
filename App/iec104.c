#include "../Core/core.h"
#include "../Iec/iec.h"
#include "../Layer/layer.h"
#include "lwip/sockets.h"
#include "gpio_dev.h"

#define IEC104_SERVER_THREAD_PROI   18
#define IEC104_RECV_THREAD_PROI   17
#define IEC104_SEND_THREAD_PROI   16

#define IEC104_THREAD_STACK_SIZE  2048
#define IEC104_THREAD_TICKS       20


/*
  单独每个socket链接的接收任务
*/
static void iec104_recv_thread(void *param)
{
  struct net_link_info *link=(struct net_link_info*)param;
  int bytes_received=0;
  
  net_link_thread_start(link);
  rt_kprintf("IEC104_MAIN: create a main thread,socket=%08x\n", link->cfg.socket);
  char *temp_buff=rt_malloc(256);
  while(1)
    {
      bytes_received = recv(link->cfg.socket,temp_buff, 256, 0);
      if (bytes_received <= 0)
        {
          goto err;
        }
      iec_sys_api_send_phyid_recv(link,temp_buff,bytes_received);
    }

 err:
  /* 关闭此socket链接,并清除iec104 context ,删除此任务*/
  closesocket(link->cfg.socket);
  rt_free(temp_buff);
  rt_thread_delete(rt_thread_self());
}

void iec104_recv_create(int socketid)
{
  //char name[24];
  //rt_memset(name, 0, 24);
  // lwip_setsockopt(socketid, SOL_SOCKET, SO_RCVTIMEO, &socket_timeout, sizeof(socket_timeout));
  //rt_sprintf(name, "%08x", socketid);
  
  
  iec_sys_api_create_link("cs8900", EVT_SUB_SYS_LINK_SOCKET,socketid,0,1);
}

/*
  TCP服务器监听IEC104固定端口
  有socket链接 ,则分配资源，创建接收任务
*/
static void iec104_tcpserver_listen_thread(void *param)
{
  int server_sock=*((int*)param);
  int accept_sock=0;
  rt_bool_t stop=RT_FALSE;

  rt_uint32_t sin_size;
  struct sockaddr_in server_addr, client_addr;

  rt_kprintf("\nTCPServer Waiting for client on port 2404...\n");
  stop=RT_FALSE;

  while(stop != RT_TRUE)
    {
      sin_size = sizeof(struct sockaddr_in);

      accept_sock = accept(server_sock, (struct sockaddr *)&client_addr, &sin_size);

      rt_kprintf("TCPSERVER:Got a connection from (%s , %d)\n",
                 inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

      led_action(LED_NET_POS, LED_ON);

      /* 创建iec104通信链路任务 */
      iec104_recv_create(accept_sock);
   }
}


static int iec104_tcpserver_bind(int *localip)
{

  int sock;
  char *lip=(char *)localip;
  struct sockaddr_in server_addr, client_addr;

 if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
       rt_kprintf("TCPSERVER:Socket error\n");
       return -1;
   }

   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(2404);
   server_addr.sin_addr.s_addr = lip[0]|(lip[1]<<8)|(lip[2]<<16)|(lip[3]<<24);
   rt_memset(&(server_addr.sin_zero),8, sizeof(server_addr.sin_zero));

   if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
   {
     rt_kprintf("TCPSERVER:Unable to bind\n");
       return -1;
   }

   if (listen(sock, 5) == -1)
   {
     rt_kprintf("TCPSERVER:Listen error\n");
       return -1;
   }

   return sock;
}

/*
  创建监听任务及主动数据发送任务
*/
static void iec104_SS_thread_create(int sock)
{
  int *main_sock=rt_malloc(sizeof(int));
  *main_sock=sock;
  rt_thread_t tid=rt_thread_create("tcpserver", iec104_tcpserver_listen_thread, main_sock, IEC104_THREAD_STACK_SIZE, IEC104_SERVER_THREAD_PROI, IEC104_THREAD_TICKS);
  rt_thread_startup(tid);

}

void iec104_cs8900_link_complete(int linkid)
{
  int *net_link=rt_malloc(sizeof(int));
   net_link=(int*)linkid;
  rt_thread_t tid=rt_thread_create("recv", iec104_recv_thread, net_link, IEC104_THREAD_STACK_SIZE, IEC104_RECV_THREAD_PROI, IEC104_THREAD_TICKS);
  rt_thread_startup(tid);
}


void iec104_cs8900_link_init(int *localip)
{
    int sock=iec104_tcpserver_bind(localip);
    if(sock<0)
      return;
    iec104_SS_thread_create(sock);
}