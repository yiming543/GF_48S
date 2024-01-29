//File=======   2023-08   Truewin ================================
//Arthor: Justin		
//Date: 2023-08-01
//Application:	ESS
//Version: V0.0.1
//History:

//	2023-08-28 V0.0.1  Checksum 0x
//  1. LTO ADBMS1818 10S initial

//20240110 V0.0.2
//6815 ADC mode 7K ==> 2K (修正電供表頭讀值和6815讀值誤差) 轉換時間3.23ms
//ADC轉換頻率太高電壓誤差太大,太低轉換時間太長
//isoSPI最大1000Khz(因為BCU上6820最多支援到1000KZ)

//20240129 V0.0.3
//ADC 2KHZ
//isoSPI 1MHZ
//使用GPIO367控制多工器GSM4518
//切換完GPIO367 要等待14ms
//使用GPIO1 讀取ADC


#include <mbed.h>

#include "BatterySys.h"
#include "BmsEngine.h"
#include "Watchdog.h"

#include "VersionInfo.h"
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"

#include "Balance.h"
#include "SOC.h"
#include "CY15B256J_I2C.h"

#include "LTC6811_daisy.h"
#include "ADBMS6815_daisy.h"
#include "ADC_Temperature.h"

#include "main.h"
#include "stm32fxx_STLparam.h"
#include "stm32fxx_STLlib.h"
#include "stm32fxx_STLmain.h"
//
//#include "rtos.h"

//#include "NetworkInterface.h"
//#include "TLSSocket.h"
using namespace bmstech;

CAN *p_bmuCan1 = new CAN(CAN1_RXD, CAN1_TXD, BMU_CAN_BAUD_250K);	//for CM(BMU) CAN Bus
CAN *p_emsCan2 = new CAN(CAN2_RXD, CAN2_TXD, EMS_CAN_BAUD_250K);	//for MBU(EMS) CAN Bus

//===============================================================================
BmsEngine *ptr_bms = new BmsEngine();

//BmsEngine Class static member init:
BCUStateType_t	BmsEngine::m_bcuState = BCUStateType_t::BCU_INIT;
BCUStateType_t	BmsEngine::m_preState = BCUStateType_t::BCU_INIT;
RELAYStatusType_t BmsEngine::m_status = RELAYStatusType_t::RELAY_OFF;
BCUDriveType_t	BmsEngine::m_bcuDrive = BCUDriveType_t::STANDBY;
volatile BCUModeChangeType_t BmsEngine::m_bcuModeChange = BCUModeChangeType_t::NONE;

BmsEngine* BmsEngine::p_bmspara = ptr_bms;

uint32_t BmsEngine::count = 0U;
uint32_t BmsEngine::fsmCount = 0U;
uint32_t BmsEngine::NFBCount = 0U;

uint16_t BmsEngine::m_bcuStatus = 0U;
uint16_t BmsEngine::m_bcuStatus1 = 0U;
uint16_t BmsEngine::m_bcuStatus2 = 0U;
uint16_t BmsEngine::m_bcuStatus3 = 0U;
uint16_t BmsEngine::m_bcuStatus4 = 0U;
uint16_t BmsEngine::m_bcuStatus5 = 0U;
uint16_t BmsEngine::m_bcuStatus6 = 0U;


//int BmsEngine::m_timeoutCount = 0U;

uint16_t BmsEngine::m_dsgAccum = DEFAULT_PACK_FCC - DEFAULT_PACK_RMC;
float BmsEngine::coulombCount = (BmsEngine::m_dsgAccum) * COULOMB_CONVERSION;
uint16_t BmsEngine::m_remainCapacity = DEFAULT_PACK_RMC;
uint16_t BmsEngine::m_fullChargeCapacity = DEFAULT_PACK_FCC;
uint16_t BmsEngine::m_designCapacity = DEFAULT_PACK_DC;
uint16_t BmsEngine::m_soc = 0U;   //0.0%
uint16_t BmsEngine::m_soh = 1000;  //100.0%

volatile uint8_t BmsEngine::m_bcuId = 0x00U;

//BMU: battery Monitor Unit, BPU: Battery Protection Unit
BatterySys* BatterySys::pBattSys = new BatterySys{p_bmuCan1, BMU_CAN_BAUD_250K, p_emsCan2, EMS_CAN_BAUD_250K, ptr_bms};
BatterySys *pBatt = BatterySys::pBattSys;//

//static members init:
uint32_t BatterySys::canCmCounter = 0U;
uint32_t BatterySys::canMbuCounter = 0U;
uint32_t BatterySys::canCurrCounter = 0U;
uint32_t BatterySys::canCurrCounter2nd = 0U;   
uint32_t BatterySys::canEmsCounter = 0U;  //current sensor can bus count
uint32_t BatterySys::canEmsCounter2nd = 0U;   

uint16_t LTC6811SPI::cell_codes[PackConst::MAX_PACK_NUM][12] = { 0 };

