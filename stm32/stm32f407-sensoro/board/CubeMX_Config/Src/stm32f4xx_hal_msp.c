/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : stm32f4xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "user_board.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */
 
/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART1)
  {
    /* USER CODE BEGIN USART1_MspInit 0 */

    /* USER CODE END USART1_MspInit 0 */
      /* Peripheral clock enable */
      __HAL_RCC_USART1_CLK_ENABLE();
    
      GPIO_UART1_CLK_ENABLE();
      /**USART1 GPIO Configuration    
      PA9     ------> USART1_TX
      PA10     ------> USART1_RX 
      */
      GPIO_InitStruct.Pin = UART1_TX_PIN|UART1_RX_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
      HAL_GPIO_Init(GPIO_UART1, &GPIO_InitStruct);

    /* USER CODE BEGIN USART1_MspInit 1 */

    /* USER CODE END USART1_MspInit 1 */
  }
  else if( huart->Instance == USART2 )
  {
      __HAL_RCC_USART2_CLK_ENABLE();

      GPIO_UART2_CLK_ENABLE();
      GPIO_InitStruct.Pin        = UART2_TX_PIN |UART2_RX_PIN ;		// PD5
      GPIO_InitStruct.Mode       = GPIO_MODE_AF_PP;					  // 复用推挽输出
      GPIO_InitStruct.Pull       = GPIO_PULLUP;						    // 上拉
      GPIO_InitStruct.Speed      = GPIO_SPEED_FREQ_VERY_HIGH;	// 高速模式
      GPIO_InitStruct.Alternate  = GPIO_AF7_USART2;	    			// 复用UART2
      HAL_GPIO_Init(GPIO_UART2,&GPIO_InitStruct );	   	    			// 

  }
  else if( huart->Instance == USART3 )
  {
      __HAL_RCC_USART3_CLK_ENABLE();

      GPIO_UART3_CLK_ENABLE();
      GPIO_InitStruct.Pin        = UART3_TX_PIN |UART3_RX_PIN ;		// 
      GPIO_InitStruct.Mode       = GPIO_MODE_AF_PP;					  // 复用推挽输出
      GPIO_InitStruct.Pull       = GPIO_PULLUP;						    // 上拉
      GPIO_InitStruct.Speed      = GPIO_SPEED_FREQ_VERY_HIGH;	// 高速模式
      GPIO_InitStruct.Alternate  = GPIO_AF7_USART3;	    			// 复用UART3
      HAL_GPIO_Init(GPIO_UART3,&GPIO_InitStruct );	   	    			// 
  }

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIO_UART1, UART1_TX_PIN|UART1_RX_PIN);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if( huart->Instance == USART2 )
  {
      __HAL_RCC_USART2_CLK_DISABLE();

      HAL_GPIO_DeInit(GPIO_UART2, UART2_TX_PIN|UART2_RX_PIN);
  }
  else if( huart->Instance == USART3 )
  {
      __HAL_RCC_USART3_CLK_DISABLE();

      HAL_GPIO_DeInit(GPIO_UART3, UART3_TX_PIN|UART3_RX_PIN);
  }


}

/* USER CODE BEGIN 1 */


