#pragma once
#include <mbed.h>

	
namespace bmstech {
	
#define SMBus_I2C_BAUD_100K	(100000) 

#define Device_Address	0x16
	//#define ComCode	0x0E
#define Voltage	0x09
#define Current	0x0a
#define RelativeStateOfCharge	0x0d
#define CellVoltage1	0x3f
#define CellVoltage2	0x3e
#define CellVoltage3	0x3d
#define CycleCount	0x17
	
//	class SMBusI2C2 : public I2C
//	{
//
//	};

	class SMBusI2C //:public I2C
	{
	public:
		 char cmd[4];
		 char crc8[5];
		 char data[3];
		 char crc8_check;
		 uint16_t temp;
		
		char crc8t_check;
		char crc8t[5];
		
		void SMBusI2C_init();
	
		int SMBusI2C_read(char address, char com);


		 ~SMBusI2C() {
			// Do nothing
		}

	};

} //namespace bmstech