Pack_t BatterySys::pack[PackConst::MAX_PACK_NUM] = { 0 };
//Pack_t BatterySys::pack[PackConst::MAX_PACK_NUM] = { 
//	{ {0}, {0}, {0}, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0}, {0}, 0, 0, {0}, {0}, {0}, 0, 0, 0, 0, 0 },
//	{ {0}, {0}, {0}, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0}, {0}, 0, 0, {0}, {0}, {0}, 0, 0, 0, 0, 0 }
//};		

uint16_t BatterySys::MaxVolt = 0U;
uint16_t BatterySys::MaxVoltModule = 0U;
uint16_t BatterySys::MaxVoltCH = 0U;
uint16_t BatterySys::MinVolt = 0U;
uint16_t BatterySys::MinVoltModule = 0U;
uint16_t BatterySys::MinVoltCH = 0U;

int16_t BatterySys::MaxTemp = 0;
uint16_t BatterySys::MaxTempCH = 0U;
uint16_t BatterySys::MaxTempModule = 0U;
int16_t BatterySys::MinTemp = 0;
uint16_t BatterySys::MinTempCH = 0U;
uint16_t BatterySys::MinTempModule = 0U;

//uint16_t BatterySys::CellBalance = 0;
int16_t BatterySys::PackAmbient_MaxTemp = 0;
int16_t BatterySys::PackAmbient_MinTemp = 0;

uint16_t Balance::Average_voltReading = 0U;


bool_t SOC::Set_BCU_SOC_Flag = 0;
bool_t SOC::Set_BCU_SOC_Time_Flag = 0;
uint8_t SOC::RegisterSetSOC_Time = 0U;


bool_t SOC::Set_BCU_SOH_Flag = 0;

	
bool_t SOC::Count_down_6hr_Flag = 0;

bool BatterySys::SendBMUVerFlag = 0;
bool_t BatterySys::CANSendBMUVer_Flag = 0;


//bool_t Balance::TBalComm_Flag = 0;

uint16_t BatterySys::m_voltReading = 0;
int16_t BatterySys::m_ampReading = 0;
//int16_t BatterySys::m_leakReading = 0;
uint8_t BatterySys::bcuSetCount = 0U;
uint32_t  BatterySys::m_sysVolt = 0U;
int32_t BatterySys::totalCurrent = 0;
int32_t BatterySys::current[] = {0};	//20220607
int32_t BatterySys::rawCurrent = 0;		//20220607

int16_t BatterySys::AvgCellTemperature = 0;

uint16_t BmsEngine::MaxSort_Volt = 0U;
uint16_t BmsEngine::MinSort_Volt = 0U;


uint8_t Balance::MaxPackVolt_id_NUM = 0U;
uint8_t Balance::MinPackVolt_id_NUM = 0U;

uint16_t SOC::OCV_SOC = 0U;
bool_t SOC::FCCcoulombCount_Flag = 0;
uint32_t SOC::FCCcoulombCount = 0;

bool_t SOC::SOHupdate_END_Flag = 0;
bool_t SOC::SOHupdate_Start_Flag = 0;
bool_t SOC::StoredSOH_Flag = 0;
bool_t SOC::SET_StoredSOH_Flag = 0;

bool BmsEngine::Flag::OV1 = 0;
bool BmsEngine::Flag::ov1_deboc = 0;
uint8_t BmsEngine::Time::OV1_time_delay = OV1_CHECK_TIME;
bool BmsEngine::Flag::UV1 = 0;
bool BmsEngine::Flag::uv1_deboc = 0;
uint8_t BmsEngine::Time::UV1_time_delay = UV1_CHECK_TIME;

bool BmsEngine::Flag::OV2 = 0;
bool BmsEngine::Flag::ov2_deboc = 0;
uint8_t BmsEngine::Time::OV2_time_delay = OV2_CHECK_TIME;

bool BmsEngine::Flag::UV2 = 0;
bool BmsEngine::Flag::uv2_deboc = 0;
uint8_t BmsEngine::Time::UV2_time_delay = UV2_CHECK_TIME;
bool BmsEngine::Flag::OV3 = 0;
bool BmsEngine::Flag::ov3_deboc = 0;
uint8_t BmsEngine::Time::OV3_time_delay = OV3_CHECK_TIME;
bool BmsEngine::Flag::UV3 = 0;
bool BmsEngine::Flag::uv3_deboc = 0;
uint8_t BmsEngine::Time::UV3_time_delay = UV3_CHECK_TIME;

bool BmsEngine::Flag::OTC1 = 0;
bool BmsEngine::Flag::otc1_deboc = 0;
uint8_t BmsEngine::Time::OTC1_time_delay = OTC1_CHECK_TIME;
bool BmsEngine::Flag::UTC1 = 0;
bool BmsEngine::Flag::utc1_deboc = 0;
uint8_t BmsEngine::Time::UTC1_time_delay = UTC1_CHECK_TIME;

bool BmsEngine::Flag::OTC2 = 0;
bool BmsEngine::Flag::otc2_deboc = 0;
uint8_t BmsEngine::Time::OTC2_time_delay = OTC2_CHECK_TIME;
bool BmsEngine::Flag::UTC2 = 0;
bool BmsEngine::Flag::utc2_deboc = 0;
uint8_t BmsEngine::Time::UTC2_time_delay = UTC2_CHECK_TIME;

