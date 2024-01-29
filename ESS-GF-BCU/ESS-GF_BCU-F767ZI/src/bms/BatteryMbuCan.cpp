//File
// <summary>
// for MBU CAN COmmand & report message
#include <mbed.h>
#include "BatterySys.h"
#include "BmsEngine.h"

#include "Balance.h"
#include "CY15B256J_I2C.h"

#include "main.h"
#include "SOC.h"
#include "LTC6811_daisy.h"
#include "ADC_Temperature.h"

extern DigitalOut RLY_NFB;			//extern
extern DigitalOut RLY_Precharge;	//extern
extern DigitalOut RLY_HV_Main;		//extern
extern DigitalOut RLY_HV_Neg;		//extern
extern DigitalOut RLY_K5_FAN;		//extern
	

extern DigitalIn RLY_FB_Precharge;	//extern
extern DigitalIn RLY_FB_HV_Main;	//extern
extern DigitalIn RLY_FB_HV_Neg;		//extern
extern DigitalIn RLY_FB_NFB;		//extern
extern DigitalIn RLY_FB_FAN;		//extern

extern DigitalOut LED_Status1;		//extern
//extern DigitalOut LED_Status1;
//extern DigitalOut LED_Status1;
//extern DigitalOut LED_Status1;

extern DigitalIn HV1_Emergency; //Emergency
extern DigitalIn HV2_Insulation; //HV2_Insulation

namespace bmstech
{
	//CRC8校驗
	//ptr:要校驗的陣列
	//len:陣列長度
	//返回值:CRC8碼
	//CRC8/ITU:		多項式 0X07, LSB First 初始值0X00 (x8+x2+x+1)  [0000 0111]
	//CRC8/MAXIM:	多項式 0X31, LSB First 初始值0X00 (x8+x5+x4+1) [0011 0001] => LSB First: 1000 1100 (0x8C)
	//SAE-J1850(Polynomial)=0x1D 	InitialValue = 0xFF XorValue = 0xFF (x^8+x^4+x^3+x^2+1) [0001 1101] (0x1D)
	//LSB first: [1011 1000]: 0xB8
	// https://github.com/zhaojuntao/CRC8-16/blob/master/CRC8_16/crc8_16.c
	// On line Calculator: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
	// Reference: http://www.ip33.com/crc.html
	FRAMI2C frami2c;
	//get_Maxim_CRC8
	uint8_t Get_Maxim_CRC8(uint8_t* ptr, uint16_t len)
	{
		uint8_t crc = 0;	//init
		//uint8_t i;		//init

		while (len--)
		{
			crc ^= *ptr++;
			//CRC8
			for (uint8_t i = 0U; i < 8; i++)
			{
				if (crc & 0x01)crc = (crc >> 1) ^ 0x8C;
				else crc >>= 1;
			}
		}
		return crc;
	}

	//================================================================
	//SAE J1850 CRC8:
	//On line Calculator:
	//http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
	//================================================================
	uint8_t CRC_1byte(const uint8_t data)
	{
		const uint8_t POLYNOMIAL = 0x1D;	//init

		uint8_t crc = data;	//init
		//CRC8
		for (int i = 0; i < 8; i++)
		{
			if (crc & 0x80) //MSB first
			{
				crc <<= 1;
				crc ^= POLYNOMIAL;
			}
			else crc <<= 1;
		}

		return (crc);
	}

	//get_SAE_CRC8
	uint8_t BatterySys::Get_SAE_CRC8(uint8_t* ptr, uint8_t length)
	{
		const uint8_t XORVALUE = 0xFF;	//init
		const uint8_t INITVALUE = 0xFF;	//init
		//init value
		uint8_t crcTemp = INITVALUE;

		//CRC8
		for (int i = 0 ; i < length ; i++)
		{
			crcTemp = CRC_1byte(crcTemp ^ ptr[i]);
		}

		return (XORVALUE ^ crcTemp);
	}

	/*
	*Reference:
	*CRC计算代码
	根据算法描述计算CRC
	*/

	/* http://wuchenxu.com/2015/12/07/CRC8-calculate/
	*
	import os
	Polynomial = 0x1D
	InitialValue = 0xFF
	XorValue = 0xFF

	def crc_1byte(data) :
	crc_1byte = data

	for i in range(0, 8) :
	if (crc_1byte & 0x80) :
	crc_1byte <<= 1
	crc_1byte ^= Polynomial
	else :
	crc_1byte <<= 1

	crc_1byte &= 0xFF
	return crc_1byte

	def crc_byte(data) :
	crcTemp = InitialValue

	for byte in data :
	crcTemp = (crc_1byte(crcTemp^byte))

	return XorValue^crcTemp

	根据长度为256的CRC表计算CRC:

	def crc_byte(data):
	crcTemp = InitialValue
	for byte in data:
		crcTemp ^= byte
		crcTemp = crc8_table[crcTemp]
	return (XorValue^crcTemp)
	*/

	//================================================================
	//SAE J1850 CRC8:
	//On line Calculator:
	//http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
	//================================================================
	uint8_t CRC_1byte_0x07(const uint8_t data)
	{
		//Honeywell Polynomial: X8+X2+X+1 (00111: 0x07)
		const uint8_t POLYNOMIAL = 0x07;	//init

		uint8_t crc = data;	//init
		//CRC8
		for (int i = 0; i < 8; i++)
		{
			if (crc & 0x80) //MSB first
				{
					crc <<= 1;
					crc ^= POLYNOMIAL;
				}
			else crc <<= 1;
		}

		return (crc);
	}

	//=================================================================
	//CRC8 for Honeywell
	//Polynomial = 0x07 (X8+X2+X+1)
	//InitialValue = 0x00
	//XorValue = 0x00
	//=================================================================
	//Lookup Table :
	const uint8_t CRC8_TABLE_0X07[] ={
		0x00,
		0x07,
		0x0E,
		0x09,
		0x1C,
		0x1B,
		0x12,
		0x15,
		0x38,
		0x3F,
		0x36,
		0x31,
		0x24,
		0x23,
		0x2A,
		0x2D,
		0x70,
		0x77,
		0x7E,
		0x79,
		0x6C,
		0x6B,
		0x62,
		0x65,
		0x48,
		0x4F,
		0x46,
		0x41,
		0x54,
		0x53,
		0x5A,
		0x5D,
		0xE0,
		0xE7,
		0xEE,
		0xE9,
		0xFC,
		0xFB,
		0xF2,
		0xF5,
		0xD8,
		0xDF,
		0xD6,
		0xD1,
		0xC4,
		0xC3,
		0xCA,
		0xCD,
		0x90,
		0x97,
		0x9E,
		0x99,
		0x8C,
		0x8B,
		0x82,
		0x85,
		0xA8,
		0xAF,
		0xA6,
		0xA1,
		0xB4,
		0xB3,
		0xBA,
		0xBD,
		0xC7,
		0xC0,
		0xC9,
		0xCE,
		0xDB,
		0xDC,
		0xD5,
		0xD2,
		0xFF,
		0xF8,
		0xF1,
		0xF6,
		0xE3,
		0xE4,
		0xED,
		0xEA,
		0xB7,
		0xB0,
		0xB9,
		0xBE,
		0xAB,
		0xAC,
		0xA5,
		0xA2,
		0x8F,
		0x88,
		0x81,
		0x86,
		0x93,
		0x94,
		0x9D,
		0x9A,
		0x27,
		0x20,
		0x29,
		0x2E,
		0x3B,
		0x3C,
		0x35,
		0x32,
		0x1F,
		0x18,
		0x11,
		0x16,
		0x03,
		0x04,
		0x0D,
		0x0A,
		0x57,
		0x50,
		0x59,
		0x5E,
		0x4B,
		0x4C,
		0x45,
		0x42,
		0x6F,
		0x68,
		0x61,
		0x66,
		0x73,
		0x74,
		0x7D,
		0x7A,
		0x89,
		0x8E,
		0x87,
		0x80,
		0x95,
		0x92,
		0x9B,
		0x9C,
		0xB1,
		0xB6,
		0xBF,
		0xB8,
		0xAD,
		0xAA,
		0xA3,
		0xA4,
		0xF9,
		0xFE,
		0xF7,
		0xF0,
		0xE5,
		0xE2,
		0xEB,
		0xEC,
		0xC1,
		0xC6,
		0xCF,
		0xC8,
		0xDD,
		0xDA,
		0xD3,
		0xD4,
		0x69,
		0x6E,
		0x67,
		0x60,
		0x75,
		0x72,
		0x7B,
		0x7C,
		0x51,
		0x56,
		0x5F,
		0x58,
		0x4D,
		0x4A,
		0x43,
		0x44,
		0x19,
		0x1E,
		0x17,
		0x10,
		0x05,
		0x02,
		0x0B,
		0x0C,
		0x21,
		0x26,
		0x2F,
		0x28,
		0x3D,
		0x3A,
		0x33,
		0x34,
		0x4E,
		0x49,
		0x40,
		0x47,
		0x52,
		0x55,
		0x5C,
		0x5B,
		0x76,
		0x71,
		0x78,
		0x7F,
		0x6A,
		0x6D,
		0x64,
		0x63,
		0x3E,
		0x39,
		0x30,
		0x37,
		0x22,
		0x25,
		0x2C,
		0x2B,
		0x06,
		0x01,
		0x08,
		0x0F,
		0x1A,
		0x1D,
		0x14,
		0x13,
		0xAE,
		0xA9,
		0xA0,
		0xA7,
		0xB2,
		0xB5,
		0xBC,
		0xBB,
		0x96,
		0x91,
		0x98,
		0x9F,
		0x8A,
		0x8D,
		0x84,
		0x83,
		0xDE,
		0xD9,
		0xD0,
		0xD7,
		0xC2,
		0xC5,
		0xCC,
		0xCB,
		0xE6,
		0xE1,
		0xE8,
		0xEF,
		0xFA,
		0xFD,
		0xF4,
		0xF3
	};
	
	//get_CRC8_Poly_0X07
	uint8_t BatterySys::Get_CRC8_Poly_0X07(uint8_t* ptr, uint8_t length)
	{
		const uint8_t XORVALUE = 0x00;	//init
		const uint8_t INITVALUE = 0x00;	//init

		uint8_t crcTemp = INITVALUE;	//init value
		//length
		for (int i = 0 ; i < length ; i++)
		{
			crcTemp = CRC_1byte_0x07(crcTemp ^ ptr[i]);
		}

		return (XORVALUE ^ crcTemp);
	}

	//get_CRC8_Poly_0X07
	uint8_t BatterySys::Get_CRC8_Poly_0X07(uint8_t* ptr)
	{
		const uint8_t XORVALUE = 0x00;	//init
		const uint8_t INITVALUE = 0x00;	//init

		uint8_t crcTemp = INITVALUE;	//init  value

		//data length
		for (int i = 0 ; i < 7 ; i++) //fixed data length:7
		{
			crcTemp ^= ptr[i];
			crcTemp = CRC8_TABLE_0X07[crcTemp];
		}

		return (XORVALUE ^ crcTemp);
	}

