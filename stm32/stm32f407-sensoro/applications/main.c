#include <stdio.h>
#include <string.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <rthw.h>

#include "SEGGER_RTT.h"
//#include <sockets.h>

#include "voice_handler.h"
#include "alpha_tasks.h"
#include "ser_alpha_data_handler.h"
#include "alpha_handler.h"


#include <button.h>



//#define USING_INTERRUPT                                   //
#define USING_MUX                         0  
#define DEBUG_LWIP                        0                 //测试lwip_udp

/******************************* 线程控制块 ******************************/
static rt_thread_t voice_thread_index = RT_NULL ;
static rt_thread_t init_thread_index = RT_NULL;
static rt_thread_t button_thread_index = RT_NULL;
static rt_thread_t open_lora_rx_thread_index = RT_NULL;
static rt_thread_t open_lora_tx_thread_index = RT_NULL;
static rt_thread_t uart3_rx_thread_index = RT_NULL;
static rt_thread_t parsing_alpha_data_thread_index = RT_NULL;
/******************************* 线程优先级 ******************************/
#define VOICE_THREAD_PRIORTY            5 
#define INIT_THREAD_PRIORTY             6
#define BUTTON_THREAD_PRIORTY           6
#define OPEN_LORA_RX_THREAD_PRIORTY     7
#define OPEN_LORA_TX_THREAD_PRIORTY     8
#define UART_RX_THREAD_PRIORTY          3
#define UDP_TEST_THREAD_PRIORTY         4
#define PARSING_ALPHA_DATA_THREAD_PRIORTY 9
/******************************* 锁 ******************************/
static rt_mutex_t m_my_mux = RT_NULL ;












/**
 * @brief 语音线程
 */
static void voice_thread_entry(void *parameter)
{
    if( NULL == parameter )
    {
        SEGGER_RTT_printf(0,"no voice index \n" );
        return ;
    }

    rt_uint8_t voice_index = *(rt_uint8_t *)parameter ;
#ifdef USING_INTERRUPT

    rt_base_t protect_level;

    protect_level = rt_hw_interrupt_disable();                  //关中断      
#endif 

#ifdef USING_MUX
    rt_mutex_take( m_my_mux , RT_WAITING_FOREVER );             //获取互斥量
#endif

    SEGGER_RTT_printf(0,"voice_index = %d \r\n" , voice_index);
    play_voice_start( voice_index );

#ifdef USING_MUX
    rt_mutex_release(m_my_mux);                                 //释放互斥量
#endif

#ifdef USING_INTERRUPT
    rt_hw_interrupt_enable(protect_level);                      //开中断
#endif 

    if( RT_EOK == rt_thread_delete( init_thread_index ) )
    {
        rt_kprintf("init_thread delete success \r\n");
    }

    SEGGER_RTT_printf(0,"play_voice finish \r\n"); 
}

/**
 * @brief 初始化线程
 */
static void init_thread_entry(void *parameter)
{
    voice_handler_init();
    rt_thread_startup(voice_thread_index);                          //初始化完成voice模块才开始播放语音
}

/**
 * @brief 按键线程
 */
static void button_thread_entry(void *parameter)
{
    while(1)
    {
        Button_Process();     //Need to call the button handler function periodically
        rt_thread_mdelay(5);
    }
}


#if( DEBUG_LWIP == 1 )                                             // 测试lwip_udp

#define BUFSZ       1024
#define SER_PORT    5000                                           // 端口号     
static rt_thread_t udp_test_thread_index = RT_NULL;
/**
 * @brief udp测试线程
 */
