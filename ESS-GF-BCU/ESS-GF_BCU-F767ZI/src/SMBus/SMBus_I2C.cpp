#include <mbed.h>
#include "SMBus_I2C.h"
#include "BatterySys.h"

namespace bmstech
{
	I2C i2csmbus(I2C4_SDA, I2C4_SCL);            // SDA, SCL
	
	void SMBusI2C::SMBusI2C_init()
	{
		i2csmbus.frequency(SMBus_I2C_BAUD_100K);
	}
	int SMBusI2C::SMBusI2C_read(char address, char com)
	{		
		cmd[0] = com;
	
		i2csmbus.write(address, cmd, 1, 1);
		i2csmbus.read((address | 0x01), data, 3, 0);
		
		crc8[0] = address;
		crc8[1] = com;
		crc8[2] = address|0x01;
		crc8[3] = data[0];
		crc8[4] = data[1];
		crc8_check = BatterySys::get_CRC8_Poly_0X07((uint8_t*)crc8, 5);
		
		temp = data[0] | data[1] << 8;
		return temp ;
	}
	

} //namespace bmstech
