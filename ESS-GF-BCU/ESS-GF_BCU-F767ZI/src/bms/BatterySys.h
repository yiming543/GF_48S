//mbed make this
#pragma once

#include <mbed.h>

#include "PackConst.h"
#include "BmsEngine.h"

#include "bcu.h"


namespace bmstech 
{
	//#define CM_CAN_BAUD_250K	(250000)
#define BMU_CAN_BAUD_250K	(250000)	//BMU=CM, battery minitor unit

	//#define MBU_CAN_BAUD_250K	(250000)
#define EMS_CAN_BAUD_250K	(250000)	//BPU=MBU, battery protection unit

	//=======================================================================
	//#define MBU_BIC_TEMP_ID_BASE	(0x7000)	//0x7XXY: XX=CM ID, Y=BCU ID
#define MBU_CELL_TEMP_ID_BASE	(0x8000030)	//0x8XXYY3I   : XX=BCU ID, YY=BMU ID, I=cellIdx
#define MBU_PACK_TEMP_ID_BASE	(0x800003F)	//0x8XXYY3F   : XX=BCU ID, YY=BMU ID, I=cellIdx
#define MBU_VOLTAGE_ID_BASE		(0x8000020)	//0x8XXYY2I  : XX=BCU ID, Y=BMU ID, I==cellIdx
#define MBU_PACK_STATUS_ID_BASE	(0x8000043)	//0x8XXYY4I   : XX=BCU ID, YY=BMU ID, I=Idx
#define BMU_VERSION_ID_BASE		(0x8000002)	//0x8XXYY02  : XX=BCU ID, Y=BMU ID, 
#define BMU_TVOC_1_ID_BASE		(0x8000080)	//0x8XXYY80  : XX=BCU ID, Y=BMU ID, 
#define BMU_TVOC_2_ID_BASE		(0x8000081)	//0x8XXYY81  : XX=BCU ID, Y=BMU ID, 
#define BMU_ACTIVE_BALANCE_ID_BASE		(0x8000050)	//0x8XXYY50  : XX=BCU ID, Y=BMU ID,
#define BMU_PASSIVE_BALANCE_ID_BASE		(0x8000051)	//0x8XXYY50  : XX=BCU ID, Y=BMU ID,
//#define MBU_CAN_ErrorCNT_ID_BASE	(0x8000060)	//0x8XXYY6I   : XX=BCU ID, YY=BMU ID, I=cellIdx
#define BMU_HEARTBEAT  (0x8000012)
#define BMU_ISOSPI_ERRORCNT_ID_BASE	(0x8000060)	//0x8XXYY6I   : XX=BCU ID, YY=BMU ID, I=cellIdx
#define BMU_HEARTBEAT_1_ID_BASE	(0x8000061)	//0x8XXYY61   : XX=BCU ID, YY=BMU ID, I=cellIdx
#define BMU_HEARTBEAT_2_ID_BASE	(0x8000062)	//0x8XXYY62   : XX=BCU ID, YY=BMU ID, I=cellIdx
#define BMU_HEARTBEAT_3_ID_BASE	(0x8000063)	//0x8XXYY63   : XX=BCU ID, YY=BMU ID, I=cellIdx
#define BMU_HEARTBEAT_4_ID_BASE	(0x8000064)	//0x8XXYY64   : XX=BCU ID, YY=BMU ID, I=cellIdx
#define BCU_TEMPERATURE_1_ID_BASE	(0x8000070)	//0x8XXYY7I   : XX=BCU ID, YY=BMU ID, I=cellIdx
#define BCU_TEMPERATURE_2_ID_BASE	(0x8000071)	//0x8XXYY7I   : XX=BCU ID, YY=BMU ID, I=cellIdx
	//#define MBU_BMS_ID_MASK	(0x000F)
	//#define MBU_CM_ID_MASK	(0x0FF0)

		//CM CAN BUS ID definition: (CAN1: Bet. BCU & CMs) (for all packs)
		//=======================================================================

#define CM_MAX_CELL_INDEX		(3)				//K:0~3
#define CM_MAX_TEMP_INDEX		(1)				//T:0,1
#define CM_BIC_TEMP_ID			(0)
#define CM_CELL_TEMP_ID			(1)
#define CM_CELL_VOLTAGE_INDEX	(4)				//message[4]:K:0~3
#define CM_CELL_TEMP_INDEX		(0)				//messgae[0]:T:0=>BIC, 1=>Battery
#define CM_CHKSUM_INDEX	(7)
#define CM_VOLT_PACKET_LENGTH (8)
#define CM_TEMP_PACKET_LENGTH (8)
			//BCU to MBU CAN BUS ID Definition (CAN2: Bet. BCU & MBU)
#define	USING_SAE_J1850_CRC8	//for BCU_MBU CAN BUS use SAE CRC8 instead