/**
 * @brief adc底层驱动函数，此函数挥会被 HAL_ADC_Init()调用
 *        初始化时钟、IO
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    if(hadc->Instance==ADC1)
    {
      __HAL_RCC_ADC1_CLK_ENABLE();                 // 使能ADC1时钟

      GPIO_ADC1_BAT_CLK_ENABLE();
      GPIO_ADC1_NTC_CLK_ENABLE();
    
      GPIO_Initure.Pin    = ADC1_BAT_DECT_PIN;            // PB0
      GPIO_Initure.Mode   = GPIO_MODE_ANALOG;      // 模拟
      GPIO_Initure.Pull   = GPIO_NOPULL;           // 不上下拉
      HAL_GPIO_Init(GPIO_ADC1_BAT, &GPIO_Initure);

      GPIO_Initure.Pin    = ADC1_NTC_DECT_PIN;            // PC0
      GPIO_Initure.Mode   = GPIO_MODE_ANALOG;      // 模拟
      GPIO_Initure.Pull   = GPIO_NOPULL;           // 不上下拉
      HAL_GPIO_Init(GPIO_ADC1_NTC, &GPIO_Initure);
    }
    

}

/**
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();
  
    HAL_GPIO_DeInit(GPIO_ADC1_BAT, ADC1_BAT_DECT_PIN);
    HAL_GPIO_DeInit(GPIO_ADC1_NTC, ADC1_NTC_DECT_PIN);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }

}



/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
  else if(htim_base->Instance==TIM11)
  {
  /* USER CODE BEGIN TIM11_MspInit 0 */

  /* USER CODE END TIM11_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM11_CLK_ENABLE();
  /* USER CODE BEGIN TIM11_MspInit 1 */

  /* USER CODE END TIM11_MspInit 1 */
  }
  else if(htim_base->Instance==TIM13)
  {
  /* USER CODE BEGIN TIM13_MspInit 0 */

  /* USER CODE END TIM13_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM13_CLK_ENABLE();
  /* USER CODE BEGIN TIM13_MspInit 1 */

  /* USER CODE END TIM13_MspInit 1 */
  }
  else if(htim_base->Instance==TIM14)
  {
  /* USER CODE BEGIN TIM14_MspInit 0 */

  /* USER CODE END TIM14_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM14_CLK_ENABLE();
  /* USER CODE BEGIN TIM14_MspInit 1 */

  /* USER CODE END TIM14_MspInit 1 */
  }
  else if(htim_base->Instance==TIM4)
  {
    /* Peripheral clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();

  }
  else if(htim_base->Instance==TIM3)
  {
    /* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

  }
}

/**
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
  else if(htim_base->Instance==TIM11)
  {
  /* USER CODE BEGIN TIM11_MspDeInit 0 */

  /* USER CODE END TIM11_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM11_CLK_DISABLE();
  /* USER CODE BEGIN TIM11_MspDeInit 1 */

  /* USER CODE END TIM11_MspDeInit 1 */
  }
  else if(htim_base->Instance==TIM13)
  {
  /* USER CODE BEGIN TIM13_MspDeInit 0 */

  /* USER CODE END TIM13_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM13_CLK_DISABLE();
  /* USER CODE BEGIN TIM13_MspDeInit 1 */

  /* USER CODE END TIM13_MspDeInit 1 */
  }
  else if(htim_base->Instance==TIM14)
  {
  /* USER CODE BEGIN TIM14_MspDeInit 0 */

  /* USER CODE END TIM14_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM14_CLK_DISABLE();
  /* USER CODE BEGIN TIM14_MspDeInit 1 */

  /* USER CODE END TIM14_MspDeInit 1 */
  }
  else if(htim_base->Instance==TIM4)
  {
    /* Peripheral clock enable */
    __HAL_RCC_TIM4_CLK_DISABLE();

  }
  else if(htim_base->Instance==TIM3)
  {
    /* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_DISABLE();

  }

}


/**
 * pwm的回调函数
 */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim->Instance==TIM1)
  {
      GPIO_InitTypeDef GPIO_Initure;
      GPIO_PWM1_BEEP_CLK_ENABLE();			              //开启GPIOD时钟，具体视硬件而定
        
      GPIO_Initure.Pin        = PWM1_BEEP_PIN;        // 映射的引脚
      GPIO_Initure.Mode       = GPIO_MODE_AF_PP;      //复用推挽输出
      GPIO_Initure.Pull       = GPIO_PULLUP;          //上拉
      GPIO_Initure.Speed      = GPIO_SPEED_HIGH;      //高速
      GPIO_Initure.Alternate  = GPIO_AF1_TIM1;	      //映射TIM1_CH1
      HAL_GPIO_Init( GPIO_PWM1_BEEP ,&GPIO_Initure );
  }

}



/**@brief SPI底层驱动函数，此函数挥会被 HAL_SPI_Init()调用
 * 初始化SPI1时钟和初始化IO
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    if(hspi->Instance==SPI1)
    {
      __HAL_RCC_SPI1_CLK_ENABLE();                            //使能SPI1时钟
      GPIO_SPI1_CLK_ENABLE();  

      GPIO_Initure.Pin        = SPI1_W25QXX_SCLK_PIN|SPI1_W25QXX_MISO_PIN;        // SCLK | MISO
      GPIO_Initure.Mode       = GPIO_MODE_AF_PP;              //复用
      GPIO_Initure.Pull       = GPIO_PULLUP;                  //上啦
      GPIO_Initure.Speed      = GPIO_SPEED_FAST;              //快速模式        
      GPIO_Initure.Alternate  = GPIO_AF5_SPI1;                //复用为SPI1
      HAL_GPIO_Init( GPIO_SPI1_W25QXX_SCLK , &GPIO_Initure );
  
      GPIO_Initure.Pin        = SPI1_W25QXX_MOSI_PIN;                   // MOSI
      GPIO_Initure.Mode       = GPIO_MODE_AF_PP;              //复用
      GPIO_Initure.Pull       = GPIO_PULLUP;                  //上啦
      GPIO_Initure.Speed      = GPIO_SPEED_FAST;              //快速模式        
      GPIO_Initure.Alternate  = GPIO_AF5_SPI1;                //复用为SPI1
      HAL_GPIO_Init( GPIO_SPI1_W25QXX_MOSI , &GPIO_Initure );

    }
    
}

/**
* @brief SPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance==SPI1)
  {

    __HAL_RCC_SPI1_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIO_SPI1_W25QXX_SCLK, SPI1_W25QXX_SCLK_PIN|SPI1_W25QXX_MISO_PIN  );
    HAL_GPIO_DeInit(GPIO_SPI1_W25QXX_MOSI, SPI1_W25QXX_MOSI_PIN );
  }


}

/**
* @brief I2S MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: I2S handle pointer
* @retval None
*/
void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s)
{
	GPIO_InitTypeDef GPIO_Initure;
    
  if( hi2s->Instance == SPI2 )
  {
      __HAL_RCC_SPI2_CLK_ENABLE();        		            // SPI2 CLK时钟使能
      GPIO_I2S_CLK_ENABLE(); 

      GPIO_Initure.Pin        = I2S_WS_PIN | I2S_CK_PIN | I2S_DOUT_PIN |I2S_SD_PIN ;  
      GPIO_Initure.Mode       = GPIO_MODE_AF_PP;          //  复用推挽
      GPIO_Initure.Pull       = GPIO_PULLUP;              //
      GPIO_Initure.Speed      = GPIO_SPEED_HIGH;          //
      GPIO_Initure.Alternate  = GPIO_AF5_SPI2;            //  复用SPI/I2S
      HAL_GPIO_Init( GPIO_I2S_WS , &GPIO_Initure );             //  初始化
    
      GPIO_Initure.Pin        = I2S_MCLK_PIN ; 
      HAL_GPIO_Init( GPIO_I2S_MCLK , &GPIO_Initure);                 //  初始化
  }
	
}


