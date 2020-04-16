#ifndef __I2S_DMA_H
#define __I2S_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/**
 * @brief i2s_dma中断回调
 * 
 *  函数指针，让第三方把处理函数传递进来
 */
extern void (*i2s_dma_finish_irq_callback)(void);

/**
 * @brief i2s2初始化
 * 
 */
void i2s2_init(void);

/**
 * @brief 开启i2s的DMA
 * 
 */
void i2s_dma_enable(void);

/**
 * @brief 设置采样率
 */
uint8_t i2s2_samplerate_set( uint32_t samplerate );


/**
 * @brief DMA传输的数据量
 */
void dma_transmit_datalength( uint32_t datalength );

/**
 * @brief i2s tx dma配置(DMA1 ,CH0)
 * 
 */
void i2s2_tx_dma_init(uint8_t *mem_buff0 , uint8_t *mem_buff1 );

/**
 * @brief i2s开始播放
 */
void i2s_play_start(void);

/**
 * @brief i2s停止播放
 */
void i2s_play_stop(void);






#ifdef __cplusplus
}
#endif



#endif 