	//MBU command sets:
	//======================================================
	void BatterySys::Cmd_resetBCU()
	{
		if ((pBattSys->canMbuRxMsg.id & BCU_SET_RESET_MASK) == BCU_CMD_RESET_ID)
		{
			//BCU_ID is @ msg[0] now & New ID is @ msg[1]
			uint8_t bcuId = pBattSys->canMbuRxMsg.data[0];

			//check if BCU ID matched:
			if ((bcuId != BmsEngine::m_bcuId) && (bcuId != 0xFF)) return;

			//check valid BCU ID range:
			//if ((bcuId < PacketConst::MIN_BCU_ID) || (bcuId > PacketConst::MAX_BCU_ID)) return;
			//			if(bcuId > PacketConst::MAX_BCU_ID) return;

			//check set command data pattern:
			//NOW_ID NEW_ID 55 AA 'S' 'E' 'T' CRC8
			if (pBattSys->canMbuRxMsg.data[1] != BCU_SET_CMD_MAGIC_WORD_CHAR1) return;
			if (pBattSys->canMbuRxMsg.data[2] != 0x11) return;
			if (pBattSys->canMbuRxMsg.data[3] != 0x22) return;
			if (pBattSys->canMbuRxMsg.data[4] != BCU_SET_CMD_MAGIC_WORD_CHAR4) return;
			if (pBattSys->canMbuRxMsg.data[5] != BCU_SET_CMD_MAGIC_WORD_CHAR5) return;
			if (pBattSys->canMbuRxMsg.data[6] != BCU_SET_CMD_MAGIC_WORD_CHAR6) return;
			//CRC8 Get
			uint8_t crc8 = Get_SAE_CRC8(pBattSys->canMbuRxMsg.data, 7);
			//CRC8 check:
////			if (pBattSys->canMbuRxMsg.data[7] != crc8) return;

			//RESET MCU
			Thread::wait(50);
			NVIC_SystemReset();                  	// Mbed reset
		}
	}
	
