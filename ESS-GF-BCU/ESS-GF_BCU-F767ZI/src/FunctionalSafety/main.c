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
UART_HandleTypeDef UartHandle;
#endif /* STL_EVAL_MODE */

#if defined(STL_EVAL_LCD)
char MyString[6];
uint32_t MyRAMCounter;
uint32_t MyFLASHCounter;
#endif /* STL_EVAL_LCD */

/* Private function prototypes -----------------------------------------------*/
static void My_SystemClock_Config(void);
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);


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
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Configure the MPU attributes as Write Through */
  MPU_Config();

  /* Enable the CPU Cache */
  CPU_CACHE_Enable();
  
  /* STM32F7xx HAL library initialization:
       - Configure the Flash ART accelerator on ITCM interface
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 216 MHz */
  My_SystemClock_Config();

  #if defined (STL_VERBOSE)
  /* Verbose messages ------------------------------------------------------*/
  USART_Configuration();
  printf("\n\r %s\n\r", MESSAGE1);
  printf(" %s\n\r", MESSAGE2);
  printf(" ... main routine starts ...\r\n");
#endif /* STL_VERBOSE */

#if defined (STL_EVAL_MODE)
  /* Evaluation board control ----------------------------------------------*/
  Eval_Board_HW_Init();

#if defined(STL_EVAL_LCD)
  /* Display message on STM32F4XX-EVAL LCD */
 // BSP_LCD_SetFont(&Font20);
  BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_ClearStringLine(0);
  BSP_LCD_ClearStringLine(1);
  BSP_LCD_DisplayStringAt(0, LINE(0), (uint8_t *)MESSAGE1, CENTER_MODE);
  BSP_LCD_DisplayStringAt(0, LINE(1), (uint8_t *)MESSAGE2, CENTER_MODE);
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DisplayStringAtLine(2, (uint8_t *)("StartUp test OK     "));
  BSP_LCD_DisplayStringAtLine(3, (uint8_t *)("Main entry...       "));
  BSP_LCD_DisplayStringAtLine(4, (uint8_t *)("Class B RAM checks:     "));
  BSP_LCD_DisplayStringAtLine(5, (uint8_t *)("Code Flash checks:      "));
#endif /* STL_EVAL_LCD */

#endif /* STL_EVAL_MODE */


  /* Self test routines initialization ---------------------------------------*/
#if defined(STL_EVAL_MODE)
  /* if you debug TIM5 it is helpful to uncomment next lines */
  /*
  __DBGMCU_CLK_ENABLE();
  __HAL_FREEZE_TIM5_DBGMCU();
  */
  BSP_LED_On(LED3);
#endif  /* STL_EVAL_MODE */

	/* -------------------------------------------------------------------------*/
	/* This is where the main self-test routines are initialized */
	STL_InitRunTimeChecks();
	/* -------------------------------------------------------------------------*/
#if defined STL_EVAL_MODE
  BSP_LED_Off(LED3);
#endif  /* STL_EVAL_MODE */

  /* Add your application initialization code here  */
  
  /* Infinite loop */  
  while (1)
  {
    /* -----------------------------------------------------------------------*/
    /* This is where the main self-test routines are executed */
    STL_DoRunTimeChecks();
    /* -----------------------------------------------------------------------*/

#if defined(STL_EVAL_LCD)
    sprintf(MyString, "%5lu", MyRAMCounter);
    BSP_LCD_DisplayStringAt(338, LINE(4), (uint8_t *)MyString, LEFT_MODE);
    sprintf(MyString, "%5lu", MyFLASHCounter);
    BSP_LCD_DisplayStringAt(338, LINE(5), (uint8_t *)MyString, LEFT_MODE);
#endif /* STL_EVAL_LCD */
  }
}
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
static void My_SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
#ifdef HSE_CLOCK_APPLIED    
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;  
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
  RCC_OscInitStruct.PLL.PLLR = 7;  
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */  
#else
  /* Enable HSI Oscillator and activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.HSICalibrationValue = 0x10; /* !!! DEFAULT TRIM IS FORCED !!! */
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
  RCC_OscInitStruct.PLL.PLLR = 7;  
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */  
#endif
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
#ifdef STL_VERBOSE_POR
    printf("PLL Osc config failure\n\r");
#endif  /* STL_VERBOSE_POR */
    FailSafePOR();
  }
  
  /* Activate the OverDrive to reach the 216 MHz Frequency */  
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
#ifdef STL_VERBOSE_POR
    printf("PWR over drive config failure\n\r");
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
    printf("PLL clock config failure\n\r");
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
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

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
#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
  RCC_OscInitStruct.PLL.PLLR = 7;  
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */  
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
#ifdef STL_VERBOSE_POR
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
    printf("PLL clock switch failure\n\r");
#endif  /* STL_VERBOSE_POR */
    FailSafePOR();
  }
}

#if defined STL_EVAL_MODE
/* -------------------------------------------------------------------------*/
/**
 * @brief  Initialization of evaluation board HW
 * @param :  None
 * @retval : None
 */
void Eval_Board_HW_Init(void) {
	/* init LEDs on evaluation board -----------------------------------------*/
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED3);

#if defined (STL_EVAL_LCD)
  /* Display message on STM32F4XX-EVAL LCD */
  /* Initialize the LCD */
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(0,LCD_FRAME_BUFFER_LAYER0);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
#endif /* STL_EVAL_LCD  */
}
#endif /* STL_EVAL_MODE */

#ifdef STL_VERBOSE
/* -------------------------------------------------------------------------*/
/**
 * @brief  Configure the UART peripheral
 * @param  None
 * @retval None
 */
void USART_Configuration(void) {
  UartHandle.Instance        = USARTx;
  UartHandle.Init.BaudRate = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX;
  __HAL_UART_RESET_HANDLE_STATE(&UartHandle);
  BSP_COM_Init(COM1, &UartHandle);
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
#ifdef __GNUC__
/* -------------------------------------------------------------------------*/
/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
int16_t _write(int16_t file, int8_t *ptr, int16_t len) {
	int16_t DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		__io_putchar(*ptr++);
	}
	return len;
}
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
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  
  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as WT for SRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x20020000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
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
static void CPU_CACHE_Enable(void)
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
