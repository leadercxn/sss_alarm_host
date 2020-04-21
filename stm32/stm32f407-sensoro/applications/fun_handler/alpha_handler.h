#ifndef __ALPHA_HANDLER_H__
#define __ALPHA_HANDLER_H__


extern rt_device_t          dev_uart3;
extern rt_mq_t              gp_alpha_rx_mq;

/**
 * @brief 串口 中断接收模式 && 接收回调函数的用法
 */
void uart3_rx_thread_entry(void *parameter);








#endif