	//cmd_resetBMU
	void BatterySys::Cmd_resetBMU()
	{		
		if ((pBattSys->canMbuRxMsg.id & BMU_SET_RESET_MASK) == BMU_CMD_RESET_ID)
		{
			char cmd[8] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //init
			//BCU_ID is @ msg[0] now & New ID is @ msg[1]
			//uint8_t bcuId = pBattSys->canMbuRxMsg.data[0];
			uint32_t id = (uint32_t)(BMU_RESET_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16));
			
			cmd[0] = 0xFF;
			cmd[1] = 0x00;
			cmd[2] = 0x00;
			cmd[3] = 0x00;
			cmd[4] = 0x00;
			cmd[5] = 0x00;
			cmd[6] = 0x00;
			
#if defined(USING_SAE_J1850_CRC8)
			cmd[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(cmd), BCU_STATUS_CHKSUM_INDEX);
#else
			cmd[BCU_STATUS_CHKSUM_INDEX] = 0;
			for (int i = 0 ; i<BCU_STATUS_CHKSUM_INDEX; i++)
			{
				cmd[BCU_STATUS_CHKSUM_INDEX] += cmd[i];
			}
#endif
		
			//RESET MCU
			int canCmsRet = pBattSys->p_canCm->write(CANMessage(id, cmd, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
			if (canCmsRet == 0)	
			{
				//BCU_CAN_REPEAT_NUM
				for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)
				//while(true)
				{
					wait_us(600);
					//REPEAT
					int canCmsRet = pBattSys->p_canCm->write(CANMessage(id, cmd, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
					if (canCmsRet == 1)
						break;
				}
			}
		}
	}

	//cmd_ClearFlagBCU
	void BatterySys::Cmd_ClearFlagBCU()
	{
		if ((pBattSys->canMbuRxMsg.id & BCU_SET_CLEAR_MASK) == BCU_CMD_CLEARFLAG_ID)
		{
			//BCU_ID is @ msg[0] now & New ID is @ msg[1]
			uint8_t bcuId = pBattSys->canMbuRxMsg.data[0];

			//check if BCU ID matched:
			if ((bcuId != BmsEngine::m_bcuId) && (bcuId != 0xFF)) return;

			//check valid BCU ID range:
			//if ((bcuId < PacketConst::MIN_BCU_ID) || (bcuId > PacketConst::MAX_BCU_ID)) return;
//			if(bcuId > PacketConst::MAX_BCU_ID) return;

			//check set command data pattern:
			//NOW_ID NEW_ID 55 AA 'S' 'E' 'T' CRC8
			if (pBattSys->canMbuRxMsg.data[1] != 0x55) return;
			if (pBattSys->canMbuRxMsg.data[2] != 0xAA) return;
			if (pBattSys->canMbuRxMsg.data[3] != 0x00) return;
			if (pBattSys->canMbuRxMsg.data[4] != BCU_SET_CMD_MAGIC_WORD_CHAR4) return;
			if (pBattSys->canMbuRxMsg.data[5] != BCU_SET_CMD_MAGIC_WORD_CHAR5) return;
			if (pBattSys->canMbuRxMsg.data[6] != BCU_SET_CMD_MAGIC_WORD_CHAR6) return;
			//CRC8 Get
			uint8_t crc8 = Get_SAE_CRC8(pBattSys->canMbuRxMsg.data, 7);
			//CRC8 check:
////			if (pBattSys->canMbuRxMsg.data[7] != crc8) return;

			//Clear Error Flag
			BmsEngine::Flag::ESS_BreakerFault = false;
			BmsEngine::Flag::ESS_PreChargeRelayFault = false;
			BmsEngine::Flag::ESS_HV_MainRelayFault = false;
			BmsEngine::Flag::ESS_HV_NegRelayFault = false;
//			FailSafePOR_Flag = 0;   //need Reset
			BmsEngine::NFBCount = 0;
			BmsEngine::count = 0;
			
			LTC6811SPI::Volt_PEC_Err = 0;
			LTC6811SPI::com_PEC_Err = 0;
			
			//Clear OV3 Flag
			BmsEngine::Flag::OV3 = 0;
			BmsEngine::m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OV3; 
			
			//Clear OV2 Flag
			BmsEngine::Flag::OV2 = 0;
			BmsEngine::m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OV2; 
			
			//Clear UV3 Flag
			BmsEngine::Flag::UV3 = 0;
			BmsEngine::m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_UV3; 
			
			//Clear UV2 Flag
			BmsEngine::Flag::UV2 = 0;
			BmsEngine::m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_UV2; 
			
			//Clear OTC3 Flag
			BmsEngine::Flag::OTC3 = 0;
			BmsEngine::m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OTC3;       
			
			//Clear OTC2 Flag
			BmsEngine::Flag::OTC2 = 0;
			BmsEngine::m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OTC2;   
			
			//Clear OTD3 Flag
			BmsEngine::Flag::OTD3 = 0;
			BmsEngine::m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OTD3;    
			
			//Clear OTD2 Flag
			BmsEngine::Flag::OTD2 = 0;
			BmsEngine::m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OTD2;  
			
			//Clear UTC3 Flag
			BmsEngine::Flag::UTC3 = 0;
			BmsEngine::m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_UTC3;  
			
			//Clear UTC2 Flag
			BmsEngine::Flag::UTC2 = 0;
			BmsEngine::m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_UTC2;  
			
			//Clear UTD3 Flag
			BmsEngine::Flag::UTD3 = 0;
			BmsEngine::m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_UTD3;     
			
			//Clear UTD2 Flag
			BmsEngine::Flag::UTD2 = 0;
			BmsEngine::m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_UTD2;     
			
			//Clear OCC3 Flag
			BmsEngine::Flag::OCC3 = 0;
			BmsEngine::m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OCC3;    
			
			//Clear OCC2 Flag
			BmsEngine::Flag::OCC2 = 0;
			BmsEngine::m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OCC2;   
			
			//Clear OCD3 Flag
			BmsEngine::Flag::OCD3 = 0;
			BmsEngine::m_bcuStatus3 &= ~(uint16_t)BCU_STATUS_OCD3;   
			
			//Clear OCD2 Flag
			BmsEngine::Flag::OCD2 = 0;
			BmsEngine::m_bcuStatus2 &= ~(uint16_t)BCU_STATUS_OCD2;   
			
			//Clear COAT3 CUAT3 DOAT3 DUAT3 Flag
			BmsEngine::Flag::COAT3_PackAmbient = 0;
			BmsEngine::Flag::CUAT3_PackAmbient = 0;
			BmsEngine::Flag::DOAT3_PackAmbient = 0;
			BmsEngine::Flag::DUAT3_PackAmbient = 0;
			
			//Clear ADCFault
			BmsEngine::Flag::ADCFault = 0;
			BmsEngine::m_bcuStatus5 &= ~(uint16_t)BMU_STATUS_ADCFAULT;
			BmsEngine::Cumulative_times = 0;
			
			//Clear Insulation Fault
			BmsEngine::Flag::HVIL2_Error = 0;
	
			
			BmsEngine::m_bcuModeChange = BCUModeChangeType_t::STANDBY;	
			BmsEngine::m_preState = BCUStateType_t::BCU_INIT;
			BmsEngine::m_bcuState = BCUStateType_t::BCU_STANDBY;
			BmsEngine::m_status = RELAYStatusType_t::STANDBY;
	

		}
	}
	
	//cmd_shutDownBcu
//	void BatterySys::Cmd_shutDownBcu()
//	{
//		if ((pBattSys->canMbuRxMsg.id & BCU_CMD_ID_MASK) == BCU_CMD_SHUTDOWN_ID)
//		{
//			//BCU_ID is @ msg[0] now & New ID is @ msg[1]
//			uint8_t bcuId = pBattSys->canMbuRxMsg.data[0];
//
//			//check if BCU ID matched:
//			if ((bcuId != BmsEngine::m_bcuId) && (bcuId != 0)) return;
//
//			//check valid BCU ID range:
//			//if ((bcuId < PacketConst::MIN_BCU_ID) || (bcuId > PacketConst::MAX_BCU_ID)) return;
//			if (bcuId > PacketConst::MAX_BCU_ID) return;
//
//			//check set command data pattern:
//			//NOW_ID NEW_ID 55 AA 'S' 'E' 'T' CRC8
//			if (pBattSys->canMbuRxMsg.data[1] != BCU_SET_CMD_MAGIC_WORD_CHAR1) return;
//			if (pBattSys->canMbuRxMsg.data[2] != BCU_SET_CMD_MAGIC_WORD_CHAR2) return;
//			if (pBattSys->canMbuRxMsg.data[3] != BCU_SET_CMD_MAGIC_WORD_CHAR3) return;
//			if (pBattSys->canMbuRxMsg.data[4] != BCU_SET_CMD_MAGIC_WORD_CHAR4) return;
//			if (pBattSys->canMbuRxMsg.data[5] != BCU_SET_CMD_MAGIC_WORD_CHAR5) return;
//			if (pBattSys->canMbuRxMsg.data[6] != BCU_SET_CMD_MAGIC_WORD_CHAR6) return;
//			//CRC8 Get
//			uint8_t crc8 = Get_SAE_CRC8(pBattSys->canMbuRxMsg.data, 7);
//			//CRC8 check:
//			if (pBattSys->canMbuRxMsg.data[7] != crc8) return;
//
//			//Shut Down Relays:
//			Thread::wait(50);
//
//		}
//	}


#define USING_STM32_RTC_BACKUP

	//0x2000002  Set BCU ID
	bcucmd_errortype_t BatterySys::Cmd_setBcuId()
	{
		//check set command data pattern:
		//NOW_ID NEW_ID 55 AA 'S' 'E' 'T' CRC8
		frami2c.newBcuId = pBattSys->canMbuRxMsg.data[1];

		//New BCU ID is not valid:
		if ((frami2c.newBcuId < PacketConst::MIN_BCU_ID) || (frami2c.newBcuId > PacketConst::MAX_BCU_ID))
			return (bcucmd_errortype_t::INVALID_NEW_ID);

		//check set command data pattern:
		//NOW_ID NEW_ID 55 AA 'S' 'E' 'T' CRC8
		if (pBattSys->canMbuRxMsg.data[2] != BCU_SET_CMD_MAGIC_WORD_CHAR2) return (bcucmd_errortype_t::BCU_MAGIC_WORD_ERROR);
		if (pBattSys->canMbuRxMsg.data[3] != BCU_SET_CMD_MAGIC_WORD_CHAR3) return (bcucmd_errortype_t::BCU_MAGIC_WORD_ERROR);
		if (pBattSys->canMbuRxMsg.data[4] != BCU_SET_CMD_MAGIC_WORD_CHAR4) return (bcucmd_errortype_t::BCU_MAGIC_WORD_ERROR);
		if (pBattSys->canMbuRxMsg.data[5] != BCU_SET_CMD_MAGIC_WORD_CHAR5) return (bcucmd_errortype_t::BCU_MAGIC_WORD_ERROR);
		if (pBattSys->canMbuRxMsg.data[6] != BCU_SET_CMD_MAGIC_WORD_CHAR6) return (bcucmd_errortype_t::BCU_MAGIC_WORD_ERROR);
		//CRC8 Get
		uint8_t crc8 = Get_SAE_CRC8(pBattSys->canMbuRxMsg.data, 7);
		//CRC8 check:
		////if(pBattSys->canMbuRxMsg.data[7] != crc8) return (BcuCmdErrorType_t::BCU_CRC_ERROR);

		frami2c.changeID_flag = 1;
		return (bcucmd_errortype_t::BCU_SET_CMD_OK);

	}


	//0x8XXYY2I
	bcucmd_errortype_t BatterySys::Cmd_setBcuMode()
	{
		//CRC8 Get
		uint8_t crc8 = Get_SAE_CRC8(pBattSys->canMbuRxMsg.data, 7);
		//CRC8 check:
/////		if(pBattSys->canMbuRxMsg.data[7] != crc8) return (BcuCmdErrorType_t::BCU_CRC_ERROR);

		uint8_t bcu_mode = pBattSys->canMbuRxMsg.data[0];	//Read BCU Mode
		//Isolation::InsulationCheck = pBattSys->canMbuRxMsg.data[1] & 0x01;
		uint8_t SnapshotON = (pBattSys->canMbuRxMsg.data[1] & 0x02) >> 1;
		BatterySys::SendBMUData = (pBattSys->canMbuRxMsg.data[1] & 0x04) >> 2;
//		BatterySys::SendBMUErrCNT = (pBattSys->canMbuRxMsg.data[1] & 0x08) >> 3;
		BatterySys::BMU_FAN_ON = (pBattSys->canMbuRxMsg.data[1] & 0x08) >> 3;
		//check bcu_mod validity:
		if (bcu_mode > (uint8_t)BCUModeChangeType_t::MAX_CHANGE)
		{
			return (bcucmd_errortype_t::INVALID_BCU_MODE);
		}
		
		if (BatterySys::SendBMUData == 1)
		{
			if (BatterySys::SendBMUDataOneFlag == 1)
			BatterySys::SendBMUDataFlag = 1;
		}
		else if (BatterySys::SendBMUData == 0)
		{
			BatterySys::SendBMUDataOneFlag = 1;
		}
		
		
		if (BatterySys::SendBMUErrCNT == 1)
		{
			if (BatterySys::SendBMUErrCNTOneFlag == 1)
				BatterySys::SendBMUErrCNTCommandFlag = 1;
		}
		else 	if (BatterySys::SendBMUErrCNT == 0)
		{
			BatterySys::SendBMUErrCNTOneFlag = 1;
		}
		
//		if (BatterySys::BMU_FAN_ON == 1)
//		{
//			RLY_K5_FAN = RLY_ON;
//		}
//		else
//		{
//			RLY_K5_FAN = RLY_OFF;
//		}

		

		//Echo_setBcuMode(pBattSys->canMbuRxMsg.data);
		//send ack packet:

		switch (static_cast<BCUModeChangeType_t>(bcu_mode))
		{
			case BCUModeChangeType_t::STANDBY: //0
			BmsEngine::m_bcuModeChange = BCUModeChangeType_t::STANDBY;
			break;

			case BCUModeChangeType_t::CHARGE_PRECHARGE: //1
			BmsEngine::m_bcuModeChange = BCUModeChangeType_t::CHARGE_PRECHARGE;
			break;

			case BCUModeChangeType_t::DRIVE_PRECHARGE://2
			BmsEngine::m_bcuModeChange = BCUModeChangeType_t::DRIVE_PRECHARGE;
			break;

//			case BCUModeChangeType_t::PRECHARGE: //3
//			BmsEngine::m_bcuModeChange = BCUModeChangeType_t::PRECHARGE;
//			break;
			
			case BCUModeChangeType_t::BCU_Emergency_Stop: //7
			BmsEngine::m_bcuModeChange = BCUModeChangeType_t::BCU_Emergency_Stop;
			break;

			default:
			BmsEngine::m_bcuModeChange = BCUModeChangeType_t::NONE;
			break;
		}

		bcuSetCount++;

		return (bcucmd_errortype_t::BCU_SET_CMD_OK);
	}
	

	//MBU Can bus:
	//================================================================
	//MBU CAN Bus command parser (ISR)
	//implemented as callback function of ISR
	//p_canMbu->attach(&mbu_canMessageParse, CAN::RxIrq);
	//================================================================
	void BatterySys::Mbu_canMessageParse()
	{
		canMbuCounter++;
		pBattSys->canMbuError.canTotalCount++;
		pBattSys->p_canMbu->read(pBattSys->canMbuRxMsg);	//read can message to clear Rx interrupt

		if ((pBattSys->canMbuRxMsg.id & BCU_SET_ID_MASK) == 0x02000002)		//setBcuId
		{
			//uint8_t bcuId = (static_cast<uint32_t>(pBattSys->canMbuRxMsg.id)& static_cast<uint32_t>(0x00ff0000)) >> 16;
			uint8_t bcuId = pBattSys->canMbuRxMsg.data[0];
			if (bcuId > PacketConst::MAX_BCU_ID)
			{
				return; //bcuId==0=> force setting (single BCU)
			}
			bcucmd_errortype_t result = bcucmd_errortype_t::ERROR_UNKNOWN;	//
			if ((bcuId == BmsEngine::m_bcuId) || (bcuId == 0))
			{
				result = Cmd_setBcuId();  
			}
			
		}
		if ((pBattSys->canMbuRxMsg.id & BCU_SET_ID_MASK) == 0x02000001)		//setBcuMode
		{
			uint8_t bcuId = (static_cast<uint32_t>(pBattSys->canMbuRxMsg.id)& static_cast<uint32_t>(0x00ff0000)) >> 16;	//
		
			//check if BCU ID matched: not matched and not broadcasting => exit
			if ((bcuId != BmsEngine::m_bcuId) && (bcuId != 0))
			{
				return;
			}

			//check valid BCU ID range:
			//if ((bcuId < PacketConst::MIN_BCU_ID) || (bcuId > PacketConst::MAX_BCU_ID)) return;
			if (bcuId > PacketConst::MAX_BCU_ID)
			{
				return; //bcuId==0=> force setting (single BCU)
			}

			//CRC8 check
			uint8_t crc8 = Get_SAE_CRC8(pBattSys->canMbuRxMsg.data, 7);
////			if (pBattSys->canMbuRxMsg.data[7] != crc8) return;

			//MBU can message parse :
			//===================================
			//uint32_t cmdId = pBattSys->canMbuRxMsg.id & BCU_SET_ID_MASK;

			bcucmd_errortype_t result = bcucmd_errortype_t::ERROR_UNKNOWN;	//
		
			if (bcuId == BmsEngine::m_bcuId) 
			{
				result = Cmd_setBcuMode();
				
				BatterySys::EmsCountFlag = 1;
				BatterySys::canEmsCounter++;
			}
		

		}
		if ((pBattSys->canMbuRxMsg.id & BCU_SET_RESET_MASK) == 0x2000010)		//setBCU Reset
		{
			uint8_t bcuId = pBattSys->canMbuRxMsg.data[0];	//
//			if (bcuId > PacketConst::MAX_BCU_ID)
//			{
//				return; //bcuId==0=> force setting (single BCU)
//			}
			if ((bcuId == BmsEngine::m_bcuId) || (bcuId == 0xFF))
			{
				BatterySys::Cmd_resetBCU();
			}
			
			
		}
		if ((pBattSys->canMbuRxMsg.id & BCU_SET_VERSION_MASK) == 0x2000021)		//setBCU Version Information
		{
			uint8_t bcuId = pBattSys->canMbuRxMsg.data[0];	//

			if (pBattSys->canMbuRxMsg.data[4] != BCU_SET_CMD_MAGIC_WORD_CHAR4) return;
			if (pBattSys->canMbuRxMsg.data[5] != BCU_SET_CMD_MAGIC_WORD_CHAR5) return;
			if (pBattSys->canMbuRxMsg.data[6] != BCU_SET_CMD_MAGIC_WORD_CHAR6) return;

			uint8_t crc8 = Get_SAE_CRC8(pBattSys->canMbuRxMsg.data, 7);	//
			//CRC8 check:
			////			if (pBattSys->canMbuRxMsg.data[7] != crc8) return;
			
			BatterySys::BCU_canVersion_Information();
			
			
		}
	
		if ((pBattSys->canMbuRxMsg.id & BCU_SET_SOC_MASK) == 0x2000030)		//setBCU SOC
		{
			uint8_t bcuId = pBattSys->canMbuRxMsg.data[0];  //

			if (pBattSys->canMbuRxMsg.data[4] != BCU_SET_CMD_MAGIC_WORD_CHAR4) return;
			if (pBattSys->canMbuRxMsg.data[5] != BCU_SET_CMD_MAGIC_WORD_CHAR7) return;
			if (pBattSys->canMbuRxMsg.data[6] != BCU_SET_CMD_MAGIC_WORD_CHAR8) return;

			uint8_t crc8 = Get_SAE_CRC8(pBattSys->canMbuRxMsg.data, 7);	//
			//CRC8 check:
			////			if (pBattSys->canMbuRxMsg.data[7] != crc8) return;
			if ((bcuId == BmsEngine::m_bcuId) || (bcuId == 0xFF))
			{
				//check if BCU ID matched:
				//if((bcuId != BmsEngine::m_bcuId) && (bcuId != 0xFF)) return;
				uint16_t SetSOC = pBattSys->canMbuRxMsg.data[2] | (pBattSys->canMbuRxMsg.data[3] << 8);
				if (SetSOC <= 0)	
				{
					SetSOC = 0;
				}   
				else if (SetSOC >= 1000)
				{
					SetSOC = 1000;
				}   
				//
				BmsEngine::coulombCount = (float)((float)SetSOC / 1000
						*(float)(BmsEngine::m_fullChargeCapacity*COULOMB_CONVERSION_MA));
				SOC::Set_BCU_SOC_Flag = 1;
				
			}        
		
			
			
		}
		
		if ((pBattSys->canMbuRxMsg.id & BCU_SET_SOH_MASK) == 0x2000031)		//setBCU SOH
		{
			uint8_t bcuId = pBattSys->canMbuRxMsg.data[0]; //bcuId==0=> force setting (single BCU)

			if (pBattSys->canMbuRxMsg.data[4] != BCU_SET_CMD_MAGIC_WORD_CHAR4) return;
			if (pBattSys->canMbuRxMsg.data[5] != BCU_SET_CMD_MAGIC_WORD_CHAR7) return;
			if (pBattSys->canMbuRxMsg.data[6] != 0x48) return;

			
			uint8_t crc8 = Get_SAE_CRC8(pBattSys->canMbuRxMsg.data, 7);	//
			//CRC8 check:
			////			if (pBattSys->canMbuRxMsg.data[7] != crc8) return;
			if ((bcuId == BmsEngine::m_bcuId) || (bcuId == 0xFF))
			{
				//check if BCU ID matched:
				//if((bcuId != BmsEngine::m_bcuId) && (bcuId != 0xFF)) return;
				uint16_t SetSOH = pBattSys->canMbuRxMsg.data[2] | (pBattSys->canMbuRxMsg.data[3] << 8);
				if (SetSOH <= 0)	
				{
					SetSOH = 0;
				}   
				else if (SetSOH >= 1000)
				{
					SetSOH = 1000;
				}   
				
				BmsEngine::m_soh = SetSOH;
				//BmsEngine::coulombCount = (float)((float)SetSOC / 1000
				//		*(float)(BmsEngine::m_fullChargeCapacity*COULOMB_CONVERSION_mA));
				SOC::Set_BCU_SOH_Flag = 1;
				SOC::StoredSOH_Flag = 1;
				SOC::SET_StoredSOH_Flag = 1;
				
			}        						
		}
		
		if ((pBattSys->canMbuRxMsg.id & BMU_SET_VERSION_MASK) == 0x2000022)		//setBMU Version Information
		{
			uint8_t bcuId = pBattSys->canMbuRxMsg.data[0];	//
			if (pBattSys->canMbuRxMsg.data[1] != 0x42) return;
			if (pBattSys->canMbuRxMsg.data[2] != 0x4D) return;
			if (pBattSys->canMbuRxMsg.data[3] != 0x55) return;
			if (pBattSys->canMbuRxMsg.data[4] != BCU_SET_CMD_MAGIC_WORD_CHAR4) return;
			if (pBattSys->canMbuRxMsg.data[5] != BCU_SET_CMD_MAGIC_WORD_CHAR5) return;
			if (pBattSys->canMbuRxMsg.data[6] != BCU_SET_CMD_MAGIC_WORD_CHAR6) return;

			uint8_t crc8 = Get_SAE_CRC8(pBattSys->canMbuRxMsg.data, 7);	//
			//CRC8 check:
			////			if (pBattSys->canMbuRxMsg.data[7] != crc8) return;		
			for (uint8_t idpack = 0 ; idpack < PackConst::MAX_PACK_NUM ; idpack++) 
			{
				pack[idpack].HWVer = 0;
				pack[idpack].SWVer = 0;

			}

//			wait(5); //Watchdog TEST
//			BatterySys::BMU_Ver_CANCommand();
			BatterySys::SendBMUVerFlag = 1;
		}
		
		if ((pBattSys->canMbuRxMsg.id & BMU_SET_RESET_MASK) == 0x2000011)		//setBMU Reset
		{
			uint8_t bcuId = pBattSys->canMbuRxMsg.data[0];	//
				//			if (bcuId > PacketConst::MAX_BCU_ID)
				//			{
				//				return; //bcuId==0=> force setting (single BCU)
				//			}
						//check if BCU ID matched:
			if ((bcuId != BmsEngine::m_bcuId) && (bcuId != 0xFF)) return;

			//check valid BCU ID range:

			if (pBattSys->canMbuRxMsg.data[1] != BCU_SET_CMD_MAGIC_WORD_CHAR1) return;
			if (pBattSys->canMbuRxMsg.data[2] != 0x55) return;
			if (pBattSys->canMbuRxMsg.data[3] != 0xAA) return;
			if (pBattSys->canMbuRxMsg.data[4] != BCU_SET_CMD_MAGIC_WORD_CHAR4) return;
			if (pBattSys->canMbuRxMsg.data[5] != BCU_SET_CMD_MAGIC_WORD_CHAR5) return;
			if (pBattSys->canMbuRxMsg.data[6] != BCU_SET_CMD_MAGIC_WORD_CHAR6) return;

			uint8_t crc8 = Get_SAE_CRC8(pBattSys->canMbuRxMsg.data, 7);	//
			//CRC8 check:
////			if (pBattSys->canMbuRxMsg.data[7] != crc8) return;
			if ((bcuId == BmsEngine::m_bcuId) || (bcuId == 0xFF))
			{
				BatterySys::Cmd_resetBMU();
//				BatterySys::Stop_iCAN_Flag = 1;
//				BatterySys::Heart_bit_Flag = 0;
			}		
			
		}
		
		if ((pBattSys->canMbuRxMsg.id & BCU_SET_CLEAR_MASK) == 0x2000012)		//setBCU Clear Error Flag
		{
			uint8_t bcuId = pBattSys->canMbuRxMsg.data[0];	//

			//check if BCU ID matched:
//			if ((bcuId != BmsEngine::m_bcuId) && (bcuId != 0xFF)) return;
						
			if ((bcuId == BmsEngine::m_bcuId) || (bcuId == 0xFF))
			{
				BatterySys::Cmd_ClearFlagBCU();
			}
			
			
		}
				
		if ((pBattSys->canMbuRxMsg.id & BCU_SET_MANUAL_BALANCE_MASK) == 0x2000050)		//Set BCU Manual Balance
		{
			uint8_t bcuId = pBattSys->canMbuRxMsg.data[0];	//
			uint8_t BMU_Id = pBattSys->canMbuRxMsg.data[1];	//
			uint8_t BMU_Channel_1 = pBattSys->canMbuRxMsg.data[2];	//
			uint8_t BMU_Channel_2 = pBattSys->canMbuRxMsg.data[3];	//
			//check if BCU ID matched:
			if ((bcuId != BmsEngine::m_bcuId) && (bcuId != 0xFF) && (bcuId != 0x00)) return;

			// check valid BCU ID range:
			if (pBattSys->canMbuRxMsg.data[4] != BCU_SET_CMD_MAGIC_WORD_CHAR4) return;
			if (pBattSys->canMbuRxMsg.data[5] != BCU_SET_CMD_MAGIC_WORD_CHAR5) return;
			if (pBattSys->canMbuRxMsg.data[6] != BCU_SET_CMD_MAGIC_WORD_CHAR6) return;

			uint8_t crc8 = Get_SAE_CRC8(pBattSys->canMbuRxMsg.data, 7);	//
			//CRC8 check:
////			if (pBattSys->canMbuRxMsg.data[7] != crc8) return;
			if ((bcuId == BmsEngine::m_bcuId) || (bcuId == 0xFF) || (bcuId == 0x00))
			{
				Balance::BCU_SET_Manual_Balance_flag = 1;
														 			
				//	將十進制到二進制轉換算法
				//步驟1：將數字除以％(模運算符)2，並將餘數存儲在數組中
				//步驟2：通過/(除法運算符)將數字除以2，
				//步驟3：重複步驟2，直到數字大於零
				int Binary[PackConst::MAX_CELL_NUM] = { 0 };   
				uint16_t BMU_Channel = (BMU_Channel_2 << 8) | BMU_Channel_1;
				
				for (int i = 0; BMU_Channel > 0; i++)    //
				{    
					Binary[i] = BMU_Channel % 2;    
					BMU_Channel = BMU_Channel / 2;  
				}    
				
				if (bcuId == 0x00)			
				{
					Balance::BCU_SET_Manual_Balance_flag = 0;
					for (int j = 0; j < PackConst::MAX_PACK_NUM; j++)//
					{
						for (int i = 0; i < PackConst::MAX_CELL_NUM; i++)//
						{
							BatterySys::pack[j].cellBalance[i] = 0;
						}						
					}
				}
				else
				{
					if (BMU_Id == 0xFF)	
					{
						for (int j = 0; j < PackConst::MAX_PACK_NUM; j++)//
						{
							for (int i = 0; i < PackConst::MAX_CELL_NUM; i++)//
							{
								BatterySys::pack[j].cellBalance[i] = 1;
							}						
						}
					}
					else if (BMU_Id == 0x00)	
					{
						Balance::BCU_SET_Manual_Balance_flag = 0;
						for (int j = 0; j < PackConst::MAX_PACK_NUM; j++)//
						{
							for (int i = 0; i < PackConst::MAX_CELL_NUM; i++)//
							{
								BatterySys::pack[j].cellBalance[i] = 0;
							}						
						}
					}
					else
					{
						for (int i = 0; i < PackConst::MAX_CELL_NUM; i++)//
						{
							BatterySys::pack[BMU_Id-1].cellBalance[i] = Binary[i];								
						}
					}

				}
												
			}	
		}
		if ((pBattSys->canMbuRxMsg.id & BCU_SET_SEMI_AUTO_BALANCE_MASK) == 0x2000051)		//Set BCU semi-automatic Balance
		{
			uint8_t bcuId = pBattSys->canMbuRxMsg.data[0];//
			uint16_t Start_CellVoltage = pBattSys->canMbuRxMsg.data[2] | (pBattSys->canMbuRxMsg.data[3] << 8 );//
			
			//check if BCU ID matched:
			if ((bcuId != BmsEngine::m_bcuId) && (bcuId != 0xFF) && (bcuId != 0x00)) return;

			//check valid BCU ID range:
			if (pBattSys->canMbuRxMsg.data[4] != BCU_SET_CMD_MAGIC_WORD_CHAR4) return;
			if (pBattSys->canMbuRxMsg.data[5] != BCU_SET_CMD_MAGIC_WORD_CHAR5) return;
			if (pBattSys->canMbuRxMsg.data[6] != BCU_SET_CMD_MAGIC_WORD_CHAR6) return;
			if ((Start_CellVoltage > 3700) || (Start_CellVoltage < 3000)) return;
			
			uint8_t crc8 = Get_SAE_CRC8(pBattSys->canMbuRxMsg.data, 7);//
			//CRC8 check:
////			if (pBattSys->canMbuRxMsg.data[7] != crc8) return;
			if ((bcuId == BmsEngine::m_bcuId) || (bcuId == 0xFF) || (bcuId == 0x00))
			{
				Balance::BCU_SET_Semi_Automatic_Balance_flag = 1;										 			
				Balance::BalanceVoltageSort(Start_CellVoltage); //BatterySys::pack[x].cellBalance[y] = 1;	
					
				if (bcuId == 0x00)			
				{
					Balance::BCU_SET_Semi_Automatic_Balance_flag = 0;
					for (int j = 0; j < PackConst::MAX_PACK_NUM; j++)//
					{
						for (int i = 0; i < PackConst::MAX_CELL_NUM; i++)//
						{
							BatterySys::pack[j].cellBalance[i] = 0;
						}						
					}
				}
												
			}	
		}

	} //mbu_canMessageParse

    // MBU Can report messgaes: Voltage, Temperature, Status
    // messages from BCU to MCU
    //================================================================
    // ReSharper disable once CppParameterMayBeConst
    /**
    * \brief Report voltage information (Packet ID:0x9xxy to MBU (thru CAN bus 2)
    * \param packNum: packet nymber (1~20)
    */
	
	

	//0x8XXYY2I:XX=BCU ID, YY=BMU ID, I==cellIdx
	void BatterySys::Mbu_canReportVoltage(int packNum) //CM ID
	{
		char dataBuf[8] = { 0 };//

		for (uint8_t cellIdx = 0; cellIdx < 4; cellIdx++)//
		{
			uint32_t id =  MBU_VOLTAGE_ID_BASE | ((uint32_t)packNum << 8) | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16) | (uint32_t)cellIdx;//

			//check if valid pack #
			if ((packNum > PackConst::MAX_PACK_NUM) || (packNum < PackConst::MIN_PACK_NUM)) return;                            

			uint16_t voltage1 = pack[packNum - 1].cellVolt[cellIdx * 3]; //
			uint16_t voltage2 = pack[packNum - 1].cellVolt[cellIdx * 3 + 1]; //
			uint16_t voltage3 = pack[packNum - 1].cellVolt[cellIdx * 3 + 2]; //

			uint8_t StatusBalance1 = pack[packNum - 1].cellBalance[cellIdx * 3]; //
			uint8_t StatusBalance2 = pack[packNum - 1].cellBalance[cellIdx * 3 + 1]; //
			uint8_t StatusBalance3 = pack[packNum - 1].cellBalance[cellIdx * 3 + 2]; //

			dataBuf[0] = (uint8_t)(voltage1 & 0x00FF);
			dataBuf[1] = (uint8_t)(((voltage1 >> 8) & 0x1F) | ((StatusBalance1 << 5) & 0xE0)); //+BALANCE status
			dataBuf[2] = (uint8_t)(voltage2 & 0x00FF);
			dataBuf[3] = (uint8_t)(((voltage2 >> 8) & 0x1F) | ((StatusBalance2 << 5) & 0xE0));
			dataBuf[4] = (uint8_t)(voltage3 & 0x00FF);
			dataBuf[5] = (uint8_t)(((voltage3 >> 8) & 0x1F) | ((StatusBalance3 << 5) & 0xE0));
			dataBuf[6] =  0x00;

#if defined(USING_SAE_J1850_CRC8)

			dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t*>(dataBuf), 7);
#else
			dataBuf[7] = 0;
			for (int i = 0 ; i < CM_CHKSUM_INDEX ; i++) dataBuf[7] += dataBuf[i];
#endif

			int canEmsRet=pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_VOLT_PACKET_LENGTH, CANData, CANExtended));//
			wait_us(BCU_CAN_DELAYTIME);
//			if (canEmsRet == 0)	
//			{
//				//for (int i = 0; i < BCU_CAN_Repeat_number; i++)
//				while (true)
//				{
//					wait_us(600);     
//					int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_VOLT_PACKET_LENGTH, CANData, CANExtended));//
//					if (canEmsRet == 1)
//						break;
//				}
//			}
		}
	}
		

	//0x8XXYY3I   : XX=CM ID, YY=BCU ID, I=cellIdx
	void BatterySys::Mbu_canReportTemperature(int packNum)
	{
		char dataBuf[8] = { 0 };//

		for (uint8_t cellIdx = 0 ; cellIdx < 2 ; cellIdx++)//
		{
			//check if valid pack #
			if ((packNum > PackConst::MAX_PACK_NUM) || (packNum < PackConst::MIN_PACK_NUM)) return;

			uint32_t id = MBU_CELL_TEMP_ID_BASE | ((uint32_t)packNum << 8) | ((uint32_t)pBattSys->p_bmsParam->m_bcuId) << 16 | (uint32_t)cellIdx;//

			int16_t Temperature1 = pack[packNum - 1].cellTemp[cellIdx * 4];//
			int16_t Temperature2 = pack[packNum - 1].cellTemp[cellIdx * 4 + 1];//
			int16_t Temperature3 = pack[packNum - 1].cellTemp[cellIdx * 4 + 2];//
			int16_t Temperature4 = pack[packNum - 1].cellTemp[cellIdx * 4 + 3];//

			dataBuf[0] = (int16_t)(Temperature1);
			dataBuf[1] = (int16_t)(((Temperature1 >> 8) & 0x0F) | ((Temperature2 << 4) & 0xF0));
			dataBuf[2] = (int16_t)(Temperature2 >> 4);
			dataBuf[3] = (int16_t)(Temperature3);
			dataBuf[4] = (int16_t)(((Temperature3 >> 8) & 0x0F) | ((Temperature4 << 4) & 0xF0));
			dataBuf[5] = (int16_t)(Temperature4 >> 4);
			dataBuf[6] =  0x00;

#if defined(USING_SAE_J1850_CRC8)

			dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), 7);
