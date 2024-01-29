#if !FEATURE_LWIP
//#error [NOT_SUPPORTED] LWIP not supported for this target
#endif

#include <mbed.h>
#include "BmsEngine.h"

#include "bcu.h"
#include "VersionInfo.h"
#include "SEGGER_RTT.h"
#include "ModbusMaster.h"

#include "BackupSram.h"

#define RTT_ENABLE	1

//for BmsEngine
#define BMS_ENABLE

//#define ZQWL_BOARD
#define HOPE_BOARD

using namespace bmstech;

CANMessage cmCanRxMsg;
static int cmCanCount = 0;
int tempCount = 0;
int voltCount = 0;

CANMessage chgCanRxMsg;
int chgCanCount = 0;

bool g_cmCanFlag{ false };


#if defined( BMS_ENABLE )

//instanciation & init BmsEngine (static members)
//===============================================================================

BmsEngine *ptr_bms = new BmsEngine();

//BmsEngine Class static member init:
BCUStateType_t	BmsEngine::m_bcuState = BCUStateType_t::BCU_INIT; //enum class
BCUStateType_t	BmsEngine::m_preState = BCUStateType_t::BCU_INIT;
RELAYStatusType_t BmsEngine::m_status = RELAYStatusType_t::RELAY_OFF;
BCUDriveType_t	BmsEngine::m_bcuDrive = BCUDriveType_t::STANDBY;
volatile BCUModeChangeType_t BmsEngine::m_bcuModeChange = BCUModeChangeType_t::NONE;

BmsEngine* BmsEngine::p_bmspara = ptr_bms;

uint8_t BmsEngine::m_relay = RLY_ALL_OFF;
uint32_t BmsEngine::count = 0;
uint32_t BmsEngine::fsmCount = 0;
uint16_t BmsEngine::m_bcuStatus = BCU_STATUS_OK;
bool BmsEngine::m_protect_enable = false;
int BmsEngine::m_timeoutCount = 0;
int BmsEngine::m_oldTimeoutCount = 0;

uint16_t BmsEngine::m_dsgAccum = DEFAULT_PACK_FCC - DEFAULT_PACK_RMC;
float BmsEngine::coulombCount = (BmsEngine::m_dsgAccum) * COULOMB_CONVERSION;
uint16_t BmsEngine::m_remainCapacity = DEFAULT_PACK_RMC; //600: 60.0Ah
uint16_t BmsEngine::m_fullChargeCapacity = DEFAULT_PACK_FCC; //750 (75.0Ah:0.1Ah)
uint16_t BmsEngine::m_designCapacity = DEFAULT_PACK_DC; // 750: 75.0Ah
uint16_t BmsEngine::m_soc = 600; //60.0% 
uint16_t BmsEngine::m_soh = 1000; //100.0% 

bool BmsEngine::m_low_bank = true; //Lower bank battery check (ID:1~10)
volatile uint8_t BmsEngine::m_bcuId = 0x00; //init to invalid ID (1~16)

											   /// <summary>
											   /// Load derfault BCU Protection parameters:
											   /// </summary>
//uint16_t BmsEngine::m_ovpVoltage = DEFAULT_OVP_VALUE;	//1mV
//uint16_t BmsEngine::m_uvpVoltage = DEFAULT_UVP_VALUE;	//1mV
//uint16_t BmsEngine::m_cotpTemp = DEFAULT_COTP_VALUE;		//0.1C
//uint16_t BmsEngine::m_dotpTemp = DEFAULT_DOTP_VALUE;		//0.1C
//uint16_t BmsEngine::m_cutpTemp = DEFAULT_CUTP_VALUE;		//0.1C
//uint16_t BmsEngine::m_dutpTemp = DEFAULT_DUTP_VALUE;		//0.1C		//0.1C
//uint16_t BmsEngine::m_cocpCurrent = DEFAULT_COCP_VALUE * 10;	//0.1A
//uint16_t BmsEngine::m_docpCurrent = DEFAULT_DOCP_VALUE * 10;	//0.1A
//uint16_t BmsEngine::m_scpCurrent = DEFAULT_SCP_VALUE * 10;	//0.1A (RS485: 1A)
//
//uint16_t BmsEngine::m_crevCurrent = DEFAULT_CREV_VALUE * 10;	//0.1A 
//uint16_t BmsEngine::m_drevCurrent = DEFAULT_DREV_VALUE * 10;	//0.1A  
//uint16_t BmsEngine::m_sbyOcpCurrent = DEFAULT_SBY_OCP_VALUE * 10;	//0.1A  

#if defined(DIODE_PROTECTION)
uint16_t BmsEngine::m_diodeOtpTemp = DEFAULT_DIODE_OTP_VALUE * 10;			//0.1C => 0.01C
uint16_t BmsEngine::m_diodeOtpRelease = DEFAULT_DIODE_OTP_RELEASE * 10;	//0.1C => 0.01C
#endif

																			//DENNIS: using static member to assure memory allocation
