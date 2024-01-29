//File
#pragma once
#include <mbed.h>
#include "PackConst.h"

namespace bmstech 
{

//#define FULLSOC (3360)
//#define EMPTYSOC (2800)
//#define SELF_CONSUMPTION (3)  //5mA(AFE)
//#define DCIR (64)	//0.64mΩ  1S30P  170mΩ/264
//#define SCALE (3.5)
//#define HSOC (80)
//#define LSOC (30)
//
//#define TIME_6HR (60*60*6)	//6hr       //(60*10) //10min
//#define MAXSET_TIMEHR (100)		//100hr 	
//#define MINSET_TIMEHR (1)		//1hr 	
//#define TESTSET_TIMEHR (110)		//10min 

const uint16_t FULLSOC = 3360U;
const uint16_t EMPTYSOC = 2800U;
const uint16_t SELF_CONSUMPTION = 3U;
	
	const uint16_t SOC5_Calibration = 5; //SOC 5% Calibration
	const uint16_t SOC95_Calibration = 95; //SOC 95% Calibration
//	struct Calibration
//	{
//		const uint16_t SOC_5 = 5;
//	}; 	
	
	class SOC
	{
		public:

		
		static uint16_t OCV_SOC;
		
		static bool_t Set_BCU_SOC_Flag;
		static bool_t Set_BCU_SOC_Time_Flag;
		static uint8_t RegisterSetSOC_Time;
		
		static bool_t Set_BCU_SOH_Flag;
		
//		static uint32_t Timeout6hr_Counter;
		static bool_t Count_down_6hr_Flag;
		
		static uint32_t FCCcoulombCount;
		static bool_t FCCcoulombCount_Flag;
		static bool_t SOHupdate_END_Flag;
		static bool_t SOHupdate_Start_Flag;
		
		static bool_t StoredSOH_Flag;
		static bool_t SET_StoredSOH_Flag;

		
		static bool_t FullCHG_END_Flag;
		static bool_t FullDSG_END_Flag;
		
		static void Count_down_Time();
		static void Correction_SOC();
		static void SET_BCU_SOC_Time();
		
		static uint8_t SOC_OCV(const uint16_t min_value);
		static void Dynamic_Calibration(unsigned int soc);
		static void Dynamic_Start(unsigned int soc);
		static void Calibration_SOC();
		static void Current_limit();
		static void SOHUpdate_Action();
		
		// Do nothing
		~SOC() {
			// Do nothing
		}
	};
} //namespace bmstech