				//BCU STATUS Report ID: from BCU to MBU: 0x06xxY, Y:BCU ID
#define BCU_STATUS_PACKET_LENGTH (8)	//All DLC=8
#define BCU_STATUS_CHKSUM_INDEX (7)		//message[7]: CRC8 of message[0]~messgage[6]
#define BCU_STATUS_ID_BASE (0x06000)
	
#define BCUCAN_DATAMAIN_BASE (0x8000040)
#define BCUCAN_CELLVOLTAGE_BASE (0x8000041)
#define BCUCAN_CELLTEMPERATURE_BASE (0x8000042)
#define BCUCAN_PROTECTION_BASE (0x8000043)
#define BCUCAN_IO_BASE (0x8000044)
#define BCUCAN_CAPACITY_BASE (0x8000045)
#define ISO_STATE1_BASE (0x8000046)
#define ISO_STATE2_BASE (0x8000047)
#define SHT3_STATE_BASE (0x8000048)
#define TVOCSTATE_BASE (0x8000049)
	
#define BCU_INFO_HW_FW_VERSION_BASE (0x8000001)		//BCU Ver
#define BMU_RESET_BASE (0x8000010)		//BMU Reset
#define BMU_INFO_HW_FW_VERSION_BASE (0x8000001)		//BMU Ver
	
#define BCUPROTECTION_STATE_BASE (0x8000070)
#define CANREPORT_BMUERRCNT_BASE (0x8000104)
	
#define BCUCAN_ERR_EWG_CNT_BASE (0x8000060)
	
#define BCU_VOLT_AMP_ID_BASE (0x8000041)

#define BCU_DIODE_TEMP_ID_BASE1 (0x06020)
#define BCU_DIODE_TEMP_ID_BASE2 (0x06030)
#define BCU_CM_SURVIVAL_ID_BASE (0x06040)
#define BCU_STATE_ID_BASE (0x06050)
	
#define BCU_CAN_REPEAT_NUM (3)
#define BCU_CAN_DELAYTIME (600)

					//ID:0x600Y MSG[0]
#define BCU_REPORT_BCU_MODE_STANDBY (0)
#define BCU_REPORT_BCU_MODE_CHARGE (1)
#define BCU_REPORT_BCU_MODE_DRIVE (2)
#define BCU_REPORT_BCU_MODE_DRIVE_WITH_CHARGE (3)
#define BCU_REPORT_BCU_MODE_DRIVE_WITH_PRECHG (4)
#define BCU_REPORT_BCU_MODE_NONE (0xFF)

						//ID:0x600Y MSG[1]
#define BCU_REPORT_RELAY1_CM24V_MASK (0x01) //B0
#define BCU_REPORT_RELAY1_SD_MASK (0x02)	//B1

							//ID:0x600Y MSG[2]
#define BCU_REPORT_RELAY2_HVNEG_MASK (0x01)		//B0
#define BCU_REPORT_RELAY2_PRECHG_MASK (0x02)	//B1
#define BCU_REPORT_RELAY2_HVCHG_MASK (0x04)		//B2 (CHG DIODE RELAY)
#define BCU_REPORT_RELAY2_HVDSG_MASK (0x08)		//B3 (DSG DIODE RELAY)

								//ID:0x600Y MSG[3]
#define BCU_REPORT_FLAG_CM_CAN_MASK (0x01)		//B0
#define BCU_REPORT_FLAG_DO1_MASK (0x02)			//B1
#define BCU_REPORT_FLAG_DO2_MASK (0x04)			//B2
#define BCU_REPORT_FLAG_AI_MASK (0x08)			//B3
#define BCU_REPORT_FLAG_VMETER_MASK (0x10)		//B4
#define BCU_REPORT_FLAG_AMETER_MASK (0x20)		//B5
#define BCU_REPORT_FLAG_LEAK_MASK (0x40)		//B6