void udpserv_thread_entry(void *parameter)
{
    int sock;
    int bytes_read;
    char *recv_data;
    rt_uint32_t addr_len;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    
    
    recv_data = rt_malloc(BUFSZ);                                                                       // 分配接收用的数据缓冲 
    if (recv_data == RT_NULL)
    {
        rt_kprintf("No memory\n");                                                                      // 分配内存失败，返回 
        return;
    }
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)                                                  // 创建一个 socket，类型是 SOCK_DGRAM， UDP 类型 
    {
        rt_kprintf("Socket error\n");
        
        rt_free(recv_data);                                                                             // 释放接收用的数据缓冲 
        return;
    }

    
    server_addr.sin_family = AF_INET;                                                                   // 初始化服务端地址 
    server_addr.sin_port = htons(SER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset( &(server_addr.sin_zero),  0, sizeof(server_addr.sin_zero) );

    
    if ( bind(sock, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1 )                    // 绑定 socket 到服务端地址
    {
        rt_kprintf("Bind error\n");                                                                     // 绑定地址失败 
        
        rt_free(recv_data);                                                                             // 释放接收用的数据缓冲 
        return;
    }

    addr_len = sizeof(struct sockaddr);
    rt_kprintf("UDPServer Waiting for client on port 5000...\n");
    while (1)
    {
        bytes_read = recvfrom(sock, recv_data, BUFSZ - 1, 0,(struct sockaddr *)&client_addr,&addr_len); // 从 sock 中收取最大 BUFSZ - 1 字节数据 
        
        recv_data[bytes_read] = '\0';                                                                   // UDP 不同于 TCP，它基本不会出现收取的数据失败的情况，除非设置了超时等待,把末端清零 
        
        rt_kprintf("\n(%s , %d) said : ",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));  // 输出接收的数据 
        rt_kprintf("%s", recv_data);
        
        if (strcmp(recv_data, "exit") == 0)                                                             // 如果接收数据是 exit，退出
        {                                                   
            lwip_close(sock);                                                   

            rt_free(recv_data);                                                                         // 释放接收用的数据缓冲
            break;
        }
    }
    return;
}

#endif




/**
 * @brief main函数在 src/components.c下有指定rtthread_startup & main_thread_entry
 *  detail: 相关控制宏:RT_USING_USER_MAIN , RT_USING_COMPONENTS_INIT
 * 
 */
