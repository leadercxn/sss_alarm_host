#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <user_board.h>

#ifndef EC200T_DEV_NAME
#define EC200T_DEV_NAME     "uart1"
#endif

#include <type.h>

#include "SEGGER_RTT.h"


static struct rt_semaphore  m_rx_sem;                                       //创建接收用于接收消息的信号量
static rt_device_t          dev_uart1 = RT_NULL;                            //串口设备
struct serial_configure     uart1_config = RT_SERIAL_CONFIG_DEFAULT;        //串口配置

static rt_err_t  uart_input( rt_device_t dev , rt_size_t size );            //函数声明

static rt_mq_t              m_alpha_mq = RT_NULL ;                          //定义一个alpha的消息队列

/**
 * @brief 串口3的初始化
 */
static int dev_uart1_init(void)
{
    dev_uart1 = rt_device_find(EC200T_DEV_NAME);                             //查找设备
    if( RT_NULL == dev_uart1 )
    {
        rt_kprintf("can't find the uart1 \r\n");
    }
    
    rt_sem_init( &m_rx_sem , "m_rx_sem" , 0 ,RT_IPC_FLAG_FIFO );            //初始化信号量
    m_alpha_mq = rt_mq_create("m_alpha_mq", sizeof(rt_uint8_t) , 512 , RT_IPC_FLAG_FIFO );    // 创建一个FIFO消息队列

    rt_device_control( dev_uart1 , RT_DEVICE_CTRL_CONFIG , &uart1_config ); //配置串口
    rt_device_open( dev_uart1 , RT_DEVICE_FLAG_INT_RX );                    //中断接收模式 && 轮询发送模式
    rt_device_set_rx_indicate( dev_uart1 , uart_input );                    //设置接收回调函数
    rt_kprintf("dev_uart1_init success \r\n");
}
INIT_COMPONENT_EXPORT(dev_uart1_init);                                      // 导出到自动初始化 

/**
 * @brief 接收回调函数
 */
static rt_err_t  uart_input( rt_device_t dev , rt_size_t size )
{
    rt_sem_release(&m_rx_sem);
    return RT_EOK;
}





/**
 * @brief 串口 中断接收模式 && 接收回调函数的用法
 */
static void serial1_thread_entry(void *parameter)
{
    rt_uint8_t ch;
    while(1)
    {
        while( 1 != rt_device_read( dev_uart1 , -1 , &ch ,1 ) )
        {
            rt_sem_take(&m_rx_sem , RT_WAITING_FOREVER );
            //rt_kprintf("%c" , ch );           //这里不要用串口打印，会占用CPU，导致数据丢包
            SEGGER_RTT_printf(0,"%c",ch);
        }
    }
}






/**
 * @brief ec200t芯片的串口通信测试
 */
static int ec200t_test(int argc, char *argv[])
{
    rt_err_t    ret = RT_EOK;
    char        send_buff[128];
    rt_uint16_t command_len = 0 ;
    rt_uint8_t  byte_index = 0;
    rt_size_t   send_szie = 0;

    for( rt_uint8_t i = 0 ; i < (argc-1) ; i++)
    {
        char *p = argv[1+i] ;
        for( rt_uint8_t j = 0 ;;j++ )
        {
            send_buff[byte_index] = *p;
            byte_index++;
            if( *p == '\0' )
            {
                break;
            }
            p++;
        }
    }

    send_buff[byte_index] = '\r';
    byte_index++;
    send_buff[byte_index] = '\n';
    byte_index++;

    send_szie = rt_device_write( dev_uart1 , 0 , send_buff , byte_index );                   //发送字符串

    static bool thread_create_done = false ;                                                 //线程是否已创建标记

    if( false == thread_create_done )
    {
        rt_thread_t thread = rt_thread_create( "ec200t_thread",                               //创建接收的线程
                                                    serial1_thread_entry ,
                                                    RT_NULL,
                                                    1024 ,
                                                    12,
                                                    20);

            if( RT_NULL != thread )
            {
                rt_thread_startup(thread);
                rt_kprintf("ec200t_thread start success \r\n");
                thread_create_done = true;
            }
            else
            {
                ret = RT_ERROR;
            }

    }
    return ret ;
}


#ifdef FINSH_USING_MSH
#include <finsh.h>
/* 导出命令到 FinSH 控制台 */
MSH_CMD_EXPORT_ALIAS( ec200t_test, ec200t , send data to ec200t chip by uart1 );
#endif /* FINSH_USING_MSH */






