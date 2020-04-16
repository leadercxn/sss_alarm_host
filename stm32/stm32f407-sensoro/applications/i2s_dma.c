#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "stm32f4xx_hal.h"

#include "type.h"
#include "i2s_dma.h"




#define I2S2_SPI_INSTANCE      SPI2
#define USER_DMA1_STREAM       DMA1_Stream4
#define USER_DMA1_CHANNEL      DMA_CHANNEL_0

I2S_HandleTypeDef m_i2s2_handler;		//定义i2s句柄
DMA_HandleTypeDef m_dma_handler;        //定义DMA句柄

static uint32_t  i2s2_standard       = I2S_STANDARD_PHILIPS ;   // i2s 标准
static uint32_t  i2s2_mode           = I2S_MODE_MASTER_TX ;     // i2s 模式
static uint32_t  i2s2_clock_polarity = I2S_CPOL_LOW ;           // i2s 时钟电平
static uint32_t  i2s2_dataformat     = I2S_DATAFORMAT_16B ;     // i2s 数据长度

static uint32_t   m_transmit_data_length  = 0 ;                  // DMA 传输的数据量

static bool    m_i2s_stop_flag       = false ;

/**
 * @brief i2s_dma中断回调
 * 
 *  函数指针，让第三方把处理函数传递进来
 */
void (*i2s_dma_finish_irq_callback)(void);

typedef struct 
{
    uint16_t samplerate_div_10 ;
    uint16_t plli2sn ;
    uint16_t plli2sr ;
    uint16_t i2sdiv  ;
    uint16_t odd     ;
} samplerate_member_t ;

/** 
 * 采样率表格 ，计算公式: fs = i2s_clk / [ 256 * (2*i2sdiv + ODD ) ] 
 *                      i2s_clk = (HSE/pllm) * PLLI2SN/PLLI2SR
 *                      HSE   = 8MHz (一般情况)
 *                      pllm  = 8    (一般情况)
 *                      PLLI2SN : 192 ~ 432
 *                      PLLI2SR : 2 ~ 7
 *                      i2sdiv  : 2 ~ 255
 *                      ODD     : 0/1
 * 
 * 表格格式:  采样率/10 , PLLI2SN , PLLI2SR , I2SDIV , ODD
 */
const samplerate_member_t i2s_psc_tbl[] =
{
    {
        .samplerate_div_10 = 800 ,
        .plli2sn           = 256 ,
        .plli2sr           = 5   ,
        .i2sdiv            = 12  ,
        .odd               = 1   ,
    },
    {
        .samplerate_div_10 = 1102 ,
        .plli2sn           = 429  ,
        .plli2sr           = 4    ,
        .i2sdiv            = 19   ,
        .odd               = 0    ,
    },
    {
        .samplerate_div_10 = 1600 ,
        .plli2sn           = 213  ,
        .plli2sr           = 2    ,
        .i2sdiv            = 13   ,
        .odd               = 0    ,
    },
    {
        .samplerate_div_10 = 2205 ,
        .plli2sn           = 429  ,
        .plli2sr           = 4    ,
        .i2sdiv            = 9    ,
        .odd               = 1    ,
    },
    {
        .samplerate_div_10 = 3200 ,
        .plli2sn           = 213  ,
        .plli2sr           = 2    ,
        .i2sdiv            = 6    ,
        .odd               = 1    ,
    },
    {
        .samplerate_div_10 = 4410 ,
        .plli2sn           = 271  ,
        .plli2sr           = 2    ,
        .i2sdiv            = 6    ,
        .odd               = 0    ,
    },
    {
        .samplerate_div_10 = 4800 ,
        .plli2sn           = 258  ,
        .plli2sr           = 3    ,
        .i2sdiv            = 3    ,
        .odd               = 1    ,
    },
    {
        .samplerate_div_10 = 8820 ,
        .plli2sn           = 316  ,
        .plli2sr           = 2    ,
        .i2sdiv            = 3    ,
        .odd               = 1    ,
    },
    {
        .samplerate_div_10 = 9600 ,
        .plli2sn           = 344  ,
        .plli2sr           = 2    ,
        .i2sdiv            = 3    ,
        .odd               = 1    ,
    },
    {
        .samplerate_div_10 = 17640,
        .plli2sn           = 361  ,
        .plli2sr           = 2    ,
        .i2sdiv            = 2    ,
        .odd               = 0    ,
    },
    {
        .samplerate_div_10 = 19200,
        .plli2sn           = 393  ,
        .plli2sr           = 2    ,
        .i2sdiv            = 2    ,
        .odd               = 0    ,
    },

};