/**
* @brief I2S MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hspi: I2S handle pointer
* @retval None
*/
void HAL_I2S_MspDeInit(I2S_HandleTypeDef *hi2s)
{
  if( hi2s->Instance == SPI2 )
  {
      __HAL_RCC_SPI2_CLK_DISABLE();        		            // 禁止SPI2 CLK时钟

      HAL_GPIO_DeInit(GPIO_I2S_WS, I2S_WS_PIN | I2S_CK_PIN | I2S_DOUT_PIN |I2S_SD_PIN );
      HAL_GPIO_DeInit(GPIO_I2S_MCLK, I2S_MCLK_PIN  );
  }
}

void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(ethHandle->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspInit 0 */

  /* USER CODE END ETH_MspInit 0 */
    /* Enable Peripheral clock */
    __HAL_RCC_ETH_CLK_ENABLE();
  
    GPIO_ETH_CLK_ENABLE();
    /**ETH GPIO Configuration    
    PC1     ------> ETH_MDC
    PA1     ------> ETH_REF_CLK
    PA2     ------> ETH_MDIO
    PA7     ------> ETH_CRS_DV
    PC4     ------> ETH_RXD0
    PC5     ------> ETH_RXD1
    PB11     ------> ETH_TX_EN
    PB12     ------> ETH_TXD0
    PB13     ------> ETH_TXD1 
    */
    GPIO_InitStruct.Pin = ETH_MDC_PIN|ETH_RXD0_PIN|ETH_RXD1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIO_ETH_MDC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ETH_REF_CLK_PIN|ETH_MDIO_PIN|ETH_CRS_DV_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIO_ETH_REF_CLK, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ETH_TX_EN_PIN|ETH_TXD0_PIN|ETH_TXD1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIO_ETH_TX_EN, &GPIO_InitStruct);

    /* Peripheral interrupt init */
//    HAL_NVIC_SetPriority(ETH_IRQn, 5, 0);
//    HAL_NVIC_EnableIRQ(ETH_IRQn);
  /* USER CODE BEGIN ETH_MspInit 1 */
    rt_kprintf(" HAL_ETH_MspInit finish \r\n");
  /* USER CODE END ETH_MspInit 1 */
  }
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
{
  if(ethHandle->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspDeInit 0 */

  /* USER CODE END ETH_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ETH_CLK_DISABLE();
  
    /**ETH GPIO Configuration    
    PC1     ------> ETH_MDC
    PA1     ------> ETH_REF_CLK
    PA2     ------> ETH_MDIO
    PA7     ------> ETH_CRS_DV
    PC4     ------> ETH_RXD0
    PC5     ------> ETH_RXD1
    PB11     ------> ETH_TX_EN
    PB12     ------> ETH_TXD0
    PB13     ------> ETH_TXD1 
    */
    HAL_GPIO_DeInit(GPIO_ETH_MDC, ETH_MDC_PIN|ETH_RXD0_PIN|ETH_RXD1_PIN );

    HAL_GPIO_DeInit(GPIO_ETH_REF_CLK, ETH_REF_CLK_PIN|ETH_MDIO_PIN|ETH_CRS_DV_PIN );

    HAL_GPIO_DeInit(GPIO_ETH_TX_EN, ETH_TX_EN_PIN|ETH_TXD0_PIN|ETH_TXD1_PIN);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(ETH_IRQn);

  /* USER CODE BEGIN ETH_MspDeInit 1 */

  /* USER CODE END ETH_MspDeInit 1 */
  }
}





/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
