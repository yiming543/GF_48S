//File
#include "BmsEngine.h"
#include "BatterySys.h"

#include "Balance.h"		

#include "SOC.h"
#include "LTC6811_daisy.h"

#include "main.h"
#include "ADC_Temperature.h"

extern DigitalOut RLY_NFB;			//extern DigitalOut
extern DigitalOut RLY_Precharge;	//extern DigitalOut
extern DigitalOut RLY_HV_Main;		//extern DigitalOut
extern DigitalOut RLY_HV_Neg;		//extern DigitalOut
extern DigitalOut RLY_K5_FAN;		//extern DigitalOut

extern DigitalOut LED_Status1;		//extern DigitalOut

extern DigitalOut Blue_LED;			//extern DigitalOut
extern DigitalOut Green_LED;		//extern DigitalOut
extern DigitalOut Red_LED;			//extern DigitalOut


extern DigitalIn RLY_FB_Precharge; //RLY_Precharge FB 
extern DigitalIn RLY_FB_HV_Main; //RLY_HV_Main FB
extern DigitalIn RLY_FB_HV_Neg; //RLY_HV_Neg FB 
extern DigitalIn RLY_FB_NFB; //RLY_FB_KQ1 NFB

DigitalIn HV1_Emergency(HVIL1, PullUp); //Emergency
DigitalIn HV2_Insulation(HVIL2); //Insulation
//DigitalIn HV3_HVIL(HVIL3);
//DigitalIn HV4_HVIL(HVIL4);

namespace bmstech
{
	Balance balance1;	//balance

	//SortCellVoltage
	void BmsEngine::SortCellVoltage()
	{
		for (uint8_t i = 1; i < (PackConst::MAX_PACK_NUM + 1); i++)//
		{
			BatterySys::pack[i].packMaxVolt =  static_cast<uint16_t>(balance1.CellVolt_max(i));			
			BatterySys::pack[i].packMinVolt =  static_cast<uint16_t>(balance1.CellVolt_min(i));
		}
	}
	
	//MaxCellVoltage
	uint16_t BmsEngine::MaxCellVoltage()
	{
		Balance::MaxPackVolt_id_NUM = static_cast<uint8_t>(balance1.PackVolt_max());
		uint16_t Max_Volt = BatterySys::pack[Balance::MaxPackVolt_id_NUM].packMaxVolt;//
		return (Max_Volt);
	}
	
	//MinCellVoltage
	uint16_t BmsEngine::MinCellVoltage()
	{
		Balance::MinPackVolt_id_NUM = static_cast<uint8_t>(balance1.PackVolt_min()); //
		uint16_t Min_Volt = BatterySys::pack[Balance::MinPackVolt_id_NUM].packMinVolt; //
		return (Min_Volt);
	}
	
	//==============================================================================
	//OV1 Check Function
	//max_vol >= 3.5V and continue 5sec , OV1 event occure
	//max_vol <= 3.4V and continue 5sec , OV1 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckVolt_OV1()
	{
		if (BmsEngine::Flag::OV1 == 0)
		{
			if (BatterySys::MaxVolt >= OV1_CHECK)
			{
				BmsEngine::Flag::ov1_deboc = 1;
				if (BmsEngine::Time::OV1_time_delay == 0)
				{
					BmsEngine::Flag::OV1 = 1;
					BmsEngine::Flag::ov1_deboc = 0;
					BmsEngine::Time::OV1_time_delay = OV1_RECOVER_TIME;
					p_bmspara->m_bcuStatus1 |= (uint16_t)BCU_STATUS_OV1; //BCU_STATUS_OVP;
				}
			}
			else if (BatterySys::MaxVolt < OV1_CHECK)
			{
				BmsEngine::Time::OV1_time_delay = OV1_CHECK_TIME;                                 		
			}
		}
		else if (BmsEngine::Flag::OV1 == 1)
		{
			if (BatterySys::MaxVolt <= OV1_RECOVER)
			{
				BmsEngine::Flag::ov1_deboc = 1;
				if (BmsEngine::Time::OV1_time_delay == 0)
				{
					BmsEngine::Flag::OV1 = 0;
					BmsEngine::Time::OV1_time_delay = OV1_CHECK_TIME;
					BmsEngine::Flag::ov1_deboc = 0;
					p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_OV1; //BCU_STATUS_OVP;
				}
			}
			else if (BatterySys::MaxVolt > OV1_RECOVER)
			{
				BmsEngine::Time::OV1_time_delay = OV1_RECOVER_TIME;
			}
		}
		return (p_bmspara->m_bcuStatus1);
	}
	
	//==============================================================================
	//OV2 Check Function
	//max_vol >= 3.6V and continue 3sec , OV2 event occure
	//max_vol <= 3.5V and continue 5sec , OV2 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckVolt_OV2()
	{
		if (BmsEngine::Flag::OV2 == 0)
		{
			if (BatterySys::MaxVolt >= OV2_CHECK)
			{
				BmsEngine::Flag::ov2_deboc = 1;
				if (BmsEngine::Time::OV2_time_delay == 0)
				{
					BmsEngine::Flag::OV2 = 1;
					BmsEngine::Flag::ov2_deboc = 0;
					BmsEngine::Time::OV2_time_delay = OV2_RECOVER_TIME;
					p_bmspara->m_bcuStatus2 |= (uint16_t)BCU_STATUS_OV2; //BCU_STATUS_OVP;
					BmsEngine::FaultCount = 0;
				}
			}
			else if (BatterySys::MaxVolt < OV2_CHECK)
			{
				BmsEngine::Time::OV2_time_delay = OV2_CHECK_TIME; //3s
			}
		}
		else if (BmsEngine::Flag::OV2 == 1)
		{
			if (BatterySys::MaxVolt <= OV2_RECOVER)
			{
				BmsEngine::Flag::ov2_deboc = 1;
				if (BmsEngine::Time::OV2_time_delay == 0)
				{										
					BmsEngine::Flag::OV2 = 0;
					BmsEngine::Time::OV2_time_delay = OV2_CHECK_TIME;
					BmsEngine::Flag::ov2_deboc = 0;
					p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OV2; //BCU_STATUS_OVP;			
		
				}
			}
			else if (BatterySys::MaxVolt > OV2_RECOVER)
			{
				BmsEngine::Time::OV2_time_delay = OV2_RECOVER_TIME;
			}
		}
		return (p_bmspara->m_bcuStatus2);
	}

	//==============================================================================
	//OV3 Check Function
	//max_vol >= 3.65V and continue 1sec , OV3 event occure
	//System Reset or Clear Flag, OV3 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckVolt_OV3()
	{
		if (BmsEngine::Flag::OV3 == 0)
		{
			if (BatterySys::MaxVolt >= OV3_CHECK)
			{
				BmsEngine::Flag::ov3_deboc = 1;
				if (BmsEngine::Time::OV3_time_delay == 0)
				{
					BmsEngine::Flag::OV3 = 1;
					BmsEngine::Flag::ov3_deboc = 0;
					BmsEngine::Time::OV3_time_delay = OV3_CHECK_TIME;	//OV3_RECOVER_TIME;
					p_bmspara->m_bcuStatus3 |= (uint16_t)BCU_STATUS_OV3; //BCU_STATUS_OVP;
					BmsEngine::FaultCount = 0;
				}
			}
			else if (BatterySys::MaxVolt < OV3_CHECK)
			{
				BmsEngine::Time::OV3_time_delay = OV3_CHECK_TIME;                                   		
			}
		}
//		else if (BmsEngine::flag::OV3 == 1)
//		{
//			if (BatterySys::MaxVolt <= OV3_Recover)
//			{
//				BmsEngine::flag::ov3_deboc = 1;
//				if (BmsEngine::time::OV3_time_delay == 0)
//				{
//					BmsEngine::flag::OV3 = 0;
//					BmsEngine::time::OV3_time_delay = OV3_Check_Time;
//					BmsEngine::flag::ov3_deboc = 0;
//					p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OV3; //BCU_STATUS_OVP;
//				}
//			}
//			else if (BatterySys::MaxVolt > OV3_Recover)
//			{
//				BmsEngine::time::OV3_time_delay = OV3_Recover_Time;
//			}
//		}
		return (p_bmspara->m_bcuStatus3);
	}
	
	//==============================================================================
	//UV1 Check Function
	//min_vol <= 2.8V and continue 5sec , UV1 event occure
	//min_vol >= 2.9V and continue 5sec , UV1 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckVolt_UV1()
	{
		if (BmsEngine::Flag::UV1 == 0)
		{
			if (BatterySys::MinVolt <= UV1_CHECK)
			{
				BmsEngine::Flag::uv1_deboc = 1;
				if (BmsEngine::Time::UV1_time_delay == 0)
				{
					BmsEngine::Flag::UV1 = 1;
					BmsEngine::Flag::uv1_deboc = 0;
					BmsEngine::Time::UV1_time_delay = UV1_RECOVER_TIME;
					p_bmspara->m_bcuStatus1 |= (uint16_t)BCU_STATUS_UV1;
				}
			}
			else if (BatterySys::MinVolt > UV1_CHECK)
			{
				BmsEngine::Time::UV1_time_delay = UV1_CHECK_TIME;                                  		
			}
		}
		else if (BmsEngine::Flag::UV1 == 1)
		{
			if (BatterySys::MinVolt >= UV1_RECOVER)
			{
				BmsEngine::Flag::uv1_deboc = 1;
				if (BmsEngine::Time::UV1_time_delay == 0)
				{
					BmsEngine::Flag::UV1 = 0;
					BmsEngine::Time::UV1_time_delay = UV1_CHECK_TIME;
					BmsEngine::Flag::uv1_deboc = 0;
					p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_UV1;
				}
			}
			else if (BatterySys::MinVolt < UV1_RECOVER)
			{
				BmsEngine::Time::UV1_time_delay = UV1_RECOVER_TIME;
			}
		}
		return (p_bmspara->m_bcuStatus1);
	}
	
	//==============================================================================
	//UV2 Check Function
	//min_vol <= 2.7V and continue 3sec , UV2 event occure
	//min_vol >= 2.8V and continue 5sec , UV2 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckVolt_UV2()
	{	
		if (BmsEngine::Flag::UV2 == 0)
		{
			if (BatterySys::MinVolt <= UV2_CHECK)
			{
				BmsEngine::Flag::uv2_deboc = 1;
				if (BmsEngine::Time::UV2_time_delay == 0)
				{
					BmsEngine::Flag::UV2 = 1;
					BmsEngine::Flag::uv2_deboc = 0;
					BmsEngine::Time::UV2_time_delay = UV2_RECOVER_TIME;
					p_bmspara->m_bcuStatus2 |= (uint16_t)BCU_STATUS_UV2;
					BmsEngine::FaultCount = 0;
				}
			}
			else if (BatterySys::MinVolt > UV2_CHECK)
			{
				BmsEngine::Time::UV2_time_delay = UV2_CHECK_TIME;                                   		
			}
		}
		else if (BmsEngine::Flag::UV2 == 1)
		{
			if (BatterySys::MinVolt >= UV2_RECOVER)
			{
				BmsEngine::Flag::uv2_deboc = 1;
				if (BmsEngine::Time::UV2_time_delay == 0)
				{
					BmsEngine::Flag::UV2 = 0;
					BmsEngine::Time::UV2_time_delay = UV2_CHECK_TIME;
					BmsEngine::Flag::uv2_deboc = 0;
					p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_UV2;
				}
			}
			else if (BatterySys::MinVolt < UV2_RECOVER)
			{
				BmsEngine::Time::UV2_time_delay = UV2_RECOVER_TIME;
			}
		}
		return (p_bmspara->m_bcuStatus2);
	}
	
	//==============================================================================
	//UV3 Check Function
	//min_vol <= 2.5V and continue 1sec , UV3 event occure
	//System Reset or Clear Flag , UV3 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckVolt_UV3()
	{
		if (BmsEngine::Flag::UV3 == 0)
		{
			if (BatterySys::MinVolt <= UV3_CHECK)
			{
				BmsEngine::Flag::uv3_deboc = 1;
				if (BmsEngine::Time::UV3_time_delay == 0)
				{
					BmsEngine::Flag::UV3 = 1;
					BmsEngine::Flag::uv3_deboc = 0;
					BmsEngine::Time::UV3_time_delay = UV3_CHECK_TIME;	//UV3_RECOVER_TIME;
					p_bmspara->m_bcuStatus3 |= (uint16_t)BCU_STATUS_UV3;
					BmsEngine::FaultCount = 0;
				}
			}
			else if (BatterySys::MinVolt > UV3_CHECK)
			{
				BmsEngine::Time::UV3_time_delay = UV3_CHECK_TIME;                                    		
			}
		}
//		else if (BmsEngine::flag::UV3 == 1)
//		{
//			if (BatterySys::MinVolt >= UV3_RECOVER)
//			{
//				BmsEngine::flag::uv3_deboc = 1;
//				if (BmsEngine::time::UV3_time_delay == 0)
//				{
//					BmsEngine::flag::UV3 = 0;
//					BmsEngine::time::UV3_time_delay = UV3_CHECK_TIME;
//					BmsEngine::flag::uv3_deboc = 0;
//					p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_UV3;
//				}
//			}
//			else if (BatterySys::MinVolt < UV3_RECOVER)
//			{
//				BmsEngine::time::UV3_time_delay = UV3_RECOVER_TIME;
//			}
//		}
		return (p_bmspara->m_bcuStatus3);
	}

	//SortCellTemperature
	void BmsEngine::SortCellTemperature()
	{
		for (uint8_t i = 1; i < (PackConst::MAX_PACK_NUM + 1); i++)//
		{
			BatterySys::pack[i].packMaxTemp = balance1.CellTemp_max(i);
			BatterySys::pack[i].packMinTemp = balance1.CellTemp_min(i);				
		}
	}
	
	//AVGCellTemperature
	int16_t BmsEngine::AVGCellTemperature()
	{					
		for (uint8_t current_ic = 0U; current_ic < PackConst::MAX_PACK_NUM; current_ic++) //
		{		
			int32_t total = 0;//
			for (uint16_t i = 0U; i < PackConst::MAX_CELL_NUM; i++)//
			{
				total += BatterySys::pack[current_ic].cellTemp[i]; 	
			}
			BatterySys::pack[current_ic].cellAVG_Temp = total / PackConst::MAX_CELL_NUM;
			
		}
		
		int32_t total_temp = 0;//
		for (uint16_t i = 0U; i < PackConst::MAX_PACK_NUM; i++)//
		{
			total_temp += BatterySys::pack[i].cellAVG_Temp; //total Sum
		}
		int16_t AVG_Temp = total_temp / PackConst::MAX_PACK_NUM;//
		
		return (AVG_Temp);
	}
	
	//MaxCellTemperature
	int16_t BmsEngine::MaxCellTemperature()
	{
		uint8_t MaxPackTemp_id_NUM = balance1.PackTemp_max();//
		int16_t Max_Temp = BatterySys::pack[MaxPackTemp_id_NUM].packMaxTemp;//
		return (Max_Temp);
	}
	
	//MinCellTemperature
	int16_t BmsEngine::MinCellTemperature()
	{
		uint8_t MinPackTemp_id_NUM = balance1.PackTemp_min();//
		int16_t Min_Temp = BatterySys::pack[MinPackTemp_id_NUM].packMinTemp;//
		return (Min_Temp);
	}
	
	//Sort PACK Ambient Max Temp
	int16_t BmsEngine::SortPack_MAXAmbientTemp()
	{
		int16_t MAX_TEMP = BatterySys::pack[0].packTemp[2];		//Ambient Temp = BatterySys::pack[0].packTemp[2]
		                     
		for (uint8_t i = 0U; i < PackConst::MAX_PACK_NUM; i++)   //PackConst::MAX_PACK_NUM+1
		{
			if (MAX_TEMP <= BatterySys::pack[i].packTemp[2])
			{
				MAX_TEMP = BatterySys::pack[i].packTemp[2];
			}
		}
	
		return (MAX_TEMP);	
	}
	
	//Sort PACK Ambient Min Temp
	int16_t BmsEngine::SortPack_MINAmbientTemp()
	{
		int16_t MIN_TEMP = BatterySys::pack[0].packTemp[2]; //Ambient Temp = BatterySys::pack[0].packTemp[2]
		                     
		for (uint8_t i = 0U; i < PackConst::MAX_PACK_NUM; i++)   //PackConst::MAX_PACK_NUM+1
		{
			if (MIN_TEMP >= BatterySys::pack[i].packTemp[2])
			{
				MIN_TEMP = BatterySys::pack[i].packTemp[2];
			}
		}
	
		return (MIN_TEMP);	
	}

	//==============================================================================
	//OTC1 Check Function- Charge
	//max_temp >= 47'c and continue 5sec,OTC1 event occure
	//max_temp <  42'c and continue 5sec,OTC1 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckTemp_OTC1()
	{
		if ((BatterySys::m_ampReading & 0x80000000) != 0)    //Negative current Charge Mode	
		{
			if (BatterySys::m_ampReading <= CHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::OTC1 == 0)
				{
					if (BatterySys::MaxTemp >= OTC1_CHECK)
					{   
						BmsEngine::Flag::otc1_deboc = 1;
						if (BmsEngine::Time::OTC1_time_delay == 0)
						{
							BmsEngine::Flag::OTC1 = 1;
							BmsEngine::Flag::otc1_deboc = 0;
							BmsEngine::Time::OTC1_time_delay = OTC1_RECOVER_TIME;               
							p_bmspara->m_bcuStatus1 |= (uint16_t)BCU_STATUS_OTC1;      
			
						}
					}
					else if (BatterySys::MaxTemp < OTC1_CHECK)
					{
						BmsEngine::Time::OTC1_time_delay = OTC1_CHECK_TIME;                                             		
					}
				}
				else if (BmsEngine::Flag::OTC1 == 1)
				{
					if (BatterySys::MaxTemp <= OTC1_RECOVER)
					{
						BmsEngine::Flag::otc1_deboc = 1;
						if (BmsEngine::Time::OTC1_time_delay == 0)
						{
							BmsEngine::Flag::OTC1 = 0;
							BmsEngine::Time::OTC1_time_delay = OTC1_CHECK_TIME;
							BmsEngine::Flag::otc1_deboc = 0;
							p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_OTC1;   
//							RLY_K5_FAN = RLY_OFF; //K5
						}
					}
					else if (BatterySys::MaxTemp > OTC1_RECOVER)
					{
						BmsEngine::Time::OTC1_time_delay = OTC1_RECOVER_TIME;
					}
				}
			}
			else
			{
				if (BmsEngine::Flag::OTC1 == 1)
				{
					if (BatterySys::MaxTemp <= OTC1_RECOVER)
					{
						BmsEngine::Flag::otc1_deboc = 1;
						if (BmsEngine::Time::OTC1_time_delay == 0)
						{
							BmsEngine::Flag::OTC1 = 0;
							BmsEngine::Time::OTC1_time_delay = OTC1_CHECK_TIME;
							BmsEngine::Flag::otc1_deboc = 0;
							p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_OTC1;                         
						}
					}
					else if (BatterySys::MaxTemp > OTC1_RECOVER)
					{
						BmsEngine::Time::OTC1_time_delay = OTC1_RECOVER_TIME;
					}
				}
			}
		}
		else
		{
			if (BmsEngine::Flag::OTC1 == 1)
			{
				if (BatterySys::MaxTemp <= OTC1_RECOVER)
				{
					BmsEngine::Flag::otc1_deboc = 1;
					if (BmsEngine::Time::OTC1_time_delay == 0)
					{
						BmsEngine::Flag::OTC1 = 0;
						BmsEngine::Time::OTC1_time_delay = OTC1_CHECK_TIME;
						BmsEngine::Flag::otc1_deboc = 0;
						p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_OTC1;                         
					}
				}
				else if (BatterySys::MaxTemp > OTC1_RECOVER)
				{
					BmsEngine::Time::OTC1_time_delay = OTC1_RECOVER_TIME;
				}
			}
		}
		return (p_bmspara->m_bcuStatus1);
	}
	
	//==============================================================================
	//OTC2 Check Function- Charge
	//max_temp >= 57'c and continue 3sec,OTC2 event occure
	//max_temp <  50'c and continue 5sec,OTC2 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckTemp_OTC2()
	{
		if ((BatterySys::m_ampReading & 0x80000000) != 0)    //Negative current  Charge Mode
		{
			if (BatterySys::m_ampReading <= CHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::OTC2 == 0)
				{
					if (BatterySys::MaxTemp >= OTC2_CHECK)
					{
						BmsEngine::Flag::otc2_deboc = 1;
						if (BmsEngine::Time::OTC2_time_delay == 0)
						{
							BmsEngine::Flag::OTC2 = 1;
							BmsEngine::Flag::otc2_deboc = 0;
							BmsEngine::Time::OTC2_time_delay = OTC2_RECOVER_TIME;               
							p_bmspara->m_bcuStatus2 |= (uint16_t)BCU_STATUS_OTC2;      
							BmsEngine::FaultCount = 0U;
						}
					}
					else if (BatterySys::MaxTemp < OTC2_CHECK)
					{
						BmsEngine::Time::OTC2_time_delay = OTC2_CHECK_TIME;                                            		
					}
				}
				else if (BmsEngine::Flag::OTC2 == 1)
				{
					if (BatterySys::MaxTemp <= OTC2_RECOVER)
					{
						BmsEngine::Flag::otc2_deboc = 1;
						if (BmsEngine::Time::OTC2_time_delay == 0)
						{
							BmsEngine::Flag::OTC2 = 0;
							BmsEngine::Time::OTC2_time_delay = OTC2_CHECK_TIME;
							BmsEngine::Flag::otc2_deboc = 0;
							p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OTC2;                       
						}
					}
					else if (BatterySys::MaxTemp > OTC2_RECOVER)
					{
						BmsEngine::Time::OTC2_time_delay = OTC2_RECOVER_TIME;
					}
				}
			}
			else
			{
				if (BmsEngine::Flag::OTC2 == 1)
				{
					if (BatterySys::MaxTemp <= OTC2_RECOVER)
					{
						BmsEngine::Flag::otc2_deboc = 1;
						if (BmsEngine::Time::OTC2_time_delay == 0)
						{
							BmsEngine::Flag::OTC2 = 0;
							BmsEngine::Time::OTC2_time_delay = OTC2_CHECK_TIME;
							BmsEngine::Flag::otc2_deboc = 0;
							p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OTC2;                        
						}
					}
					else if (BatterySys::MaxTemp > OTC2_RECOVER)
					{
						BmsEngine::Time::OTC2_time_delay = OTC2_RECOVER_TIME;
					}
				}
			}
		}
		else
		{
			if (BmsEngine::Flag::OTC2 == 1)
			{
				if (BatterySys::MaxTemp <= OTC2_RECOVER)
				{
					BmsEngine::Flag::otc2_deboc = 1;
					if (BmsEngine::Time::OTC2_time_delay == 0)
					{
						BmsEngine::Flag::OTC2 = 0;
						BmsEngine::Time::OTC2_time_delay = OTC2_CHECK_TIME;
						BmsEngine::Flag::otc2_deboc = 0;
						p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OTC2;                         
					}
				}
				else if (BatterySys::MaxTemp > OTC2_RECOVER)
				{
					BmsEngine::Time::OTC2_time_delay = OTC2_RECOVER_TIME;
				}
			}
		}
		return (p_bmspara->m_bcuStatus2);
	}
	
	//==============================================================================
	//OTC3 Check Function- Charge
	//max_temp >= 62'c and continue 1sec, OTC3 event occure
	//System Reset or Clear Flag, OTC3 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckTemp_OTC3()
	{                                           	
		if ((BatterySys::m_ampReading & 0x80000000) != 0)    //Negative current  Charge Mode
		{
			if (BatterySys::m_ampReading <= CHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::OTC3 == 0)
				{
					if (BatterySys::MaxTemp >= OTC3_CHECK)
					{
						BmsEngine::Flag::otc3_deboc = 1;
						if (BmsEngine::Time::OTC3_time_delay == 0)
						{
							BmsEngine::Flag::OTC3 = 1;
							BmsEngine::Flag::otc3_deboc = 0;
							BmsEngine::Time::OTC3_time_delay = OTC3_CHECK_TIME;	//OTC3_RECOVER_TIME;             
							p_bmspara->m_bcuStatus3 |= (uint16_t)BCU_STATUS_OTC3;      
							BmsEngine::FaultCount = 0U;
						}
					}
					else if (BatterySys::MaxTemp < OTC3_CHECK)
					{
						BmsEngine::Time::OTC3_time_delay = OTC3_CHECK_TIME;                                           		
					}
				}
//				else if (BmsEngine::flag::OTC3 == 1)
//				{
//					if (BatterySys::MaxTemp <= OTC3_RECOVER)
//					{
//						BmsEngine::flag::otc3_deboc = 1;
//						if (BmsEngine::time::OTC3_time_delay == 0)
//						{
//							BmsEngine::flag::OTC3 = 0;
//							BmsEngine::time::OTC3_time_delay = OTC3_CHECK_TIME;
//							BmsEngine::flag::otc3_deboc = 0;
//							p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OTC3;                      
//						}
//					}
//					else if (BatterySys::MaxTemp > OTC3_RECOVER)
//					{
//						BmsEngine::time::OTC3_time_delay = OTC3_RECOVER_TIME;
//					}
//				}
			}
//			else
//			{
//				if (BmsEngine::flag::OTC3 == 1)
//				{
//					if (BatterySys::MaxTemp <= OTC3_RECOVER)
//					{
//						BmsEngine::flag::otc3_deboc = 1;
//						if (BmsEngine::time::OTC3_time_delay == 0)
//						{
//							BmsEngine::flag::OTC3 = 0;
//							BmsEngine::time::OTC3_time_delay = OTC3_CHECK_TIME;
//							BmsEngine::flag::otc3_deboc = 0;
//							p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OTC3;                       
//						}
//					}
//					else if (BatterySys::MaxTemp > OTC3_RECOVER)
//					{
//						BmsEngine::time::OTC3_time_delay = OTC3_RECOVER_TIME;
//					}
//				}
//			}
		}
