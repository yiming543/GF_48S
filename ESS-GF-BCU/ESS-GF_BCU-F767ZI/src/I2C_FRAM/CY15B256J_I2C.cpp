//File
#include <mbed.h>
#include "CY15B256J_I2C.h"
#include "SOC.h"
#include "BmsEngine.h"


//
//extern DigitalIn DIP_B0;
//extern DigitalIn DIP_B1;
//extern DigitalIn DIP_B2;
//extern DigitalIn DIP_B3;

namespace bmstech
{
    I2C i2c1(I2C1_SDA, I2C1_SCL);           // SDA, SCL
	
    //ID_READ
	void FRAMI2C::FRAMI2C_ID_Check()
	{
		BmsEngine::m_bcuId = frami2c.FRAM_READ(BCU_ID_ADDRESS);	
		if (BmsEngine::m_bcuId == 0xFF)
		{
			BmsEngine::m_bcuId = 0x01;
		}
//		FRAMI2C::FRAM_DATA  = frami2c.read_device_id(0x00);
//		uint16_t ManufacturerID = FRAMI2C::FRAM_DATA >> 8;
//		if (ManufacturerID == (Device_ID >> 8))
//		{
//			BmsEngine::m_bcuId = frami2c.FRAM_READ(BCU_ID_ADDRESS);	
//			//BcuCmdErrorType_t::BCU_SET_CMD_OK
//		}

	}
	
	//FCC_READ
	void FRAMI2C::FRAMI2C_SOH_READ()
	{

		BmsEngine::m_soh = FRAMI2C::FRAM_READDoubleByte(FRAM_SOH_ADDRESS); //READ 
		if (BmsEngine::m_soh >= 1000)
		{
			BmsEngine::m_soh = 1000;
			
		}

		BmsEngine::m_fullChargeCapacity = BmsEngine::m_designCapacity * BmsEngine::m_soh/10/100;

	}

	//ID_SET
    void FRAMI2C::FRAMI2C_ID_SET()
	{
		if (frami2c.changeID_flag == 1)
		{
			frami2c.FRAM_WRITE(BCU_ID_ADDRESS, frami2c.newBcuId);
			wait_ms(5);
			uint8_t storedBcuId = frami2c.FRAM_READ(BCU_ID_ADDRESS);	//READ storedBcuId
			if (storedBcuId == frami2c.newBcuId)
			{
				BmsEngine::m_bcuId = storedBcuId;

			}
			else
			{
			
				//BCU EEPROM ERROR
			}
			frami2c.changeID_flag = 0;
		}			

	}
	
	//SOH_SET
	void FRAMI2C::FRAMI2C_SOH_SET()
	{
		if (SOC::StoredSOH_Flag == 1)
		{
			FRAMI2C::FRAM_WriteDoubleByte(FRAM_SOH_ADDRESS, BmsEngine::m_soh);
			wait_ms(5);
			uint16_t storedSOH = FRAMI2C::FRAM_READDoubleByte(FRAM_SOH_ADDRESS); //READ storedSOH
			if (storedSOH == BmsEngine::m_soh)
			{
				if (SOC::SET_StoredSOH_Flag == 1)
				{
					//	FCC = SOH * DC;
					BmsEngine::m_fullChargeCapacity = ((BmsEngine::m_soh / 10) * BmsEngine::m_designCapacity) / 100;  
					SOC::SET_StoredSOH_Flag = 0;
				}
				BmsEngine::m_soh = storedSOH;
				
				uint16_t SOC_temp = (float)((float)BmsEngine::coulombCount  / (float)BmsEngine::m_fullChargeCapacity * COULOMB_CONVERSION_MA);
				SOC_temp = (uint16_t)(SOC_temp * 1000.0);
				BmsEngine::m_soc = (uint16_t)SOC_temp;
			}	
			else
			{
			
				//BCU EEPROM ERROR
			}
			SOC::StoredSOH_Flag = 0;
		}			
//		else if (SOC::SET_StoredSOH_Flag == 1)
//		{
//			FRAMI2C::FRAM_WriteDoubleByte(FRAM_SOH_ADDRESS, BmsEngine::m_soh);
//			wait_ms(5);
//			uint16_t storedSOH = FRAMI2C::FRAM_READDoubleByte(FRAM_SOH_ADDRESS); //READ storedSOH
//			if (storedSOH == BmsEngine::m_soh)
//			{
//				BmsEngine::m_soh = storedSOH;
//				
//				//	FCC = SOH * DC;
//				BmsEngine::m_fullChargeCapacity = ((BmsEngine::m_soh / 10) * BmsEngine::m_designCapacity) / 100;  
//					
//				//				BmsEngine::m_fullChargeCapacity = (uint16_t)(SOC::FCCcoulombCount / COULOMB_CONVERSION_MA);
//				//				uint32_t m_DC_temp = (uint32_t)BmsEngine::m_designCapacity * COULOMB_CONVERSION_MA;  
//				//				float soh_temp = (float)((float)SOC::FCCcoulombCount / (float)m_DC_temp)*(float)1000;  
//				//				BmsEngine::m_soh = (uint16_t)soh_temp;
//				//				uint16_t FCC_temp = BmsEngine::m_fullChargeCapacity / COULOMB_CONVERSION_MA;
//				//				FCC_mA = (float)((float)BmsEngine::m_fullChargeCapacity* COULOMB_CONVERSION_MA);
//				uint16_t SOC_temp = (float)((float)BmsEngine::coulombCount  / (float)BmsEngine::m_fullChargeCapacity * COULOMB_CONVERSION_MA);
//				SOC_temp = (uint16_t)(SOC_temp * 1000.0);
//				BmsEngine::m_soc = (uint16_t)SOC_temp;
//			}	
//			else
//			{
//			
//				//BCU EEPROM ERROR
//			}
//			SOC::SET_StoredSOH_Flag = 0;
//		}
	}
	
