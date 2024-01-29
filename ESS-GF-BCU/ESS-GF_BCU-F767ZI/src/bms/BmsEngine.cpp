//File
#include "BmsEngine.h"
#include "BatterySys.h"

#include "SOC.h"
#include "ADC_Temperature.h"
#include "Balance.h"
#include "LTC6811_daisy.h"


using namespace bmstech;

//extern "C" void mbed_reset();

DigitalOut RLY_Precharge(RLY_ON_A);		//RLY_Precharge
DigitalOut RLY_HV_Main(RLY_ON_B);		//RLY_HV_Main
DigitalOut RLY_HV_Neg(RLY_ON_C);		//RLY_HV_Neg
DigitalOut RLY_NFB(RLY_ON_D);			//RLY_NFB
DigitalOut RLY_K5_FAN(RLY_ON_E);		//RLY_K5_FAN

DigitalIn RLY_FB_Precharge(RLY_FB_A, PullUp); //RLY_Precharge FB 
DigitalIn RLY_FB_HV_Main(RLY_FB_B, PullUp); //RLY_HV_Main FB
DigitalIn RLY_FB_HV_Neg(RLY_FB_C, PullUp); //RLY_HV_Neg FB 
DigitalIn RLY_FB_NFB(RLY_FB_D, PullUp); //RLY_FB_NFB
DigitalIn RLY_FB_FAN(RLY_FB_E, PullUp); //RLY_FB_FAN

namespace bmstech
{
		
	SOC soc;	//SOC

#if 1
	// @param
	BmsEngine::BmsEngine()
	//	: pack_SOC(0)
	{
	
//		Initialize memory data here:
//		for(uint16_t i = 0 ; i < BMS_MEMORY_LENGTH ; i++)
//			bms_memory[i] = 0x00;
//
//		bms_memory[BMS_MODE_OFFSET] = static_cast<uint16_t>(BCUStateType_t::BCU_INIT);  // 0x00;
//		bms_memory[BMS_FAULT_CLEAR_OFFSET] = 0;  // 0x00;
//		bms_memory[BMS_CTRL_MODE_OFFSET] = 0;  // 0x00;
//
//		Pack Status Area: 0x21XX
//		PackConst::MAX_PACK_NUM
//		for(uint16_t i = 0 ; i < PackConst::MAX_PACK_NUM ; i++)
//		{
//			bms_memory[PACK_STATUS_OFFSET | i] = DEFAULT_PACK_STATUS;
//		}
//
//		bms_memory[BALANCE_VOLTAGE_OFFSET] = DEFAULT_BALANCE_VOLTAGE;
//		bms_memory[BALANCE_OVERTEMP_START_OFFSET] = DEFAULT_BALANCE_OT_START;
//		bms_memory[BALANCE_OVERTEMP_RELEASE_OFFSET] = DEFAULT_BALANCE_OT_RELEASE;
//
//		//for debug only:
//		for(uint16_t i = 0x6000 ; i < BMS_MEMORY_LENGTH ; i++)
//			bms_memory[i] = i;
	}
#endif
	//
	BmsEngine::~BmsEngine()
	{
	}

	//=========================================================
	//BCU Finite State Machine:
	//=========================================================

	uint16_t waitCount = 0U;	//init
	