#else
			dataBuf[7] = 0;
			for (int i = 0 ; i < CM_CHKSUM_INDEX ; i++) dataBuf[7] += dataBuf[i];
#endif
			//
			int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_TEMP_PACKET_LENGTH, CANData, CANExtended));
			wait_us(BCU_CAN_DELAYTIME);  
			//0x8XXYY3I   : XX=BCU ID, YY=BMU ID, I=cellIdx
//			if (canEmsRet == 0)
//			{
//				//for (int i = 0; i < BCU_CAN_Repeat_number; i++)
//				while (true)
//				{
//					wait_us(600);     
//					int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_TEMP_PACKET_LENGTH, CANData, CANExtended));//
//					if (canEmsRet == 1)
//						break;					
//				}
//
//			}

		}
	}

	
	//0x8XXYY3F   : XX=CM ID, YY=BCU ID,
	void BatterySys::BMU_canReportPackTemperature(int packNum)
	{
		char dataBuf[8] = { 0 };//

		//check if valid pack #
		if ((packNum > PackConst::MAX_PACK_NUM) || (packNum < PackConst::MIN_PACK_NUM)) return;

		//
		uint32_t id = MBU_PACK_TEMP_ID_BASE | ((uint32_t)packNum << 8) | ((uint32_t)pBattSys->p_bmsParam->m_bcuId) << 16 | 0x0F;

		const int16_t TEMPERATURE1 = pack[packNum - 1].packTemp[0];//
		const int16_t TEMPERATURE2 = pack[packNum - 1].packTemp[1];//
		const int16_t TEMPERATURE3 = pack[packNum - 1].packTemp[2];//
		const int16_t TEMPERATURE4 = pack[packNum - 1].packTemp[3];//

		dataBuf[0] = static_cast<int16_t>(TEMPERATURE1);
		dataBuf[1] = static_cast<int16_t>((TEMPERATURE1 >> 8) & 0x0F) | static_cast<int16_t>((TEMPERATURE2 << 4) & 0xF0);
		dataBuf[2] = static_cast<int16_t>(TEMPERATURE2 >> 4);
		dataBuf[3] = static_cast<int16_t>(TEMPERATURE3);
		dataBuf[4] = static_cast<int16_t>((TEMPERATURE3 >> 8) & 0x0F) | static_cast<int16_t>((TEMPERATURE4 << 4) & 0xF0);
		dataBuf[5] = static_cast<int16_t>(TEMPERATURE4 >> 4);
		dataBuf[6] =  0x00;

#if defined(USING_SAE_J1850_CRC8)

		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), 7);
