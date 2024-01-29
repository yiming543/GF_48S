/* mbed Microcontroller Library
 * Copyright (c) 2006-2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "hal_tick.h"

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32fxx_STLparam.h"
#include "stm32fxx_STLlib.h"
#include "stm32f7xx_it.h"

// A 32-bit timer is used
#if !TIM_MST_16BIT

#define timer_irq_handler TIM2_IRQHandler       //JY

extern TIM_HandleTypeDef TimMasterHandle;

volatile uint32_t PreviousVal = 0;

void us_ticker_irq_handler(void);

void timer_irq_handler(void)
{
    // Channel 1 for mbed timeout
    if (__HAL_TIM_GET_FLAG(&TimMasterHandle, TIM_FLAG_CC1) == SET) 
    {
        if (__HAL_TIM_GET_IT_SOURCE(&TimMasterHandle, TIM_IT_CC1) == SET) 
        {
            __HAL_TIM_CLEAR_IT(&TimMasterHandle, TIM_IT_CC1);
            us_ticker_irq_handler();	        
        }
//	    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
//	    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
//	    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	 
    }
//	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
//	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
//	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	/* Verify TickCounter integrity */
	    
//		    if ((TickCounter ^ TickCounterInv) == 0xFFFFFFFFuL)
//		    {
//			    TickCounter++;
//			    TickCounterInv = ~TickCounter;
//	
//			    if (TickCounter >= SYSTICK_10ms_TB)
//			    {
//				    uint32_t RamTestResult;
//	
//	#if defined STL_EVAL_MODE
//				    /* Toggle LED1 for debug purposes */
//				    BSP_LED_Toggle(LED11);
//	#endif  /* STL_EVAL_MODE */
//	        
//				    /* Reset timebase counter */
//				    TickCounter = 0u;
//				    TickCounterInv = 0xFFFFFFFFuL;
//	
//				    /* Set Flag read in main loop */
//				    TimeBaseFlag = 0xAAAAAAAAuL;
//				    TimeBaseFlagInv = 0x55555555uL;
//	
//				    ISRCtrlFlowCnt += RAM_MARCHC_ISR_CALLER;
//				    __disable_irq();
//				    RamTestResult = STL_TranspMarch();
//				    __enable_irq();
//				    ISRCtrlFlowCntInv -= RAM_MARCHC_ISR_CALLER;
//	
//				    switch (RamTestResult)
//				    {
//				    case TEST_RUNNING:
//					    break;
//				    case TEST_OK:
//	#ifdef STL_VERBOSE
//					    /* avoid any long string output here in the interrupt, '#' marks ram test completed ok */
//	#ifndef __GNUC__
//					    putchar((int16_t)'#');
//	#else
//					    __io_putchar((int16_t)'#');
//	#endif /* __GNUC__ */
//	#endif  /* STL_VERBOSE */
//	#ifdef STL_EVAL_MODE
//					    /* Toggle LED1 for debug purposes */
//					    BSP_LED_Toggle(LED11);
//	#endif /* STL_EVAL_MODE */
//	#if defined(STL_EVAL_LCD)
//					    ++MyRAMCounter;
//	#endif /* STL_EVAL_LCD */
//					    break;
//				    case TEST_FAILURE:
//				    case CLASS_B_DATA_FAIL:
//				    default:
//	#ifdef STL_VERBOSE
//					    printf("\n\r >>>>>>>>>>>>>>>>>>>  RAM Error (March C- Run-time check)\n\r");
//	#endif  /* STL_VERBOSE */
//					    FailSafePOR();
//					    break;
//				    } /* End of the switch */
//	
//				    /* Do we reached the end of RAM test? */
//				    /* Verify 1st ISRCtrlFlowCnt integrity */
//				    if ((ISRCtrlFlowCnt ^ ISRCtrlFlowCntInv) == 0xFFFFFFFFuL)
//				    {
//					    if (RamTestResult == TEST_OK)
//					    {
//						    /* ==============================================================================*/
//						    /* MISRA violation of rule 17.4 - pointer arithmetic is used to check RAM test control flow */
//	#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
//	#pragma diag_suppress=Pm088
//	#endif /* __IAR_SYSTEMS_ICC__ */
//						    if (ISRCtrlFlowCnt != RAM_TEST_COMPLETED)
//	#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
//	#pragma diag_default=Pm088
//	#endif /* __IAR_SYSTEMS_ICC__ */
//							    /* ==============================================================================*/
//						    {
//	#ifdef STL_VERBOSE
//							    printf("\n\r Control Flow error (RAM test) \n\r");
//	#endif  /* STL_VERBOSE */
//							    FailSafePOR();
//						    }
//						    else  /* Full RAM was scanned */
//						    {
//							    ISRCtrlFlowCnt = 0u;
//							    ISRCtrlFlowCntInv = 0xFFFFFFFFuL;
//						    }
//					    } /* End of RAM completed if*/
//				    } /* End of control flow monitoring */
//				    else
//				    {
//	#ifdef STL_VERBOSE
//					    printf("\n\r Control Flow error in ISR \n\r");
//	#endif  /* STL_VERBOSE */
//					    FailSafePOR();
//				    }
//	#if defined STL_EVAL_MODE
//				    /* Toggle LED1 for debug purposes */
//				    BSP_LED_Toggle(LED11);
//	#endif  /* STL_EVAL_MODE */
//			    } /* End of the 10 ms timebase interrupt */
//		    }
}

