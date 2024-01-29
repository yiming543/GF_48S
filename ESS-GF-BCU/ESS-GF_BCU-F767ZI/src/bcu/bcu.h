//File
#pragma once

#include <mbed.h>

//PLATFORM: NUCLEO STM32F756ZG BOARD 
//HSE: 216MHz
//mbed Target: NUCLEO STM32F756ZG
//=====================================================================
//主控MCU			STM32F756ZG - LQFP144			Flash:1024KB RAM:319KB 216MHz
//以太网芯片			DP83848 - LQFP48
//串口				1个RS232接口	UART1				PA9:UART1_TX, PA10:UART1_RX
//RS485接口			1个RS485接口	UART2				PD5:UART2_TX, PD6:UART2_RX, PD7:RS485 Tx EN: Act High
//CAN接口			2个CAN接口 CAN1/CAN2				CAN1:PD0(Rx) PD1(Tx) / CAN2:PB5(Rx) PB6(Tx)
//TF卡				9脚小卡，						由PC与PD的引脚共同控制
//无线接口			P3 引出NRF24L01接口
//USB接口			一个主USB 一个从USB
//存储EEPROM			24C02							PB8-I2C1-SCL, PB9-I2CI-SDA
//25Q64存储芯片		25Q64							PE3-FLASH-CS(LOW), PC2-SPI2-MISO, B10-SPI2-SCK, PC3-SPI2-MOSI
//温度传感器接口		引出DS18B20传感器接口				P7:18B20, PE2-DQ
//
//JTAG接口			对应JLINK 的JTAG (J-LINK V10: Good for Upgrade V6.23C)
//
//按键				K1、K2、K3 三个用户按键			SW1:PE_10, SW2:PE_11, SW3:PE_12
//指示灯				LED1、LED2、LED3 三个用户指示灯	PE_13, PE_14, PE_15
//====================================================================
//====================================================================
//RS485 Port: UART2
//PD6 - USART2 - RX
//PD7 - 485    - DE (Low:Rx, High:Tx) Normal Low
//PD5 - USART2 - TX


	
//enum HopePinName
//{
	//HOPE_F407VE Board (LED1/LED2/LED3), 2 CAN, RS232 x 1, RS485 x 1
//	PN_LED1 = PE_13,
//	PN_LED2 = PE_14,
//	PN_LED3 = PE_15,
//
//	PN_LED4 = PN_LED1,
//	PN_LED5 = PN_LED1,

//	PN_CAN1_RX = PB_12, 	// PD0-CAN1-RX
//	PN_CAN1_TX = PB_13, 	// PD1-CAN1-TX
//	PN_CAN2_RX = PB_5,	// PB5_CAN2_RX
//	PN_CAN2_TX = PB_6,	// PB6_CAN2_TX

//	PN_CAN2_RX = PB_8,		//JY 20190722
//	PN_CAN2_TX = PB_9,		//JY 20190722
//
//	PN_RS485_TX = PD_5,
//	PN_RS485_RX = PD_6,
//	PN_RS485_TXE = PD_7, //HIGH: TX, LOW: RX
//
//	PN_RS232_TX = PA_9,	//Act Low
//	PN_RS232_RX = PA_10,
//
//	PN_SW1 = PE_10,
//	PN_SW2 = PE_11,
//	PM_SW3 = PE_12,	//Act Low
//};

//typedef struct
//{
//	uint32_t    isDefault;           //must by 0x12345678
//	uint32_t    LastTestNumber;
//	uint32_t    LastUserNumber;
//	uint32_t    LastModeTest;
//	uint32_t    calibv;
//	uint32_t    calibc;
//	uint32_t    WorkTime;
//	int32_t     RTCCalib;
//	uint32_t    LCDContrast;
//} sBKPSRAM_t;

//DENNIS: RS485 Master Error Statistics
//enum MasterCommType_t : uint8_t
//{
//	RELAY_COMM = 0,
//	DSG_COMM = 1,
//	CHG_COMM = 2
//}
//;

//enum myPinName: uint8_t
//{
//	CAN2_RXD = PB_8, 			//JY 20190722
//	CAN2_TXD = PB_9,		//JY 20190722
//
//	CAN1_RXD = PB_12,		//JY 20190722
//	CAN1_TXD = PB_13,		//JY 20190722
//
//	PN_RS485_TX = PD_5,
//	PN_RS485_RX = PD_6,
//	PN_RS485_TXE = PD_7, //HIGH: TX, LOW: RX

//	RS485_TXD = PD_5,
//	RS485_RXD = PD_6,
//	RS485_TXEN = PD_7,
//
//	RS232_TXD = PA_9,
//	RS232_RXD = PA_10,
//
//	hbLED1 = PE_13,
//	hbLED2 = PE_14,
//	hbLED3 = PE_15
//};

//#define TRACE_LENGTH 16
//struct mbMasterComm_t {
//	uint32_t totalCount;
//	uint32_t errorCount;
//	uint32_t timeoutCount;
//	uint8_t errorCode[TRACE_LENGTH];
//	uint8_t ptr;
//}
//;

//DENNIS: RS485 Slave Error Statistics
//struct rs485Comm_t {
//	uint32_t totalCount;
//	uint32_t noiseCount;
//	uint32_t idleCount;
//}
//;

//DENNIS: CAN BUS Error Statistics
struct CanError_t
{
	uint32_t canTickSec; 	//unit: sec (time elapse for seconds)
	uint32_t packetErrRate; 	//unit: PPM (0.001%)
	uint32_t canTotalCount;

	uint32_t canError_Chksum;
	uint32_t canError_Pid;
	uint32_t canError_Index;
	uint32_t canError_TempId;

	uint32_t sensor_OverRange;
};

#define	CAN_WIRE_CHECK_ENABLE	(1)		//enable CAN wire broken check: 2017/7/7
#define CAN_LOSS_PROTECTION_ENABLE (1)	//enable CAN Packet Loss Protection

//Modbus Master Read/Write Control:
//===========================================================
//#define READ_METER
#define READ_CHARGE_METER
#define READ_DISCHARGE_METER
#define WRITE_RELAY_MODULE

//#define METER_DELAY	(25)	//msec: 20 (25: 2017/7/7)
//#define METER_READ_DELAY_INSERTION	(1)
//#undef METER_READ_DELAY_10MS

#define MODBUS_MASTER_FLUSH	(1)

//RS485 Modbus Slave Delay Time
//=========================================================
#define PRE_TX_DELAY	(0)	//US: 10
#define POST_TX_DELAY	(200)	//US: 100US is good for 115200

#define RS232_RESETTING	(1)	//for Modbus Master Resetting
							//Working around 2017/7/5

//#define _MULTI_THREAD	(1) //undefine this for single thread control process

//TEST Behaviour Control:
//#define	_MODBUS_MASTER_DEBUG	(1)		//disable thjis for nomal program

//#define DI_MODULE_DEBUG	(1)
//#undef DI_MODULE_DEBUG

//#define SIMPLE_MODBUS_MASTER	(1)
//Simple Modbus Master
//=====================================================

//#define polling 200 // the scan rate
//#define retry_count 10
//#define TxEnablePin 2
//#define LED 13

//enum {
//	PACKET1,
//	PACKET2,
//	TOTAL_NO_OF_PACKETS // leave this last entry
//}
//;