#else
		dataBuf[7] = 0;
		for (int i = 0 ; i < CM_CHKSUM_INDEX ; i++) dataBuf[7] += dataBuf[i];
#endif

		int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_TEMP_PACKET_LENGTH, CANData, CANExtended));//
		wait_us(BCU_CAN_DELAYTIME);
		//0x8XXYY3F   : XX=BCU ID, YY=BMU ID, I=cellIdx
//		if (canEmsRet == 0)
//		{
//			//for (int i = 0; i < BCU_CAN_Repeat_number; i++)
//			while (true)
//			{
//				wait_us(600);
//				//
//				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_TEMP_PACKET_LENGTH, CANData, CANExtended));
//				if (canEmsRet == 1)
//					break;					
//			}
//
//		}


	}

	//CanReportPECerror 0x8XX0060:
	void BatterySys::CanReportPECerror()
	{
		char dataBuf[8] = { 0 };//
		//=============================================================================
		uint32_t id = BMU_ISOSPI_ERRORCNT_ID_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16);

//		uint32_t id =  BMU_isoSPI_ErrorCNT_ID_BASE | (packidNum << 8) | (pBattSys->p_bmsParam->m_bcuId) << 16;
		
		dataBuf[0] = LTC6811SPI::Volt_PEC_Err;
		dataBuf[1] = LTC6811SPI::Volt_PEC_Err >> 8;
		dataBuf[2] = LTC6811SPI::com_PEC_Err;
		dataBuf[3] = LTC6811SPI::com_PEC_Err >> 8;
		dataBuf[4] = 0x00;
		dataBuf[5] = 0x00;
		dataBuf[6] = 0x00;
														
#if defined(USING_SAE_J1850_CRC8)
		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0; //check sum
		for (int i = 0; i < BCU_STATUS_CHKSUM_INDEX; i++) dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i]; //chksum
#endif
				
		int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));//
		wait_us(BCU_CAN_DELAYTIME);
//		if (canEmsRet == 0)	
//		{
////			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
//			while (true)
//			{
//				wait_us(BCU_CAN_DELAYTIME);
//				//
//				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
//				if (canEmsRet == 1)
//					break;
//			}
//		}
		
	}
	
	//CanReportBCU_Temperature_1 0x8XX0070:
	void BatterySys::CanReportBCU_Temperature_1()
	{
		char dataBuf[8] = { 0 };//
		//=============================================================================
		uint32_t id = BCU_TEMPERATURE_1_ID_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16);	
		
		dataBuf[0] = ADCC::TempTxCH[0];
		dataBuf[1] = ((ADCC::TempTxCH[0] >> 8) & 0x0F) | ((ADCC::TempTxCH[1] << 4) & 0xF0);
		dataBuf[2] = (ADCC::TempTxCH[1] >> 4);
		dataBuf[3] = ADCC::TempTxCH[2];
		dataBuf[4] = ((ADCC::TempTxCH[2] >> 8) & 0x0F) | ((ADCC::TempTxCH[3] << 4) & 0xF0);
		dataBuf[5] = (ADCC::TempTxCH[3] >> 4);
		dataBuf[6] =  0x00;			
