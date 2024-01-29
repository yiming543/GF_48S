/**
  ******************************************************************************
  * @file    stm32fxx_STLclassBvar.h 
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    19-June-2017
  * @brief   Contains all safety critical variables; they must have
  *          predefined addresses and inverse redundant storage
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STL_CLASS_B_VAR_H
#define __STL_CLASS_B_VAR_H

/* This avoids having multiply defined global variables */
#ifdef ALLOC_GLOBALS
#define EXTERN
#else
#define EXTERN extern
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
  /* RAM location for temporary storage of original values at run time RAM transparent test */
  EXTERN __no_init uint32_t aRunTimeRamBuf[RT_RAM_BLOCKSIZE + 2U] @ "RUN_TIME_RAM_BUF";

  /* pointers for run time RAM transparent test */
  EXTERN __no_init uint32_t *pRunTimeRamChk @ "RUN_TIME_RAM_PNT";
  EXTERN __no_init uint32_t *pRunTimeRamChkInv @ "RUN_TIME_RAM_PNT";
  EXTERN __no_init uint32_t aGAP_FOR_RAM_TEST_OVERLAY[2] @ "RUN_TIME_RAM_PNT";

  /* Class B non inverted variables area */
  /* Counter for verifying correct program execution at start */
  EXTERN __no_init uint32_t CtrlFlowCnt @ "CLASS_B_RAM";
  /* Counter for verifying correct program execution in interrupt */
  EXTERN __no_init uint32_t ISRCtrlFlowCnt @ "CLASS_B_RAM";

  EXTERN __no_init uint32_t PeriodValue @ "CLASS_B_RAM";      /* LSI period measurement at TIM5 IRQHandler */
   /* Software time base used in main program (incremented in SysTick timer ISR */
  EXTERN __no_init uint32_t TickCounter @ "CLASS_B_RAM";

  EXTERN __no_init __IO uint32_t TimeBaseFlag @ "CLASS_B_RAM";/* Indicates to the main routine a 100ms tick */
  EXTERN __no_init __IO uint32_t LSIPeriodFlag @ "CLASS_B_RAM";/* Indicates to the main routine a 100ms tick */
  /* Stores the Control flow counter from one main loop to the other */
  EXTERN __no_init uint32_t LastCtrlFlowCnt @ "CLASS_B_RAM";

  EXTERN __no_init uint32_t *pRunCrc32Chk @ "CLASS_B_RAM";    /* Pointer to FLASH for crc32 run-time tests */
  EXTERN __no_init uint32_t RefCrc32 @ "CLASS_B_RAM";         /* Reference 32-bit CRC for run-time tests */
    
  /* Class B inverted variables area */
  EXTERN __no_init uint32_t CtrlFlowCntInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init uint32_t ISRCtrlFlowCntInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init uint32_t PeriodValueInv @ "CLASS_B_RAM_REV";  
  EXTERN __no_init uint32_t TickCounterInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init __IO uint32_t TimeBaseFlagInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init __IO uint32_t LSIPeriodFlagInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init uint32_t LastCtrlFlowCntInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init uint32_t *pRunCrc32ChkInv @ "CLASS_B_RAM_REV";
  EXTERN __no_init uint32_t RefCrc32Inv @ "CLASS_B_RAM_REV";

  /* Magic pattern for Stack overflow in this array */
  EXTERN __no_init __IO uint32_t aStackOverFlowPtrn[4] @ "STACK_BOTTOM";
#endif  /* __IAR_SYSTEMS_ICC__ */


