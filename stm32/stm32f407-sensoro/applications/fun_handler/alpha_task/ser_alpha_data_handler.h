#ifndef __SER_ALPHA_DATA_HANDLER_H
#define __SER_ALPHA_DATA_HANDLER_H

#include "type.h"

/**@brief The types of serialization packets. */
typedef enum
{
    SER_PKT_TYPE_CMD = 0,     /**< Command packet type. */
    SER_PKT_TYPE_RESP,        /**< Command Response packet type. */
    SER_PKT_TYPE_EVT,         /**< Event packet type. */
    SER_PKT_TYPE_DTM_CMD,     /**< DTM Command packet type. */
    SER_PKT_TYPE_DTM_RESP,    /**< DTM Response packet type. */
    SER_PKT_TYPE_RESET_CMD,   /**< System Reset Command packet type. */
    SER_PKT_TYPE_MAX          /**< Upper bound. */
} ser_pkt_type_t;

/**
 * @brief ser发送到alpha端的数据结构
 */
typedef struct 
{
    rt_uint16_t tx_pkg_len ;
    rt_uint8_t  command_type ;
    rt_uint8_t  items_index ;
    rt_uint8_t  *data_buff ;                            //这种实现的方法有个弊端，指针指向的结构体内存里还存在指针
}  __attribute__ ((__packed__)) ser_pkg_to_alpha_t ;

/**
 * @brief ser发送到alpha端的数据结构
 */
typedef struct 
{
    rt_uint16_t rx_pkg_len ;
    rt_uint8_t  type_resp ;
    rt_uint8_t  items_index ;
    rt_uint32_t err_code ;
    rt_uint8_t  *data_buff ;                            //这种实现的方法有个弊端，指针指向的结构体内存里还存在指针
}  __attribute__ ((__packed__)) alpha_pkg_to_ser_t ;




/**
 * @brief 控制alpha打开lora接收功能的参数
 */
typedef struct 
{
    rt_uint32_t freq ;
    rt_uint32_t bandwidth ;
    rt_uint32_t daterate  ;
    rt_uint8_t  coderate ;
    rt_uint16_t preamblelen ;
    rt_uint16_t symbtimeout ;
    bool  crcon ;
    bool  iqinverted ;
} __attribute__ ((__packed__)) ser_alpha_open_rx_param_t ;

/**
 * @brief 控制alpha打开lora发送功能的参数
 */
typedef struct 
{
    rt_uint32_t freq ;
    rt_int8_t   power;
    rt_uint32_t bandwidth ;
    rt_uint32_t daterate  ;
    rt_uint8_t  coderate ;
    rt_uint16_t preamblelen ;
    bool  crcon ;
    bool  iqinverted ;
    rt_uint32_t timeout ;
    rt_uint16_t tx_data_len ;
    rt_uint8_t  *tx_data ;
} __attribute__ ((__packed__)) ser_alpha_open_tx_param_t ;









/**
 * @brief 打包 控制alpha打开lora接收功能的 数据
 */
ser_pkg_to_alpha_t  open_lora_rx_pkg_enc( ser_alpha_open_rx_param_t *p_ser_alpha_open_rx_param );

/**
 * @brief 打包 控制alpha打开lora发送功能的 数据
 */
ser_pkg_to_alpha_t  open_lora_tx_pkg_enc( ser_alpha_open_tx_param_t *p_ser_alpha_open_tx_param );


#endif