bool BmsEngine::Flag::OTC3 = 0;
bool BmsEngine::Flag::otc3_deboc = 0;
uint8_t BmsEngine::Time::OTC3_time_delay = OTC3_CHECK_TIME;
bool BmsEngine::Flag::UTC3 = 0;
bool BmsEngine::Flag::utc3_deboc = 0;
uint8_t BmsEngine::Time::UTC3_time_delay = UTC3_CHECK_TIME;

bool BmsEngine::Flag::OTD1 = 0;
bool BmsEngine::Flag::otd1_deboc = 0;
uint8_t BmsEngine::Time::OTD1_time_delay = OTD1_CHECK_TIME;
bool BmsEngine::Flag::UTD1 = 0;
bool BmsEngine::Flag::utd1_deboc = 0;
uint8_t BmsEngine::Time::UTD1_time_delay = UTD1_CHECK_TIME;

bool BmsEngine::Flag::OTD2 = 0;
bool BmsEngine::Flag::otd2_deboc = 0;
uint8_t BmsEngine::Time::OTD2_time_delay = OTD2_CHECK_TIME;
bool BmsEngine::Flag::UTD2 = 0;
bool BmsEngine::Flag::utd2_deboc = 0;
uint8_t BmsEngine::Time::UTD2_time_delay = UTD2_CHECK_TIME;

bool BmsEngine::Flag::OTD3 = 0;
bool BmsEngine::Flag::otd3_deboc = 0;
uint8_t BmsEngine::Time::OTD3_time_delay = OTD3_CHECK_TIME;
bool BmsEngine::Flag::UTD3 = 0;
bool BmsEngine::Flag::utd3_deboc = 0;
uint8_t BmsEngine::Time::UTD3_time_delay = UTD3_CHECK_TIME;

bool BmsEngine::Flag::OCC1 = 0;
bool BmsEngine::Flag::occ1_deboc = 0;
uint8_t BmsEngine::Time::OCC1_time_delay = OCC1_CHECK_TIME;
bool BmsEngine::Flag::OCD1 = 0;
bool BmsEngine::Flag::ocd1_deboc = 0;
uint8_t BmsEngine::Time::OCD1_time_delay = OCD1_CHECK_TIME;

bool BmsEngine::Flag::OCC2 = 0;
bool BmsEngine::Flag::occ2_deboc = 0;
uint8_t BmsEngine::Time::OCC2_time_delay = OCC2_CHECK_TIME;
bool BmsEngine::Flag::OCD2 = 0;
bool BmsEngine::Flag::ocd2_deboc = 0;
uint16_t BmsEngine::Time::OCD2_time_delay = OCD2_CHECK_TIME;

bool BmsEngine::Flag::OCC3 = 0;
bool BmsEngine::Flag::occ3_deboc = 0;
uint8_t BmsEngine::Time::OCC3_time_delay = OCC3_CHECK_TIME;
bool BmsEngine::Flag::OCD3 = 0;
bool BmsEngine::Flag::ocd3_deboc = 0;
uint8_t BmsEngine::Time::OCD3_time_delay = OCD3_CHECK_TIME;

int16_t BmsEngine::MaxSort_Temp = 0;
int16_t BmsEngine::MinSort_Temp = 0;


bool BmsEngine::Flag::hvil1_deboc = 0;
bool BmsEngine::Flag::HVIL1_Error = 0;

uint16_t BmsEngine::Time::hvil1_time_delay = HVIL1_CHECK_TIME;

bool BmsEngine::Flag::hvil2_deboc = 0;
bool BmsEngine::Flag::HVIL2_Error = 0;

uint16_t BmsEngine::Time::hvil2_time_delay = HVIL2_CHECK_TIME;


bool BmsEngine::Flag::IV1 = 0;
bool BmsEngine::Flag::iv1_deboc = 0;
uint8_t BmsEngine::Time::IV1_time_delay = IV1_CHECK_TIME;
	

bool BmsEngine::Flag::current_deboc = 0;
uint16_t BmsEngine::Time::current_time_delay = CSCF_CHECK_TIME;
bool BmsEngine::Flag::CCF = 0;
bool BatterySys::CurrCountFlag = 0;

bool BmsEngine::Flag::ems_deboc = 0;
uint16_t BmsEngine::Time::ems_time_delay = ESCF_CHECK_TIME;
bool BmsEngine::Flag::ECF = 0;
bool BatterySys::EmsCountFlag = 0;

bool BmsEngine::Flag ::CHG_E = 0;
bool BmsEngine::Flag::chg_e_deboc = 0;
uint8_t BmsEngine::Time::CHG_E_time_delay = CHG_E_CHECK_TIME;

bool BmsEngine::Flag::DSG_E = 0;
bool BmsEngine::Flag::dsg_e_deboc = 0;
uint8_t BmsEngine::Time::DSG_E_time_delay = DSG_E_CHECK_TIME;

bool BmsEngine::Flag::FullCHG = 0;
bool BmsEngine::Flag::fullchg_deboc = 0;
uint8_t BmsEngine::Time::FullCHG_time_delay = FULLCHG_CHECK_TIME;