#if defined(USING_SAE_J1850_CRC8)
		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0; //check sum
		for (int i = 0; i < BCU_STATUS_CHKSUM_INDEX; i++) dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i]; //chksum
#endif
					
		int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));//
		wait_us(BCU_CAN_DELAYTIME);
//		if (canEmsRet == 0)	
//		{
////			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
//			while (true)
//			{
//				wait_us(BCU_CAN_DELAYTIME);
//				//
//				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
//				if (canEmsRet == 1)
//					break;
//			}
//		}
		
	}
	
	//CanReportBCU_Temperature_2 0x8XX0071:
	void BatterySys::CanReportBCU_Temperature_2()
	{
		char dataBuf[8] = { 0 };//
		//=============================================================================
		uint32_t id = BCU_TEMPERATURE_2_ID_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16);
		
		dataBuf[0] = ADCC::TempTxCH[4];
		dataBuf[1] = ((ADCC::TempTxCH[4] >> 8) & 0x0F) | ((ADCC::TempTxCH[5] << 4) & 0xF0);
		dataBuf[2] = (ADCC::TempTxCH[5] >> 4);
		dataBuf[3] = ADCC::TempTxCH[6];
		dataBuf[4] = ((ADCC::TempTxCH[6] >> 8) & 0x0F) | ((ADCC::TempTxCH[7] << 4) & 0xF0);
		dataBuf[5] = (ADCC::TempTxCH[7] >> 4);
		dataBuf[6] =  0x00;			
#if defined(USING_SAE_J1850_CRC8)
		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0; //check sum
		for (int i = 0; i < BCU_STATUS_CHKSUM_INDEX; i++) dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i]; //chksum
#endif
						
		int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));//
		wait_us(BCU_CAN_DELAYTIME);
//		if (canEmsRet == 0)	
//		{
////			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
//			while (true)
//			{
//				wait_us(BCU_CAN_DELAYTIME);
//				//
//				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
//				if (canEmsRet == 1)
//					break;
//			}
//		}
		
	}
	
	//Heartbeat 0x8XX0012:
	void BatterySys::CanReportHeartbeat()
	{
		char dataBuf[8] = { 0 };//
		//=============================================================================
		uint32_t id = BMU_HEARTBEAT | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16);
	
		dataBuf[0] = BatterySys::pack[0].PackHeartbeat[0] | BatterySys::pack[1].PackHeartbeat[0] << 4;
		dataBuf[1] = BatterySys::pack[2].PackHeartbeat[0] | (BatterySys::pack[3].PackHeartbeat[0] << 4);
		dataBuf[2] = BatterySys::pack[4].PackHeartbeat[0] | BatterySys::pack[5].PackHeartbeat[0] << 4;
		dataBuf[3] = BatterySys::pack[6].PackHeartbeat[0] | BatterySys::pack[7].PackHeartbeat[0] << 4;
		dataBuf[4] = BatterySys::pack[8].PackHeartbeat[0] | BatterySys::pack[9].PackHeartbeat[0] << 4;
		dataBuf[5] = BatterySys::pack[10].PackHeartbeat[0] | BatterySys::pack[11].PackHeartbeat[0] << 4;
		dataBuf[6] = BatterySys::pack[12].PackHeartbeat[0] | BatterySys::pack[13].PackHeartbeat[0] << 4;
														
#if defined(USING_SAE_J1850_CRC8)
		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0;
		for (int i = 0 ; i < BCU_STATUS_CHKSUM_INDEX ; i++)
		{
			dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i];
		}
#endif
		
//		int canCmRet = pBattSys->p_canCm->write(CANMessage(id, dataBuf, 8, CANData, CANExtended));
//		if (canCmRet == 0)	
//		{
//			for (int i = 0; i < BCU_CAN_Repeat_number; i++)
//			//while (true)	
//			{
//				wait_us(600);     
//				int canCmRet = pBattSys->p_canCm->write(CANMessage(id, dataBuf, 8, CANData, CANExtended));
//				if (canCmRet == 1)
//					break;
//			}
//
//		}
		//
		int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_VOLT_PACKET_LENGTH, CANData, CANExtended));
		if (canEmsRet == 0)	
		{
			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
				//while(true)
			{
				wait_us(600);
				//
				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_VOLT_PACKET_LENGTH, CANData, CANExtended));
				if (canEmsRet == 1)
					break;
			}
		}
		
	}
	
	
	//Heartbeat-1 0x8XX0061:
	void BatterySys::CanReportHeartbeat_1()
	{
		char dataBuf[8] = { 0 };//
		uint32_t id = BMU_HEARTBEAT_1_ID_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16); //
	
		dataBuf[0] = BatterySys::pack[0].PackHeartbeat[0] | BatterySys::pack[1].PackHeartbeat[0] << 4;
		dataBuf[1] = BatterySys::pack[2].PackHeartbeat[0] | BatterySys::pack[3].PackHeartbeat[0] << 4;
		dataBuf[2] = BatterySys::pack[4].PackHeartbeat[0] | BatterySys::pack[5].PackHeartbeat[0] << 4;
		dataBuf[3] = BatterySys::pack[6].PackHeartbeat[0] | BatterySys::pack[7].PackHeartbeat[0] << 4;
		dataBuf[4] = BatterySys::pack[8].PackHeartbeat[0] | BatterySys::pack[9].PackHeartbeat[0] << 4;
		dataBuf[5] = BatterySys::pack[10].PackHeartbeat[0] | BatterySys::pack[11].PackHeartbeat[0] << 4;
		dataBuf[6] = BatterySys::pack[12].PackHeartbeat[0] | BatterySys::pack[13].PackHeartbeat[0] << 4;
														
#if defined(USING_SAE_J1850_CRC8)
		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0; //check sum
		for (int i = 0; i < BCU_STATUS_CHKSUM_INDEX; i++) dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i]; //chksum
#endif
		//
		int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_VOLT_PACKET_LENGTH, CANData, CANExtended));
		if (canEmsRet == 0)	
		{
			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
			//while(true)
			{
				wait_us(600);     
				//
				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_VOLT_PACKET_LENGTH, CANData, CANExtended));
				if (canEmsRet == 1)
					break;
			}
		}
		
	}
	
	//Heartbeat-2 0x8XX0062:
	void BatterySys::CanReportHeartbeat_2()
	{
		char dataBuf[8] = { 0 };//
		uint32_t id = BMU_HEARTBEAT_2_ID_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16); //
	
		dataBuf[0] = BatterySys::pack[14].PackHeartbeat[0] | BatterySys::pack[15].PackHeartbeat[0] << 4;
		dataBuf[1] = BatterySys::pack[16].PackHeartbeat[0] | BatterySys::pack[17].PackHeartbeat[0] << 4;
		dataBuf[2] = BatterySys::pack[18].PackHeartbeat[0] | BatterySys::pack[19].PackHeartbeat[0] << 4;
		dataBuf[3] = BatterySys::pack[20].PackHeartbeat[0] | BatterySys::pack[21].PackHeartbeat[0] << 4;
		dataBuf[4] = BatterySys::pack[22].PackHeartbeat[0] | BatterySys::pack[23].PackHeartbeat[0] << 4;
		dataBuf[5] = BatterySys::pack[24].PackHeartbeat[0] | BatterySys::pack[25].PackHeartbeat[0] << 4;
		dataBuf[6] = BatterySys::pack[26].PackHeartbeat[0] | BatterySys::pack[27].PackHeartbeat[0] << 4;
														
#if defined(USING_SAE_J1850_CRC8)
		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0; //check sum
		for (int i = 0; i < BCU_STATUS_CHKSUM_INDEX; i++) dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i]; //chksum
#endif
		//
		int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_VOLT_PACKET_LENGTH, CANData, CANExtended));
		if (canEmsRet == 0)	
		{
			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
			//while(true)
			{
				wait_us(600);
				//
				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_VOLT_PACKET_LENGTH, CANData, CANExtended));
				if (canEmsRet == 1)
					break;
			}
		}
		
	}
	
	//Heartbeat-3 0x8XX0063:
	void BatterySys::CanReportHeartbeat_3()
	{
		char dataBuf[8] = { 0 };//
		uint32_t id = BMU_HEARTBEAT_3_ID_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16); //
	
		dataBuf[0] = BatterySys::pack[28].PackHeartbeat[0] | BatterySys::pack[29].PackHeartbeat[0] << 4;
		dataBuf[1] = BatterySys::pack[30].PackHeartbeat[0] | BatterySys::pack[31].PackHeartbeat[0] << 4;
		dataBuf[2] = BatterySys::pack[32].PackHeartbeat[0] | BatterySys::pack[33].PackHeartbeat[0] << 4;
		dataBuf[3] = BatterySys::pack[34].PackHeartbeat[0] | BatterySys::pack[35].PackHeartbeat[0] << 4;
		dataBuf[4] = BatterySys::pack[36].PackHeartbeat[0] | BatterySys::pack[37].PackHeartbeat[0] << 4;
		dataBuf[5] = 0x00;
		dataBuf[6] = 0x00;
														
#if defined(USING_SAE_J1850_CRC8)
		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0; //check sum
		for (int i = 0; i < BCU_STATUS_CHKSUM_INDEX; i++) dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i]; //chksum
#endif
		//
		int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_VOLT_PACKET_LENGTH, CANData, CANExtended));
		if (canEmsRet == 0)	
		{
			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
			//while(true)
			{
				wait_us(600);
				//
				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_VOLT_PACKET_LENGTH, CANData, CANExtended));
				if (canEmsRet == 1)
					break;
			}
		}
		
	}
	//BMU Version
	void BatterySys::BMU_canReportVersion(int packidNum) //CM ID
	{
		char dataBuf[8] = { 0 };//

		uint32_t id =  BMU_VERSION_ID_BASE | ((uint32_t)packidNum << 8) | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16) ;//

		//check if valid pack #
		if ((packidNum > PackConst::MAX_PACK_NUM) || (packidNum < PackConst::MIN_PACK_NUM)) return;

		const uint16_t CANHWVER = pack[packidNum - 1].HWVer;  //
		const uint16_t CANSWVER = pack[packidNum - 1].SWVer;  //

		dataBuf[0] = static_cast<uint8_t>(CANHWVER & 0x00FF);
		dataBuf[1] = (CANHWVER >> 8) | (CANHWVER >> 12);
		dataBuf[2] = static_cast<uint8_t>(CANSWVER & 0x00FF);
		dataBuf[3] = (CANSWVER >> 8) | (CANSWVER << 12);
		dataBuf[4] = 0x00;
		dataBuf[5] = 0x00;
		dataBuf[6] = 0x00;

#if defined(USING_SAE_J1850_CRC8)

		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t*>(dataBuf), 7);
#else
		dataBuf[7] = 0;
		for (int i = 0 ; i < CM_CHKSUM_INDEX ; i++) dataBuf[7] += dataBuf[i];