									//ID:0x600Y MSG[4]
#define BCU_REPORT_EVENT_OV_MASK (0x01)			//B0
#define BCU_REPORT_EVENT_UV_MASK (0x02)			//B1
#define BCU_REPORT_EVENT_OT_MASK (0x04)			//B2
#define BCU_REPORT_EVENT_UT_MASK (0x08)			//B3
#define BCU_REPORT_EVENT_DOCP_MASK (0x10)		//B4
#define BCU_REPORT_EVENT_COCP_MASK (0x20)		//B5
#define BCU_REPORT_EVENT_CMCAN_MASK (0x40)		//B6
#define BCU_REPORT_FLAG_DIODE_OT_MASK (0x80)	//B7	

										//ID:0x600Y MSG[5]
#define BCU_REPORT_EVENT_SBY_OC_MASK (0x01)			//B0
#define BCU_REPORT_EVENT_RS485_MASK (0x02)			//B1
#define BCU_REPORT_EVENT_DREVP_MASK (0x04)			//B2
#define BCU_REPORT_EVENT_CREVP_MASK (0x08)			//B3

//ID:0x8010043 MSG[0] MSG[1] MSG[2]	
#define BCU_REPORT_EVENT_OVP_MASK (0x01)			//B0
#define BCU_REPORT_EVENT_UVP_MASK (0x02)		//B1
#define BCU_REPORT_EVENT_OTCP_MASK (0x04)		//B2
#define BCU_REPORT_EVENT_OTDP_MASK (0x08)		//B3
#define BCU_REPORT_EVENT_UTCP_MASK (0x10)		//B4
#define BCU_REPORT_EVENT_UTDP_MASK (0x20) 	//B5
#define BCU_REPORT_EVENT_OCCP_MASK (0x40)		//B6
#define BCU_REPORT_EVENT_OCDP_MASK (0x80)		//B7

											//Set BCU ID command:
											//BCU COMMAND ID (From MBU to BCU for Setting BCU Mode)
											//NEW_ID: BYTE0 BYTE1 55 AA DA 9C 5C CRC8 (SAE)
#define BCU_SET_CMD_MAGIC_WORD_CHAR1	(0x00)
#define BCU_SET_CMD_MAGIC_WORD_CHAR2	(0x55)
#define BCU_SET_CMD_MAGIC_WORD_CHAR3	(0xAA)
#define BCU_SET_CMD_MAGIC_WORD_CHAR4	(0x53) //'S'
#define BCU_SET_CMD_MAGIC_WORD_CHAR5	(0x45) //'E'
#define BCU_SET_CMD_MAGIC_WORD_CHAR6	(0x54) //'T'
#define BCU_SET_CMD_MAGIC_WORD_CHAR7	(0x4F) //'O'
#define BCU_SET_CMD_MAGIC_WORD_CHAR8	(0x43) //'C'
	
#define BCU_SET_CMD_MAGIC_WORD_T	(0x54) //'T'
#define BCU_SET_CMD_MAGIC_WORD_I	(0x49) //'I'
#define BCU_SET_CMD_MAGIC_WORD_M	(0x4D) //'M'
#define BCU_SET_CMD_MAGIC_WORD_E	(0x45) //'E'

#define BCU_REQ_CMD_MAGIC_WORD_CHAR1	(0x00)
#define BCU_REQ_CMD_MAGIC_WORD_CHAR2	(0x55)
#define BCU_REQ_CMD_MAGIC_WORD_CHAR3	(0xAA)
#define BCU_REQ_CMD_MAGIC_WORD_CHAR4	(0x52) //'R'
#define BCU_REQ_CMD_MAGIC_WORD_CHAR5	(0x45) //'E'
#define BCU_REQ_CMD_MAGIC_WORD_CHAR6	(0x51) //'Q'

#define BCU_CMD_BCU_ID_INDEX (0)	//message[0] works as BCU ID Index 1~15
#define BCU_CMD_CHKSUM_INDEX (7)	//message[7]: CRC8 of message[0]~messgage[6]

#define BCU_SET_ID_MASK (0x0F0000FF)
#define BCU_SET_RESET_MASK (0x0F0000FF)
#define BMU_SET_RESET_MASK (0x0F0000FF)
#define BCU_SET_CLEAR_MASK (0x0F0000FF)
#define BCU_SET_VERSION_MASK (0x0F0000FF)
#define BCU_SET_SOC_MASK (0x0F0000FF)//(0x0F0000F0)
#define BCU_SET_SOH_MASK (0x0F0000FF)//(0x0F0000F0)
#define BCU_SET_SOC_TIME_MASK (0x0F0000FF)
#define BMU_SET_VERSION_MASK (0x0F0000FF)
#define BCU_SET_MANUAL_BALANCE_MASK (0x0F0000FF)
#define BCU_SET_SEMI_AUTO_BALANCE_MASK (0x0F0000FF)
	
//const uint32_t BCU_SET_ID_MASK = 0x0F0000FFU;
	
//#define BCU_CMD_ID_MASK (0x00000FFF)
//#define BCU_CMD_RESET_ID (0x02000010)
//#define BMU_CMD_RESET_ID (0x02000011)
//#define BCU_CMD_CLEARFLAG_ID (0x02000012)
const uint32_t BCU_CMD_RESET_ID = 0x02000010U;
const uint32_t BMU_CMD_RESET_ID = 0x02000011U;
const uint32_t BCU_CMD_CLEARFLAG_ID = 0x02000012U;	

//#define BCU_CMD_SHUTDOWN_ID (0x0B00) //obselete
//#define BCU_CMD_BCU_CONTROL_ID (0x0B00) //added 2017/11/23
//#define BCU_CMD_SET_BCUID_ID (0x0B01)
//#define BCU_CMD_SET_BCU_MODE_ID (0x0B02)
//#define BCU_CMD_SET_BCU_SOC_SOH_ID (0x0B03)
//#define BCU_CMD_CLEAR_BCU_EVENT_ID (0x0B04) //added 2017/11/23