//		else
//		{
//			if (BmsEngine::flag::OTC3 == 1)
//			{
//				if (BatterySys::MaxTemp <= OTC3_RECOVER)
//				{
//					BmsEngine::flag::otc3_deboc = 1;
//					if (BmsEngine::time::OTC3_time_delay == 0)
//					{
//						BmsEngine::flag::OTC3 = 0;
//						BmsEngine::time::OTC3_time_delay = OTC3_CHECK_TIME;
//						BmsEngine::flag::otc3_deboc = 0;
//						p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OTC3;                        
//					}
//				}
//				else if (BatterySys::MaxTemp > OTC3_RECOVER)
//				{
//					BmsEngine::time::OTC3_time_delay = OTC3_RECOVER_TIME;
//				}
//			}
//		}
		return (p_bmspara->m_bcuStatus3);
	}
	
	//==============================================================================
	//OTD1 Check Function- Discharge
	//max_temp >= 47'c and continue 5sec,OTD1 event occure
	//max_temp <  42'c and continue 5sec,OTD1 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckTemp_OTD1()
	{
		if ((BatterySys::m_ampReading & 0x80000000) == 0)    //Positive current  Discharge Mode
		{
			if (BatterySys::m_ampReading >= DISCHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::OTD1 == 0)
				{
					if (BatterySys::MaxTemp >= OTD1_CHECK)
					{
						BmsEngine::Flag::otd1_deboc = 1;
						if (BmsEngine::Time::OTD1_time_delay == 0)
						{
							BmsEngine::Flag::OTD1 = 1;
							BmsEngine::Flag::otd1_deboc = 0;
							BmsEngine::Time::OTD1_time_delay = OTD1_RECOVER_TIME;                
							p_bmspara->m_bcuStatus1 |= (uint16_t)BCU_STATUS_OTD1;     
						
						}
					}
					else if (BatterySys::MaxTemp < OTD1_CHECK)
					{
						BmsEngine::Time::OTD1_time_delay = OTD1_CHECK_TIME;                                              		
					}
				}
				else if (BmsEngine::Flag::OTD1 == 1)
				{
					if (BatterySys::MaxTemp <= OTD1_RECOVER)
					{
						BmsEngine::Flag::otd1_deboc = 1;
						if (BmsEngine::Time::OTD1_time_delay == 0)
						{
							BmsEngine::Flag::OTD1 = 0;
							BmsEngine::Time::OTD1_time_delay = OTD1_CHECK_TIME;
							BmsEngine::Flag::otd1_deboc = 0;
							p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_OTD1;                         
						}
					}
					else if (BatterySys::MaxTemp > OTD1_RECOVER)
					{
						BmsEngine::Time::OTD1_time_delay = OTD1_RECOVER_TIME;
					}
				}
			}
			else
			{
				if (BmsEngine::Flag::OTD1 == 1)
				{
					if (BatterySys::MaxTemp <= OTD1_RECOVER)
					{
						BmsEngine::Flag::otd1_deboc = 1;
						if (BmsEngine::Time::OTD1_time_delay == 0)
						{
							BmsEngine::Flag::OTD1 = 0;
							BmsEngine::Time::OTD1_time_delay = OTD1_CHECK_TIME;
							BmsEngine::Flag::otd1_deboc = 0;
							p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_OTD1;                          
						}
					}
					else if (BatterySys::MaxTemp > OTD1_RECOVER)
					{
						BmsEngine::Time::OTD1_time_delay = OTD1_RECOVER_TIME;
					}
				}
			}
		}
		else
		{
			if (BmsEngine::Flag::OTD1 == 1)
			{
				if (BatterySys::MaxTemp <= OTD1_RECOVER)
				{
					BmsEngine::Flag::otd1_deboc = 1;
					if (BmsEngine::Time::OTD1_time_delay == 0)
					{
						BmsEngine::Flag::OTD1 = 0;
						BmsEngine::Time::OTD1_time_delay = OTD1_CHECK_TIME;
						BmsEngine::Flag::otd1_deboc = 0;
						p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_OTD1;                          
					}
				}
				else if (BatterySys::MaxTemp > OTD1_RECOVER)
				{
					BmsEngine::Time::OTD1_time_delay = OTD1_RECOVER_TIME;
				}
			}
		}

		return (p_bmspara->m_bcuStatus1);
	}
	
	//==============================================================================
	//OTD2 Check Function- Discharge
	//max_temp >= 57'c and continue 3sec,OTD2 event occure
	//max_temp <  50'c and continue 5sec,OTD2 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckTemp_OTD2()
	{
		if ((BatterySys::m_ampReading & 0x80000000) == 0)    //Positive current  Discharge Mode
		{
			if (BatterySys::m_ampReading >= DISCHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::OTD2 == 0)
				{
					if (BatterySys::MaxTemp >= OTD2_CHECK)
					{
						BmsEngine::Flag::otd2_deboc = 1;
						if (BmsEngine::Time::OTD2_time_delay == 0)
						{
							BmsEngine::Flag::OTD2 = 1;
							BmsEngine::Flag::otd2_deboc = 0;
							BmsEngine::Time::OTD2_time_delay = OTD2_RECOVER_TIME;                    
							p_bmspara->m_bcuStatus2 |= (uint16_t)BCU_STATUS_OTD2;    
							BmsEngine::FaultCount = 0U;
						}
					}
					else if (BatterySys::MaxTemp < OTD2_CHECK)
					{
						BmsEngine::Time::OTD2_time_delay = OTD2_CHECK_TIME;                                               		
					}
				}
				else if (BmsEngine::Flag::OTD2 == 1)
				{
					if (BatterySys::MaxTemp <= OTD2_RECOVER)
					{
						BmsEngine::Flag::otd2_deboc = 1;
						if (BmsEngine::Time::OTD2_time_delay == 0)
						{
							BmsEngine::Flag::OTD2 = 0;
							BmsEngine::Time::OTD2_time_delay = OTD2_CHECK_TIME;
							BmsEngine::Flag::otd2_deboc = 0;
							p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OTD2;                          
						}
					}
					else if (BatterySys::MaxTemp > OTD2_RECOVER)
					{
						BmsEngine::Time::OTD2_time_delay = OTD2_RECOVER_TIME;
					}
				}
			}
			else
			{
				if (BmsEngine::Flag::OTD2 == 1)
				{
					if (BatterySys::MaxTemp <= OTD2_RECOVER)
					{
						BmsEngine::Flag::otd2_deboc = 1;
						if (BmsEngine::Time::OTD2_time_delay == 0)
						{
							BmsEngine::Flag::OTD2 = 0;
							BmsEngine::Time::OTD2_time_delay = OTD2_CHECK_TIME;
							BmsEngine::Flag::otd2_deboc = 0;
							p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OTD2;                           
						}
					}
					else if (BatterySys::MaxTemp > OTD2_RECOVER)
					{
						BmsEngine::Time::OTD2_time_delay = OTD2_RECOVER_TIME;
					}
				}
			}
		}
		else
		{
			if (BmsEngine::Flag::OTD2 == 1)
			{
				if (BatterySys::MaxTemp <= OTD2_RECOVER)
				{
					BmsEngine::Flag::otd2_deboc = 1;
					if (BmsEngine::Time::OTD2_time_delay == 0)
					{
						BmsEngine::Flag::OTD2 = 0;
						BmsEngine::Time::OTD2_time_delay = OTD2_CHECK_TIME;
						BmsEngine::Flag::otd2_deboc = 0;
						p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OTD2;                            
					}
				}
				else if (BatterySys::MaxTemp > OTD2_RECOVER)
				{
					BmsEngine::Time::OTD2_time_delay = OTD2_RECOVER_TIME;
				}
			}
		}
		return (p_bmspara->m_bcuStatus2);
	}
	
	//==============================================================================
	//OTD3 Check Function- Discharge
	//max_temp >= 62'c and continue 1sec, OTD3 event occure
	//System Reset or Clear Flag, OTD3 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckTemp_OTD3()
	{
		if ((BatterySys::m_ampReading & 0x80000000) == 0)    //Positive current  Discharge Mode
		{
			if (BatterySys::m_ampReading >= DISCHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::OTD3 == 0)
				{
					if (BatterySys::MaxTemp >= OTD3_CHECK)
					{
						BmsEngine::Flag::otd3_deboc = 1;
						if (BmsEngine::Time::OTD3_time_delay == 0)
						{
							BmsEngine::Flag::OTD3 = 1;
							BmsEngine::Flag::otd3_deboc = 0;
							BmsEngine::Time::OTD3_time_delay = OTD3_CHECK_TIME;	//OTD3_RECOVER_TIME;                        
							p_bmspara->m_bcuStatus3 |= (uint16_t)BCU_STATUS_OTD3;      
							BmsEngine::FaultCount = 0U;
						}
					}
					else if (BatterySys::MaxTemp < OTD3_CHECK)
					{
						BmsEngine::Time::OTD3_time_delay = OTD3_CHECK_TIME;                                                		
					}
				}
//				else if (BmsEngine::flag::OTD3 == 1)
//				{
//					if (BatterySys::MaxTemp <= OTD3_RECOVER)
//					{
//						BmsEngine::flag::otd3_deboc = 1;
//						if (BmsEngine::time::OTD3_time_delay == 0)
//						{
//							BmsEngine::flag::OTD3 = 0;
//							BmsEngine::time::OTD3_time_delay = OTD3_CHECK_TIME;
//							BmsEngine::flag::otd3_deboc = 0;
//							p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OTD3;                          
//						}
//					}
//					else if (BatterySys::MaxTemp > OTD3_RECOVER)
//					{
//						BmsEngine::time::OTD3_time_delay = OTD3_RECOVER_TIME;
//					}
//				}
			}
//			else
//			{
//				if (BmsEngine::flag::OTD3 == 1)
//				{
//					if (BatterySys::MaxTemp <= OTD3_RECOVER)
//					{
//						BmsEngine::flag::otd3_deboc = 1;
//						if (BmsEngine::time::OTD3_time_delay == 0)
//						{
//							BmsEngine::flag::OTD3 = 0;
//							BmsEngine::time::OTD3_time_delay = OTD3_CHECK_TIME;
//							BmsEngine::flag::otd3_deboc = 0;
//							p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OTD3;                            
//						}
//					}
//					else if (BatterySys::MaxTemp > OTD3_RECOVER)
//					{
//						BmsEngine::time::OTD3_time_delay = OTD3_RECOVER_TIME;
//					}
//				}
//			}
		}
