//File
#pragma once
#include <mbed.h>

//extern bool_t TBalComm_Flag;
//extern uint16_t bankIndex;

namespace bmstech
{
//#define Num_ChargeMode	0x11
//#define Num_DischargeMode	0x01
//#define Num_NormalMode	0x00
//	
//#define Passive_timeout_0m	0x00			//0 min
//#define Passive_timeout_05m 0x01			//0.5 min
//#define Passive_timeout_1m	0x02		//1 min
//#define Passive_timeout_2m	0x03		//2 min
//#define Passive_timeout_5m	0x06		//5 min
//#define Passive_timeout_10m	0x07		//10 min
//#define Passive_timeout_15m	0x08		//15 min
	
//#define PASSIVE_IV	5		//mV max_Volt-min_Volt
const uint16_t PASSIVE_IV = 5;
#define PASSIVE_MINVOLT	3300		//mV
#define PASSIVE_MAXVOLT 3700
//const uint16_t PASSIVE_MINVOLT = 2600;		//mV
//const uint16_t PASSIVE_MAXVOLT = 3100;
const uint16_t OFFSET_VOLT = 1;
//#define OFFSET_VOLT 1
	
//#define PASSIVE_CYCLE_TIME		300		//1min=60sec=60000ms 60000ms/500ms=120  60000ms/200ms=300
//#define PASSIVE_STARTUP_TIME	270		//1.5s*36=54s  0.2s*270=54s

//#define Active_IV	20		//mV
//#define Active_minVolt	3500		//mV
//#define Active_maxVolt   4100

	class Balance
	{
	public:
		
		static uint8_t Cellnum;
		
		//static uint8_t MaxCell_num;
		static uint16_t CellVolt_max(uint8_t packid);
		static uint8_t MaxPackVolt_id_NUM;
		

		static void BalanceVoltageSort(const uint16_t AVG_volt);
		
		static void PassiveBalancejudge();
		
		static uint16_t Average_voltReading;
		
		//static uint8_t MinCell_num;
		static uint16_t CellVolt_min(uint8_t packid);
		static uint8_t MinPackVolt_id_NUM;
	

		//static uint16_t CellVolt_max(uint8_t packid, uint8_t mode);
		static uint16_t PackVolt_max();
		//static uint16_t CellVolt_min(uint8_t packid, uint8_t mode);
		static uint16_t PackVolt_min();
		
		static int16_t CellTemp_max(uint8_t packid);
		static int16_t CellTemp_min(uint8_t packid);
		//static int16_t CellTemp_max(uint8_t packid, uint8_t mode);
		//static int16_t CellTemp_min(uint8_t packid, uint8_t mode);
		static uint16_t PackTemp_max();

		static uint16_t PackTemp_min();
		
		static bool_t PassiveBalancejudge_flag;
		static bool_t PassiveBalanceCommand_ON_flag;
		
		static bool_t BCU_SET_Manual_Balance_flag;
		static bool_t BCU_SET_Semi_Automatic_Balance_flag;
		
		static void PassiveBalanceProcess(const uint16_t max_volt, const uint16_t min_volt);

		// Do nothing
		~Balance() {
			// Do nothing
		}
	};
} //namespace bmstech