	void BmsEngine::CheckEvent(const BCUStateType_t state)
	{
		uint16_t bcuStatus = p_bmspara->m_bcuStatus;//
		uint16_t bcuStatus2 = p_bmspara->m_bcuStatus2;//
		uint16_t bcuStatus3 = p_bmspara->m_bcuStatus3;//
		uint16_t bcuStatus5 = p_bmspara->m_bcuStatus5;//
		uint16_t bcuStatus6 = p_bmspara->m_bcuStatus6;//
	
		//Checking p_bmspara->m_bcuStatus & set m_relay
		//for state transition & protection:
		//====================================================================

		//Check: CAN FAULT
		if ((bcuStatus & BCU_STATUS_CMCAN_FAULT)
				|| (bcuStatus & CURRENT_STATUS_CAN_FAULT)
				|| (bcuStatus & EMS_STATUS_MBUCAN_FAULT))//
		{
			m_preState = state;    //save state
			m_bcuState = BCUStateType_t::BCU_FAULT;
			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
//					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			BmsEngine::FaultCount++;	//NFB Close Time counter
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2U*SECOND_1)
			{			
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}		
			return;		
		}
		else if ((bcuStatus2 & BCU_STATUS_OCC2) || (bcuStatus3 & BCU_STATUS_OCC3))
		{
			if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT) 
			{
				return;		
			}
			m_preState = state;  //save state
			m_bcuState = BCUStateType_t::BCU_COCP;  //jump to BCU COCP state
			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}
			return;
		}
		else if ((bcuStatus2 & BCU_STATUS_OCD2) || (bcuStatus3 & BCU_STATUS_OCD3))
		{
			if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT) 
			{
				return;		
			}
			m_preState = state;  //save state
			m_bcuState = BCUStateType_t::BCU_DOCP;  //jump to BCU DOCP state
			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}
			return;
		}
		else if ((bcuStatus2 & BCU_STATUS_OV2) || (bcuStatus3 & BCU_STATUS_OV3))
		{
			if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT) 
			{
				return;		
			}
			m_preState = state;  //save state
			m_bcuState = BCUStateType_t::BCU_OVP;  //jump to BCU OVP state
			
			Balance::PassiveBalanceCommand_ON_flag = 0;	//close Passive Balance
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}			
			return;
		}
		//eferry for Bitrode charging: battery voltage needs to be detected for charging action
		//check if UVP:
		else if ((bcuStatus2 & BCU_STATUS_UV2) || (bcuStatus3 & BCU_STATUS_UV3))
		{
			if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT) 
			{
				return;		
			}			
			m_preState = state;  //save state
			m_bcuState = BCUStateType_t::BCU_UVP;  //jump to BCU UVP state 
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}		
			return;
		}
		else if ((bcuStatus2 & BCU_STATUS_OTC2) || (bcuStatus3 & BCU_STATUS_OTC3))
		{
			if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT) 
			{
				return;		
			}
			m_preState = state;   //save state
			m_bcuState = BCUStateType_t::BCU_OTCP;   //jump to BCU OTCP state
		
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			if ((BmsEngine::Flag::OTC1 == 0) && (BmsEngine::Flag::OTD1 == 0) && (BatterySys::BMU_FAN_ON == 0))
			{
				RLY_K5_FAN = RLY_OFF; //K5 OFF
			}
			else if ((BmsEngine::Flag::OTC1 == 1) || (BmsEngine::Flag::OTD1 == 1) || (BatterySys::BMU_FAN_ON == 1))
			{
				RLY_K5_FAN = RLY_ON; //K5 ON
			}
			
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}			
			return;
		}
		else if ((bcuStatus2 & BCU_STATUS_OTD2) || (bcuStatus3 & BCU_STATUS_OTD3))
		{
			if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT) 
			{
				return;		
			}
			m_preState = state;    //save state
			m_bcuState = BCUStateType_t::BCU_OTDP;    //jump to BCU OTDP state
		
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			if ((BmsEngine::Flag::OTC1 == 0) && (BmsEngine::Flag::OTD1 == 0) && (BatterySys::BMU_FAN_ON == 0))
			{
				RLY_K5_FAN = RLY_OFF; //K5 OFF
			}
			else if ((BmsEngine::Flag::OTC1 == 1) || (BmsEngine::Flag::OTD1 == 1) || (BatterySys::BMU_FAN_ON == 1))
			{
				RLY_K5_FAN = RLY_ON; //K5 ON
			}
					
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}			
			return;
		}
		
		else if ((bcuStatus2 & BCU_STATUS_UTC2) || (bcuStatus3 & BCU_STATUS_UTC3))
		{
			if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT) 
			{
				return;		
			}
			m_preState = state;    //save state
			m_bcuState = BCUStateType_t::BCU_UTCP;    //jump to BCU UTCP state
			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}			
			return;
		}
		else if ((bcuStatus2 & BCU_STATUS_UTD2) || (bcuStatus3 & BCU_STATUS_UTD3))
		{
			if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT)
			{
				return;		
			}
			m_preState = state;     //save state
			m_bcuState = BCUStateType_t::BCU_UTDP;     //jump to BCU UTDP state			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}			

			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				NFBCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}			
			return;
		}
	
		//Check: AFE BMU ADC Fault Check Function
		else if (bcuStatus5 & BMU_STATUS_ADCFAULT)
		{
			if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT) 
			{
				return;		
			}
			m_preState = state;       //save state 
			m_bcuState = BCUStateType_t::BMU_ADC_FAULT;  			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}		

			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}			
			return;
		}

		else if (bcuStatus6 & BCU_STATUS_SELFTESTFAULT) 
		{
			m_preState = state;     //save state
			m_bcuState = BCUStateType_t::BCU_SELFTEST_FAULT;
			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2U*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}			
			return;		
		}
		
		else if (BmsEngine::Flag::HVIL1_Error == 1) 
		{
			if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT) 
			{
				return;		
			}			
			m_preState = state;    //save state
			m_bcuState = BCUStateType_t::BCU_HVIL_FAULT;
			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			RLY_NFB = NFB_OFF;
			RLY_Precharge = RLY_OFF;
			RLY_HV_Main = RLY_OFF;
			RLY_HV_Neg = RLY_OFF;
			BmsEngine::FaultCount = 0;
			waitCount = 0;
			count = 0;
			NFBCount = 0;
			return;		
		}
		
		else if(BmsEngine::Flag::HVIL2_Error == 1) 
		{
//			if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT) 
//			{
//				return;		
//			}			
			m_preState = state; //save state
			m_bcuState = BCUStateType_t::Insulation_FAULT;
			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			RLY_NFB = NFB_OFF;
			RLY_Precharge = RLY_OFF;
			RLY_HV_Main = RLY_OFF;
			RLY_HV_Neg = RLY_OFF;
			BmsEngine::FaultCount = 0;
			waitCount = 0;
			count = 0;
			NFBCount = 0;
			return;		
		}

		
		else if (BmsEngine::Flag::ESS_PreChargeRelayFault == true) 
		{
			m_preState = state;      //save state
			m_bcuState = BCUStateType_t::ESS_PRECHGRELAY_FAULT;
			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}			
			return;		
		}
		else if (BmsEngine::Flag::ESS_BreakerFault == true)
		{
			m_preState = state; //save state
			m_bcuState = BCUStateType_t::ESS_NFB_FAULT;
			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}			
			return;		
		}
		else if (BmsEngine::Flag::ESS_HV_MainRelayFault == true)
		{
			m_preState = state; //save state
			m_bcuState = BCUStateType_t::ESS_HV_MAINRELAY_FAULT;
			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}			
			return;		
		}
		else if (BmsEngine::Flag::ESS_HV_NegRelayFault == true)
		{
			m_preState = state; //save state
			m_bcuState = BCUStateType_t::ESS_HV_NEGRELAY_FAULT;
			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}		
			return;		
		}
		else if (BmsEngine::Flag::OT2_PCS_Positive == true)
		{
			if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT) 
			{
				return;		
			}
			m_preState = state; //save state
			m_bcuState = BCUStateType_t::BPU_OTP;
			
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
			
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}			
			return;		
		}
		else if ((BmsEngine::Flag::COAT3_PackAmbient == 1) || (BmsEngine::Flag::CUAT3_PackAmbient == 1) 
			 || (BmsEngine::Flag::DOAT3_PackAmbient == 1) || (BmsEngine::Flag::DUAT3_PackAmbient == 1))
		{
			if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT) 
			{
				return;		
			}
			m_preState = state; //save state
			m_bcuState = BCUStateType_t::PACK_OATP;
						
			Balance::PassiveBalanceCommand_ON_flag = 0;
			Balance::PassiveBalancejudge_flag = 0;
			for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
			{
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
				{
					BatterySys::pack[x].cellBalance[y] = 0;				
				}
			}
						
			BmsEngine::FaultCount++;
			RLY_NFB = NFB_OFF;
			if (BmsEngine::FaultCount == 2*SECOND_1)
			{
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				waitCount = 0;
				count = 0;
				BmsEngine::FaultCount = 0;
				NFBCount = 0;
			}			
			return;		
		}	
		else if ((BmsEngine::Flag::OTC1 == 1) || (BmsEngine::Flag::OTD1 == 1) || (BatterySys::BMU_FAN_ON == 1))
		{
			RLY_K5_FAN = RLY_ON; //K5 ON
				
			return;		
		}
		else if ((BmsEngine::Flag::OTC1 == 0) && (BmsEngine::Flag::OTD1 == 0) && (BatterySys::BMU_FAN_ON == 0))
		{
			RLY_K5_FAN = RLY_OFF; //K5 OFF
				
			return;		
		}
		else
		{		
			BmsEngine::FaultCount = 0;
			return;		
		}
	}


	//goto Standdby mode when recovery
	void BmsEngine::EFerryStateRecovery(const BCUStateType_t state)
	{
		//Auto Recovery from Error event
		if ((p_bmspara->m_bcuStatus2 == BCU_STATUS_OK2) && (p_bmspara->m_bcuStatus3 == BCU_STATUS_OK3) 
			&& (p_bmspara->m_bcuStatus == BCU_STATUS_OK) && (p_bmspara->m_bcuStatus5 == BCU_STATUS_OK5)
			&& (p_bmspara->m_bcuStatus6 == BCU_STATUS_OK6) && (BmsEngine::Flag::HVIL1_Error == 0) 
			&& (BmsEngine::Flag::HVIL2_Error == 0) 
			&& (BmsEngine::Flag::ESS_BreakerFault == false) && (BmsEngine::Flag::ESS_PreChargeRelayFault == false) 
			&& (BmsEngine::Flag::ESS_HV_MainRelayFault == false) && (BmsEngine::Flag::ESS_HV_NegRelayFault == false))//no event

		{
			waitCount++;

			if (waitCount > RECOVERY_DELAY)
			{
				waitCount = 0U;
				if (BmsEngine::BMUCANFAULTFlag == 1)
				{
					BmsEngine::BMUCANFAULTFlag = 0;
				
					BmsEngine::SortCellVoltage();
					BmsEngine::MaxSort_Volt = BmsEngine::MaxCellVoltage();
					BmsEngine::MinSort_Volt = BmsEngine::MinCellVoltage();
					SOC::OCV_SOC = SOC::SOC_OCV(BatterySys::MinVolt);                        		//MinSort_Volt  
					m_soc = SOC::OCV_SOC * 10;
					BmsEngine::coulombCount = (float)((float)SOC::OCV_SOC / 100 *(float)(m_fullChargeCapacity*COULOMB_CONVERSION_MA));
			    }
				m_bcuState = BCUStateType_t::BCU_STANDBY;  //jump back to standby state;
											
				m_preState = state;  //save present state
				
//				RLY_NFB = NFB_OFF;
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
				
				m_bcuModeChange = BCUModeChangeType_t::NONE;
			}
		}
	}

	//
	BCUStateType_t BmsEngine::BcuFSM(void)
	{
		fsmCount++;
		switch (m_bcuState)
		{
			case BCUStateType_t::BCU_INIT:	 //start up process	0x01
			{		
				count++;
	
				if (count == SECOND_1) // 1 sec
				{
					RLY_NFB = NFB_OFF;				//K4
					RLY_Precharge = RLY_OFF;		//K1
					RLY_HV_Main = RLY_OFF;			//K2
					RLY_HV_Neg = RLY_OFF;			//K3
//					RLY_K5_FAN = RLY_OFF;			//K5
				}

				if (count == 3U*SECOND_1)	// 3 sec
				{
					BatterySys::BCU_canVersion_Information(); //setBCU Version Information
				}
				
				if (count == 5U*SECOND_1)	// 5 sec
				{
					BmsEngine::SortCellVoltage();
					BmsEngine::MaxSort_Volt = BmsEngine::MaxCellVoltage();
					BmsEngine::MinSort_Volt = BmsEngine::MinCellVoltage();
					SOC::OCV_SOC = SOC::SOC_OCV(BatterySys::MinVolt);           	//MinSort_Volt  
					m_soc = SOC::OCV_SOC * 10;
					BmsEngine::coulombCount = (float)((float)SOC::OCV_SOC / 100 *(float)(m_fullChargeCapacity*COULOMB_CONVERSION_MA));
					BmsEngine::SortCellTemperature();
					BmsEngine::MaxSort_Temp = BmsEngine::MaxCellTemperature();
					BmsEngine::MinSort_Temp = BmsEngine::MinCellTemperature();

		
					if (RLY_FB_NFB == NFB_FB_ON)
					{
						RLY_NFB = NFB_OFF;
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
				
						m_preState = BCUStateType_t::BCU_INIT;                              //save present state
						m_bcuState = BCUStateType_t::ESS_NFB_FAULT;                              //set next state
						BmsEngine::Flag::ESS_BreakerFault = true;
						count = 0U;
					}
		
					if (RLY_FB_HV_Main == RLY_FB_ON)
					{
						RLY_NFB = NFB_OFF;
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
				
						m_preState = BCUStateType_t::BCU_INIT; //save present state
						m_bcuState = BCUStateType_t::ESS_HV_MAINRELAY_FAULT; //set next state
						BmsEngine::Flag::ESS_HV_MainRelayFault = true;
						count = 0U;
					}
					if (RLY_FB_HV_Neg == RLY_FB_ON)
					{
						RLY_NFB = NFB_OFF;
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
				
						m_preState = BCUStateType_t::BCU_INIT; //save present state
						m_bcuState = BCUStateType_t::ESS_HV_NEGRELAY_FAULT; //set next state
						BmsEngine::Flag::ESS_HV_NegRelayFault = true;
						count = 0U;
					}
					if (RLY_FB_Precharge == RLY_FB_ON)
					{
						RLY_NFB = NFB_OFF;
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
				
						m_preState = BCUStateType_t::BCU_INIT; //save present state
						m_bcuState = BCUStateType_t::ESS_PRECHGRELAY_FAULT; //set next state
						BmsEngine::Flag::ESS_PreChargeRelayFault = true;
						count = 0U;
					}
					
//					if (RLY_FB_FAN == RLY_FB_ON)
//					{				
//						BmsEngine::flag::ESS_FAN_Warning = true;
//	
//					}
				}

				//auto checking if no error goto BCU_STANDBY:
				//NOTE: Wait CM App to run
				if (count > INIT_WAIT_TIME) //10 s tick  (10sec)  JY
				{
					BmsEngine::SortCellVoltage();
					BmsEngine::MaxSort_Volt = BmsEngine::MaxCellVoltage();
					BmsEngine::MinSort_Volt = BmsEngine::MinCellVoltage();
					SOC::OCV_SOC = SOC::SOC_OCV(BatterySys::MinVolt);                         		//MinSort_Volt  
					m_soc = SOC::OCV_SOC * 10;
					BmsEngine::coulombCount = (float)((float)SOC::OCV_SOC / 100 *(float)(m_fullChargeCapacity*COULOMB_CONVERSION_MA));
			
					count = 0U;
					
					BmsEngine::SBCUCANMessageCapacity::Initialed = 1; 		
					
					if (RLY_FB_NFB == NFB_FB_ON)
					{
						RLY_NFB = NFB_OFF;
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						
						m_preState = BCUStateType_t::BCU_INIT;                               //save present state
						m_bcuState = BCUStateType_t::ESS_NFB_FAULT;                               //set next state
						BmsEngine::Flag::ESS_BreakerFault = true;
					}
					else if (RLY_FB_HV_Main == RLY_FB_ON)
					{
						RLY_NFB = NFB_OFF;
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						
						m_preState = BCUStateType_t::BCU_INIT; //save present state
						m_bcuState = BCUStateType_t::ESS_HV_MAINRELAY_FAULT; //set next state
						BmsEngine::Flag::ESS_HV_MainRelayFault = true;
					}
					else if (RLY_FB_HV_Neg == RLY_FB_ON)
					{
						RLY_NFB = NFB_OFF;
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						
						m_preState = BCUStateType_t::BCU_INIT; //save present state
						m_bcuState = BCUStateType_t::ESS_HV_NEGRELAY_FAULT; //set next state
						BmsEngine::Flag::ESS_HV_NegRelayFault = true;
					}
					else if (RLY_FB_Precharge == RLY_FB_ON)
					{
						RLY_NFB = NFB_OFF;
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						
						m_preState = BCUStateType_t::BCU_INIT; //save present state
						m_bcuState = BCUStateType_t::ESS_PRECHGRELAY_FAULT; //set next state
						BmsEngine::Flag::ESS_PreChargeRelayFault = true;
					}
					else
					{
						m_preState = BCUStateType_t::BCU_INIT;                             //save present state
						m_bcuState = BCUStateType_t::BCU_STANDBY;                            //set next state				
				
					}
															
	
					if (BatterySys::BMU_FAN_ON == 1)
					{
						if (RLY_FB_FAN == RLY_FB_OFF)
						{	
							BmsEngine::FAN_Warning_Count++;
							if (BmsEngine::FAN_Warning_Count == RELAY_CHECK_TIME)
							{
								BmsEngine::Flag::ESS_FAN_Warning = true;	
							}
						}
						else
						{
							BmsEngine::FAN_Warning_Count = 0U;
							BmsEngine::Flag::ESS_FAN_Warning = false;
						}
					}
					else
					{
						if (RLY_FB_FAN == RLY_FB_ON)
						{	
							BmsEngine::FAN_Warning_Count++;
							if (BmsEngine::FAN_Warning_Count == RELAY_CHECK_TIME)
							{
								BmsEngine::Flag::ESS_FAN_Warning = true;	
							}
						}
						else
						{
							BmsEngine::FAN_Warning_Count = 0U;
							BmsEngine::Flag::ESS_FAN_Warning = false;
						}
					}
				}
			}
			break;
			
			
			case BCUStateType_t::BCU_STANDBY:		//0x10
			{
				RLY_NFB = NFB_ON;	//20230320 BCU STANDBY State MCCB:ON CONTACTOR : OFF

				m_bcuDrive = BCUDriveType_t::STANDBY;
							
#if defined (Auto_MODE)
				m_bcuModeChange = BCUModeChangeType_t::DRIVE_PRECHARGE;		  
#endif //(Auto_MODE)
				
				//Mode (0x02) Discharge Mode
				if (m_bcuModeChange == BCUModeChangeType_t::DRIVE_PRECHARGE) //mode change happened	(0x02)
				{
					NFBCount++;	
					RLY_NFB = NFB_ON;
					if (NFBCount >= 2U*SECOND_1)  //20220917 JY  (==) --> (>=)
					{
						if (RLY_FB_NFB == NFB_FB_ON)
						{
							m_preState = BCUStateType_t::BCU_STANDBY;						
							m_bcuState = BCUStateType_t::BCU_DRIVE_THRU_PRECHARGE;
							m_status = RELAYStatusType_t::PRECHARGE;
				
							RLY_Precharge = RLY_OFF;
							RLY_HV_Main = RLY_OFF;
							RLY_HV_Neg = RLY_ON;
							count = 0U;
							NFBCount = 0U;
							m_bcuModeChange = BCUModeChangeType_t::NONE;
						}
						else
						{
							RLY_NFB = NFB_OFF;
							RLY_Precharge = RLY_OFF;
							RLY_HV_Main = RLY_OFF;
							RLY_HV_Neg = RLY_OFF;
						
							m_preState = BCUStateType_t::BCU_STANDBY; //save present state
							m_bcuState = BCUStateType_t::ESS_NFB_FAULT; //set next state
							BmsEngine::Flag::ESS_BreakerFault = true;
							NFBCount = 0U;
							m_bcuModeChange = BCUModeChangeType_t::NONE;
						}
					}
					
				}
				//Mode (0x01) Charge Mode
				else if (m_bcuModeChange == BCUModeChangeType_t::CHARGE_PRECHARGE) //mode change happened(0x01)
				{

					NFBCount++;	
					RLY_NFB = NFB_ON;
					if (NFBCount >= 2U*SECOND_1)  //20220917 JY  (==) --> (>=)
					{
						if (RLY_FB_NFB == NFB_FB_ON)
						{
							m_preState = BCUStateType_t::BCU_STANDBY;						
							m_bcuState = BCUStateType_t::BCU_CHARGE_THRU_PRECHARGE;
							m_status = RELAYStatusType_t::PRECHARGE;
				
							RLY_Precharge = RLY_OFF;
							RLY_HV_Main = RLY_OFF;
							RLY_HV_Neg = RLY_ON;
							count = 0U;
							NFBCount = 0U;
							m_bcuModeChange = BCUModeChangeType_t::NONE;
						}
						else
						{
							RLY_NFB = NFB_OFF;
							RLY_Precharge = RLY_OFF;
							RLY_HV_Main = RLY_OFF;
							RLY_HV_Neg = RLY_OFF;
						
							m_preState = BCUStateType_t::BCU_STANDBY; //save present state
							m_bcuState = BCUStateType_t::ESS_NFB_FAULT; //set next state
							BmsEngine::Flag::ESS_BreakerFault = true;
							NFBCount = 0U;
							m_bcuModeChange = BCUModeChangeType_t::NONE;
						}
					}										
				}

				else if (m_bcuModeChange == BCUModeChangeType_t::STANDBY)	//mode change happened	(0x00)
				{
					m_preState = BCUStateType_t::BCU_STANDBY;
					m_bcuState = BCUStateType_t::BCU_STANDBY;
					m_status = RELAYStatusType_t::STANDBY;
	
					RLY_NFB = NFB_ON; //20230320 BCU STANDBY State MCCB:ON CONTACTOR : OFF
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_OFF;
					RLY_Precharge = RLY_OFF;
					NFBCount = 0U;
					count = 0U;
					m_bcuModeChange = BCUModeChangeType_t::NONE;
			
				}
				
				else if(m_bcuModeChange == BCUModeChangeType_t::BCU_Emergency_Stop)	//mode change happened (0x07)
				{
					m_preState = BCUStateType_t::BCU_STANDBY;
					m_bcuState = BCUStateType_t::BCU_Emergency_Stop;
					m_status = RELAYStatusType_t::STANDBY;
	
					RLY_NFB = NFB_OFF; //20230320 BCU_Emergency_Stop State MCCB:OFF CONTACTOR : OFF
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_OFF;
					RLY_Precharge = RLY_OFF;
					NFBCount = 0U;
					count = 0U;
					m_bcuModeChange = BCUModeChangeType_t::NONE;
				}

			}
			break;
			
			case BCUStateType_t::BCU_Emergency_Stop:		//0xE7
			{
				RLY_NFB = NFB_OFF; //20230320 BCU_Emergency_Stop State MCCB:OFF CONTACTOR : OFF
				RLY_K5_FAN = RLY_OFF; //K5 OFF
				m_bcuDrive = BCUDriveType_t::Emergency_Stop;
								

				if (m_bcuModeChange == BCUModeChangeType_t::STANDBY)	//mode change happened	(0x00)
				{
					m_preState = BCUStateType_t::BCU_Emergency_Stop;
					m_bcuState = BCUStateType_t::BCU_STANDBY;
					m_status = RELAYStatusType_t::STANDBY;
	
					RLY_NFB = NFB_ON; //20230320 BCU STANDBY State MCCB:ON CONTACTOR : OFF
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_OFF;
					RLY_Precharge = RLY_OFF;
					NFBCount = 0U;
					count = 0U;
					m_bcuModeChange = BCUModeChangeType_t::NONE;			
				}	
				else if(m_bcuModeChange == BCUModeChangeType_t::BCU_Emergency_Stop)	//mode change happened (0x07)
				{
					m_preState = BCUStateType_t::BCU_STANDBY;
					m_bcuState = BCUStateType_t::BCU_Emergency_Stop;
					m_status = RELAYStatusType_t::STANDBY;
	
					RLY_NFB = NFB_OFF; //20230320 BCU_Emergency_Stop State MCCB:OFF CONTACTOR : OFF
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_OFF;
					RLY_Precharge = RLY_OFF;
					NFBCount = 0U;
					count = 0U;
					m_bcuModeChange = BCUModeChangeType_t::NONE;
				}

			}
			break;

			//defined for eferry
			//=============================================
			case BCUStateType_t::BCU_DRIVE_THRU_PRECHARGE:		//0x21		Discharge Precharge Mode
			{
				count++;
	
				if (m_bcuModeChange == BCUModeChangeType_t::STANDBY) 
				{
					m_preState = BCUStateType_t::BCU_DRIVE_THRU_PRECHARGE;
					m_bcuState = BCUStateType_t::BCU_STANDBY;
					m_status = RELAYStatusType_t::STANDBY;
	
//					RLY_NFB = NFB_OFF;
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_OFF;
					RLY_Precharge = RLY_OFF;
					count = 0U;
					m_bcuModeChange = BCUModeChangeType_t::NONE;
			
				}
				else if (m_bcuModeChange == BCUModeChangeType_t::BCU_Emergency_Stop) 
				{
					m_preState = BCUStateType_t::BCU_DRIVE_THRU_PRECHARGE;
					m_bcuState = BCUStateType_t::BCU_Emergency_Stop;
					m_status = RELAYStatusType_t::Emergency_Stop;
	
					RLY_NFB = NFB_OFF;
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_OFF;
					RLY_Precharge = RLY_OFF;
					count = 0U;
					m_bcuModeChange = BCUModeChangeType_t::NONE;
			
				}
				else //illegal command
				{
					m_bcuModeChange = BCUModeChangeType_t::NOT_VALID;                    //0x80
				}
				
				if (count == PRECHARGE_DELAY) //0.5 sec tick (2: 1 sec)
				{						
					RLY_Precharge = RLY_ON;
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_ON;
				}
				else if (count == (PRECHARGE_DELAY + PRECHG_TIME)) //0.5 sec tick (3 sec)
				{		
					if (RLY_FB_Precharge == RLY_FB_ON)
					{
						RLY_HV_Main = RLY_ON;
					}
					else
					{
						BmsEngine::Flag::ESS_PreChargeRelayFault = true;
						RLY_NFB = RLY_OFF;
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						m_preState = BCUStateType_t::BCU_DRIVE_THRU_PRECHARGE;                                 //save present state
						m_bcuState = BCUStateType_t::ESS_PRECHGRELAY_FAULT;                                 //set next state
						count = 0U;
						//m_bcuModeChange = BCUModeChangeType_t::NONE;
					}
				}
				else if (count >= (PRECHARGE_DELAY + PRECHG_TIME + SECOND_1))
				{
					if (RLY_FB_HV_Main == RLY_FB_ON)
					{
						m_preState = BCUStateType_t::BCU_DRIVE_THRU_PRECHARGE;
						m_bcuState = BCUStateType_t::BCU_DRIVE;
						m_status = RELAYStatusType_t::DISCHARGE;				                
						RLY_Precharge = RLY_OFF;				//turn off precharge relay
						count = 0U;
					}
					else
					{
						BmsEngine::Flag::ESS_HV_MainRelayFault = true;
						RLY_NFB = RLY_OFF;
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						m_preState = BCUStateType_t::BCU_DRIVE_THRU_PRECHARGE; //save present state
						m_bcuState = BCUStateType_t::ESS_HV_MAINRELAY_FAULT; //set next state
						count = 0U;
					}
	
				}
			}
			break;
			
			case BCUStateType_t::BCU_CHARGE_THRU_PRECHARGE :	//0x22		Charge Precharge Mode
			{							
				count++;
				if (m_bcuModeChange == BCUModeChangeType_t::STANDBY)
				{
					m_preState = BCUStateType_t::BCU_CHARGE_THRU_PRECHARGE;
					m_bcuState = BCUStateType_t::BCU_STANDBY;
					m_status = RELAYStatusType_t::STANDBY;
					
//					RLY_NFB = NFB_OFF;
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_OFF;
					RLY_Precharge = RLY_OFF;
					count = 0U;
					m_bcuModeChange = BCUModeChangeType_t::NONE;
				}
				else if (m_bcuModeChange == BCUModeChangeType_t::BCU_Emergency_Stop)
				{
					m_preState = BCUStateType_t::BCU_CHARGE_THRU_PRECHARGE;
					m_bcuState = BCUStateType_t::BCU_Emergency_Stop;
					m_status = RELAYStatusType_t::Emergency_Stop;
					
					RLY_NFB = NFB_OFF;
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_OFF;
					RLY_Precharge = RLY_OFF;
					count = 0U;
					m_bcuModeChange = BCUModeChangeType_t::NONE;
				}
				else //illegal command
				{
					m_bcuModeChange = BCUModeChangeType_t::NOT_VALID;                     //0x80
				}
				
				if (count == PRECHARGE_DELAY) //0.5 sec tick (2: 1 sec)
				{						
					RLY_Precharge = RLY_ON;
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_ON;
				}
				else if (count == (PRECHARGE_DELAY + PRECHG_TIME)) //0.5 sec tick (3 sec)
				{		
					if (RLY_FB_Precharge == RLY_FB_ON)
					{
						RLY_HV_Main = RLY_ON;
					}
					else
					{
						BmsEngine::Flag::ESS_PreChargeRelayFault = true;
						RLY_NFB = RLY_OFF;
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;    
						m_preState = BCUStateType_t::BCU_CHARGE_THRU_PRECHARGE;      //save present state
						m_bcuState = BCUStateType_t::ESS_PRECHGRELAY_FAULT;          //set next state
						count = 0U;
						//m_bcuModeChange = BCUModeChangeType_t::NONE;
					}
				}
				else if (count >= (PRECHARGE_DELAY + PRECHG_TIME + SECOND_1))
				{
					if (RLY_FB_HV_Main == RLY_FB_ON)
					{
						m_preState = BCUStateType_t::BCU_CHARGE_THRU_PRECHARGE;
						m_bcuState = BCUStateType_t::BCU_CHARGE;
						m_status = RELAYStatusType_t::CHARGE;				                
						RLY_Precharge = RLY_OFF;				//turn off precharge relay
						count = 0U;
					}
					else
					{
						BmsEngine::Flag::ESS_HV_MainRelayFault = true;
						RLY_NFB = RLY_OFF;
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						m_preState = BCUStateType_t::BCU_CHARGE_THRU_PRECHARGE; //save present state
						m_bcuState = BCUStateType_t::ESS_HV_MAINRELAY_FAULT; //set next state
						count = 0U;
					}
	
				}
			}
			break;
													
			case BCUStateType_t::BCU_DRIVE:		//0x30		Discharge Mode
			{
				m_bcuDrive = BCUDriveType_t::DRIVE;

				//Checking p_bmspara->m_bcuStatus & set m_relay
				//for state transition & protection:
				//====================================================================
				CheckEvent(BCUStateType_t::BCU_DRIVE);
								
				if (RLY_FB_NFB == NFB_FB_OFF)
				{
					NFB_FAULT_Count++;
					if (NFB_FAULT_Count == RELAY_CHECK_TIME)
					{
						RLY_NFB = NFB_OFF;				
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						m_preState = BCUStateType_t::BCU_DRIVE;           //save present state
						m_bcuState = BCUStateType_t::ESS_NFB_FAULT;       //set next state
						BmsEngine::Flag::ESS_BreakerFault = true;
						NFB_FAULT_Count = 0U;
						NFBCount = 0U;
						m_bcuModeChange = BCUModeChangeType_t::NONE;
					}
				}
				else if (RLY_FB_HV_Main == RLY_FB_OFF)
				{
					BmsEngine::Main_FAULT_Count++;
					if (BmsEngine::Main_FAULT_Count == RELAY_CHECK_TIME)
					{
						RLY_NFB = NFB_OFF;				
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						m_preState = BCUStateType_t::BCU_DRIVE; //save present state
						m_bcuState = BCUStateType_t::ESS_HV_MAINRELAY_FAULT; //set next state
						BmsEngine::Flag::ESS_HV_MainRelayFault = true;
						BmsEngine::Main_FAULT_Count = 0U;
						NFBCount = 0U;
						m_bcuModeChange = BCUModeChangeType_t::NONE;
					}
				}
				else if (RLY_FB_HV_Neg == RLY_FB_OFF)
				{
					BmsEngine::Neg_FAULT_Count++;
					if (BmsEngine::Neg_FAULT_Count == RELAY_CHECK_TIME)
					{
						RLY_NFB = NFB_OFF;				
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						m_preState = BCUStateType_t::BCU_DRIVE; //save present state
						m_bcuState = BCUStateType_t::ESS_HV_NEGRELAY_FAULT; //set next state
						BmsEngine::Flag::ESS_HV_NegRelayFault = true;
						BmsEngine::Neg_FAULT_Count = 0U;
						NFBCount = 0U;
						m_bcuModeChange = BCUModeChangeType_t::NONE;
					}
				}
				else if (RLY_FB_Precharge == RLY_FB_ON)
				{
					BmsEngine::Precharge_FAULT_Count++;
					if (BmsEngine::Precharge_FAULT_Count == RELAY_CHECK_TIME)
					{
						RLY_NFB = NFB_OFF;				
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						m_preState = BCUStateType_t::BCU_DRIVE; //save present state
						m_bcuState = BCUStateType_t::ESS_PRECHGRELAY_FAULT; //set next state
						BmsEngine::Flag::ESS_PreChargeRelayFault = true;
						BmsEngine::Precharge_FAULT_Count = 0U;
						NFBCount = 0U;
						m_bcuModeChange = BCUModeChangeType_t::NONE;
					}
				}
				else
				{
					BmsEngine::NFB_FAULT_Count = 0U;
					BmsEngine::Main_FAULT_Count = 0U;
					BmsEngine::Neg_FAULT_Count = 0U;
					BmsEngine::Precharge_FAULT_Count = 0U;
				}
				
				
				if (BatterySys::BMU_FAN_ON == 1)
				{
					if (RLY_FB_FAN == RLY_FB_OFF)
					{	
						BmsEngine::FAN_Warning_Count++;
						if (BmsEngine::FAN_Warning_Count == RELAY_CHECK_TIME)
						{
							BmsEngine::Flag::ESS_FAN_Warning = true;	
						}
					}
					else
					{
						BmsEngine::FAN_Warning_Count = 0U;
						BmsEngine::Flag::ESS_FAN_Warning = false;
					}
				}
				else
				{
					if (RLY_FB_FAN == RLY_FB_ON)
					{	
						BmsEngine::FAN_Warning_Count++;
						if (BmsEngine::FAN_Warning_Count == RELAY_CHECK_TIME)
						{
							BmsEngine::Flag::ESS_FAN_Warning = true;	
						}
					}
					else
					{
						BmsEngine::FAN_Warning_Count = 0U;
						BmsEngine::Flag::ESS_FAN_Warning = false;
					}
				}
				
				if (m_bcuModeChange == BCUModeChangeType_t::STANDBY)
				{								
//					NFBCount++;			
//					RLY_NFB = NFB_OFF;

//					if (NFBCount == 2U*SECOND_1)
//					{
//						if (RLY_FB_NFB == NFB_FB_OFF)
//						{
							m_preState = BCUStateType_t::BCU_DRIVE;
							m_bcuState = BCUStateType_t::BCU_STANDBY;
							m_status = RELAYStatusType_t::STANDBY;
	
							RLY_HV_Main = RLY_OFF;
							RLY_HV_Neg = RLY_OFF;
							RLY_Precharge = RLY_OFF;
							count = 0U;
							NFBCount = 0U;
							NFB_FAULT_Count = 0U;
							m_bcuModeChange = BCUModeChangeType_t::NONE;
//						}
//					}			
				}
				else if (m_bcuModeChange == BCUModeChangeType_t::BCU_Emergency_Stop)
				{
					m_preState = BCUStateType_t::BCU_DRIVE;
					m_bcuState = BCUStateType_t::BCU_Emergency_Stop;
					m_status = RELAYStatusType_t::Emergency_Stop;
					
					RLY_NFB = NFB_OFF;			
					RLY_K5_FAN = RLY_OFF; //K5 OFF
					Balance::PassiveBalanceCommand_ON_flag = 0; //close Passive Balance
					Balance::PassiveBalancejudge_flag = 0;
					for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
					{
						for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
						{
							BatterySys::pack[x].cellBalance[y] = 0;				
						}
					}			
				
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_OFF;
					RLY_Precharge = RLY_OFF;
					count = 0U;
					NFBCount = 0U;
					NFB_FAULT_Count = 0U;
					m_bcuModeChange = BCUModeChangeType_t::NONE;
					
				}
				else //illegal command
				{
					m_bcuModeChange = BCUModeChangeType_t::NOT_VALID;                   //0x80
				}
				
	
			}
			break;

			case BCUStateType_t::BCU_CHARGE:		//0x40		Charge Mode
			{
				m_bcuDrive = BCUDriveType_t::CHARGE;

		        CheckEvent(BCUStateType_t::BCU_CHARGE);
				
				if (RLY_FB_NFB == NFB_FB_OFF)
				{
					NFB_FAULT_Count++;
					if (NFB_FAULT_Count == RELAY_CHECK_TIME)
					{
						RLY_NFB = NFB_OFF;				
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						m_preState = BCUStateType_t::BCU_CHARGE; //save present state
						m_bcuState = BCUStateType_t::ESS_NFB_FAULT; //set next state
						BmsEngine::Flag::ESS_BreakerFault = true;
						NFB_FAULT_Count = 0U;
						NFBCount = 0U;
						m_bcuModeChange = BCUModeChangeType_t::NONE;
					}
				}
				else if (RLY_FB_HV_Main == RLY_FB_OFF)
				{
					BmsEngine::Main_FAULT_Count++;
					if (BmsEngine::Main_FAULT_Count == RELAY_CHECK_TIME)
					{
						RLY_NFB = NFB_OFF;				
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						m_preState = BCUStateType_t::BCU_CHARGE; //save present state
						m_bcuState = BCUStateType_t::ESS_HV_MAINRELAY_FAULT; //set next state
						BmsEngine::Flag::ESS_HV_MainRelayFault = true;
						BmsEngine::Main_FAULT_Count = 0U;
						NFBCount = 0U;
						m_bcuModeChange = BCUModeChangeType_t::NONE;
					}
				}
				else if (RLY_FB_HV_Neg == RLY_FB_OFF)
				{
					BmsEngine::Neg_FAULT_Count++;
					if (BmsEngine::Neg_FAULT_Count == RELAY_CHECK_TIME)
					{
						RLY_NFB = NFB_OFF;				
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						m_preState = BCUStateType_t::BCU_CHARGE; //save present state
						m_bcuState = BCUStateType_t::ESS_HV_NEGRELAY_FAULT; //set next state
						BmsEngine::Flag::ESS_HV_NegRelayFault = true;
						BmsEngine::Neg_FAULT_Count = 0U;
						NFBCount = 0U;
						m_bcuModeChange = BCUModeChangeType_t::NONE;
					}
				}
				else if (RLY_FB_Precharge == RLY_FB_ON)
				{
					BmsEngine::Precharge_FAULT_Count++;
					if (BmsEngine::Precharge_FAULT_Count == RELAY_CHECK_TIME)
					{
						RLY_NFB = NFB_OFF;				
						RLY_Precharge = RLY_OFF;
						RLY_HV_Main = RLY_OFF;
						RLY_HV_Neg = RLY_OFF;
						m_preState = BCUStateType_t::BCU_CHARGE; //save present state
						m_bcuState = BCUStateType_t::ESS_PRECHGRELAY_FAULT; //set next state
						BmsEngine::Flag::ESS_PreChargeRelayFault = true;
						BmsEngine::Precharge_FAULT_Count = 0U;
						NFBCount = 0U;
						m_bcuModeChange = BCUModeChangeType_t::NONE;
					}
				}
				else
				{
					BmsEngine::NFB_FAULT_Count = 0U;
					BmsEngine::Main_FAULT_Count = 0U;
					BmsEngine::Neg_FAULT_Count = 0U;
					BmsEngine::Precharge_FAULT_Count = 0U;
				}
				
				if (BatterySys::BMU_FAN_ON == 1)
				{
					if (RLY_FB_FAN == RLY_FB_OFF)
					{	
						BmsEngine::FAN_Warning_Count++;
						if (BmsEngine::FAN_Warning_Count == RELAY_CHECK_TIME)
						{
							BmsEngine::Flag::ESS_FAN_Warning = true;	
						}
					}
					else
					{
						BmsEngine::FAN_Warning_Count = 0U;
						BmsEngine::Flag::ESS_FAN_Warning = false;
					}
				}
				else
				{
					if (RLY_FB_FAN == RLY_FB_ON)
					{	
						BmsEngine::FAN_Warning_Count++;
						if (BmsEngine::FAN_Warning_Count == RELAY_CHECK_TIME)
						{
							BmsEngine::Flag::ESS_FAN_Warning = true;	
						}
					}
					else
					{
						BmsEngine::FAN_Warning_Count = 0U;
						BmsEngine::Flag::ESS_FAN_Warning = false;
					}
				}

				if (m_bcuModeChange == BCUModeChangeType_t::STANDBY)
				{					
//					NFBCount++;			
//					RLY_NFB = NFB_OFF;

//					if (NFBCount == 2U*SECOND_1)
//					{
//						if (RLY_FB_NFB == NFB_FB_OFF)
//						{
							m_preState = BCUStateType_t::BCU_CHARGE;
							m_bcuState = BCUStateType_t::BCU_STANDBY;
							m_status = RELAYStatusType_t::STANDBY;
	
							RLY_HV_Main = RLY_OFF;
							RLY_HV_Neg = RLY_OFF;
							RLY_Precharge = RLY_OFF;
							count = 0U;
							NFBCount = 0U;
							NFB_FAULT_Count = 0U;
							m_bcuModeChange = BCUModeChangeType_t::NONE;
//						}
//					}			
				}
				else if (m_bcuModeChange == BCUModeChangeType_t::BCU_Emergency_Stop)
				{
					m_preState = BCUStateType_t::BCU_CHARGE;
					m_bcuState = BCUStateType_t::BCU_Emergency_Stop;
					m_status = RELAYStatusType_t::Emergency_Stop;
					
					RLY_NFB = NFB_OFF;
					RLY_K5_FAN = RLY_OFF; //K5 OFF
					Balance::PassiveBalanceCommand_ON_flag = 0; //close Passive Balance
					Balance::PassiveBalancejudge_flag = 0;
					for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)//
					{
						for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)//
						{
							BatterySys::pack[x].cellBalance[y] = 0;				
						}
					}
					
					RLY_HV_Main = RLY_OFF;
					RLY_HV_Neg = RLY_OFF;
					RLY_Precharge = RLY_OFF;
					count = 0U;
					NFBCount = 0U;
					NFB_FAULT_Count = 0U;
					m_bcuModeChange = BCUModeChangeType_t::NONE;
					
				}
				else //illegal command
				{
					m_bcuModeChange = BCUModeChangeType_t::NOT_VALID; //0x80
				}
				
			}
			break;


			case BCUStateType_t::BCU_FAULT:
			{
				
				#if defined(FAULT_AUTO_RECOVERY)
				EFerryStateRecovery(BCUStateType_t::BCU_FAULT);
				#endif
			}
			break;

			case BCUStateType_t::BCU_COCP:
			{

				#if defined(COCP_AUTO_RECOVERY)
				EFerryStateRecovery(BCUStateType_t::BCU_COCP);
				#endif
			}
			break;

			case BCUStateType_t::BCU_DOCP:
			{

				#if defined(DOCP_AUTO_RECOVERY)
				//Auto Recovery from DOCP Error event
				EFerryStateRecovery(BCUStateType_t::BCU_DOCP);
				#endif
			}
			break;

			case BCUStateType_t::BCU_OVP:
			{
				#if defined(OVP_AUTO_RECOVERY)
				//StateRecovery(BCUStateType_t::BCU_OVP); //Auto Recovery from OVP Error event
				EFerryStateRecovery(BCUStateType_t::BCU_OVP);
				#endif
			}
			break;

			case BCUStateType_t::BCU_UVP:
			{

			#if defined(UVP_AUTO_RECOVERY)
			EFerryStateRecovery(BCUStateType_t::BCU_UVP);
			#endif
		    }
			break;
			
			case BCUStateType_t::BCU_OTCP:
			{			
				#if defined(OVP_AUTO_RECOVERY)	
				EFerryStateRecovery(BCUStateType_t::BCU_OTCP);
				#endif
			}
			break;
			
			case BCUStateType_t::BCU_OTDP:
			{			
				#if defined(OVP_AUTO_RECOVERY)	
				EFerryStateRecovery(BCUStateType_t::BCU_OTDP);
				#endif
			}
			break;
			
			case BCUStateType_t::BCU_UTCP:
			{			
				#if defined(OVP_AUTO_RECOVERY)	
				EFerryStateRecovery(BCUStateType_t::BCU_UTCP);
				#endif
			}
			break;
			
			case BCUStateType_t::BCU_UTDP:
			{		
				#if defined(OVP_AUTO_RECOVERY)	
				EFerryStateRecovery(BCUStateType_t::BCU_UTDP);
				#endif
			}
			break;
			

			
			case BCUStateType_t::BCU_HVIL_FAULT:
			{

				#if defined(HVIL_AUTO_RECOVERY)
				EFerryStateRecovery(BCUStateType_t::BCU_HVIL_FAULT);
				#endif
			}
			break;
			
			case BCUStateType_t::ESS_NFB_FAULT:
			{
		
//			#if defined (Breaker)
//				if (RLY_FB_NFB == NFB_FB_OFF)
////				if (RLY_FB_D == RLY_FB_OFF) 
//				{
//					NFB_FAULT_Count++;
//					if (NFB_FAULT_Count >= 10*TIME_1SEC)
//					{
//						BmsEngine::flag::ESS_BreakerFault = false;
//						NFB_FAULT_Count = 0;
//					}
//				}
//				else
//				{
//					NFB_FAULT_Count = 0;
//				}
//			
//			#endif //Breaker			
				
				//#if defined(HVIL_AUTO_RECOVERY)
//				eFerryStateRecovery(BCUStateType_t::ESS_NFB_FAULT);
				//#endif
			}
			break;
			
			case BCUStateType_t::ESS_PRECHGRELAY_FAULT:			//0xEC
			{

			#if defined(ESS_PreCHGRelay_FAULT_AUTO_RECOVERY)
				eFerryStateRecovery(BCUStateType_t::ESS_PreCHGRelay_FAULT);
			#endif
			}
			break;			
			
			case BCUStateType_t::BMU_ADC_FAULT:
			{

				//#if defined(OGP_AUTO_RECOVERY)
				EFerryStateRecovery(BCUStateType_t::BMU_ADC_FAULT);
				//#endif
			}
			break;
			
			case BCUStateType_t::BPU_OTP:
			{

				//#if defined(OGP_AUTO_RECOVERY)
				EFerryStateRecovery(BCUStateType_t::BPU_OTP);
				//#endif
			}
			break;
			
			
			default:
			{
				m_status = RELAYStatusType_t::RELAY_OFF;
			
				RLY_NFB = NFB_OFF;
				RLY_Precharge = RLY_OFF;
				RLY_HV_Main = RLY_OFF;
				RLY_HV_Neg = RLY_OFF;
			}
			break;
		} //switch

		return (m_bcuState);
	}



	//BmsEngine::packChkCounter checker[26];
	void BmsEngine::LogEvent(uint16_t status)
	{
		m_eventLog[m_eventCount] = status;  //p_bmspara->m_bcuStatus;
//		p_bmspara->bms_memory[EVENT_LOG_OFFSET + m_eventCount] = status;  //p_bmspara->m_bcuStatus;

		m_eventCount++;
		if (m_eventCount >= EVENT_LOG_LENGTH) m_eventCount = 0;

//		p_bmspara->bms_memory[EVENT_COUNT_OFFSET] = m_eventCount;
	}