#ifdef __CC_ARM   /* KEIL Compiler */
  /* RAM location for temporary storage of original values at run time RAM transparent test */
  EXTERN uint32_t aRunTimeRamBuf[RT_RAM_BLOCKSIZE + 2] __attribute__((section("RUN_TIME_RAM_BUF")));

  /* RAM pointer for run-time tests */
  EXTERN uint32_t *pRunTimeRamChk        __attribute__((section("RUN_TIME_RAM_PNT")));
  EXTERN uint32_t *pRunTimeRamChkInv     __attribute__((section("RUN_TIME_RAM_PNT")));
  EXTERN uint32_t aGAP_FOR_RAM_TEST_OVERLAY[2] __attribute__((section("RUN_TIME_RAM_PNT")));

  /* Note: the zero_init forces the linker to place variables in the bss section */
  /* This allows the UNINIT directive (in scatter file) to work. On the contrary */
  /* all Class B variables pairs should be initialized properly by user before using them */

  /* Counter for verifying correct program execution at start */
  EXTERN uint32_t CtrlFlowCnt             __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t CtrlFlowCntInv          __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Counter for verifying correct program execution in interrupt */
  EXTERN uint32_t ISRCtrlFlowCnt          __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t ISRCtrlFlowCntInv       __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* LSI period measurement at TIM5 IRQHandler */
  EXTERN uint32_t PeriodValue           __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t PeriodValueInv        __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Sofware time base used in main program (incremented in SysTick timer ISR */
  EXTERN uint32_t TickCounter             __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t TickCounterInv          __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Indicates to the main routine a 100ms tick */
  EXTERN __IO uint32_t TimeBaseFlag       __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN __IO uint32_t TimeBaseFlagInv    __attribute__((section("CLASS_B_RAM_REV"), zero_init));
  
  /* Indicates to the main routine a 100ms tick */
  EXTERN __IO uint32_t LSIPeriodFlag      __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN __IO uint32_t LSIPeriodFlagInv   __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Stores the Control flow counter from one main loop to the other */
  EXTERN uint32_t LastCtrlFlowCnt         __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t LastCtrlFlowCntInv      __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Pointer to FLASH for crc32 run-time tests */
  EXTERN uint32_t *pRunCrc32Chk           __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t *pRunCrc32ChkInv        __attribute__((section("CLASS_B_RAM_REV"), zero_init));

/* Reference 32-bit CRC for run-time tests */
  EXTERN uint32_t RefCrc32                __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t RefCrc32Inv             __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Magic pattern for Stack overflow in this array */
  EXTERN __IO uint32_t aStackOverFlowPtrn[4]   __attribute__((section("STACK_BOTTOM"), zero_init));
#endif  /* __CC_ARM */

#ifdef __GNUC__   /* GCC Compiler */
  #ifndef ALLOC_GLOBALS
  /* RAM location for temporary storage of original values at run time RAM transparent test */
  extern uint32_t aRunTimeRamBuf[RT_RAM_BLOCKSIZE + 2] __attribute__((section(".run_time_ram_buf")));

  /* RAM pointer for run-time tests */
  extern uint32_t *pRunTimeRamChk        __attribute__((section(".run_time_ram_pnt")));
  extern uint32_t *pRunTimeRamChkInv     __attribute__((section(".run_time_ram_pnt")));//
  extern uint32_t aGAP_FOR_RAM_TEST_OVERLAY[2] __attribute__((section(".run_time_ram_pnt")));//

  /* Note: the zero_init forces the linker to place variables in the bss section */
  /* This allows the UNINIT directive (in scatter file) to work. On the contrary */
  /* all Class B variables pairs should be initialized properly by user before using them */

  /* Counter for verifying correct program execution at start */
  extern uint32_t CtrlFlowCnt             __attribute__((section (".class_b_ram")));
  extern uint32_t CtrlFlowCntInv          __attribute__((section (".class_b_ram_rev")));//

  /* Counter for verifying correct program execution in interrupt */
  extern uint32_t ISRCtrlFlowCnt          __attribute__((section(".class_b_ram")));
  extern uint32_t ISRCtrlFlowCntInv       __attribute__((section(".class_b_ram_rev")));//

  /* LSI period measurement at TIM5 IRQHandler */
  extern uint32_t PeriodValue           __attribute__((section(".class_b_ram")));
  extern uint32_t PeriodValueInv        __attribute__((section(".class_b_ram_rev")));//

  /* Sofware time base used in main program (incremented in SysTick timer ISR */
  extern uint32_t TickCounter             __attribute__((section(".class_b_ram")));
  extern uint32_t TickCounterInv          __attribute__((section(".class_b_ram_rev")));//

  /* Indicates to the main routine a 100ms tick */
  extern __IO uint32_t TimeBaseFlag       __attribute__((section(".class_b_ram")));
  extern __IO uint32_t TimeBaseFlagInv    __attribute__((section(".class_b_ram_rev")));//

  /* Indicates to the main routine a 100ms tick */
  extern __IO uint32_t LSIPeriodFlag      __attribute__((section(".class_b_ram")));
  extern __IO uint32_t LSIPeriodFlagInv   __attribute__((section(".class_b_ram_rev")));//

  /* Stores the Control flow counter from one main loop to the other */
  extern uint32_t LastCtrlFlowCnt         __attribute__((section(".class_b_ram")));
  extern uint32_t LastCtrlFlowCntInv      __attribute__((section(".class_b_ram_rev")));//

  /* Pointer to FLASH for crc32 run-time tests */
  extern uint32_t *pRunCrc32Chk           __attribute__((section(".class_b_ram")));
  extern uint32_t *pRunCrc32ChkInv        __attribute__((section(".class_b_ram_rev")));//

/* Reference 32-bit CRC for run-time tests */
  extern uint32_t RefCrc32                __attribute__((section(".class_b_ram")));
  extern uint32_t RefCrc32Inv             __attribute__((section(".class_b_ram_rev")));//

  /* Magic pattern for Stack overflow in this array */
  extern __IO uint32_t aStackOverFlowPtrn[4]   __attribute__((section(".stack_bottom")));
  #else
  /* RAM location for temporary storage of original values at run time RAM transparent test */
  uint32_t aRunTimeRamBuf[RT_RAM_BLOCKSIZE + 2] __attribute__((section(".run_time_ram_buf")));

  /* RAM pointer for run-time tests */
  uint32_t *pRunTimeRamChk        __attribute__((section(".run_time_ram_pnt")));
  uint32_t *pRunTimeRamChkInv     __attribute__((section(".run_time_ram_pnt")));//
  uint32_t aGAP_FOR_RAM_TEST_OVERLAY[2] __attribute__((section(".run_time_ram_pnt")));//

  /* Note: the zero_init forces the linker to place variables in the bss section */
  /* This allows the UNINIT directive (in scatter file) to work. On the contrary */
  /* all Class B variables pairs should be initialized properly by user before using them */

  /* Counter for verifying correct program execution at start */
  uint32_t CtrlFlowCnt             __attribute__((section (".class_b_ram"))) = { 0 };
  uint32_t CtrlFlowCntInv          __attribute__((section (".class_b_ram_rev"))) = { 0 };//

  /* Counter for verifying correct program execution in interrupt */
  uint32_t ISRCtrlFlowCnt          __attribute__((section(".class_b_ram"))) = { 0 };
  uint32_t ISRCtrlFlowCntInv       __attribute__((section(".class_b_ram_rev"))) = { 0 };//

  /* LSI period measurement at TIM5 IRQHandler */
  uint32_t PeriodValue           __attribute__((section(".class_b_ram"))) = { 0 };
  uint32_t PeriodValueInv        __attribute__((section(".class_b_ram_rev"))) = { 0 };//

  /* Sofware time base used in main program (incremented in SysTick timer ISR */
  uint32_t TickCounter             __attribute__((section(".class_b_ram"))) = { 0 };
  uint32_t TickCounterInv          __attribute__((section(".class_b_ram_rev"))) = { 0 };//

  /* Indicates to the main routine a 100ms tick */
  __IO uint32_t TimeBaseFlag       __attribute__((section(".class_b_ram"))) = { 0 };
  __IO uint32_t TimeBaseFlagInv    __attribute__((section(".class_b_ram_rev"))) = { 0 };//

  /* Indicates to the main routine a 100ms tick */
  __IO uint32_t LSIPeriodFlag      __attribute__((section(".class_b_ram"))) = { 0 };
  __IO uint32_t LSIPeriodFlagInv   __attribute__((section(".class_b_ram_rev"))) = { 0 };//

  /* Stores the Control flow counter from one main loop to the other */
  uint32_t LastCtrlFlowCnt         __attribute__((section(".class_b_ram"))) = { 0 };
  uint32_t LastCtrlFlowCntInv      __attribute__((section(".class_b_ram_rev"))) = { 0 };//

  /* Pointer to FLASH for crc32 run-time tests */
  uint32_t *pRunCrc32Chk           __attribute__((section(".class_b_ram"))) = { 0 };
  uint32_t *pRunCrc32ChkInv        __attribute__((section(".class_b_ram_rev"))) = { 0 };//

/* Reference 32-bit CRC for run-time tests */
  uint32_t RefCrc32                __attribute__((section(".class_b_ram"))) = { 0 };
  uint32_t RefCrc32Inv             __attribute__((section(".class_b_ram_rev"))) = { 0 };//

  /* Magic pattern for Stack overflow in this array */
  __IO uint32_t aStackOverFlowPtrn[4]   __attribute__((section(".stack_bottom"))) = { 0 };
  #endif /* ALLOC_GLOBALS */
#endif  /* __GNUC__ */

#endif /* __STL_CLASS_B_VAR_H */

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