    //FRAMI2C_init
    void FRAMI2C::FRAMI2C_init()
    {
	    i2c1.frequency(FRAM_I2C_BAUD_400K);
    }

    //read_device_id
//    uint32_t FRAMI2C::Read_device_id(char* Device_id)
//    {
//	    char Slave_Address1 = FRAM_I2C_ADDRESS;	//init
//
//	    i2c1.write(0xF8U, &Slave_Address1, 1, true);
//	    i2c1.read(0xF9U, Device_id, 3, 0);
//
//	    return (Device_id[0] << 16U | Device_id[1] << 8U | Device_id[2]);
//	    
//    }

    //FRAM_READ
	uint16_t FRAMI2C::FRAM_READ(uint16_t address)
    {
        char byte_address[2];	//init
	    char data = 0x00U;		//init

        byte_address[0] = (address >> 8U) & 0xFFU;		//MSB
        byte_address[1] = (address >> 0U) & 0xFFU;		//LSB

	    i2c1.write(FRAM_I2C_ADDRESS, byte_address, 2U, true);
	  
	    i2c1.read((FRAM_I2C_ADDRESS | 0x01U), &data, 1);
	 
	    return data;
    }



	//FRAM_WRITE
	void FRAMI2C::FRAM_WRITE(int16_t address, uint8_t data)
    {
		char byte_address[3];	//init

        byte_address[0] = (address >> 8) & 0xFF;		//MSB
        byte_address[1] = (address >> 0) & 0xFF;		//LSB
	    byte_address[2] = data ;
	    i2c1.write(FRAM_I2C_ADDRESS, byte_address, 3, false);
//	    i2c1.write(FRAM_I2C_ADDRESS, byte_address, 2, true);

    }
	
	//FRAM_READ
	uint16_t FRAMI2C::FRAM_READDoubleByte(uint16_t address)
	{
		char byte_address[2]; //init
		char data[2]; 

		byte_address[0] = (address >> 8) & 0xFF; //MSB
		byte_address[1] = (address >> 0) & 0xFF; //LSB

		i2c1.write(FRAM_I2C_ADDRESS, byte_address, 2, true);

		i2c1.read((FRAM_I2C_ADDRESS | 0x01), data, 2, false); 
	
		uint16_t udata = (uint16_t)((uint16_t)data[0] << 8 | data[1]); 

		return udata;
			
	}
	
	//FRAM_WriteDoubleByte
	void FRAMI2C::FRAM_WriteDoubleByte(uint16_t address, uint16_t data)
	{
		char byte_address[4]; //init

		byte_address[0] = ((address >> 8) & 0xFF);
		byte_address[1] = ((address >> 0) & 0xFF);
		byte_address[2] = ((data >> 8) & 0xFF);
		byte_address[3] =  ((data >> 0) & 0xFF);
		i2c1.write(FRAM_I2C_ADDRESS, byte_address, 4, 0);

	}

	//FRAMI2C::fill
//    void FRAMI2C::Fill(uint32_t address, uint8_t data, uint32_t length)
//    {
//        char byte_address[2];	//init
//        // char i2c_adrs = (_address | ((address >> 15) & 0x02));
//
//        byte_address[0] = ((address >> 8U) & 0xFFU);
//        byte_address[1] = ((address >> 0U) & 0xFFU);
//	    i2c1.write(FRAM_I2C_ADDRESS, byte_address, 2, true);
//	    //length
//        for (uint32_t i = 0U; i < length; i++)
//        {
//	        i2c1.write(data);
//        }
//    }

//	void FRAMI2C::SETSOC_TIME_init()
//	{
//		uint8_t Timetemp = frami2c.FRAM_READ(SET_SOC_TIME_ADDRESS);	
//		if ((Timetemp <= MAXSetTimehr)&&(Timetemp >= MINSetTimehr))
//		{
//			SOC::Timeout6hr_Counter = (uint32_t)Timetemp * 60 * 60;	
//			FRAMI2C::Init_storedTIME_Sec = (uint32_t)Timetemp * 60 * 60;
//		}
//		else
//		{
//			SOC::Timeout6hr_Counter = Time6hr;	
//			FRAMI2C::Init_storedTIME_Sec = Time6hr;	
//		}					
//
//	}
} //namespace bmstech
