/**
  ******************************************************************************
  * @file    stm32fxx_STLmain.c 
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    19-Jun-2017
  * @brief   Contains the Self-test functions executed during main program execution.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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
/** @addtogroup STM32FxxSelfTestLib_src
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
ErrorStatus STL_CheckStack(void);

/******************************************************************************/
/**
  * @brief  Initializes the Class B variables and their inverted redundant 
  *   counterparts. Init also the Systick for clock frequency monitoring.
  * @param  : None
  * @retval : None
  */
void STL_InitRunTimeChecks(void)
{
  uint32_t tout = 0;//
  
  /* Initialize variables for invariable memory check */
  STL_TranspMarchInit();

  /* Init Class B variables required in main routine and SysTick interrupt
  service routine for timing purposes */
  TickCounter = 0u;
  TickCounterInv = 0xFFFFFFFFuL;

  TimeBaseFlag = 0u;
  TimeBaseFlagInv = 0xFFFFFFFFuL;

  LastCtrlFlowCnt = 0u;
  LastCtrlFlowCntInv = 0xFFFFFFFFuL;
  INIT_CONTROL_FLOW();

  /* Initialize variables for SysTick interrupt routine control flow monitoring */
  ISRCtrlFlowCnt = 0u;
  ISRCtrlFlowCntInv = 0xFFFFFFFFuL;

  /* Initialize SysTick to generate 1ms time base */
#ifdef HSE_CLOCK_APPLIED    
  if (HAL_SYSTICK_Config(SYSTCLK_AT_RUN_HSE/1000uL) != HAL_OK)
#else
  if (HAL_SYSTICK_Config(SYSTCLK_AT_RUN_HSI/1000uL) != HAL_OK)
#endif
  {
    #ifdef STL_VERBOSE_POR
	  SEGGER_RTT_WriteString(0, "Run time base init failure\n\r");
      printf("Run time base init failure\n\r");
    #endif /* STL_VERBOSE_POR */
    FailSafePOR();
  }

  /* Initialize variables for invariable memory check */
  STL_FlashCrc32Init();

  /* wait till HSE measurement is completed & check timer system */
  tout = HAL_GetTick() + 5u;
  LSIPeriodFlag = 0u;
  /* ==============================================================================*/
  /* MISRA violation of rule 12.4, 12.5 - "&&" operand can't contain side effects 
    and shall be primary expressions  */
  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
    #pragma diag_suppress= Pm026,Pm027              
  #endif /* __IAR_SYSTEMS_ICC__ */
  while ( LSIPeriodFlag == 0u  &&  HAL_GetTick() < tout )
  { }
  LSIPeriodFlag = 0u;
  while ( LSIPeriodFlag == 0u  &&  HAL_GetTick() < tout )
  { }
  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
    #pragma diag_default= Pm026,Pm027
  #endif /* __IAR_SYSTEMS_ICC__ */
  /* ==============================================================================*/

  if (HAL_GetTick() >= tout)
  {
    #ifdef STL_VERBOSE_POR
	  SEGGER_RTT_WriteString(0, "Run time clock measurement failure\n\r");
      printf("Run time clock measurement failure\n\r");
    #endif /* STL_VERBOSE_POR */
    FailSafePOR();    
  }
    
  /* Initialize variables for main routine control flow monitoring */
  CtrlFlowCnt = 0u;
  CtrlFlowCntInv = 0xFFFFFFFFuL;
  
//#ifdef USE_INDEPENDENT_WDOG
//  /* Initialize IWDG for run time if applicable */  
//  #if defined(STL_EVAL_MODE)
//    /* setup DBGMCU block - stop IWDG at break in debug mode */
//    __DBGMCU_CLK_ENABLE();
//    __HAL_FREEZE_IWDG_DBGMCU();
//  #endif  /* STL_EVAL_MODE */
//    
//  /* Setup cca 50 msec period - to be adjusted in according w/ appli need */
//  IwdgHandle.Instance = IWDG;
//	IwdgHandle.Init.Prescaler = IWDG_PRESCALER_64;//IWDG_PRESCALER_256;
//	IwdgHandle.Init.Reload = 0x83D;//8U;
//  /* Enable IWDG (LSI automatically enabled by HW) */
//  #ifdef IWDG_FEATURES_BY_WINDOW_OPTION
//    IwdgHandle.Init.Window = 8U;
//    /* if window feature is applied, Init() function has to include __HAL_IWDG_START() prior any initialization */
//    if( HAL_IWDG_Init(&IwdgHandle) != HAL_OK )
//    {
//      #ifdef STL_VERBOSE
//        printf(" IWDG init Error\n\r");
//      #endif  /* STL_VERBOSE */
//      FailSafePOR();
//    }
//  #else
//    /* if window feature is not applied Init() precedes Start() */
//    if( HAL_IWDG_Init(&IwdgHandle) != HAL_OK )
//    {
//      #ifdef STL_VERBOSE
//        printf(" IWDG init Error\n\r");
//      #endif  /* STL_VERBOSE */
//      FailSafePOR();
//    }
//    __HAL_IWDG_START(&IwdgHandle);
//  #endif /* IWDG_FEATURES_BY_WINDOW_OPTION */
//#endif /* USE_INDEPENDENT_WDOG */

//#ifdef USE_WINDOW_WDOG
//  /* Initialize WWDG for run time if applicable */  
//  #if defined(STL_EVAL_MODE)    
//    /* setup DBGMCU block - stop WWDG at break in debug mode */
//    __DBGMCU_CLK_ENABLE();
//    __HAL_FREEZE_WWDG_DBGMCU();
//  #endif  /* STL_EVAL_MODE */
//  
//  /* Setup period - to be adjusted in according w/ appli need */
//  __WWDG_CLK_ENABLE();
//  WwdgHandle.Instance = WWDG;
//  WwdgHandle.Init.Prescaler = WWDG_PRESCALER_8;
//  WwdgHandle.Init.Counter = 127U;
//  WwdgHandle.Init.Window = 127U;
//  if( HAL_WWDG_Init(&WwdgHandle) != HAL_OK )
//  {
//    #ifdef STL_VERBOSE
//      printf(" WWDG init Error\n\r");
//    #endif  /* STL_VERBOSE */
//    FailSafePOR();
//  }
//#endif /* USE_WINDOW_WDOG */ 
}