uint16_t BmsEngine::bms_memory[BMS_MEMORY_LENGTH] = { 0 };

#if defined(DS1302_SOC_STORAGE)
DS1302* BmsEngine::p_rtcDs1302 = pRTC;
#endif

BmsEngine::rs485CommFault BmsEngine::m_rs485CommFlags = { 0 };

#endif //if defined( BMS_ENABLE )



//DigitalOut g_LED(LED1);
//DigitalOut g_LED1((PinName)hbLED1); //Active Low
//DigitalOut g_LED2((PinName)hbLED2);
//DigitalOut g_LED3((PinName)hbLED3);

//EthernetInterface eth;



#if defined(HOPE_BOARD)
CAN *p_cmCan1 = new CAN((PinName)CAN1_RXD, (PinName)CAN1_TXD); //for CMs CAN Bus
CAN *p_chgCan2 = new CAN((PinName)CAN2_RXD, (PinName)CAN2_TXD); //for MBU CAN Bus
#elif defined(ZQWL_BOARD)
CAN *p_cmCan1 = new CAN(PD_0, PD_1); //for CMs CAN Bus
CAN *p_chgCan2 = new CAN(PB_5, PB_6); //for MBU CAN Bus
#endif


//CAN can1(PD_0, PD_1);	//CAN (Rx,Tx)
//CAN can2(PB_5, PB_6);	



void static cm_canMessageParse()
{
	p_cmCan1->read(cmCanRxMsg); //read can message to clear Rx interrupt

	cmCanCount++;
	g_cmCanFlag = true;

	//0x000008xx: Temperature message:
	if ((cmCanRxMsg.id & 0x1FFFFF00) == 0x00000800)
	{
		tempCount++;
	}
	else if ((cmCanRxMsg.id & 0x1FFFFF00) == 0x00000900)
	{
		voltCount++;
	}

}

void static chg_canMessageParse()
{
	p_chgCan2->read(cmCanRxMsg); //read can message to clear Rx interrupt

	chgCanCount++;
	
}

typedef void(*callback_function)(void); // type for conciseness

//p_canCm->reset(); //DO NOT reset() 2017/10/17

void canInit( CAN *ptr_can, int baud, callback_function parser)
{
	ptr_can->frequency(baud);
	ptr_can->attach(parser, CAN::RxIrq); //Hook-up CAN Rx ISR to static message parser canTest()
}

void canTest()
{
	canInit(p_cmCan1, 250000, cm_canMessageParse);
	canInit(p_chgCan2, 250000, chg_canMessageParse);
}

char cmd[8] = { 0x00, 0x00, 0x55, 0xAA, 0xFF, 0x00, 0xFF, 0x00 };
int sendCount = 0;

//Can bus commands  
//================================================================
int canSendCommand( CAN *ptr_can, int canId)
{

	sendCount++;

	//Extended ID
	//ID field: (Slave_ID <<16) | 0x09 (CMD_ExcuteApp: 0x0800_3000)
	//Slave_ID=0: Broadcasting
	cmd[1]= lowByte(sendCount);
	cmd[0] = highByte(sendCount);
	
	return (ptr_can->write(CANMessage(canId, cmd, 8, CANData, CANExtended)));
}

#if defined(HOPE_BOARD)
Serial rs232(PA_9, PA_10); //tx, rx 發射, 接收

RawSerial rs485(PD_5, PD_6); //PD_7: TX_EN	UART2, note: using RawSerial
DigitalOut rs485_TxEn(PD_7);
#elif defined(ZQWL_BOARD)
Serial rs232(PA_9, PA_10); //tx, rx 發射, 接收

RawSerial rs485(PD_5, PD_6); //PD_7: TX_EN	UART2, note: using RawSerial
DigitalOut rs485_TxEn(PD_4);
#endif



ModbusMaster master;	// instantiate ModbusMaster object

void mosbusMasterInit(uint8_t id, int baud)
{

}

void writeLoop()
{
	static uint32_t i;
	uint8_t result;
	
	i++;

	// set word 0 of TX buffer to least-significant word of counter (bits 15..0)
	master.setTransmitBuffer(0, lowWord(i));

	// set word 1 of TX buffer to most-significant word of counter (bits 31..16)
	master.setTransmitBuffer(1, highWord(i));

	master.setTransmitBuffer(0, 0x55AA);
	// slave: write TX buffer to (2) 16-bit registers starting at register 0
	result = master.writeMultipleRegisters(0, 1);


}

