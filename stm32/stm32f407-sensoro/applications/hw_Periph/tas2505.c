#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "type.h"
#include "tas2505.h"
#include <user_board.h>

#ifndef I2C_BUS_NAME
#define I2C_BUS_NAME    "i2c1"
#endif

static struct rt_i2c_bus_device *mp_i2c_bus = RT_NULL ;

/**
 * @brief 创建控制结构体
 */
static int i2c_auto_init(void)
{
    mp_i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(I2C_BUS_NAME);
    rt_kprintf("i2c set i2c bus to %s \n" , I2C_BUS_NAME );
}
INIT_COMPONENT_EXPORT(i2c_auto_init);            //导入到自动初始化


/**
 * @brief tas2505寄存器的写
 */
static int tas2505_reg_write( uint8_t reg, uint8_t value )
{
    rt_uint8_t buf[2];

    struct rt_i2c_msg msgs ;
    rt_size_t result ;

    buf[0] = reg ;
    buf[1] = value ;

    msgs.addr  = TAS2505_I2C_ADDRESS ;
    msgs.flags = RT_I2C_WR ;
    msgs.buf = buf;
    msgs.len = 2;

    result = rt_i2c_transfer(mp_i2c_bus, &msgs, 1);

    if (result == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}


/**
 * @brief tas2505寄存器的读
 */
static int tas2505_reg_read( uint8_t reg, uint8_t *p_value )
{
    rt_uint8_t buf[1];

    struct rt_i2c_msg msgs ;
    rt_size_t result ;

    buf[0] = reg ;

    msgs.addr  = TAS2505_I2C_ADDRESS ;
    msgs.flags = RT_I2C_WR ;
    msgs.buf = buf;
    msgs.len = 1;

    result = rt_i2c_transfer(mp_i2c_bus, &msgs, 1) ;

    if (result == 1)
    {

    }
    else
    {
        return -RT_ERROR;
    }

    msgs.addr  = TAS2505_I2C_ADDRESS ;
    msgs.flags = RT_I2C_RD ;
    msgs.buf = p_value;
    msgs.len = 1;

    result = rt_i2c_transfer(mp_i2c_bus, &msgs, 1);

    if (result == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

/**
 * @brief 硬件复位tas2505
 * 
 */
static void tas2505_hw_rst(void)
{
    rt_pin_mode( TAS25_RESET , PIN_MODE_OUTPUT );                             /* set TAS25_RST_PIN pin mode to output */

    rt_pin_write(TAS25_RESET , PIN_LOW);                                      // hw reset
    rt_thread_mdelay(50);
    rt_pin_write(TAS25_RESET, PIN_HIGH);    
    rt_thread_mdelay(100);
}

/**
 * @brief 软件复位
 * 
 */
static void tas2505_soft_rst(void)
{
    tas2505_reg_write( TAS2505_REG_PAGE_SELECT , TAS2505_PAGE_0 );
    tas2505_reg_write( TAS2505_REG_SOFTRESET , 1 );

    tas2505_reg_write( TAS2505_REG_PAGE_SELECT , TAS2505_PAGE_1 );
    tas2505_reg_write( TAS2505_REG_LDO_CTL , 0 );
}


/**
 * @brief 设置采样率
 * 
 */
int tas2505_samplerate_set( uint8_t ndac , uint8_t mdac , uint16_t dsor )
{
    uint8_t dsor_lsb ,dsor_msb ;
    dsor_lsb = dsor & 0xff;
    dsor_msb = (dsor >> 8) & 0x03;
 
    int err = 0 ;

    err = tas2505_reg_write( TAS2505_REG_PAGE_SELECT , TAS2505_PAGE_0);
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    err = tas2505_reg_write( TAS2505_REG_CLOCK_1 , 0 );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    err = tas2505_reg_write( TAS2505_REG_CLOCK_6 , ndac );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    err = tas2505_reg_write( TAS2505_REG_CLOCK_7 , mdac );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    err = tas2505_reg_write( TAS2505_REG_DAC_OSR_1 , dsor_msb );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    err = tas2505_reg_write( TAS2505_REG_DAC_OSR_2 , dsor_lsb );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    return err ;
}

/**
 * @brief 编码配置
 * 
 */
int tas2505_codec_config(void)
{
    int err = 0 ;

    err = tas2505_reg_write( TAS2505_REG_PAGE_SELECT , TAS2505_PAGE_0);
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    err = tas2505_reg_write( TAS2505_REG_CLOCK_1 , 0x03 );                      // select  PLL_clkin = MCLK, codec_clkin = PLL_CLK
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    err = tas2505_reg_write( TAS2505_REG_CLOCK_2 , 0x94 );                      // set p=1,r=4
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    err = tas2505_reg_write( TAS2505_REG_CLOCK_3 , 0x0e );                      // set j=E = 14
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    err = tas2505_reg_write( TAS2505_REG_CLOCK_4 , 0x0 );                       // set d=112 = 0X70 
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    err = tas2505_reg_write( TAS2505_REG_CLOCK_5 , 0x70 );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    rt_thread_mdelay(100);

    err = tas2505_reg_write( TAS2505_REG_CLOCK_6 , 0x82 );                      // set DAC NDAC power up and NDAC=2
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    
    err = tas2505_reg_write( TAS2505_REG_CLOCK_7 , 0x87 );                      // set DAC MDAC power up and MDAC=7
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    err = tas2505_reg_write( TAS2505_REG_DAC_OSR_1 , 0 );                       // set DAC OSR:128
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    err = tas2505_reg_write( TAS2505_REG_DAC_OSR_2 , 0x80 );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

   /*
    wordlength: 16bit
    BCLK & MCLK : input
    audio interface: i2s
   */
    err = tas2505_reg_write( TAS2505_REG_AUDIO_1 , 0x0 );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    err = tas2505_reg_write( TAS2505_REG_AUDIO_2 , 0x0 );                       // data offset = 0 BCLK's
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    err = tas2505_reg_write( TAS2505_REG_DAC_INSTRUCTION , 0x02 );              //Dac Instruction programming PRB #2 for Mono routing
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
   
    err = tas2505_reg_write( TAS2505_REG_PAGE_SELECT , TAS2505_PAGE_1 );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    err = tas2505_reg_write( TAS2505_REG_RFL_BGAP_CTL , 0x10 );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    err = tas2505_reg_write( TAS2505_REG_COMM_MODE_CTL , 0 );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    err = tas2505_reg_write( TAS2505_REG_SPEAKER_VOL_CTL_1 , 0 );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    //err = tas2505_reg_write( TAS2505_REG_SPEAKER_AMP_CTL_2 , 0x50 );              // 音量大得有点失真
    err = tas2505_reg_write( TAS2505_REG_SPEAKER_AMP_CTL_2 , 0x10 );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    err = tas2505_reg_write( TAS2505_REG_SPEAKER_AMP_CTL_1 , 0x02 );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    return err ;
}

/**
 * @brief 编码使能
 * 
 */
int tas2505_codec_enable(void)
{
    int err = 0 ;

    err = tas2505_reg_write( TAS2505_REG_PAGE_SELECT , TAS2505_PAGE_0);
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    /* select DAC data is picked from Left Channel Audio Interface Data
      select Soft step 1 per Fs
      DAC channel power up
    */
    err = tas2505_reg_write( TAS2505_REG_DAC_CHN_SET_1 , 0x90 );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    err = tas2505_reg_write( TAS2505_REG_DAC_CHN_VOL_CTL , 0x20 );          // set DAC channel gain 0dB
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    
    err = tas2505_reg_write( TAS2505_REG_DAC_CHN_SET_2 , 0x04 );            // 0: DAC Channel not muted
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    return err ;
}

/**@brief 设置音量
 * 
 */
int tas2505_volume_config(uint8_t volume)
{
    int err = 0 ;

    err = tas2505_reg_write( TAS2505_REG_PAGE_SELECT , TAS2505_PAGE_1);
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    err = tas2505_reg_write( TAS2505_REG_SPEAKER_VOL_CTL_1 , volume);
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
   
    return err ;
}

/**@brief 设置模拟量
 * 
 */
int tas2505_analog_set(uint8_t volume)
{
    int err = 0 ;

    err = tas2505_reg_write( TAS2505_REG_PAGE_SELECT , TAS2505_PAGE_1);
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    err = tas2505_reg_write( TAS2505_REG_RFL_BGAP_CTL , 0x10 );                     // Enable Master Analog Power Control
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    
    err = tas2505_reg_write( TAS2505_REG_COMM_MODE_CTL , 0x00 );                    // Program common-mode voltage for DAC to 0.9v
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    
    err = tas2505_reg_write( TAS2505_REG_SPEAKER_VOL_CTL_1 , 0x00 );                // set speaker volume =0 dB
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    //set speaker driver gain = 6 dB
    //err = tas2505_reg_write( TAS2505_REG_SPEAKER_AMP_CTL_2 , 0x50 );              // 音量大得有点失真
    err = tas2505_reg_write( TAS2505_REG_SPEAKER_AMP_CTL_2 , 0x10 );
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    err = tas2505_reg_write( TAS2505_REG_SPEAKER_AMP_CTL_1 , 0x02 );                // power up speaker
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    
    return err ;
}

/**
 * @brief 设置输出
 *  enable /disable speaker
 */
int tas2505_output_set(bool mode)
{
    int err = 0 ;
    uint8_t tem_mode  = 0x00 ;

    err = tas2505_reg_write( TAS2505_REG_PAGE_SELECT , TAS2505_PAGE_1);
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    if( true == mode )
    {
        tem_mode = 0x02 ;
    }

    
    err = tas2505_reg_write( TAS2505_REG_SPEAKER_AMP_CTL_1 , tem_mode);         // disable /enable tas2505 speaker output 
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    return err ;
}

/**
 * @brief tas2505芯片初始化
 * 
 */
int tas2505_init(void)
{
    int err = 0 ;

    tas2505_hw_rst()    ;    
    tas2505_soft_rst()  ;

    err = tas2505_codec_config() ;
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }

    err = tas2505_codec_enable() ;
    if ( (-RT_ERROR) == err )
    {
        return err ;
    }
    
    return err ;
}




/**
 * @brief tas2505 命令列表 -- 模拟i2c测试
 */
static void i2c_tas2505_sample(int argc, char *argv[])
{
    uint8_t send_data = 0x0A ;
    uint8_t rev_data ;

    mp_i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(I2C_BUS_NAME);
    rt_kprintf("i2c set i2c bus to %s \n" , I2C_BUS_NAME );

    tas2505_hw_rst();                                                           // 硬件复位
    tas2505_soft_rst();                                                         // 软件复位
#if 1                                                                           // 测试模拟I2C和TAS2505之间的通信
    tas2505_reg_write( TAS2505_REG_PAGE_SELECT , TAS2505_PAGE_0 );              // test
    tas2505_reg_write( TAS2505_REG_DAC_OSR_2 , send_data );

    tas2505_reg_write( TAS2505_REG_PAGE_SELECT , TAS2505_PAGE_0 );
    tas2505_reg_read( TAS2505_REG_DAC_OSR_2, &rev_data);

    rt_kprintf(" rev_data = 0x%02x \n" , rev_data );
#endif
}
MSH_CMD_EXPORT(i2c_tas2505_sample, i2c tas2505 sample);                         // 导出到 msh 命令列表中