#endif
			//
			int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_VOLT_PACKET_LENGTH, CANData, CANExtended));
			if (canEmsRet == 0)	
			{
//				for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
				while(true)
				{
					wait_us(700);     
					//
					int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, CM_VOLT_PACKET_LENGTH, CANData, CANExtended));
					if (canEmsRet == 1)
						break;
				}
			}
		
	}
	
	
	//0x8XXYY40
	void BatterySys::BCUCANMessageDataMain()
	{
		char dataBuf[8] = { 0 };//
		int16_t AmpReading_m = m_ampReading;//

		uint32_t id = BCUCAN_DATAMAIN_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16);//

		dataBuf[0] = BatterySys::m_voltReading;  //static_cast<uint8_t>(m_voltReading & 0x00ff); 
		dataBuf[1] = BatterySys::m_voltReading>>8; //static_cast<uint8_t>((m_voltReading & 0xff00) >> 8);
		dataBuf[2] = static_cast<int16_t>(AmpReading_m);    		
		dataBuf[3] = static_cast<int16_t>((AmpReading_m) >> 8);     					
		dataBuf[4] = static_cast<uint8_t>(BmsEngine::m_soc & 0x00ff);
		dataBuf[5] = static_cast<uint8_t>((BmsEngine::m_soc & 0xff00) >> 8);
		dataBuf[6] = (char)pBattSys->p_bmsParam->m_bcuState;

#if defined(USING_SAE_J1850_CRC8)

		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0;
		for (int i = 0 ; i < BCU_STATUS_CHKSUM_INDEX ; i++)
		{
			dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i];
		}
#endif
		//
		int canEmsRet=pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
		wait_us(BCU_CAN_DELAYTIME);
//		if (canEmsRet == 0)	
//		{
////			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
//			while (true)
//			{
//				wait_us(600);
//				//
//				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
//				if (canEmsRet == 1)
//					break;
//			}
//		}
	}
	
	//0x8XXYY41
	void BatterySys::BCUCANMessageCellVoltage()
	{
		char dataBuf[8] = { 0 };//

		uint32_t id = BCUCAN_CELLVOLTAGE_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16);//

		dataBuf[0] = BatterySys::MaxVolt & 0x00ff; 
		dataBuf[1] = (BatterySys::MaxVolt & 0xff00) >> 8; 
		dataBuf[2] = BatterySys::MinVolt & 0x00ff; 		
		dataBuf[3] = (BatterySys::MinVolt & 0xff00) >> 8;  					
		dataBuf[4] = BatterySys::MaxVoltCH << 7 | BatterySys::MaxVoltModule;     
		dataBuf[5] = BatterySys::MaxVoltCH >> 1 | BatterySys::MinVoltModule << 4;
		dataBuf[6] = BatterySys::MinVoltModule >> 4 |  BatterySys::MinVoltCH << 3;  
				
#if defined(USING_SAE_J1850_CRC8)

		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
	    dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0;

	    for (int i = 0 ; i < BCU_STATUS_CHKSUM_INDEX ; i++)
	    {
	    	dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i];
	    }
#endif
	    //
		int canEmsRet=pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
		wait_us(BCU_CAN_DELAYTIME);
//		if (canEmsRet == 0)	
//		{
////			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
//			while (true)
//			{
//				wait_us(BCU_CAN_DELAYTIME);
//				//
//				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
//				if (canEmsRet == 1)
//					break;
//			}
//
//		}
	}
	
	//0x8XXYY42
	void BatterySys::BCUCANMessageCellTemperature()
	{
		char dataBuf[8] = { 0 };//

		uint32_t id = BCUCAN_CELLTEMPERATURE_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16);//
		//BmsEngine::MaxSort_Temp = BatterySys::MaxTemp;
	
		dataBuf[0] =  BatterySys::MaxTemp & 0x00FF;  
		dataBuf[1] = (BatterySys::MaxTemp & 0xFF00) >> 8;
		dataBuf[2] = BatterySys::MinTemp & 0x00FF;       	
		dataBuf[3] = (BatterySys::MinTemp & 0xFF00) >> 8;     					
		dataBuf[4] = BatterySys::MaxTempCH << 7 | BatterySys::MaxTempModule;      
		dataBuf[5] = BatterySys::MaxTempCH >> 1 | BatterySys::MinTempModule << 4;
		dataBuf[6] = BatterySys::MinTempModule >> 4 |  BatterySys::MinTempCH << 3;	
	
#if defined(USING_SAE_J1850_CRC8)

		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0;

		for (int i = 0 ; i < BCU_STATUS_CHKSUM_INDEX ; i++)
		{
			dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i];
		}
#endif
		//
		int canEmsRet=pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
		wait_us(BCU_CAN_DELAYTIME);
//		if (canEmsRet == 0)	
//		{
////			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
//			while (true)
//			{
//				wait_us(BCU_CAN_DELAYTIME);
//				//
//				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
//				if (canEmsRet == 1)
//					break;
//			}
//
//		}
	}
	
	//0x8XXYY43
	void BatterySys::BCUCANMessageProtection()
	{
		char dataBuf[8] = { 0 };//
		uint32_t id = BCUCAN_PROTECTION_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16);//

		uint8_t status1 = 0;//
		if (BmsEngine::m_bcuStatus1 & BCU_STATUS_OV1) status1 |= BCU_REPORT_EVENT_OVP_MASK;
		if (BmsEngine::m_bcuStatus1 & BCU_STATUS_UV1) status1 |= BCU_REPORT_EVENT_UVP_MASK;
		if (BmsEngine::m_bcuStatus1 & BCU_STATUS_OTC1) status1 |= BCU_REPORT_EVENT_OTCP_MASK;
		if (BmsEngine::m_bcuStatus1 & BCU_STATUS_OTD1) status1 |= BCU_REPORT_EVENT_OTDP_MASK;
		if (BmsEngine::m_bcuStatus1 & BCU_STATUS_UTC1) status1 |= BCU_REPORT_EVENT_UTCP_MASK;
		if (BmsEngine::m_bcuStatus1 & BCU_STATUS_UTD1) status1 |= BCU_REPORT_EVENT_UTDP_MASK;
		if (BmsEngine::m_bcuStatus1 & BCU_STATUS_OCC1) status1 |= BCU_REPORT_EVENT_OCCP_MASK;
		if (BmsEngine::m_bcuStatus1 & BCU_STATUS_OCD1) status1 |= BCU_REPORT_EVENT_OCDP_MASK;
		dataBuf[0] = status1;

		uint8_t status4 = 0;//
//		if (BmsEngine::m_bcuStatus4 & BCU_STATUS_FAN) status4 |= 0x01;
		if (BmsEngine::Flag::ESS_FAN_Warning == 1)  status4 |= 0x01;
		if (BmsEngine::Flag::OT1_PCS_Positive == 1) status4 |= 0x02;
		if (BmsEngine::Flag::OT1_PCS_Negative == 1) status4 |= 0x04;
		if (BmsEngine::Flag::OT1_HVDC_Positive == 1) status4 |= 0x08;
		if (BmsEngine::Flag::OT1_HVDC_Negative == 1) status4 |= 0x10;
		if (BmsEngine::Flag::OT1_Fuse == 1) status4 |= 0x20;
		if (BmsEngine::Flag::OT1_TA == 1) status4 |= 0x40;
		if (BmsEngine::m_bcuStatus4 & BCU_STATUS_IV1) status4 |= 0x80;
		dataBuf[1] = status4;
	
		uint8_t status2 = 0;//
		if (BmsEngine::m_bcuStatus2 & BCU_STATUS_OV2) status2 |= BCU_REPORT_EVENT_OVP_MASK;
		if (BmsEngine::m_bcuStatus2 & BCU_STATUS_UV2) status2 |= BCU_REPORT_EVENT_UVP_MASK;
		if (BmsEngine::m_bcuStatus2 & BCU_STATUS_OTC2) status2 |= BCU_REPORT_EVENT_OTCP_MASK;
		if (BmsEngine::m_bcuStatus2 & BCU_STATUS_OTD2) status2 |= BCU_REPORT_EVENT_OTDP_MASK;
		if (BmsEngine::m_bcuStatus2 & BCU_STATUS_UTC2) status2 |= BCU_REPORT_EVENT_UTCP_MASK;
		if (BmsEngine::m_bcuStatus2 & BCU_STATUS_UTD2) status2 |= BCU_REPORT_EVENT_UTDP_MASK;
		if (BmsEngine::m_bcuStatus2 & BCU_STATUS_OCC2) status2 |= BCU_REPORT_EVENT_OCCP_MASK;
		if (BmsEngine::m_bcuStatus2 & BCU_STATUS_OCD2) status2 |= BCU_REPORT_EVENT_OCDP_MASK;
		dataBuf[2] = status2;

		uint8_t status5 = 0;//
		if (BmsEngine::Flag::HVIL1_Error == 1) status5 |= 0x01;
		if (BmsEngine::Flag::OT2_PCS_Positive == 1) status5 |= 0x02;
		if (BmsEngine::Flag::OT2_PCS_Negative == 1) status5 |= 0x04;
		if (BmsEngine::Flag::OT2_HVDC_Positive == 1) status5 |= 0x08;
		if (BmsEngine::Flag::OT2_HVDC_Negative == 1) status5 |= 0x10;
		if (BmsEngine::Flag::OT2_Fuse == 1) status5 |= 0x20;
		if (BmsEngine::m_bcuStatus5 & BMU_STATUS_ADCFAULT) status5 |= 0x40;
		if (BmsEngine::Flag::HVIL2_Error == 1) status5 |= 0x80;

		dataBuf[3] = status5;
	
		uint8_t status3 = 0;//
		if (BmsEngine::m_bcuStatus3 & BCU_STATUS_OV3) status3 |= BCU_REPORT_EVENT_OVP_MASK;
		if (BmsEngine::m_bcuStatus3 & BCU_STATUS_UV3) status3 |= BCU_REPORT_EVENT_UVP_MASK;
		if (BmsEngine::m_bcuStatus3 & BCU_STATUS_OTC3) status3 |= BCU_REPORT_EVENT_OTCP_MASK;
		if (BmsEngine::m_bcuStatus3 & BCU_STATUS_OTD3) status3 |= BCU_REPORT_EVENT_OTDP_MASK;
		if (BmsEngine::m_bcuStatus3 & BCU_STATUS_UTC3) status3 |= BCU_REPORT_EVENT_UTCP_MASK;
		if (BmsEngine::m_bcuStatus3 & BCU_STATUS_UTD3) status3 |= BCU_REPORT_EVENT_UTDP_MASK;
		if (BmsEngine::m_bcuStatus3 & BCU_STATUS_OCC3) status3 |= BCU_REPORT_EVENT_OCCP_MASK;
		if (BmsEngine::m_bcuStatus3 & BCU_STATUS_OCD3) status3 |= BCU_REPORT_EVENT_OCDP_MASK;
		dataBuf[4] = status3;

		uint8_t status6 = 0;//
		if (BmsEngine::Flag::COAT3_PackAmbient == 1)  
		{
			status6 |= 0x01;
		}
		
		if (BmsEngine::Flag::DOAT3_PackAmbient == 1)  
		{
			status6 |= 0x02;
		}
		
		if (BmsEngine::Flag::CUAT3_PackAmbient == 1)  
		{
			status6 |= 0x04;
		}
		
		if (BmsEngine::Flag::DUAT3_PackAmbient == 1)  
		{
			status6 |= 0x08;
		}
		
		if (BmsEngine::m_bcuStatus6 & BCU_STATUS_SELFTESTFAULT) 
		{
			status6 |= 0x40;
		}
		