void readLoop()
{
	uint8_t j, result;
	uint16_t data[6];

	// slave: read (6) 16-bit registers starting at register 2 to RX buffer
	result = master.readHoldingRegisters(2, 6);

	// do something with data if read is successful
	if (result == master.ku8MBSuccess)
	{
		for (j = 0; j < 6; j++)
		{
			data[j] = master.getResponseBuffer(j);
		}
	}
}

//enable RS485 Tx Enable
void rs485EnableTx()
{
	rs485_TxEn = 1;
}

//Disable RS485 Tx Enable & Enable Rx
void rs485EnableRx()
{
	wait_us(1000);
	rs485_TxEn = 0; //RX enable
}



//DigitalOut g_LED(LED1);



//Allocate BKPSRAM memory: sBKPSRAM defined in BackupSram.h
//sBKPSRAM  *BKPSRAM = (sBKPSRAM *)BKPSRAM_BASE;


uint16_t count{ 0 };
int16_t write_arr = 0, read_arr = 0;
uint32_t lastMode = 0;

int main() 
{
	g_LED1 = 1;
	g_LED2 = 1;
	g_LED3 = 1;

	rs232.baud(115200);
	rs485.baud(9600);	// use rs485 (port 2); initialize Modbus communication baud rate	
	master.begin(2, rs485); // communicate with Modbus slave ID 2 over Serial (UART2)
	master.preTransmission(rs485EnableTx);
	master.postTransmission(rs485EnableRx);

	canTest();

	enable_backup_sram();
	//__HAL_RCC_BKPSRAM_CLK_ENABLE();
	

	//
	//__HAL_RCC_BKPSRAM_CLK_ENABLE();
	ifDefault();
	//__HAL_RCC_BKPSRAM_CLK_DISABLE();

	//backupData.a = 0;
	//backupData.b = 0;
	//backupData.c = 0;

	//set_time(1256729737);  // Set RTC time to Wed, 28 Oct 2009 11:35:37

#if defined(RTT_ENABLE)
	dumpVersionInfo();

#endif 

#if defined(RTT_ENABLE)
	//SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
	SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);

	SEGGER_RTT_printf(0, "CPU SystemCoreClock is %d Hz\r\n", SystemCoreClock);
	/*
	SEGGER_RTT_printf(0, "Version: %s \n\r", versionString);
	SEGGER_RTT_printf(0, "Compile Date & Time: %s \r\n", timestamp);

	SEGGER_RTT_printf(0, "==============================================\r\n");
	SEGGER_RTT_printf(0, "Mbed Version: %u \r\n", mbedVersion);
	SEGGER_RTT_printf(0, "Protocol Version: %u \r\n", bcuProtocolVersion);
	SEGGER_RTT_printf(0, "BCU Hardware Version: %u \r\n", bcuHardwareVersion);
	SEGGER_RTT_printf(0, "BCU Firmware Version: %u \r\n", bcuFirmwareVersion);
	SEGGER_RTT_printf(0, "Build Number: %u \r\n", buildNumber); //auto build number
	SEGGER_RTT_printf(0, "Epoch buildTimeStamp: %u \r\n", buildTimeStamp); //epoch timestamp
	SEGGER_RTT_printf(0, "==============================================\r\n\n");
	*/

#endif


	for (;;)
	{
#if defined (BMS_ENABLE)
		ptr_bms->m_soc++;
#endif
		//p_bmspara->count;
		//BmsEngine::count++;

		g_LED1 = 0;
		wait_ms(50);
		g_LED1 = 1;

		wait_ms(100);

		if (g_cmCanFlag) g_LED3 = 0; //cmCAN Rx data
		wait_ms(50);
		g_LED3 = 1;
		g_cmCanFlag = false;

		wait_ms(1000);

		printf("Test\n\r");
		rs232.printf("Count Value: %d \n\r", count);
		SEGGER_RTT_printf(0, "Count value is: %d \r\n", count);

		time_t seconds = time(NULL);

		SEGGER_RTT_printf(0,"Time as seconds since January 1, 1970 = %d\r\n", seconds);

		//SEGGER_RTT_printf(0,"Time as a basic string = %s", ctime(&seconds));

		//char buffer[32];
		//strftime(buffer, 32, "%I:%M %p\n", localtime(&seconds));
		//SEGGER_RTT_printf(0,"Time as a custom formatted string = %s", buffer);

		 
	
		count++;

#if 0
		enable_bkpsram_access();
		
		read_arr = BKPSRAM->counter++;
		lastMode = BKPSRAM->LastModeTest;

		disable_bkpsram_access();
		

		rtc_write_backup_reg(RTC_BKP_DR0, read_arr);
		write_arr = rtc_read_backup_reg(RTC_BKP_DR0);
#endif

		//CAN Write Test:
		canSendCommand(p_cmCan1, 0x100);
		canSendCommand(p_chgCan2, 0x200);

		//RS485 Master test loop:
//		writeLoop();
	}
}