//		else
//		{
//			if (BmsEngine::flag::OTD3 == 1)
//			{
//				if (BatterySys::MaxTemp <= OTD3_RECOVER)
//				{
//					BmsEngine::flag::otd3_deboc = 1;
//					if (BmsEngine::time::OTD3_time_delay == 0)
//					{
//						BmsEngine::flag::OTD3 = 0;
//						BmsEngine::time::OTD3_time_delay = OTD3_CHECK_TIME;
//						BmsEngine::flag::otd3_deboc = 0;
//						p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OTD3;                            
//					}
//				}
//				else if (BatterySys::MaxTemp > OTD3_RECOVER)
//				{
//					BmsEngine::time::OTD3_time_delay = OTD3_RECOVER_TIME;
//				}
//			}
//		}
		return (p_bmspara->m_bcuStatus3);
	}
	
	//==============================================================================
	//UTC1 Check Function - Charge
	//min_temp <= 5'c and continue 5sec,UTC1 event occure
	//min_temp >  10'c and continue 5sec,UTC1 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckTemp_UTC1()
	{
		if ((BatterySys::m_ampReading & 0x80000000) != 0)    //Negative current Charge Mode		
		{
			if (BatterySys::m_ampReading <= CHARGE_CURRENT_THRESHOULD)
			{
				if (BmsEngine::Flag::UTC1 == 0)
				{
					if (BatterySys::MinTemp <= UTC1_CHECK)
					{ 
						BmsEngine::Flag::utc1_deboc = 1;
						if (BmsEngine::Time::UTC1_time_delay == 0)
						{
							BmsEngine::Flag::UTC1 = 1;
							BmsEngine::Flag::utc1_deboc = 0;
							BmsEngine::Time::UTC1_time_delay = UTC1_RECOVER_TIME;                  
							p_bmspara->m_bcuStatus1 |= (uint16_t)BCU_STATUS_UTC1;         
						
						}
					}
					else if (BatterySys::MinTemp > UTC1_CHECK)
					{
						BmsEngine::Time::UTC1_time_delay = UTC1_CHECK_TIME;                                            		
					}
				}
				else if (BmsEngine::Flag::UTC1 == 1)
				{
					if (BatterySys::MinTemp >= UTC1_RECOVER)
					{
						BmsEngine::Flag::utc1_deboc = 1;
						if (BmsEngine::Time::UTC1_time_delay == 0)
						{
							BmsEngine::Flag::UTC1 = 0;
							BmsEngine::Time::UTC1_time_delay = UTC1_CHECK_TIME;
							BmsEngine::Flag::utc1_deboc = 0;
							p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_UTC1;                       
						}
					}
					else if (BatterySys::MinTemp < UTC1_RECOVER)
					{
						BmsEngine::Time::UTC1_time_delay = UTC1_RECOVER_TIME;
					}
				}
			}
			else
			{
				if (BmsEngine::Flag::UTC1 == 1)
				{
					if (BatterySys::MinTemp >= UTC1_RECOVER)
					{
						BmsEngine::Flag::utc1_deboc = 1;
						if (BmsEngine::Time::UTC1_time_delay == 0)
						{
							BmsEngine::Flag::UTC1 = 0;
							BmsEngine::Time::UTC1_time_delay = UTC1_CHECK_TIME;
							BmsEngine::Flag::utc1_deboc = 0;
							p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_UTC1;                        
						}
					}
					else if (BatterySys::MinTemp < UTC1_RECOVER)
					{
						BmsEngine::Time::UTC1_time_delay = UTC1_RECOVER_TIME;
					}
				}
			}
		}
		else 
		{			
			if (BmsEngine::Flag::UTC1 == 1)
			{
				if (BatterySys::MinTemp >= UTC1_RECOVER)
				{
					BmsEngine::Flag::utc1_deboc = 1;
					if (BmsEngine::Time::UTC1_time_delay == 0)
					{
						BmsEngine::Flag::UTC1 = 0;
						BmsEngine::Time::UTC1_time_delay = UTC1_CHECK_TIME;
						BmsEngine::Flag::utc1_deboc = 0;
						p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_UTC1;                        
					}
				}
				else if (BatterySys::MinTemp < UTC1_RECOVER)
				{
					BmsEngine::Time::UTC1_time_delay = UTC1_RECOVER_TIME;
				}
			}	
		}
		return (p_bmspara->m_bcuStatus1);
	}
	
	//==============================================================================
	//UTC2 Check Function - Charge
	//min_temp <= 0'c and continue 3sec,UTC2 event occure
	//min_temp >  5'c and continue 5sec,UTC2 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckTemp_UTC2()
	{
		if ((BatterySys::m_ampReading & 0x80000000) != 0)    //Negative current Charge Mode	
		{
			if (BatterySys::m_ampReading <= CHARGE_CURRENT_THRESHOULD)
			{
				if (BmsEngine::Flag::UTC2 == 0)
				{
					if (BatterySys::MinTemp <= UTC2_CHECK)
					{
						BmsEngine::Flag::utc2_deboc = 1;
						if (BmsEngine::Time::UTC2_time_delay == 0)
						{
							BmsEngine::Flag::UTC2 = 1;
							BmsEngine::Flag::utc2_deboc = 0;
							BmsEngine::Time::UTC2_time_delay = UTC2_RECOVER_TIME;                  
							p_bmspara->m_bcuStatus2 |= (uint16_t)BCU_STATUS_UTC2;    
							BmsEngine::FaultCount = 0U;
						}
					}
					else if (BatterySys::MinTemp > UTC2_CHECK)
					{
						BmsEngine::Time::UTC2_time_delay = UTC2_CHECK_TIME;                                           		
					}
				}
				else if (BmsEngine::Flag::UTC2 == 1)
				{
					if (BatterySys::MinTemp >= UTC2_RECOVER)
					{
						BmsEngine::Flag::utc2_deboc = 1;
						if (BmsEngine::Time::UTC2_time_delay == 0)
						{
							BmsEngine::Flag::UTC2 = 0;
							BmsEngine::Time::UTC2_time_delay = UTC2_CHECK_TIME;
							BmsEngine::Flag::utc2_deboc = 0;
							p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_UTC2;                      
						}
					}
					else if (BatterySys::MinTemp < UTC2_RECOVER)
					{
						BmsEngine::Time::UTC2_time_delay = UTC2_RECOVER_TIME;
					}
				}
			}
			else
			{
				if (BmsEngine::Flag::UTC2 == 1)
				{
					if (BatterySys::MinTemp >= UTC2_RECOVER)
					{
						BmsEngine::Flag::utc2_deboc = 1;
						if (BmsEngine::Time::UTC2_time_delay == 0)
						{
							BmsEngine::Flag::UTC2 = 0;
							BmsEngine::Time::UTC2_time_delay = UTC2_CHECK_TIME;
							BmsEngine::Flag::utc2_deboc = 0;
							p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_UTC2;                       
						}
					}
					else if (BatterySys::MinTemp < UTC2_RECOVER)
					{
						BmsEngine::Time::UTC2_time_delay = UTC2_RECOVER_TIME;
					}
				}
			}
		}
		else
		{			
			if (BmsEngine::Flag::UTC2 == 1)
			{
				if (BatterySys::MinTemp >= UTC2_RECOVER)
				{
					BmsEngine::Flag::utc2_deboc = 1;
					if (BmsEngine::Time::UTC2_time_delay == 0)
					{
						BmsEngine::Flag::UTC2 = 0;
						BmsEngine::Time::UTC2_time_delay = UTC2_CHECK_TIME;
						BmsEngine::Flag::utc2_deboc = 0;
						p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_UTC2;                       
					}
				}
				else if (BatterySys::MinTemp < UTC2_RECOVER)
				{
					BmsEngine::Time::UTC2_time_delay = UTC2_RECOVER_TIME;
				}
			}	
		}
		return (p_bmspara->m_bcuStatus2);
	}
	
	//==============================================================================
	//UTC3 Check Function - Charge
	//min_temp <= -5'c and continue 1sec, UTC3 event occure
	//System Reset or Clear Flag, UTC3 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckTemp_UTC3()
	{
		if ((BatterySys::m_ampReading & 0x80000000) != 0)    //Negative current Charge Mode	
		{
			if (BatterySys::m_ampReading <= CHARGE_CURRENT_THRESHOULD)
			{
				if (BmsEngine::Flag::UTC3 == 0)
				{
					if (BatterySys::MinTemp <= UTC3_CHECK)
					{
						BmsEngine::Flag::utc3_deboc = 1;
						if (BmsEngine::Time::UTC3_time_delay == 0)
						{
							BmsEngine::Flag::UTC3 = 1;
							BmsEngine::Flag::utc3_deboc = 0;
							BmsEngine::Time::UTC3_time_delay = UTC3_CHECK_TIME;	//UTC3_RECOVER_TIME;             
							p_bmspara->m_bcuStatus3 |= (uint16_t)BCU_STATUS_UTC3;  
							BmsEngine::FaultCount = 0U;
						}
					}
					else if (BatterySys::MinTemp > UTC3_CHECK)
					{
						BmsEngine::Time::UTC3_time_delay = UTC3_CHECK_TIME;                                          		
					}
				}
//				else if (BmsEngine::flag::UTC3 == 1)
//				{
//					if (BatterySys::MinTemp >= UTC3_RECOVER)
//					{
//						BmsEngine::flag::utc3_deboc = 1;
//						if (BmsEngine::time::UTC3_time_delay == 0)
//						{
//							BmsEngine::flag::UTC3 = 0;
//							BmsEngine::time::UTC3_time_delay = UTC3_CHECK_TIME;
//							BmsEngine::flag::utc3_deboc = 0;
//							p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_UTC3;                     
//						}
//					}
//					else if (BatterySys::MinTemp < UTC3_RECOVER)
//					{
//						BmsEngine::time::UTC3_time_delay = UTC3_RECOVER_TIME;
//					}
//				}
			}
//			else
//			{
//				if (BmsEngine::flag::UTC3 == 1)
//				{
//					if (BatterySys::MinTemp >= UTC3_RECOVER)
//					{
//						BmsEngine::flag::utc3_deboc = 1;
//						if (BmsEngine::time::UTC3_time_delay == 0)
//						{
//							BmsEngine::flag::UTC3 = 0;
//							BmsEngine::time::UTC3_time_delay = UTC3_CHECK_TIME;
//							BmsEngine::flag::utc3_deboc = 0;
//							p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_UTC3;                     
//						}
//					}
//					else if (BatterySys::MinTemp < UTC3_RECOVER)
//					{
//						BmsEngine::time::UTC3_time_delay = UTC3_RECOVER_TIME;
//					}
//				}
//			}
		}
//		else 
//		{			
//			if (BmsEngine::flag::UTC3 == 1)
//			{
//				if (BatterySys::MinTemp >= UTC3_RECOVER)
//				{
//					BmsEngine::flag::utc3_deboc = 1;
//					if (BmsEngine::time::UTC3_time_delay == 0)
//					{
//						BmsEngine::flag::UTC3 = 0;
//						BmsEngine::time::UTC3_time_delay = UTC3_CHECK_TIME;
//						BmsEngine::flag::utc3_deboc = 0;
//						p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_UTC3;                      
//					}
//				}
//				else if (BatterySys::MinTemp < UTC3_RECOVER)
//				{
//					BmsEngine::time::UTC3_time_delay = UTC3_RECOVER_TIME;
//				}
//			}	
//		}
		return (p_bmspara->m_bcuStatus3);
	}
	
	//==============================================================================
	//UTD1 Check Function - Discharge
	//min_temp <= -10'c and continue 5sec,UTD1 event occure
	//min_temp >  0'c and continue 5sec,UTD1 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckTemp_UTD1()
	{
		if ((BatterySys::m_ampReading & 0x80000000) == 0)    //Positive current  Discharge Mode	
		{
			if (BatterySys::m_ampReading >= DISCHARGE_CURRENT_THRESHOULD)
			{
				if (BmsEngine::Flag::UTD1 == 0)
				{
					if (BatterySys::MinTemp <= UTD1_CHECK)
					{
						BmsEngine::Flag::utd1_deboc = 1;
						if (BmsEngine::Time::UTD1_time_delay == 0)
						{
							BmsEngine::Flag::UTD1 = 1;
							BmsEngine::Flag::utd1_deboc = 0;
							BmsEngine::Time::UTD1_time_delay = UTD1_RECOVER_TIME;               
							p_bmspara->m_bcuStatus1 |= (uint16_t)BCU_STATUS_UTD1;                       
						}
					}
					else if (BatterySys::MinTemp > UTD1_CHECK)
					{
						BmsEngine::Time::UTD1_time_delay = UTD1_CHECK_TIME;                                             		
					}
				}
				else if (BmsEngine::Flag::UTD1 == 1)
				{
					if (BatterySys::MinTemp >= UTD1_RECOVER)
					{
						BmsEngine::Flag::utd1_deboc = 1;
						if (BmsEngine::Time::UTD1_time_delay == 0)
						{
							BmsEngine::Flag::UTD1 = 0;
							BmsEngine::Time::UTD1_time_delay = UTD1_CHECK_TIME;
							BmsEngine::Flag::utd1_deboc = 0;
							p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_UTD1;                        
						}
					}
					else if (BatterySys::MinTemp < UTD1_RECOVER)
					{
						BmsEngine::Time::UTD1_time_delay = UTD1_RECOVER_TIME;
					}
				}
			}
			else
			{
				if (BmsEngine::Flag::UTD1 == 1)
				{
					if (BatterySys::MinTemp >= UTD1_RECOVER)
					{
						BmsEngine::Flag::utd1_deboc = 1;
						if (BmsEngine::Time::UTD1_time_delay == 0)
						{
							BmsEngine::Flag::UTD1 = 0;
							BmsEngine::Time::UTD1_time_delay = UTD1_CHECK_TIME;
							BmsEngine::Flag::utd1_deboc = 0;
							p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_UTD1;                         
						}
					}
					else if (BatterySys::MinTemp < UTD1_RECOVER)
					{
						BmsEngine::Time::UTD1_time_delay = UTD1_RECOVER_TIME;
					}
				}
			}
		}
		else 
		{			
			if (BmsEngine::Flag::UTD1 == 1)
			{
				if (BatterySys::MinTemp >= UTD1_RECOVER)
				{
					BmsEngine::Flag::utd1_deboc = 1;
					if (BmsEngine::Time::UTD1_time_delay == 0)
					{
						BmsEngine::Flag::UTD1 = 0;
						BmsEngine::Time::UTD1_time_delay = UTD1_CHECK_TIME;
						BmsEngine::Flag::utd1_deboc = 0;
						p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_UTD1;                        
					}
				}
				else if (BatterySys::MinTemp < UTD1_RECOVER)
				{
					BmsEngine::Time::UTD1_time_delay = UTD1_RECOVER_TIME;
				}
			}	
		}
		return (p_bmspara->m_bcuStatus1);
	}
	
	//==============================================================================
	//UTD2 Check Function - Discharge
	//min_temp <= -20'c and continue 3sec,UTD2 event occure
	//min_temp >  -15'c and continue 5sec,UTD2 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckTemp_UTD2()
	{
		if ((BatterySys::m_ampReading & 0x80000000) == 0)    //Positive current  Discharge Mode
		{
			if (BatterySys::m_ampReading >= DISCHARGE_CURRENT_THRESHOULD)
			{
				if (BmsEngine::Flag::UTD2 == 0)
				{
					if (BatterySys::MinTemp <= UTD2_CHECK)
					{
						BmsEngine::Flag::utd2_deboc = 1;
						if (BmsEngine::Time::UTD2_time_delay == 0)
						{
							BmsEngine::Flag::UTD2 = 1;
							BmsEngine::Flag::utd2_deboc = 0;
							BmsEngine::Time::UTD2_time_delay = UTD2_RECOVER_TIME;                     
							p_bmspara->m_bcuStatus2 |= (uint16_t)BCU_STATUS_UTD2;  
							BmsEngine::FaultCount = 0U;
						}
					}
					else if (BatterySys::MinTemp > UTD2_CHECK)
					{
						BmsEngine::Time::UTD2_time_delay = UTD2_CHECK_TIME;                                              		
					}
				}
				else if (BmsEngine::Flag::UTD2 == 1)
				{
					if (BatterySys::MinTemp >= UTD2_RECOVER)
					{
						BmsEngine::Flag::utd2_deboc = 1;
						if (BmsEngine::Time::UTD2_time_delay == 0)
						{
							BmsEngine::Flag::UTD2 = 0;
							BmsEngine::Time::UTD2_time_delay = UTD2_CHECK_TIME;
							BmsEngine::Flag::utd2_deboc = 0;
							p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_UTD2;                         
						}
					}
					else if (BatterySys::MinTemp < UTD2_RECOVER)
					{
						BmsEngine::Time::UTD2_time_delay = UTD2_RECOVER_TIME;
					}
				}
			}
			else
			{
				if (BmsEngine::Flag::UTD2 == 1)
				{
					if (BatterySys::MinTemp >= UTD2_RECOVER)
					{
						BmsEngine::Flag::utd2_deboc = 1;
						if (BmsEngine::Time::UTD2_time_delay == 0)
						{
							BmsEngine::Flag::UTD2 = 0;
							BmsEngine::Time::UTD2_time_delay = UTD2_CHECK_TIME;
							BmsEngine::Flag::utd2_deboc = 0;
							p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_UTD2;                        
						}
					}
					else if (BatterySys::MinTemp < UTD2_RECOVER)
					{
						BmsEngine::Time::UTD2_time_delay = UTD2_RECOVER_TIME;
					}
				}
			}
		}
		else 
		{			
			if (BmsEngine::Flag::UTD2 == 1)
			{
				if (BatterySys::MinTemp >= UTD2_RECOVER)
				{
					BmsEngine::Flag::utd2_deboc = 1;
					if (BmsEngine::Time::UTD2_time_delay == 0)
					{
						BmsEngine::Flag::UTD2 = 0;
						BmsEngine::Time::UTD2_time_delay = UTD2_CHECK_TIME;
						BmsEngine::Flag::utd2_deboc = 0;
						p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_UTD2;                         
					}
				}
				else if (BatterySys::MinTemp < UTD2_RECOVER)
				{
					BmsEngine::Time::UTD2_time_delay = UTD2_RECOVER_TIME;
				}
			}	
		}
		return (p_bmspara->m_bcuStatus2);
	}
	
	//==============================================================================
	//UTD3 Check Function - Discharge
	//min_temp <= -25'c and continue 1sec, UTD3 event occure
	//System Reset or Clear Flag, UTD3 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckTemp_UTD3()
	{
		if ((BatterySys::m_ampReading & 0x80000000) == 0)    //Positive current  Discharge Mode
		{
			if (BatterySys::m_ampReading >= DISCHARGE_CURRENT_THRESHOULD)
			{
				if (BmsEngine::Flag::UTD3 == 0)
				{
					if (BatterySys::MinTemp <= UTD3_CHECK)
					{
						BmsEngine::Flag::utd3_deboc = 1;
						if (BmsEngine::Time::UTD3_time_delay == 0)
						{
							BmsEngine::Flag::UTD3 = 1;
							BmsEngine::Flag::utd3_deboc = 0;
							BmsEngine::Time::UTD3_time_delay = UTD3_CHECK_TIME;	//UTD3_RECOVER_TIME;                    
							p_bmspara->m_bcuStatus3 |= (uint16_t)BCU_STATUS_UTD3;   
							BmsEngine::FaultCount = 0U;
						}
					}
					else if (BatterySys::MinTemp > UTD3_CHECK)
					{
						BmsEngine::Time::UTD3_time_delay = UTD3_CHECK_TIME;                                               	
					}
				}
//				else if (BmsEngine::flag::UTD3 == 1)
//				{
//					if (BatterySys::MinTemp >= UTD3_RECOVER)
//					{
//						BmsEngine::flag::utd3_deboc = 1;
//						if (BmsEngine::time::UTD3_time_delay == 0)
//						{
//							BmsEngine::flag::UTD3 = 0;
//							BmsEngine::time::UTD3_time_delay = UTD3_CHECK_TIME;
//							BmsEngine::flag::utd3_deboc = 0;
//							p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_UTD3;                          
//						}
//					}
//					else if (BatterySys::MinTemp < UTD3_RECOVER)
//					{
//						BmsEngine::time::UTD3_time_delay = UTD3_RECOVER_TIME;
//					}
//				}
			}
//			else
//			{
//				if (BmsEngine::flag::UTD3 == 1)
//				{
//					if (BatterySys::MinTemp >= UTD3_RECOVER)
//					{
//						BmsEngine::flag::utd3_deboc = 1;
//						if (BmsEngine::time::UTD3_time_delay == 0)
//						{
//							BmsEngine::flag::UTD3 = 0;
//							BmsEngine::time::UTD3_time_delay = UTD3_CHECK_TIME;
//							BmsEngine::flag::utd3_deboc = 0;
//							p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_UTD3;                          
//						}
//					}
//					else if (BatterySys::MinTemp < UTD3_RECOVER)
//					{
//						BmsEngine::time::UTD3_time_delay = UTD3_RECOVER_TIME;
//					}
//				}
//			}
		}
//		else 
//		{			
//			if (BmsEngine::flag::UTD3 == 1)
//			{
//				if (BatterySys::MinTemp >= UTD3_RECOVER)
//				{
//					BmsEngine::flag::utd3_deboc = 1;
//					if (BmsEngine::time::UTD3_time_delay == 0)
//					{
//						BmsEngine::flag::UTD3 = 0;
//						BmsEngine::time::UTD3_time_delay = UTD3_CHECK_TIME;
//						BmsEngine::flag::utd3_deboc = 0;
//						p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_UTD3;                          
//					}
//				}
//				else if (BatterySys::MinTemp < UTD3_RECOVER)
//				{
//					BmsEngine::time::UTD3_time_delay = UTD3_RECOVER_TIME;
//				}
//			}	
//		}
		return (p_bmspara->m_bcuStatus3);
	}
	
	//==============================================================================
	//OCC1 Charge Check Function
	//Charge Current >= 180 A and continue 5 sec  , OCC1  event occure
	//Charge Current <  170 A and continue 5 sec , OCC1  event recover
	//==============================================================================
	uint16_t BmsEngine::CheckCurrent_OCC1()
	{
		if ((BatterySys::m_ampReading & 0x80000000) != 0)    //Negative current  Charge Mode
		{	
			if (BmsEngine::Flag::OCC1 == 0)
			{
				if (BatterySys::m_ampReading  <= OCC1_CHECK)    //AmpReading_m
				{
					BmsEngine::Flag::occ1_deboc = 1;
					if (BmsEngine::Time::OCC1_time_delay == 0)
					{
						BmsEngine::Flag::OCC1 = 1;
						BmsEngine::Flag::occ1_deboc = 0;
						BmsEngine::Time::OCC1_time_delay = OCC1_RECOVER_TIME;             
						p_bmspara->m_bcuStatus1 |= (uint16_t)BCU_STATUS_OCC1;                          
					}
				}
				else if (BatterySys::m_ampReading > OCC1_CHECK)
				{						
					BmsEngine::Time::OCC1_time_delay = OCC1_CHECK_TIME;   
				} 					
			}
			else if (BmsEngine::Flag::OCC1 == 1)
			{ 	  	
				if (BatterySys::m_ampReading  > OCC1_RECOVER)
				{
					BmsEngine::Flag::occ1_deboc = 1;
					if (BmsEngine::Time::OCC1_time_delay == 0)
					{
						BmsEngine::Flag::OCC1 = 0;
						BmsEngine::Flag::occ1_deboc = 0;
						BmsEngine::Time::OCC1_time_delay = OCC1_CHECK_TIME; 
						p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_OCC1;                          
					}
				}
				else if (BatterySys::m_ampReading <= OCC1_RECOVER)
				{
					BmsEngine::Time::OCC1_time_delay = OCC1_RECOVER_TIME;   			
				} 
					
			}  
		}
		else
		{   
			if (BmsEngine::Flag::OCC1 == 1)
			{ 			 
				BmsEngine::Flag::occ1_deboc = 1;
				if (BmsEngine::Time::OCC1_time_delay == 0)
				{
					BmsEngine::Flag::OCC1 = 0;
					BmsEngine::Flag::occ1_deboc = 0;
					BmsEngine::Time::OCC1_time_delay = OCC1_CHECK_TIME; 
					p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_OCC1;                          
				}	
			}
			else if (BmsEngine::Flag::OCC1 == 0)
			{
				BmsEngine::Flag::occ1_deboc = 0;
				BmsEngine::Time::OCC1_time_delay = OCC1_CHECK_TIME; 
			}  
		}
		return (p_bmspara->m_bcuStatus1);
	}
	
	//==============================================================================
	//OCC2 Charge Check Function
	//Charge Current >= 190 A and continue 3 sec  , OCC2  event occure
	//Charge Current <  1 A and continue 30 sec , OCC2  event recover
	//==============================================================================
	uint16_t BmsEngine::CheckCurrent_OCC2()
	{
		if ((BatterySys::m_ampReading & 0x80000000) != 0)    //Negative current  Charge Mode
		{	
			if (BmsEngine::Flag::OCC2 == 0)
			{
				if (BatterySys::m_ampReading  <= OCC2_CHECK)    //AmpReading_m
				{
					BmsEngine::Flag::occ2_deboc = 1;
					if (BmsEngine::Time::OCC2_time_delay == 0)
					{
						BmsEngine::Flag::OCC2 = 1;
						BmsEngine::Flag::occ2_deboc = 0;
						BmsEngine::Time::OCC2_time_delay = OCC2_RECOVER_TIME;                         
						p_bmspara->m_bcuStatus2 |= (uint16_t)BCU_STATUS_OCC2;    
						BmsEngine::FaultCount = 0U;
					}
				}
				else if (BatterySys::m_ampReading > OCC2_CHECK)
				{						
					BmsEngine::Time::OCC2_time_delay = OCC2_CHECK_TIME;   
				} 					
			}
			else if (BmsEngine::Flag::OCC2 == 1)
			{ 	  	
				if (BatterySys::m_ampReading  > OCC2_RECOVER)
				{
					BmsEngine::Flag::occ2_deboc = 1;
					if (BmsEngine::Time::OCC2_time_delay == 0)
					{
						BmsEngine::Flag::OCC2 = 0;
						BmsEngine::Flag::occ2_deboc = 0;
						BmsEngine::Time::OCC2_time_delay = OCC2_CHECK_TIME; 
						p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OCC2;                            
					}
				}
				else if (BatterySys::m_ampReading <= OCC2_RECOVER)
				{
					BmsEngine::Time::OCC2_time_delay = OCC2_RECOVER_TIME;   		
				} 					
			}  
		}
		else
		{   
			if (BmsEngine::Flag::OCC2 == 1)
			{ 			 
				BmsEngine::Flag::occ2_deboc = 1;
				if (BmsEngine::Time::OCC2_time_delay == 0)
				{
					BmsEngine::Flag::OCC2 = 0;
					BmsEngine::Flag::occ2_deboc = 0;
					BmsEngine::Time::OCC2_time_delay = OCC2_CHECK_TIME; 
					p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OCC2;                            
				}	
			}
			else if (BmsEngine::Flag::OCC2 == 0)
			{
				BmsEngine::Flag::occ2_deboc = 0;
				BmsEngine::Time::OCC2_time_delay = OCC2_CHECK_TIME; 
			}  
		}
		return (p_bmspara->m_bcuStatus2);
	}
	
	//==============================================================================
	//OCC3 Charge Check Function
	//Charge Current >= 195 A and continue 1 sec, OCC3  event occure
	//System Reset or Clear Flag, OCC3  event recover
	//==============================================================================
	uint16_t BmsEngine::CheckCurrent_OCC3()
	{
		if ((BatterySys::m_ampReading & 0x80000000) != 0)    //Negative current  Charge Mode
		{	
			if (BmsEngine::Flag::OCC3 == 0)
			{
				if (BatterySys::m_ampReading  <= OCC3_CHECK)    //AmpReading_m
				{
					BmsEngine::Flag::occ3_deboc = 1;
					if (BmsEngine::Time::OCC3_time_delay == 0)
					{
						BmsEngine::Flag::OCC3 = 1;
						BmsEngine::Flag::occ3_deboc = 0;
						BmsEngine::Time::OCC3_time_delay = OCC3_CHECK_TIME;	//OCC3_RECOVER_TIME;                      
						p_bmspara->m_bcuStatus3 |= (uint16_t)BCU_STATUS_OCC3;    
						BmsEngine::FaultCount = 0U;
					}
				}
				else if (BatterySys::m_ampReading > OCC3_CHECK)
				{						
					BmsEngine::Time::OCC3_time_delay = OCC3_CHECK_TIME;   
				} 					
			}
//			else if (BmsEngine::flag::OCC3 == 1)
//			{ 	  	
//				if (BatterySys::m_ampReading  > OCC3_RECOVER)
//				{
//					BmsEngine::flag::occ3_deboc = 1;
//					if (BmsEngine::time::OCC3_time_delay == 0)
//					{
//						BmsEngine::flag::OCC3 = 0;
//						BmsEngine::flag::occ3_deboc = 0;
//						BmsEngine::time::OCC3_time_delay = OCC3_CHECK_TIME; 
//						p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OCC3;                             
//					}
//				}
//				else if (BatterySys::m_ampReading <= OCC3_RECOVER)
//				{
//					BmsEngine::time::OCC3_time_delay = OCC3_RECOVER_TIME;   		
//				} 					
//			}  
		}
//		else
//		{   
//			if (BmsEngine::flag::OCC3 == 1)
//			{ 			 
//				BmsEngine::flag::occ3_deboc = 1;
//				if (BmsEngine::time::OCC3_time_delay == 0)
//				{
//					BmsEngine::flag::OCC3 = 0;
//					BmsEngine::flag::occ3_deboc = 0;
//					BmsEngine::time::OCC3_time_delay = OCC3_CHECK_TIME; 
//					p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OCC3;                            
//				}	
//			}
//			else if (BmsEngine::flag::OCC3 == 0)
//			{
//				BmsEngine::flag::occ3_deboc = 0;
//				BmsEngine::time::OCC3_time_delay = OCC3_CHECK_TIME; 
//			}  
//		}
		return (p_bmspara->m_bcuStatus3);
	}
	
	//==============================================================================
	//OCD1 Discharge Check Function
	//Discharge Current >= 180 A and continue 5 sec  , OCD1  event occure
	//Discharge Current < 170 A and continue 5 sec , OCD1  event recover
	//==============================================================================
	uint16_t BmsEngine::CheckCurrent_OCD1()
	{
		if ((BatterySys::m_ampReading & 0x80000000) == 0)    //Positive current  Discharge Mode
		{	
			if (BmsEngine::Flag::OCD1 == 0)
			{
				if (BatterySys::m_ampReading  >= OCD1_CHECK)    //AmpReading_m
				{
					BmsEngine::Flag::ocd1_deboc = 1;
					if (BmsEngine::Time::OCD1_time_delay == 0)
					{
						BmsEngine::Flag::OCD1 = 1;
						BmsEngine::Flag::ocd1_deboc = 0;
						BmsEngine::Time::OCD1_time_delay = OCD1_RECOVER_TIME;                  
						p_bmspara->m_bcuStatus1 |= (uint16_t)BCU_STATUS_OCD1;                           
					}
				}
				else if (BatterySys::m_ampReading < OCD1_CHECK)
				{						
					BmsEngine::Time::OCD1_time_delay = OCD1_CHECK_TIME;   
				} 					
			}
			else if (BmsEngine::Flag::OCD1 == 1)
			{ 	  	
				if (BatterySys::m_ampReading  < OCD1_RECOVER)
				{
					BmsEngine::Flag::ocd1_deboc = 1;
					if (BmsEngine::Time::OCD1_time_delay == 0)
					{
						BmsEngine::Flag::OCD1 = 0;
						BmsEngine::Flag::ocd1_deboc = 0;
						BmsEngine::Time::OCD1_time_delay = OCD1_CHECK_TIME; 
						p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_OCD1;                           
					}
				}
				else if (BatterySys::m_ampReading >= OCD1_RECOVER)
				{
					BmsEngine::Time::OCD1_time_delay = OCD1_RECOVER_TIME;   			
				} 
					
			}  
		}
		else
		{   
			if (BmsEngine::Flag::OCD1 == 1)
			{ 			 
				BmsEngine::Flag::ocd1_deboc = 1;
				if (BmsEngine::Time::OCD1_time_delay == 0)
				{
					BmsEngine::Flag::OCD1 = 0;
					BmsEngine::Flag::ocd1_deboc = 0;
					BmsEngine::Time::OCD1_time_delay = OCD1_CHECK_TIME; 
					p_bmspara->m_bcuStatus1 &= ~(uint16_t)BCU_STATUS_OCD1;                             
				}	
			}
			else if (BmsEngine::Flag::OCD1 == 0)
			{
				BmsEngine::Flag::ocd1_deboc = 0;
				BmsEngine::Time::OCD1_time_delay = OCD1_CHECK_TIME; 
			}  
		}
		return (p_bmspara->m_bcuStatus1);
	}
	
	//==============================================================================
	//OCD2 Discharge Check Function
	//Discharge Current >= 190 A and continue 3 sec  , OCD2  event occure
	//Discharge Current <  1 A and continue 30 sec , OCD2  event recover
	//==============================================================================
	uint16_t BmsEngine::CheckCurrent_OCD2()
	{
		if ((BatterySys::m_ampReading & 0x80000000) == 0)    //Positive current  Discharge Mode
		{	
			if (BmsEngine::Flag::OCD2 == 0)
			{
				if (BatterySys::m_ampReading  >= OCD2_CHECK)    //AmpReading_m
				{
					BmsEngine::Flag::ocd2_deboc = 1;
					if (BmsEngine::Time::OCD2_time_delay == 0)
					{
						BmsEngine::Flag::OCD2 = 1;
						BmsEngine::Flag::ocd2_deboc = 0;
						BmsEngine::Time::OCD2_time_delay = OCD2_RECOVER_TIME;                         
						p_bmspara->m_bcuStatus2 |= (uint16_t)BCU_STATUS_OCD2;     
						BmsEngine::FaultCount = 0U;
					}
				}
				else if (BatterySys::m_ampReading < OCD2_CHECK)
				{						
					BmsEngine::Time::OCD2_time_delay = OCD2_CHECK_TIME;   
				} 
					
			}
			else if (BmsEngine::Flag::OCD2 == 1)
			{ 	  	
				if (BatterySys::m_ampReading  < OCD2_RECOVER)
				{
					BmsEngine::Flag::ocd2_deboc = 1;
					if (BmsEngine::Time::OCD2_time_delay == 0)
					{
						BmsEngine::Flag::OCD2 = 0;
						BmsEngine::Flag::ocd2_deboc = 0;
						BmsEngine::Time::OCD2_time_delay = OCD2_CHECK_TIME; 
						p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OCD2;                            
					}
				}
				else if (BatterySys::m_ampReading >= OCD2_RECOVER)
				{
					BmsEngine::Time::OCD2_time_delay = OCD2_RECOVER_TIME;   		
				} 					
			}  
		}
		else
		{   
			if (BmsEngine::Flag::OCD2 == 1)
			{ 			 
				BmsEngine::Flag::ocd2_deboc = 1;
				if (BmsEngine::Time::OCD2_time_delay == 0)
				{
					BmsEngine::Flag::OCD2 = 0;
					BmsEngine::Flag::ocd2_deboc = 0;
					BmsEngine::Time::OCD2_time_delay = OCD2_CHECK_TIME; 
					p_bmspara->m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OCD2;                              
				}	
			}
			else if (BmsEngine::Flag::OCD2 == 0)
			{
				BmsEngine::Flag::ocd2_deboc = 0;
				BmsEngine::Time::OCD2_time_delay = OCD2_CHECK_TIME; 
			}  
		}
		return (p_bmspara->m_bcuStatus2);
	}
	
	//==============================================================================
	//OCD3 Discharge Check Function
	//Discharge Current >= 195 A and continue 1 sec, OCD3  event occure
	//System Reset or Clear Flag, OCD3  event recover
	//==============================================================================
	uint16_t BmsEngine::CheckCurrent_OCD3()
	{
		if ((BatterySys::m_ampReading & 0x80000000) == 0)    //Positive current  Discharge Mode
		{	
			if (BmsEngine::Flag::OCD3 == 0)
			{
				if (BatterySys::m_ampReading  >= OCD3_CHECK)    //AmpReading_m
				{
					BmsEngine::Flag::ocd3_deboc = 1;
					if (BmsEngine::Time::OCD3_time_delay == 0)
					{
						BmsEngine::Flag::OCD3 = 1;
						BmsEngine::Flag::ocd3_deboc = 0;
						BmsEngine::Time::OCD3_time_delay = OCD3_CHECK_TIME;	//OCD3_RECOVER_TIME;                          
						p_bmspara->m_bcuStatus3 |= (uint16_t)BCU_STATUS_OCD3;     
						BmsEngine::FaultCount = 0U;
					}
				}
				else if (BatterySys::m_ampReading < OCD3_CHECK)
				{						
					BmsEngine::Time::OCD3_time_delay = OCD3_CHECK_TIME;   
				} 					
			}
//			else if (BmsEngine::flag::OCD3 == 1)
//			{ 	  	
//				if (BatterySys::m_ampReading  < OCD3_RECOVER)
//				{
//					BmsEngine::flag::ocd3_deboc = 1;
//					if (BmsEngine::time::OCD3_time_delay == 0)
//					{
//						BmsEngine::flag::OCD3 = 0;
//						BmsEngine::flag::ocd3_deboc = 0;
//						BmsEngine::time::OCD3_time_delay = OCD3_CHECK_TIME; 
//						p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OCD3;                              
//					}
//				}
//				else if (BatterySys::m_ampReading >= OCD3_RECOVER)
//				{
//					BmsEngine::time::OCD3_time_delay = OCD3_RECOVER_TIME;   			
//				} 					
//			}  
		}
//		else
//		{   
//			if (BmsEngine::flag::OCD3 == 1)
//			{ 			 
//				BmsEngine::flag::ocd3_deboc = 1;
//				if (BmsEngine::time::OCD3_time_delay == 0)
//				{
//					BmsEngine::flag::OCD3 = 0;
//					BmsEngine::flag::ocd3_deboc = 0;
//					BmsEngine::time::OCD3_time_delay = OCD3_CHECK_TIME; 
//					p_bmspara->m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OCD3;                               
//				}	
//			}
//			else if (BmsEngine::flag::OCD3 == 0)
//			{
//				BmsEngine::flag::ocd3_deboc = 0;
//				BmsEngine::time::OCD3_time_delay = OCD3_CHECK_TIME; 
//			}  
//		}
		return (p_bmspara->m_bcuStatus3);
	}
	

	//==============================================================================
	//HVIL1 Check Function
	//HVIL1_Emergency Disconnect and continue 500ms , hvil event occure
	//HVIL1_Emergency Connect and continue 1000ms , hvil event recover
	//==============================================================================
	void BmsEngine::HVIL1_check(void)
	{ 	  
		if (BmsEngine::Flag::HVIL1_Error == 0)		//check HVIL event occure    
		{
			//hvil1 event not occure,HV1_HVIL H to L
			if ((BmsEngine::Flag::hvil1_deboc == 0) && (HV1_Emergency == 1))
			{		
				BmsEngine::Flag::hvil1_deboc = 1; //debounce start
			}  
			else if ((BmsEngine::Flag::hvil1_deboc == 1) && (HV1_Emergency == 0))        //hvil1 event not occure,debounce start
			{		
				BmsEngine::Flag::hvil1_deboc = 0; //but hvil1 L to H,clear debounce
				BmsEngine::Time::hvil1_time_delay = HVIL1_CHECK_TIME;
			}   
			if (BmsEngine::Flag::hvil1_deboc == 1)                         
			{            
				if (BmsEngine::Time::hvil1_time_delay == 0)		//check iso H event active time
				{
					BmsEngine::Flag::hvil1_deboc = 0;
					BmsEngine::Flag::HVIL1_Error = 1;
					BmsEngine::Time::hvil1_time_delay = HVIL1_RECOVER_TIME;
					RLY_NFB = NFB_OFF;
					RLY_Precharge = RLY_OFF;
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_OFF;			
				}                                       
			}  
		}
		else if (BmsEngine::Flag::HVIL1_Error == 1)
		{
			//hvil1 event not occure,HV1_HVIL L to H
			if ((BmsEngine::Flag::hvil1_deboc == 0) && (HV1_Emergency == 0))
			{		
				BmsEngine::Flag::hvil1_deboc = 1; //debounce start
			} 
			else if ((BmsEngine::Flag::hvil1_deboc == 1) && (HV1_Emergency == 1))
			{
				BmsEngine::Flag::hvil1_deboc = 0;
				BmsEngine::Time::hvil1_time_delay = HVIL1_RECOVER_TIME;				
			}
			if (BmsEngine::Flag::hvil1_deboc == 1)                         
			{		              
				if (BmsEngine::Time::hvil1_time_delay == 0)             //check iso H event active time
				{
					BmsEngine::Flag::hvil1_deboc = 0;
					BmsEngine::Flag::HVIL1_Error = 0;
					BmsEngine::Time::hvil1_time_delay = HVIL1_CHECK_TIME;
				}                                       
			}  
		}
	}     

	//==============================================================================
	//HVIL2 Check Function
	//HVIL2_Insulation Fault and continue 500ms , hvi2 event occure
	//HVIL2_Insulation Normal and continue 1000ms , hvi2 event recover
	//==============================================================================	
	void BmsEngine::HVIL2_Insulation_Check(void)
	{ 	  
		if (BmsEngine::Flag::HVIL2_Error == 0)		//check HVIL event occure    
		{
			if ((BmsEngine::Flag::hvil2_deboc == 0) && (HV2_Insulation == 1))            //hvil1 event not occure,HV1_HVIL H to L     
			{		
				BmsEngine::Flag::hvil2_deboc = 1; //debounce start
			}  
			else if((BmsEngine::Flag::hvil2_deboc == 1) && (HV2_Insulation == 0))        //hvil1 event not occure,debounce start
			{		
				BmsEngine::Flag::hvil2_deboc = 0; //but hvil2 L to H,clear debounce
				BmsEngine::Time::hvil2_time_delay = HVIL2_CHECK_TIME;
			}   
			if (BmsEngine::Flag::hvil2_deboc == 1)                         
			{            
				if (BmsEngine::Time::hvil2_time_delay == 0)		//check iso H event active time
				{
					BmsEngine::Flag::hvil2_deboc = 0;
					BmsEngine::Flag::HVIL2_Error = 1;
					BmsEngine::Time::hvil2_time_delay = HVIL2_RECOVER_TIME;
					RLY_NFB = NFB_OFF;			//?保護動作及回復動作
					RLY_Precharge = RLY_OFF;
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_OFF;			
				}                                       
			}  
		}
//		else if (BmsEngine::Flag::HVIL2_Error == 1)
//		{
//			if ((BmsEngine::Flag::hvil2_deboc == 0) && (HV2_Insulation == 0))   //hvil1 event not occure,HV1_HVIL L to H
//			{		
//				BmsEngine::Flag::hvil2_deboc = 1; //debounce start
//			} 
//			else if((BmsEngine::Flag::hvil2_deboc == 1) && (HV2_Insulation == 1))
//			{
//				BmsEngine::Flag::hvil2_deboc = 0;
//				BmsEngine::Time::hvil2_time_delay = HVIL2_RECOVER_TIME;				
//			}
//			if (BmsEngine::Flag::hvil2_deboc == 1)                         
//			{		              
//				if (BmsEngine::Time::hvil2_time_delay == 0)             //check iso H event active time
//				{
//					BmsEngine::Flag::hvil2_deboc = 0;
//					BmsEngine::Flag::HVIL2_Error = 0;
//					BmsEngine::Time::hvil2_time_delay = HVIL2_CHECK_TIME;
//				}                                       
//			}  
//		}
	}     
		
	//==============================================================================
	//IV1 Check Function
	//Imbalance Voltage >= 150mV and continue 10sec , IV1 event occure
	//Imbalance Voltage < 50mV and continue 10sec , IV1 event recover
	//==============================================================================
	uint16_t BmsEngine::CheckImbalance_IV1()
	{
		if (BmsEngine::Flag::IV1 == 0)
		{
			if ((BatterySys::MaxVolt - BatterySys::MinVolt)  >= IV1_CHECK)    //
			{
				BmsEngine::Flag::iv1_deboc = 1;
				if (BmsEngine::Time::IV1_time_delay == 0)
				{
					BmsEngine::Flag::IV1 = 1;
					BmsEngine::Flag::iv1_deboc = 0;
					BmsEngine::Time::IV1_time_delay = IV1_RECOVER_TIME;                      
					p_bmspara->m_bcuStatus4 |= (uint16_t)BCU_STATUS_IV1;                           
				}
			}
			else if ((BatterySys::MaxVolt - BatterySys::MinVolt) < IV1_CHECK)
			{						
				BmsEngine::Time::IV1_time_delay = IV1_CHECK_TIME;   
			} 					
		}
		else if (BmsEngine::Flag::IV1 == 1)
		{ 	  	
			if ((BatterySys::MaxVolt - BatterySys::MinVolt)  < IV1_RECOVER)
			{
				BmsEngine::Flag::iv1_deboc = 1;
				if (BmsEngine::Time::IV1_time_delay == 0)
				{
					BmsEngine::Flag::IV1 = 0;
					BmsEngine::Flag::iv1_deboc = 0;
					BmsEngine::Time::IV1_time_delay = IV1_CHECK_TIME; 
					p_bmspara->m_bcuStatus4 &= ~(uint16_t)BCU_STATUS_IV1;                           
				}
			}
			else if ((BatterySys::MaxVolt - BatterySys::MinVolt) >= IV1_RECOVER)
			{
				BmsEngine::Time::IV1_time_delay = IV1_RECOVER_TIME;   			
			} 
					
		}  

		return (p_bmspara->m_bcuStatus4);
	}
	
	//==============================================================================
	//Charge Enable Check Function
	//max_vol >= 3.55V  and continue 3sec , Charge Disable event occure
	//max_vol <= 3.36V  and continue 3sec , Charge Enable event occure
	//==============================================================================
	void BmsEngine::Check_CHG_Enable()
	{
		if (BmsEngine::m_bcuStatus & BCU_STATUS_CMCAN_FAULT)
		{
			return;
		}
		if (BmsEngine::Flag::CHG_E == 0)
		{
			if (BatterySys::MaxVolt <= CHG_E_CHECK)
			{
				BmsEngine::Flag::chg_e_deboc = 1;
				if (BmsEngine::Time::CHG_E_time_delay == 0)
				{
					BmsEngine::Flag::CHG_E = 1;
					BmsEngine::Flag::chg_e_deboc = 0;
					BmsEngine::Time::CHG_E_time_delay = CHG_E_RECOVER_TIME;
					BmsEngine::SBCUCANMessageCapacity::ChargeEnable = 1;
				}
			}
			else if (BatterySys::MaxVolt > CHG_E_CHECK)
			{
				BmsEngine::Time::CHG_E_time_delay = CHG_E_CHECK_TIME;                                 		
			}
		}
		else if (BmsEngine::Flag::CHG_E == 1)
		{
			if (BatterySys::MaxVolt >= CHG_E_RECOVER)
			{
				BmsEngine::Flag::chg_e_deboc = 1;
				if (BmsEngine::Time::CHG_E_time_delay == 0)
				{
					BmsEngine::Flag::CHG_E = 0;
					BmsEngine::Time::CHG_E_time_delay = CHG_E_CHECK_TIME;
					BmsEngine::Flag::chg_e_deboc = 0;
					BmsEngine::SBCUCANMessageCapacity::ChargeEnable = 0;
				}
			}
			else if (BatterySys::MaxVolt < CHG_E_RECOVER)
			{
				BmsEngine::Time::CHG_E_time_delay = CHG_E_RECOVER_TIME;
			}
		}

	
	}
	
	//==============================================================================
	//Discharge Enable Check Function
	//MinVolt <= 3.4V  and continue 3sec , Discharge Disable event occure
	//MinVolt >= 3.45V  and continue 3sec , Discharge Enable event occure
	//==============================================================================
	void BmsEngine::Check_DSG_Enable()
	{
		if (BmsEngine::m_bcuStatus & BCU_STATUS_CMCAN_FAULT)
		{
			return;
		}
		if (BmsEngine::Flag::DSG_E == 0)
		{
			if (BatterySys::MinVolt >= DSG_E_CHECK)
			{
				BmsEngine::Flag::dsg_e_deboc = 1;
				if (BmsEngine::Time::DSG_E_time_delay == 0)
				{
					BmsEngine::Flag::DSG_E = 1;
					BmsEngine::Flag::dsg_e_deboc = 0;
					BmsEngine::Time::DSG_E_time_delay = DSG_E_RECOVER_TIME;
					BmsEngine::SBCUCANMessageCapacity::DischargeEnable = 1;
				}
			}
			else if (BatterySys::MinVolt < DSG_E_CHECK)
			{
				BmsEngine::Time::DSG_E_time_delay = DSG_E_CHECK_TIME;                                 		
			}
		}
		else if (BmsEngine::Flag::DSG_E == 1)
		{
			if (BatterySys::MinVolt <= DSG_E_RECOVER)
			{
				BmsEngine::Flag::dsg_e_deboc = 1;
				if (BmsEngine::Time::DSG_E_time_delay == 0)
				{
					BmsEngine::Flag::DSG_E = 0;
					BmsEngine::Time::DSG_E_time_delay = DSG_E_CHECK_TIME;
					BmsEngine::Flag::dsg_e_deboc = 0;
					BmsEngine::SBCUCANMessageCapacity::DischargeEnable = 0;
				}
			}
			else if (BatterySys::MinVolt > DSG_E_RECOVER)
			{
				BmsEngine::Time::DSG_E_time_delay = DSG_E_RECOVER_TIME;
			}
		}

	}
	
	//==============================================================================
	//Full Charge Check Function
	//MinVolt >= 3.480V  and MaxVolt-MinVolt < 40mV and Charge Current <= 1A and continue 3sec , Full Charge event occure
	//SOC <= 99.9%  and continue 3sec , Full Charge event recover
	//MaxVolt <= 3.36V and continue 3sec , Full Charge event recover	
	//==============================================================================
	void BmsEngine::Check_FullCHG()
	{
		if (BmsEngine::m_bcuStatus & BCU_STATUS_CMCAN_FAULT)
		{
			return;
		}
		if (BmsEngine::Flag::FullCHG == 0)
		{
			if ((BatterySys::MinVolt >= FULLCHG_MINVOLT_CHECK)
					&& ((BatterySys::MaxVolt - BatterySys::MinVolt) < FULLCHG_VOLTAGE_DIFFERENCE)
					&& (BatterySys::m_ampReading >= FULLCHG_A_CHECK))
			{
				BmsEngine::Flag::fullchg_deboc = 1;
				if (BmsEngine::Time::FullCHG_time_delay == 0)
				{
					BmsEngine::Flag::FullCHG = 1;
					BmsEngine::Flag::fullchg_deboc = 0;
					BmsEngine::Time::FullCHG_time_delay = FULLCHG_RECOVER_TIME;	
					BmsEngine::SBCUCANMessageCapacity::FullCharge = 1;
					SOC::FullCHG_END_Flag = 1;
					if (SOC::FCCcoulombCount_Flag == 0)
					{
						SOC::SOHupdate_Start_Flag = 1;
					}
					
				}
			}
			else if ((BatterySys::MinVolt < FULLCHG_MINVOLT_CHECK)
					&& ((BatterySys::MaxVolt - BatterySys::MinVolt) >= FULLCHG_VOLTAGE_DIFFERENCE)
					&& (BatterySys::m_ampReading < FULLCHG_A_CHECK))
			{
				BmsEngine::Time::FullCHG_time_delay = FULLCHG_CHECK_TIME;                                 		
			}
		}
		else if (BmsEngine::Flag::FullCHG == 1)
		{
//			if (BmsEngine::m_soc  <= FULLCHG_SOC_RECOVER)
			if (BatterySys::MaxVolt <= FULLCHG_SOC_RECOVER)
			{
				BmsEngine::Flag::fullchg_deboc = 1;
				if (BmsEngine::Time::FullCHG_time_delay == 0)
				{
					BmsEngine::Flag::FullCHG = 0;
					BmsEngine::Time::FullCHG_time_delay = FULLCHG_CHECK_TIME;
					BmsEngine::Flag::fullchg_deboc = 0;
					BmsEngine::SBCUCANMessageCapacity::FullCharge = 0;	
				}
			}
//			else if (BmsEngine::m_soc > FULLCHG_SOC_RECOVER)
			else if (BatterySys::MaxVolt > FULLCHG_SOC_RECOVER)
			{
				BmsEngine::Time::FullCHG_time_delay = FULLCHG_RECOVER_TIME;
			}
		}

	}

	//==============================================================================
	//Full Discharge Check Function
	//MinVolt <= 2.8V  and continue 3sec , Full Discharge event occure
	//SOC >= 5%  and MinVolt > 3.000V and continue 3sec , Full Discharge event recover
	//==============================================================================
	void BmsEngine::Check_FullDSG()
	{
		if (BmsEngine::m_bcuStatus & BCU_STATUS_CMCAN_FAULT)
		{
			return;
		}
	
		if (BmsEngine::Flag::FullDSG == 0)
		{							
			if (BatterySys::MinVolt <= FULLDSG_V_CHECK) 	
			{
				BmsEngine::Flag::fulldsg_deboc = 1;
				if (BmsEngine::Time::FullDSG_time_delay == 0)
				{
					BmsEngine::Flag::FullDSG = 1;
					BmsEngine::Flag::fulldsg_deboc = 0;
					BmsEngine::Time::FullDSG_time_delay = FULLDSG_RECOVER_TIME;	
					BmsEngine::SBCUCANMessageCapacity::FullDischarge = 1;
					SOC::FullDSG_END_Flag = 1;
				}
			}
			else if (BatterySys::MinVolt > FULLDSG_V_CHECK) 
			{
				BmsEngine::Time::FullDSG_time_delay = FULLDSG_CHECK_TIME;                                 		
			}
		}
		else if (BmsEngine::Flag::FullDSG == 1)
		{
			if ((BmsEngine::m_soc  >= FULLDSG_SOC_RECOVER) && (BatterySys::MinVolt > FULLDSG_V_RECOVER)) 
			{
				BmsEngine::Flag::fulldsg_deboc = 1;
				if (BmsEngine::Time::FullDSG_time_delay == 0)
				{
					BmsEngine::Flag::FullDSG = 0;
					BmsEngine::Time::FullDSG_time_delay = FULLDSG_CHECK_TIME;
					BmsEngine::Flag::fulldsg_deboc = 0;
					BmsEngine::SBCUCANMessageCapacity::FullDischarge = 0;
				}
			}
			else if ((BmsEngine::m_soc < FULLDSG_SOC_RECOVER)&& (BatterySys::MinVolt > FULLDSG_V_RECOVER)) 
			{
				BmsEngine::Time::FullDSG_time_delay = FULLDSG_RECOVER_TIME;
			}
		}
		
	
	}
	
	//==============================================================================
	//Discharge State SOC 5% Check Function
	//MinVolt <= 3.0V  and Discharge Current >= 0.1A and continue 3sec , Discharge event occure
	//SOC >= 5%  and MinVolt > 3.000V and continue 3sec , Full Discharge event recover
	//==============================================================================
	
	
	//==============================================================================
	//AFE BMU ADC Warning Check Function
	//BMU ADC CHECK TEMP > 26'c and ADC CHECK TEMP < 24'c and continue 10sec , BMU_ADC_Warning event occure
	//BMU ADC CHECK TEMP <= 26'c and ADC CHECK TEMP >= 24'c and continue 10sec , BMU_ADC_Warning event recover	
	//==============================================================================
	uint16_t BmsEngine::Check_ADCWarning()
	{
		BmsEngine::Flag::ADCTempWarning = 0;
	
		for (int current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++) //
		{
			if ((BatterySys::pack[current_ic].packTemp[3] > ADCWARNING_CHECK_UPPERTEMP) 
				|| (BatterySys::pack[current_ic].packTemp[3] < ADCWARNING_CHECK_LOWERTEMP))
			{
				BatterySys::pack[current_ic].packADCWarning = 1;
				BmsEngine::Flag::ADCTempWarning = 1;
			}
			else                            
			{
				if (BmsEngine::Flag::ADCTempWarning == 1)
				{
					BmsEngine::Flag::ADCTempWarning = 1;
				}			
				else
				{
					BmsEngine::Flag::ADCTempWarning = 0;
				}
			}			
		}

		if (BmsEngine::Flag::ADCWarning == 0)
		{		
			if (BmsEngine::Flag::ADCTempWarning == 1)
			{
				BmsEngine::Flag::adcWarning_deboc = 1;
				if (BmsEngine::Time::ADCWarning_time_delay == 0)
				{
					BmsEngine::Flag::ADCWarning = 1;
					BmsEngine::Flag::adcWarning_deboc = 0;
					BmsEngine::Time::ADCWarning_time_delay = ADCWARNING_RECOVER_TIME;	
				}			
			}
			else 
			{
				BmsEngine::Time::ADCWarning_time_delay = ADCWARNING_CHECK_TIME;                                 		
			}
		}
		else if (BmsEngine::Flag::ADCWarning == 1)
		{
			if (BmsEngine::Flag::ADCTempWarning == 0)  
			{
				BmsEngine::Flag::adcWarning_deboc = 1;
				if (BmsEngine::Time::ADCWarning_time_delay == 0)
				{		
					BmsEngine::Flag::ADCWarning = 0;
					BmsEngine::Time::ADCWarning_time_delay = ADCWARNING_CHECK_TIME;
					BmsEngine::Flag::adcWarning_deboc = 0;						
				}
			}
			else 
			{
				BmsEngine::Time::ADCWarning_time_delay = ADCWARNING_RECOVER_TIME;
			}
		}				
		return (BmsEngine::Flag::ADCWarning);
	}
	
	//==============================================================================
	//AFE BMU ADC Fault Check Function
	//BMU ADC CHECK TEMP > 26.5'c and ADC CHECK TEMP < 23.5'c and continue 5sec , BMU_ADC_FAULT event occure
	//All ADCFault == false  and continue 10sec , BMU_ADC_FAULT event recover	
	//==============================================================================
	uint16_t BmsEngine::Check_ADCFault()
	{
		BmsEngine::Flag::ADCTempFault = 0;
	
		for (uint16_t current_ic = 0U; current_ic < LTC6811SPI::TOTAL_IC; current_ic++) //
		{
			if ((BatterySys::pack[current_ic].packTemp[3] > ADCFAULT_CHECK_UPPERTEMP) 
				|| (BatterySys::pack[current_ic].packTemp[3] < ADCFAULT_CHECK_LOWERTEMP))
			{
				BatterySys::pack[current_ic].packADCFault = 1;
				BmsEngine::Flag::ADCTempFault = 1;
			}
			else                            
			{
				if (BmsEngine::Flag::ADCTempFault == 1)
				{
					BmsEngine::Flag::ADCTempFault = 1;
				}			
				else
				{
					BmsEngine::Flag::ADCTempFault = 0;
				}
			}			
		}

		if (BmsEngine::Flag::ADCFault == 0)
		{		
			if (BmsEngine::Flag::ADCTempFault == 1)
			{
				BmsEngine::Flag::adcfault_deboc = 1;
				if (BmsEngine::Time::ADCFault_time_delay == 0)
				{
					BmsEngine::Flag::ADCFault = 1;
					BmsEngine::Flag::adcfault_deboc = 0;
					BmsEngine::Time::ADCFault_time_delay = ADCFAULT_RECOVER_TIME;	
					p_bmspara->m_bcuStatus5 |= (uint16_t)BMU_STATUS_ADCFAULT;   
					BmsEngine::FaultCount = 0U;
				}
				
			}
			else 
			{
				BmsEngine::Time::ADCFault_time_delay = ADCFAULT_CHECK_TIME;                                 		
			}

		}
		else if (BmsEngine::Flag::ADCFault == 1)
		{
			if (BmsEngine::Flag::ADCTempFault == 0)  
			{
				BmsEngine::Flag::adcfault_deboc = 1;
				if (BmsEngine::Time::ADCFault_time_delay == 0)
				{
					BmsEngine::Cumulative_times++;
					if (BmsEngine::Cumulative_times > CUMULATIVE_COUNT)
					{
						BmsEngine::Cumulative_times = CUMULATIVE_COUNT;
						BmsEngine::Flag::ADCFault = 1;
						BmsEngine::Time::ADCFault_time_delay = ADCFAULT_RECOVER_TIME;
						BmsEngine::Flag::adcfault_deboc = 0;
						
					}
					else
					{
						BmsEngine::Flag::ADCFault = 0;
						BmsEngine::Time::ADCFault_time_delay = ADCFAULT_CHECK_TIME;
						BmsEngine::Flag::adcfault_deboc = 0;
						p_bmspara->m_bcuStatus5 &= ~(uint16_t)BMU_STATUS_ADCFAULT;
					}

				}
			}
			else 
			{
				BmsEngine::Time::ADCFault_time_delay = ADCFAULT_RECOVER_TIME;
			}
		}	
			
		return (p_bmspara->m_bcuStatus5);
	}
	
	//==============================================================================
	//BCU Self Test Check Function
	//==============================================================================
	uint16_t BmsEngine::Check_SelfTestFault()
	{
		if (FailSafePOR_Flag == 1)	
		{
			p_bmspara->m_bcuStatus6 |= (uint16_t)BCU_STATUS_SELFTESTFAULT;   
		}
		else
		{
			p_bmspara->m_bcuStatus6 &= ~(uint16_t)BCU_STATUS_SELFTESTFAULT;
		}
	
		return (p_bmspara->m_bcuStatus6);
	}

	//==============================================================================
	//OT1_PCS_Positive Check Function
	//PCS_Positive_temp >= 85'c or PCS_Positive_temp <= 0'c and continue 10sec,OT1_PCS_Positive  event occure
	//PCS_Positive_temp <  75'c and PCS_Positive_temp > 5'c and continue 10sec,OT1_PCS_Positive  event recover
	//==============================================================================	
	void BmsEngine::CheckTempOT1_PCS_Positive()		
	{
		if (BmsEngine::Flag::OT1_PCS_Positive == 0)
		{
			if ((ADCC::TempTxCH[0] >= OT1_PCS_POSITIVE_CHECK_TEMP) || (ADCC::TempTxCH[0] <= OT1_PCS_POSITIVE_CHECK_LOWTEMP))
			{   
				BmsEngine::Flag::ot1_PCS_Positive_deboc = 1;
				if (BmsEngine::Time::OT1_PCS_Positive_time_delay == 0)
				{
					BmsEngine::Flag::OT1_PCS_Positive = 1;
					BmsEngine::Flag::ot1_PCS_Positive_deboc = 0;
					BmsEngine::Time::OT1_PCS_Positive_time_delay = OT1_PCS_POSITIVE_RECOVER_TIME;   					
                   
				}
			}
			else if ((ADCC::TempTxCH[0] < OT1_PCS_POSITIVE_CHECK_TEMP) && (ADCC::TempTxCH[0] > OT1_PCS_POSITIVE_CHECK_LOWTEMP))
			{
				BmsEngine::Time::OT1_PCS_Positive_time_delay = OT1_PCS_POSITIVE_CHECK_TIME;
			}
		}
		else if (BmsEngine::Flag::OT1_PCS_Positive == 1)
		{
			if ((ADCC::TempTxCH[0] < OT1_PCS_POSITIVE_RECOVER_TEMP) && (ADCC::TempTxCH[0] > OT1_PCS_POSITIVE_RECOVER_LOWTEMP))
			{
				BmsEngine::Flag::ot1_PCS_Positive_deboc = 1;
				if (BmsEngine::Time::OT1_PCS_Positive_time_delay == 0)
				{
					BmsEngine::Flag::OT1_PCS_Positive = 0;
					BmsEngine::Time::OT1_PCS_Positive_time_delay = OT1_PCS_POSITIVE_CHECK_TIME;
					BmsEngine::Flag::ot1_PCS_Positive_deboc = 0;
                  
				}
			}
			else if ((ADCC::TempTxCH[0] >= OT1_PCS_POSITIVE_RECOVER_TEMP)
					|| (ADCC::TempTxCH[0] <= OT1_PCS_POSITIVE_RECOVER_LOWTEMP))
			{
				BmsEngine::Time::OT1_PCS_Positive_time_delay = OT1_PCS_POSITIVE_RECOVER_TIME;
			}
		}		
	}	   //TempTxCH[0] = BCU PCS+ Temperature

	//CheckTempOT2_PCS_Positive
	void BmsEngine::CheckTempOT2_PCS_Positive()		
	{
		if (BmsEngine::Flag::OT2_PCS_Positive == 0)
		{
			if ((ADCC::TempTxCH[0] >= OT2_PCS_POSITIVE_CHECK_TEMP) && (ADCC::Temp_abnormal[0] == 0))
			{   
				BmsEngine::Flag::ot2_PCS_Positive_deboc = 1;
				if (BmsEngine::Time::OT2_PCS_Positive_time_delay == 0)
				{
					BmsEngine::Flag::OT2_PCS_Positive = 1;
					BmsEngine::Flag::ot2_PCS_Positive_deboc = 0;
					BmsEngine::Time::OT2_PCS_Positive_time_delay = OT2_PCS_POSITIVE_RECOVER_TIME;   					
                   
				}
			}
			else if ((ADCC::TempTxCH[0] < OT2_PCS_POSITIVE_CHECK_TEMP) || (ADCC::Temp_abnormal[0] == 1))
			{
				BmsEngine::Time::OT2_PCS_Positive_time_delay = OT2_PCS_POSITIVE_CHECK_TIME;
			}
		}		
	}	   //TempTxCH[0] = BCU PCS+ Temperature
	
	//==============================================================================
	//OT1_PCS_Negative Check Function
	//PCS_Negative_temp >= 85'c or PCS_Negative_temp <= 0'c and continue 10sec,OT1_PCS_Negative  event occure
	//PCS_Negative_temp <  75'c and PCS_Negative_temp > 5'c and continue 10sec,OT1_PCS_Negative  event recover
	//==============================================================================
	void BmsEngine::CheckTempOT1_PCS_Negative()
	{
		if (BmsEngine::Flag::OT1_PCS_Negative == 0)
		{
			if ((ADCC::TempTxCH[1] >= OT1_PCS_NEGATIVE_CHECK_TEMP) || (ADCC::TempTxCH[1] <= OT1_PCS_NEGATIVE_CHECK_LOWTEMP))
			{   
				BmsEngine::Flag::ot1_PCS_Negative_deboc = 1;
				if (BmsEngine::Time::OT1_PCS_Negative_time_delay == 0)
				{
					BmsEngine::Flag::OT1_PCS_Negative = 1;
					BmsEngine::Flag::ot1_PCS_Negative_deboc = 0;
					BmsEngine::Time::OT1_PCS_Negative_time_delay = OT1_PCS_NEGATIVE_RECOVER_TIME;   					
                    
				}
			}
			else if ((ADCC::TempTxCH[1] < OT1_PCS_NEGATIVE_CHECK_TEMP) && (ADCC::TempTxCH[1] > OT1_PCS_NEGATIVE_CHECK_LOWTEMP))
			{
				BmsEngine::Time::OT1_PCS_Negative_time_delay = OT1_PCS_NEGATIVE_CHECK_TIME;
			}
		}
		else if (BmsEngine::Flag::OT1_PCS_Negative == 1)
		{
			if ((ADCC::TempTxCH[1] < OT1_PCS_NEGATIVE_RECOVER_TEMP) && (ADCC::TempTxCH[1] > OT1_PCS_NEGATIVE_RECOVER_LOWTEMP))
			{
				BmsEngine::Flag::ot1_PCS_Negative_deboc = 1;
				if (BmsEngine::Time::OT1_PCS_Negative_time_delay == 0)
				{
					BmsEngine::Flag::OT1_PCS_Negative = 0;
					BmsEngine::Time::OT1_PCS_Negative_time_delay = OT1_PCS_NEGATIVE_CHECK_TIME;
					BmsEngine::Flag::ot1_PCS_Negative_deboc = 0;
                    
				}
			}
			else if ((ADCC::TempTxCH[1] >= OT1_PCS_NEGATIVE_RECOVER_TEMP)
					|| (ADCC::TempTxCH[1] <= OT1_PCS_NEGATIVE_RECOVER_LOWTEMP))
			{
				BmsEngine::Time::OT1_PCS_Negative_time_delay = OT1_PCS_NEGATIVE_RECOVER_TIME;
			}
		}		
	}	   //TempTxCH[1] = BCU PCS- Temperature
	
	//CheckTempOT2_PCS_Negative
	void BmsEngine::CheckTempOT2_PCS_Negative()
	{
		if (BmsEngine::Flag::OT2_PCS_Negative == 0)
		{
			if (ADCC::TempTxCH[1] >= OT2_PCS_NEGATIVE_CHECK_TEMP)
			{   
				BmsEngine::Flag::ot2_PCS_Negative_deboc = 1;
				if (BmsEngine::Time::OT2_PCS_Negative_time_delay == 0)
				{
					BmsEngine::Flag::OT2_PCS_Negative = 1;
					BmsEngine::Flag::ot2_PCS_Negative_deboc = 0;
					BmsEngine::Time::OT2_PCS_Negative_time_delay = OT2_PCS_NEGATIVE_RECOVER_TIME;   				
                 
				}
			}
			else if (ADCC::TempTxCH[1] < OT2_PCS_NEGATIVE_CHECK_TEMP)
			{
				BmsEngine::Time::OT2_PCS_Negative_time_delay = OT2_PCS_NEGATIVE_CHECK_TIME;
			}
		}

	}	   //TempTxCH[1] = BCU PCS- Temperature

	//==============================================================================
	//OT1_HVDC_Positive Check Function
	//HVDC_Positive_temp >= 85'c or HVDC_Positive_temp <= 0'c and continue 10sec,OT1_HVDC_Positive  event occure
	//HVDC_Positive_temp <  75'c and HVDC_Positive_temp > 5'c and continue 10sec,OT1_HVDC_Positive  event recover
	//==============================================================================
	void BmsEngine::CheckTempOT1_HVDC_Positive()
	{
		if (BmsEngine::Flag::OT1_HVDC_Positive == 0)
		{
			if ((ADCC::TempTxCH[2] >= OT1_HVDC_POSITIVE_CHECK_TEMP) || (ADCC::TempTxCH[2] <= OT1_HVDC_POSITIVE_CHECK_LOWTEMP))
			{   
				BmsEngine::Flag::ot1_HVDC_Positive_deboc = 1;
				if (BmsEngine::Time::OT1_HVDC_Positive_time_delay == 0)
				{
					BmsEngine::Flag::OT1_HVDC_Positive = 1;
					BmsEngine::Flag::ot1_HVDC_Positive_deboc = 0;
					BmsEngine::Time::OT1_HVDC_Positive_time_delay = OT1_HVDC_POSITIVE_RECOVER_TIME;   				
                     
				}
			}
			else if ((ADCC::TempTxCH[2] < OT1_HVDC_POSITIVE_CHECK_TEMP) && (ADCC::TempTxCH[2] > OT1_HVDC_POSITIVE_CHECK_LOWTEMP))
			{
				BmsEngine::Time::OT1_HVDC_Positive_time_delay = OT1_HVDC_POSITIVE_CHECK_TIME;
			}
		}
		else if (BmsEngine::Flag::OT1_HVDC_Positive == 1)
		{
			if ((ADCC::TempTxCH[2] < OT1_HVDC_POSITIVE_RECOVER_TEMP) && (ADCC::TempTxCH[2] > OT1_HVDC_POSITIVE_RECOVER_LOWTEMP))
			{
				BmsEngine::Flag::ot1_HVDC_Positive_deboc = 1;
				if (BmsEngine::Time::OT1_HVDC_Positive_time_delay == 0)
				{
					BmsEngine::Flag::OT1_HVDC_Positive = 0;
					BmsEngine::Time::OT1_HVDC_Positive_time_delay = OT1_HVDC_POSITIVE_CHECK_TIME;
					BmsEngine::Flag::ot1_HVDC_Positive_deboc = 0;
                    
				}
			}
			else if ((ADCC::TempTxCH[2] >= OT1_HVDC_POSITIVE_RECOVER_TEMP)
					|| (ADCC::TempTxCH[2] <= OT1_HVDC_POSITIVE_RECOVER_LOWTEMP))
			{
				BmsEngine::Time::OT1_HVDC_Positive_time_delay = OT1_HVDC_POSITIVE_RECOVER_TIME;
			}
		}		
	}		//TempTxCH[2] = BCU HVDC+ Temperature
	
	//CheckTempOT2_HVDC_Positive
	void BmsEngine::CheckTempOT2_HVDC_Positive()
	{
		if (BmsEngine::Flag::OT2_HVDC_Positive == 0)
		{
			if (ADCC::TempTxCH[2] >= OT2_HVDC_POSITIVE_CHECK_TEMP)
			{   
				BmsEngine::Flag::ot2_HVDC_Positive_deboc = 1;
				if (BmsEngine::Time::OT2_HVDC_Positive_time_delay == 0)
				{
					BmsEngine::Flag::OT2_HVDC_Positive = 1;
					BmsEngine::Flag::ot2_HVDC_Positive_deboc = 0;
					BmsEngine::Time::OT2_HVDC_Positive_time_delay = OT2_HVDC_POSITIVE_RECOVER_TIME;   					
                    
				}
			}
			else if (ADCC::TempTxCH[2] < OT2_HVDC_POSITIVE_CHECK_TEMP)
			{
				BmsEngine::Time::OT2_HVDC_Positive_time_delay = OT2_HVDC_POSITIVE_CHECK_TIME;
			}
		}

	}		//TempTxCH[2] = BCU HVDC+ Temperature
	
	
	//CheckTempOT1_HVDC_Negative
	void BmsEngine::CheckTempOT1_HVDC_Negative()
	{
		if (BmsEngine::Flag::OT1_HVDC_Negative == 0)
		{
			if ((ADCC::TempTxCH[3] >= OT1_HVDC_NEGATIVE_CHECK_TEMP) || (ADCC::TempTxCH[3] <= OT1_HVDC_NEGATIVE_CHECK_LOWTEMP))
			{   
				BmsEngine::Flag::ot1_HVDC_Negative_deboc = 1;
				if (BmsEngine::Time::OT1_HVDC_Negative_time_delay == 0)
				{
					BmsEngine::Flag::OT1_HVDC_Negative = 1;
					BmsEngine::Flag::ot1_HVDC_Negative_deboc = 0;
					BmsEngine::Time::OT1_HVDC_Negative_time_delay = OT1_HVDC_NEGATIVE_RECOVER_TIME;   					
                    
				}
			}
			else if ((ADCC::TempTxCH[3] < OT1_HVDC_NEGATIVE_CHECK_TEMP) && (ADCC::TempTxCH[3] > OT1_HVDC_NEGATIVE_CHECK_LOWTEMP))
			{
				BmsEngine::Time::OT1_HVDC_Negative_time_delay = OT1_HVDC_NEGATIVE_CHECK_TIME;
			}
		}
		else if (BmsEngine::Flag::OT1_HVDC_Negative == 1)
		{
			if ((ADCC::TempTxCH[3] < OT1_HVDC_NEGATIVE_RECOVER_TEMP) && (ADCC::TempTxCH[3] > OT1_HVDC_NEGATIVE_RECOVER_LOWTEMP))
			{
				BmsEngine::Flag::ot1_HVDC_Negative_deboc = 1;
				if (BmsEngine::Time::OT1_HVDC_Negative_time_delay == 0)
				{
					BmsEngine::Flag::OT1_HVDC_Negative = 0;
					BmsEngine::Time::OT1_HVDC_Negative_time_delay = OT1_HVDC_NEGATIVE_CHECK_TIME;
					BmsEngine::Flag::ot1_HVDC_Negative_deboc = 0;
	                   
				}
			}
			else if ((ADCC::TempTxCH[3] >= OT1_HVDC_NEGATIVE_RECOVER_TEMP)
					|| (ADCC::TempTxCH[3] <= OT1_HVDC_NEGATIVE_RECOVER_LOWTEMP))
			{
				BmsEngine::Time::OT1_HVDC_Negative_time_delay = OT1_HVDC_NEGATIVE_RECOVER_TIME;
			}
		}		
	}		//TempTxCH[3] = BCU HVDC- Temperature
	
	//CheckTempOT2_HVDC_Negative
	void BmsEngine::CheckTempOT2_HVDC_Negative()
	{
		if (BmsEngine::Flag::OT2_HVDC_Negative == 0)
		{
			if (ADCC::TempTxCH[3] >= OT2_HVDC_NEGATIVE_CHECK_TEMP)
			{   
				BmsEngine::Flag::ot2_HVDC_Negative_deboc = 1;
				if (BmsEngine::Time::OT2_HVDC_Negative_time_delay == 0)
				{
					BmsEngine::Flag::OT2_HVDC_Negative = 1;
					BmsEngine::Flag::ot2_HVDC_Negative_deboc = 0;
					BmsEngine::Time::OT2_HVDC_Negative_time_delay = OT2_HVDC_NEGATIVE_RECOVER_TIME;   					
                    
				}
			}
			else if (ADCC::TempTxCH[3] < OT2_HVDC_NEGATIVE_CHECK_TEMP)
			{
				BmsEngine::Time::OT2_HVDC_Negative_time_delay = OT2_HVDC_NEGATIVE_CHECK_TIME;
			}
		}		
	}		//TempTxCH[3] = BCU HVDC- Temperature
	
	
	//==============================================================================
	//OT1_Fuse Check Function
	//Fuse_temp >= 75'c and continue 5sec,OT1_Fuse  event occure
	//Fuse_temp <  65'c and continue 10sec,OT1_Fuse  event recover
	//==============================================================================
	void BmsEngine::CheckTempOT1_Fuse()
	{
		if (BmsEngine::Flag::OT1_Fuse == 0)
		{
			if ((ADCC::TempTxCH[4] >= OT1_FUSE_CHECK_TEMP) || (ADCC::TempTxCH[4] <= OT1_FUSE_CHECK_LOWTEMP))
			{   
				BmsEngine::Flag::ot1_Fuse_deboc = 1;
				if (BmsEngine::Time::OT1_Fuse_time_delay == 0)
				{
					BmsEngine::Flag::OT1_Fuse = 1;
					BmsEngine::Flag::ot1_Fuse_deboc = 0;
					BmsEngine::Time::OT1_Fuse_time_delay = OT1_FUSE_RECOVER_TIME;   					
                    
				}
			}
			else if ((ADCC::TempTxCH[4] < OT1_FUSE_CHECK_TEMP) && (ADCC::TempTxCH[4] > OT1_FUSE_CHECK_LOWTEMP))
			{
				BmsEngine::Time::OT1_Fuse_time_delay = OT1_FUSE_CHECK_TIME;                                             		
			}
		}
		else if (BmsEngine::Flag::OT1_Fuse == 1)
		{
			if ((ADCC::TempTxCH[4] < OT1_FUSE_RECOVER_TEMP) && (ADCC::TempTxCH[4] > OT1_FUSE_RECOVER_LOWTEMP))
			{
				BmsEngine::Flag::ot1_Fuse_deboc = 1;
				if (BmsEngine::Time::OT1_Fuse_time_delay == 0)
				{
					BmsEngine::Flag::OT1_Fuse = 0;
					BmsEngine::Time::OT1_Fuse_time_delay = OT1_FUSE_CHECK_TIME;
					BmsEngine::Flag::ot1_Fuse_deboc = 0;                    
				}
			}
			else if ((ADCC::TempTxCH[4] >= OT1_FUSE_RECOVER_TEMP) || (ADCC::TempTxCH[4] <= OT1_FUSE_RECOVER_LOWTEMP))
			{
				BmsEngine::Time::OT1_Fuse_time_delay = OT1_FUSE_RECOVER_TIME;
			}
		}	
	}			//TempTxCH[4] = BCU Fuse Temperature
	
	//CheckTempOT2_Fuse
	void BmsEngine::CheckTempOT2_Fuse()
	{
		if (BmsEngine::Flag::OT2_Fuse == 0)
		{
			if (ADCC::TempTxCH[4] >= OT2_FUSE_CHECK_TEMP)
			{   
				BmsEngine::Flag::ot2_Fuse_deboc = 1;
				if (BmsEngine::Time::OT2_Fuse_time_delay == 0)
				{
					BmsEngine::Flag::OT2_Fuse = 1;
					BmsEngine::Flag::ot2_Fuse_deboc = 0;
					BmsEngine::Time::OT2_Fuse_time_delay = OT2_FUSE_RECOVER_TIME;   					                     
				}
			}
			else if (ADCC::TempTxCH[4] < OT2_FUSE_CHECK_TEMP)
			{
				BmsEngine::Time::OT2_Fuse_time_delay = OT2_FUSE_CHECK_TIME;                                             		
			}
		}		
	}			//TempTxCH[4] = BCU Fuse Temperature
	
	//==============================================================================
	//OT1_TA Check Function
	//TA_temp >= 85'c or TA_temp <= 0'c and continue 10sec,OT1_TA  event occure
	//TA_temp <  75'c and TA_temp > 5'c and continue 10sec,OT1_TA  event recover
	//==============================================================================	
	void BmsEngine::CheckTempOT1_TA()
	{
		if (BmsEngine::Flag::OT1_TA == 0)
		{
			if ((ADCC::TempTxCH[5] >= OT1_TA_CHECK_TEMP) || (ADCC::TempTxCH[5] <= OT1_TA_CHECK_LOWTEMP))
			{   
				BmsEngine::Flag::ot1_TA_deboc = 1;
				if (BmsEngine::Time::OT1_TA_time_delay == 0)
				{
					BmsEngine::Flag::OT1_TA = 1;
					BmsEngine::Flag::ot1_TA_deboc = 0;
					BmsEngine::Time::OT1_TA_time_delay = OT1_TA_RECOVER_TIME;   					                
				}
			}
			else if ((ADCC::TempTxCH[5] < OT1_TA_CHECK_TEMP) && (ADCC::TempTxCH[5] > OT1_TA_CHECK_LOWTEMP))
			{
				BmsEngine::Time::OT1_TA_time_delay = OT1_TA_CHECK_TIME;                                             		
			}
		}
		else if (BmsEngine::Flag::OT1_TA == 1)
		{
			if ((ADCC::TempTxCH[5] < OT1_TA_RECOVER_TEMP) && (ADCC::TempTxCH[5] > OT1_TA_RECOVER_LOWTEMP))
			{
				BmsEngine::Flag::ot1_TA_deboc = 1;
				if (BmsEngine::Time::OT1_TA_time_delay == 0)
				{
					BmsEngine::Flag::OT1_TA = 0;
					BmsEngine::Time::OT1_TA_time_delay = OT1_TA_CHECK_TIME;
					BmsEngine::Flag::ot1_TA_deboc = 0;
                     
				}
			}
			else if ((ADCC::TempTxCH[5] >= OT1_TA_RECOVER_TEMP) || (ADCC::TempTxCH[5] <= OT1_TA_RECOVER_LOWTEMP))
			{
				BmsEngine::Time::OT1_TA_time_delay = OT1_TA_RECOVER_TIME;
			}
		}	
	}			//TempTxCH[5] = BCU TA Temperature
	
	//==============================================================================
	//Charging state COAT1_Over Ambient Temperature Check Function
	//Pack Ambient_temp >= 35'c and continue 5sec,COAT1_PackAmbient  event occure
	//Pack Ambient_temp <  30'c and continue 5sec,COAT1_PackAmbient  event recover
	//==============================================================================	
	void BmsEngine::CheckTempCOAT1_PackAmbientTemp(int16_t Max_PackAmbient_Temp)
	{
		if ((BatterySys::m_ampReading & 0x80000000) != 0)    //Negative current  Charge Mode
		{
			if (BatterySys::m_ampReading <= CHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::COAT1_PackAmbient == 0)
				{
					if (Max_PackAmbient_Temp >= COAT1_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Flag::COAT1_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::COAT1_PackAmbient_Time_Delay == 0)
						{        	
							BmsEngine::Flag::COAT1_PackAmbient = 1;
							BmsEngine::Time::COAT1_PackAmbient_Time_Delay = COAT1_PACKAMBIENT_RECOVER_TIME;
							BmsEngine::Flag::COAT1_PackAmbient_Debounce = 0;        
						}
					}
					else if (Max_PackAmbient_Temp < COAT1_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Time::COAT1_PackAmbient_Time_Delay = COAT1_PACKAMBIENT_CHECK_TIME;
					}
				}
				else if (BmsEngine::Flag::COAT1_PackAmbient == 1)
				{
					if (Max_PackAmbient_Temp < COAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Flag::COAT1_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::COAT1_PackAmbient_Time_Delay == 0)
						{
							BmsEngine::Flag::COAT1_PackAmbient = 0;
							BmsEngine::Time::COAT1_PackAmbient_Time_Delay = COAT1_PACKAMBIENT_CHECK_TIME;
							BmsEngine::Flag::COAT1_PackAmbient_Debounce = 0;
			               
						}
					}
					else if (Max_PackAmbient_Temp >= COAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Time::COAT1_PackAmbient_Time_Delay = COAT1_PACKAMBIENT_RECOVER_TIME;
					}
				}
			}
			else
			{
				if (BmsEngine::Flag::COAT1_PackAmbient == 1)
				{
					if (Max_PackAmbient_Temp < COAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Flag::COAT1_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::COAT1_PackAmbient_Time_Delay == 0)
						{
							BmsEngine::Flag::COAT1_PackAmbient = 0;
							BmsEngine::Time::COAT1_PackAmbient_Time_Delay = COAT1_PACKAMBIENT_CHECK_TIME;
							BmsEngine::Flag::COAT1_PackAmbient_Debounce = 0;			           
						}
					}
					else if (Max_PackAmbient_Temp >= COAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Time::COAT1_PackAmbient_Time_Delay = COAT1_PACKAMBIENT_RECOVER_TIME;
					}
				}
			}
		}
		else
		{
			if (BmsEngine::Flag::COAT1_PackAmbient == 1)
			{
				if (Max_PackAmbient_Temp < COAT1_PACKAMBIENT_RECOVER_TEMP)
				{
					BmsEngine::Flag::COAT1_PackAmbient_Debounce = 1;
					if (BmsEngine::Time::COAT1_PackAmbient_Time_Delay == 0)
					{
						BmsEngine::Flag::COAT1_PackAmbient = 0;
						BmsEngine::Time::COAT1_PackAmbient_Time_Delay = COAT1_PACKAMBIENT_CHECK_TIME;
						BmsEngine::Flag::COAT1_PackAmbient_Debounce = 0;
				  
					}
				}
				else if (Max_PackAmbient_Temp >= COAT1_PACKAMBIENT_RECOVER_TEMP)
				{
					BmsEngine::Time::COAT1_PackAmbient_Time_Delay = COAT1_PACKAMBIENT_RECOVER_TIME;
				}
			}
		}

	}
	
	//==============================================================================
	//Charging state COAT3_Over Ambient Temperature Check Function
	//Pack Ambient_temp >= 40'c and continue 3sec, COAT3_PackAmbient  event occure
	//System Reset or Clear Flag, COAT3_PackAmbient  event recover
	//==============================================================================	
	void BmsEngine::CheckTempCOAT3_PackAmbientTemp(int16_t Max_PackAmbient_Temp)
	{
		if ((BatterySys::m_ampReading & 0x80000000) != 0)    //Negative current  Charge Mode
		{
			if (BatterySys::m_ampReading <= CHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::COAT3_PackAmbient == 0)
				{
					if (Max_PackAmbient_Temp >= COAT3_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Flag::COAT3_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::COAT3_PackAmbient_Time_Delay == 0)
						{        	
							BmsEngine::Flag::COAT3_PackAmbient = 1;
							BmsEngine::Time::COAT3_PackAmbient_Time_Delay = COAT3_PACKAMBIENT_CHECK_TIME;	
							BmsEngine::Flag::COAT3_PackAmbient_Debounce = 0;  
							BmsEngine::FaultCount = 0;	//NFB Close count down init
						}
					}
					else if (Max_PackAmbient_Temp < COAT3_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Time::COAT3_PackAmbient_Time_Delay = COAT3_PACKAMBIENT_CHECK_TIME;
					}
				}

			}
		}
	}
	
	//==============================================================================
	//Charging state CUAT1_Under Ambient Temperature Check Function
	//Pack Ambient_temp <= 8'c and continue 5sec,CUAT1_PackAmbient  event occure
	//Pack Ambient_temp >  13'c and continue 5sec,CUAT1_PackAmbient  event recover
	//==============================================================================	
	void BmsEngine::CheckTempCUAT1_PackAmbientTemp(int16_t Min_PackAmbient_Temp)
	{
		if ((BatterySys::m_ampReading & 0x80000000) != 0)    //Negative current  Charge Mode
		{
			if (BatterySys::m_ampReading <= CHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::CUAT1_PackAmbient == 0)
				{
					if (Min_PackAmbient_Temp <= CUAT1_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Flag::CUAT1_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::CUAT1_PackAmbient_Time_Delay == 0)
						{        	
							BmsEngine::Flag::CUAT1_PackAmbient = 1;
							BmsEngine::Time::CUAT1_PackAmbient_Time_Delay = CUAT1_PACKAMBIENT_RECOVER_TIME;
							BmsEngine::Flag::CUAT1_PackAmbient_Debounce = 0;        
						}
					}
					else if (Min_PackAmbient_Temp > CUAT1_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Time::CUAT1_PackAmbient_Time_Delay = CUAT1_PACKAMBIENT_CHECK_TIME;
					}
				}
				else if (BmsEngine::Flag::CUAT1_PackAmbient == 1)
				{
					if (Min_PackAmbient_Temp > CUAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Flag::CUAT1_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::CUAT1_PackAmbient_Time_Delay == 0)
						{
							BmsEngine::Flag::CUAT1_PackAmbient = 0;
							BmsEngine::Time::CUAT1_PackAmbient_Time_Delay = CUAT1_PACKAMBIENT_CHECK_TIME;
							BmsEngine::Flag::CUAT1_PackAmbient_Debounce = 0;
			               
						}
					}
					else if (Min_PackAmbient_Temp <= CUAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Time::CUAT1_PackAmbient_Time_Delay = CUAT1_PACKAMBIENT_RECOVER_TIME;
					}
				}
			}
			else
			{
				if (BmsEngine::Flag::CUAT1_PackAmbient == 1)
				{
					if (Min_PackAmbient_Temp > CUAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Flag::CUAT1_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::CUAT1_PackAmbient_Time_Delay == 0)
						{
							BmsEngine::Flag::CUAT1_PackAmbient = 0;
							BmsEngine::Time::CUAT1_PackAmbient_Time_Delay = CUAT1_PACKAMBIENT_CHECK_TIME;
							BmsEngine::Flag::CUAT1_PackAmbient_Debounce = 0;			           
						}
					}
					else if (Min_PackAmbient_Temp <= CUAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Time::CUAT1_PackAmbient_Time_Delay = CUAT1_PACKAMBIENT_RECOVER_TIME;
					}
				}
			}
		}
		else
		{
			if (BmsEngine::Flag::CUAT1_PackAmbient == 1)
			{
				if (Min_PackAmbient_Temp > CUAT1_PACKAMBIENT_RECOVER_TEMP)
				{
					BmsEngine::Flag::CUAT1_PackAmbient_Debounce = 1;
					if (BmsEngine::Time::CUAT1_PackAmbient_Time_Delay == 0)
					{
						BmsEngine::Flag::CUAT1_PackAmbient = 0;
						BmsEngine::Time::CUAT1_PackAmbient_Time_Delay = CUAT1_PACKAMBIENT_CHECK_TIME;
						BmsEngine::Flag::CUAT1_PackAmbient_Debounce = 0;
				  
					}
				}
				else if (Min_PackAmbient_Temp <= CUAT1_PACKAMBIENT_RECOVER_TEMP)
				{
					BmsEngine::Time::CUAT1_PackAmbient_Time_Delay = CUAT1_PACKAMBIENT_RECOVER_TIME;
				}
			}
		}

	}
	
	//==============================================================================
	//Charging state CUAT3_Under Ambient Temperature Check Function
	//Pack Ambient_temp <= 3'c and continue 3sec, CUAT3_PackAmbient  event occure
	//System Reset or Clear Flag, COAT3_PackAmbient  event recover
	//==============================================================================	
	void BmsEngine::CheckTempCUAT3_PackAmbientTemp(int16_t Min_PackAmbient_Temp)
	{
		if ((BatterySys::m_ampReading & 0x80000000) != 0)    //Negative current  Charge Mode
		{
			if (BatterySys::m_ampReading <= CHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::CUAT3_PackAmbient == 0)
				{
					if (Min_PackAmbient_Temp <= CUAT3_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Flag::CUAT3_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::CUAT3_PackAmbient_Time_Delay == 0)
						{        	
							BmsEngine::Flag::CUAT3_PackAmbient = 1;
							BmsEngine::Time::CUAT3_PackAmbient_Time_Delay = CUAT3_PACKAMBIENT_CHECK_TIME; 
							BmsEngine::Flag::CUAT3_PackAmbient_Debounce = 0;      
							BmsEngine::FaultCount = 0; //NFB Close count down init
						}
					}
					else if (Min_PackAmbient_Temp > CUAT3_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Time::CUAT3_PackAmbient_Time_Delay = CUAT3_PACKAMBIENT_CHECK_TIME;
					}
				}

			}
		}
	}
	
	//==============================================================================
	//Discharging state DOAT1_Over Ambient Temperature Check Function
	//Pack Ambient_temp >= 45'c and continue 5sec,DOAT1_PackAmbient  event occure
	//Pack Ambient_temp <  40'c and continue 5sec,DOAT1_PackAmbient  event recover
	//==============================================================================		
	void BmsEngine::CheckTempDOAT1_PackAmbientTemp(int16_t Max_PackAmbient_Temp)
	{
		if ((BatterySys::m_ampReading & 0x80000000) == 0)    //Positive current  Discharge Mode	
		{
			if (BatterySys::m_ampReading >= DISCHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::DOAT1_PackAmbient == 0)
				{
					if (Max_PackAmbient_Temp >= DOAT1_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Flag::DOAT1_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::DOAT1_PackAmbient_Time_Delay == 0)
						{        	
							BmsEngine::Flag::DOAT1_PackAmbient = 1;
							BmsEngine::Time::DOAT1_PackAmbient_Time_Delay = DOAT1_PACKAMBIENT_RECOVER_TIME;
							BmsEngine::Flag::DOAT1_PackAmbient_Debounce = 0;        
						}
					}
					else if (Max_PackAmbient_Temp < DOAT1_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Time::DOAT1_PackAmbient_Time_Delay = DOAT1_PACKAMBIENT_CHECK_TIME;
					}
				}
				else if (BmsEngine::Flag::DOAT1_PackAmbient == 1)
				{
					if (Max_PackAmbient_Temp < DOAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Flag::DOAT1_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::DOAT1_PackAmbient_Time_Delay == 0)
						{
							BmsEngine::Flag::DOAT1_PackAmbient = 0;
							BmsEngine::Time::DOAT1_PackAmbient_Time_Delay = DOAT1_PACKAMBIENT_CHECK_TIME;
							BmsEngine::Flag::DOAT1_PackAmbient_Debounce = 0;
			               
						}
					}
					else if (Max_PackAmbient_Temp >= DOAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Time::DOAT1_PackAmbient_Time_Delay = DOAT1_PACKAMBIENT_RECOVER_TIME;
					}
				}
			}
			else
			{
				if (BmsEngine::Flag::DOAT1_PackAmbient == 1)
				{
					if (Max_PackAmbient_Temp < DOAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Flag::DOAT1_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::DOAT1_PackAmbient_Time_Delay == 0)
						{
							BmsEngine::Flag::DOAT1_PackAmbient = 0;
							BmsEngine::Time::DOAT1_PackAmbient_Time_Delay = DOAT1_PACKAMBIENT_CHECK_TIME;
							BmsEngine::Flag::DOAT1_PackAmbient_Debounce = 0;			           
						}
					}
					else if (Max_PackAmbient_Temp >= DOAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Time::DOAT1_PackAmbient_Time_Delay = DOAT1_PACKAMBIENT_RECOVER_TIME;
					}
				}
			}
		}
		else
		{
			if (BmsEngine::Flag::DOAT1_PackAmbient == 1)
			{
				if (Max_PackAmbient_Temp < DOAT1_PACKAMBIENT_RECOVER_TEMP)
				{
					BmsEngine::Flag::DOAT1_PackAmbient_Debounce = 1;
					if (BmsEngine::Time::DOAT1_PackAmbient_Time_Delay == 0)
					{
						BmsEngine::Flag::DOAT1_PackAmbient = 0;
						BmsEngine::Time::DOAT1_PackAmbient_Time_Delay = DOAT1_PACKAMBIENT_CHECK_TIME;
						BmsEngine::Flag::DOAT1_PackAmbient_Debounce = 0;
				  
					}
				}
				else if (Max_PackAmbient_Temp >= DOAT1_PACKAMBIENT_RECOVER_TEMP)
				{
					BmsEngine::Time::DOAT1_PackAmbient_Time_Delay = DOAT1_PACKAMBIENT_RECOVER_TIME;
				}
			}
		}

	}
	
	//==============================================================================
	//Discharging state DOAT3_Over Ambient Temperature Check Function
	//Pack Ambient_temp >= 50'c and continue 3sec,DOAT3_PackAmbient  event occure
	//System Reset or Clear Flag, DOAT3_PackAmbient  event recover
	//==============================================================================		
	void BmsEngine::CheckTempDOAT3_PackAmbientTemp(int16_t Max_PackAmbient_Temp)
	{
		if ((BatterySys::m_ampReading & 0x80000000) == 0)    //Positive current  Discharge Mode	
		{
			if (BatterySys::m_ampReading >= DISCHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::DOAT3_PackAmbient == 0)
				{
					if (Max_PackAmbient_Temp >= DOAT3_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Flag::DOAT3_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::DOAT3_PackAmbient_Time_Delay == 0)
						{        	
							BmsEngine::Flag::DOAT3_PackAmbient = 1;
							BmsEngine::Time::DOAT3_PackAmbient_Time_Delay = DOAT3_PACKAMBIENT_CHECK_TIME;
							BmsEngine::Flag::DOAT3_PackAmbient_Debounce = 0;       
							BmsEngine::FaultCount = 0; //NFB Close count down init
						}
					}
					else if (Max_PackAmbient_Temp < DOAT3_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Time::DOAT3_PackAmbient_Time_Delay = DOAT3_PACKAMBIENT_CHECK_TIME;
					}
				}
			}	
		}

	}
	
	//==============================================================================
	//Discharging state DUAT1_Under Ambient Temperature Check Function
	//Pack Ambient_temp <= -5'c and continue 5sec,DUAT1_PackAmbient  event occure
	//Pack Ambient_temp >  0'c and continue 5sec,DUAT1_PackAmbient  event recover
	//==============================================================================		
	void BmsEngine::CheckTempDUAT1_PackAmbientTemp(int16_t Min_PackAmbient_Temp)
	{
		if ((BatterySys::m_ampReading & 0x80000000) == 0)    //Positive current  Discharge Mode	
		{
			if (BatterySys::m_ampReading >= DISCHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::DUAT1_PackAmbient == 0)
				{
					if (Min_PackAmbient_Temp <= DUAT1_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Flag::DUAT1_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::DUAT1_PackAmbient_Time_Delay == 0)
						{        	
							BmsEngine::Flag::DUAT1_PackAmbient = 1;
							BmsEngine::Time::DUAT1_PackAmbient_Time_Delay = DUAT1_PACKAMBIENT_RECOVER_TIME;
							BmsEngine::Flag::DUAT1_PackAmbient_Debounce = 0;        
						}
					}
					else if (Min_PackAmbient_Temp > DUAT1_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Time::DUAT1_PackAmbient_Time_Delay = DUAT1_PACKAMBIENT_CHECK_TIME;
					}
				}
				else if (BmsEngine::Flag::DUAT1_PackAmbient == 1)
				{
					if (Min_PackAmbient_Temp > DUAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Flag::DUAT1_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::DUAT1_PackAmbient_Time_Delay == 0)
						{
							BmsEngine::Flag::DUAT1_PackAmbient = 0;
							BmsEngine::Time::DUAT1_PackAmbient_Time_Delay = DUAT1_PACKAMBIENT_CHECK_TIME;
							BmsEngine::Flag::DUAT1_PackAmbient_Debounce = 0;
			               
						}
					}
					else if (Min_PackAmbient_Temp <= DUAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Time::DUAT1_PackAmbient_Time_Delay = DUAT1_PACKAMBIENT_RECOVER_TIME;
					}
				}
			}
			else
			{
				if (BmsEngine::Flag::DUAT1_PackAmbient == 1)
				{
					if (Min_PackAmbient_Temp > DUAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Flag::DUAT1_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::DUAT1_PackAmbient_Time_Delay == 0)
						{
							BmsEngine::Flag::DUAT1_PackAmbient = 0;
							BmsEngine::Time::DUAT1_PackAmbient_Time_Delay = DUAT1_PACKAMBIENT_CHECK_TIME;
							BmsEngine::Flag::DUAT1_PackAmbient_Debounce = 0;			           
						}
					}
					else if (Min_PackAmbient_Temp <= DUAT1_PACKAMBIENT_RECOVER_TEMP)
					{
						BmsEngine::Time::DUAT1_PackAmbient_Time_Delay = DUAT1_PACKAMBIENT_RECOVER_TIME;
					}
				}
			}
		}
		else
		{
			if (BmsEngine::Flag::DUAT1_PackAmbient == 1)
			{
				if (Min_PackAmbient_Temp > DUAT1_PACKAMBIENT_RECOVER_TEMP)
				{
					BmsEngine::Flag::DUAT1_PackAmbient_Debounce = 1;
					if (BmsEngine::Time::DUAT1_PackAmbient_Time_Delay == 0)
					{
						BmsEngine::Flag::DUAT1_PackAmbient = 0;
						BmsEngine::Time::DUAT1_PackAmbient_Time_Delay = DUAT1_PACKAMBIENT_CHECK_TIME;
						BmsEngine::Flag::DUAT1_PackAmbient_Debounce = 0;
				  
					}
				}
				else if (Min_PackAmbient_Temp <= DUAT1_PACKAMBIENT_RECOVER_TEMP)
				{
					BmsEngine::Time::DUAT1_PackAmbient_Time_Delay = DUAT1_PACKAMBIENT_RECOVER_TIME;
				}
			}
		}

	}
	
	//==============================================================================
	//Discharging state DUAT3_Under Ambient Temperature Check Function
	//Pack Ambient_temp <= -10'c and continue 3sec, DUAT3_PackAmbient  event occure
	//System Reset or Clear Flag, DUAT3_PackAmbient  event recover
	//==============================================================================		
	void BmsEngine::CheckTempDUAT3_PackAmbientTemp(int16_t Min_PackAmbient_Temp)
	{
		if ((BatterySys::m_ampReading & 0x80000000) == 0)    //Positive current  Discharge Mode	
		{
			if (BatterySys::m_ampReading >= DISCHARGE_CURRENT_THRESHOULD)
			{	
				if (BmsEngine::Flag::DUAT3_PackAmbient == 0)
				{
					if (Min_PackAmbient_Temp <= DUAT3_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Flag::DUAT3_PackAmbient_Debounce = 1;
						if (BmsEngine::Time::DUAT3_PackAmbient_Time_Delay == 0)
						{        	
							BmsEngine::Flag::DUAT3_PackAmbient = 1;
							BmsEngine::Time::DUAT3_PackAmbient_Time_Delay = DUAT3_PACKAMBIENT_CHECK_TIME;	
							BmsEngine::Flag::DUAT3_PackAmbient_Debounce = 0;     
							BmsEngine::FaultCount = 0; //NFB Close count down init
						}
					}
					else if (Min_PackAmbient_Temp > DUAT3_PACKAMBIENT_CHECK_TEMP) 
					{
						BmsEngine::Time::DUAT3_PackAmbient_Time_Delay = DUAT3_PACKAMBIENT_CHECK_TIME;
					}
				}
	
			}
		}
	
	}
	
	//==============================================================================
	// 5% SOC Calibration Check 
	//min_vol <= 3.0V and SOC > 5% ，continue 6sec , SOC5% event occure
	//min_vol >= 3.1V and continue 6sec , SOC5% event recover
	//==============================================================================
	void BmsEngine::CheckSOC5_Calibration()
	{	
		if (BmsEngine::Flag::SOC_CAL5 == 0)
		{
			if ((BatterySys::MinVolt <= SOC5_CHECK_Volt) && (BmsEngine::m_soc > SOC_CHECK_5))
			{
				BmsEngine::Flag::SOC_CAL5_deboc = 1;
				if (BmsEngine::Time::SOC_CAL5_time_delay == 0)
				{
					BmsEngine::Flag::SOC_CAL5 = 1;
					BmsEngine::Flag::SOC_CAL5_deboc = 0;
					BmsEngine::Time::SOC_CAL5_time_delay = SOC5_RECOVER_TIME;
					BmsEngine::Flag::SOC5_Calibration_START = 1;
				}                                                                                                                                                                                    
			}
			else if ((BatterySys::MinVolt > SOC5_CHECK_Volt) || (BmsEngine::m_soc <= SOC_CHECK_5))
			{
				BmsEngine::Time::SOC_CAL5_time_delay = SOC5_CHECK_TIME;                                   		
			}
		}
		else if (BmsEngine::Flag::SOC_CAL5 == 1)
		{
			if (BatterySys::MinVolt >= SOC5_RECOVER_Volt)
			{
				BmsEngine::Flag::SOC_CAL5_deboc = 1;
				if (BmsEngine::Time::SOC_CAL5_time_delay == 0)
				{
					BmsEngine::Flag::SOC_CAL5 = 0;
					BmsEngine::Time::SOC_CAL5_time_delay = SOC5_CHECK_TIME;
					BmsEngine::Flag::SOC_CAL5_deboc = 0;
				
				}
			}
			else if (BatterySys::MinVolt < SOC5_RECOVER_Volt)
			{
				BmsEngine::Time::SOC_CAL5_time_delay = SOC5_RECOVER_TIME;
			}
		}
		
	}
	
	//==============================================================================
	// 95% SOC Calibration Check 
	//max_vol >= 3.36V and SOC < 95% ，continue 6sec , SOC95% event occure
	//max_vol <= 3.3V and continue 6sec , SOC95% event recover
	//==============================================================================
	void BmsEngine::CheckSOC95_Calibration()
	{	
		if (BmsEngine::Flag::SOC_CAL95 == 0)
		{
			if ((BatterySys::MaxVolt >= SOC95_CHECK_Volt) && (BmsEngine::m_soc < SOC_CHECK_95))
			{
				BmsEngine::Flag::SOC_CAL95_deboc = 1;
				if (BmsEngine::Time::SOC_CAL95_time_delay == 0)
				{
					BmsEngine::Flag::SOC_CAL95 = 1;
					BmsEngine::Flag::SOC_CAL95_deboc = 0;
					BmsEngine::Time::SOC_CAL95_time_delay = SOC95_RECOVER_TIME;
					BmsEngine::Flag::SOC95_Calibration_START = 1; 
				}                                                                                                                                                                                    
			}
			else if ((BatterySys::MaxVolt < SOC95_CHECK_Volt) || (BmsEngine::m_soc >= SOC_CHECK_95))
			{
				BmsEngine::Time::SOC_CAL95_time_delay = SOC95_CHECK_TIME;                                   		
			}
		}
		else if (BmsEngine::Flag::SOC_CAL95 == 1)
		{
			if (BatterySys::MaxVolt <= SOC95_RECOVER_Volt)
			{
				BmsEngine::Flag::SOC_CAL95_deboc = 1;
				if (BmsEngine::Time::SOC_CAL95_time_delay == 0)
				{
					BmsEngine::Flag::SOC_CAL95 = 0;
					BmsEngine::Time::SOC_CAL95_time_delay = SOC95_CHECK_TIME;
					BmsEngine::Flag::SOC_CAL95_deboc = 0;
				
				}
			}
			else if (BatterySys::MaxVolt > SOC95_RECOVER_Volt)
			{
				BmsEngine::Time::SOC_CAL95_time_delay = SOC95_RECOVER_TIME;
			}
		}
		
	}
	
	//ClearCanTimeoutCount
	void BmsEngine::ClearCanTimeoutCount()
	{
		for (int i = 0; i < PackConst::MAX_PACK_NUM; i++)//
		{
			for (int j = 0; j < PacketConst::CELL_VOLT_NUM; j++)//
			{
				BatterySys::pack[i].timeoutCount[j] = 0;
				BatterySys::pack[i].btimeout[j] = false;
				BatterySys::pack[i].bexist = false;
			}
		}
	}

	//UpdateTimeoutCount
	void BmsEngine::UpdateTimeoutCount()
	{
		for (int i = 0; i < PackConst::MAX_PACK_NUM; i++)//
		{
			for (int j = 0; j < PacketConst::CELL_VOLT_NUM; j++)//
			{
				if (BatterySys::pack[i].timeoutCount[j] < CAN_PACKET_CHECK_TIMEOUT)
				{
					BatterySys::pack[i].timeoutCount[j]++;

					if (BatterySys::pack[i].timeoutCount[j] >= CAN_PACKET_CHECK_TIMEOUT)
					{
						BatterySys::pack[i].btimeout[j] = true;
					}
				}
			}
		}
	}