/**
 * @brief i2s2初始化
 * 
 */
void i2s2_init(void)
{
    m_i2s2_handler.Instance         = I2S2_SPI_INSTANCE ;              // i2s2硬件映射的SPI2
    m_i2s2_handler.Init.Mode        = i2s2_mode ;                      // i2s2模式
    m_i2s2_handler.Init.Standard    = i2s2_standard ;                  // i2s的标准
    m_i2s2_handler.Init.DataFormat  = i2s2_dataformat ;                // i2s的数据格式
    m_i2s2_handler.Init.MCLKOutput  = I2S_MCLKOUTPUT_ENABLE ;          // 主时钟输出使能
    m_i2s2_handler.Init.AudioFreq   = I2S_AUDIOFREQ_DEFAULT;           // i2s频率设置
    m_i2s2_handler.Init.CPOL        = i2s2_clock_polarity ;            // i2s时钟电平
	m_i2s2_handler.Init.ClockSource = I2S_CLOCK_PLL;		           // i2s时钟源为PLL
	HAL_I2S_Init(&m_i2s2_handler);                                     // i2s2初始化

    I2S2_SPI_INSTANCE->CR2 |= 1<<1 ;                                   // SPI2 TX DMA请求使能
	__HAL_I2S_ENABLE(&m_i2s2_handler);					               // 使能I2S2
}


/**
 * @brief 开启i2s的DMA
 * 
 */
void i2s_dma_enable(void)
{
    uint32_t temp_reg = 0 ;

    temp_reg = I2S2_SPI_INSTANCE->CR2 ;    	//  读出先前寄存器		
	temp_reg |= 1<<1 ;			            //  使能DMA
	I2S2_SPI_INSTANCE->CR2 = temp_reg;		//  写入CR2寄存器
}

/**
 * @brief 设置采样率
 * 采样率表格 ，计算公式: fs = i2s_clk / [ 256 * (2*i2sdiv + ODD ) ] 
 *                      i2s_clk = (HSE/pllm) * PLLI2SN/PLLI2SR
 *                      HSE   = 8MHz (一般情况)
 *                      pllm  = 8    (一般情况)
 *                      PLLI2SN : 192 ~ 432
 *                      PLLI2SR : 2 ~ 7
 *                      i2sdiv  : 2 ~ 255
 *                      ODD     : 0/1
 */
uint8_t i2s2_samplerate_set( uint32_t samplerate )
{
    uint8_t  i  = 0 ;
    uint32_t temp_reg = 0 ;
    uint8_t  sample_table_taotal = sizeof(i2s_psc_tbl)/sizeof(samplerate_member_t) ;    // 计算采样表有多少组数据

    RCC_PeriphCLKInitTypeDef RCCI2S2_ClkInitSture ;     // 定义控制i2s时钟控制结构体

    for( i = 0 ; i < sample_table_taotal ; i++ )        //根据预设的采样表来查表
    {
        if ( i2s_psc_tbl[i].samplerate_div_10 == samplerate/10 )       
        {
            break;
        }
    }

    if( i == sample_table_taotal )                      // 在采样表里查不到要设置的采样率
    {
        return 1;
    }

    RCCI2S2_ClkInitSture.PeriphClockSelection = RCC_PERIPHCLK_I2S ;             
    RCCI2S2_ClkInitSture.PLLI2S.PLLI2SN = (uint32_t) i2s_psc_tbl[i].plli2sn ;
    RCCI2S2_ClkInitSture.PLLI2S.PLLI2SR = (uint32_t) i2s_psc_tbl[i].plli2sr ;
    HAL_RCCEx_PeriphCLKConfig(&RCCI2S2_ClkInitSture);     //设置时钟

    RCC->CR |= 1<<26;                                     // 开启I2S时钟
    while( (RCC->CR&1<<27) == 0 );		                  // 等待I2S时钟开启成功

    temp_reg = i2s_psc_tbl[i].i2sdiv << 0 ;               // 设置i2sdiv
    temp_reg |= i2s_psc_tbl[i].odd << 8 ;                 // 设置odd位

    temp_reg |= 1 << 9 ;                                  // 使能MCKOE位，输出MCLK
    I2S2_SPI_INSTANCE->I2SPR = temp_reg ;                 // 设置I2SPR寄存器

    return 0 ;
}

/**
 * @brief DMA传输的数据量
 */
void dma_transmit_datalength( uint32_t dataLength )
{
    m_transmit_data_length = dataLength ;

    m_dma_handler.Instance->NDTR = dataLength;
}


