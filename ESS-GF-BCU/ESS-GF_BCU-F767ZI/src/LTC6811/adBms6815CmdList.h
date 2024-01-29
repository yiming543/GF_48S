/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
******************************************************************************
* @file:    adBms6815CmdList.h
* @brief:   Command header file
* @version: $Revision$
* @date:    $Date$
* Developed by: ADIBMS Software team, Bangalore, India
*****************************************************************************/
/*! @addtogroup BMS_DRIVER
*  @{
*
*/

/*! @addtogroup BMS_COMMAND BMS COMMAND
*  @{
*
*/
#pragma once
#include <mbed.h>

//#ifndef __ADBMSCOMMAND_H
//#define __ADBMSCOMMAND_H

//#include "common.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//namespace bmstech
//{
//
////	class ADBMSCOMMAND 
////    {
////		public:

/* Configation registers commands */
static uint8_t WRCFGA[2] = { 0x00, 0x01 };
static uint8_t WRCFGB[2] = { 0x00, 0x24 };
static uint8_t RDCFGA[2] = { 0x00, 0x02 };
static uint8_t RDCFGB[2] = { 0x00, 0x26 };

/* Read cell voltage result registers commands */
static uint8_t RDCVA[2] = { 0x00, 0x04 };
static uint8_t RDCVB[2] = { 0x00, 0x06 };
static uint8_t RDCVC[2] = { 0x00, 0x08 };
static uint8_t RDCVD[2] = { 0x00, 0x0A };
static uint8_t RDCVALL[2] = { 0x00, 0x38 };

/* Read CD voltage result registers commands */
static uint8_t RDCDA[2] = { 0x00, 0x30 };
static uint8_t RDCDB[2] = { 0x00, 0x31 };
static uint8_t RDCDC[2] = { 0x00, 0x32 };
static uint8_t RDCDD[2] = { 0x00, 0x33 };
static uint8_t RDCDALL[2] = { 0x00, 0x3A };

/* Read aux results */
static uint8_t RDAUXA[2] = { 0x00, 0x0C };
static uint8_t RDAUXB[2] = { 0x00, 0x0E };
static uint8_t RDAUXC[2] = { 0x00, 0x0D };

/* Read status registers */
static uint8_t RDSTATA[2] = { 0x00, 0x10 };
static uint8_t RDSTATB[2] = { 0x00, 0x12 };
static uint8_t RDSTATC[2] = { 0x00, 0x13 };
static uint8_t RDASALL[2] = { 0x00, 0x3C };

/* Pwm registers commands */
static uint8_t WRPWM[2] = { 0x00, 0x20 };
static uint8_t RDPWM[2] = { 0x00, 0x22 };

/* Clear commands */
static uint8_t CLRCELL[2] = { 0x07, 0x11 };
static uint8_t CLRAUX[2] = { 0x07, 0x12 };
static uint8_t CLRSTAT[2] = { 0x07, 0x13 };
static uint8_t CLRCD[2] = { 0x07, 0x16 };
static uint8_t CLRFLAG[8] = { 0x07, 0x17 };

/* Poll adc command */
static uint8_t PLADC[2] = { 0x07, 0x14 };

/* Diagn command */
static uint8_t DIAGN[2] = { 0x07, 0x15 };

/* GPIOs Comm commands */
static uint8_t WRCOMM[2] = { 0x07, 0x21 };
static uint8_t RDCOMM[2] = { 0x07, 0x22 };
static uint8_t STCOMM[13] = { 0x07, 0x23, 0xB9, 0xE4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* Mute and Unmute commands */
static uint8_t MUTE[2] = { 0x00, 0x28 };
static uint8_t UNMUTE[2] = { 0x00, 0x29 };

static uint8_t RSTCC[2] = { 0x00, 0x2E };
static uint8_t SRST[2] = { 0x00, 0x1F };

/* Read SID command */
static uint8_t RDSID[2] = { 0x00, 0x2C };

//		// Do nothing
//	    ~ADBMSCOMMAND() {
//			// Do nothing
//		}
//	private:
	
//	};

//} //namespace bmstech
//#endif
/** @}*/
/** @}*/