int main(void)
{
    //static rt_uint8_t void_index = 0 ;

#if USING_MUX
    /* 创建一个互斥锁 */
    m_my_mux = rt_mutex_create( "my_mux " , RT_IPC_FLAG_PRIO ) ; 
#endif
    /* 创建初始化线程 */
    init_thread_index = rt_thread_create(    "init_thread"    ,             
                                            init_thread_entry ,
                                            RT_NULL ,
                                            4096 ,
                                            INIT_THREAD_PRIORTY,
                                            20 );
    if( RT_NULL != init_thread_index )
    {
        rt_thread_startup(init_thread_index);
        rt_kprintf("init_thread_creat success \n");
    }
    else
    {
        rt_kprintf("init_thread_creat fail \n");
    }

    /* 创建语音播放线程 */
    voice_thread_index = rt_thread_create(  "voice_thread"     ,            
                                            voice_thread_entry ,
                                            RT_NULL,
                                            4096 ,
                                            VOICE_THREAD_PRIORTY,
                                            20 );
    if( RT_NULL != voice_thread_index )
    {
        rt_kprintf("voice_thread_creat success \n");
    }
    else
    {
        rt_kprintf("voice_thread_creat fail \n");
    }

    /* 创建按键线程 */
    button_thread_index = rt_thread_create(    "button_thread"     ,        
                                            button_thread_entry ,
                                            RT_NULL ,
                                            512 ,
                                            BUTTON_THREAD_PRIORTY,
                                            5 );
    if( RT_NULL != button_thread_index )
    {
        rt_thread_startup(button_thread_index);                             
        rt_kprintf("button_thread_creat success \n");
    }
    else
    {
        rt_kprintf("button_thread_creat fail \n");
    }
   
    /* 创建串口3接收的线程 */
    uart3_rx_thread_index = rt_thread_create( "alpha_rx_thread",                              
                                                    uart3_rx_thread_entry ,
                                                    RT_NULL,
                                                    512 ,
                                                    UART_RX_THREAD_PRIORTY,
                                                    20);
    if( RT_NULL != uart3_rx_thread_index )
    {
        rt_thread_startup(uart3_rx_thread_index);
        rt_kprintf("uart3_rx_thread_creat success \n");
    }
    else
    {
        rt_kprintf("uart3_rx_thread_creat fail \n");
    }

    /* 创建解析alpha串口接收数据的线程 */
    parsing_alpha_data_thread_index = rt_thread_create( "parsing_alpha_data_thread",                              
                                                    parsing_rx_alpha_data ,
                                                    RT_NULL,
                                                    512 ,
                                                    PARSING_ALPHA_DATA_THREAD_PRIORTY,
                                                    20);
    if( RT_NULL != parsing_alpha_data_thread_index )
    {
        rt_thread_startup(parsing_alpha_data_thread_index);
        rt_kprintf("parsing_alpha_data_thread_creat success \n");
    }
    else
    {
        rt_kprintf("parsing_alpha_data_thread_creat fail \n");
    }

#if 0
    /* 打开lora接收 */
    static ser_alpha_open_rx_param_t ser_alpha_open_rx_param ;
    ser_alpha_open_rx_param.freq = 433 ;
    ser_alpha_open_rx_param.bandwidth = 0 ; 
    ser_alpha_open_rx_param.daterate = 7 ; 
    ser_alpha_open_rx_param.crcon = true ;
    ser_alpha_open_rx_param.iqinverted = false ;
    open_lora_rx_thread_index = rt_thread_create(    "open_lora_rx_thread"   ,      
                                            open_lora_rx_entry ,
                                            &ser_alpha_open_rx_param ,
                                            512 ,
                                            OPEN_LORA_RX_THREAD_PRIORTY,
                                            20 );
    if( RT_NULL != open_lora_rx_thread_index )
    {
        rt_thread_startup(open_lora_rx_thread_index);
        rt_kprintf("open_lora_rx_thread_creat success \n");
    }
    else
    {
        rt_kprintf("open_lora_rx_thread_creat fail \n");
    }

    /* 打开lora发送 */
    static rt_uint8_t tx_buf[]={ 0x01 , 0x02 ,0x03 ,0x04 ,0x05 ,0x06 ,0x07 ,0x08 ,0x09 ,0x0A };
    static ser_alpha_open_tx_param_t  ser_alpha_open_tx_param ;
    ser_alpha_open_tx_param.freq = 434 ;
    ser_alpha_open_tx_param.power = 0 ;
    ser_alpha_open_tx_param.bandwidth = 1 ;
    ser_alpha_open_tx_param.daterate = 6 ;
    ser_alpha_open_tx_param.crcon =  true ;
    ser_alpha_open_tx_param.iqinverted = false ;
    ser_alpha_open_tx_param.tx_data_len = sizeof(tx_buf);
    memcpy( ser_alpha_open_tx_param.tx_data , tx_buf , sizeof(tx_buf) );
    open_lora_tx_thread_index = rt_thread_create(    "open_lora_tx_thread"   ,      
                                            open_lora_tx_entry ,
                                            &ser_alpha_open_tx_param ,
                                            512 ,
                                            OPEN_LORA_TX_THREAD_PRIORTY,
                                            20 );
    if( RT_NULL != open_lora_tx_thread_index )
    {
        rt_thread_startup(open_lora_tx_thread_index);
        rt_kprintf("open_lora_tx_thread_creat success \n");
    }
    else
    {
        rt_kprintf("open_lora_tx_thread_creat fail \n");
    }

#endif





    /* 测试lwip_udp */
#if( DEBUG_LWIP == 1)                                                       

    udp_test_thread_index = rt_thread_create(    "udp_test_thread"     ,    // 创建lwip_udp线程
                                            udpserv_thread_entry ,
                                            RT_NULL ,
                                            2048 ,
                                            UDP_TEST_THREAD_PRIORTY,
                                            20 );
    if( RT_NULL != udp_test_thread_index )
    {
        rt_thread_startup(udp_test_thread_index);                           // 开启线程的调度
        rt_kprintf("udp_test_thread_creat success \r\n ");
    }
    else
    {
        rt_kprintf("udp_test_thread_creat fail \r\n ");
    }

#endif





    return RT_EOK;
}