												//Current Hall Sensor MV Parameters
//#define MV_NUMBER (16)
//#define MV_AVG	(4)
//#define MAX_CURRENT (500000)	//unit:1mA =>500A
//#define MIN_CURRENT (-500000)	//unit:1mA =>-500A
	
const uint16_t MV_NUMBER = 16U;
const uint16_t MV_AVG = 4U;
const int32_t MAX_CURRENT = 500000;
const int32_t MIN_CURRENT = -500000;
	
	
	
//#define Time7s (7)	//7s

	struct Pack_t 
	{
		uint16_t cellVolt[PackConst::MAX_CELL_NUM];                                                   //1mV
		uint8_t cellBalance[PackConst::MAX_BALANCE_NUM];                                    //
		
		uint16_t PassiveBalanceComm[PackConst::MAX_BALANCE_NUM];
			
		uint16_t packMaxVolt;
		uint16_t packMinVolt;
		uint8_t packMaxVolt_NUM;
		uint8_t packMinVolt_NUM;

		int16_t packMaxTemp;
		int16_t packMinTemp;
		uint8_t packMaxTemp_NUM;
		uint8_t packMinTemp_NUM;
			
		int16_t cellAVG_Temp;
//		int16_t packAVG_Temp;

		uint8_t channel;
		int16_t cellTemp[6];
//		int16_t cellTemp[18];
		int16_t packTemp[4];
		bool_t packADCFault;
		bool_t packADCWarning;

		uint8_t PackHeartbeat[4];

//		int8_t bicTemp[PacketConst::BIC_TEMP_NUM];                                                  //1.0C
		uint8_t timeoutCount[PacketConst::CELL_VOLT_NUM];
		bool btimeout[PacketConst::CELL_VOLT_NUM];
		bool bexist;
		uint16_t packVolt;
		
		//uint16_t voltPECErrCNT[PackConst::MAX_CELL_NUM];    
		
//		uint16_t tempVolt;
//		uint32_t cmInfo[PackConst::FUNC_ID_NUM];                                                   //CM info
		
//		uint32_t cellErrCNT[PackConst::MAX_CELL_NUM];         

		uint8_t ADCFault=0;

			
		uint16_t HWVer;
		uint16_t SWVer;

	};
	
	struct CANEchoPacket_t {
		bool echoFlag;
		uint32_t id;
		uint8_t dlc;
		char data[8];
	};

	struct CANReplyPacket_t {
		bool replyFlag;
		uint32_t id;
		uint8_t dlc;
		char data[8];
	};

	enum class bcucmd_errortype_t : int8_t {
		BCU_SET_CMD_OK = 0,
		BCU_REQ_CMD_OK = 0,
		BCU_INVALID_ID = -1,
		BCU_MAGIC_WORD_ERROR = -2,
		BCU_CRC_ERROR = -3,

		STORE_NEW_ID_ERROR = -4,
		//for 0xB02 DS1302 save new ID error
		INVALID_NEW_ID = -5,
		//for 0xB02 set BCI ID
		INVALID_BCU_MODE = -6,
		//0xB01 set BCU Mode

