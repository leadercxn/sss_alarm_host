#include <string.h>
#include "voice_handler.h"

#include "type.h"
#include "i2s_dma.h"
#include "w25qxx.h"
#include "tas2505.h"


#define I2S_BUFF_SIZE       1024
#define MAX_VOICE_COUNT     16
#define HEADER_START_ADDR   0x00000000

#define  CHECK_SEND_DUAL                                        // 宏: 来查看日志发送了多少个双字节

typedef struct              
{               
    uint32_t start_addr ;                                       // 语音的开始地址
    uint32_t size       ;                                       // 语音条的大小
} voice_info_t ;                

typedef struct              
{               
    uint32_t        voice_count;                                // 语音条的数量
    voice_info_t    info[MAX_VOICE_COUNT];                      // 语音条信息的结构
} voice_header_t;                                               // flash区头的数据结构(详细看语雀)

                                                                // 因为使用了STM32 DMA双缓存
static  uint8_t  m_i2s_buffer0[ I2S_BUFF_SIZE ] = { 0 } ;       // 只能定义全局静态变量，定义指针的话，会有BUG
static  uint8_t  m_i2s_buffer1[ I2S_BUFF_SIZE ] = { 0 } ;       // 只能定义全局静态变量，定义指针的话，会有BUG

static  voice_header_t  m_voice_head  ;                         // 语音数据头

typedef struct 
{
    uint16_t index                  ;                           // 当前语音的序号
    uint32_t voice_start_addr       ;                           // 语音的起始地址
    uint32_t voice_total_size       ;                           // 语音长度
    uint32_t voice_remain_size      ;                           // 剩余语音长度
} voice_param_t ;
static voice_param_t m_voice_param  ;                           // 当前播放语音相关参数   

static uint8_t  m_using_mem_buff = 0 ;                          // 使用完哪个mem传输，默认一开始是用mem0
static uint32_t m_sample_rate    = 11020 ;                      // 定义采样率

#ifdef  CHECK_SEND_DUAL
static uint32_t m_send_dual_byte   = 0 ;
#endif

/**
 * @brief 获取数据头结构
 */
static void voice_header_get( voice_header_t *p_voice_head )
{
    uint32_t voice_count = 0 ;
    int err_code = RT_EOK ;

    err_code = w25qxx_read( HEADER_START_ADDR , (uint8_t *)&voice_count , sizeof(uint32_t) );

    err_code = w25qxx_read( HEADER_START_ADDR , (uint8_t *)p_voice_head , ( sizeof(uint32_t) + voice_count * sizeof(voice_info_t) ) );
#if 0
    SEGGER_RTT_printf(0,"voice_count = %d  \r\n" , voice_count );
    SEGGER_RTT_printf(0,"info[0].start_addr = 0x%08x  \r\n" , p_voice_head->info[0].start_addr );
    SEGGER_RTT_printf(0,"info[0].size = 0x%08x  \r\n" , p_voice_head->info[0].size );
    SEGGER_RTT_printf(0,"info[1].start_addr = 0x%08x  \r\n" , p_voice_head->info[1].start_addr );
    SEGGER_RTT_printf(0,"info[1].size = 0x%08x  \r\n" , p_voice_head->info[1].size );
#endif
}

/**
 * @brief 开始播放序号语音
 * 
 *  处理事项: 根据语音的文件的大小来填充两个双缓冲区
 */
