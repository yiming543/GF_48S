//File
#pragma once

#include <mbed.h>

#include "PackConst.h"


	
namespace bmstech
{
#ifdef __cplusplus
	extern "C"
	{
#endif
#define	USING_SAE_J1850_CRC8

#define ZERO_CURRENT_THRESHOULD (1)

#define ZERO_THRESHOULD 2		//200mA
#define ZERO_THRESHOULD_MA (50)		//50mA
	
#define CHARGE_ZERO_THRESHOULD_MA (50)		//50mA (mA�����)
#define DISCHARGE_ZERO_THRESHOULD_MA (-50)		//-50mA (mA�����)

#define HALF_BANK_PACK_NUM	(10)	//10 packs for lower half bank

#define DEFAULT_BCU_ID (1)	//valid 1~15

				//BCU ID Address in DS1302:
				//=====================================================
				//#define BCU_ID_DS1302_ADDRESS	(0)
				//================================================================================
				//DS1302: 31 x 8 Battery-Backed General-Purpose RAM
				//Note m_remainCapacity & m_fullChargeCapacity => store to non-valitile memory
				//memorize m_designCapacity too
				//=> m_dsgAccum = m_fullChargeCapacity - m_remainCapacity;
				//
				//SOC/SOH calculation :
				//float soc_f = (float)m_remainCapacity / (float)m_fullChargeCapacity;
				//float soh_f = (float)m_fullChargeCapacity / (float)m_designCapacity;
				//================================================================================
				//BCU ID Address in DS1302:
				//=====================================================
#if defined(DS1302_SOC_STORAGE)
#define DS1302_MAGIC_WORD	(0x12345678)
#define BCU_ID_DS1302_ADDRESS	(0)
#define BCU_REMAIN_CAPACITY_BASE_ADDRESS		(1)		//Uint16 HB|LB (HB first) (Addr 1~2: HB,LB)
#define BCU_FULL_CHARGE_CAPACITY_BASE_ADDRESS	(3)		//Uint16 HB|LB (HB first) (Addr 3~4: HB,LB)
#define BCU_DESIGN_CAPACITY_BASE_ADDRESS		(5)		//Uint16 HB|LB (HB first) (Addr 5~6: HB,LB): 1000 =>100.0Ah
#define BCU_IS_DEFAULT_ADDRESS					(7)	   // Uint32 0x123456 (Addr 7~10)

#elif defined(STM32F4_RTC_SOC_STORAGE)
				//================================================================================
				//BCU ID Address in STM32F4 RTC Registers: UInt32 x 20
				//================================================================================
#define RTC_MAGIC_WORD	(0x12345678)
#define BCU_IS_DEFAULT_RTC_REG_ADDRESS	         (RTC_BKP_DR0)	//Magic Word: 0x12345678
#define BCU_ID_RTC_REG_ADDRESS	                 (RTC_BKP_DR1)  //BCU ID: 0x01~0xFF (0x00: broadcasting ID)
#define BCU_REMAIN_CAPACITY_RTC_REG_ADDRESS		 (RTC_BKP_DR2)  //Uint16 stored as uint32 (uint: 0.1Ah)
#define BCU_FULL_CHARGE_CAPACITY_RTC_REG_ADDRESS (RTC_BKP_DR3)	//Uint16 stored as uint32 (uint: 0.1Ah)
#define BCU_DESIGN_CAPACITY_RTC_REG_ADDRESS		 (RTC_BKP_DR4)	//Uint16 stored as uint32 (uint: 0.1Ah): 1000 =>100.0Ah
#endif

				//BCU FSM Behaviour
				//==================================================
				//TODO: eferry disable auto jump state 2017/11/7
				//wait for can command to switch

				//#define AUTO_JUMP_TO_PASSIVE
				//#define AUTO_JUMP_TO_AUTO

#define FAULT_AUTO_RECOVERY		//BCU automatically recover from fault state
#define OVP_AUTO_RECOVERY		//to standby
#define UVP_AUTO_RECOVERY		//to standby
#define DOCP_AUTO_RECOVERY		//to standby
#define COCP_AUTO_RECOVERY		//to standby

#define HVIL_AUTO_RECOVERY		//to standby
#define IVP_AUTO_RECOVERY		//to standby
#define ITP_AUTO_RECOVERY		//to standby
#define OGP_AUTO_RECOVERY		//to standby

#define SEC_BCU_TICK		(1)					//0.5sec BCU FSM tick
#define	INIT_WAIT_TIME		(10*SEC_BCU_TICK)	//wait for CM App to run (9*SEC_BCU_TICK)
							

#define SECOND_1			(1*SEC_BCU_TICK)	// 1 second
#define MINUTE_1			(60*SEC_BCU_TICK)	// 1 minute

#define AUTOWAIT_DELAY		(1*SEC_BCU_TICK)	// delay time to turn on precharge relay

#define PRECHARGE_DELAY		(1*SEC_BCU_TICK)	// delay time to turn on precharge relay
#define PRECHG_TIME			(3*SEC_BCU_TICK)	// Precharge Relay ON time (Duration
#define QUICK_PRECHG_TIME	(1*SEC_BCU_TICK)	// Precharge Relay ON time (Duration

																//BMS Protection Setting:

//#define TEMP_CHECK_TIME		(3*SEC_BCU_TICK)
//#define OCP_CHECK_TIME		(15*SEC_BCU_TICK)		//Over current time threshould 10 sec
//#define DRIVE_CHECK_TIME	(3*SEC_BCU_TICK)
//#define REVERSE_CHECK_TIME	(10*SEC_BCU_TICK)		//Reverse current time threshould 10 sec
//#define SBYOCP_CHECK_TIME	(10*SEC_BCU_TICK)		//Standby Over current time threshould 10 sec
#define CAN_PACKET_CHECK_TIMEOUT (3*SEC_BCU_TICK)	//CM CAN Packet LOSS check timeout (5) sec [12/11:changed to 25 sec]

#define	PASSIVE_TIME_KEEP	(60*SEC_BCU_TICK)	//10 minute (1200 ticks)

#define TIME_1SEC			(1*SEC_BCU_TICK)	//2
#define TIME_1MIN			(60*SEC_BCU_TICK)	//120

#define RECOVERY_DELAY	(1*SEC_BCU_TICK)		//(3*SEC_BCU_TICK) JY

#define	DISCHARGE_AMP_THRESHOULD	(10)		//unit:0.1A => 1000mA
#define	CHARGE_AMP_THRESHOULD		(-10)		//unit:0.1A => 1000mA: for bi-derectional sensor
																//#define	CHARGE_AMP_THRESHOULD		(10)		//unit:0.1A => 1000mA

#define COULOMB_CONVERSION			(3600L)	 //0.1A @0.5 SEC to 0.1Ah conversion 0.1 Ah = 360 ASEC / 0.05ASEC ==> 7200L
#define COULOMB_CONVERSION_MA		(360000L)	//0.001A @1.0 SEC to 0.1Ah conversion, 0.1Ah = 360 ASEC / 0.001ASEC => 360000
////0.001A @10ms to 0.1Ah conversion, 0.1Ah = 360 ASEC / 0.1ASEC => 3600
	
#define EVENT_LOG_LENGTH	(32)
										
//=============================================================
#define DEFAULT_PACK_SOC		((uint16_t)80)		// 80%
#define DEFAULT_PACK_SOH		((uint16_t)100)		// 100%

#define DEFAULT_PACK_RMC		((uint16_t)1700)	// 88.0 Ah, => SOC=RMC/FCC = 80%
#define DEFAULT_PACK_FCC		((uint16_t)1700)	// 190.0 Ah, => SOH=FCC/DC  = 100%	//20230830 190-->170Ah
#define DEFAULT_PACK_DC			((uint16_t)1700)	// 90.0 Ah //(95% of nominal Ah)

#define DEFAULT_PACK_CYCLE		((uint16_t)3)		// 3 times

//=========================================================================================
//BCU Protection parameter valid data range:
//=========================================================================================
//==============================================================================
//OV1 Check Function
//max_vol >= 3.5V and continue 5sec , OV1 event occure
//max_vol <= 3.4V and continue 5sec , OV1 event recover
//==============================================================================
#define OV1_CHECK (3500)	//1mV
#define OV1_RECOVER (3400)	//1mV
#define OV1_CHECK_TIME (10)	//0.5sec
#define OV1_RECOVER_TIME (10)	//0.5sec

//==============================================================================
//OV2 Check Function
//max_vol >= 3.6V and continue 3sec , OV2 event occure
//max_vol <= 3.5V and continue 5sec , OV2 event recover
//==============================================================================
#define OV2_CHECK (3600)	//1mV
#define OV2_RECOVER (3500)	//1mV
#define OV2_CHECK_TIME (6)	//0.5sec
#define OV2_RECOVER_TIME (10)	//0.5sec
	
//==============================================================================
//OV3 Check Function
//max_vol >= 3.65V and continue 1sec , OV3 event occure
//System Reset or Clear Flag, OV3 event recover
//==============================================================================
#define OV3_CHECK (3650)	//1mV
#define OV3_RECOVER (3500)	//1mV
#define OV3_CHECK_TIME (2)	//0.5sec
#define OV3_RECOVER_TIME (10)	//0.5sec

//==============================================================================
//UV1 Check Function
//min_vol <= 2.8V and continue 5sec , UV1 event occure
//min_vol >= 2.9V and continue 5sec , UV1 event recover
//==============================================================================
#define UV1_CHECK (2800)	//1mV
#define UV1_RECOVER (2900)	//1mV
#define UV1_CHECK_TIME (10)	//0.5sec
#define UV1_RECOVER_TIME (10)	//0.5sec

//==============================================================================
//UV2 Check Function
//min_vol <= 2.7V and continue 3sec , UV2 event occure
//min_vol >= 2.8V and continue 5sec , UV2 event recover
//==============================================================================
#define UV2_CHECK (2700)	//1mV  
#define UV2_RECOVER (2800)	//1mV
#define UV2_CHECK_TIME (6)	//0.5sec
#define UV2_RECOVER_TIME (10)	//0.5sec

//==============================================================================
//UV3 Check Function
//min_vol <= 2.5V and continue 1sec , UV3 event occure
//System Reset or Clear Flag , UV3 event recover
//==============================================================================
#define UV3_CHECK (2500)	//1mV  
#define UV3_RECOVER (2800)	//1mV
#define UV3_CHECK_TIME (2)	//0.5sec
#define UV3_RECOVER_TIME (10)	//0.5sec

//==============================================================================
//OTC1 Check Function- Charge
//max_temp >= 52'c and continue 5sec,OTC1 event occure
//max_temp <  47'c and continue 5sec,OTC1 event recover
//==============================================================================
#define OTC1_CHECK (520)	//0.1'c
#define OTC1_RECOVER (470)	//0.1'c
#define OTC1_CHECK_TIME (10)	//0.5sec
#define OTC1_RECOVER_TIME (10)	//0.5sec

//==============================================================================
//OTC2 Check Function- Charge
//max_temp >= 57'c and continue 3sec,OTC2 event occure
//max_temp <  52'c and continue 5sec,OTC2 event recover
//==============================================================================
#define OTC2_CHECK (570)	//0.1'c
#define OTC2_RECOVER (520)	//0.1'c
#define OTC2_CHECK_TIME (6)	//0.5sec
#define OTC2_RECOVER_TIME (10)	//0.5sec

//==============================================================================
//OTC3 Check Function- Charge
//max_temp >= 62'c and continue 1sec,OTC3 event occure
//System Reset or Clear Flag, OTC3 event recover
//==============================================================================
#define OTC3_CHECK (620)	//0.1'c
#define OTC3_RECOVER (500)	//0.1'c
#define OTC3_CHECK_TIME (2)	//0.5sec
#define OTC3_RECOVER_TIME (10)	//0.5sec

//==============================================================================
//OTD1 Check Function- Discharge
//max_temp >= 52'c and continue 5sec,OTD1 event occure
//max_temp <  47'c and continue 5sec,OTD1 event recover
//==============================================================================
#define OTD1_CHECK (520)	//0.1'c
#define OTD1_RECOVER (470)	//0.1'c
#define OTD1_CHECK_TIME (10)	//0.1sec
#define OTD1_RECOVER_TIME (10)	//0.1sec

//==============================================================================
//OTD2 Check Function- Discharge
//max_temp >= 57'c and continue 3sec,OTD2 event occure
//max_temp <  52'c and continue 5sec,OTD2 event recover
//==============================================================================
#define OTD2_CHECK (570)	//0.1'c
#define OTD2_RECOVER (520)	//0.1'c
#define OTD2_CHECK_TIME (6)	//0.1sec
#define OTD2_RECOVER_TIME (10)	//0.1sec

//==============================================================================
//OTD3 Check Function- Discharge
//max_temp >= 62'c and continue 1sec,OTD3 event occure
//System Reset or Clear Flag, OTD3 event recover
//==============================================================================
#define OTD3_CHECK (620)	//0.1'c
#define OTD3_RECOVER (500)	//0.1'c
#define OTD3_CHECK_TIME (2)	//0.5sec
#define OTD3_RECOVER_TIME (10)	//0.5sec
	
//==============================================================================
//UTC1 Check Function - Charge
//min_temp <= 5'c and continue 5sec,UTC1 event occure
//min_temp >  10'c and continue 5sec,UTC1 event recover
//==============================================================================
#define UTC1_CHECK (50)	//0.1'c
#define UTC1_RECOVER (100)	//0.1'c
#define UTC1_CHECK_TIME (10)	//0.5sec
#define UTC1_RECOVER_TIME (10)	//0.5sec
#define CHARGE_CURRENT_THRESHOULD (-1)
	
//==============================================================================
//UTC2 Check Function - Charge
//min_temp <= 0'c and continue 3sec,UTC2 event occure
//min_temp >  5'c and continue 5sec,UTC2 event recover
//==============================================================================
#define UTC2_CHECK (0)	//0.1'c
#define UTC2_RECOVER (50)	//0.1'c
#define UTC2_CHECK_TIME (6)	//0.5sec
#define UTC2_RECOVER_TIME (10)	//0.5sec

//==============================================================================
//UTC3 Check Function - Charge
//min_temp <= -5'c and continue 1sec,UTC3 event occure
//System Reset or Clear Flag, UTC3 event recover
//==============================================================================
#define UTC3_CHECK (-50)	//0.1'c	
#define UTC3_RECOVER (50)	//0.1'c
#define UTC3_CHECK_TIME (2)	//sec
#define UTC3_RECOVER_TIME (10)	//sec

//==============================================================================
//UTD1 Check Function - Discharge
//min_temp <= -10'c and continue 5sec,UTD1 event occure
//min_temp >  0'c and continue 5sec,UTD1 event recover
//==============================================================================
#define UTD1_CHECK (-100)	//0.1'c		
#define UTD1_RECOVER (0)	//0.1'c
#define UTD1_CHECK_TIME (10)	//0.1sec
#define UTD1_RECOVER_TIME (10)	//0.1sec
#define DISCHARGE_CURRENT_THRESHOULD (1)
	
//==============================================================================
//UTD2 Check Function - Discharge
//min_temp <= -20'c and continue 3sec,UTD2 event occure
//min_temp >  -15'c and continue 5sec,UTD2 event recover
//==============================================================================
#define UTD2_CHECK (-200)	//0.1'c
#define UTD2_RECOVER (-150)	//0.1'c
#define UTD2_CHECK_TIME (6)	//0.1sec
#define UTD2_RECOVER_TIME (10)	//0.1sec
	
//==============================================================================
//UTD3 Check Function - Discharge
//min_temp <= -25'c and continue 1sec, UTD3 event occure
//System Reset or Clear Flag, UTD3 event recover
//==============================================================================
#define UTD3_CHECK (-250)	//0.1'c
#define UTD3_RECOVER (-150)	//0.1'c
#define UTD3_CHECK_TIME (2)	//0.1sec
#define UTD3_RECOVER_TIME (10)	//0.1sec
	
//==============================================================================
//OCC1 Charge Check Function
//Charge Current >= 180 A and continue 5 sec  , OCC1  event occure
//Charge Current <  170 A and continue 5 sec , OCC1  event recover
//==============================================================================
#define OCC1_CHECK (-1800)	//0.1A
#define OCC1_RECOVER (-1700)	//0.1A
#define OCC1_CHECK_TIME (10)	//0.5sec
#define OCC1_RECOVER_TIME (10)	//0.5sec
	
//==============================================================================
//OCC2 Charge Check Function
//Charge Current >= 190 A and continue 3 sec, OCC2  event occure
//Charge Current <  1 A and continue 30 sec, OCC2  event recover
//==============================================================================
#define OCC2_CHECK (-1900)	//0.1A
#define OCC2_RECOVER (-10)	//0.1A
#define OCC2_CHECK_TIME (6)	//0.5sec
#define OCC2_RECOVER_TIME (60)	//sec
	
//==============================================================================
//OCC3 Charge Check Function
//Charge Current >= 195 A and continue 1 sec, OCC3  event occure
//System Reset or Clear Flag, OCC3  event recover
//==============================================================================
#define OCC3_CHECK (-1950)	//0.1A
#define OCC3_RECOVER (-10)	//0.1A
#define OCC3_CHECK_TIME (2)	//0.5sec
#define OCC3_RECOVER_TIME (60)	//0.5sec
	
//==============================================================================
//OCD1 Discharge Check Function
//Discharge Current >= 180 A and continue 5 sec  , OCD1  event occure
//Discharge Current < 170 A and continue 5 sec , OCD1  event recover
//==============================================================================
#define OCD1_CHECK (1800)	//0.1A
#define OCD1_RECOVER (1700)	//0.1A
#define OCD1_CHECK_TIME (10)	//0.5sec
#define OCD1_RECOVER_TIME (10)	//0.5sec
	
//==============================================================================
//OCD2 Discharge Check Function
//Discharge Current >= 190 A and continue 3 sec  , OCD2  event occure
//Discharge Current <  1 A and continue 30 sec , OCD2  event recover
//==============================================================================
#define OCD2_CHECK (1900)	//0.1A
#define OCD2_RECOVER (10)	//0.1A
#define OCD2_CHECK_TIME (6) //(10*60*2)	//0.5sec
#define OCD2_RECOVER_TIME (60)	//0.5sec

//==============================================================================
//OCD3 Discharge Check Function
//Discharge Current >= 195 A and continue 1 sec, OCD3  event occure
//System Reset or Clear Flag, OCD3  event recover
//==============================================================================
#define OCD3_CHECK (1950)	//0.1A
#define OCD3_RECOVER (10)	//0.1A
#define OCD3_CHECK_TIME (2)//(60)	//0.5sec
#define OCD3_RECOVER_TIME (60)	//0.5sec
		
//==============================================================================
//HVIL1 Emergency Check Function
//HVIL1_Emergency Disconnect and continue 500ms , hvil event occure
//HVIL1_Emergency Connect and continue 1000ms , hvil event recover
//==============================================================================
#define HVIL1_CHECK_TIME (500)	//ms
#define HVIL1_RECOVER_TIME (1000)	//ms	

//==============================================================================
//HVIL2 Check Function
//HVIL2_Emergency Disconnect and continue 500ms , hvi2 event occure
//HVIL2_Emergency Connect and continue 1000ms , hvi2 event recover
//==============================================================================
#define HVIL2_CHECK_TIME (500)	//ms
#define HVIL2_RECOVER_TIME (1000)	//ms	
		
//==============================================================================
//IV1 Check Function
//Imbalance Voltage >= 150mV and continue 10sec , IV1 event occure
//Imbalance Voltage < 50mV and continue 10sec , IV1 event recover
//==============================================================================
#define IV1_CHECK (150)	//mV
#define IV1_RECOVER (50)	//mV
#define IV1_CHECK_TIME (20)	//sec
#define IV1_RECOVER_TIME (20)	//sec
	
//==============================================================================
//Current_STATUS_CAN_FAULT Check Function
//==============================================================================
#define CSCF_CHECK_TIME (6)	//sec
#define CSCF_RECOVER_TIME (6)	//sec	
	
//==============================================================================
//Ems_STATUS_CAN_FAULT Check Function
//==============================================================================
#define ESCF_CHECK_TIME (10)	//sec
#define ESCF_RECOVER_TIME (6)	//sec	
	

//==============================================================================
//Charge Enable Check Function
//max_vol >= 3.55V  and continue 3sec , Charge Disable event occure
//max_vol <= 3.36V  and continue 3sec , Charge Enable event occure
//==============================================================================
#define CHG_E_CHECK (3360)		//1mV
#define CHG_E_RECOVER (3550)	//1mV
#define CHG_E_CHECK_TIME (6)	//0.5sec
#define CHG_E_RECOVER_TIME (6)	//0.5sec	
	
//==============================================================================
//Discharge Enable Check Function
//MinVolt <= 2.8V  and continue 3sec , Discharge Disable event occure
//MinVolt >= 3.2V  and continue 3sec , Discharge Enable event occure
//==============================================================================
#define DSG_E_CHECK (3200)		//1mV
#define DSG_E_RECOVER (2800)	//1mV
#define DSG_E_CHECK_TIME (6)	//0.5sec
#define DSG_E_RECOVER_TIME (6)	//0.5sec	
	
//==============================================================================
//Full Charge Check Function
//MinVolt >= 3.480V  and MaxVolt-MinVolt < 40mV and Charge Current <= 1A and continue 3sec , Full Charge event occure
// X SOC <= 99.9%  and continue 3sec , Full Charge event recover
//MaxVolt <= 3.36V and continue 3sec , Full Charge event recover		
//==============================================================================
#define FULLCHG_MINVOLT_CHECK (3480)	//1mV
#define FULLCHG_A_CHECK (-10)	//0.1A 
#define FULLCHG_VOLTAGE_DIFFERENCE	(40)	//1mV
//#define FULLCHG_SOC_RECOVER (999)	//0.1%
#define FULLCHG_SOC_RECOVER (3360)	//20230531 JY
#define FULLCHG_CHECK_TIME (6)	//0.5sec
#define FULLCHG_RECOVER_TIME (6)	//0.5sec	
	
//==============================================================================
//Full Discharge Check Function
//MinVolt <= 2.8V  and continue 3sec , Full Discharge event occure
//SOC >= 5%  and MinVolt > 3.000V and continue 3sec , Full Discharge event recover
//==============================================================================
#define FULLDSG_V_CHECK (2800)	//1mV
#define FULLDSG_V_RECOVER (3000)	//1mV
//#define FullDSG_A_Check (100)	//0.1A 
#define FULLDSG_SOC_RECOVER (50)	//0.1%
#define FULLDSG_CHECK_TIME (6)	//0.5sec
#define FULLDSG_RECOVER_TIME (6)	//0.5sec	

		
//==============================================================================
//AFE BMU ADC Warning Check Function
//BMU ADC CHECK TEMP > 26'c and ADC CHECK TEMP < 24'c and continue 10sec , BMU_ADC_Warning event occure
//BMU ADC CHECK TEMP <= 26'c and ADC CHECK TEMP >= 24'c and continue 10sec , BMU_ADC_Warning event recover	
//==============================================================================
#define ADCWARNING_CHECK_UPPERTEMP (260)
#define ADCWARNING_CHECK_LOWERTEMP (240)
#define ADCWARNING_CHECK_TIME (20)		//0.5sec
#define ADCWARNING_RECOVER_TIME (20)	//0.5sec	

		
//==============================================================================
//AFE BMU ADC Fault Check Function
//BMU ADC CHECK TEMP > 26.5'c and ADC CHECK TEMP < 23.5'c and continue 5sec , BMU_ADC_FAULT event occure
//All ADCFault == false  and continue 10sec , BMU_ADC_FAULT event recover	
//==============================================================================
#define ADCFAULT_CHECK_UPPERTEMP (265)
#define ADCFAULT_CHECK_LOWERTEMP (235)
#define ADCFAULT_CHECK_TIME (10)		//0.5sec
#define ADCFAULT_RECOVER_TIME (20)	//0.5sec	
#define CUMULATIVE_COUNT (3)
			
//==============================================================================
//OT1_PCS_Positive Check Function
//PCS_Positive_temp >= 85'c or PCS_Positive_temp <= 0'c and continue 10sec,OT1_PCS_Positive  event occure
//PCS_Positive_temp <  75'c and PCS_Positive_temp > 5'c and continue 10sec,OT1_PCS_Positive  event recover
//==============================================================================		
#define OT1_PCS_POSITIVE_CHECK_TEMP (500)
#define OT1_PCS_POSITIVE_RECOVER_TEMP (400)
#define OT1_PCS_POSITIVE_CHECK_LOWTEMP (0)
#define OT1_PCS_POSITIVE_RECOVER_LOWTEMP (50)
#define OT1_PCS_POSITIVE_CHECK_TIME (20)		//0.5sec
#define OT1_PCS_POSITIVE_RECOVER_TIME (20)	//0.5sec		
	
//==============================================================================
//OT2_PCS_Positive Check Function
//PCS_Positive_temp >= 100'c and continue 10sec,OT2_PCS_Positive  event occure
//System Reset,OT2_PCS_Positive  event recover
//==============================================================================		
#define OT2_PCS_POSITIVE_CHECK_TEMP (1000)
#define OT2_PCS_POSITIVE_CHECK_TIME (20)		//0.5sec
#define OT2_PCS_POSITIVE_RECOVER_TIME (20)	//0.5sec	
		
//==============================================================================
//OT1_PCS_Negative Check Function
//PCS_Negative_temp >= 85'c or PCS_Negative_temp <= 0'c and continue 10sec,OT1_PCS_Negative  event occure
//PCS_Negative_temp <  75'c and PCS_Negative_temp > 5'c and continue 10sec,OT1_PCS_Negative  event recover
//==============================================================================
#define OT1_PCS_NEGATIVE_CHECK_TEMP (500)
#define OT1_PCS_NEGATIVE_RECOVER_TEMP (400)
#define OT1_PCS_NEGATIVE_CHECK_LOWTEMP (0)
#define OT1_PCS_NEGATIVE_RECOVER_LOWTEMP (50)
#define OT1_PCS_NEGATIVE_CHECK_TIME (20)		//0.5sec
#define OT1_PCS_NEGATIVE_RECOVER_TIME (20)	//0.5sec	
		
//==============================================================================
//OT2_PCS_Negative Check Function
//PCS_Negative_temp >= 100'c and continue 5sec,OT2_PCS_Negative  event occure
//System Reset,OT2_PCS_Negative  event recover
//==============================================================================
#define OT2_PCS_NEGATIVE_CHECK_TEMP (1000)
#define OT2_PCS_NEGATIVE_CHECK_TIME (20)		//0.5sec
#define OT2_PCS_NEGATIVE_RECOVER_TIME (20)	//0.5sec	

//==============================================================================
//OT1_HVDC_Positive Check Function
//HVDC_Positive_temp >= 85'c or HVDC_Positive_temp <= 0'c and continue 10sec,OT1_HVDC_Positive  event occure
//HVDC_Positive_temp <  75'c and HVDC_Positive_temp > 5'c and continue 10sec,OT1_HVDC_Positive  event recover
//==============================================================================		
#define OT1_HVDC_POSITIVE_CHECK_TEMP (500)
#define OT1_HVDC_POSITIVE_RECOVER_TEMP (400)
#define OT1_HVDC_POSITIVE_CHECK_LOWTEMP (0)
#define OT1_HVDC_POSITIVE_RECOVER_LOWTEMP (50)
#define OT1_HVDC_POSITIVE_CHECK_TIME (20)		//0.5sec
#define OT1_HVDC_POSITIVE_RECOVER_TIME (20)	//0.5sec			
	
//==============================================================================
//OT2_HVDC_Positive Check Function
//HVDC_Positive_temp >= 100'c and continue 5sec,OT2_HVDC_Positive  event occure
//System Reset,HVDC_Positive_temp  event recover
//==============================================================================		
#define OT2_HVDC_POSITIVE_CHECK_TEMP (1000)
#define OT2_HVDC_POSITIVE_CHECK_TIME (20)		//0.5sec
#define OT2_HVDC_POSITIVE_RECOVER_TIME (20)	//0.5sec	
		
//==============================================================================
//OT1_HVDC_Negative Check Function
//HVDC_Negative_temp >= 85'c or HVDC_Negative_temp <= 0'c and continue 10sec,OT1_HVDC_Negative  event occure
//HVDC_Negative_temp <  75'c and HVDC_Negative_temp > 5'c and continue 10sec,OT1_HVDC_Negative  event recover
//==============================================================================
#define OT1_HVDC_NEGATIVE_CHECK_TEMP (500)
#define OT1_HVDC_NEGATIVE_RECOVER_TEMP (400)
#define OT1_HVDC_NEGATIVE_CHECK_LOWTEMP (0)
#define OT1_HVDC_NEGATIVE_RECOVER_LOWTEMP (50)
#define OT1_HVDC_NEGATIVE_CHECK_TIME (10)		//0.5sec
#define OT1_HVDC_NEGATIVE_RECOVER_TIME (20)	//0.5sec	
		
//==============================================================================
//OT2_HVDC_Negative Check Function
//HVDC_Negative_temp >= 100'c and continue 10sec,OT2_HVDC_Negative  event occure
//System Reset,HVDC_Negative_temp  event recover
//==============================================================================
#define OT2_HVDC_NEGATIVE_CHECK_TEMP (1000)
#define OT2_HVDC_NEGATIVE_CHECK_TIME (20)		//0.5sec
#define OT2_HVDC_NEGATIVE_RECOVER_TIME (20)	//0.5sec	
		
//==============================================================================
//OT1_Fuse Check Function
//Fuse_temp >= 85'c or Fuse_temp <= 0'c and continue 10sec,OT1_Fuse  event occure
//Fuse_temp <  75'c and Fuse_temp > 5'c and continue 10sec,OT1_Fuse  event recover
//==============================================================================		
#define OT1_FUSE_CHECK_TEMP (500)
#define OT1_FUSE_RECOVER_TEMP (400)
#define OT1_FUSE_CHECK_LOWTEMP (0)
#define OT1_FUSE_RECOVER_LOWTEMP (50)
#define OT1_FUSE_CHECK_TIME (10)		//0.5sec
#define OT1_FUSE_RECOVER_TIME (20)	//0.5sec			
		
//==============================================================================
//OT2_Fuse Check Function
//Fuse_temp >= 100'c and continue 10sec,OT2_Fuse  event occure
//System Reset,OT2_Fuse  event recover
//==============================================================================		
#define OT2_FUSE_CHECK_TEMP (1000)
#define OT2_FUSE_CHECK_TIME (20)		//0.5sec
#define OT2_FUSE_RECOVER_TIME (20)	//0.5sec				
		
//==============================================================================
//OT1_TA Check Function
//TA_temp >= 85'c or TA_temp <= 0'c and continue 10sec,OT1_TA  event occure
//TA_temp <  75'c and TA_temp > 5'c and continue 10sec,OT1_TA  event recover
//==============================================================================		
#define OT1_TA_CHECK_TEMP (500)
#define OT1_TA_RECOVER_TEMP (400)
#define OT1_TA_CHECK_LOWTEMP (0)
#define OT1_TA_RECOVER_LOWTEMP (50)
#define OT1_TA_CHECK_TIME (10)		//0.5sec
#define OT1_TA_RECOVER_TIME (20)	//0.5sec		
				
//==============================================================================
//Charging state COAT1_Over Ambient Temperature Check Function
//Pack Ambient_temp >= 35'c and continue 5sec,COAT1_PackAmbient  event occure
//Pack Ambient_temp <  30'c and continue 5sec,COAT1_PackAmbient  event recover
//==============================================================================		
#define COAT1_PACKAMBIENT_CHECK_TEMP (350)
#define COAT1_PACKAMBIENT_RECOVER_TEMP (300)
#define COAT1_PACKAMBIENT_CHECK_TIME (10)		//0.5sec
#define COAT1_PACKAMBIENT_RECOVER_TIME (10)		//0.5sec		
		
//==============================================================================
//Charging state COAT3_Over Ambient Temperature Check Function
//Pack Ambient_temp >= 40'c and continue 3sec, COAT3_PackAmbient  event occure
//System Reset or Clear Flag, COAT3_PackAmbient  event recover
//==============================================================================		
#define COAT3_PACKAMBIENT_CHECK_TEMP (400)
#define COAT3_PACKAMBIENT_RECOVER_TEMP (300)
#define COAT3_PACKAMBIENT_CHECK_TIME (6)		//0.5sec
#define COAT3_PACKAMBIENT_RECOVER_TIME (10)		//0.5sec		
		
//==============================================================================
//Charging state CUAT1_Under Ambient Temperature Check Function
//Pack Ambient_temp <= 8'c and continue 5sec,CUAT1_PackAmbient  event occure
//Pack Ambient_temp >  13'c and continue 5sec,CUAT1_PackAmbient  event recover
//==============================================================================		
#define CUAT1_PACKAMBIENT_CHECK_TEMP (80)
#define CUAT1_PACKAMBIENT_RECOVER_TEMP (130)
#define CUAT1_PACKAMBIENT_CHECK_TIME (10)		//0.5sec
#define CUAT1_PACKAMBIENT_RECOVER_TIME (10)		//0.5sec		
		
//==============================================================================
//Charging state CUAT3_Under Ambient Temperature Check Function
//Pack Ambient_temp <= 3'c and continue 3sec, CUAT3_PackAmbient  event occure
//System Reset or Clear Flag, COAT3_PackAmbient  event recover
//==============================================================================		
#define CUAT3_PACKAMBIENT_CHECK_TEMP (30)
#define CUAT3_PACKAMBIENT_RECOVER_TEMP (130)
#define CUAT3_PACKAMBIENT_CHECK_TIME (6)		//0.5sec
#define CUAT3_PACKAMBIENT_RECOVER_TIME (10)		//0.5sec	
		
//==============================================================================
//Discharging state DOAT1_Over Ambient Temperature Check Function
//Pack Ambient_temp >= 45'c and continue 5sec,DOAT1_PackAmbient  event occure
//Pack Ambient_temp <  40'c and continue 5sec,DOAT1_PackAmbient  event recover
//==============================================================================			
#define DOAT1_PACKAMBIENT_CHECK_TEMP (450)
#define DOAT1_PACKAMBIENT_RECOVER_TEMP (400)
#define DOAT1_PACKAMBIENT_CHECK_TIME (10)		//0.5sec
#define DOAT1_PACKAMBIENT_RECOVER_TIME (10)		//0.5sec				

//==============================================================================
//Discharging state DOAT3_Over Ambient Temperature Check Function
//Pack Ambient_temp >= 50'c and continue 3sec,DOAT3_PackAmbient  event occure
//System Reset or Clear Flag, DOAT3_PackAmbient  event recover
//==============================================================================			
#define DOAT3_PACKAMBIENT_CHECK_TEMP (500)
#define DOAT3_PACKAMBIENT_RECOVER_TEMP (400)
#define DOAT3_PACKAMBIENT_CHECK_TIME (6)		//0.5sec
#define DOAT3_PACKAMBIENT_RECOVER_TIME (10)		//0.5sec		
		
//==============================================================================
//Discharging state DUAT1_Under Ambient Temperature Check Function
//Pack Ambient_temp <= -5'c and continue 5sec,DUAT1_PackAmbient  event occure
//Pack Ambient_temp >  0'c and continue 5sec,DUAT1_PackAmbient  event recover
//==============================================================================			
#define DUAT1_PACKAMBIENT_CHECK_TEMP (-50)
#define DUAT1_PACKAMBIENT_RECOVER_TEMP (0)
#define DUAT1_PACKAMBIENT_CHECK_TIME (10)		//0.5sec
#define DUAT1_PACKAMBIENT_RECOVER_TIME (10)		//0.5sec		
		
//==============================================================================
//Discharging state DUAT3_Under Ambient Temperature Check Function
//Pack Ambient_temp <= -10'c and continue 3sec, DUAT3_PackAmbient  event occure
//System Reset or Clear Flag, DUAT3_PackAmbient  event recover
//==============================================================================			
#define DUAT3_PACKAMBIENT_CHECK_TEMP (-100)
#define DUAT3_PACKAMBIENT_RECOVER_TEMP (0)
#define DUAT3_PACKAMBIENT_CHECK_TIME (6)		//0.5sec
#define DUAT3_PACKAMBIENT_RECOVER_TIME (10)		//0.5sec	
				
//==============================================================================
//Pre-Charge Relay Status Check Function
//Pre-Charge Relay Status Connect and continue 3s , PCRS event recover
//==============================================================================	
#define PCRS_CHECK_TIME (6)	//0.5sec
#define PCRS_RECOVER_TIME (6)	//0.5sec	
	
//==============================================================================
//EBMS BCU BMU Version delay time
//==============================================================================	
#define BMUVER_DELAY_TIME (2)	//1sec

//==============================================================================
// SOC correction Current limit Function
// Discharge Current <= 10 A and continue 5 sec  , DCI  event occure
// Charge  Current <= 10 A and continue 5 sec  , CCI  event occure
//==============================================================================
#define DCI_CHECK (100)	//0.1A
#define DCI_CHECK_TIME (10)	//0.5sec
#define CCI_CHECK (-100)	//0.1A
#define CCI_CHECK_TIME (10)	//0.5sec
		
//==============================================================================
// 5% SOC Calibration Check 
//min_vol <= 3.0V and SOC > 5% ，continue 6sec , SOC5% event occure
//min_vol >= 3.1V and continue 6sec , SOC5% event recover
//==============================================================================
#define SOC5_CHECK_Volt (3000)	//1mV  
#define SOC5_RECOVER_Volt (3100)	//1mV
#define SOC_CHECK_5 (50)	//0.1%
#define SOC5_CHECK_TIME (12)	//0.5sec
#define SOC5_RECOVER_TIME (12)	//0.5sec

//==============================================================================
// 95% SOC Calibration Check 
//max_vol >= 3.36V and SOC < 95% ，continue 6sec , SOC95% event occure
//max_vol <= 3.3V and continue 6sec , SOC95% event recover
//==============================================================================
#define SOC95_CHECK_Volt (3360)	//1mV  
#define SOC95_RECOVER_Volt (3300)	//1mV
#define SOC_CHECK_95 (950)	//0.1%
#define SOC95_CHECK_TIME (12)	//0.5sec
#define SOC95_RECOVER_TIME (12)	//0.5sec
		

//==============================================================================
// SOH Update Action Function
// SOC < 10 % and MinVolt < 3000mV ,SOH Update
// SOH = FCC / DC
// SOC = RMC / FCC
//==============================================================================
#define SOH_MIN_Volt (3000)	//1mV	
#define SOH_USOC (900)	//0.1%
		
//Relay
#define RLY_ON (1)
#define RLY_OFF (0)	
#define RLY_FB_ON (0)
#define RLY_FB_OFF (1)	

//NFB Breaker
#define NFB_ON (1)			//20220817 ����
#define NFB_OFF (0)			//20220817 ����
#define NFB_FB_ON (0)		//20220817 ����
#define NFB_FB_OFF (1)		//20220817 ����

		
//==============================================================================
		
//==============================================================================
#define RELAY_CHECK_TIME (5)	// 1sec

		
	enum bcustatus_type_t : uint16_t
	{
		BCU_STATUS_OK   = 0x0000,
		BCU_STATUS_OVP  = 0x0001,
		BCU_STATUS_UVP  = 0x0002,
		BCU_STATUS_SBYOC = 0x0008,                               // added for standby mode over current detection (warning)
		BCU_STATUS_COTP = 0x0010,
		BCU_STATUS_CUTP = 0x0020,
		BCU_STATUS_DOTP = 0x0040,
		BCU_STATUS_DUTP = 0x0080,
		BCU_STATUS_COCP = 0x0100,
		BCU_STATUS_DOCP = 0x0200,
		BCU_STATUS_CREVP = 0x0400,                               //added for charge mode reverse current (warning)
		BCU_STATUS_DREVP = 0x0800,                               //added for drive mode reverse current (warning)
		BCU_STATUS_SCP  = 0x1000
		//BCU_STATUS_DIODE_OT = 0x2000,                                //eferry
		//Isolation_STATUS_UIP= 0x2000,     
//		Current_STATUS_CAN_FAULT=0x2000,
//		BCU_STATUS_RS485_FAULT = 0x4000,                               //added for any RS458 sub moudule comm fault
//		BCU_STATUS_CMCAN_FAULT = 0x8000	//CM CAN error
	
	};
	enum BCUStatusType_F : uint16_t
	{
		BCU_FAULT_OK  = 0x0000,
		EMS_STATUS_MBUCAN_FAULT  = 0x0001,						//MBU CAN error
		BCU_STATUS_RS485_FAULT = 0x0002,                                //added for any RS458 sub moudule comm fault
		CURRENT_STATUS_CAN_FAULT = 0x0004,
		BCU_STATUS_CMCAN_FAULT = 0x0008	                     //CM CAN error
		
	};
	enum BCUStatusType_1 : uint16_t
	{
		BCU_STATUS_OK1  = 0x0000,
		BCU_STATUS_OV1  = 0x0001,
		BCU_STATUS_UV1  = 0x0002,
		BCU_STATUS_OCC1 = 0x0004,
		BCU_STATUS_OCD1 = 0x0008,
		BCU_STATUS_OTC1 = 0x0010,
		BCU_STATUS_OTD1 = 0x0020,
		BCU_STATUS_UTC1 = 0x0040,
		BCU_STATUS_UTD1 = 0x0080
	};
	enum BCUStatusType_2 : uint16_t
	{
		BCU_STATUS_OK2  = 0x0000,
		BCU_STATUS_OV2  = 0x0001,
		BCU_STATUS_UV2  = 0x0002,
		BCU_STATUS_OCC2 = 0x0004,
		BCU_STATUS_OCD2 = 0x0008,
		BCU_STATUS_OTC2 = 0x0010,
		BCU_STATUS_OTD2 = 0x0020,
		BCU_STATUS_UTC2 = 0x0040,
		BCU_STATUS_UTD2 = 0x0080
	};
//	enum BCUStatusType_2nd : uint16_t
//	{
//		BCU_STATUS_OK2nd  = 0x0000,
//		BCU_STATUS_OV2nd  = 0x0001,
//		BCU_STATUS_UV2nd  = 0x0002,
//		BCU_STATUS_OCC2nd = 0x0004,
//		BCU_STATUS_OCD2nd = 0x0008,
//		BCU_STATUS_OTC2nd = 0x0010,
//		BCU_STATUS_OTD2nd = 0x0020,
//		BCU_STATUS_UTC2nd = 0x0040,
//		BCU_STATUS_UTD2nd = 0x0080
//	};
	enum BCUStatusType_3 : uint16_t
	{
		BCU_STATUS_OK3  = 0x0000,
		BCU_STATUS_OV3  = 0x0001,
		BCU_STATUS_UV3  = 0x0002,
		BCU_STATUS_OCC3 = 0x0004,
		BCU_STATUS_OCD3 = 0x0008,
		BCU_STATUS_OTC3 = 0x0010,
		BCU_STATUS_OTD3 = 0x0020,
		BCU_STATUS_UTC3 = 0x0040,
		BCU_STATUS_UTD3 = 0x0080
	};
	enum BCUStatusType_4 : uint16_t
	{
		BCU_STATUS_OK4 = 0x0000,
		BCU_STATUS_FAN = 0x0001,
		BCU_STATUS_OS1 = 0x0002,
		BCU_STATUS_US1 = 0x0004,
		BCU_STATUS_IV1 = 0x0008,
		BCU_STATUS_IT1 = 0x0010,
		BCU_STATUS_OG1 = 0x0020
		//BCU_STATUS_UTC3 = 0x0040,
		//BCU_STATUS_UTD3 = 0x0080,
	};
	enum BCUStatusType_5 : uint16_t
	{
		BCU_STATUS_OK5 = 0x0000,
//		Isolation_STATUS_UI2 = 0x0001,
		BCU_STATUS_OS2 = 0x0002,
		BCU_STATUS_US2 = 0x0004,
		BCU_STATUS_IV2 = 0x0008,
		BCU_STATUS_IT2 = 0x0010,
		BCU_STATUS_OG2 = 0x0020,
		BMU_STATUS_ADCFAULT = 0x0040
		//BCU_STATUS_UTD3 = 0x0080,
	};
	enum BCUStatusType_6 : uint16_t
	{
		BCU_STATUS_OK6 = 0x0000,
//		Isolation_STATUS_UI3 = 0x0001,
		BCU_STATUS_OS3 = 0x0002,
		BCU_STATUS_US3 = 0x0004,
		BCU_STATUS_IV3 = 0x0008,
		BCU_STATUS_IT3 = 0x0010,
		BCU_STATUS_OG3 = 0x0020,
		BCU_STATUS_SELFTESTFAULT = 0x0040
		//BCU_STATUS_UTD3 = 0x0080,
	};
//	enum BCUStatusType_Init : uint8_t
//	{
//		BCU_STATUS_Initialed  = 0x01,
//		BCU_STATUS_BMUinitialed = 0x02,
//		BCU_STATUS_ShuntInitialed = 0x04,
//		BCU_STATUS_InsulationInitialed  = 0x08,
//		BCU_STATUS_ChargeEnable  = 0x10,
//		BCU_STATUS_DischargeEnable  = 0x20,
//		BCU_STATUS_FullCharge  = 0x40,
//		BCU_STATUS_FullDischarge  = 0x80
//		
//	};
	// NOTE: BCU State Redefined
	/*! Status of the internal modbus FSM. */
	enum class BCUStateType_t : uint8_t 
	{
		//BCU state
		BCU_INIT = 0x01,
		BCU_STANDBY = 0x10,
		BCU_DRIVE = 0x30,                                 			//drive mode 
		BCU_DRIVE_THRU_PRECHARGE = 0x21,     //precharge state before drive
		BCU_CHARGE_THRU_PRECHARGE = 0x22,  
		BCU_CHARGE = 0x40,                                			//charge only
		BCU_PASSIVE_PRECHARGE = 0x20,
		BCU_IDLE = 0x11,
		
//		BCU_AUTO = 0xAA,
//		BCU_AUTO_PRECHARGE = 0x2B,                                 //precharge
//
//		BCU_PASSIVE = 0x2A,
			
//		BCU_DRIVE_THRU_QUICK_PRECHARGE = 0x62,                                 //with quick precharge		

//		BCU_DRIVE_TO_CHARGE = 0x80,
//		BCU_CHARGE_TO_DRIVE = 0x90,

//		BCU_DRIVE_WITH_CHG_ON = 0xA0,                                		//DRIVE wtih charge enabled
//		BCU_DRIVE_WITH_CHG_ON_THRU_PRECHARGE = 0xA1,

//		BCU_DRIVE_WITH_PRECHG_ON = 0xB0,                                	//DRIVE wtih Precharge enabled
//		BCU_DRIVE_WITH_PRECHG_ON_THRU_PRECHARGE = 0xB1,

		
		//Error Status
		BCU_OVP = 0xF0,  	//Over Voltage Protection
		BCU_UVP = 0xF1,  	//Under Voltage Protection
		BCU_OTCP = 0xF2,  	//Charge Over Temperature Protection
		BCU_OTDP = 0xF3,  	//Discharge Over Temperature Protection
		BCU_UTCP = 0xF4,   	//Charge Under Temperature Protection
		BCU_UTDP = 0xF5,   	//Discharge Under Temperature Protection
		BCU_COCP = 0xF6,    //Over Charge Protection
		BCU_DOCP = 0xF7,    //Over Discharge Protection
		BPU_OTP = 0xF8,  	//BPU Over Temperature Protection(PCS+- HVDC+- Fuse)
		PACK_OATP = 0xF9,	//Abnormal Pack Ambient Temperature Protection

		Insulation_FAULT = 0xFD,				//Insulation_Check FAULT
		BCU_HVIL_FAULT = 0xFE,					//HVIL FAULT
		BCU_FAULT = 0xFF,                       //BCU_STATUS_CMCAN_FAULT	
		
		BMU_ADC_FAULT = 0xE0,					//BMU ADC FAULT
		BCU_SELFTEST_FAULT = 0xEE,				//BCU Self-Test FAULT
		BCU_Emergency_Stop = 0xE7,				//BCU_Emergency Stop 
		
		ESS_NFB_FAULT = 0xEB,					//BPU Breaker Status FAULT
		ESS_PRECHGRELAY_FAULT = 0xEC,			//BPU Pre-Charge Relay Status FAULT
		ESS_HV_MAINRELAY_FAULT = 0xED,			//BPU Pre-Charge Relay Status FAULT
		ESS_HV_NEGRELAY_FAULT = 0xEF,			//BPU Pre-Charge Relay Status FAULT
		
		BCU_STATE_NONE = 0x99
	};

	enum class RELAYStatusType_t : uint8_t {
		RELAY_OFF = 0x00,
		PRECHARGE = 0x01,
		DISCHARGE = 0x02,
		/** only discharging relay ON.*/
		CHARGE = 0x03,
		/** only charging relay ON.*/
		DSG_AND_CHG = 0x04,
		/** only charging relay ON.*/
		DSG_AND_PRECHG = 0x05,
		
		Emergency_Stop = 0x07, //BCU_Emergency Stop 
		
		STANDBY = 0x06,

		STATUS_NONE = 0xFF
	};

	enum class BCUDriveType_t : uint8_t {
		STANDBY = 0x00,
		CHARGE = 0x01,
		DRIVE = 0x02,
		Emergency_Stop = 0x07,
		DRIVE_WITH_CHARGE = 0x03,
		//added fro commsioning
	    DRIVE_WITH_PRECHG = 0x04,
		//added fro commsioning
		IDLE = 0x05,
	    NONE = 0xFF
	};

	//TODO: eferry bcu mode changing command
	enum class BCUModeChangeType_t : uint8_t {
		STANDBY = 0x00,
		CHARGE = 0x03,
		DRIVE = 0x04,
		
		CHARGE_PRECHARGE = 0x01,
		DRIVE_PRECHARGE = 0x02,
		//PRECHARGE = 0x03,     
		BCU_Emergency_Stop = 0x07,	

		DRIVE_WITH_CHARGE = 0x05,                               
		DRIVE_WITH_PRECHG = 0x06,                               

		MAX_CHANGE = 0x0A, //check with > MAX_CHANGE

		SHUTDOWN = 0x08, //not implemented
	
		NOT_VALID = 0x80, //illegal command for FSM state
					//eg.: Precharge can only accept under standby mode
			NONE = 0xFF
//		STANDBY = 0x00,
//		CHARGE = 0x04,
//		DRIVE = 0x09,
//		
//		CHARGE_PRECHARGE = 0x01,
//        DRIVE_PRECHARGE = 0x02,
//		PRECHARGE = 0x03,     
//		BCU_Emergency_Stop = 0x09,	
//
//		DRIVE_WITH_CHARGE = 0x05,                               
//		DRIVE_WITH_PRECHG = 0x07,                               
//
//		MAX_CHANGE = 0x0A,                               //check with > MAX_CHANGE
//
//		SHUTDOWN = 0x08,                               //not implemented
//	
//		NOT_VALID = 0x80,                              	//illegal command for FSM state
//							//eg.: Precharge can only accept under standby mode
//		NONE = 0xFF
	};

	class BmsEngine
	{
	public:
		BmsEngine();
		~BmsEngine();
			
		struct Flag
		{
			static bool OV1;
			static bool ov1_deboc;
			static bool UV1;
			static bool uv1_deboc;

			static bool OV2;
			static bool ov2_deboc;
			static bool OV2nd;
			static bool ov2nd_deboc;
			static bool UV2;
			static bool uv2_deboc;

			static bool OV3;
			static bool ov3_deboc;
			static bool UV3;
			static bool uv3_deboc;

			static bool OTC1;
			static bool otc1_deboc;
			static bool UTC1;
			static bool utc1_deboc;

			static bool OTC2;
			static bool otc2_deboc;
			static bool UTC2;
			static bool utc2_deboc;

			static bool OTC3;
			static bool otc3_deboc;
			static bool UTC3;
			static bool utc3_deboc;
			
			static bool OTD1;
			static bool otd1_deboc;
			static bool UTD1;
			static bool utd1_deboc;
			
			static bool OTD2;
			static bool otd2_deboc;
			static bool UTD2;
			static bool utd2_deboc;
			
			static bool OTD3;
			static bool otd3_deboc;
			static bool UTD3;
			static bool utd3_deboc;
			
			static bool OCC1;
			static bool occ1_deboc;
			static bool OCD1;
			static bool ocd1_deboc;
			
			static bool OCC2;
			static bool occ2_deboc;
			static bool OCD2;
			static bool ocd2_deboc;
			
			static bool OCC3;
			static bool occ3_deboc;
			static bool OCD3;
			static bool ocd3_deboc;

			
			//static bool HVIL1;
			static bool hvil1_deboc;
			static bool HVIL1_Error;
			
			static bool hvil2_deboc;
			static bool HVIL2_Error;
			
			
			static bool IV1;
			static bool iv1_deboc;
			static bool IV2;
			static bool iv2_deboc;
			static bool IV3;
			static bool iv3_deboc;
			
			
			static bool CCF;
			static bool current_deboc;
			
			static bool ECF;
			static bool ems_deboc;
			
			static bool CHG_E;
			static bool chg_e_deboc;
			
			static bool DSG_E;
			static bool dsg_e_deboc;
			
			static bool FullCHG;
			static bool fullchg_deboc;
			
			static bool FullDSG;
			static bool fulldsg_deboc;
			
			static bool ADCTempFault;
			static bool ADCVoltFault;
			static bool ADCFault;
			static bool adcfault_deboc;
			//			static bool BMU_ADCFault;
			static bool ADCTempWarning;
			static bool ADCWarning;
			static bool adcWarning_deboc;
			
			static bool ESS_BreakerFault;
			static bool ESS_PreChargeRelayFault;
			static bool ESS_HV_MainRelayFault;
			static bool ESS_HV_NegRelayFault;
			static bool ESS_FAN_Warning;
	
			
//			static bool PCRS_Error;
			static bool pcrs_deboc;
			
			static bool DCI;
			static bool dci_deboc;
			static bool CCI;
			static bool cci_deboc;			
			
			static bool OT1_PCS_Positive;
			static bool ot1_PCS_Positive_deboc;
			static bool OT2_PCS_Positive;
			static bool ot2_PCS_Positive_deboc;
			
			static bool OT1_PCS_Negative;
			static bool ot1_PCS_Negative_deboc;
			static bool OT2_PCS_Negative;
			static bool ot2_PCS_Negative_deboc;
			
			static bool OT1_HVDC_Positive;
			static bool ot1_HVDC_Positive_deboc;
			static bool OT2_HVDC_Positive;
			static bool ot2_HVDC_Positive_deboc;
			
			static bool OT1_HVDC_Negative;
			static bool ot1_HVDC_Negative_deboc;
			static bool OT2_HVDC_Negative;
			static bool ot2_HVDC_Negative_deboc;
			
			static bool OT1_Fuse;
			static bool ot1_Fuse_deboc;
			static bool OT2_Fuse;
			static bool ot2_Fuse_deboc;
			
			static bool OT1_TA;
			static bool ot1_TA_deboc;
			
			static bool OAT1_PackAmbient;
			static bool OAT1_PackAmbient_Debounce;
			static bool OAT3_PackAmbient;
			static bool OAT3_PackAmbient_Debounce;
			
			//Pack Ambient Temp protection
			static bool COAT1_PackAmbient;
			static bool COAT1_PackAmbient_Debounce;
			static bool COAT3_PackAmbient;
			static bool COAT3_PackAmbient_Debounce;
			static bool CUAT1_PackAmbient;
			static bool CUAT1_PackAmbient_Debounce;
			static bool CUAT3_PackAmbient;
			static bool CUAT3_PackAmbient_Debounce;
			
			static bool DOAT1_PackAmbient;
			static bool DOAT1_PackAmbient_Debounce;
			static bool DOAT3_PackAmbient;
			static bool DOAT3_PackAmbient_Debounce;
			static bool DUAT1_PackAmbient;
			static bool DUAT1_PackAmbient_Debounce;
			static bool DUAT3_PackAmbient;
			static bool DUAT3_PackAmbient_Debounce;
			
			static bool SOC_CAL5;
			static bool SOC_CAL5_deboc;
			static bool SOC5_Calibration_START;
			
			static bool SOC_CAL95;
			static bool SOC_CAL95_deboc;
			static bool SOC95_Calibration_START;
		};
		
		struct Time
		{
			static uint8_t OV1_time_delay;
			static uint8_t UV1_time_delay;

			static uint8_t OV2_time_delay;
			static uint8_t UV2_time_delay;

			static uint8_t OV3_time_delay;
			static uint8_t UV3_time_delay;

			static uint8_t OTC1_time_delay;
			static uint8_t UTC1_time_delay;

			static uint8_t OTC2_time_delay;
			static uint8_t UTC2_time_delay;

			static uint8_t OTC3_time_delay;
			static uint8_t UTC3_time_delay;
			
			static uint8_t OTD1_time_delay;
			static uint8_t UTD1_time_delay;

			static uint8_t OTD2_time_delay;
			static uint8_t UTD2_time_delay;

			static uint8_t OTD3_time_delay;
			static uint8_t UTD3_time_delay;
			
			static uint8_t OCC1_time_delay;
			static uint8_t OCD1_time_delay;
			
			static uint8_t OCC2_time_delay;
			static uint16_t OCD2_time_delay;
			
			static uint8_t OCC3_time_delay;
			static uint8_t OCD3_time_delay;

			
			static uint16_t hvil1_time_delay;
			static uint16_t hvil2_time_delay;
			static uint16_t hvil3_time_delay;
			
			
			static uint8_t IV1_time_delay;
			static uint8_t IV2_time_delay;
			static uint8_t IV3_time_delay;
			
			static uint8_t IT1_time_delay;
			static uint8_t IT2_time_delay;
			static uint8_t IT3_time_delay;
			
			static uint16_t current_time_delay;
			static uint16_t ems_time_delay;
			
			
			static uint8_t CHG_E_time_delay;
			
			static uint8_t DSG_E_time_delay;
			
			static uint8_t FullCHG_time_delay;
			static uint8_t FullDSG_time_delay;
			
			static uint8_t ADCFault_time_delay;
			static uint8_t ADCWarning_time_delay;
			
			static uint8_t pcrs_time_delay;
			
			static uint8_t DCI_time_delay;
			static uint8_t CCI_time_delay;
						
			static uint16_t OT1_PCS_Positive_time_delay;
			static uint16_t OT2_PCS_Positive_time_delay;
	
			static uint16_t OT1_PCS_Negative_time_delay;
			static uint16_t OT2_PCS_Negative_time_delay;
			
			static uint16_t OT1_HVDC_Positive_time_delay;
			static uint16_t OT2_HVDC_Positive_time_delay;
	
			static uint16_t OT1_HVDC_Negative_time_delay;
			static uint16_t OT2_HVDC_Negative_time_delay;
			
			static uint16_t OT1_Fuse_time_delay;
			static uint16_t OT2_Fuse_time_delay;
			
			static uint16_t OT1_TA_time_delay;
			
			static uint16_t OAT1_PackAmbient_Time_Delay;
			static uint16_t OAT3_PackAmbient_Time_Delay;
			
			static uint16_t COAT1_PackAmbient_Time_Delay;
			static uint16_t COAT3_PackAmbient_Time_Delay;
			static uint16_t CUAT1_PackAmbient_Time_Delay;
			static uint16_t CUAT3_PackAmbient_Time_Delay;
			
			static uint16_t DOAT1_PackAmbient_Time_Delay;
			static uint16_t DOAT3_PackAmbient_Time_Delay;
			static uint16_t DUAT1_PackAmbient_Time_Delay;
			static uint16_t DUAT3_PackAmbient_Time_Delay;		
			
			static uint16_t SOC_CAL5_time_delay;
			
			static uint16_t SOC_CAL95_time_delay;
		};
		
		struct SBCUCANMessageCapacity
		{
			static bool Initialed;
			static bool BMUinitialed; 
			static bool ShuntInitialed;	
			static bool InsulationInitialed;		
			static bool ChargeEnable;				
			static bool DischargeEnable;	
			static bool FullCharge;				
			static bool FullDischarge;				
			
		};
	//}SBCUCANMessageCapacity;
		
//		struct BCUCANMessageProtectionComm
//		{
//			static bool I2C_Communication;
//			static bool Insulation_Communication; 
//			static bool Shunt_Communication;	
//			static bool BMU_Communication;		
//	
//			
//		}BCUCANMessageProtectionComm;


		//static BatterySys *p_battsys;
		static BmsEngine *p_bmspara;
		static BCUStateType_t m_bcuState;

		static BCUStateType_t m_preState;
		static RELAYStatusType_t m_status;
		static BCUDriveType_t m_bcuDrive;
		static volatile BCUModeChangeType_t m_bcuModeChange;                               //TODO: for eferry


		static uint16_t m_bcuStatus;
		static uint16_t m_bcuStatus1;
		static uint16_t m_bcuStatus2;
		static uint16_t m_bcuStatus3;
		static uint16_t m_bcuStatus4;
		static uint16_t m_bcuStatus5;
		static uint16_t m_bcuStatus6;


//		static int m_timeoutCount;
	
		static uint16_t MaxSort_Volt;
		static uint16_t MinSort_Volt;
		static void SortCellVoltage();
		static uint16_t MaxCellVoltage();
		static uint16_t MinCellVoltage();

		static int16_t MaxSort_Temp;
		static int16_t MinSort_Temp;
		static void SortCellTemperature();
		static int16_t MaxCellTemperature();
		static int16_t MinCellTemperature();

		static int16_t AVGCellTemperature();
	

		static volatile uint8_t m_bcuId;
		

		static uint16_t m_dsgAccum;           //unti: 0.1Ah (increase when discharge, decrease while charge)
		static uint16_t m_fccgAccum;
		static uint16_t m_remainCapacity;                              		//RMC: SOC=RMC/FCC
		static uint16_t m_fullChargeCapacity;                              	//FCC:
		static uint16_t m_designCapacity;                              		//DC:  SOH= FCC/DC
//		static uint16_t m_cycleCount;
		static float coulombCount;                              	//0.5sec integration (raw integ.)
		static uint16_t m_soc;                               //0.1%
		static uint16_t m_soh;                               //0.1%

		static uint16_t Cumulative_times;

		static bool BMUCANFAULTFlag;

		static BCUStateType_t BcuFSM();
//		static uint8_t bcuRelayStatus();


		static uint16_t CheckVolt_OV1();
		static uint16_t CheckVolt_UV1();
		static uint16_t CheckVolt_OV2();

		static uint16_t CheckVolt_UV2();
		static uint16_t CheckVolt_OV3();
		static uint16_t CheckVolt_UV3();


		static uint16_t CheckTemp_OTC1();
		static uint16_t CheckTemp_OTC2();
		static uint16_t CheckTemp_OTC3();
		static uint16_t CheckTemp_OTD1();
		static uint16_t CheckTemp_OTD2();
		static uint16_t CheckTemp_OTD3();
		static uint16_t CheckTemp_UTC1();
		static uint16_t CheckTemp_UTC2();
		static uint16_t CheckTemp_UTC3();
		static uint16_t CheckTemp_UTD1();
		static uint16_t CheckTemp_UTD2();
		static uint16_t CheckTemp_UTD3();
		
		static uint16_t CheckCurrent_OCC1();
		static uint16_t CheckCurrent_OCC2();
		static uint16_t CheckCurrent_OCC3();
		static uint16_t CheckCurrent_OCD1();
		static uint16_t CheckCurrent_OCD2();
		static uint16_t CheckCurrent_OCD3();

		
		static void HVIL1_check();
//		static void HVIL2_check();
		static void HVIL2_Insulation_Check(void);

			
		static void PreCHGRealyStatus_check();

		static uint16_t CheckImbalance_IV1();
	
		static uint16_t Check_FAN_FB();
		
		static int16_t SortPack_MAXAmbientTemp();
		static int16_t SortPack_MINAmbientTemp();

		static void CheckTempCOAT1_PackAmbientTemp(int16_t Max_PackAmbient_Temp);
		static void CheckTempCOAT3_PackAmbientTemp(int16_t Max_PackAmbient_Temp);
		static void CheckTempCUAT1_PackAmbientTemp(int16_t Min_PackAmbient_Temp);
		static void CheckTempCUAT3_PackAmbientTemp(int16_t Min_PackAmbient_Temp);
		
		static void CheckTempDOAT1_PackAmbientTemp(int16_t Max_PackAmbient_Temp);
		static void CheckTempDOAT3_PackAmbientTemp(int16_t Max_PackAmbient_Temp);
		static void CheckTempDUAT1_PackAmbientTemp(int16_t Min_PackAmbient_Temp);
		static void CheckTempDUAT3_PackAmbientTemp(int16_t Min_PackAmbient_Temp);
		
		static void CheckTempOT1_PCS_Positive();
		static void CheckTempOT2_PCS_Positive();
		static void CheckTempOT1_PCS_Negative();
		static void CheckTempOT2_PCS_Negative();
		
		static void CheckTempOT1_HVDC_Positive();
		static void CheckTempOT2_HVDC_Positive();
		static void CheckTempOT1_HVDC_Negative();
		static void CheckTempOT2_HVDC_Negative();
		
		static void CheckTempOT1_Fuse();
		static void CheckTempOT2_Fuse();
		
		static void CheckTempOT1_TA();
		
		static void Check_CHG_Enable();
		static void Check_DSG_Enable();		
		static void Check_FullCHG();		
		static void Check_FullDSG();
		
		static uint16_t Check_ADCFault();				//BMU ADC Fault
		static uint16_t Check_ADCWarning();			//BMU ADC Warning
		static uint16_t Check_SelfTestFault();			//BCU Self Test Fault
		
		static void CheckSOC5_Calibration();
		static void CheckSOC95_Calibration();
		
		static uint32_t FaultCount;
		static uint32_t NFB_FAULT_Count;
		static uint16_t Main_FAULT_Count;
		static uint16_t Neg_FAULT_Count;
		static uint16_t Precharge_FAULT_Count;
		static uint16_t FAN_Warning_Count;

		uint8_t GetBcuId();
		void SetBcuId(uint8_t newBcuId);

		uint16_t GetRemainCapacity();
		void PutRemainCapacity(uint16_t rmcValue);

		uint16_t GetFullChargeCapacity();
		void PutFullChargeCapacity(uint16_t fccValue);

		uint16_t GetDesignCapacity();
		void PutDesignCapacity(uint16_t dcValue);
		void SetDefaultCapacity(uint16_t fcc, uint16_t rmc, uint16_t dc);

		//void ResetDefaultCapacity(uint8_t soc, uint8_t soh, float designCapacity);
		void ResetCoulombCountSoc();
		void RetrieveStoredCapacity();
		void SetDefaultRtcBackup();


		static void StateOfCharge(int32_t ampere);                               //ampere unit: 1mA

		void BcuProcess();

		uint16_t CheckCellMonitorCanBusWire();
		uint32_t CheckCurrentMonitorCanBusWire();
		uint16_t CheckCurrentCanBusWire();
		
		uint16_t CheckEMSCanBusWire();
		static void CheckEmsCanCounter();
		static void CheckCurrCanCounter();
		
		static void CheckIsoSPIcommunication(void);
		
		static void CheckT500msCountdown();
		static void CheckHVILCountdown();

		
		static int status_count;
		static void LED_Status();
		
		uint8_t CheckCanRs485Comm();
		uint16_t CheckCellMonitorCanBusWire(const uint16_t start_idx, const uint16_t stop_idx);
		void ClearCanTimeoutCount();


		uint16_t m_eventLog[EVENT_LOG_LENGTH]{ 0 };
		int m_eventCount{ 0 };


		struct CmInfoReq_t {
			bool cmInfoReq;
			char cmId;
			char funcId;
		};
		CmInfoReq_t m_cmInfoRequest;

		struct CmInfoReport_t {
			bool cmInfoRpt;
			char cmId;
			char funcId;
		};
		CmInfoReport_t m_cmInfoReport;

		static uint16_t socCounter;
		int16_t ampReading{ 0 };
		// int32_t ampReading_mA{ 0 };
		static int32_t ampReading_mA;

		uint16_t chkResult{bcustatus_type_t::BCU_STATUS_OK};
		static uint32_t NFBCount;
		static uint32_t count;
		
		private :

		bool canTimeoutFlag = false;

		static uint32_t fsmCount;
		


		//void LogEvent();
		void LogEvent(uint16_t status);
		void UpdateTimeoutCount();
		//void ClearCanTimeoutCount();

		static void CheckEvent(const BCUStateType_t state);
		static void StateRecovery(const BCUStateType_t state);
		static void EFerryStateRecovery(const BCUStateType_t state);
	};
#ifdef __cplusplus
		
	}
#endif // __cplusplus
} //namespace bmstech
