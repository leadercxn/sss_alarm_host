#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <arpa/inet.h>
#include <netdev.h>

#include <sal.h>            /* SAL 组件结构体存放头文件 */
#include <sal_socket.h>
#include <af_inet.h>  

#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文件 */
#include "netdb.h"

#define SERVER_HOST   "192.168.1.11"
#define SERVER_PORT   8080

/**
 * @brief 板子作为client测试
 */
static int client_test( int argc, char **argv )
{
    struct sockaddr_in client_addr;     //创建一个client套接字
    struct netdev *netdev = RT_NULL ;   //创建一个网络设备
    int sockfd = -1 ;                   //创建一个套接字描述符
    char string_send[] = "server,i am from client";
    static rt_uint8_t init_flag = 0 ;   //初始化完成标志

    
#if 0
    if(argc != 3)
    {
        rt_kprintf("client_test  [netdev_name] [string_send]  -- client test   \n");
        return -RT_ERROR;
    }


    netdev = netdev_get_by_name(argv[1]);   // 通过名称获取网卡对象 
    if( netdev == RT_NULL )
    {
        rt_kprintf("cloud not find the %s netdev \n" , argv[1] );
        return -RT_ERROR;
    }
#endif
    if(init_flag == 0)
    {
        sal_init();
        init_flag = 1 ; 
    }

    sockfd = socket( AF_INET , SOCK_STREAM , 0 );
    
    if( sockfd < 0 )
    {
        rt_kprintf("cloud not find the %s netdev \n" , argv[1] );
        return -RT_ERROR;
    }

    client_addr.sin_family = AF_INET ;                      //
    client_addr.sin_port   = htons( SERVER_PORT );          //
    client_addr.sin_addr.s_addr = inet_addr( SERVER_HOST ); //
    rt_memset( &(client_addr.sin_zero) , 0 , sizeof(client_addr.sin_zero) );

    if( connect( sockfd , (struct sockaddr *)&client_addr , sizeof(struct sockaddr) ) < 0 )
    {   
        rt_kprintf("clinet cloud not connect server \n" );
        closesocket(sockfd);
        return -RT_ERROR;
    }
    else
    {
        rt_kprintf("clinet connect server success \n" );
    }

    if (send(sockfd, string_send, sizeof(string_send) , 0 ) > 0  )
    {
        rt_kprintf("clinet send data success \n" );
    }
    else
    {
        rt_kprintf("clinet send data fail \n" );
    }
    closesocket(sockfd);    //关闭连接
    return RT_EOK ;
}



#ifdef FINSH_USING_MSH
#include <finsh.h>
/* 导出命令到 FinSH 控制台 */
MSH_CMD_EXPORT_ALIAS( client_test, client, client send data to server );
#endif /* FINSH_USING_MSH */