//	uint16_t BmsEngine::CheckCellMonitorCanBusWire(const uint16_t start_idx, const uint16_t stop_idx)
//	{
//		m_timeoutCount = 0;
//
//		UpdateTimeoutCount();
//
//		for (int i = start_idx; i < stop_idx; i++)
//		{
//			for (int j = 0; j < PacketConst::CELL_VOLT_NUM; j++)
//			{
//				if (BatterySys::pack[i].btimeout[j])
//				{
//					m_timeoutCount++;
////					p_bmspara->bms_memory[PACK_STATUS_OFFSET + i] |= BCU_STATUS_CMCAN_FAULT;
//					BatterySys::pack[i].bexist = false;
//
//					BatterySys::pack[i].cellVolt[j] = 1;
//					BatterySys::pack[i].cellVolt[j + 4] = 1;
//
//					if(j < PackConst::MAX_TEMP_NUM)
//					{
//						BatterySys::pack[i].cellTemp[j] = -127;
//						////BatterySys::pack[i].bicTemp[j] = -127;
//					}
//				}
//				else
//				{
////					p_bmspara->bms_memory[PACK_STATUS_OFFSET + i] &= ~BCU_STATUS_CMCAN_FAULT;
//					BatterySys::pack[i].bexist = true;
//					
//				}
//			}
//		}
//
//		if (m_timeoutCount)
//		{
//			p_bmspara->m_bcuStatus |= BCU_STATUS_CMCAN_FAULT;
//
//			if (!canTimeoutFlag) LogEvent(BCU_STATUS_CMCAN_FAULT);
//
//			canTimeoutFlag = true;
//		}
//		else
//		{
//			p_bmspara->m_bcuStatus &= ~BCU_STATUS_CMCAN_FAULT;
//			canTimeoutFlag = false;
//		}
//
////		p_bmspara->bms_memory[PACK_STATUS_OVERALL] = p_bmspara->m_bcuStatus;
//
//		return (p_bmspara->m_bcuStatus);
//	}