bool BmsEngine::Flag::FullDSG = 0;
bool BmsEngine::Flag::fulldsg_deboc = 0;
uint8_t BmsEngine::Time::FullDSG_time_delay = FULLDSG_CHECK_TIME;

bool BmsEngine::BMUCANFAULTFlag = 0;

bool BmsEngine::Flag::ADCTempFault = 0;
bool BmsEngine::Flag::ADCVoltFault = 0;
bool BmsEngine::Flag::ADCFault = 0;
bool BmsEngine::Flag::adcfault_deboc = 0;
uint8_t BmsEngine::Time::ADCFault_time_delay = ADCFAULT_CHECK_TIME;	
uint16_t BmsEngine::Cumulative_times = 0;

bool BmsEngine::Flag::ADCTempWarning = 0;
bool BmsEngine::Flag::ADCWarning = 0;
bool BmsEngine::Flag::adcWarning_deboc = 0;
uint8_t BmsEngine::Time::ADCWarning_time_delay = ADCWARNING_CHECK_TIME;

bool BmsEngine::Flag::COAT1_PackAmbient = 0;
bool BmsEngine::Flag::COAT1_PackAmbient_Debounce = 0;
uint16_t BmsEngine::Time::COAT1_PackAmbient_Time_Delay = COAT1_PACKAMBIENT_CHECK_TIME;
bool BmsEngine::Flag::COAT3_PackAmbient = 0;
bool BmsEngine::Flag::COAT3_PackAmbient_Debounce = 0;
uint16_t BmsEngine::Time::COAT3_PackAmbient_Time_Delay = COAT3_PACKAMBIENT_CHECK_TIME;

bool BmsEngine::Flag::CUAT1_PackAmbient = 0;
bool BmsEngine::Flag::CUAT1_PackAmbient_Debounce = 0;
uint16_t BmsEngine::Time::CUAT1_PackAmbient_Time_Delay = CUAT1_PACKAMBIENT_CHECK_TIME;
bool BmsEngine::Flag::CUAT3_PackAmbient = 0;
bool BmsEngine::Flag::CUAT3_PackAmbient_Debounce = 0;
uint16_t BmsEngine::Time::CUAT3_PackAmbient_Time_Delay = CUAT3_PACKAMBIENT_CHECK_TIME;

bool BmsEngine::Flag::DOAT1_PackAmbient = 0;
bool BmsEngine::Flag::DOAT1_PackAmbient_Debounce = 0;
uint16_t BmsEngine::Time::DOAT1_PackAmbient_Time_Delay = DOAT1_PACKAMBIENT_CHECK_TIME;
bool BmsEngine::Flag::DOAT3_PackAmbient = 0;
bool BmsEngine::Flag::DOAT3_PackAmbient_Debounce = 0;
uint16_t BmsEngine::Time::DOAT3_PackAmbient_Time_Delay = DOAT3_PACKAMBIENT_CHECK_TIME;

bool BmsEngine::Flag::DUAT1_PackAmbient = 0;
bool BmsEngine::Flag::DUAT1_PackAmbient_Debounce = 0;
uint16_t BmsEngine::Time::DUAT1_PackAmbient_Time_Delay = DUAT1_PACKAMBIENT_CHECK_TIME;
bool BmsEngine::Flag::DUAT3_PackAmbient = 0;
bool BmsEngine::Flag::DUAT3_PackAmbient_Debounce = 0;
uint16_t BmsEngine::Time::DUAT3_PackAmbient_Time_Delay = DUAT3_PACKAMBIENT_CHECK_TIME;

uint32_t BmsEngine::FaultCount = 0U;
uint32_t BmsEngine::NFB_FAULT_Count = 0U;
uint16_t BmsEngine::Main_FAULT_Count = 0U;
uint16_t BmsEngine::Neg_FAULT_Count = 0U;
uint16_t BmsEngine::Precharge_FAULT_Count = 0U;
uint16_t BmsEngine::FAN_Warning_Count = 0U;


bool BmsEngine::Flag::ESS_BreakerFault = false;
bool BmsEngine::Flag::ESS_PreChargeRelayFault = false;
bool BmsEngine::Flag::ESS_HV_MainRelayFault = false;
bool BmsEngine::Flag::ESS_HV_NegRelayFault = false;
bool BmsEngine::Flag::ESS_FAN_Warning = false;

//bool BmsEngine::flag::PCRS_Error = false;
bool BmsEngine::Flag::pcrs_deboc = 0;
uint8_t BmsEngine::Time::pcrs_time_delay = PCRS_CHECK_TIME;

bool BmsEngine::Flag::DCI = 0;
bool BmsEngine::Flag::dci_deboc = 0;
uint8_t BmsEngine::Time::DCI_time_delay = DCI_CHECK_TIME;
bool BmsEngine::Flag::CCI = 0;
bool BmsEngine::Flag::cci_deboc = 0;
uint8_t BmsEngine::Time::CCI_time_delay = CCI_CHECK_TIME;
//uint32_t SOC::Timeout6hr_Counter = TIME_6HR;

//bool BmsEngine::flag::OT1_BPU = 0;
//bool BmsEngine::flag::ot1_bpu_deboc = 0;
//uint16_t BmsEngine::time::OT1_BPU_time_delay = BPU_Check_Time;

