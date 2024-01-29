/**
  ******************************************************************************
  * @file    stm32756g_eval.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    30-December-2016
  * @brief   This file provides a set of firmware functions to manage LEDs, 
  *          push-buttons and COM ports available on STM32756G-EVAL and STM32746G-EVAL
  *          evaluation board(MB1167) from STMicroelectronics.
  *
  @verbatim
            This driver requires the stm32756g_eval_io.c/.h files to manage the 
            IO module resources mapped on the MFX IO expander.
            These resources are mainly LEDs, Joystick push buttons, SD detect pin, 
            USB OTG power switch/over current drive pins, Camera plug pin, Audio
            INT pin
            The use of the above eval resources is conditioned by the "USE_IOEXPANDER"
            preprocessor define which is enabled by default for the STM327x6G-EVAL
            boards Rev A. However for Rev B boards these resources are disabled by default
            (except LED1 and LED2) and to be able to use them, user must add "USE_IOEXPANDER"
            define in the compiler preprocessor configuration (or any header file that
            is processed before stm32756g_eval.h).
            On the STM327x6G-EVAL RevB LED1 and LED2 are directly mapped on GPIO pins,
            to avoid the unnecessary overhead of code brought by the use of MFX IO 
            expander when no further evaluation board resources are needed by the 
            application/example.
            For precise details on the use of the MFX IO expander, you can refer to
            the description provided within the stm32756g_eval_io.c file header
  @endverbatim
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
  
/* Includes ------------------------------------------------------------------*/
#include "stm32756g_eval.h"
#if defined(USE_IOEXPANDER)
#include "stm32756g_eval_io.h"
#endif /* USE_IOEXPANDER */

/** @addtogroup BSP
  * @{
  */ 

/**
 * @brief STM32756G EVAL BSP Driver version number V2.0.0
   */
#define __STM32756G_EVAL_BSP_VERSION_MAIN   (0x02) /*!< [31:24] main version */
#define __STM32756G_EVAL_BSP_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __STM32756G_EVAL_BSP_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __STM32756G_EVAL_BSP_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32756G_EVAL_BSP_VERSION         ((__STM32756G_EVAL_BSP_VERSION_MAIN << 24)\
                                             |(__STM32756G_EVAL_BSP_VERSION_SUB1 << 16)\
                                             |(__STM32756G_EVAL_BSP_VERSION_SUB2 << 8 )\
                                             |(__STM32756G_EVAL_BSP_VERSION_RC))

/** @defgroup STM32756G_EVAL_LOW_LEVEL_Private_Variables STM32756G-EVAL LOW LEVEL Private Variables
  * @{
  */

#if defined(USE_IOEXPANDER)
const uint32_t GPIO_PIN[LEDn] = {LED1_PIN,
                                 LED2_PIN,
                                 LED3_PIN,
                                 LED4_PIN};
#else
//
const uint32_t GPIO_PIN[LEDN] = {LED1_PIN,
                                 LED3_PIN};
#endif /* USE_IOEXPANDER */


/**
  * @brief  Configures LED on GPIO and/or on MFX.
  * @param  Led: LED to be configured. 
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval None
  */
void BSP_LED_Init(Led_TypeDef Led)
{
#if !defined(USE_STM32756G_EVAL_REVA)
  /* On RevB and above evaluation boards, LED1 and LED3 are connected to GPIOs */
  /* To use LED1 on RevB board, ensure that JP24 is in position 2-3, potentiometer is then no more usable */
  /* To use LED3 on RevB board, ensure that JP23 is in position 2-3, camera is then no more usable */
  GPIO_InitTypeDef  gpio_init_structure;
  GPIO_TypeDef*     gpio_led;//gpio_led

  if ((Led == LED11) || (Led == LED33))
  {
    if (Led == LED11)
    {
      gpio_led = LED1_GPIO_PORT;
      /* Enable the GPIO_LED clock */
      LED1_GPIO_CLK_ENABLE();
    }
    else
    {
      gpio_led = LED3_GPIO_PORT;
      /* Enable the GPIO_LED clock */
      LED3_GPIO_CLK_ENABLE();
    }

    /* Configure the GPIO_LED pin */
    gpio_init_structure.Pin = GPIO_PIN[Led];
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  
    HAL_GPIO_Init(gpio_led, &gpio_init_structure);
    
    /* By default, turn off LED */
    HAL_GPIO_WritePin(gpio_led, GPIO_PIN[Led], GPIO_PIN_SET);
  }
  else
  {
#endif /* !USE_STM32756G_EVAL_REVA */



#if !defined(USE_STM32756G_EVAL_REVA)
  }
#endif /* !USE_STM32756G_EVAL_REVA */
}