/**
 * @brief i2s tx dma配置(DMA1 ,CH0)
 * 
 */
void i2s2_tx_dma_init(uint8_t *mem_buff0 , uint8_t *mem_buff1 )
{
    __HAL_RCC_DMA1_CLK_ENABLE();                                // 使能DMA1时钟
    __HAL_LINKDMA( &m_i2s2_handler, hdmatx, m_dma_handler);     // 将DMA与I2S联系起来

    m_dma_handler.Instance          = USER_DMA1_STREAM  ;                   // DMA1数据流
    m_dma_handler.Init.Channel      = USER_DMA1_CHANNEL ;                   // DMA1通道
    m_dma_handler.Init.Direction    = DMA_MEMORY_TO_PERIPH;         		// DMA1方向: 储存器 -> 外设
    m_dma_handler.Init.PeriphInc    = DMA_PINC_DISABLE;             		// 外设非增量模式
    m_dma_handler.Init.MemInc       = DMA_MINC_ENABLE;                 		// 储存器增量模式
    m_dma_handler.Init.PeriphDataAlignment  = DMA_PDATAALIGN_HALFWORD;      // 外设数据长度 16位  	
    m_dma_handler.Init.MemDataAlignment     = DMA_MDATAALIGN_HALFWORD;      // 储存器数据长度  	
    m_dma_handler.Init.Mode         = DMA_CIRCULAR;                      	// 使用循环模式	
    m_dma_handler.Init.Priority     = DMA_PRIORITY_HIGH;             		// 高优先级
    m_dma_handler.Init.FIFOMode     = DMA_FIFOMODE_DISABLE;          		// 不使用FIFO
    m_dma_handler.Init.MemBurst     = DMA_MBURST_SINGLE;             		// 储存器突发单次传输
    m_dma_handler.Init.PeriphBurst  = DMA_PBURST_SINGLE;          		    // 外设突发单次传输
    HAL_DMA_DeInit(&m_dma_handler);                            		        // 消除以前的设置
    HAL_DMA_Init(&m_dma_handler);	                                        // 初始化DMA

    HAL_DMAEx_MultiBufferStart(&m_dma_handler, (uint32_t)mem_buff0, (uint32_t)&I2S2_SPI_INSTANCE->DR, (uint32_t)mem_buff1, m_transmit_data_length ); //开启双缓冲
    __HAL_DMA_DISABLE( &m_dma_handler );                         		    // 先关闭DMA
    #if 0
    delay_us(10);                                                   		// 10us延时，防止优化出现问题 
    #endif	
    __HAL_DMA_ENABLE_IT( &m_dma_handler,  DMA_IT_TC );             		    // 开启传输完成中断
    __HAL_DMA_CLEAR_FLAG( &m_dma_handler, DMA_FLAG_TCIF0_4 );     		    // 消除DMA传输完成中断标志位
    HAL_NVIC_SetPriority( DMA1_Stream4_IRQn, 0, 0 );                    	// DMA中断优先级
    HAL_NVIC_EnableIRQ( DMA1_Stream4_IRQn );

}

/**
 * @brief DMA1 数据流4产生的中断
 * 
 */
void DMA1_Stream4_IRQHandler(void)
{  
    if(__HAL_DMA_GET_FLAG( &m_dma_handler, DMA_FLAG_TCIF0_4) != RESET)    // DMA传输完成
    {
        __HAL_DMA_CLEAR_FLAG( &m_dma_handler, DMA_FLAG_TCIF0_4 );         // 消除DMA传输完成的中断标志位

        if( false == m_i2s_stop_flag )                                    // 停止了就不再调用
        {
            i2s_dma_finish_irq_callback();	                              // i2s DMA传输完成回调 
        }
    } 
} 

/**
 * @brief i2s开始播放
 */
void i2s_play_start(void)
{   	  
    m_i2s_stop_flag = false ;
	__HAL_DMA_ENABLE(&m_dma_handler);                                     // 开启DMA TX 传输
    __HAL_DMA_ENABLE_IT( &m_dma_handler,  DMA_IT_TC );             		  // 开启传输完成中断
}

/**
 * @brief i2s停止播放
 */
void i2s_play_stop(void)
{   
    m_i2s_stop_flag = true ;
	__HAL_DMA_DISABLE(&m_dma_handler);                                     // 禁止DMA	
    __HAL_DMA_DISABLE_IT( &m_dma_handler,  DMA_IT_TC );             	   // 禁止传输完成中断
} 