bool BmsEngine::Flag::OT1_PCS_Positive = 0;
bool BmsEngine::Flag::ot1_PCS_Positive_deboc = 0;
uint16_t BmsEngine::Time::OT1_PCS_Positive_time_delay = OT1_PCS_POSITIVE_CHECK_TIME;
bool BmsEngine::Flag::OT2_PCS_Positive = 0;
bool BmsEngine::Flag::ot2_PCS_Positive_deboc = 0;
uint16_t BmsEngine::Time::OT2_PCS_Positive_time_delay = OT2_PCS_POSITIVE_CHECK_TIME;

bool BmsEngine::Flag::OT1_PCS_Negative = 0;
bool BmsEngine::Flag::ot1_PCS_Negative_deboc = 0;
uint16_t BmsEngine::Time::OT1_PCS_Negative_time_delay = OT1_PCS_NEGATIVE_CHECK_TIME;
bool BmsEngine::Flag::OT2_PCS_Negative = 0;
bool BmsEngine::Flag::ot2_PCS_Negative_deboc = 0;
uint16_t BmsEngine::Time::OT2_PCS_Negative_time_delay = OT2_PCS_NEGATIVE_CHECK_TIME;


bool BmsEngine::Flag::OT1_HVDC_Positive = 0;
bool BmsEngine::Flag::ot1_HVDC_Positive_deboc = 0;
uint16_t BmsEngine::Time::OT1_HVDC_Positive_time_delay = OT1_HVDC_POSITIVE_CHECK_TIME;
bool BmsEngine::Flag::OT2_HVDC_Positive = 0;
bool BmsEngine::Flag::ot2_HVDC_Positive_deboc = 0;
uint16_t BmsEngine::Time::OT2_HVDC_Positive_time_delay = OT2_HVDC_POSITIVE_CHECK_TIME;

bool BmsEngine::Flag::OT1_HVDC_Negative = 0;
bool BmsEngine::Flag::ot1_HVDC_Negative_deboc = 0;
uint16_t BmsEngine::Time::OT1_HVDC_Negative_time_delay = OT1_HVDC_NEGATIVE_CHECK_TIME;
bool BmsEngine::Flag::OT2_HVDC_Negative = 0;
bool BmsEngine::Flag::ot2_HVDC_Negative_deboc = 0;
uint16_t BmsEngine::Time::OT2_HVDC_Negative_time_delay = OT2_HVDC_NEGATIVE_CHECK_TIME;

bool BmsEngine::Flag::OT1_Fuse = 0;
bool BmsEngine::Flag::ot1_Fuse_deboc = 0;
uint16_t BmsEngine::Time::OT1_Fuse_time_delay = OT1_FUSE_CHECK_TIME;
bool BmsEngine::Flag::OT2_Fuse = 0;
bool BmsEngine::Flag::ot2_Fuse_deboc = 0;
uint16_t BmsEngine::Time::OT2_Fuse_time_delay = OT2_FUSE_CHECK_TIME;

bool BmsEngine::Flag::OT1_TA = 0;
bool BmsEngine::Flag::ot1_TA_deboc = 0;
uint16_t BmsEngine::Time::OT1_TA_time_delay = OT1_TA_CHECK_TIME;

int8_t LTC6811SPI::error_message = 0;

bool BmsEngine::Flag::SOC_CAL5 = 0;
bool BmsEngine::Flag::SOC_CAL5_deboc = 0;
uint16_t BmsEngine::Time::SOC_CAL5_time_delay = SOC5_CHECK_TIME;
bool BmsEngine::Flag::SOC5_Calibration_START = 0;

bool BmsEngine::Flag::SOC_CAL95 = 0;
bool BmsEngine::Flag::SOC_CAL95_deboc = 0;
uint16_t BmsEngine::Time::SOC_CAL95_time_delay = SOC95_CHECK_TIME;
bool BmsEngine::Flag::SOC95_Calibration_START = 0;
	

//int packIndex = 1;
//uint16_t bankIndex = 1;

//bool BmsEngine::BCUCANMessageProtectionComm::I2C_Communication = 0;
//bool BmsEngine::BCUCANMessageProtectionComm::Insulation_Communication = 0;
//bool BmsEngine::BCUCANMessageProtectionComm::Shunt_Communication = 0;	
//bool BmsEngine::BCUCANMessageProtectionComm::BMU_Communication = 0;		

bool BmsEngine::SBCUCANMessageCapacity::Initialed = 0;
bool BmsEngine::SBCUCANMessageCapacity::BMUinitialed = 0;
bool BmsEngine::SBCUCANMessageCapacity::ShuntInitialed = 0;	
bool BmsEngine::SBCUCANMessageCapacity::InsulationInitialed = 0;		
bool BmsEngine::SBCUCANMessageCapacity::ChargeEnable = 0;				
bool BmsEngine::SBCUCANMessageCapacity::DischargeEnable = 0;	
bool BmsEngine::SBCUCANMessageCapacity::FullCharge = 0;				
bool BmsEngine::SBCUCANMessageCapacity::FullDischarge = 0;				