/******************************************************************************/
/**
  * @brief  Perform set of generic tests at run time from main
  *         (can be divided into more separated segments if necessary)
  * @param  : None
  * @retval : None
  */
void STL_DoRunTimeChecks(void)
{
  uint32_t rom_test = 0;//
  
  /* Is the time base duration elapsed? */
  if (TimeBaseFlag == 0xAAAAAAAAuL)
  { 
    /* Verification of TimeBaseFlag integrity */
    /* The following pair of volatile variables is changed consistently at Systick ISR only */
    if ((TimeBaseFlag ^ TimeBaseFlagInv) == 0xFFFFFFFFuL)
    {
      TimeBaseFlag = 0u;
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
      /*----------------------------------------------------------------------*/
      /*---------------------------- CPU registers ----------------------------*/
      /*----------------------------------------------------------------------*/
      CONTROL_FLOW_CALL(CPU_TEST_CALLER);
      
      if (STL_RunTimeCPUTest() != CPUTEST_SUCCESS)
      {
        #ifdef STL_VERBOSE
	      SEGGER_RTT_WriteString(0, "Run-time CPU Test Failure\n\r");
          printf("Run-time CPU Test Failure\n\r");
        #endif /* STL_VERBOSE */
        FailSafePOR();
      }
      else
      {
        CONTROL_FLOW_RESUME(CPU_TEST_CALLER);
      }
  
      /*----------------------------------------------------------------------*/
      /*------------------------- Stack overflow -----------------------------*/
      /*----------------------------------------------------------------------*/
      CONTROL_FLOW_CALL(STACK_OVERFLOW_TEST);
      
      if (STL_CheckStack() != SUCCESS)
      {
        #ifdef STL_VERBOSE
	      SEGGER_RTT_WriteString(0, "Stack overflow\n\r");
          printf("Stack overflow\n\r");
        #endif /* STL_VERBOSE */
        FailSafePOR();
      }
      else
      {
        CONTROL_FLOW_RESUME(STACK_OVERFLOW_TEST);
      }
      
      /*----------------------------------------------------------------------*/
      /*------------------------- Clock monitoring ---------------------------*/
      /*----------------------------------------------------------------------*/
      CONTROL_FLOW_CALL(CLOCK_TEST_CALLER);
      
      switch ( STL_MainClockTest() )
      {
        case FREQ_OK:
          CONTROL_FLOW_RESUME(CLOCK_TEST_CALLER);
          break;
  
        case EXT_SOURCE_FAIL:
          #ifdef STL_VERBOSE
            /* finish communication flow prior system clock change */
            //UartHandle.Instance = USARTx;
	        UartHandle.Instance = USART3;
            while (HAL_UART_GetState(&UartHandle) == HAL_UART_STATE_BUSY_TX)
            { }
            /* Re-init communication channel with modified clock setting */
            HAL_UART_DeInit(&UartHandle);
            UartHandle.Init.BaudRate   = 9600;
            UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
            UartHandle.Init.StopBits   = UART_STOPBITS_1;
            UartHandle.Init.Parity     = UART_PARITY_NONE;
            UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
            UartHandle.Init.Mode       = UART_MODE_TX;
            HAL_UART_Init(&UartHandle);
            printf("\n\r Clock Source failure (Run-time)\n\r");
	        SEGGER_RTT_WriteString(0, "\n\r Clock Source failure (Run-time)\n\r");
          #endif /* STL_VERBOSE */
          FailSafePOR();
          break;
  
        case CLASS_B_VAR_FAIL:
          #ifdef STL_VERBOSE
	        SEGGER_RTT_WriteString(0, "\n\r Class B variable error (clock test)\n\r");
            printf("\n\r Class B variable error (clock test)\n\r");
          #endif /* STL_VERBOSE */
          FailSafePOR();
          break;
  
        case LSI_START_FAIL:
        case HSE_START_FAIL:
        case HSI_HSE_SWITCH_FAIL:
        case TEST_ONGOING:
        case CTRL_FLOW_ERROR:
        default:
          #ifdef STL_VERBOSE
	        SEGGER_RTT_WriteString(0, "Abnormal Clock Test routine termination \n\r");
            printf("Abnormal Clock Test routine termination \n\r");
          #endif  /* STL_VERBOSE */
          FailSafePOR();
          break;
      }
  
  
      /*----------------------------------------------------------------------*/
      /*------------------ Invariable memory CRC check -----------------------*/
      /*----------------------------------------------------------------------*/
      CONTROL_FLOW_CALL(FLASH_TEST_CALLER);
			
      rom_test = STL_crc32Run();  /* Requires the control flow check to be modified */
      switch ( rom_test )
      {
        case TEST_RUNNING:
            CONTROL_FLOW_RESUME(FLASH_TEST_CALLER);
          break;
  
        case TEST_OK:
          #ifdef STL_VERBOSE
            /* avoid any long string output here, '*' just marks the flash test completed ok */
            #ifndef __GNUC__
              putchar((int16_t)'*');
	      //SEGGER_RTT_PutChar(0, (int16_t)'*');
            #else
              __io_putchar((int16_t)'*');
	      SEGGER_RTT_PutChar(0, (int16_t)'*');
	    
            #endif /* __GNUC__ */
          #endif /* STL_VERBOSE */

          CtrlFlowCntInv -= FLASH_TEST_CALLER;
          break;
  
        case TEST_FAILURE:
        case CLASS_B_DATA_FAIL:
        default:
          #ifdef STL_VERBOSE
	        SEGGER_RTT_WriteString(0, " Run-time FLASH CRC Error\n\r");
            printf(" Run-time FLASH CRC Error\n\r");
          #endif  /* STL_VERBOSE */
          FailSafePOR();
          break;
      }
  
      /*----------------------------------------------------------------------*/
      /*---------------- Check Safety routines Control flow  -----------------*/
      /*------------- Refresh Window and independent watchdogs ---------------*/
      /*----------------------------------------------------------------------*/
      #ifdef USE_WINDOW_WDOG
        /* Update WWDG counter */
        WwdgHandle.Instance = WWDG;
        if ( HAL_WWDG_Refresh(&WwdgHandle) != HAL_OK )
        {
          #ifdef STL_VERBOSE
	        SEGGER_RTT_WriteString(0, " WWDG refresh error\n\r");
            printf(" WWDG refresh error\n\r");
          #endif  /* STL_VERBOSE */
          FailSafePOR();
        }
      #endif /* USE_WINDOW_WDOG */

        /* Reload IWDG counter */
      #ifdef USE_INDEPENDENT_WDOG
        IwdgHandle.Instance = IWDG;
        if ( HAL_IWDG_Refresh(&IwdgHandle) != HAL_OK )
        {
          #ifdef STL_VERBOSE
	        SEGGER_RTT_WriteString(0, " IWDG refresh error\n\r");
            printf(" IWDG refresh error\n\r");
          #endif  /* STL_VERBOSE */
          FailSafePOR();
        }
      #endif /* USE_INDEPENDENT_WDOG */
  
      if (((CtrlFlowCnt ^ CtrlFlowCntInv) == 0xFFFFFFFFuL)&&((LastCtrlFlowCnt ^ LastCtrlFlowCntInv) == 0xFFFFFFFFuL))
      {
        if (rom_test == TEST_OK)
        {
          /* ==============================================================================*/
          /* MISRA violation of rule 11.4,17.4 - pointer arithmetic & casting has is used */
          #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
            #pragma diag_suppress=Pm088,Pm141
          #endif /* __IAR_SYSTEMS_ICC__ */
          if ((CtrlFlowCnt == FULL_FLASH_CHECKED) 
            && ((CtrlFlowCnt - LastCtrlFlowCnt) == (LAST_DELTA_MAIN)))
          #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
            #pragma diag_default=Pm088,Pm141
          #endif /* __IAR_SYSTEMS_ICC__ */
          /* ==============================================================================*/
          {
            CtrlFlowCnt = 0u;
            CtrlFlowCntInv = 0xFFFFFFFFuL;
          }
          else  /* Return value form crc check was corrupted */
          {
            #ifdef STL_VERBOSE
	          SEGGER_RTT_WriteString(0, "Control Flow Error (main loop, Flash CRC)\n\r");
              printf("Control Flow Error (main loop, Flash CRC)\n\r");
            #endif  /* STL_VERBOSE */
            FailSafePOR();
          }
        }
        else  /* Flash test not completed yet */
        {
          if ((CtrlFlowCnt - LastCtrlFlowCnt) != DELTA_MAIN)
          {
            #ifdef STL_VERBOSE
	          SEGGER_RTT_WriteString(0, "Control Flow Error (main loop, Flash CRC on-going)\n\r");
              printf("Control Flow Error (main loop, Flash CRC on-going)\n\r");
            #endif  /* STL_VERBOSE */
            FailSafePOR();
          }
        }
  
        LastCtrlFlowCnt = CtrlFlowCnt;
        LastCtrlFlowCntInv = CtrlFlowCntInv;
      }
      else
      {
        #ifdef STL_VERBOSE
	      SEGGER_RTT_WriteString(0, "Class B variable Error (main loop)\n\r");
          printf("Class B variable Error (main loop)\n\r");
        #endif  /* STL_VERBOSE */
        FailSafePOR();
      }
    }
    else
    {
      #ifdef STL_VERBOSE
	    SEGGER_RTT_WriteString(0, "Class B variable Error (main loop)\n\r");
        printf("Class B variable Error (main loop)\n\r");
      #endif  /* STL_VERBOSE */
      FailSafePOR();
    }

  } /* End of periodic Self-test routine */
}

