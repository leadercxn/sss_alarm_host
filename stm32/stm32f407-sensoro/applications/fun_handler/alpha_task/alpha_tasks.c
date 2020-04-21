#include <stdio.h>
#include <string.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "alpha_tasks.h"
#include "ser_alpha_data_handler.h"
#include "alpha_handler.h"
#include "ser_alpha.h"
#include "SEGGER_RTT.h"
#include "usr_util.h"

/**
 * @brief 打开ser open lora功能的线程
 */
void open_lora_rx_entry(void *parameter)
{
    ser_pkg_to_alpha_t  ser_pkg_to_alpha ;
    ser_pkg_to_alpha = open_lora_rx_pkg_enc( ( ser_alpha_open_rx_param_t *) parameter );

    rt_kprintf("tx_pkg_len = %d \n" , ( ser_pkg_to_alpha.tx_pkg_len + sizeof(rt_uint16_t) ) );
    rt_uint8_t *p = (rt_uint8_t *)&ser_pkg_to_alpha;
    rt_kprintf("pkg:");
    for( rt_uint8_t i = 0 ; i < ( ser_pkg_to_alpha.tx_pkg_len + sizeof(rt_uint16_t) ); i++ )       //打印数据包内容
    {
        rt_kprintf("0x%02x " , *p );
        rt_device_write( dev_uart3 , 0 , p , 1 );       //串口发送                                                
        p ++ ;
        if( i == 3 )
        {
            p = ser_pkg_to_alpha.data_buff;
        }
    }
    rt_kprintf("\n");
}



/**
 * @brief 打开ser open lora功能的线程
 */
void open_lora_tx_entry(void *parameter)
{
    ser_alpha_open_tx_param_t *p_ser_alpha_open_tx_param = ( ser_alpha_open_tx_param_t *) parameter ;   //传参
    ser_pkg_to_alpha_t  ser_pkg_to_alpha ;
    ser_pkg_to_alpha = open_lora_tx_pkg_enc( ( ser_alpha_open_tx_param_t *) parameter );

    rt_kprintf("tx_pkg_len = %d \n" , ( ser_pkg_to_alpha.tx_pkg_len + sizeof(rt_uint16_t) ) );
    rt_uint8_t *p = (rt_uint8_t *)&ser_pkg_to_alpha;
    rt_kprintf("pkg:");
    for( rt_uint8_t i = 0 ; i < ( ser_pkg_to_alpha.tx_pkg_len + sizeof(rt_uint16_t) ); i++ )       //打印buff的数据
    {
        rt_kprintf("0x%02x " , *p );
        rt_device_write( dev_uart3 , 0 , p , 1 );       //串口发送 
        p ++ ;
        if( i == 3 )
        {
            p = ser_pkg_to_alpha.data_buff;
        }
    }

    rt_kprintf("\n");
}


/**
 * @brief 解析串口3接收到的数据包
 */
void parsing_rx_alpha_data(void *parameter)
{
    rt_uint8_t buff[128];

    while(1)
    {
        if( rt_mq_recv( gp_alpha_rx_mq , buff , sizeof(buff) , RT_WAITING_FOREVER ) == RT_EOK )
        {
            SEGGER_RTT_printf(0,"recv a mq \n");
            
            switch(buff[1])
            {
                case ALPHA_ALIVE_ACCESS:
                    rt_kprintf("alpha chip is alive \n");
                break;




            }
        }
    }
}



/**
 * @brief 通过串口uart3发送命令到alpha芯片
 */
static int open_lora_rx( int argc, char *argv[] )
{
    ser_pkg_to_alpha_t  ser_pkg_to_alpha ;

    ser_alpha_open_rx_param_t ser_alpha_open_rx_param ;
    ser_alpha_open_rx_param.freq = 433 * 1e6  ;
    ser_alpha_open_rx_param.bandwidth = 0 ; 
    ser_alpha_open_rx_param.daterate = 11 ; 
    ser_alpha_open_rx_param.crcon = false ;
    ser_alpha_open_rx_param.iqinverted = false ;

    ser_pkg_to_alpha = open_lora_rx_pkg_enc( &ser_alpha_open_rx_param );

    rt_kprintf("tx_pkg_len = %d \n" , ( ser_pkg_to_alpha.tx_pkg_len + sizeof(rt_uint16_t) ) );
    rt_uint8_t *p = (rt_uint8_t *)&ser_pkg_to_alpha;
    rt_kprintf("pkg:");
    for( rt_uint8_t i = 0 ; i < ( ser_pkg_to_alpha.tx_pkg_len + sizeof(rt_uint16_t) ); i++ )       //打印数据包内容
    {
        rt_kprintf("0x%02x " , *p );
        rt_device_write( dev_uart3 , 0 , p , 1 );       //串口发送                                                
        p ++ ;
        if( i == 3 )
        {
            p = ser_pkg_to_alpha.data_buff;
        }
    }
    rt_kprintf("\n");
}


/**
 * @brief 通过串口uart3发送命令到alpha芯片
 */
static int open_lora_tx( int argc, char *argv[] )
{
    ser_pkg_to_alpha_t  ser_pkg_to_alpha ;
    rt_uint8_t tx_buf[]={ 0x01 , 0x02 ,0x03 ,0x04 ,0x05 ,0x06 ,0x07 ,0x08 ,0x09 ,0x0A };
    ser_alpha_open_tx_param_t  ser_alpha_open_tx_param ;
    ser_alpha_open_tx_param.freq = 433 * 1e6 ;
    ser_alpha_open_tx_param.power = 14 ;
    ser_alpha_open_tx_param.bandwidth = 0 ;
    ser_alpha_open_tx_param.daterate = 11 ;
    ser_alpha_open_tx_param.crcon =  false ;
    ser_alpha_open_tx_param.iqinverted = false ;
    ser_alpha_open_tx_param.tx_data_len = sizeof(tx_buf);
    memcpy( ser_alpha_open_tx_param.tx_data , tx_buf , sizeof(tx_buf) );

    ser_pkg_to_alpha = open_lora_tx_pkg_enc( &ser_alpha_open_tx_param );

    rt_kprintf("tx_pkg_len = %d \n" , ( ser_pkg_to_alpha.tx_pkg_len + sizeof(rt_uint16_t) ) );
    rt_uint8_t *p = (rt_uint8_t *)&ser_pkg_to_alpha;
    rt_kprintf("pkg:");
    for( rt_uint8_t i = 0 ; i < ( ser_pkg_to_alpha.tx_pkg_len + sizeof(rt_uint16_t) ); i++ )       //打印buff的数据
    {
        rt_kprintf("0x%02x " , *p );
        rt_device_write( dev_uart3 , 0 , p , 1 );       //串口发送 
        p ++ ;
        if( i == 3 )
        {
            p = ser_pkg_to_alpha.data_buff;
        }
    }

    rt_kprintf("\n");
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
/* 导出命令到 FinSH 控制台 */
MSH_CMD_EXPORT_ALIAS( open_lora_rx, open_lora_rx , set alpha lora in rx mode );
MSH_CMD_EXPORT_ALIAS( open_lora_tx, open_lora_tx , set alpha lora in tx mode );
#endif /* FINSH_USING_MSH */










