/**
  ******************************************************************************
  * @file      stm32f4xx_STLcpustartGCC.s
  * @author    MCD Application Team
  * @version   V2.1.0
  * @date      19-Dec-2016 
  * @brief     This file contains the Cortex-M4 CPU tests to be done at start-up. 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  *
  * Copyright (c) 2016 STMicroelectronics International N.V. All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb

  /* Reference to the FailSafe routine to be executed in case of non-recoverable
     failure */
  .extern FailSafePOR

  /* reference to C variables for control flow monitoring */
  .extern CtrlFlowCnt
  .extern CtrlFlowCntInv

  .global  STL_StartUpCPUTest

/**
 ******************************************************************************
 * @brief   Cortex-M4 CPU test during start-up
 *          If possible, branches are 16-bit only in dependence on BL instruction
 *          relative offset
 *          Test jumps directly to a Fail Safe routine in case of failure
 *          All registers are destroyed when exiting this function (including
 *          preserved registers R4 to R11) and excluding stack point R13
 * @param   None
 * @retval : CPUTEST_SUCCESS (=1) if test is ok
*/

  .section  .text.STL_StartUpCPUTest
  .type  STL_StartUpCPUTest, %function
  .size  STL_StartUpCPUTest, .-STL_StartUpCPUTest

STL_StartUpCPUTest:
    MOVS R0, #0x00000000      /* Set Z(ero) Flag */
    BNE.W FailSafePOR         /* Fails if Z clear */
    SUBS R0,#1                /* Set N(egative) Flag */
    BPL.W FailSafePOR         /* Fails if N clear */
    ADDS R0,#2                /* Set C(arry) Flag and do not set Z */
    BCC.W FailSafePOR         /* Fails if C clear */
    MOVS R0, #0x80000000      /* Prepares Overflow test */
    ADDS R0, R0, R0           /* Set V(overflow) Flag */
    BVC.W FailSafePOR         /* Fails if V clear */
    MOVS R0, #0xFFFFFFFF      /* Prepares Saturation test */
    USAT R1,#10,R0            /* Set Q(saturation) Flag */
    MRS R0, APSR              /* Get Status register */
    CMP R0, #0xB8000000       /* Verifies that N=C=V=Q=1 */
    BNE.W FailSafePOR         /* Fails if Q+N+C=V clear */

  /* Register R0 (holds value returned by the function) */
    MOVS R0, #0xAAAAAAAA
    CMP R0, #0xAAAAAAAA
    BNE.W FailSafePOR
    MOVS R0, #0x55555555
    CMP R0, #0x55555555
    BNE.W FailSafePOR

  /* This is for control flow test (ENTRY point)  */
    LDR R0,=CtrlFlowCnt
  /* Assumes R1 OK; If not, error will be detected by R1 test and Ctrl flow test later on  */
    LDR R1,[R0]
    ADDS R1,R1,#0x3	      /* CtrlFlowCnt += OxO3 */
    STR R1,[R0]

  /* Link register (Register R14) */
    MOVS R0, R14              /* Contains the return address and must be saved */
    MOVS R14, #0xAAAAAAAA
    CMP R14, #0xAAAAAAAA
    BNE.W FailSafePOR
    MOVS R14, #0x55555555
    CMP R14, #0x55555555
    BNE.W FailSafePOR
    MOVS R14, R0
    MOVS R0, #0x0             /* For ramp test */

  /* Register R1 */
    MOVS R1, #0xAAAAAAAA
    CMP R1, #0xAAAAAAAA
    BNE.W FailSafePOR
    MOVS R1, #0x55555555
    CMP R1, #0x55555555
    BNE.W FailSafePOR
    MOVS R1, #0x01            /* For ramp test */

  /* Register R2 */
    MOVS R2, #0xAAAAAAAA
    CMP R2, #0xAAAAAAAA
    BNE.W FailSafePOR
    MOVS R2, #0x55555555
    CMP R2, #0x55555555
    BNE.W FailSafePOR
    MOVS R2, #0x02            /* For ramp test */

  /* Register R3 */
    MOVS R3, #0xAAAAAAAA
    CMP R3, #0xAAAAAAAA
    BNE.W FailSafePOR
    MOVS R3, #0x55555555
    CMP R3, #0x55555555
    BNE.W FailSafePOR
    MOVS R3, #0x03            /* For ramp test */

  /* Register R4 */
    MOVS R4, #0xAAAAAAAA
    CMP R4, #0xAAAAAAAA
    BNE.W FailSafePOR
    MOVS R4, #0x55555555
    CMP R4, #0x55555555
    BNE.W FailSafePOR
    MOVS R4, #0x04            /* For ramp test */

  /* Register R5 */
    MOVS R5, #0xAAAAAAAA
    CMP R5, #0xAAAAAAAA
    BNE.W FailSafePOR
    MOVS R5, #0x55555555
    CMP R5, #0x55555555
    BNE.W FailSafePOR
    MOVS R5, #0x05            /* For ramp test */

  /* Register R6 */
    MOVS R6, #0xAAAAAAAA
    CMP R6, #0xAAAAAAAA
    BNE.W FailSafePOR
    MOVS R6, #0x55555555
    CMP R6, #0x55555555
    BNE.W FailSafePOR
    MOVS R6, #0x06            /* For ramp test */

  /* Register R7 */
    MOVS R7, #0xAAAAAAAA
    CMP R7, #0xAAAAAAAA
    BNE.W FailSafePOR
    MOVS R7, #0x55555555
    CMP R7, #0x55555555
    BNE.W FailSafePOR
    MOVS R7, #0x07            /* For ramp test */

  /* Register R8 */
    MOVS R8, #0xAAAAAAAA
    CMP R8, #0xAAAAAAAA
    BNE.W FailSafePOR
    MOVS R8, #0x55555555
    CMP R8, #0x55555555
    BNE.W FailSafePOR
    MOVS R8, #0x08            /* For ramp test */

  /* Register R9 */
    MOVS R9, #0xAAAAAAAA
    CMP R9, #0xAAAAAAAA
    BNE.W FailSafePOR
    MOVS R9, #0x55555555
    CMP R9, #0x55555555
    BNE.W FailSafePOR
    MOVS R9, #0x09            /* For ramp test */

  /* Register R10 */
    MOVS R10, #0xAAAAAAAA
    CMP R10, #0xAAAAAAAA
    BNE FailSafePOR
    MOVS R10, #0x55555555
    CMP R10, #0x55555555
    BNE FailSafePOR
    MOVS R10, #0x0A           /* For ramp test */

  /* Register R11 */
    MOVS R11, #0xAAAAAAAA
    CMP R11, #0xAAAAAAAA
    BNE FailSafePOR
    MOVS R11, #0x55555555
    CMP R11, #0x55555555
    BNE FailSafePOR
    MOVS R11, #0x0B           /* For ramp test */

  /* Register R12 */
    MOVS R12, #0xAAAAAAAA
    CMP R12, #0xAAAAAAAA
    BNE FailSafePOR
    MOVS R12, #0x55555555
    CMP R12, #0x55555555
    BNE FailSafePOR
    MOVS R12, #0x0C           /* For ramp test */

  /* Ramp pattern verification */
    CMP R0, #0x00
    BNE FailSafePOR
    CMP R1, #0x01
    BNE FailSafePOR
    CMP R2, #0x02
    BNE FailSafePOR
    CMP R3, #0x03
    BNE FailSafePOR
    CMP R4, #0x04
    BNE FailSafePOR
    CMP R5, #0x05
    BNE FailSafePOR
    CMP R6, #0x06
    BNE FailSafePOR
    CMP R7, #0x07
    BNE FailSafePOR
    CMP R8, #0x08
    BNE FailSafePOR
    CMP R9, #0x09
    BNE FailSafePOR
    CMP R10, #0x0A
    BNE FailSafePOR
    CMP R11, #0x0B
    BNE FailSafePOR
    CMP R12, #0x0C
    BNE FailSafePOR

  /* Process Stack pointer (banked Register R13) */
    MRS R0, PSP         /* Save process stack value */
    MOVW R1, #0xAAA8     /* Test is different (PSP is word aligned) */
    MOVT R1, #0xAAAA    /* Load in two times and 2 least significant bits cleared */
    MSR PSP, R1         /* load process stack value */
    MRS R2, PSP         /* Get back process stack value */
    CMP R2, R1          /* Verify value */
    BNE FailSafePOR
    MOVW R1, #0x5554     /* Test is different (PSP is word aligned) */
    MOVT R1, #0x5555    /* Load in two times and 2 least significant bits cleared */
    MSR PSP, R1         /* load process stack value */
    MRS R2, PSP         /* Get back process stack value */
    CMP R2, R1          /* Verify value */
    BNE FailSafePOR
    MSR PSP, R0         /* Restore process stack value */

  /* Stack pointer (Register R13) */
    MRS R0, MSP         /* Save process stack value */
    MOVW R1, #0xAAA8     /* Test is different (MSP is word aligned) */
    MOVT R1, #0xAAAA    /* Load in two times and 2 least significant bits cleared */
    MSR MSP, R1         /* load process stack value */
    MRS R2, MSP         /* Get back process stack value */
    CMP R2, R1          /* Verify value */
    BNE FailSafePOR
    MOVW R1, #0x5554     /* Test is different (MSP is word aligned) */
    MOVT R1, #0x5555    /* Load in two times and 2 least significant bits cleared */
    MSR MSP, R1         /* load process stack value */
    MRS R2, MSP         /* Get back process stack value */
    CMP R2, R1          /* Verify value */
    BNE FailSafePOR
    MSR MSP, R0         /* Restore process stack value */
	
  /* Control flow test (EXIT point) */
    LDR R0,=CtrlFlowCntInv
    LDR R1,[R0]
    SUBS R1,R1,#0x3	/* CtrlFlowCntInv -= OxO3 */
    STR R1,[R0]

  /* If next instruction is not executed, R0 will hold a value different from 0 */
    MOVS R0, #0x1       /* CPUTEST_SUCCESS */
    BX LR               /* return to the caller */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