void play_voice_start( uint16_t voice_index )
{
    

    int  err = 0 ;

    m_voice_param.index = voice_index ;
    m_voice_param.voice_start_addr  = m_voice_head.info[voice_index].start_addr ;   // 获取语音的起始地址
    m_voice_param.voice_total_size  = m_voice_head.info[voice_index].size ;         // 获取语音的大小
    m_voice_param.voice_remain_size = m_voice_head.info[voice_index].size ;         // 语音剩余长度为总语音尺寸的大小

#if 0
    SEGGER_RTT_printf(0," play_voice_start  \r\n" );
    SEGGER_RTT_printf(0,"index = %d  \r\n" , m_voice_param.index );
    SEGGER_RTT_printf(0,"voice_start_addr = 0x%08x  \r\n" , m_voice_param.voice_start_addr );
    SEGGER_RTT_printf(0,"voice_total_size = 0x%08x  \r\n" , m_voice_param.voice_total_size );
    SEGGER_RTT_printf(0,"voice_remain_size = 0x%08x  \r\n",m_voice_param.voice_remain_size );
#endif

    #ifdef  CHECK_SEND_DUAL
        m_send_dual_byte   = 0 ;
    #endif

    m_using_mem_buff = 0 ;                                                                              // 一开始默认使用mem0地址

    if ( m_voice_param.voice_total_size >= I2S_BUFF_SIZE )                                              // 语音长度超1K
    {

        dma_transmit_datalength( ( I2S_BUFF_SIZE/sizeof(uint16_t) ) );                                  // dma传输的数据量
        err = w25qxx_read( m_voice_param.voice_start_addr , m_i2s_buffer0 , I2S_BUFF_SIZE );            // 先往mem0地址填充数据
        if( (-RT_ERROR) == err )                                              
        {
#if 0
            SEGGER_RTT_printf(0," w25q read voice fail  \r\n" );
#endif
        }
        m_voice_param.voice_remain_size = m_voice_param.voice_remain_size - I2S_BUFF_SIZE ;             // 剩余的size = 总剩余size - 这次使用的size

        #ifdef CHECK_SEND_DUAL
            m_send_dual_byte += I2S_BUFF_SIZE / 2 ;                                                     // 使用了多少个双字节
        #endif

        if( m_voice_param.voice_remain_size > I2S_BUFF_SIZE )                                           // 剩余的语音数据的超1K
        {
            err = w25qxx_read( (m_voice_param.voice_start_addr + ( m_voice_param.voice_total_size - m_voice_param.voice_remain_size ) ) , m_i2s_buffer1 , I2S_BUFF_SIZE );   // 再往mem1地址填充数据
            if( (-RT_ERROR) == err )                                              
            {
#if 0
                SEGGER_RTT_printf(0," w25q read voice fail  \r\n" );
#endif
            }
            m_voice_param.voice_remain_size = m_voice_param.voice_remain_size - I2S_BUFF_SIZE ;         // 剩余的size = 总剩余size - 这次使用的size
            #ifdef CHECK_SEND_DUAL
            m_send_dual_byte += I2S_BUFF_SIZE / 2 ;                                                       // 使用了多少个双字节
            #endif
        }
        else                                                                                            // 剩余的语音数据的不超1K
        {
            err = w25qxx_read( (m_voice_param.voice_start_addr + ( m_voice_param.voice_total_size - m_voice_param.voice_remain_size ) ) , m_i2s_buffer1 , m_voice_param.voice_remain_size );   // 再往mem1地址填充数据
            if( (-RT_ERROR) == err )                                              
            {
#if 0
                SEGGER_RTT_printf(0," w25q read voice fail  \r\n" );
#endif
            }
            
            #ifdef CHECK_SEND_DUAL
                m_send_dual_byte += m_voice_param.voice_remain_size / 2 ;                               // 使用了多少个双字节
            #endif

            m_voice_param.voice_remain_size = 0 ;                                                       // 剩余的size = 总剩余size - 这次使用的size
        }
    }
    else                                                                                                // 语音总长度不超过1k的
    {
        err = w25qxx_read( m_voice_param.voice_start_addr , m_i2s_buffer0 , m_voice_param.voice_total_size );   // 一次全装数据

        #ifdef CHECK_SEND_DUAL
            m_send_dual_byte += m_voice_param.voice_total_size / 2 ;                                    // 使用了多少个双字节
        #endif

        if( (-RT_ERROR) == err )                                                  
        {
#if 0
            SEGGER_RTT_printf(0," w25q read voice fail  \r\n" );
#endif
        }
        m_voice_param.voice_remain_size = 0 ;                                                           // 剩余为0

        dma_transmit_datalength( (m_voice_param.voice_total_size/sizeof(uint16_t)) );                   // 设置DMA传输的数据长度

        m_using_mem_buff = 2 ;                                                                          // 播放完即可停止
    }   
    
    tas2505_output_set(true);                                                                           // 先打开语音输出
    i2s_play_start();                                                                                   // 读完数据之后，即可开启i2s的数据传输
}

/**
 * @brief i2s完成一次传输回调处理函数
 */