//	uint16_t BmsEngine::CheckCellMonitorCanBusWire()
//	{
//#if defined(CAN_LOSS_PROTECTION_ENABLE)
//		m_timeoutCount = 0;
//
//		UpdateTimeoutCount();
//
//		for (int i = 0; i < PackConst::MAX_PACK_NUM; i++)
//		{
//			for (int j = 0; j < PacketConst::CELL_VOLT_NUM; j++)
//			{
//				if (BatterySys::pack[i].btimeout[j])
//				{
//					m_timeoutCount++;
////					p_bmspara->bms_memory[PACK_STATUS_OFFSET + i] |= BCU_STATUS_CMCAN_FAULT;
//					BatterySys::pack[i].bexist = false;
//
//					BatterySys::pack[i].cellVolt[j] = 1;
//					BatterySys::pack[i].cellVolt[j + 3] = 1;
//					BatterySys::pack[i].cellVolt[j + 6] = 1;
//					BatterySys::pack[i].cellVolt[j + 9] = 1;
//					
//					if(j < PackConst::MAX_TEMP_NUM)
//					{
//						BatterySys::pack[i].cellTemp[j] = -127;
//						BatterySys::pack[i].cellTemp[j+3] = -127;
//						BatterySys::pack[i].cellTemp[j+6] = -127;
//						BatterySys::pack[i].cellTemp[j+9] = -127;
//						////						BatterySys::pack[i].bicTemp[j] = -127;
//					}
//				}
//				else
//				{
////					p_bmspara->bms_memory[PACK_STATUS_OFFSET + i] &= ~BCU_STATUS_CMCAN_FAULT;
//	
//					BatterySys::pack[i].bexist = true;
//				}
//			}
//		}
//
//		if (m_timeoutCount)
//		{
//			p_bmspara->m_bcuStatus |= BCU_STATUS_CMCAN_FAULT;
//			
//			BmsEngine::BMUCANFAULTFlag = 1;
//			
//			if (!canTimeoutFlag) LogEvent(BCU_STATUS_CMCAN_FAULT);
//
//			canTimeoutFlag = true;
//		}
//		else
//		{
//			p_bmspara->m_bcuStatus &= ~BCU_STATUS_CMCAN_FAULT;
//
//			canTimeoutFlag = false;
//		}
//
////		p_bmspara->bms_memory[PACK_STATUS_OVERALL] = p_bmspara->m_bcuStatus;
//
//		return (p_bmspara->m_bcuStatus);
//#else
//		m_timeoutCount = 0;
//
//		UpdateTimeoutCount();
//
//		for (int i = 0; i < PackConst::MAX_PACK_NUM; i++)
//		{
//			for (int j = 0; j < PacketConst::CELL_VOLT_NUM; j++)
//			{
//				if (BatterySys::pack[i].btimeout[j])
//				{
//					m_timeoutCount++;
//					p_bmspara->bms_memory[PACK_STATUS_OFFSET + i] |= BCU_STATUS_CMCAN_FAULT;
//				}
//				//else
//				//{
//				//	p_bmspara->bms_memory[PACK_STATUS_OFFSET + i] &= ~BCU_STATUS_WIRE;
//				//}
//			}
//		}
//		//do not update p_bmspara->m_bcuStatus: no protection in FSM
//		//==============================================================
//		//if (m_timeoutCount) p_bmspara->m_bcuStatus |= BCU_STATUS_WIRE;
//		//else 			p_bmspara->m_bcuStatus &= ~BCU_STATUS_WIRE;
//		if(m_timeoutCount)
//		{
//			p_bmspara->bms_memory[PACK_STATUS_OVERALL] |= BCU_STATUS_CMCAN_FAULT;
//
//			if (!canTimeoutFlag) LogEvent(BCU_STATUS_CMCAN_FAULT);
//
//			canTimeoutFlag = true;
//
//			return ((p_bmspara->m_bcuStatus | BCU_STATUS_CMCAN_FAULT));
//		}
//		else
//		{
//			//DENNIS: 2017/7/10 Latch the status using(0x50FE to clear fault)
//			//p_bmspara->m_bcuStatus &= ~BCU_STATUS_WIRE;
//			canTimeoutFlag = false;
//			return (p_bmspara->m_bcuStatus);
//		}
//
//#endif
//	}
	
	uint16_t BmsEngine::CheckCurrentCanBusWire()
	{
		if (BmsEngine::Flag::CCF == 0)
		{
			if (BatterySys::canCurrCounter2nd == 0)
			{
				BmsEngine::Flag::current_deboc = 1;
				if (BmsEngine::Time::current_time_delay == 0)
				{
					BatterySys::CurrCountFlag = 0;
					BmsEngine::Flag::CCF = 1;
					BmsEngine::Flag::current_deboc = 0;
					BmsEngine::Time::current_time_delay = CSCF_CHECK_TIME; 			
					BmsEngine::ampReading_mA =  0;
					BatterySys::m_ampReading = 0;
					p_bmspara->m_bcuStatus |= CURRENT_STATUS_CAN_FAULT;    
					BmsEngine::FaultCount = 0; //NFB Close count down init
				}
			}
			else
			{
				BmsEngine::Time::current_time_delay = CSCF_CHECK_TIME; 
			}
		}
		else if (BmsEngine::Flag::CCF == 1)
		{
			if (BatterySys::CurrCountFlag == 1)
			{
				BmsEngine::Flag::current_deboc = 1;
				if (BmsEngine::Time::current_time_delay == 0)
				{
					BmsEngine::Flag::CCF = 0;
					BmsEngine::Flag::current_deboc = 0;   					
					BmsEngine::Time::current_time_delay = CSCF_RECOVER_TIME;			
					p_bmspara->m_bcuStatus &= ~CURRENT_STATUS_CAN_FAULT;                           			
				}
			}
			else
			{
				BmsEngine::Time::current_time_delay = CSCF_RECOVER_TIME;
			}
		}
		return (p_bmspara->m_bcuStatus);
	}

	//CheckEMSCanBusWire
	uint16_t BmsEngine::CheckEMSCanBusWire()
	{
		if (BmsEngine::Flag::ECF == 0)
		{
			if (BatterySys::canEmsCounter2nd == 0)
			{
				BmsEngine::Flag::ems_deboc = 1;
				if (BmsEngine::Time::ems_time_delay == 0)
				{
					BatterySys::EmsCountFlag = 0;
					BmsEngine::Flag::ECF = 1;
					BmsEngine::Flag::ems_deboc = 0;
					BmsEngine::Time::ems_time_delay = ESCF_RECOVER_TIME; 					         
					p_bmspara->m_bcuStatus |= EMS_STATUS_MBUCAN_FAULT;   
					BmsEngine::FaultCount = 0; //NFB Close count down init
				}
			}
			else
			{
				BmsEngine::Time::ems_time_delay = ESCF_CHECK_TIME; 
			}
		}
		else if (BmsEngine::Flag::ECF == 1)
		{
			if (BatterySys::EmsCountFlag == 1)
			{
				BmsEngine::Flag::ems_deboc = 1;
				if (BmsEngine::Time::ems_time_delay == 0)
				{
					BmsEngine::Flag::ECF = 0;
					BmsEngine::Flag::ems_deboc = 0;   					
					BmsEngine::Time::ems_time_delay = ESCF_CHECK_TIME;				
					p_bmspara->m_bcuStatus &= ~EMS_STATUS_MBUCAN_FAULT;                           				
				}
			}
			else
			{
				BmsEngine::Time::ems_time_delay = ESCF_RECOVER_TIME;
			}
		}
		return (p_bmspara->m_bcuStatus);
	}
	
	//CheckCurrCanCounter
	void BmsEngine::CheckCurrCanCounter(void)
	{
		if (BatterySys::canCurrCounter <= BatterySys::canCurrCounter2nd)
		{
			BatterySys::CurrCountFlag = 0;
		}
	
		if (BatterySys::CurrCountFlag == 1)
		{
			BatterySys::canCurrCounter2nd++;
			BatterySys::canCurrCounter = 0;	
		}
		else if (BatterySys::CurrCountFlag == 0)
		{
			BatterySys::canCurrCounter2nd = 0;
			BatterySys::canCurrCounter = 0;
		}
	}
	
	//CheckEmsCanCounter
	void BmsEngine::CheckEmsCanCounter(void)
	{
		if (BatterySys::canEmsCounter <= BatterySys::canEmsCounter2nd)
		{
			BatterySys::EmsCountFlag = 0;
		}
	
		if (BatterySys::EmsCountFlag == 1)
		{
			BatterySys::canEmsCounter2nd++;
			BatterySys::canEmsCounter = 0;	
		}
		else if (BatterySys::EmsCountFlag == 0)
		{
			BatterySys::canEmsCounter2nd = 0;
			BatterySys::canEmsCounter = 0;
		}
	}
	
	//Check BMU communication
	void BmsEngine::CheckIsoSPIcommunication(void)
	{
		if ((LTC6811SPI::communication_Volt_counter_Flag == 1) || (LTC6811SPI::communication_Temp_counter_Flag == 1))        
		{
			LTC6811SPI::communication_Volt_counter++;
			LTC6811SPI::communication_Volt_counter_Flag = 0;
			LTC6811SPI::communication_Temp_counter_Flag = 0;
			
			if (LTC6811SPI::communication_Volt_counter >= ISOSPI_COMMUNICATION_COUNTER)
			{
				p_bmspara->m_bcuStatus |= BCU_STATUS_CMCAN_FAULT;

	
			}
			if (LTC6811SPI::communication_Volt_counter >= MAX_ISOSPI_COMMUNICATION_COUNTER)
			{
				LTC6811SPI::communication_Volt_counter = MAX_ISOSPI_COMMUNICATION_COUNTER;
			}
		}
		else
		{
			if (LTC6811SPI::communication_Volt_counter > 0)
			{
				LTC6811SPI::communication_Volt_counter--;
			}
			else
			{
				LTC6811SPI::communication_Volt_counter = 0;
				p_bmspara->m_bcuStatus &= ~BCU_STATUS_CMCAN_FAULT;
			}
		}
	}

	//CheckT500msCountdown
	void BmsEngine::CheckT500msCountdown(void)
	{
	
		//銝���V霅血��
		if (BmsEngine::Flag::ov1_deboc == 1)
		{
			if (BmsEngine::Time::OV1_time_delay > 0)
			{
				BmsEngine::Time::OV1_time_delay -= 1; //Count down
			}
		}
		
		//銝���V霅血��
		if (BmsEngine::Flag::uv1_deboc == 1)
		{
			if (BmsEngine::Time::UV1_time_delay > 0)
			{
				BmsEngine::Time::UV1_time_delay -= 1; //Count down
			}
		}
		
		//銝���TC霅血��
		if (BmsEngine::Flag::otc1_deboc == 1)
		{
			if (BmsEngine::Time::OTC1_time_delay > 0)
			{
				BmsEngine::Time::OTC1_time_delay -= 1; //Count down
			}
		}
		
		//銝���TC霅血��
		if (BmsEngine::Flag::utc1_deboc == 1)
		{
			if (BmsEngine::Time::UTC1_time_delay > 0)
			{
				BmsEngine::Time::UTC1_time_delay -= 1; //Count down
			}
		}
		
		//銝���TD霅血��
		if (BmsEngine::Flag::otd1_deboc == 1)
		{
			if (BmsEngine::Time::OTD1_time_delay > 0)
			{
				BmsEngine::Time::OTD1_time_delay -= 1; //Count down
			}
		}
		
		//銝���TD霅血��
		if (BmsEngine::Flag::utd1_deboc == 1)
		{
			if (BmsEngine::Time::UTD1_time_delay > 0)
			{
				BmsEngine::Time::UTD1_time_delay -= 1; //Count down
			}
		}
		
		//銝���CC霅血��
		if (BmsEngine::Flag::occ1_deboc == 1)
		{
			if (BmsEngine::Time::OCC1_time_delay > 0)
			{
				BmsEngine::Time::OCC1_time_delay -= 1; //Count down
			}
		}
		
		//銝���CD霅血��
		if (BmsEngine::Flag::ocd1_deboc == 1)
		{
			if (BmsEngine::Time::OCD1_time_delay > 0)
			{
				BmsEngine::Time::OCD1_time_delay -= 1; //Count down
			}
		}
		
		//銝���t1_PCS_Positive霅血��
		if (BmsEngine::Flag::ot1_PCS_Positive_deboc == 1)
		{
			if (BmsEngine::Time::OT1_PCS_Positive_time_delay > 0)
			{
				BmsEngine::Time::OT1_PCS_Positive_time_delay -= 1; //Count down
			}
		}
		
		//銝���t1_PCS_Negative霅血��
		if (BmsEngine::Flag::ot1_PCS_Negative_deboc == 1)
		{
			if (BmsEngine::Time::OT1_PCS_Negative_time_delay > 0)
			{
				BmsEngine::Time::OT1_PCS_Negative_time_delay -= 1; //Count down
			}
		}
		
		//銝���t1_HVDC_Positive霅血��
		if (BmsEngine::Flag::ot1_HVDC_Positive_deboc == 1)
		{
			if (BmsEngine::Time::OT1_HVDC_Positive_time_delay > 0)
			{
				BmsEngine::Time::OT1_HVDC_Positive_time_delay -= 1; //Count down
			}
		}
		
		//銝���t1_HVDC_Negative霅血��
		if (BmsEngine::Flag::ot1_HVDC_Negative_deboc == 1)
		{
			if (BmsEngine::Time::OT1_HVDC_Negative_time_delay > 0)
			{
				BmsEngine::Time::OT1_HVDC_Negative_time_delay -= 1; //Count down
			}
		}
		
		//銝���t1_Fuse霅血��
		if (BmsEngine::Flag::ot1_Fuse_deboc == 1)
		{
			if (BmsEngine::Time::OT1_Fuse_time_delay > 0)
			{
				BmsEngine::Time::OT1_Fuse_time_delay -= 1; //Count down
			}
		}
		
		//銝���t1_TA霅血��
		if (BmsEngine::Flag::ot1_TA_deboc == 1)
		{
			if (BmsEngine::Time::OT1_TA_time_delay > 0)
			{
				BmsEngine::Time::OT1_TA_time_delay -= 1; //Count down
			}
		}
				
		//銝���OAT1_PackAmbient霅血��
		if (BmsEngine::Flag::COAT1_PackAmbient_Debounce == 1)
		{
			if (BmsEngine::Time::COAT1_PackAmbient_Time_Delay > 0)
			{
				BmsEngine::Time::COAT1_PackAmbient_Time_Delay -= 1; //Count down
			}
		}
		
		//銝���UAT1_PackAmbient霅血��
		if (BmsEngine::Flag::CUAT1_PackAmbient_Debounce == 1)
		{
			if (BmsEngine::Time::CUAT1_PackAmbient_Time_Delay > 0)
			{
				BmsEngine::Time::CUAT1_PackAmbient_Time_Delay -= 1; //Count down
			}
		}
		
		//銝���OAT1_PackAmbient霅血��
		if (BmsEngine::Flag::DOAT1_PackAmbient_Debounce == 1)
		{
			if (BmsEngine::Time::DOAT1_PackAmbient_Time_Delay > 0)
			{
				BmsEngine::Time::DOAT1_PackAmbient_Time_Delay -= 1; //Count down
			}
		}
		
		//銝���UAT1_PackAmbient霅血��
		if (BmsEngine::Flag::DUAT1_PackAmbient_Debounce == 1)
		{
			if (BmsEngine::Time::DUAT1_PackAmbient_Time_Delay > 0)
			{
				BmsEngine::Time::DUAT1_PackAmbient_Time_Delay -= 1; //Count down
			}
		}
		

		//鈭�V靽風
		if (BmsEngine::Flag::ov2_deboc == 1)
		{
			if (BmsEngine::Time::OV2_time_delay > 0)
			{
				BmsEngine::Time::OV2_time_delay -= 1; //Count down
			}
		}

		//鈭�V靽風
		if (BmsEngine::Flag::uv2_deboc == 1)
		{
			if (BmsEngine::Time::UV2_time_delay > 0)
			{
				BmsEngine::Time::UV2_time_delay -= 1; //Count down
			}
		}
		
		//鈭�TC靽風
		if (BmsEngine::Flag::otc2_deboc == 1)
		{
			if (BmsEngine::Time::OTC2_time_delay > 0)
			{
				BmsEngine::Time::OTC2_time_delay -= 1; //Count down
			}
		}
		
		//鈭�TC靽風
		if (BmsEngine::Flag::utc2_deboc == 1)
		{
			if (BmsEngine::Time::UTC2_time_delay > 0)
			{
				BmsEngine::Time::UTC2_time_delay -= 1; //Count down
			}
		}
		
		//鈭�TD靽風
		if (BmsEngine::Flag::otd2_deboc == 1)
		{
			if (BmsEngine::Time::OTD2_time_delay > 0)
			{
				BmsEngine::Time::OTD2_time_delay -= 1; //Count down
			}
		}
		
		//鈭�TD靽風
		if (BmsEngine::Flag::utd2_deboc == 1)
		{
			if (BmsEngine::Time::UTD2_time_delay > 0)
			{
				BmsEngine::Time::UTD2_time_delay -= 1; //Count down
			}
		}
		
		//鈭�CC靽風
		if (BmsEngine::Flag::occ2_deboc == 1)
		{
			if (BmsEngine::Time::OCC2_time_delay > 0)
			{
				BmsEngine::Time::OCC2_time_delay -= 1; //Count down
			}
		}
		
		//鈭�CD靽風
		if (BmsEngine::Flag::ocd2_deboc == 1)
		{
			if (BmsEngine::Time::OCD2_time_delay > 0)
			{
				BmsEngine::Time::OCD2_time_delay -= 1; //Count down
			}
		}
		
		//鈭�t2_PCS_Positive靽風
		if (BmsEngine::Flag::ot2_PCS_Positive_deboc == 1)
		{
			if (BmsEngine::Time::OT2_PCS_Positive_time_delay > 0)
			{
				BmsEngine::Time::OT2_PCS_Positive_time_delay -= 1; //Count down
			}
		}
		
		//鈭�t2_PCS_Negative靽風
		if (BmsEngine::Flag::ot2_PCS_Negative_deboc == 1)
		{
			if (BmsEngine::Time::OT2_PCS_Negative_time_delay > 0)
			{
				BmsEngine::Time::OT2_PCS_Negative_time_delay -= 1; //Count down
			}
		}
		
		//鈭�t2_HVDC_Positive靽風
		if (BmsEngine::Flag::ot2_HVDC_Positive_deboc == 1)
		{
			if (BmsEngine::Time::OT2_HVDC_Positive_time_delay > 0)
			{
				BmsEngine::Time::OT2_HVDC_Positive_time_delay -= 1; //Count down
			}
		}
		
		//鈭�t2_HVDC_Negative靽風
		if (BmsEngine::Flag::ot2_HVDC_Negative_deboc == 1)
		{
			if (BmsEngine::Time::OT2_HVDC_Negative_time_delay > 0)
			{
				BmsEngine::Time::OT2_HVDC_Negative_time_delay -= 1; //Count down
			}
		}
		
		//鈭�t2_Fuse靽風
		if (BmsEngine::Flag::ot2_Fuse_deboc == 1)
		{
			if (BmsEngine::Time::OT2_Fuse_time_delay > 0)
			{
				BmsEngine::Time::OT2_Fuse_time_delay -= 1; //Count down
			}
		}
		
		//銝�V靽風
		if (BmsEngine::Flag::ov3_deboc == 1)
		{
			if (BmsEngine::Time::OV3_time_delay > 0)
			{
				BmsEngine::Time::OV3_time_delay -= 1; //Count down
			}
		}
		
		//銝�V靽風
		if (BmsEngine::Flag::uv3_deboc == 1)
		{
			if (BmsEngine::Time::UV3_time_delay > 0)
			{
				BmsEngine::Time::UV3_time_delay -= 1; //Count down
			}
		}
		
		//銝�TC靽風
		if (BmsEngine::Flag::otc3_deboc == 1)
		{
			if (BmsEngine::Time::OTC3_time_delay > 0)
			{
				BmsEngine::Time::OTC3_time_delay -= 1; //Count down
			}
		}
		
		//銝�TC靽風
		if (BmsEngine::Flag::utc3_deboc == 1)
		{
			if (BmsEngine::Time::UTC3_time_delay > 0)
			{
				BmsEngine::Time::UTC3_time_delay -= 1; //Count down
			}
		}
		
		//銝�TD靽風
		if (BmsEngine::Flag::otd3_deboc == 1)
		{
			if (BmsEngine::Time::OTD3_time_delay > 0)
			{
				BmsEngine::Time::OTD3_time_delay -= 1; //Count down
			}
		}
		
		//銝�TD靽風
		if (BmsEngine::Flag::utd3_deboc == 1)
		{
			if (BmsEngine::Time::UTD3_time_delay > 0)
			{
				BmsEngine::Time::UTD3_time_delay -= 1; //Count down
			}
		}
		
		//銝�CC靽風
		if (BmsEngine::Flag::occ3_deboc == 1)
		{
			if (BmsEngine::Time::OCC3_time_delay > 0)
			{
				BmsEngine::Time::OCC3_time_delay -= 1; //Count down
			}
		}
		
		//銝�CD靽風
		if (BmsEngine::Flag::ocd3_deboc == 1)
		{
			if (BmsEngine::Time::OCD3_time_delay > 0)
			{
				BmsEngine::Time::OCD3_time_delay -= 1; //Count down
			}
		}

		//IV霅血��
		if (BmsEngine::Flag::iv1_deboc == 1)
		{
			if (BmsEngine::Time::IV1_time_delay > 0)
			{
				BmsEngine::Time::IV1_time_delay -= 1; //Count down
			}
		}
		
		//current���風
		if (BmsEngine::Flag::current_deboc == 1)
		{
			if (BmsEngine::Time::current_time_delay > 0)
			{
				BmsEngine::Time::current_time_delay -= 1; //Count down
			}
		}
		
		//BSU���風
		if (BmsEngine::Flag::ems_deboc == 1)
		{
			if (BmsEngine::Time::ems_time_delay > 0)
			{
				BmsEngine::Time::ems_time_delay -= 1; //Count down
			}
		}

		if (BmsEngine::Flag::chg_e_deboc == 1)
		{
			if (BmsEngine::Time::CHG_E_time_delay > 0)
			{
				BmsEngine::Time::CHG_E_time_delay -= 1; //Count down
			}
		}
		
		if (BmsEngine::Flag::dsg_e_deboc == 1)
		{
			if (BmsEngine::Time::DSG_E_time_delay > 0)
			{
				BmsEngine::Time::DSG_E_time_delay -= 1; //Count down
			}
		}
		
		if (BmsEngine::Flag::fullchg_deboc == 1)
		{
			if (BmsEngine::Time::FullCHG_time_delay > 0)
			{
				BmsEngine::Time::FullCHG_time_delay -= 1; //Count down
			}
		}
		
		if (BmsEngine::Flag::fulldsg_deboc == 1)
		{
			if (BmsEngine::Time::FullDSG_time_delay > 0)
			{
				BmsEngine::Time::FullDSG_time_delay -= 1; //Count down
			}
		}
		
		//BMU ADC Warning
		if (BmsEngine::Flag::adcWarning_deboc == 1)
		{
			if (BmsEngine::Time::ADCWarning_time_delay > 0)
			{
				BmsEngine::Time::ADCWarning_time_delay -= 1; //Count down
			}
		}
		
		//BMU ADC Fault靽風
		if (BmsEngine::Flag::adcfault_deboc == 1)
		{
			if (BmsEngine::Time::ADCFault_time_delay > 0)
			{
				BmsEngine::Time::ADCFault_time_delay -= 1; //Count down
			}
		}
		
		if (BmsEngine::Flag::pcrs_deboc == 1)
		{
			if (BmsEngine::Time::pcrs_time_delay > 0)
			{
				BmsEngine::Time::pcrs_time_delay -= 1; //Count down
			}
		}
		
		if (BmsEngine::Flag::dci_deboc == 1)
		{
			if (BmsEngine::Time::DCI_time_delay > 0)
			{
				BmsEngine::Time::DCI_time_delay -= 1; //Count down
			}
		}
		
		if (BmsEngine::Flag::cci_deboc == 1)
		{
			if (BmsEngine::Time::CCI_time_delay > 0)
			{
				BmsEngine::Time::CCI_time_delay -= 1; //Count down
			}
		}
				
		//銝�OAT3_PackAmbient靽風
		if (BmsEngine::Flag::COAT3_PackAmbient_Debounce == 1)
		{
			if (BmsEngine::Time::COAT3_PackAmbient_Time_Delay > 0)
			{
				BmsEngine::Time::COAT3_PackAmbient_Time_Delay -= 1; //Count down
			}
		}
		
		//銝�UAT3_PackAmbient靽風
		if (BmsEngine::Flag::CUAT3_PackAmbient_Debounce == 1)
		{
			if (BmsEngine::Time::CUAT3_PackAmbient_Time_Delay > 0)
			{
				BmsEngine::Time::CUAT3_PackAmbient_Time_Delay -= 1; //Count down
			}
		}
		
		//銝�OAT3_PackAmbient靽風
		if (BmsEngine::Flag::DOAT3_PackAmbient_Debounce == 1)
		{
			if (BmsEngine::Time::DOAT3_PackAmbient_Time_Delay > 0)
			{
				BmsEngine::Time::DOAT3_PackAmbient_Time_Delay -= 1; //Count down
			}
		}
		
		//銝�UAT3_PackAmbient靽風
		if (BmsEngine::Flag::DUAT3_PackAmbient_Debounce == 1)
		{
			if (BmsEngine::Time::DUAT3_PackAmbient_Time_Delay > 0)
			{
				BmsEngine::Time::DUAT3_PackAmbient_Time_Delay -= 1; //Count down
			}
		}
		
		if (BmsEngine::Flag::SOC_CAL5_deboc == 1)
		{
			if (BmsEngine::Time::SOC_CAL5_time_delay > 0)
			{
				BmsEngine::Time::SOC_CAL5_time_delay -= 1; //Count down
			}
		}
		
		if (BmsEngine::Flag::SOC_CAL95_deboc == 1)
		{
			if (BmsEngine::Time::SOC_CAL95_time_delay > 0)
			{
				BmsEngine::Time::SOC_CAL95_time_delay -= 1; //Count down
			}
		}
	
	}
	
	//CheckHVILCountdown
	void BmsEngine::CheckHVILCountdown(void)
	{
		//HVIL1 Emergency
		if (BmsEngine::Flag::hvil1_deboc == 1)
		{
			if (BmsEngine::Time::hvil1_time_delay > 0)
			{
				BmsEngine::Time::hvil1_time_delay -= 1; //Count down
			}
		}
	
		//HVIL2 Insulation
		if (BmsEngine::Flag::hvil2_deboc == 1)
		{
			if (BmsEngine::Time::hvil2_time_delay > 0)
			{
				BmsEngine::Time::hvil2_time_delay -= 1; //Count down
			}
		}
	}
	
	//LED_Status
	void BmsEngine::LED_Status(void)
	{
		status_count++;
		if (status_count >= 22)
		{
			status_count = 0;
		}
		if ((BmsEngine::m_bcuStatus2 == 0x00) || (BmsEngine::m_bcuStatus3 == 0x00) || (BmsEngine::m_bcuStatus5 == 0x00) 
			|| (BmsEngine::m_bcuStatus6 == 0x00)  || (BmsEngine::Flag::HVIL1_Error == 0) || (BmsEngine::m_bcuStatus == 0x00) 
			|| (BmsEngine::Flag::ESS_BreakerFault == false) || (BmsEngine::Flag::ESS_HV_MainRelayFault == false) 
			|| (BmsEngine::Flag::ESS_HV_NegRelayFault == false) || (BmsEngine::Flag::ESS_PreChargeRelayFault == false) 
			|| (m_bcuState != BCUStateType_t::BPU_OTP) ||(m_bcuState != BCUStateType_t::PACK_OATP)
			|| (BmsEngine::Flag::HVIL2_Error == 0))
		{
			switch (status_count)
			{
				case 0:
				{
					LED_Status1 = 1;
				}
				break;	
				
				case 1:
				{
					LED_Status1 = 0;
				}
				break;	
				
				default:
				{
					LED_Status1 = 0;
				}
				break;
			}
		}
		if ((BmsEngine::m_bcuStatus2 != 0x00) || (BmsEngine::m_bcuStatus3 != 0x00) || (BmsEngine::m_bcuStatus5 != 0x00) 
			|| (BmsEngine::m_bcuStatus6 != 0x00) || (BmsEngine::Flag::HVIL1_Error == 1) || (BmsEngine::m_bcuStatus != 0x00) 
			|| (BmsEngine::Flag::ESS_BreakerFault == true) || (BmsEngine::Flag::ESS_HV_MainRelayFault == true) 
			|| (BmsEngine::Flag::ESS_HV_NegRelayFault == true) || (BmsEngine::Flag::ESS_PreChargeRelayFault == true)
			|| (m_bcuState == BCUStateType_t::BPU_OTP) ||(m_bcuState == BCUStateType_t::PACK_OATP)
			|| (BmsEngine::Flag::HVIL2_Error == 1))
		{
			switch (status_count)
			{
				case 0:
				{
					LED_Status1 = 1;
				}
				break;	
				
				case 1:
				{
					LED_Status1 = 0;
				}
				break;	
				
				case 2:
				{
					LED_Status1 = 1;
				}
				break;	
				
				case 3:
				{
					LED_Status1 = 0;
				}
				break;	
				
				default:
				{
					LED_Status1 = 0;
				}
				break;
			}
		}
	}
}
