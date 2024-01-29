/**
  ******************************************************************************
  * @file    stm32756g_eval.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    30-December-2016
  * @brief   This file contains definitions for STM32756G_EVAL and STM32746G_EVAL LEDs,
  *          push-buttons and COM ports hardware resources.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32756G_EVAL_H
#define __STM32756G_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* USE_STM32756G_EVAL_REVA must USE USE_IOEXPANDER */
#if defined(USE_STM32756G_EVAL_REVA)
#ifndef USE_IOEXPANDER
#define USE_IOEXPANDER
#endif /* USE_IOEXPANDER */
#endif /* USE_STM32756G_EVAL_REVA */

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
   
/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32756G_EVAL
  * @{
  */
      
/** @addtogroup STM32756G_EVAL_LOW_LEVEL
  * @{
  */ 

/** @defgroup STM32756G_EVAL_LOW_LEVEL_Exported_Types LEVEL Exported Types
  * @{
  */
typedef enum 
{
#if defined(USE_IOEXPANDER)
LED1 = 0,
LED_GREEN = LED1,
LED2 = 1,
LED_ORANGE = LED2,
LED3 = 2,
LED_RED = LED3,
LED4 = 3,
LED_BLUE = LED4
#else
LED11 = 0,
LED_GREEN = LED11,
LED33 = 1,
LED_RED = LED33,
#endif /* USE_IOEXPANDER */
}Led_TypeDef;

typedef enum 
{  
  BUTTON_WAKEUP = 0,
  BUTTON_TAMPER = 1,
  BUTTON_KEY = 2
}Button_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
}ButtonMode_TypeDef;

#if defined(USE_IOEXPANDER)
typedef enum 
{  
  JOY_MODE_GPIO = 0,
  JOY_MODE_EXTI = 1
}JOYMode_TypeDef;

typedef enum 
{ 
  JOY_NONE  = 0,
  JOY_SEL   = 1,
  JOY_DOWN  = 2,
  JOY_LEFT  = 3,
  JOY_RIGHT = 4,
  JOY_UP    = 5
}JOYState_TypeDef;
#endif /* USE_IOEXPANDER */

typedef enum 
{
  COM1 = 0,
  COM2 = 1
}COM_TypeDef;
/**
  * @}
  */ 

/** @defgroup STM32756G_EVAL_LOW_LEVEL_Exported_Constants LOW LEVEL Exported Constants
  * @{
  */ 

/** 
  * @brief  Define for STM32756G_EVAL board
  */ 
#if !defined (USE_STM32756G_EVAL)
 #define USE_STM32756G_EVAL
#endif

/** @addtogroup STM32756G_EVAL_LOW_LEVEL_LED
  * @{
  */

#if !defined(USE_STM32756G_EVAL_REVA)

#if defined(USE_IOEXPANDER)
#define LEDN                             ((uint8_t)4)
#define LED2_PIN                         IO_PIN_17
#define LED4_PIN                         IO_PIN_19
#else
#define LEDN                             ((uint8_t)2)
#endif /* USE_IOEXPANDER */

#define LED1_GPIO_PORT                   GPIOB
#define LED1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

/* To use LED1, ensure that JP24 is in position 2-3, potentiometer is then no more usable */
#define LED1_PIN                         GPIO_PIN_0

#define LED3_GPIO_PORT                   GPIOB
#define LED3_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED3_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

/* To use LED3, ensure that JP23 is in position 2-3, camera is then no more usable */
#define LED3_PIN                         GPIO_PIN_14

#else

#define LEDN                             ((uint8_t)4)
#define LED1_PIN                         IO_PIN_16
#define LED2_PIN                         IO_PIN_17
#define LED3_PIN                         IO_PIN_18
#define LED4_PIN                         IO_PIN_19

#endif /* !USE_STM32756G_EVAL_REVA */


/**
  * @brief MFX_IRQOUt pin
  */
//#define MFX_IRQOUT_PIN                    GPIO_PIN_8
//#define MFX_IRQOUT_GPIO_PORT              GPIOI
//#define MFX_IRQOUT_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOI_CLK_ENABLE()
//#define MFX_IRQOUT_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOI_CLK_DISABLE()
//#define MFX_IRQOUT_EXTI_IRQn              EXTI9_5_IRQn

/** @addtogroup STM32756G_EVAL_LOW_LEVEL_BUTTON
  * @{
  */ 
/* Joystick pins are connected to IO Expander (accessible through I2C1 interface) */ 
//#define BUTTONn                             ((uint8_t)3) 

/**
  * @brief Wakeup push-button
  */
//#define WAKEUP_BUTTON_PIN                   GPIO_PIN_13
//#define WAKEUP_BUTTON_GPIO_PORT             GPIOC
//#define WAKEUP_BUTTON_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
//#define WAKEUP_BUTTON_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOC_CLK_DISABLE()
//#define WAKEUP_BUTTON_EXTI_IRQn             EXTI15_10_IRQn 

/**
  * @brief Tamper push-button
  */
//#define TAMPER_BUTTON_PIN                    GPIO_PIN_13
//#define TAMPER_BUTTON_GPIO_PORT              GPIOC
//#define TAMPER_BUTTON_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
//#define TAMPER_BUTTON_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOC_CLK_DISABLE()
//#define TAMPER_BUTTON_EXTI_IRQn              EXTI15_10_IRQn


//uint32_t         BSP_GetVersion(void);  
void             BSP_LED_Init(Led_TypeDef Led);
void             BSP_LED_DeInit(Led_TypeDef Led);
void             BSP_LED_On(Led_TypeDef Led);
void             BSP_LED_Off(Led_TypeDef Led);
//void             BSP_LED_Toggle(Led_TypeDef Led);
//void             BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
//void             BSP_PB_DeInit(Button_TypeDef Button);
//uint32_t         BSP_PB_GetState(Button_TypeDef Button);
//void             BSP_COM_Init(COM_TypeDef COM, UART_HandleTypeDef *husart);
//void             BSP_COM_DeInit(COM_TypeDef COM, UART_HandleTypeDef *huart);
//void             BSP_POTENTIOMETER_Init(void);
//uint32_t         BSP_POTENTIOMETER_GetLevel(void);
#if defined(USE_IOEXPANDER)
uint8_t          BSP_JOY_Init(JOYMode_TypeDef JoyMode);
void             BSP_JOY_DeInit(void);
JOYState_TypeDef BSP_JOY_GetState(void);
uint8_t          BSP_TS3510_IsDetected(void);
#endif /* USE_IOEXPANDER */


#ifdef __cplusplus
}
#endif

#endif /* __STM32756G_EVAL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