		ERROR_UNKNOWN = -128
	};

	typedef void(*Callback_function)(void);                                                   // type for conciseness

	class BatterySys
	{
	public:
		//BatterySys(CAN* device, int baudrate, BmsEngine* bms);
//		BatterySys(CAN* can1, const int bdCan1, CAN* can2, const int bdCan2, BmsEngine* pBms, DS1302* pRtc);
		BatterySys(CAN* can1, const int bdCan1, CAN* can2, const int bdCan2, BmsEngine* pBms);
		BatterySys(CAN* can1, const int bdCan1, CAN* can2, const int bdCan2);
		BatterySys(CAN* can1, const int bdCan1, Callback_function parser);
		BatterySys(CAN* can1, const int bdCan1);
		~BatterySys();

		void CanInit(CAN * can1, const int bdCan1);

		//void init(CAN &device, int baud, BmsEngine &bms);
		void Init() const;

		//static uint8_t funcIdMap(uint8_t funcId);

		static void Cmd_resetBCU();
		static void Cmd_resetBMU();
		static void Cmd_shutDownBcu();
		
//		static BcuCmdErrorType_t cmd_bcuActionControl();
		static bcucmd_errortype_t Cmd_setBcuId();
//		static BcuCmdErrorType_t Echo_setBcuMode(uint8_t buf[]);
		static bcucmd_errortype_t Cmd_setBcuMode();
//		static BcuCmdErrorType_t cmd_setBcuSocSoh();
//		static BcuCmdErrorType_t cmd_clearBcuEvent();
//		static BcuCmdErrorType_t cmd_setBalanceParameter();
//
//		static BcuCmdErrorType_t cmd_setBalanceStartStop();
//
//		static BcuCmdErrorType_t cmd_forceBalanChannelOn();
//
//		static BcuCmdErrorType_t cmd_requestBcuInfo();
//
//		static BcuCmdErrorType_t cmd_requestCmInfo();

//		static BcuCmdErrorType_t cmd_reportCmInfo();

//		static void autoBalanceCommandProcess();

		//static void balanceCommandProcess();
//		static void CanCommandActiveBalance(uint16_t max_volt, uint8_t max_num, uint16_t min_volt, uint8_t min_num);
		
		//static void CanCommandPassiveBalance(uint16_t max_volt, uint8_t max_num);
//		static void CanCommandPassiveBalance(uint16_t max_volt, uint8_t max_chnnel, uint8_t max_module, uint8_t timeout);
//		static void CanCommandPassiveMultipleBalance(uint16_t packID, uint8_t timeout);


//		static void reportCmInfo(char cmId, char funcId);
//		static void reportCmVersionInfo(char cmId);
//		static void reportCmCanErrorInfo(char cmId);


		//CAN2: BCU <-> MBU
		static void Mbu_canMessageParse();                                                  	//MBU command parser (ISR)
		static void Mbu_canReportVoltage(int packNum);
		static void Mbu_canReportTemperature(int packNum);
		static void BMU_canReportPackTemperature(int packNum);
		
		static void BMU_canReportVersion(int packidNum);
//		static void mbu_canReportTVOC_1(int packidNum);
//		static void mbu_canReportTVOC_2(int packidNum);
		
//		static void CanReportBcuStatus();
//		static void CanReportBcuProtection();
//		static void CanReportVoltAmp();
//		static void CanReportDischargeDiodeTemp();
//		static void CanReportChargeDiodeTemp();
//		static void CanReportCmSurvivalList();
//		static void CanReportBcuState();
		static void BCU_canReportBcuStatus();

		static void CanReportPECerror();
		
		static void CanReportHeartbeat();
		static void CanReportHeartbeat_1();
		static void CanReportHeartbeat_2();
		static void CanReportHeartbeat_3();
		
		static void BCUCANMessageDataMain();
		static void BCUCANMessageCellVoltage();
		static void BCUCANMessageCellTemperature();
		static void BCUCANMessageProtection();
		static void BCUCANMessageIO();
		static void BCUCANMessageCapacity();
		static void BMUCANMessageCAN_ERROR_CNT(int packNum);

		//static void CanCommandBalance(uint16_t max_volt, uint16_t min_volt);

		static void CanReportBmuERRCNT(uint16_t function_cmd);
		