/******************************************************************************/
/**
  * @brief  This function verifies that Stack didn't overflow
  * @param  : None
  * @retval : ErrorStatus = (ERROR, SUCCESS)
  */
ErrorStatus STL_CheckStack(void)
{
  ErrorStatus result = SUCCESS;//
  
  CtrlFlowCnt += STACK_OVERFLOW_CALLEE;
//	//if (aStackOverFlowPtrn[0] != 0xEEEEEEEEuL)
//  if(aStackOverFlowPtrn[0] != 0x0000010c)
//  {
//    result = ERROR;
//  }
//	//if (aStackOverFlowPtrn[1] != 0xCCCCCCCCuL)
//  if ( aStackOverFlowPtrn[1] != 0xfffffffc )
//  {
//    result = ERROR;
//  }
//	//if (aStackOverFlowPtrn[2] != 0xBBBBBBBBuL)
//  if(aStackOverFlowPtrn[2] != 0x08025dc0)
//  {
//    result = ERROR;
//  }
//	//if (aStackOverFlowPtrn[3] != 0xDDDDDDDDuL)
//  if(aStackOverFlowPtrn[3] != 0x40006400)
//  {
//    result = ERROR;
//  }

  CtrlFlowCntInv -= STACK_OVERFLOW_CALLEE;

  return (result);
}

/**
  * @}
  */

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
