/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    19-Jun-2017
  * @brief   Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32fxx_STLparam.h"
#include "stm32fxx_STLlib.h"
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#include <stdio.h>
/** @addtogroup STM32F7xx_IEC60335_Example
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if defined (STL_EVAL_MODE) || defined(STL_VERBOSE)
#define MESSAGE1   " STM32F7xx Cortex-M7 "
#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
#define MESSAGE2   " IEC60335 test @IARc "
#endif /* __IAR_SYSTEMS_ICC__ */
#ifdef __CC_ARM             /* KEIL Compiler */
#define MESSAGE2   " IEC60335 test @ARMc "
#endif /* __CC_ARM */
#ifdef __GNUC__             /* GCC Compiler */
#define MESSAGE2   " IEC60335 test @GCCc "
#endif /* __GNUC__ */
#endif /* STL_EVAL_MODE */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined(STL_EVAL_MODE) || defined(STL_VERBOSE)
UART_HandleTypeDef UartHandle;//

#endif /* STL_EVAL_MODE */



/* Private function prototypes -----------------------------------------------*/
////static void My_SystemClock_Config(void);
////static void MPU_Config(void);
////static void CPU_CACHE_Enable(void);


#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int16_t __io_putchar(int16_t ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
/* int32_t fputc(int32_t ch, FILE *f) */
#endif /* __GNUC__ */
	PUTCHAR_PROTOTYPE;


/* Private functions ---------------------------------------------------------*/


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
void My_SystemClock_Config(void)
{
	
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };//
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };//
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };//

	HAL_PWR_DeInit();

	HAL_PWR_EnableBkUpAccess();
	
	// Enable power clock
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)	//JY 20201125
	{
#ifdef STL_VERBOSE_POR
		SEGGER_RTT_WriteString(0, " PLL Osc config failure \n\r");
		printf(" PLL Osc config failure \n\r");
#endif  /* STL_VERBOSE_POR */
		 FailSafePOR();
	}

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
//	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;	
//	RCC_OscInitStruct.HSEState = RCC_HSE_ON; // External xtal on OSC_IN/OSC_OUT  JY 20230201
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS; // External clock on OSC_IN  JY 20230201
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 216;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 9;	
	 
  /* Activate the OverDrive to reach the 216 MHz Frequency */  
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
#ifdef STL_VERBOSE_POR
	  SEGGER_RTT_WriteString(0, " PWR over drive config failure \n\r");
    printf(" PWR over drive config failure \n\r");
#endif  /* STL_VERBOSE_POR */
    FailSafePOR();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; 
	
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
#ifdef STL_VERBOSE_POR
	  SEGGER_RTT_WriteString(0, " PLL clock config failure\n\r");
    printf(" PLL clock config failure\n\r");
#endif  /* STL_VERBOSE_POR */
    FailSafePOR();
  }
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief  Startup Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 16
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            PLL_R                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
 * @param  None
 * @retval None
 */
void StartUpClock_Config(void) {
  RCC_ClkInitTypeDef RCC_ClkInitStruct;//
  RCC_OscInitTypeDef RCC_OscInitStruct;//

  /* Activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.HSICalibrationValue = 0x10; /* !!! DEFAULT TRIM IS FORCED !!! */
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) 
 // RCC_OscInitStruct.PLL.PLLR = 7;  
	RCC_OscInitStruct.PLL.PLLR = 2;  
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */  
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
#ifdef STL_VERBOSE_POR
	 SEGGER_RTT_WriteString(0, "PLL clock config failure\n\r");
     printf("PLL clock config failure\n\r");
#endif  /* STL_VERBOSE_POR */
     FailSafePOR();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
#ifdef STL_VERBOSE_POR
	SEGGER_RTT_WriteString(0, "PLL clock switch failure\n\r");
    printf("PLL clock switch failure\n\r");
#endif  /* STL_VERBOSE_POR */
    FailSafePOR();
  }
}


#ifdef STL_VERBOSE
/* -------------------------------------------------------------------------*/
/**
 * @brief  Configure the UART peripheral
 * @param  None
 * @retval None
 */
