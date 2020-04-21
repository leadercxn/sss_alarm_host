#include <string.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "ser_alpha_data_handler.h"
#include "ble_gap.h"
#include "ser_alpha.h"

#include "SEGGER_RTT.h"

/**
 * @brief 打包 控制alpha打开lora接收功能的 数据
 */
ser_pkg_to_alpha_t  open_lora_rx_pkg_enc( ser_alpha_open_rx_param_t *p_ser_alpha_open_rx_param )
{
    ser_pkg_to_alpha_t  ser_pkg_to_alpha ;        
    
    ser_pkg_to_alpha.command_type = SER_PKT_TYPE_CMD ;
    ser_pkg_to_alpha.items_index = ALPHA_CMD_LORA_OPEN_RX ;

    ser_pkg_to_alpha.data_buff = (rt_uint8_t * )p_ser_alpha_open_rx_param;
    ser_pkg_to_alpha.tx_pkg_len = sizeof(ser_alpha_open_rx_param_t) + sizeof(rt_uint8_t) + sizeof(rt_uint8_t);

    return ser_pkg_to_alpha;
}


/**
 * @brief 打包 控制alpha打开lora发送功能的 数据
 */
ser_pkg_to_alpha_t  open_lora_tx_pkg_enc( ser_alpha_open_tx_param_t *p_ser_alpha_open_tx_param )
{
    ser_pkg_to_alpha_t  ser_pkg_to_alpha ;        
    rt_uint16_t param_len = 0 ;

    ser_pkg_to_alpha.command_type = SER_PKT_TYPE_CMD ;
    ser_pkg_to_alpha.items_index = ALPHA_CMD_LORA_OPEN_TX ;
    ser_pkg_to_alpha.data_buff = ( rt_uint8_t *)p_ser_alpha_open_tx_param;
    param_len = sizeof(rt_uint32_t) + sizeof(rt_uint8_t) + sizeof(rt_uint32_t) + sizeof(rt_uint32_t) + sizeof(rt_uint8_t) \
                + sizeof(rt_uint8_t) + sizeof(rt_uint8_t) + p_ser_alpha_open_tx_param->tx_data_len;
    ser_pkg_to_alpha.tx_pkg_len = param_len + sizeof(rt_uint8_t) + sizeof(rt_uint8_t);

    return ser_pkg_to_alpha;
}






