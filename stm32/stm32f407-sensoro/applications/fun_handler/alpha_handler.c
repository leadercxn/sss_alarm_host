#include <stdio.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <user_board.h>

#ifndef ALPHA_DEV_NAME
#define ALPHA_DEV_NAME     "uart3"
#endif

#include <type.h>

#include "SEGGER_RTT.h"


//static struct rt_semaphore  m_rx_sem;                                       //创建接收用于接收消息的信号量
static rt_sem_t m_rx_sem = RT_NULL;
static rt_device_t          dev_uart3 = RT_NULL;                            //串口设备
struct serial_configure     uart3_config = RT_SERIAL_CONFIG_DEFAULT;        //串口配置

static rt_err_t  uart_input( rt_device_t dev , rt_size_t size );            //函数声明

static rt_mq_t              m_alpha_mq = RT_NULL ;                          //定义一个alpha的消息队列

/**
 * @brief 串口3的初始化
 */
static int dev_uart3_init(void)
{
    dev_uart3 = rt_device_find(ALPHA_DEV_NAME);                             //查找设备
    if( RT_NULL == dev_uart3 )
    {
        rt_kprintf("can't find the uart3 \r\n");
    }
    //rt_sem_init( &m_rx_sem , "m_rx_sem" , 0 ,RT_IPC_FLAG_FIFO );            //初始化静态信号量
    m_rx_sem = rt_sem_create("m_rx_sem", 0, RT_IPC_FLAG_FIFO);              //创建动态的信号量
    m_alpha_mq = rt_mq_create("m_alpha_mq", sizeof(rt_uint8_t) , 512 , RT_IPC_FLAG_FIFO );    // 创建一个FIFO消息队列

    uart3_config.baud_rate = BAUD_RATE_9600;                                //配置波特率

    rt_device_control( dev_uart3 , RT_DEVICE_CTRL_CONFIG , &uart3_config ); //配置串口
    rt_device_open( dev_uart3 , RT_DEVICE_FLAG_INT_RX );                    //中断接收模式 && 轮询发送模式
    rt_device_set_rx_indicate( dev_uart3 , uart_input );                    //设置接收回调函数

    rt_kprintf("dev_uart3_init && creat message queue success  \n");
}
INIT_COMPONENT_EXPORT(dev_uart3_init);                                      // 导出到自动初始化 

/**
 * @brief 接收回调函数
 */
static rt_err_t  uart_input( rt_device_t dev , rt_size_t size )
{
    rt_sem_release(m_rx_sem);
    return RT_EOK;
}


/**
 * @brief 串口 中断接收模式 && 接收回调函数的用法
 */
void uart3_rx_thread_entry(void *parameter)
{
    rt_uint8_t ch  ;

    while(1)
    {
        while( 1 != rt_device_read( dev_uart3 , -1 , &ch ,1 ) )
        {
            rt_sem_take(m_rx_sem , RT_WAITING_FOREVER );
            //rt_kprintf("0x%02x\n" , ch );                 //这里建议不要用串口打印，会占用CPU，串口过快会导致数据丢包
            //SEGGER_RTT_printf(0,"0x%02x\n",ch);           // 不要在这打印，会出现数据读取偏移的现象，没能好好理解
        }
        SEGGER_RTT_printf(0,"0x%02x\n",ch);
    }
}



/**
 * @brief 通过串口uart3发送命令到alpha芯片
 */
static int alpha_cmd_test( int argc, char *argv[] )
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

    send_szie = rt_device_write( dev_uart3 , 0 , send_buff , byte_index );                   //发送字符串

    return ret ;
}

/**
 * @brief 通过串口uart3发送数据到alpha芯片
 */
static int alpha_hexdata_test( int argc , char *argv[] )
{
    rt_err_t    ret = RT_EOK;
    rt_uint8_t  send_buff[128];
    rt_uint8_t  i;
    rt_size_t   send_szie = 0;

    if(1 == argc )
    {
        rt_kprintf("please input hex-data \n");
        return RT_EINVAL ;
    }

    for( i = 1 ; i < argc ; i++ )
    {
        sscanf( argv[i] , "%02x" , &send_buff[i-1] );       //注意格式
    }

    for( rt_uint8_t j = 0 ; j < (argc-1) ; j++ )
    {
        rt_kprintf("send_buff[%d] = %02x \n" , j ,send_buff[j] );
    }

    send_szie = rt_device_write( dev_uart3 , 0 , send_buff , (i-1) );                   //发送字符串

    return ret ;
}




#ifdef FINSH_USING_MSH
#include <finsh.h>
/* 导出命令到 FinSH 控制台 */
MSH_CMD_EXPORT_ALIAS( alpha_cmd_test, alpha_cmd , send cmd to alpha chip by uart3 demo: alpha_cmd gpio );
MSH_CMD_EXPORT_ALIAS( alpha_hexdata_test, alpha_hexdata , send hex-data to alpha chip by uart3 demo: alpha_hexdata 03 04 05 );
#endif /* FINSH_USING_MSH */


/**
 * @brief alpha芯片的串口通信测试
 */
static int alpha_reset(int argc, char *argv[])
{
    rt_pin_mode(ALPHA_RESET, PIN_MODE_OUTPUT);     //配置成输出

    rt_pin_write(ALPHA_RESET, PIN_LOW);
    rt_thread_mdelay(100);
    rt_pin_write(ALPHA_RESET, PIN_HIGH);

    
    HAL_GPIO_DeInit(GPIO_ALPHA_RESET, ALPHA_RESET_PIN); //配置成默认状态，别影响下载
    rt_kprintf("reset alpha finish \r\n");
    return RT_EOK;

}

#ifdef FINSH_USING_MSH
#include <finsh.h>
/* 导出命令到 FinSH 控制台 */
MSH_CMD_EXPORT_ALIAS( alpha_reset, alpha_reset , reset the alpha chip );
#endif /* FINSH_USING_MSH */