void USART_Configuration(void) 
{
//  UartHandle.Instance        = USARTx;
//  UartHandle.Init.BaudRate = 115200;
//  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
//  UartHandle.Init.StopBits   = UART_STOPBITS_1;
//  UartHandle.Init.Parity = UART_PARITY_NONE;
//  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
//  UartHandle.Init.Mode       = UART_MODE_TX;
//  __HAL_UART_RESET_HANDLE_STATE(&UartHandle);
//  BSP_COM_Init(COM1, &UartHandle);

	
	UartHandle.Instance = USART3;
//	HAL_UART_DeInit(&UartHandle);
	UartHandle.Init.BaudRate = 9600;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits = UART_STOPBITS_1;
	UartHandle.Init.Parity = UART_PARITY_NONE;
	UartHandle.Init.Mode = UART_MODE_TX;//UART_MODE_TX_RX;
	UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
//	UartHandle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
//	UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	HAL_UART_Init(&UartHandle);
	if (HAL_UART_Init(&UartHandle) != HAL_OK)
	{
//		Error_Handler();
	}
}
/* -------------------------------------------------------------------------*/
/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE {
	/* Place your implementation of fputc here */
	/* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
	HAL_UART_Transmit(&UartHandle, (uint8_t *) &ch, 1, 0xFFFF);

	return ch;
}

#ifdef  __GNUC__
////#if 0 //Willy
/* -------------------------------------------------------------------------*/
/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
//int16_t _write(int16_t file, int8_t *ptr, int16_t len) {
//	int16_t DataIdx;
//
//	for (DataIdx = 0; DataIdx < len; DataIdx++) {
//		__io_putchar(*ptr++);
//	}
//	return len;
//}
#endif /* __GNUC__ */

#endif /* STL_VERBOSE */

/**
  * @brief  Configure the MPU attributes as Write Through for Internal SRAM1/2.
  * @note   The Base Address is 0x20020000 since this memory interface is the AXI.
  *         The Configured Region Size is 512KB because the internal SRAM1/2 
  *         memory size is 384KB.
  * @param  None
  * @retval None
  */
void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;//
  
  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as WT for SRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x20020000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;		//JY
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	printf(">>>Assert failed<<< Wrong parameters value: file %s on line %d\r\n", file, line);

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 
void SysTickClassB_Handler(void)
{    
	/* Verify TickCounter integrity */
	if ((TickCounter ^ TickCounterInv) == 0xFFFFFFFFuL)
	{
		TickCounter++;
		TickCounterInv = ~TickCounter;
	
		if (TickCounter >= SYSTICK_10MS_TB)
		{
			uint32_t RamTestResult =0;//
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
      
			/* Reset timebase counter */
			TickCounter = 0u;
			TickCounterInv = 0xFFFFFFFFuL;

			/* Set Flag read in main loop */
			TimeBaseFlag = 0xAAAAAAAAuL;
			TimeBaseFlagInv = 0x55555555uL;

			ISRCtrlFlowCnt += RAM_MARCHC_ISR_CALLER;
			__disable_irq();
			RamTestResult = STL_TranspMarch();
			__enable_irq();
			ISRCtrlFlowCntInv -= RAM_MARCHC_ISR_CALLER;

			switch (RamTestResult)
			{
			case TEST_RUNNING:
				break;
			case TEST_OK:
			#ifdef STL_VERBOSE
				/* avoid any long string output here in the interrupt, '#' marks ram test completed ok */
			#ifndef __GNUC__
				putchar((int16_t)'#');
			#else
				__io_putchar((int16_t)'#');
			#endif /* __GNUC__ */
			#endif  /* STL_VERBOSE */

			#if defined(STL_EVAL_LCD)
				++MyRAMCounter;
			#endif /* STL_EVAL_LCD */
				break;
			case TEST_FAILURE:
			case CLASS_B_DATA_FAIL:
			default:
			#ifdef STL_VERBOSE
				printf("\n\r >>>>>>>>>>>>>>>>>>>  RAM Error (March C- Run-time check)\n\r");
				SEGGER_RTT_WriteString(0, "\n\r >>>>>>>>>>>>>>>>>>>  RAM Error (March C- Run-time check)\n\r");
			#endif  /* STL_VERBOSE */
				FailSafePOR();
				break;
			} /* End of the switch */

			/* Do we reached the end of RAM test? */
			/* Verify 1st ISRCtrlFlowCnt integrity */
			if ((ISRCtrlFlowCnt ^ ISRCtrlFlowCntInv) == 0xFFFFFFFFuL)
			{
				if (RamTestResult == TEST_OK)
				{
					/* ==============================================================================*/
					/* MISRA violation of rule 17.4 - pointer arithmetic is used to check RAM test control flow */
				#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
				#pragma diag_suppress=Pm088
				#endif /* __IAR_SYSTEMS_ICC__ */
					if (ISRCtrlFlowCnt != RAM_TEST_COMPLETED)
				#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
				#pragma diag_default=Pm088
				#endif /* __IAR_SYSTEMS_ICC__ */
						/* ==============================================================================*/
					{
					#ifdef STL_VERBOSE
						SEGGER_RTT_WriteString(0, "\n\r Control Flow error (RAM test) \n\r");
						printf("\n\r Control Flow error (RAM test) \n\r");
					#endif  /* STL_VERBOSE */
						FailSafePOR();
					}
					else  /* Full RAM was scanned */
					{
						ISRCtrlFlowCnt = 0u;
						ISRCtrlFlowCntInv = 0xFFFFFFFFuL;
					}
				} /* End of RAM completed if*/
			} /* End of control flow monitoring */
			else
			{
			#ifdef STL_VERBOSE
				SEGGER_RTT_WriteString(0, "\n\r Control Flow error in ISR \n\r");
				printf("\n\r Control Flow error in ISR \n\r");
			#endif  /* STL_VERBOSE */
				FailSafePOR();
			}

		} /* End of the 10 ms timebase interrupt */
	}
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