/**
  * @brief  DeInit LEDs.
  * @param  Led: LED to be configured. 
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @note Led DeInit does not disable the GPIO clock nor disable the Mfx 
  * @retval None
  */
void BSP_LED_DeInit(Led_TypeDef Led)
{
#if !defined(USE_STM32756G_EVAL_REVA)
  GPIO_InitTypeDef  gpio_init_structure;//
  GPIO_TypeDef*     gpio_led;//

  /* On RevB led1 and Led3 are on GPIO while Led2 and Led4 on Mfx*/
  if ((Led == LED11) || (Led == LED33))
  {
    if (Led == LED11)
    {
      gpio_led = LED1_GPIO_PORT;
    }
    else
    {
      gpio_led = LED3_GPIO_PORT;
    }
    /* Turn off LED */
    HAL_GPIO_WritePin(gpio_led, GPIO_PIN[Led], GPIO_PIN_RESET);
    /* Configure the GPIO_LED pin */
    gpio_init_structure.Pin = GPIO_PIN[Led];
    HAL_GPIO_DeInit(gpio_led, gpio_init_structure.Pin);
  }
  else
  {
#endif /* !USE_STM32756G_EVAL_REVA */

#if defined(USE_IOEXPANDER)   /* (USE_IOEXPANDER always defined for RevA) */
    /* GPIO_PIN[Led]  depends on the board revision: */
    /*  - in case of RevA all leds are deinit  */
    /*  - in case of RevB just led 2 and led4 are deinit */
    BSP_IO_ConfigPin(GPIO_PIN[Led], IO_MODE_OFF);
#endif /* USE_IOEXPANDER */     

#if !defined(USE_STM32756G_EVAL_REVA)
  }
#endif /* !USE_STM32756G_EVAL_REVA */
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: LED to be set on 
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval None
  */
void BSP_LED_On(Led_TypeDef Led)
{
#if !defined(USE_STM32756G_EVAL_REVA)
  GPIO_TypeDef*     gpio_led;//

  if ((Led == LED11) || (Led == LED33))	/* Switch On LED connected to GPIO */
  {
    if (Led == LED11)
    {
      gpio_led = LED1_GPIO_PORT;
    }
    else
    {
      gpio_led = LED3_GPIO_PORT;
    }
    HAL_GPIO_WritePin(gpio_led, GPIO_PIN[Led], GPIO_PIN_RESET);
  }
  else
  {
#endif /* !USE_STM32756G_EVAL_REVA */

#if defined(USE_IOEXPANDER)            /* Switch On LED connected to MFX */
      BSP_IO_WritePin(GPIO_PIN[Led], BSP_IO_PIN_RESET);
#endif /* USE_IOEXPANDER */     

#if !defined(USE_STM32756G_EVAL_REVA)
  }
#endif /* !USE_STM32756G_EVAL_REVA */
}

/**
  * @brief  Turns selected LED Off. 
  * @param  Led: LED to be set off
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval None
  */
void BSP_LED_Off(Led_TypeDef Led)
{
#if !defined(USE_STM32756G_EVAL_REVA)
  GPIO_TypeDef*     gpio_led;//

  if ((Led == LED11) || (Led == LED33)) /* Switch Off LED connected to GPIO */
  {
    if (Led == LED11)
    {
      gpio_led = LED1_GPIO_PORT;
    }
    else
    {
      gpio_led = LED3_GPIO_PORT;
    }
    HAL_GPIO_WritePin(gpio_led, GPIO_PIN[Led], GPIO_PIN_SET);
  }
  else
  {
#endif /* !USE_STM32756G_EVAL_REVA */

#if defined(USE_IOEXPANDER)		/* Switch Off LED connected to MFX */
     BSP_IO_WritePin(GPIO_PIN[Led], BSP_IO_PIN_SET);
#endif /* USE_IOEXPANDER */     

#if !defined(USE_STM32756G_EVAL_REVA)
  }
#endif /* !USE_STM32756G_EVAL_REVA */

}

    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
