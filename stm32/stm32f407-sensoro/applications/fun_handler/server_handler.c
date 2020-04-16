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

#define CLIENT_PORT     8060

/**
 * @brief 板子作为服务器测试
 */
static int server_test( int argc, char **argv )
{
    char *recv_data;
    struct sockaddr_in server_addr;     //创建一个server套接字
    int sockfd = -1 ;
    struct netdev *netdev = RT_NULL ;   //创建一个网络设备

    if(argc != 2)
    {
        rt_kprintf("server  [netdev_name]   -- server test   \n");
        return -RT_ERROR;
    }

    netdev = netdev_get_by_name(argv[1]);   // 通过名称获取网卡对象 
    if( netdev == RT_NULL )
    {
        rt_kprintf("cloud not find the %s netdev \n" , argv[1] );
        return -RT_ERROR;
    }

    sockfd = socket( AF_INET , SOCK_STREAM , 0 );

    if( sockfd < 0 )
    {
        rt_kprintf("cloud not find the %s netdev \n" , argv[1] );
        return -RT_ERROR;
    }

    server_addr.sin_family = AF_INET ;                      //
    server_addr.sin_port   = htons( CLIENT_PORT );          //
    server_addr.sin_addr.s_addr = netdev->ip_addr.addr;
    //server_addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset( &(server_addr.sin_zero) , 0 , sizeof(server_addr.sin_zero) );

    if( bind( sockfd , (struct sockaddr *)&server_addr , sizeof(struct sockaddr) ) < 0 )
    {   
        rt_kprintf(" server cloud not bind the socket \n" );
        closesocket(sockfd);
        return -RT_ERROR;
    }
    rt_kprintf(" server bind the socket success \n" );

    if( listen(sockfd,1) < 0 )                                  //开始监听
    {
        rt_kprintf(" server listen fail \n" );
        closesocket(sockfd);
        return -RT_ERROR;
    }
    rt_kprintf(" listen success \n" );

    struct sockaddr_in client_addr;     //创建一个client套接字
    int clientfd = -1 ;
    int len; 
#if 0
    rt_memset(&(client_addr.sin_zero), 0, sizeof(client_addr.sin_zero));
#endif
    clientfd = accept( sockfd , (struct sockaddr *)&client_addr , &len );
    if( clientfd == -1 )        //失败
    {
        rt_kprintf(" client connect fail \n" );
    }
    rt_kprintf(" client connect success \n" );

    char read_buff[128];
    int ret ;

    rt_memset( read_buff , 0 , sizeof(read_buff) );
    ret = read(clientfd , read_buff , 128 );
    if( ret < 0 )
    {
        rt_kprintf(" read clientfd fail \n" );
    }
    else
    {
        rt_kprintf(" read data: %s \n" , read_buff );
    }


    closesocket(clientfd);
    closesocket(sockfd);
    return RT_EOK;
}



#ifdef FINSH_USING_MSH
#include <finsh.h>
/* 导出命令到 FinSH 控制台 */
MSH_CMD_EXPORT_ALIAS( server_test, server, server test  );
#endif /* FINSH_USING_MSH */




