//		if (FailSafePOR_Flag == 1)		
//		{
//			status6 |= 0x40;
//		}
	
		dataBuf[5] = status6;
		
		uint8_t status7 = 0; //

		if (BmsEngine::m_bcuStatus & EMS_STATUS_MBUCAN_FAULT)  status7 |= 0x01;
		
		if (BmsEngine::m_bcuStatus & CURRENT_STATUS_CAN_FAULT)  status7 |= 0x04;
		if (BmsEngine::m_bcuStatus & BCU_STATUS_CMCAN_FAULT)  status7 |= 0x08;
		
		if (BmsEngine::Flag::ESS_PreChargeRelayFault == 1)  status7 |= 0x10;
		if (BmsEngine::Flag::ESS_HV_MainRelayFault == 1) status7 |= 0x20;
		if (BmsEngine::Flag::ESS_HV_NegRelayFault == 1) status7 |= 0x40;
		if (BmsEngine::Flag::ESS_BreakerFault == 1) status7 |= 0x80;

//		BmsEngine::BCUCANMessageProtectionComm::Shunt_Communication = 1;
//		BmsEngine::BCUCANMessageProtectionComm::BMU_Communication = 1;	
		
//		if (BmsEngine::BCUCANMessageProtectionComm::Shunt_Communication == 1)				 status7 &= 0xFB;		
//																																						else  status7 |= 0x04;		
//		if (BmsEngine::BCUCANMessageProtectionComm::BMU_Communication == 1)				 status7 &= 0xF7;		
//																																						else  status7 |= 0x08;		
		dataBuf[6] = status7;
	
#if defined(USING_SAE_J1850_CRC8)

		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0;
		for (int i = 0 ; i < BCU_STATUS_CHKSUM_INDEX ; i++)
		{
			dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i];
		}
#endif
		//
		int canEmsRet=pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
		wait_us(BCU_CAN_DELAYTIME);
//		if (canEmsRet == 0)	
//		{
////			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
//			while (true)
//			{
//				wait_us(BCU_CAN_DELAYTIME);
//				//
//				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
//				if (canEmsRet == 1)
//					break;
//			}
//
//		}
	}
	
	//0x8XXYY44
	void BatterySys::BCUCANMessageIO()
	{
		char dataBuf[8] = { 0 };//
		uint32_t id = BCUCAN_IO_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16);//
		uint8_t status1 = 0;//
		if (RLY_Precharge == RLY_ON)	status1 |= 0x01;
		if (RLY_HV_Main == RLY_ON)		status1 |= 0x02;
		if (RLY_HV_Neg == RLY_ON)		status1 |= 0x04;
		if (RLY_NFB == NFB_ON)		status1 |= 0x08;
	
		if (RLY_FB_Precharge == RLY_OFF)        status1 |= 0x10;
		if (RLY_FB_HV_Main == RLY_OFF)        status1 |= 0x20;
		if (RLY_FB_HV_Neg == RLY_OFF)        status1 |= 0x40;
		if (RLY_FB_NFB == NFB_OFF)        status1 |= 0x80;
		dataBuf[0] = status1;
	
		uint8_t status2 = 0x00;//
		//if (BmsEngine::Flag::HVIL1_Error == 1) 
		if (HV1_Emergency == 1)
		{
			status2 |= 0x01;	
		}
		if (HV2_Insulation == 1)
		{
			status2 |= 0x02;	
		}

		dataBuf[1] = status2;
	
		uint8_t status3 = 0x00;//
		if (RLY_K5_FAN == RLY_ON)		  status3 |= 0x01;

		if (RLY_FB_FAN == RLY_OFF)        status3 |= 0x10;

		dataBuf[2] = status3;          	
	
		uint8_t status4 = 0x00;//
		if (LED_Status1 == 1) status4 |= 0x01;	
		dataBuf[3] = status4;
	
		dataBuf[4] = 0x00;
		dataBuf[5] = 0x00;
		
		uint8_t status7 = 0;//
		if (BmsEngine::Flag::COAT1_PackAmbient == 1)  
		{
			status7 |= 0x01;
		}
		
		if (BmsEngine::Flag::DOAT1_PackAmbient == 1)  
		{
			status7 |= 0x02;
		}
		
		if (BmsEngine::Flag::CUAT1_PackAmbient == 1)  
		{
			status7 |= 0x04;
		}
		
		if (BmsEngine::Flag::DUAT1_PackAmbient == 1)  
		{
			status7 |= 0x08;
		}
		
		if (BmsEngine::Flag::ADCWarning == 1)  
		{
			status7 |= 0x40;
		}
		
		dataBuf[6] = status7;	
	
#if defined(USING_SAE_J1850_CRC8)

		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0;
		for (int i = 0 ; i < BCU_STATUS_CHKSUM_INDEX ; i++)
		{
			dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i];
		}
#endif
		//
		int canEmsRet=pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
		wait_us(BCU_CAN_DELAYTIME);
//		if (canEmsRet == 0)	
//		{
////			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
//			while (true)
//			{
//				wait_us(BCU_CAN_DELAYTIME);
//				//
//				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
//				if (canEmsRet == 1)
//					break;
//			}
//
//		}
	}
	
	//0x8XXYY45
	void BatterySys::BCUCANMessageCapacity()
	{
		char dataBuf[8] = { 0 };//
		uint32_t id = BCUCAN_CAPACITY_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16); //
		uint16_t RMC_Capacity = (float) (BmsEngine::coulombCount / COULOMB_CONVERSION_MA); //
		//SOC::FCCcoulombCount = 100000*COULOMB_CONVERSION_mA;
		//uint32_t temp11 = (uint32_t)BmsEngine::m_designCapacity*COULOMB_CONVERSION_mA;  
		//float temp22 = (float)((float)SOC::FCCcoulombCount / (float) temp11)*(float)1000;  
		
		//uint32_t tmp =
		//(float)((float)SOC::FCCcoulombCount/(uint32_t)BmsEngine::m_designCapacity*COULOMB_CONVERSION_mA)*(float)1000;
////		float soc_f = (float)m_remainCapacity / (float)m_fullChargeCapacity;
////		float soh_f = (float)m_fullChargeCapacity / (float)m_designCapacity;
		
		uint16_t SOH_Capacity = (float)(SOC::FCCcoulombCount / COULOMB_CONVERSION_MA); 
		
		dataBuf[0] = (uint16_t)RMC_Capacity;
		dataBuf[1] = (uint16_t)RMC_Capacity >> 8; 
		dataBuf[2] = (uint16_t)BmsEngine::m_fullChargeCapacity;
		dataBuf[3] = (uint16_t)BmsEngine::m_fullChargeCapacity >> 8;
		dataBuf[4] = BmsEngine::m_soh;  					       
		dataBuf[5] = BmsEngine::m_soh >> 8;   				//SOH = FCC / DC  = 100 %          
		uint8_t InitStatus = 0x00;//
		if (BmsEngine::m_bcuState != BCUStateType_t::BCU_INIT)  
		{
			BmsEngine::SBCUCANMessageCapacity::Initialed = 1; 
			
			if (BmsEngine::m_bcuStatus & (uint16_t)BCU_STATUS_CMCAN_FAULT)
				BmsEngine::SBCUCANMessageCapacity::BMUinitialed = 0;
			else
				BmsEngine::SBCUCANMessageCapacity::BMUinitialed = 1;
			
			if (BmsEngine::m_bcuStatus & CURRENT_STATUS_CAN_FAULT)
				BmsEngine::SBCUCANMessageCapacity::ShuntInitialed = 0;
			else
				BmsEngine::SBCUCANMessageCapacity::ShuntInitialed = 1;
				
		}
				
		
		if (BmsEngine::SBCUCANMessageCapacity::Initialed == 1)				 InitStatus |= 0x01;						
		if (BmsEngine::SBCUCANMessageCapacity::BMUinitialed == 1)			 InitStatus |= 0x02;		
		if (BmsEngine::SBCUCANMessageCapacity::ShuntInitialed == 1)			 InitStatus |= 0x04;		
		if (BmsEngine::SBCUCANMessageCapacity::InsulationInitialed == 1)	 InitStatus |= 0x08;				


		if (BmsEngine::SBCUCANMessageCapacity::ChargeEnable == 1)			 InitStatus |= 0x10;		
		if (BmsEngine::SBCUCANMessageCapacity::DischargeEnable == 1)		 InitStatus |= 0x20;		
		if (BmsEngine::SBCUCANMessageCapacity::FullCharge == 1)				 InitStatus |= 0x40;
		if (BmsEngine::SBCUCANMessageCapacity::FullDischarge == 1)			 InitStatus |= 0x80;
		dataBuf[6] = InitStatus;

#if defined(USING_SAE_J1850_CRC8)

		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0;
		for (int i = 0 ; i < BCU_STATUS_CHKSUM_INDEX ; i++)
		{
			dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i];
		}
#endif
		//
		int canEmsRet=pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
		wait_us(BCU_CAN_DELAYTIME);
//		if (canEmsRet == 0)	
//		{
////			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
//			while (true)
//			{
//				wait_us(BCU_CAN_DELAYTIME);
//				//
//				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
//				if (canEmsRet == 1)
//					break;
//			}
//
//		}
	}
	

	//0x8XX0104
	void BatterySys::CanReportBmuERRCNT(uint16_t function_cmd)
	{
		char dataBuf[8] = { 0 };//
		uint32_t id = CANREPORT_BMUERRCNT_BASE | ((uint32_t)pBattSys->p_bmsParam->m_bcuId << 16); //
		
		dataBuf[0] = function_cmd;
		dataBuf[1] = 0x00;
		dataBuf[2] = 0x00;
		dataBuf[3] = 0x00;
		dataBuf[4] = 0x00;                               
		dataBuf[5] = 0x00;
		dataBuf[6] = 0x00;

#if defined(USING_SAE_J1850_CRC8)

		dataBuf[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(dataBuf), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0;
		for (int i = 0 ; i < BCU_STATUS_CHKSUM_INDEX ; i++)
		{
			dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i];
		}
#endif
		//
		int canCmRet=pBattSys->p_canCm->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));	
		if (canCmRet == 0)	
		{
			//for (int i = 0; i < BCU_CAN_Repeat_number; i++)
			while (true)
			{
				wait_us(600);
				//
				int canCmRet = pBattSys->p_canCm->write(CANMessage(id, dataBuf, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));	
				if (canCmRet == 1)
					break;
			}
		}
	}



	//mbu_canReportBcuStatus: 
	void BatterySys::BCU_canReportBcuStatus()
	{
		BCUCANMessageDataMain();				//0x8XXYY40
		BCUCANMessageCellVoltage();			//0x8XXYY41
		BCUCANMessageCellTemperature();		//0x8XXYY42
		BCUCANMessageProtection();				//0x8XXYY43
		BCUCANMessageIO();						//0x8XXYY44
		BCUCANMessageCapacity();				//0x8XXYY45

	}
}
