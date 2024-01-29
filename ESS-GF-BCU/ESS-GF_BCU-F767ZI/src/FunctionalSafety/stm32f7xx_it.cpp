/**
  ******************************************************************************
  * @file    stm32f7xx_it.c
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    19-Jun-2017
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
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
#include <mbed.h>
#include "FunctionalSafety\stm32fxx_STLparam.h"
//#include "FunctionalSafety\stm32f7xx_it.h"
/* Includes ------------------------------------------------------------------*/
//#include "FunctionalSafety\main.h"

//#include "stm32fxx_STLlib.h"

namespace bmstech
{
	volatile static uint32_t tmpCC4_last; /* keep last TIM5/Chn4 captured value */

/******************************************************************************/
/**
  * @brief Configure TIM5 to measure LSI period
  * @param  : None
  * @retval : ErrorStatus = (ERROR, SUCCESS)
  */
	ErrorStatus STL_InitClock_Xcross_Measurement(void)
	{
		ErrorStatus result = SUCCESS;
		TIM_HandleTypeDef  tim_capture_handle;
		TIM_IC_InitTypeDef tim_input_config;
  
		/*## Enable peripherals and GPIO Clocks ####################################*/
		/* TIMx Peripheral clock enable */
		__TIM5_CLK_ENABLE();
  
		/*## Configure the NVIC for TIMx ###########################################*/
		HAL_NVIC_SetPriority(TIM5_IRQn, 4u, 0u);
  
		/* Enable the TIM5 global Interrupt */
		HAL_NVIC_EnableIRQ(TIM5_IRQn);
  
		/* TIM5 configuration: Input Capture mode ---------------------
		The LSI oscillator is connected to TIM5 CH4.
		The Rising edge is used as active edge, ICC input divided by 8
		The TIM5 CCR4 is used to compute the frequency value. 
		------------------------------------------------------------ */
		tim_capture_handle.Instance = TIM5;
		tim_capture_handle.Init.Prescaler         = 0u; 
		tim_capture_handle.Init.CounterMode       = TIM_COUNTERMODE_UP;  
		tim_capture_handle.Init.Period            = 0xFFFFFFFFul; 
		tim_capture_handle.Init.ClockDivision     = 0u;     
		tim_capture_handle.Init.RepetitionCounter = 0u;
		/* define internal HAL driver status here as handle structure is defined locally */
		__HAL_RESET_HANDLE_STATE(&tim_capture_handle);
		if (HAL_TIM_IC_Init(&tim_capture_handle) != HAL_OK)
		{
			/* Initialization Error */
			result = ERROR;
		}
		/* Connect internally the TIM5_CH4 Input Capture to the LSI clock output */
		HAL_TIMEx_RemapConfig(&tim_capture_handle, TIM_TIM5_LSI);
  
		/* Configure the TIM5 Input Capture of channel 4 */
		tim_input_config.ICPolarity  = TIM_ICPOLARITY_RISING;
		tim_input_config.ICSelection = TIM_ICSELECTION_DIRECTTI;
		tim_input_config.ICPrescaler = TIM_ICPSC_DIV8;
		tim_input_config.ICFilter    = 0u;
		if (HAL_TIM_IC_ConfigChannel(&tim_capture_handle, &tim_input_config, TIM_CHANNEL_4) != HAL_OK)
		{
			/* Initialization Error */
			result = ERROR;
		}
  
		/* Reset the flags */
		tim_capture_handle.Instance->SR = 0u;
		LSIPeriodFlag = 0u;
  
		/* Start the TIM Input Capture measurement in interrupt mode */
		if (HAL_TIM_IC_Start_IT(&tim_capture_handle, TIM_CHANNEL_4) != HAL_OK)
		{
			/* Initialization Error */
			result = ERROR;
		}
		return (result);
	}


} //namespace bmstech
