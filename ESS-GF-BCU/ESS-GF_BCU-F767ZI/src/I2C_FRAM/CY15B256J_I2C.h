//File
#pragma once
#include <mbed.h>



namespace bmstech
{

//#define FRAM_I2C_BAUD_400K	(400000)
//
//#define FRAM_I2C_ADDRESS  (0xA0)
//#define I2C_WRITE           0
//#define I2C_READ            1
//
//#define BCU_ID_ADDRESS  0x00A5
//#define SET_SOC_TIME_ADDRESS  0x00B0
//
//#define DEVICE_ID   0x4231
//
//protection
//#define BCU_OV2_ADDRESS  0x0F01
//#define BCU_OV3_ADDRESS  0x0F02
//const uint8_t I2C_WRITE = 0U;
//const uint8_t I2C_READ = 1U;
//const uint16_t DEVICE_ID = 0x4231U;
	
#define FRAM_SOH_ADDRESS  (0xB0)	
	
//const uint16_t SET_SOC_TIME_ADDRESS = 0x00B0U;
const uint32_t FRAM_I2C_BAUD_400K = 400000U;
const uint8_t FRAM_I2C_ADDRESS = 0xA0U;

const uint16_t BCU_ID_ADDRESS = 0x00A5U;

	
    class FRAMI2C 
    {
    public:

//	    static int32_t FRAM_DATA;
//	    static uint32_t Init_storedTIME_Sec;
//	    static uint32_t READFRAM_TIME_Hr;
//      uint32_t Read_device_id(char* device_id);
		static void FRAM_WriteDoubleByte(uint16_t address, uint16_t data);
//		void Fill(uint32_t address, uint8_t data, uint32_t length);
	    static bool changeID_flag;
	    static uint8_t newBcuId;
        static void FRAMI2C_init();
	    static void FRAMI2C_ID_SET();
	    static void FRAMI2C_ID_Check();

	    static uint16_t  FRAM_READ(uint16_t address);

	    static void FRAM_WRITE(int16_t address, uint8_t data);
	    static uint16_t FRAM_READDoubleByte(uint16_t address);
	    static void FRAMI2C_SOH_SET();
	    
	    static void FRAMI2C_SOH_READ();
	    ~FRAMI2C() {
		    // Do nothing
	    }
        private:


    };
    extern	FRAMI2C frami2c;	//extern
	
} //namespace bmstech