		static void CanReportBCU_Temperature_1();		
		static void CanReportBCU_Temperature_2();
		static void Cmd_ClearFlagBCU();
		
//		static void SBCUCANMessageCAN_ERR_EWG_CNT();
		//CAN1: CMs <-> BCU
		//CM can methods:
		//static void cmCanParser();
		//static void cmCanParser(); // static function to be used as ISR callback

		static void BMU_CANMessageParse();

//		static void cmCanParser();
		//static void cmCanParser(CANMessage * pCanMsg);

		//static int cm_canCheckTimeout();
		
	

		static int BCU_CANStartAppCommand();

		static void BCU_canVersion_Information();
		
		static void BMU_Ver_CANCommand();
		


		// static class pointer: for static member vars access
		static BatterySys *pBattSys;

		//pack can messgae cell info storage (physical unit converted)
		static Pack_t pack[PackConst::MAX_PACK_NUM];

		static uint16_t MaxVolt;
		static uint16_t MaxVoltCH;
		static uint16_t MaxVoltModule;
		static uint16_t MinVolt;
		static uint16_t MinVoltCH;
		static uint16_t MinVoltModule;
		
		static int16_t MaxTemp;
		static uint16_t MaxTempCH;
		static uint16_t MaxTempModule;
		static int16_t MinTemp;
		static uint16_t MinTempCH;
		static uint16_t MinTempModule;
		
		static int16_t AvgCellTemperature;
		
		static int16_t PackAmbient_MaxTemp;
		static int16_t PackAmbient_MinTemp;
		
		//static uint16_t CellBalance;
		//static uint8_t Status_balance1;

		static int32_t totalCurrent;
		static int32_t current[MV_NUMBER];
		static int32_t rawCurrent;

		static uint32_t canCmCounter;
		bool m_canCmFlag{ false }
		;
		CanError_t canCmError = { 0 };

		static uint32_t canMbuCounter;
		bool m_canMbuFlag{ false }
		;
		CanError_t canMbuError = { 0 };

		static uint32_t canCurrCounter;
		static uint32_t canCurrCounter2nd;
		
		static bool CurrCountFlag;
		
		static uint32_t canEmsCounter;
		static uint32_t canEmsCounter2nd;	
		static bool EmsCountFlag;
		
		bool m_canCurrFlag{ false }
		;
		//static canError_t canCurrError;
		CanError_t canCurrError = { 0 };

		CANEchoPacket_t canEchoPacket;
		CANReplyPacket_t canReplyPacket;
		CANReplyPacket_t canCmInfoPacket;

		static uint8_t Get_SAE_CRC8(uint8_t *ptr, uint8_t length);
		static uint8_t Get_CRC8_Poly_0X07(uint8_t* ptr, uint8_t length);
		static uint8_t Get_CRC8_Poly_0X07(uint8_t* ptr);

		static uint16_t m_voltReading;
		static int16_t m_ampReading;                                                   //0.1A
//		static int16_t m_leakReading;
		static uint32_t m_sysVolt;                                                   //totla sum of pack voltage, unit: 10mV

		static uint16_t m_totalVolt;

		//static uint16_t MaxSort_Volt;
		
		static bool SendBMUVerFlag;
		static bool_t CANSendBMUVer_Flag;
		
		static bool_t BMU_FAN_ON;
			
		static uint8_t SendBMUData;
		static uint8_t SendBMUErrCNT;
		static bool SendBMUDataFlag;
		static bool SendBMUDataOneFlag;
		static bool SendBMUErrCNTFlag;
		static bool SendBMUErrCNTOneFlag;
		static bool SendBMUErrCNTCommandFlag;

		
		CAN* p_canCm;                                                  	//CM CAN Device
		//CANMessage canCmRxMsg;
		int m_baudCm;

		//static CANMessage canCmRxMsg;
		CANMessage canCmRxMsg;

		//CAN* p_canMbu;	//MBU CAN Device
		//int m_baudMbu;
		//CANMessage canMbuRxMsg;

	private :
//		DS1302* p_rtc;

		//CAN* p_canCm;	//CM CAN Device
		//int m_baudCm;
		//CANMessage canCmRxMsg;

		CAN* p_canMbu;                                                  	//MBU CAN Device
		int m_baudMbu;
		CANMessage canMbuRxMsg;
		//for BMS memory access (RS485 memory defined in BmsEngine class)
		BmsEngine* p_bmsParam;

		static uint8_t bcuSetCount;
	};
} //namespace 
