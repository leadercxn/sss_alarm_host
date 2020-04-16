#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <user_board.h>

#ifndef UART2_NAME
#define UART2_NAME  "uart2"
#endif


static struct rt_semaphore rx_sem;  // 创建接收用于接收消息的信号量
static rt_device_t dev_uart2 ;
struct serial_configure uart2_config = RT_SERIAL_CONFIG_DEFAULT;
static rt_uint8_t m_init_done = 0 ;   //初始化完成标记

static rt_err_t  uart_input( rt_device_t dev , rt_size_t size );    //函数声明

static int dev_uart2_init(void)
{
    dev_uart2 = rt_device_find(UART2_NAME);                         //查找设备
    
    rt_sem_init( &rx_sem , "rx_sem" , 0 ,RT_IPC_FLAG_FIFO );        //初始化信号量

    rt_device_open( dev_uart2 , RT_DEVICE_FLAG_INT_RX );            //中断接收模式 && 轮询发送模式

    rt_device_set_rx_indicate( dev_uart2 , uart_input );            //设置接收回调函数

    m_init_done = 1 ;

    rt_kprintf("dev_uart2_init success \r\n");
}
//INIT_COMPONENT_EXPORT(dev_uart2_init);                              // 导出到自动初始化 


/**
 * @brief 接收回调函数
 */
static rt_err_t  uart_input( rt_device_t dev , rt_size_t size )
{
    rt_sem_release(&rx_sem);
    return RT_EOK;
}



/**
 * @brief 串口 中断接收模式 && 接收回调函数的用法
 */
static void serial2_thread_entry(void *parameter)
{
    char ch;
    while(1)
    {
        while( 1 != rt_device_read( dev_uart2 , -1 , &ch ,1 ) )
        {
            rt_sem_take(&rx_sem , RT_WAITING_FOREVER );
        }
        rt_device_write( dev_uart2 , 0 , &ch , 1 );
    }
}

/**
 * @brief demo
 */
static int uart2_sample( int argc ,char *argv[] )
{
    rt_err_t ret = RT_EOK;
    char uart_name[RT_NAME_MAX];
    char str[] = "CAO NI MA\r\n";

    rt_size_t write_size =  0 ;

    if ( 2 == argc )
    {
        rt_strncpy( uart_name , argv[1] , RT_NAME_MAX );
    }
    else
    {
        rt_strncpy( uart_name , UART2_NAME , RT_NAME_MAX  );
    }

#if 1
    if( 0 == m_init_done )
    {
        m_init_done = 1 ;

        dev_uart2 = rt_device_find(uart_name);                                          //查找设备
        if( RT_NULL != dev_uart2  )
        {
            rt_kprintf("find %s success !\n", uart_name);
        }
        else
        {
            rt_kprintf("find %s failed!\n", uart_name);
            return RT_ERROR;
        }
        
        rt_device_control( dev_uart2 , RT_DEVICE_CTRL_CONFIG , &uart2_config );

        rt_sem_init( &rx_sem , "rx_sem" , 0 ,RT_IPC_FLAG_FIFO );                         //初始化信号量

        rt_device_open( dev_uart2 , RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_INT_RX );        //中断接收模式 && 轮询发送模式

        rt_device_set_rx_indicate( dev_uart2 , uart_input );                             //设置接收回调函数
    }
#endif

#if 1
    write_size = rt_device_write( dev_uart2 , 0 , str , sizeof(str) );                   //发送字符串
#endif

    rt_thread_t thread = rt_thread_create( "serial_thread",
                                            serial2_thread_entry ,
                                            RT_NULL,
                                            1024 ,
                                            11,
                                            20);

    if( RT_NULL != thread )
    {
        rt_thread_startup(thread);
        rt_kprintf("dev_uart2 start success \r\n", uart_name);
    }
    else
    {
        ret = RT_ERROR;
    }
    
    return ret ;
}


MSH_CMD_EXPORT(uart2_sample, uart2 demo);       // 导出到 msh 命令列表中





