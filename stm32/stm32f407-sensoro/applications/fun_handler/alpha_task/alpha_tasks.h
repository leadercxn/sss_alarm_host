#ifndef __ALPHA_TASKS_H__
#define __ALPHA_TASKS_H__










/**
 * @brief 打开ser open lora功能的线程
 */
void open_lora_rx_entry(void *parameter);
    
/**
 * @brief 打开ser open lora功能的线程
 */
void open_lora_tx_entry(void *parameter);

/**
 * @brief 解析串口3接收到的数据包
 */
void parsing_rx_alpha_data(void *parameter);

#endif