//int32_t FRAMI2C::FRAM_DATA = 0;
bool FRAMI2C::changeID_flag = 0;
uint8_t FRAMI2C::newBcuId = 0U;

bool_t BatterySys::BMU_FAN_ON = 0;
uint8_t BatterySys::SendBMUData = 0U;
uint8_t BatterySys::SendBMUErrCNT = 0U;
bool BatterySys::SendBMUDataFlag = 0;
bool BatterySys::SendBMUDataOneFlag = 1;
bool BatterySys::SendBMUErrCNTFlag = 0;
bool BatterySys::SendBMUErrCNTOneFlag = 1;
bool BatterySys::SendBMUErrCNTCommandFlag = 0;


int32_t BmsEngine::ampReading_mA = 0;
uint16_t BmsEngine::socCounter = 0;

bool_t Balance::PassiveBalancejudge_flag = 0;
bool_t Balance::PassiveBalanceCommand_ON_flag = 0;

bool_t Balance::BCU_SET_Manual_Balance_flag = 0;
bool_t Balance::BCU_SET_Semi_Automatic_Balance_flag = 0;

bool_t SOC::FullCHG_END_Flag = 0;
bool_t SOC::FullDSG_END_Flag = 0;


//Thread bcuThread;
Thread emsThread;		//EmsThread
Thread IsoSPIThread;	//IsoSPIThread
Thread UL_ClassBThread;	//ClassBThread

Timer timey;	//A general purpose timer

Ticker time_up;		//call a function at a recurring interval
Ticker time_counter;	//call a function at a recurring interval
Ticker time_ems;	//call a function at a recurring interval
Ticker time10ms_counter;	//call a function at a recurring interval

volatile int16_t T100ms = 0;	//time variable
volatile int T500ms = 0;	//time variable
volatile int T1000ms = 0;	//time variable
volatile int T6811ms = 0;	//time variable

int BmsEngine::status_count = 0;

volatile int E50ms = 0;	    //time variable
//volatile int E100ms = 0;	//time variable
//volatile int E500ms = 0;	//time variable
volatile int E1000ms = 0;	//time variable
bool_t E50ms_Flag = 0;		//time flag
//bool_t E100ms_Flag = 0;		//time flag
bool_t E500ms_Flag = 0;		//time flag
bool_t E1000ms_Flag = 0;	//time flag

//bool_t T1000ms_Flag = 0;
bool_t ADCC::ADC_T1000ms_Flag = 0;

bool_t ULClassB_Flag = 0;	//time flag

int8_t LTC6811SPI::error = 0;
uint32_t LTC6811SPI::conv_time = 0;
uint8_t LTC6811SPI::sw = 0;
uint8_t LTC6811SPI::heartbeatSwitch = 0;

uint16_t ADCC::result_TH[AVERAGE_NUMBER] = { 0 };

int16_t ADCC::Temp_TH = 0;
uint16_t ADCC::Max_Temp_sort = 0;
uint16_t ADCC::Min_Temp_sort = 0;

int16_t ADCC::Tempchannels[8] = { 0 };
int16_t ADCC::TempTxCH[8] = { 0 };
int16_t ADCC::Temp_previous[8] = { 0 };
bool_t ADCC::Temp_abnormal[8] = { 0 };
uint16_t ADCC::TempCount[8] = { 0 };

uint32_t ADCC::result_Temp_SUM = 0;


uint8_t LTC6811SPI::cell_data[384] = { 0 }; //LTC681x_rdcv
uint8_t LTC6811SPI::read_buffer[384] = { 0 };	//LTC681x_rdcomm
uint8_t LTC6811SPI::data[384] = { 0 };	//read_6811
uint8_t LTC6811SPI::cmd[384] = { 0 };	//write_6811
uint8_t LTC6811SPI::write_buffer[384] = { 0 };	//LTC681x_wrcfg

bool_t LTC6811SPI::Overlap_Test_Flag = 0;

Watchdog wdog;	//Watchdog 3S
//Balance balance;	//Balance
//BmsEngine bmsengine;	//BmsEngine


uint8_t packNumIndex = PackConst::MIN_PACK_NUM;		//init

//uint8_t ErrNumIndex = PackConst::MIN_PACK_NUM;

//Thread thread;

DigitalOut Green_LED(LED1);		//A digital output
DigitalOut Blue_LED(LED2);		//A digital output
DigitalOut Red_LED(LED3);		//A digital output

DigitalOut LED_Status1(DOUT1);	//A digital output

DigitalOut can1_en(CAN1_EN);	//A digital output
DigitalOut can2_en(CAN2_EN);	//A digital output

AnalogIn temperature1(ADC_NTC1);	//An analog input
AnalogIn temperature2(ADC_NTC2);	//An analog input
AnalogIn temperature3(ADC_NTC3);	//An analog input
AnalogIn temperature4(ADC_NTC4);	//An analog input
AnalogIn temperature5(ADC_NTC5);	//An analog input
AnalogIn temperature6(ADC_NTC6);	//An analog input
AnalogIn temperature7(ADC_NTC7);	//An analog input
AnalogIn temperature8(ADC_NTC8);	//An analog input

