#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "alpha_tasks.h"
#include "ser_alpha_data_handler.h"


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
        if( i > 3 )
        {
            rt_kprintf("0x%02x " , p[i-4] );
        }
        else
        {
           rt_kprintf("0x%02x " , p[i] );
        }

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
    for( rt_uint8_t i = 0 ; i < ( ser_pkg_to_alpha.tx_pkg_len + sizeof(rt_uint16_t) - p_ser_alpha_open_tx_param->tx_data_len ); i++ )       //打印buff的数据
    {
        if( i > 3 )
        {
            rt_kprintf("0x%02x " , p[i-4] );
        }
        else
        {
           rt_kprintf("0x%02x " , p[i] );
        }

        if( i == 3 )
        {
            p = ser_pkg_to_alpha.data_buff;
        }
    }

    p = p_ser_alpha_open_tx_param->tx_data ;        
    for( rt_uint8_t i = 0 ; i <  p_ser_alpha_open_tx_param->tx_data_len ; i++ )       //打印buff的数据
    {
        rt_kprintf("0x%02x " , p[i] );
    }

    rt_kprintf("\n");
}













