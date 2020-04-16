#ifndef __TAS2505_H
#define __TAS2505_H


#define TAS2505_I2C_ADDRESS                         (0x30>>1)   //RTT的I2C地址需要右移1bit

#define TAS2505_PAGE_0   0
#define TAS2505_PAGE_1   1

#define TAS2505_REG_PAGE_SELECT                     0
#define TAS2505_REG_SOFTRESET                       0x01
#define TAS2505_REG_LDO_CTL                         0x02
#define TAS2505_REG_CLOCK_1                         0x04
#define TAS2505_REG_CLOCK_2                         0x05
#define TAS2505_REG_CLOCK_3                         0x06
#define TAS2505_REG_CLOCK_4                         0x07
#define TAS2505_REG_CLOCK_5                         0x08
#define TAS2505_REG_CLOCK_6                         0x0B
#define TAS2505_REG_CLOCK_7                         0x0C
#define TAS2505_REG_CLOCK_10                        0x19
#define TAS2505_REG_CLOCK_11                        0x1A
#define TAS2505_REG_CLOCK_12                        0x1E
#define TAS2505_REG_DAC_OSR_1                       0x0D
#define TAS2505_REG_DAC_OSR_2                       0x0E
#define TAS2505_REG_AUDIO_1                         0x1B
#define TAS2505_REG_AUDIO_2                         0x1C
#define TAS2505_REG_AUDIO_3                         0x1D
#define TAS2505_REG_AUDIO_4                         0x1F
#define TAS2505_REG_AUDIO_5                         0x20
#define TAS2505_REG_AUDIO_6                         0x21
#define TAS2505_REG_DAC_FLAG_1                      0x25
#define TAS2505_REG_DAC_FLAG_2                      0x26
#define TAS2505_REG_DAC_INSTRUCTION                 0x3C
#define TAS2505_REG_DAC_CHN_SET_1                   0x3F
#define TAS2505_REG_DAC_CHN_SET_2                   0x40
#define TAS2505_REG_DAC_CHN_VOL_CTL                 0x41
#define TAS2505_REG_SPEAKER_AMP_CTL_1               0x2D
#define TAS2505_REG_SPEAKER_AMP_CTL_2               0x30
#define TAS2505_REG_SPEAKER_VOL_CTL_1               0x2E
#define TAS2505_REG_OUTPUT_CTL                      0x09
#define TAS2505_REG_HP_DR_GAIN                      0x10
#define TAS2505_REG_HP_VOL_CTL                      0x16
#define TAS2505_REG_RFL_BGAP_CTL                    0x01
#define TAS2505_REG_COMM_MODE_CTL                   0x0A
#define TAS2505_REG_DIN_CTL                         0x36




/**
 * @brief tas2505芯片初始化
 * 
 */
int tas2505_init(void);

/**
 * @brief 设置输出
 *  enable /disable speaker
 */
int tas2505_output_set(bool mode);

#endif
















