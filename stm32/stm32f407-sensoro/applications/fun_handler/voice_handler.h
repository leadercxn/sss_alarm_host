#ifndef __VOICE_HANDLER_H
#define __VOICE_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"


/**
 * @brief 语音初始化
 */
void voice_handler_init(void);

/**
 * @brief 开始播放序号语音
 */
void play_voice_start( uint16_t voice_index );


#ifdef __cplusplus
}
#endif



#endif 