#if 1

	//SOC/SOH parameter retrieve & saving
	/// <summary>
	/// Sets the default capacity in non-volitile memory & set SOC/SOH.
	/// </summary>
	/// <param name="fcc">The FCC: 0.1Ah.</param>
	/// <param name="rmc">The RMC: 0.1Ah.</param>
	/// <param name="dc">The DC: 0.1Ah.</param>
	void BmsEngine::SetDefaultCapacity(uint16_t fcc, uint16_t rmc, uint16_t dc)
	{
		//check if resonable parameters, keep from dirty stored pars.
		if ((dc > DEFAULT_PACK_DC) || (fcc > DEFAULT_PACK_FCC))
		{
			m_designCapacity = DEFAULT_PACK_DC;
			m_fullChargeCapacity = DEFAULT_PACK_FCC;
			m_remainCapacity = DEFAULT_PACK_RMC;
		}
		else //retrieve parameters
		{
			m_designCapacity = dc;
			m_fullChargeCapacity = fcc;
			m_remainCapacity = rmc;
		}

		//check if resonable RMC
		if (rmc > fcc)
		{
			m_remainCapacity = m_fullChargeCapacity; //100.0%
		}

		//Recalculate Coulomb counter: m_dsgAccum (unit:0.1Ah)
		//and set m_soc & m_soh
		ResetCoulombCountSoc();

		//Save to non-volatile memory
		PutFullChargeCapacity(m_fullChargeCapacity);
		PutRemainCapacity(m_remainCapacity);
		PutDesignCapacity(m_designCapacity);
	}

	void BmsEngine::ResetCoulombCountSoc()
	{
		//Recalculate Coulomb counter: m_dsgAccum (unit:0.1Ah)
		if (m_fullChargeCapacity > m_remainCapacity)
		{
			m_dsgAccum = m_fullChargeCapacity - m_remainCapacity;
		}
		else
		{
			m_dsgAccum = 0;
			m_remainCapacity = m_fullChargeCapacity; //update remain capacity
		}

		//conversion from 0.1Ah to 0.05ASEC: (0.05ASEC to 0.1Ah) (m_dsgAccum: unit=0.1Ah)
		//m_dsgAccum = (uint16_t)(m_coulombCounter / COULOMB_CONVERSION); // 0.1Ah=7200*0.05ASEC (360ASEC)
		coulombCount = (uint32_t)m_dsgAccum * COULOMB_CONVERSION;

		float soc_f = (float)m_remainCapacity / (float)m_fullChargeCapacity; //SOC
		float soh_f = (float)m_fullChargeCapacity / (float)m_designCapacity; //SOH
		if (soh_f > 1.0) soh_f = 1.0; //saturate

////		m_soc = (uint16_t)(soc_f * 1000.0); //0.1%
		m_soh = (uint16_t)(soh_f * 1000.0); //0.1%
	}


	void BmsEngine::RetrieveStoredCapacity()			//Init SOC
	{

		//retrieve parameters from non-valitile memory
		//m_fullChargeCapacity = GetFullChargeCapacity();
		//m_remainCapacity = GetRemainCapacity();
		//m_designCapacity = GetDesignCapacity();
                                                                                                                                                                                                     
		uint16_t readDC = GetDesignCapacity();	//init
		uint16_t readFCC = GetFullChargeCapacity();	//init
		uint16_t readRMC = GetRemainCapacity();	//init

		//RMC = DC-FCC
		//DEFAULT_PACK_FCC
		//DEFAULT_PACK_RMC

		//check if resonable parameters, keep from dirty stored pars.
		if ((readDC > DEFAULT_PACK_DC) || (readFCC > DEFAULT_PACK_FCC))
		{
			m_designCapacity = DEFAULT_PACK_DC;
			m_fullChargeCapacity = DEFAULT_PACK_FCC;
			m_remainCapacity = DEFAULT_PACK_RMC;
		}
		else //retrieve parameters
		{
			m_designCapacity = readDC;
			m_fullChargeCapacity = readFCC;
			m_remainCapacity = readRMC;
		}

		//check if resonable RMC
		if (readRMC > readFCC)
		{
			m_remainCapacity = m_fullChargeCapacity; //100.0%
		}

		//Recalculate Coulomb counter: m_dsgAccum (unit:0.1Ah)
		//and set m_soc & m_soh
		ResetCoulombCountSoc();
	}

	// @param
	void BmsEngine::SetDefaultRtcBackup()
	{
		m_bcuId = DEFAULT_BCU_ID;
		SetBcuId(m_bcuId); //svae to RTC Backup

		//save to RTC Backup
		SetDefaultCapacity(DEFAULT_PACK_FCC, DEFAULT_PACK_RMC, DEFAULT_PACK_DC);
	}