extern DigitalOut RLY_NFB;			//extern digital output
extern DigitalOut RLY_Precharge;	//extern digital output
extern DigitalOut RLY_HV_Main;		//extern digital output
extern DigitalOut RLY_HV_Neg;		//extern digital output
extern DigitalOut RLY_K5_FAN;		//extern digital output

uint8_t	LTC6811SPI::heartbeat_value = 0;
LTC6811SPI::Cell_asic bms_ic[LTC6811SPI::TOTAL_IC];		
uint16_t LTC6811SPI::Temp_CH[10] = { 0 };
int16_t LTC6811SPI::ADCTemp_CH[10] = { 0 };
uint16_t LTC6811SPI::Volt_CH[PackConst::MAX_CELL_NUM] = { 0 };
int16_t LTC6811SPI::PackTemp_CH[4] = { 0 };
int16_t LTC6811SPI::ADCPackTemp_CH[4] = { 0 };

uint16_t LTC6811SPI::Configuration_RegisterA = 0;
uint16_t LTC6811SPI::Configuration_RegisterB = 0;
bool_t LTC6811SPI::Input_GPIO9 = 0;

uint8_t LTC6811SPI::heartbeat_CH[4] = { 0 };

bool_t LTC6811SPI::BMSLTC6811VoltTemp_Flag = 0;

uint16_t LTC6811SPI::Volt_PEC_Err = 0;
uint16_t LTC6811SPI::communication_Volt_counter = 0;
bool_t LTC6811SPI::communication_Volt_counter_Flag = 0;
bool_t LTC6811SPI::communication_Temp_counter_Flag = 0;

uint16_t LTC6811SPI::Volt_error_counter = 0; 
bool_t LTC6811SPI::Volt_PEC_Err_Flag = 0;
uint16_t LTC6811SPI::com_PEC_Err = 0;

uint16_t LTC6811SPI::com_PEC_Err_Flag = 0;
uint16_t LTC6811SPI::com_error_counter = 0;

uint8_t FailSafePOR_Flag = 0;		//init

ADBMS6815::cell_asic IC[LTC6811SPI::TOTAL_IC];		

// relay state & OCP/OVP/UVP/OT/UT...
void BcuProcess()
{
	ptr_bms->BcuProcess(); // relay state & OCP/OVP/UVP/OT/UT...
}

//EmsThread
void EmsProcess()
{
	uint16_t packidNum = 0;		//init

	for (;;)
	{
		if (BatterySys::SendBMUDataFlag == 1)
		{	
			if (E50ms_Flag == 1)
			{
				BatterySys::Mbu_canReportVoltage(packNumIndex);
				wait_ms(2);
				BatterySys::Mbu_canReportTemperature(packNumIndex);
				BatterySys::BMU_canReportPackTemperature(packNumIndex);	
				wait_ms(2);
	
				packNumIndex++;
				if (packNumIndex > PackConst::MAX_PACK_NUM)
				{
					packNumIndex = PackConst::MIN_PACK_NUM;
					BatterySys::SendBMUDataFlag = 0;
					BatterySys::SendBMUDataOneFlag = 0;
				}			
				
				E50ms_Flag = 0;			
			}
		}
		
		if (BatterySys::CANSendBMUVer_Flag == 1)
		{	
			BatterySys::BMU_canReportVersion(packidNum);   	
			wait_ms(2);                                                                                                                                                                                            
			packidNum++;
			if (packidNum > PackConst::MAX_PACK_NUM)
			{
				packidNum = PackConst::MIN_PACK_NUM;
				BatterySys::CANSendBMUVer_Flag = 0;
			}			
		}
		if (E500ms_Flag == 1)
		{
			BatterySys::BCU_canReportBcuStatus();
			wait_ms(2);
			E500ms_Flag = 0;	
		}		
		if (E1000ms_Flag == 1)
		{
			BatterySys::CanReportPECerror();
			wait_ms(2);
			BatterySys::CanReportBCU_Temperature_1();
			wait_ms(2);
			BatterySys::CanReportBCU_Temperature_2();		
			wait_ms(2);
			E1000ms_Flag = 0;	
		}		
		wait_ms(2);
	}
}


//ClassBThread
void SysTickClassB_Process()
{
//	for (; ;)
	while (1)
	{		
		if (ULClassB_Flag == 1)
		{
			SysTickClassB_Handler();
			ULClassB_Flag = 0;
		}
		wait_ms(15);		
	}
}