static void i2s_dma_finish_handler(void)
{
    int  err = 0 ;

    /**
     *  @brief  也可以通过 DMA1_Stream4->CR & bit19(CT位) 来判断当前准备使用的储存器0 or 储存器1
     */
#if 0
    if( DMA1_Stream4->CR & (1<<19) )
    {
        SEGGER_RTT_printf(0," ready using mem1   \r\n" );
    }
    else
    {
        SEGGER_RTT_printf(0," ready using mem0   \r\n" );
    }
#endif

    if( 0 == m_using_mem_buff )                                                                          // mem0使用完就往mem0地址里面读入数据
    {
        if( m_voice_param.voice_remain_size >= I2S_BUFF_SIZE )                                           // 剩余的语音数据的超1K
        {
            err = w25qxx_read( (m_voice_param.voice_start_addr + ( m_voice_param.voice_total_size - m_voice_param.voice_remain_size ) ) , m_i2s_buffer0 , I2S_BUFF_SIZE );   // 再往mem0地址填充数据
            if( (-RT_ERROR) == err )                                              
            {
#if 0
                SEGGER_RTT_printf(0," w25q read voice fail  \r\n" );
#endif
            }

            #ifdef CHECK_SEND_DUAL
                m_send_dual_byte += I2S_BUFF_SIZE / 2 ;                                                   // 使用了多少个双字节
            #endif
            m_voice_param.voice_remain_size = m_voice_param.voice_remain_size - I2S_BUFF_SIZE ;         // 剩余的size = 总剩余size - 这次使用的size
        }
        else                                                                                            // 剩余的语音数据的不超1K
        {
            err = w25qxx_read( (m_voice_param.voice_start_addr + ( m_voice_param.voice_total_size - m_voice_param.voice_remain_size ) ) , m_i2s_buffer0 , m_voice_param.voice_remain_size );   // 再往mem0地址填充数据
            if( false == err )                                              
            {
#if 0
                SEGGER_RTT_printf(0," w25q read voice fail  \r\n" );
#endif
            }
            dma_transmit_datalength( (m_voice_param.voice_remain_size/sizeof(uint16_t)) );              // 设置DMA传输的数据长度

            #ifdef CHECK_SEND_DUAL
                m_send_dual_byte += m_voice_param.voice_remain_size / 2 ;                                 // 使用了多少个双字节
            #endif

            m_voice_param.voice_remain_size = 0 ;                                                       // 剩余的size = 总剩余size - 这次使用的size
        }

        if( 0 == m_voice_param.voice_remain_size )                                                      // 无剩余了
        {
            m_using_mem_buff = 2 ;                                                                      // 下次即可停止 i2s_dma
        }
        else                                                                                            // 还有剩余的数据
        {
            m_using_mem_buff = 1 ;                                                                      // 下次切换到其他mem1
        }
    }
    else if( 1 == m_using_mem_buff )                                                                    // mem1使用完就往mem1地址里面读入数据
    {
        if( m_voice_param.voice_remain_size >= I2S_BUFF_SIZE )                                           // 剩余的语音数据的超1K
        {
            err = w25qxx_read( (m_voice_param.voice_start_addr + ( m_voice_param.voice_total_size - m_voice_param.voice_remain_size ) ) , m_i2s_buffer1 , I2S_BUFF_SIZE );   // 再往mem1地址填充数据
            if( (-RT_ERROR) == err )                                              
            {
#if 0
                SEGGER_RTT_printf(0," w25q read voice fail  \r\n" );
#endif
            }
            #ifdef CHECK_SEND_DUAL
                m_send_dual_byte += I2S_BUFF_SIZE / 2 ;                                                   // 使用了多少个双字节
            #endif
            m_voice_param.voice_remain_size = m_voice_param.voice_remain_size - I2S_BUFF_SIZE ;         // 剩余的size = 总剩余size - 这次使用的size
        }
        else                                                                                            // 剩余的语音数据的不超1K
        {
            err = w25qxx_read( (m_voice_param.voice_start_addr + ( m_voice_param.voice_total_size - m_voice_param.voice_remain_size ) ) , m_i2s_buffer1 , m_voice_param.voice_remain_size );   // 再往mem1地址填充数据
            if( (-RT_ERROR) == err )                                              
            {
#if 0
                SEGGER_RTT_printf(0," w25q read voice fail  \r\n" );
#endif
            }
            dma_transmit_datalength( (m_voice_param.voice_remain_size/sizeof(uint16_t)) );              // 设置DMA传输的数据长度
            #ifdef CHECK_SEND_DUAL
                m_send_dual_byte += m_voice_param.voice_remain_size / 2 ;                                 // 使用了多少个双字节
            #endif
            m_voice_param.voice_remain_size = 0 ;                                                       // 剩余的size = 总剩余size - 这次使用的size
        }

        if( 0 == m_voice_param.voice_remain_size )                                                      // 无剩余了
        {
            m_using_mem_buff = 2 ;                                                                      // 下次即可停止 i2s_dma
        }
        else                                                                                            // 还有剩余的数据
        {
            m_using_mem_buff = 0 ;                                                                      // 下次切换到其他mem0
        }
    }
    else                                                                                                // 禁止DMA
    {
        #ifdef CHECK_SEND_DUAL
#if 0       
                SEGGER_RTT_printf(0," m_send_dual_byte = %d   \r\n" , m_send_dual_byte );
#endif
        #endif

        i2s_play_stop();
        tas2505_output_set(false);                                                                      // 关闭语音输出，否则有杂音
        memset( m_i2s_buffer0 , 0 , I2S_BUFF_SIZE );                                                    // 清0缓存区
        memset( m_i2s_buffer1 , 0 , I2S_BUFF_SIZE );
    }
}


/**
 * @brief 语音初始化
 */
void voice_handler_init(void)
{
                                                                        //w25q在从文件已自动导入初始化

    voice_header_get( &m_voice_head );                                  // 语音头获取

    tas2505_init();                                                     // tas2505初始化

    i2s2_init();                                                        // i2s初始化

    i2s2_samplerate_set( m_sample_rate );                               // 设置采样率 ，根据语音的设置
    dma_transmit_datalength( ( I2S_BUFF_SIZE/sizeof(uint16_t) ) );      // dma传输的数据量
    i2s2_tx_dma_init( m_i2s_buffer0 , m_i2s_buffer1 );                  // i2s dma传输

    i2s_dma_finish_irq_callback = i2s_dma_finish_handler ;              // 传递处理函数到i2s层

}