#endif

	// SOC measurement based on bi-directional Honeywell Hall Sensor(1mA) +-500A.
	// <param name="ampere">unit: 1mA (Discharge is positive, charging is negative).</param>
	void BmsEngine::StateOfCharge(int32_t ampere) //mA
	{
		float current_ma = 0;	//init
		uint64_t FCC_mA = 0;	//init
		float SOC_temp = 0;	//init
		socCounter++;  //if SOC Updated inc socCounter
	//SOC::FCCcoulombCount_Flag = 1;
	#if defined(AUTO_ZERO_CURRENT)

		if ((m_bcuDrive == BCUDriveType_t::STANDBY) //auto zeroing for standby mode
		{
			if (abs(ampere) < ZERO_THREADSHOUYLD_UNIT_100MA)
			{
				//calculate MV(16) for zeroing current
				int16_t ampere_zero = 0;
				ampere_zero = ampere;
			}
			else //alarm
			{
				ampere_zero = 0;
			}
		}
		else
		{
		}
	#endif

	#if defined(SOC_STATE_OREINTED) //Using State for Chg/Dsg
		//SOC Calculation Here:
		//==============================================
		if (m_bcuDrive != BCUDriveType_t::CHARGE) //discharge or standby mode:
		{
			//TODO: full Charge Capacity estimation required

			//for RS485 conersion to mA
			current_ma = (uint32_t)(abs(ampere)) * 100L; //meter reading unit:100mA

			//Coulomb Counting:
			coulombCount += abs(ampere); //0.5sec inc/0.1A => unit:0.05ASEC
			coulombCount += offset_100ma; //~ 2*0.58uA self-consumption estimation (only in discharge mode)

			//saturation control of integration:
			//if (coulombCount > (uint32_t)(m_designCapacity) * COULOMB_CONVERSION)
			//{
			//	coulombCount = (uint32_t)(m_designCapacity) * COULOMB_CONVERSION; //0.1Ah to 0.05Asec
			//}

			if (coulombCount > (uint32_t)(m_fullChargeCapacity) * COULOMB_CONVERSION)
			{
				coulombCount = (uint32_t)(m_fullChargeCapacity) * COULOMB_CONVERSION; //0.1Ah to 0.05Asec
			}
			//conversion to 0.1Ah: (0.05ASEC to 0.1Ah)
			m_dsgAccum = (uint16_t)(coulombCount / COULOMB_CONVERSION); // 0.1Ah=7200*0.05ASEC (360ASEC)

			//RMC update:
			if (m_dsgAccum < m_fullChargeCapacity) m_remainCapacity = m_fullChargeCapacity - m_dsgAccum;
			else m_remainCapacity = 0;
		}
		else //charging mode:
		{
			current_ma = (uint32_t)(abs(ampere)) * 100L; //meter reading unit:100mA

			//Add self-consumption compensation here:
			//current_ma -= offset;

			//Coulomb Counting:
			if (coulombCount >= abs(ampere)) coulombCount -= abs(ampere); //0.5sec inc/0.1A => unit:0.05ASEC
			else
			{
				coulombCount = 0; //minimum value: 0
			}

			m_dsgAccum = (uint16_t)(coulombCount / COULOMB_CONVERSION); // 0.1Ah=7200*0.05ASEC (360ASEC)

			//RMC Update:
			if (m_dsgAccum < m_fullChargeCapacity) m_remainCapacity = m_fullChargeCapacity - m_dsgAccum;

			//Saturation control: 0 <= m_remainCapacity <= m_designCapacity
			if (m_remainCapacity > m_fullChargeCapacity) m_remainCapacity = m_fullChargeCapacity;

			//m_fccAccum update:
			//cycle count update
		}

	#else //Curent reading sign for chg/dsg (plus:dsg, minus chg)

	if (SOC::Set_BCU_SOC_Flag == 1)		//Set BCU SOC
	{
		//BmsEngine::coulombCount = (float)((float)SetSOC / 1000
		//*(float)(BmsEngine::m_fullChargeCapacity*COULOMB_CONVERSION_mA));
		FCC_mA = (float)((float)m_fullChargeCapacity* COULOMB_CONVERSION_MA);
		SOC_temp  = (float)coulombCount  / (float)FCC_mA;           //
		m_soc = (uint16_t)(SOC_temp * 1000.0);
		SOC::Set_BCU_SOC_Flag = 0;		
	}		
	
	if (BmsEngine::Flag::SOC5_Calibration_START == 1)
	{
		FCC_mA = (float)((float)m_fullChargeCapacity* COULOMB_CONVERSION_MA);
		BmsEngine::coulombCount = (float)((float)SOC5_Calibration/100 * (float)(m_fullChargeCapacity*COULOMB_CONVERSION_MA));
		SOC_temp  = ((float)coulombCount / (float)FCC_mA); //
		m_soc = (uint16_t)(SOC_temp * 1000.0);
		BmsEngine::Flag::SOC5_Calibration_START = 0;	
	
	}
	else if (BmsEngine::Flag::SOC95_Calibration_START == 1)
	{
		FCC_mA = (float)((float)m_fullChargeCapacity* COULOMB_CONVERSION_MA);
		BmsEngine::coulombCount = (float)((float)SOC95_Calibration/100 * (float)(m_fullChargeCapacity*COULOMB_CONVERSION_MA));
		SOC_temp  = ((float)coulombCount / (float)FCC_mA); //
		m_soc = (uint16_t)(SOC_temp * 1000.0);
		BmsEngine::Flag::SOC95_Calibration_START = 0;	
	
	}
		
	#if defined( ZERO_CURRENT_THRESHOULD)
	if (abs(ampere) < ZERO_THRESHOULD_MA) 
	{
		if ((BmsEngine::m_bcuState == BCUStateType_t::BCU_STANDBY) && (BatterySys::MinVolt <= FULLSOC))
		{
			if (socCounter >= 100)			//time 10ms*100=1000ms
			{
				socCounter = 0;
				current_ma = SELF_CONSUMPTION;        //honeywell sensor reading unit:1mA
			}
			else 
			{
				current_ma = 0;
			}

			//Add self-consumption compensation here:			
			if (coulombCount >= current_ma)
			{
				coulombCount -= current_ma;
			}
			else
			{
				coulombCount = 0;
			}
			
				
			if (coulombCount > m_fullChargeCapacity*COULOMB_CONVERSION_MA)
				coulombCount = m_fullChargeCapacity*COULOMB_CONVERSION_MA;
			
			FCC_mA = (float)((float)m_fullChargeCapacity* COULOMB_CONVERSION_MA);
			SOC_temp  = (float)coulombCount / (float)FCC_mA;            //
//			float soh_f = (float)m_fullChargeCapacity / (float)m_designCapacity;
//			if (soh_f > 1.0) soh_f = 1.0;   //saturate
//
//			m_soc = (uint16_t)(temp2 * 1000.0);
		}
//		return 0;
	}
	#endif

	if ((ampReading_mA >= CHARGE_ZERO_THRESHOULD_MA) && (BatterySys::MinVolt >= EMPTYSOC))	//Charging State ?SOC_Tab[1]	
	{
		if (socCounter >= 100)			//time 10ms*100=1000ms
		{
			current_ma = (float)((abs(ampere) - SELF_CONSUMPTION) * 0.01);     //honeywell hall sensor reading unit:1mA
			socCounter = 0;
		}
		else
		{
			current_ma = (float)((abs(ampere)) * 0.01);      //honeywell hall sensor reading unit:1mA
		}
		
		//Coulomb Counting:
		coulombCount += current_ma;	//coulombCount = (float)coulombCount + (current_ma);

		//SOH
		if (SOC::FCCcoulombCount_Flag == 1)
		{
			if (SOC::FCCcoulombCount >= current_ma)
			{
				SOC::FCCcoulombCount -= current_ma;
			}
			else
			{
				SOC::FCCcoulombCount = 0;
			}
			
		}
		

		//SOC 100%
		if (SOC::FullCHG_END_Flag == 0)
		{
			if (coulombCount >= (m_fullChargeCapacity*COULOMB_CONVERSION_MA))
			{
				if (BmsEngine::SBCUCANMessageCapacity::FullCharge == 1)
				{
					coulombCount = m_fullChargeCapacity*COULOMB_CONVERSION_MA;
					
				}
				else
				{
					coulombCount = (m_fullChargeCapacity-1)*COULOMB_CONVERSION_MA;
				}
			
			}
			
		}
		else if (SOC::FullCHG_END_Flag == 1)
		{
			coulombCount = m_fullChargeCapacity*COULOMB_CONVERSION_MA;
			SOC::FullCHG_END_Flag = 0;
		}

		FCC_mA = (float)((float)m_fullChargeCapacity* COULOMB_CONVERSION_MA);
		SOC_temp  = (float)coulombCount / (float)FCC_mA;        //
			
	}
	else if ((ampReading_mA <= DISCHARGE_ZERO_THRESHOULD_MA) && (BatterySys::MaxVolt <= FULLSOC)) //Discharging state ?SOC_Tab[100]
	{
		if (socCounter >= 100)			//time 10ms*100=1000ms
		{
			current_ma = (float)((abs(ampere) - SELF_CONSUMPTION) * 0.01);     //honeywell sensor reading unit:1mA
			socCounter = 0;
		}
		else
		{
			current_ma = (float)((abs(ampere)) * 0.01);      //honeywell sensor reading unit:1mA
		}
		
		//Coulomb Counting:
		if (coulombCount >= current_ma)
		{
			coulombCount -= current_ma;
		}
		else
		{
			coulombCount = 0;
		}
		
		//SOH 
		if (SOC::FCCcoulombCount_Flag == 1)
		{
			SOC::FCCcoulombCount += current_ma;	
			
			if (SOC::FCCcoulombCount > BmsEngine::m_designCapacity*COULOMB_CONVERSION_MA)
			SOC::FCCcoulombCount = (uint32_t)(BmsEngine::m_designCapacity*COULOMB_CONVERSION_MA);
		}
		
		//SOC 0%
		if (SOC::FullDSG_END_Flag == 0)
		{
			if (coulombCount <= 0)	//
			{
				if (BmsEngine::SBCUCANMessageCapacity::FullDischarge == 1)
				{
					coulombCount = 0;
					
				}
				else
				{
					coulombCount = 1*COULOMB_CONVERSION_MA;
				}
			
			}
			
		}
		else if (SOC::FullDSG_END_Flag == 1)
		{
			coulombCount = 0;
			SOC::FullDSG_END_Flag = 0;
		}

		//Saturation control: 0 <= m_remainCapacity <= m_designCapacity
		if (coulombCount > m_fullChargeCapacity*COULOMB_CONVERSION_MA)
			coulombCount = m_fullChargeCapacity*COULOMB_CONVERSION_MA;
			
		FCC_mA = (float)((float)m_fullChargeCapacity* COULOMB_CONVERSION_MA);
		SOC_temp  = (float)coulombCount  / (float)FCC_mA;         //
			
	}
	else 
	{
		if (coulombCount >= current_ma)
		{
			coulombCount -= current_ma;
		}
		else
		{
			coulombCount = 0;
		}
		if (coulombCount > m_fullChargeCapacity*COULOMB_CONVERSION_MA)
			coulombCount = m_fullChargeCapacity*COULOMB_CONVERSION_MA;
			
		FCC_mA = (float)((float)m_fullChargeCapacity* COULOMB_CONVERSION_MA);
		SOC_temp  = (float)coulombCount  / (float)FCC_mA;          //

	}
#endif

		//SOC/SOH calculation :
		//SOHupdate=SOH_FCC/(1-SOC)
		//if (SOHupdate<SOH_FCC*0.95)
        //SOH_FCC=SOH_FCC*0.95;
        //else SOH_FCC=SOHupdate;
		
		//	 SOH = FCC / DC
		//   SOC = RMC / FCC

		if (SOC::SOHupdate_END_Flag == 1)
		{
			uint32_t m_DC_temp = (uint32_t)BmsEngine::m_designCapacity * COULOMB_CONVERSION_MA;  
			
			if (SOC::FCCcoulombCount < (BmsEngine::m_fullChargeCapacity * COULOMB_CONVERSION_MA * 0.95))	
			{
				SOC::FCCcoulombCount = BmsEngine::m_fullChargeCapacity * COULOMB_CONVERSION_MA * 0.95;	
			}
			else if (SOC::FCCcoulombCount >= (BmsEngine::m_fullChargeCapacity * COULOMB_CONVERSION_MA))
			{
				SOC::FCCcoulombCount = (uint32_t)(BmsEngine::m_fullChargeCapacity * COULOMB_CONVERSION_MA);
			}
			
			BmsEngine::m_fullChargeCapacity = (uint16_t)(SOC::FCCcoulombCount / COULOMB_CONVERSION_MA);
			//
			float soh_temp = (float)((float)SOC::FCCcoulombCount / (float)m_DC_temp)*(float)1000;  
			// if (BmsEngine::m_soc > 100)
			// LMDEND_soc = 100;
			float m_SOH_temp = soh_temp / ((float)1 - ((float)BmsEngine::m_soc / 1000));
			if (m_SOH_temp > 1000)
				m_SOH_temp = 1000;
			BmsEngine::m_soh = static_cast<uint16_t>(m_SOH_temp);
			//soh_f = SOC::FCCcoulombCount / (m_designCapacity* COULOMB_CONVERSION_mA);
			SOC::SOHupdate_END_Flag = 0;
			SOC::StoredSOH_Flag = 1;
		}
		////float soh_f = (float)m_fullChargeCapacity / (float)m_designCapacity;
		//if (soh_f > 1.0) soh_f = 1.0; //saturate

		//	m_soc = (uint16_t)(soc_f * 1000.0); //0.1%
		
//		if (SOC::FullCHG_END_Flag == 1)		//Full CHG SOC�甇�100%
//		{
//			m_soc = 1000;
//			coulombCount = m_fullChargeCapacity*COULOMB_CONVERSION_mA;
//			SOC::FullCHG_END_Flag = 0;
//		}
//		else if (SOC::FullDSG_END_Flag == 1)
//		{
//			m_soc = 0;
//			coulombCount = 0;
//			SOC::FullDSG_END_Flag = 0;
//		}
//		else
//		{
//
//			m_soc=(uint16_t)(temp2*1000.0);
//		}
		
		m_soc = (uint16_t)(SOC_temp*1000.0);
		//m_soh = (uint16_t)(soh_f * 1000.0); //0.1%

	}

	//Bcu Process
	void BmsEngine::BcuProcess()
	{
		//BCU FSM
		//BCU State Machine:
		//===============================================
		BcuFSM();

		//=================================================================

		//protection check
		if (m_bcuState == BCUStateType_t::BCU_INIT) //BCU_INT
		{
			return;
		}

		BmsEngine::SortCellVoltage();
		BmsEngine::MaxSort_Volt = BmsEngine::MaxCellVoltage();
		BmsEngine::MinSort_Volt = BmsEngine::MinCellVoltage();

		chkResult = CheckVolt_OV1();
		chkResult = CheckVolt_UV1();
		chkResult = CheckVolt_OV2();
		chkResult = CheckVolt_UV2();
		chkResult = CheckVolt_OV3();
		if (p_bmspara->m_bcuStatus & BCU_STATUS_CMCAN_FAULT) 
		{
//			return;		
		}
		else
		{
			chkResult = CheckVolt_UV3();
		}
	
		
		BmsEngine::SortCellTemperature();
		BmsEngine::MaxSort_Temp = BmsEngine::MaxCellTemperature();
		BmsEngine::MinSort_Temp = BmsEngine::MinCellTemperature();
		chkResult = CheckTemp_OTC1();
		chkResult = CheckTemp_OTC2();
		chkResult = CheckTemp_OTC3();
		chkResult = CheckTemp_OTD1();
		chkResult = CheckTemp_OTD2();
		chkResult = CheckTemp_OTD3();
		
		chkResult = CheckTemp_UTC1();
		chkResult = CheckTemp_UTC2();
		chkResult = CheckTemp_UTC3();
		chkResult = CheckTemp_UTD1();
		chkResult = CheckTemp_UTD2();
		chkResult = CheckTemp_UTD3();
		
		chkResult = CheckCurrent_OCC1();
		chkResult = CheckCurrent_OCC2();
		chkResult = CheckCurrent_OCC3();
		chkResult = CheckCurrent_OCD1();
		chkResult = CheckCurrent_OCD2();
		chkResult = CheckCurrent_OCD3();	
		
		chkResult = CheckImbalance_IV1();
		
//		BatterySys::PackAmbient_MaxTemp = BmsEngine::SortPack_MAXAmbientTemp();
//		BatterySys::PackAmbient_MinTemp = BmsEngine::SortPack_MINAmbientTemp();

//		BmsEngine::CheckTempCOAT1_PackAmbientTemp(BatterySys::PackAmbient_MaxTemp);
//		BmsEngine::CheckTempCOAT3_PackAmbientTemp(BatterySys::PackAmbient_MaxTemp);
//		BmsEngine::CheckTempCUAT1_PackAmbientTemp(BatterySys::PackAmbient_MinTemp);
//		BmsEngine::CheckTempCUAT3_PackAmbientTemp(BatterySys::PackAmbient_MinTemp);
//		BmsEngine::CheckTempDOAT1_PackAmbientTemp(BatterySys::PackAmbient_MaxTemp);
//		BmsEngine::CheckTempDOAT3_PackAmbientTemp(BatterySys::PackAmbient_MaxTemp);
//		BmsEngine::CheckTempDUAT1_PackAmbientTemp(BatterySys::PackAmbient_MinTemp);
//		BmsEngine::CheckTempDUAT3_PackAmbientTemp(BatterySys::PackAmbient_MinTemp);
		
		Check_CHG_Enable();
		Check_DSG_Enable();

		BmsEngine::Check_FullCHG();
		BmsEngine::Check_FullDSG();
				
		BmsEngine::CheckTempOT1_TA();
	

//		if (m_bcuState != BCUStateType_t::BCU_FAULT)
//		{
//			if (LTC6811SPI::communication_Temp_counter_Flag == 0)
//			{
//				BatterySys::AvgCellTemperature = BmsEngine::AVGCellTemperature();
//				chkResult = Check_ADCWarning();
//				chkResult = Check_ADCFault();
//			}
//
//
//		}


	

		chkResult = Check_SelfTestFault();
		//TODO: MBU & CM CAN BUS broken wire check
		//CM CAN BUS broken wire check:
		//===============================================
		

#if defined(EMS_BCU_CANBUS)
		chkResult = CheckEMSCanBusWire();
#endif	//EMS_BCU_CANBUS	

#if defined(HALLSENSOR_BCU_CANBUS)
		chkResult = CheckCurrentCanBusWire();
#endif	//HALLSENSOR_BCU_CANBUS		
		
#if defined(BMU_BCU_ISOSPI)
		BmsEngine::CheckIsoSPIcommunication();
#endif	//BMU_BCU_ISOSPI		
		
#if defined(BCU_TEMP_PROTECTION)	
		BmsEngine::CheckTempOT1_PCS_Positive();
		BmsEngine::CheckTempOT2_PCS_Positive();
		BmsEngine::CheckTempOT1_PCS_Negative();
		BmsEngine::CheckTempOT2_PCS_Negative();
		
		BmsEngine::CheckTempOT1_HVDC_Positive();
		BmsEngine::CheckTempOT2_HVDC_Positive();
		BmsEngine::CheckTempOT1_HVDC_Negative();
		BmsEngine::CheckTempOT2_HVDC_Negative();
		
		BmsEngine::CheckTempOT1_Fuse();
		BmsEngine::CheckTempOT2_Fuse();
#endif	//BCU_TEMP_PROTECTION
		
		if (m_bcuState != BCUStateType_t::BCU_FAULT)
		{
			BmsEngine::CheckSOC5_Calibration(); //SOC 5% Calibration
//			BmsEngine::CheckSOC95_Calibration(); //SOC 95% Calibration
		}

//#endif	//BCU_TEST_PROTECTION	
		
		SOC::SOHUpdate_Action();
		
		CheckEvent(BCUStateType_t::BCU_IDLE);
	
	} //BcuProcess
} //namespace bmstech