//Time_base1ms
void Time_base1ms()
{
	T100ms += 1;
	T500ms += 1;
	T1000ms += 1;
	T6811ms += 1;
	
	BmsEngine::CheckHVILCountdown(); 	//HVIL Time Count down
	
	E50ms += 1;


	if (E50ms >= 5)		//38*2=76ms 
	{
		E50ms = 0;		
		E50ms_Flag = 1;
	}


	if (T100ms >= 102)
	{			
		BmsEngine::LED_Status(); 	//LED_Status
		T100ms = 0;

		ULClassB_Flag = 1;	//TB_Flag
	}
	
	if (T6811ms >= 1000)	//ADBMS6815 isoSPI
	{
		LTC6811SPI::BMSLTC6811VoltTemp_Flag = 1;	//isoSPI

		T6811ms = 0;
	}
	
	if (T500ms >= 500)	//
	{
		T500ms = 0;
		Blue_LED = !Blue_LED;
		E500ms_Flag = 1;
		BmsEngine::CheckT500msCountdown();	 //Time 500ms Count down
	}

	if (T1000ms >= 1002)
	{
		T1000ms	= 0;
		E1000ms_Flag = 1;	
		ADCC::ADC_T1000ms_Flag = 1;
		BmsEngine::CheckCurrCanCounter();	//Current CAN Fault
		BmsEngine::CheckEmsCanCounter();	//EMS CAN Fault		
	}


}

//SocProcess Time 10ms
void SocProcess()		 //SOC Calc
{
	if (BmsEngine::m_bcuState == BCUStateType_t::BCU_INIT) //BCU_INT 
	{
		return;
	}
	
	BmsEngine::StateOfCharge(BmsEngine::ampReading_mA); //SOC Calc
	
}

void IsoSPIProcess()
{
	while (1)
	{
		ADBMS6815::ADBMS6815Process(); //isoSPI
		wait_ms(15);		
	}
}


//main Thread
int main()
{
	Green_LED = 1;
	Blue_LED = 1;
	Red_LED = 1;
	can1_en = 0;
	can2_en = 0;
	RLY_NFB = NFB_OFF;
	RLY_HV_Main = RLY_OFF;
	RLY_HV_Neg = RLY_OFF;
	RLY_Precharge = RLY_OFF;
	RLY_K5_FAN = RLY_OFF; 

	/* Configure the MPU attributes as Write Through */
	MPU_Config();
	
	/* Enable the CPU Cache */
	CPU_CACHE_Enable(); 
	
	/* STM32F7xx HAL library initialization:
	     - Configure the Flash ART accelerator on ITCM interface
	     - Configure the Systick to generate an interrupt each 1 msec
	     - Set NVIC Group Priority to 4
	     - Low Level Initialization*/
	HAL_Init();

	/* Configure the system clock to 216 MHz */
	My_SystemClock_Config();

	/* -------------------------------------------------------------------------*/
	/* This is where the main self-test routines are initialized */
	STL_InitRunTimeChecks();

	HAL_RCC_EnableCSS(); //Enables the Clock Security System
	/* -------------------------------------------------------------------------*/
	//=================================================================

	wdog.Configure(3.0);

	//Hardware Init:
	//=================================================================

	pBatt->Init();

	frami2c.FRAMI2C_init(); //EEPROM init
	frami2c.FRAMI2C_ID_Check(); //EEPROM
	FRAMI2C::FRAMI2C_SOH_READ();	//READ SOH
	
	LTC6811SPI::SPI_init();
	wait_ms(10);
	LTC6811SPI::Wakeup_SLEEP(LTC6811SPI::TOTAL_IC);
	LTC6811SPI::Wakeup_IDLE(LTC6811SPI::TOTAL_IC);	
	
	ADBMS6815::adBms6815_init_configA_B(ADBMS6815::TOTAL_IC, &IC[0]);
	ADBMS6815::adBms6815_reset_cmd_count(ADBMS6815::TOTAL_IC);
	ADBMS6815::adBms6815_reset_pec_error_flag(ADBMS6815::TOTAL_IC, &IC[0]);
	//55ms
	ADBMS6815::adBms6815_set_reset_gpio_pins(ADBMS6815::TOTAL_IC, &IC[0]);
	ADBMS6815::adBms6815_read_device_sid(ADBMS6815::TOTAL_IC, &IC[0]);
	
	//Insert self checking here:
	//=================================================================
	DumpVersionInfo(); //300ms
        
	//Start Threads here:
	//=================================================================
	time_up.attach(&BcuProcess, 1.000);
	emsThread.start(callback(EmsProcess)); //EMS can message process
	time_counter.attach(&Time_base1ms, 0.001); //time count
	IsoSPIThread.start(callback(IsoSPIProcess)); //isoSPI
	time10ms_counter.attach(&SocProcess, 0.01); //SOC Calc 10ms
	UL_ClassBThread.start(callback(SysTickClassB_Process)); //UL ClassB
	                                                                                                                                                      
	timey.start();

	//main thread
	//=================================================================
			
	for (;;)
	{
		wdog.Service(); //clear watchdog
		/* -----------------------------------------------------------------------*/
		/* This is where the main self-test routines are executed */

#ifdef DORUNTIMECHECKS
		STL_DoRunTimeChecks(); //Class B 60730-1 and 60335-1 Functional Safety		  
#endif // DoRunTimeChecks

		/* -----------------------------------------------------------------------*/
			
		frami2c.FRAMI2C_ID_SET(); //EEPROM ID SET
		
		FRAMI2C::FRAMI2C_SOH_SET();
		
		BmsEngine::HVIL1_check();	//Emergency Check
		
		BmsEngine::HVIL2_Insulation_Check();	//Insulation_Check

		ADCC::Temperature_measurement();
		
		wait_ms(100);
		
	}

				
	return 0;

}


