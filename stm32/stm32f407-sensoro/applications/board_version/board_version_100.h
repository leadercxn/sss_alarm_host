
#ifndef __BOARD_VERSION110_H
#define __BOARD_VERSION110_H

#include "stm32f4xx_hal.h"

/* 外设IO、时钟配置，该部分应用在 stm32fxxx_hal_msp.c 文件的底层配置 */
#define UART1_TX_PIN                    GPIO_PIN_6
#define UART1_RX_PIN                    GPIO_PIN_7
#define GPIO_UART1                      GPIOB
#define GPIO_UART1_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE();

#define UART2_TX_PIN                    GPIO_PIN_5
#define UART2_RX_PIN                    GPIO_PIN_6
#define GPIO_UART2                      GPIOD
#define GPIO_UART2_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE();

#define UART3_TX_PIN                    GPIO_PIN_8
#define UART3_RX_PIN                    GPIO_PIN_9
#define GPIO_UART3                      GPIOD
#define GPIO_UART3_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE();

#define ADC1_BAT_DECT_PIN               GPIO_PIN_0
#define GPIO_ADC1_BAT                   GPIOB
#define GPIO_ADC1_BAT_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE();

#define ADC1_NTC_DECT_PIN               GPIO_PIN_0
#define GPIO_ADC1_NTC                   GPIOC
#define GPIO_ADC1_NTC_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE();

#define PWM1_BEEP_PIN                   GPIO_PIN_9
#define GPIO_PWM1_BEEP                  GPIOE
#define GPIO_PWM1_BEEP_CLK_ENABLE()     __HAL_RCC_GPIOE_CLK_ENABLE();

#define SPI1_W25QXX_SCLK_PIN            GPIO_PIN_5
#define SPI1_W25QXX_MISO_PIN            GPIO_PIN_6
#define SPI1_W25QXX_MOSI_PIN            GPIO_PIN_5
#define SPI1_W25QXX_CS_PIN              GPIO_PIN_4
#define GPIO_SPI1_W25QXX_SCLK           GPIOA
#define GPIO_SPI1_W25QXX_MISO           GPIOA
#define GPIO_SPI1_W25QXX_MOSI           GPIOB
#define GPIO_SPI1_W25QXX_CS             GPIOA
#define GPIO_SPI1_CLK_ENABLE()          { __HAL_RCC_GPIOA_CLK_ENABLE();__HAL_RCC_GPIOB_CLK_ENABLE(); }

#define I2S_WS_PIN                      GPIO_PIN_9
#define I2S_CK_PIN                      GPIO_PIN_10
#define I2S_DOUT_PIN                    GPIO_PIN_14
#define I2S_SD_PIN                      GPIO_PIN_15
#define I2S_MCLK_PIN                    GPIO_PIN_6
#define GPIO_I2S_WS                     GPIOB
#define GPIO_I2S_CK                     GPIOB
#define GPIO_I2S_DOUT                   GPIOB
#define GPIO_I2S_SD                     GPIOB
#define GPIO_I2S_MCLK                   GPIOC
#define GPIO_I2S_CLK_ENABLE()          { __HAL_RCC_GPIOB_CLK_ENABLE();__HAL_RCC_GPIOC_CLK_ENABLE(); }

#define ETH_MDC_PIN                     GPIO_PIN_1
#define ETH_REF_CLK_PIN                 GPIO_PIN_1
#define ETH_MDIO_PIN                    GPIO_PIN_2
#define ETH_CRS_DV_PIN                  GPIO_PIN_7
#define ETH_RXD0_PIN                    GPIO_PIN_4
#define ETH_RXD1_PIN                    GPIO_PIN_5
#define ETH_TX_EN_PIN                   GPIO_PIN_11
#define ETH_TXD0_PIN                    GPIO_PIN_12
#define ETH_TXD1_PIN                    GPIO_PIN_13
#define GPIO_ETH_MDC                    GPIOC
#define GPIO_ETH_RXD0                   GPIOC
#define GPIO_ETH_RXD1                   GPIOC
#define GPIO_ETH_REF_CLK                GPIOA
#define GPIO_ETH_MDIO                   GPIOA
#define GPIO_ETH_CRS_DV                 GPIOA
#define GPIO_ETH_TX_EN                  GPIOB
#define GPIO_ETH_TXD0                   GPIOB
#define GPIO_ETH_TXD1                   GPIOB
#define GPIO_ETH_CLK_ENABLE()           { __HAL_RCC_GPIOC_CLK_ENABLE(); __HAL_RCC_GPIOA_CLK_ENABLE(); __HAL_RCC_GPIOB_CLK_ENABLE(); }

/* 应用层IO的配置，应用在应用层IO配置的 */
#define PCIE_PWR_EN                     GET_PIN(D, 15)
#define PCIE_RESET                      GET_PIN(D, 1)

#define TAS25_RESET                     GET_PIN(C, 7)
#define PAIR_BUTTON                     GET_PIN(E, 14)
#define INSTALL_BUTTON                  GET_PIN(E, 13)     
#define VCC_3V3_2_SUPPLY                GET_PIN(E, 10)      //打开3V3_2供电    
#define LAN8720_RESET                   GET_PIN(E, 15)      //LAN8720复位
#define ALPHA_RESET                     GET_PIN(D, 10)      //alpha芯片的复位
#define ALPHA_RESET_PIN                 GPIO_PIN_10
#define GPIO_ALPHA_RESET                GPIOD
/* 应用设备名称 */
#define W25Q_DEVICE_NAME        "W25Q128"
#define SPI_BUS_NAME            "spi1"
#define SPI_DEVICE_NAME         "spi10"
#define I2C_BUS_NAME            "i2c1"
#define ADC_DEV_NAME            "adc1" 
#define PWM_DEV_NAME            "pwm1"      /* PWM设备名称 */
#define HWTIMER_DEV_NAME        "timer3"     /* 定时器名称 */
#define UART2_NAME              "uart2"
#define BC26_DEV_NAME           "uart1"
#define ALPHA_DEV_NAME          "uart3"

#endif



















