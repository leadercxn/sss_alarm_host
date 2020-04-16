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


#define LOG_TAG     "UDP_HANDLER"
#define LOG_LVL     LOG_LVL_DBG   
#include <ulog.h> 

#define SENSORO_IOT_PORT    51700

#define SENSORO_IP "40.73.41.183"                   //网站服务器IP
#define SENSORO_IOT_TEST "iot-test.sensoro.com"     //域名


static rt_uint8_t send_buff[] = {
0x02,                                           // version
0x11, 0x22,                                     // token
0x02,                                           // type
0x60,0x47,0x62,0x17,0xC6,0x49,0x50,0x15};       // eUI: 60 47 62 17 C6 49 50 15

static rt_uint8_t read_buff[256];

static int udp_client(int argc ,char **argv)
{
    int sockfd = -1 ;
    struct hostent *host;
    struct sockaddr_in client_send_addr;
    int udp_send_fnt = 10 ;                                 //udp发送多少次数据

//  int sever_recv_sockfd = -1 ;                            //udp配合我们公司的iot平台，不需要创建第二个套接字
    struct sockaddr_in server_recv_addr, client_recv_addr;
    int bytes_read;
    struct timeval timeout;
    fd_set readset;
    int recv_len;

    host = gethostbyname( SENSORO_IOT_TEST );     //通过函数入口参数url获得host地址（如果是域名，会做域名解析)
    if( host == RT_NULL )
    {
        LOG_E("Get host by %s fail" , SENSORO_IOT_TEST ) ;
        return -RT_ERROR;
    }

    if( (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1 ) //创建一个套接字
    {
        LOG_E("Create clinet_send socket error");
        return -RT_ERROR;
    }

    /* 初始化client_send端的套接字 */
    client_send_addr.sin_family = AF_INET;
    client_send_addr.sin_port = htons(SENSORO_IOT_PORT);
    if(argc == 2)
    {
        client_send_addr.sin_addr.s_addr = inet_addr( argv[1] );
    }
    else
    {
        //client_send_addr.sin_addr.s_addr = inet_addr( SENSORO_IP );
        client_send_addr.sin_addr = *((struct in_addr *)host->h_addr);
    }

    rt_memset(&(client_send_addr.sin_zero), 0, sizeof(client_send_addr.sin_zero));

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    recv_len = sizeof(struct sockaddr);
    while( udp_send_fnt > 0)
    {
        if( sendto( sockfd , send_buff , sizeof(send_buff) , 0 , (struct sockaddr *)&client_send_addr, sizeof(struct sockaddr) ) > 0 )      //向目标端发送
        {
            LOG_I("udp client send success" ) ;
        }
        else
        {
            LOG_E("udp client send fail" ) ;
        }

        FD_ZERO(&readset);                                                                                                                  
        FD_SET(sockfd, &readset);                                                                                           //将需要监听可读事件的描述符加入列表
        if (select(sockfd + 1, &readset, RT_NULL, RT_NULL, &timeout) == 0)                                                  //等待接收
        {
            LOG_W("recv timeout");
            udp_send_fnt -- ;
            continue;
        }

        if (FD_ISSET(sockfd, &readset))                                                                                     //查看 sock 描述符上有没有发生可读事件
        {
            /* 从sock中收取最大BUFSZ - 1字节数据 */
            bytes_read = recvfrom(sockfd, read_buff, sizeof(read_buff), 0,(struct sockaddr *)&client_recv_addr, &recv_len); //读取接收到的数据
            if(bytes_read <= 0)
            {
                LOG_E("Received error");
            }
            else
            {
                // read_buff[bytes_read] = '\0'; /* 把末端清零 */
                for( rt_uint8_t i = 0 ; i < bytes_read ; i++ )
                {
                    LOG_I("read_buff[%d] = 0x%02x " , i , read_buff[i] );
                }
            }
        }
        
        rt_thread_mdelay(5000);                                                                                             //休眠
        udp_send_fnt -- ;
    }

    LOG_I("udp client send finish" ) ;

    closesocket(sockfd);
    
    return RT_EOK;
}


#ifdef FINSH_USING_MSH
#include <finsh.h>
/* 导出命令到 FinSH 控制台 */
MSH_CMD_EXPORT_ALIAS( udp_client, udp, udp test  );
#endif /* FINSH_USING_MSH */