// Overwrite the default ST HAL function (defined as "weak") in order to configure an HW timer
// used for mbed timeouts only (not used for the Systick configuration).
// Additional notes:
// - The default ST HAL_InitTick function initializes the Systick to 1 ms and this is not correct for mbed
//   as the mbed Systick as to be configured to 1 us instead.
// - Furthermore the Systick is configured by mbed RTOS directly.
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    uint32_t PclkFreq;

    // Get clock configuration
    // Note: PclkFreq contains here the Latency (not used after)
    HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &PclkFreq);

    // Get timer clock value
#if TIM_MST_PCLK == 1
    PclkFreq = HAL_RCC_GetPCLK1Freq();
#else
    PclkFreq = HAL_RCC_GetPCLK2Freq();
#endif

    // Enable timer clock
    TIM_MST_RCC;

    // Reset timer
    TIM_MST_RESET_ON;
    TIM_MST_RESET_OFF;

    // Configure time base
    TimMasterHandle.Instance = TIM_MST;
    TimMasterHandle.Init.Period          = 0xFFFFFFFF;

    // TIMxCLK = PCLKx when the APB prescaler = 1 else TIMxCLK = 2 * PCLKx
#if TIM_MST_PCLK == 1
    if (RCC_ClkInitStruct.APB1CLKDivider == RCC_HCLK_DIV1) {
#else
    if (RCC_ClkInitStruct.APB2CLKDivider == RCC_HCLK_DIV1) {
#endif
        TimMasterHandle.Init.Prescaler   = (uint16_t)((PclkFreq) / 1000000) - 1; // 1 us tick
    } else {
        TimMasterHandle.Init.Prescaler   = (uint16_t)((PclkFreq * 2) / 1000000) - 1; // 1 us tick
    }

    TimMasterHandle.Init.ClockDivision     = 0;
    TimMasterHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
#if !TARGET_STM32L1
    TimMasterHandle.Init.RepetitionCounter = 0;
#endif
#ifdef TIM_AUTORELOAD_PRELOAD_DISABLE
    TimMasterHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
#endif
    HAL_TIM_OC_Init(&TimMasterHandle);

//    NVIC_SetVector(TIM_MST_IRQ, (uint32_t)timer_irq_handler);    // JY
    NVIC_EnableIRQ(TIM_MST_IRQ);

    // Channel 1 for mbed timeout
    HAL_TIM_OC_Start(&TimMasterHandle, TIM_CHANNEL_1);

    // Freeze timer on stop/breakpoint
    // Define the FREEZE_TIMER_ON_DEBUG macro in mbed_app.json for example
#if !defined(NDEBUG) && defined(FREEZE_TIMER_ON_DEBUG) && defined(TIM_MST_DBGMCU_FREEZE)
    TIM_MST_DBGMCU_FREEZE;
#endif

    return HAL_OK;
}

#endif // !TIM_MST_16BIT
