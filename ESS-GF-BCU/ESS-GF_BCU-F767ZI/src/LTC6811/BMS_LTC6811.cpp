//File
#include <mbed.h>
#include "LTC6811_daisy.h"
#include "BatterySys.h"
#include "Balance.h"
#include "ADC_Temperature.h"

#include "ADBMS6815_daisy.h"
#include "adBms6815CmdList.h"

extern DigitalOut Green_LED;	//extern
//extern DigitalOut Blue_LED;
//extern DigitalOut Red_LED;
extern bmstech::LTC6811SPI::Cell_asic bms_ic[bmstech::LTC6811SPI::TOTAL_IC];

extern bmstech::ADBMS6815::cell_asic IC[bmstech::LTC6811SPI::TOTAL_IC];		
//
//extern DigitalOut CS; // ssel

namespace bmstech
{	
	//Start Cell ADC Measurement	
	void LTC6811SPI::Start_ADC_Measurement()
	{
		LTC6811SPI::LTC681x_adcv(LTC6811SPI::ADC_CONVERSION_MODE, LTC6811SPI::ADC_DCP, LTC6811SPI::CELL_CH_TO_CONVERT);
	}
	
	//Read Cell Voltage Registers
	void LTC6811SPI::Read_Cell_Voltage()
	{
		error = LTC6811SPI::LTC6811_rdcv(0, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back all cell voltage registers
		
		//current_ic = MAX_PACK_NUM
		for (uint16_t current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
		{
			//cell_channels
			for (uint16_t i = 0; i < bms_ic[0].ic_reg.cell_channels; i++)
			{					
				if ((bms_ic[current_ic].cells.pec_match[0] == 1) || (bms_ic[current_ic].cells.pec_match[1] == 1)
						|| (bms_ic[current_ic].cells.pec_match[2] == 1) || (bms_ic[current_ic].cells.pec_match[3] == 1))
				{
					LTC6811SPI::Volt_PEC_Err++; 					
					LTC6811SPI::communication_Volt_counter_Flag = 1;

					if (BmsEngine::m_bcuStatus & BCU_STATUS_CMCAN_FAULT)
					{
						Volt_CH[i] = 1; //unit:1mV
					}					
				}
				else
				{					
					Volt_CH[i] = static_cast<uint16_t>(bms_ic[current_ic].cells.c_codes[i] / 10); //unit:1mV
					
					if (LTC6811SPI::Volt_PEC_Err <= 0)
					{
						LTC6811SPI::Volt_PEC_Err = 0;
					}
					else
					{
						LTC6811SPI::Volt_PEC_Err--; 	
					}

				}									
			}					
			
			
			//MAX_CELL_NUM
			for (uint16_t i = 0; i < PackConst::MAX_CELL_NUM; i++)
			{
				BatterySys::pack[current_ic].cellVolt[i] = Volt_CH[i];	
			}	
		}		
	}
		
	// Write byte data I2C Communication on the GPIO Ports
	void LTC6811SPI::SwitchADG729_Channel(uint8_t I2C_address, uint8_t D1)
	{
		/************************************************************
			Ensure to set the GPIO bits to 1 in the CFG register group.  
		*************************************************************/   
		const uint8_t A0 = 0x60 | ((I2C_address >> 4) & 0x0F);		// Icom Start (6) + I2C_address D0
		const uint8_t A1 = (((I2C_address) << 4) & 0xF0) | 0x08;	//                  Fcom ACK from slave(8)
		const uint8_t A2 = 0x00 | ((D1 >> 4) & 0x0F);				// Icom Blank (0) + eeprom address(word address) D1
		const uint8_t A3 = ((D1 << 4) & 0xF0) | 0x09;				//                  Fcom master NACK + Stop(9)
//		uint8_t A4 = (0x60 | ((I2C_address >> 4) & 0x0F));
//		uint8_t A5 = ((((I2C_address | 0x01) << 4) & 0xF0) | 0x08);
		
		//current_ic = MAX_PACK_NUM
		for (uint8_t current_ic = 0U; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
		{
			//Communication control bits and communication data bytes. Refer to the data sheet.  						
			//(Write operation to set the word address)
			bms_ic[current_ic].com.tx_data[0] = A0; // Icom Start (6) + I2C_address D0 (C0)
			bms_ic[current_ic].com.tx_data[1] = A1; //                  Fcom ACK from slave(8)  
			bms_ic[current_ic].com.tx_data[2] = A2; // Icom Blank (0) + eeprom address(word address) D1 (0x90)
			bms_ic[current_ic].com.tx_data[3] = A3; //                  Fcom master NACK + Stop(9) 
//			bms_ic[current_ic].com.tx_data[4] = A4; // Icom Start (6) + I2C_address D2 (0xC1)(Read operation)
//			bms_ic[current_ic].com.tx_data[5] = A5; //                  Fcom ACK from slave(7) ? 	
			
//			bms_ic[current_ic].com.tx_data[0] = 0x68; // Icom Start(6) + I2C_address D0 (0xA0)
//			bms_ic[current_ic].com.tx_data[1] = 0xA8; // Fcom master NACK(8)
//			bms_ic[current_ic].com.tx_data[2] = 0x00; // Icom Blank (0) + eeprom address D1 (0x00)
//			bms_ic[current_ic].com.tx_data[3] = 0x18; // Fcom master NACK(8)
//			bms_ic[current_ic].com.tx_data[4] = 0x01; // Icom Blank (0) + data D2 (0x13)
//			bms_ic[current_ic].com.tx_data[5] = 0x39; // Fcom master NACK + Stop(9)
		}

		LTC6811SPI::LTC681x_wrcomm(LTC6811SPI::TOTAL_IC, bms_ic); // write to comm register 

		LTC6811SPI::LTC681x_stcomm(2); // data length=3 // initiates communication between master and the I2C slave
		
//		error = LTC6811SPI::LTC681x_rdcomm(LTC6811SPI::TOTAL_IC, bms_ic); // read from comm register    
	}
	
	
	// Read byte data I2C Communication on the GPIO Ports
	void LTC6811SPI::LTC6811_temperature(uint8_t I2C_address,uint8_t D1)
	{
		/************************************************************
			Ensure to set the GPIO bits to 1 in the CFG register group.  
		*************************************************************/   
		const uint8_t A0 = 0x60 | ((I2C_address >> 4) & 0x0F);		// Icom Start (6) + I2C_address D0 (C0)
		const uint8_t A1 = (((I2C_address) << 4) & 0xF0) | 0x08;	//                  Fcom ACK from slave(8)
		const uint8_t A2 = 0x00 | ((D1 >> 4) & 0x0F);				// Icom Blank (0) + eeprom address(word address) D1
		const uint8_t A3 = ((D1 << 4) & 0xF0) | 0x08;				//                  Fcom ACK from slave(8)
		const uint8_t A4 = 0x60 | ((I2C_address >> 4) & 0x0F);		// Icom Start (6) + I2C_address D2 (0xC1)(Read operation)
		const uint8_t A5 = (((I2C_address | 0x01) << 4) & 0xF0) | 0x08;	 //                  Fcom ACK from slave(8) ?
		
		//current_ic = MAX_PACK_NUM
		for (uint8_t current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
		{
			//Communication control bits and communication data bytes. Refer to the data sheet.  						
			//(Write operation to set the word address)
			bms_ic[current_ic].com.tx_data[0] = A0;		// Icom Start (6) + I2C_address D0 (C0)
			bms_ic[current_ic].com.tx_data[1] = A1;		//                  Fcom ACK from slave(8)  
			bms_ic[current_ic].com.tx_data[2] = A2;		// Icom Blank (0) + eeprom address(word address) D1 (0x90)
			bms_ic[current_ic].com.tx_data[3] = A3;		//                  Fcom ACK from slave(8)
			bms_ic[current_ic].com.tx_data[4] = A4;		// Icom Start (6) + I2C_address D2 (0xC1)(Read operation)
			bms_ic[current_ic].com.tx_data[5] = A5;     //                  Fcom ACK from slave(8) ? 	
		}
		
		LTC6811SPI::LTC681x_wrcomm(LTC6811SPI::TOTAL_IC, bms_ic); // write to comm register 
	
		LTC6811SPI::LTC681x_stcomm(3); // data length=3 // initiates communication between master and the I2C slave
		
		//current_ic = MAX_PACK_NUM
		for (uint8_t current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
		{ 
			//Communication control bits and communication data bytes. Refer to the data sheet.       
			bms_ic[current_ic].com.tx_data[0] = 0x0F; // Icom Blank (0) + data D0 (FF)
			bms_ic[current_ic].com.tx_data[1] = 0xF0; //                  Fcom master ACK(0)  master NACK(8)
			bms_ic[current_ic].com.tx_data[2] = 0x0F; // Icom Blank (0) + data D1 (FF)
			bms_ic[current_ic].com.tx_data[3] = 0xF0; //                  Fcom master ACK(0)  master NACK(8)
			bms_ic[current_ic].com.tx_data[4] = 0x0F; // Icom Blank (0) + data D2 (FF)
			bms_ic[current_ic].com.tx_data[5] = 0xF9; //                  Fcom master NACK + Stop(9)
		}  		
	
		LTC6811SPI::LTC681x_wrcomm(LTC6811SPI::TOTAL_IC, bms_ic); // write to comm register
	
		LTC6811SPI::LTC681x_stcomm(3); // data length=1 // initiates communication between master and the I2C slave

		error = LTC6811SPI::LTC681x_rdcomm(LTC6811SPI::TOTAL_IC, bms_ic); // read from comm register                
	
		//current_ic = MAX_PACK_NUM
		for (uint16_t current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
		{
			//cell_channels
			for (uint8_t i = 0; i < bms_ic[0].ic_reg.cell_channels; i++)
			{
				if (bms_ic[current_ic].com.rx_pec_match == 1)		//(error == -1)	check pec error  ic[c_ic].com.rx_pec_match = 1;
				{
					LTC6811SPI::com_PEC_Err++; 					
					LTC6811SPI::communication_Temp_counter_Flag = 1;
//					BmsEngine::FaultCount = 0;

					if (BmsEngine::m_bcuStatus & BCU_STATUS_CMCAN_FAULT)
					{
						Temp_CH[i] = 1;
						LTC6811SPI::com_PEC_Err_Flag = 1;
					}

				}
				else
				{
					if (LTC6811SPI::com_PEC_Err <= 0)
					{
						LTC6811SPI::com_PEC_Err = 0;
					}
					else
					{
						LTC6811SPI::com_PEC_Err--; 	
					}
						
				}
							
			}				
			
		}
			
		
//		for (int current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++) 
//		{
//			if (bms_ic[current_ic].com.rx_pec_match == 1)		//(error == -1)	//check pec error  ic[c_ic].com.rx_pec_match = 1;
//			{
//				for (int i = 0; i < bms_ic[0].ic_reg.cell_channels; i++) 
//				{
////					Temp_CH[i] = 1;
//					LTC6811SPI::com_PEC_Err++; 
//					
//				}
//				
//			}
//		}
			
//		if (LTC6811SPI::com_PEC_Err_Flag == 0)
//		if (bms_ic[aaa].com.rx_pec_match == 0)
		if (LTC6811SPI::communication_Temp_counter_Flag == 0)
		{
			for (uint16_t current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
			{

				if (D1 == TEMP_CH_IDX1)
				{
					Temp_CH[0] = ((bms_ic[current_ic].com.rx_data[0] << 8) & 0x0F00)
							   | ((bms_ic[current_ic].com.rx_data[1]) & 0x00F0)
							   | ((bms_ic[current_ic].com.rx_data[2]) & 0x000F);
					Temp_CH[1] = ((bms_ic[current_ic].com.rx_data[3] << 4) & 0x0F00)
							   | ((bms_ic[current_ic].com.rx_data[4] << 4) & 0x00F0)
							   | ((bms_ic[current_ic].com.rx_data[5] >> 4) & 0x000F);
					
					if ((Temp_CH[0] & 0x0800) == 0)
					{
						BatterySys::pack[current_ic].cellTemp[0] = Temp_CH[0];   		
					}
					else if ((Temp_CH[0] & 0x0800) != 0)
					{
						Temp_CH[0] = (int16_t)(Temp_CH[0] | 0xF000); //12bit-->16bit
						BatterySys::pack[current_ic].cellTemp[0] = Temp_CH[0];
					}
					
					if ((Temp_CH[1] & 0x0800) == 0)
					{
						BatterySys::pack[current_ic].cellTemp[1] = Temp_CH[1];   		
					}
					else if ((Temp_CH[1] & 0x0800) != 0)
					{
						Temp_CH[1] = (int16_t)(Temp_CH[1] | 0xF000); //12bit-->16bit
						BatterySys::pack[current_ic].cellTemp[1] = Temp_CH[1];
					}					
					
				
				}
				else if (D1 == TEMP_CH_IDX2)
				{			
					Temp_CH[2] = ((bms_ic[current_ic].com.rx_data[0] << 8) & 0x0F00)
							   | ((bms_ic[current_ic].com.rx_data[1]) & 0x00F0)
							   | ((bms_ic[current_ic].com.rx_data[2]) & 0x000F);
					Temp_CH[3] = ((bms_ic[current_ic].com.rx_data[3] << 4) & 0x0F00)
							   | ((bms_ic[current_ic].com.rx_data[4] << 4) & 0x00F0)
							   | ((bms_ic[current_ic].com.rx_data[5] >> 4) & 0x000F);

					if ((Temp_CH[2] & 0x0800) == 0)
					{
						BatterySys::pack[current_ic].cellTemp[2] = Temp_CH[2];   		
					}
					else if ((Temp_CH[2] & 0x0800) != 0)
					{
						Temp_CH[2] = (int16_t)(Temp_CH[2] | 0xF000); //12bit-->16bit
						BatterySys::pack[current_ic].cellTemp[2] = Temp_CH[2];
					}
					
					if ((Temp_CH[3] & 0x0800) == 0)
					{
						BatterySys::pack[current_ic].cellTemp[3] = Temp_CH[3];   		
					}
					else if ((Temp_CH[3] & 0x0800) != 0)
					{
						Temp_CH[3] = (int16_t)(Temp_CH[3] | 0xF000); //12bit-->16bit
						BatterySys::pack[current_ic].cellTemp[3] = Temp_CH[3];
					}
					
					
				}
				else if (D1 == TEMP_CH_IDX3)
				{
					Temp_CH[4] = ((bms_ic[current_ic].com.rx_data[0] << 8) & 0x0F00)
							   | ((bms_ic[current_ic].com.rx_data[1]) & 0x00F0)
							   | ((bms_ic[current_ic].com.rx_data[2]) & 0x000F);
					Temp_CH[5] = ((bms_ic[current_ic].com.rx_data[3] << 4) & 0x0F00)
							   | ((bms_ic[current_ic].com.rx_data[4] << 4) & 0x00F0)
							   | ((bms_ic[current_ic].com.rx_data[5] >> 4) & 0x000F);
		
					if ((Temp_CH[4] & 0x0800) == 0)
					{
						BatterySys::pack[current_ic].cellTemp[4] = Temp_CH[4];   		
					}
					else if ((Temp_CH[4] & 0x0800) != 0)
					{
						Temp_CH[4] = (int16_t)(Temp_CH[4] | 0xF000); //12bit-->16bit
						BatterySys::pack[current_ic].cellTemp[4] = Temp_CH[4];
					}
					
					if ((Temp_CH[5] & 0x0800) == 0)
					{
						BatterySys::pack[current_ic].cellTemp[5] = Temp_CH[5];   		
					}
					else if ((Temp_CH[5] & 0x0800) != 0)
					{
						Temp_CH[5] = (int16_t)(Temp_CH[5] | 0xF000); //12bit-->16bit
						BatterySys::pack[current_ic].cellTemp[5] = Temp_CH[5];
					}
					
				}
				else if (D1 == TEMP_CH_IDX4)
				{
					Temp_CH[6] = ((bms_ic[current_ic].com.rx_data[0] << 8) & 0x0F00)
							   | ((bms_ic[current_ic].com.rx_data[1]) & 0x00F0)
							   | ((bms_ic[current_ic].com.rx_data[2]) & 0x000F);
					Temp_CH[7] = ((bms_ic[current_ic].com.rx_data[3] << 4) & 0x0F00)
							   | ((bms_ic[current_ic].com.rx_data[4] << 4) & 0x00F0)
							   | ((bms_ic[current_ic].com.rx_data[5] >> 4) & 0x000F);
		
					if ((Temp_CH[6] & 0x0800) == 0)
					{
						BatterySys::pack[current_ic].cellTemp[6] = Temp_CH[6];   		
					}
					else if ((Temp_CH[6] & 0x0800) != 0)
					{
						Temp_CH[6] = (int16_t)(Temp_CH[6] | 0xF000); //12bit-->16bit
						BatterySys::pack[current_ic].cellTemp[6] = Temp_CH[6]; 
					}
					
					if ((Temp_CH[7] & 0x0800) == 0)
					{
						BatterySys::pack[current_ic].cellTemp[7] = Temp_CH[7];   		
					}
					else if ((Temp_CH[7] & 0x0800) != 0)
					{
						Temp_CH[7] = (int16_t)(Temp_CH[7] | 0xF000); //12bit-->16bit
						BatterySys::pack[current_ic].cellTemp[7] = Temp_CH[7];
					}
					
				}
				else if (D1 == TEMP_CH_IDX5)	//PACK TEMP	
				{
					Temp_CH[8] = ((bms_ic[current_ic].com.rx_data[0] << 8) & 0x0F00)
							   | ((bms_ic[current_ic].com.rx_data[1]) & 0x00F0)
							   | ((bms_ic[current_ic].com.rx_data[2]) & 0x000F);
					Temp_CH[9] = ((bms_ic[current_ic].com.rx_data[3] << 4) & 0x0F00)
							   | ((bms_ic[current_ic].com.rx_data[4] << 4) & 0x00F0)
							   | ((bms_ic[current_ic].com.rx_data[5] >> 4) & 0x000F);
		
					if ((Temp_CH[8] & 0x0800) == 0)   
					{
						BatterySys::pack[current_ic].packTemp[0] = Temp_CH[8];   		
					}
					else if ((Temp_CH[8] & 0x0800) != 0) 
					{
						Temp_CH[8] = (int16_t)(Temp_CH[8] | 0xF000); //12bit-->16bit
						BatterySys::pack[current_ic].packTemp[0] = Temp_CH[8];
					}
					
					if ((Temp_CH[9] & 0x0800) == 0)   
					{
						BatterySys::pack[current_ic].packTemp[1] = Temp_CH[9];   		
					}
					else if ((Temp_CH[9] & 0x0800) != 0)    
					{
						Temp_CH[9] = (int16_t)(Temp_CH[9] | 0xF000); //12bit-->16bit
						BatterySys::pack[current_ic].packTemp[1] = Temp_CH[9];
					}
					
				}
				else if (D1 == TEMP_CH_IDX6)	//PACK TEMP	
				{
					Temp_CH[10] = ((bms_ic[current_ic].com.rx_data[0] << 8) & 0x0F00)
								| ((bms_ic[current_ic].com.rx_data[1]) & 0x00F0)
								| ((bms_ic[current_ic].com.rx_data[2]) & 0x000F);
					Temp_CH[11] = ((bms_ic[current_ic].com.rx_data[3] << 4) & 0x0F00)
								| ((bms_ic[current_ic].com.rx_data[4] << 4) & 0x00F0)
								| ((bms_ic[current_ic].com.rx_data[5] >> 4) & 0x000F);
			
					if ((Temp_CH[10] & 0x0800) == 0)
					{
						BatterySys::pack[current_ic].packTemp[2] = Temp_CH[10]; //PACK TEMP	
					}
					else if ((Temp_CH[10] & 0x0800) != 0)    
					{
						Temp_CH[10] = (int16_t)(Temp_CH[10] | 0xF000); //12bit-->16bit
						BatterySys::pack[current_ic].packTemp[2] = Temp_CH[10]; 
					}
					
					if ((Temp_CH[11] & 0x0800) == 0)   
					{
						BatterySys::pack[current_ic].packTemp[3] = Temp_CH[11];   		
					}
					else if ((Temp_CH[11] & 0x0800) != 0)   
					{
						Temp_CH[11] = (int16_t)(Temp_CH[11] | 0xF000); //12bit-->16bit
						BatterySys::pack[current_ic].packTemp[3] = Temp_CH[11];
					}
					
				}
				else
				{
				
				}



			}
		}
		

		
	}
	
	//Ensure to set the GPIO bits to 1 in the CFG register group.
	void LTC6811SPI::LTC6811_i2c_commmand(uint8_t I2C_address, uint8_t D1)
	{
		/************************************************************
			Ensure to set the GPIO bits to 1 in the CFG register group.  
		*************************************************************/     
		const uint8_t C0 = 0x60 | ((I2C_address >> 4) & 0x0F);		// Icom Start (6) + I2C_address D0 (C0)
		const uint8_t C1 = (((I2C_address) << 4) & 0xF0) | 0x08;	//                  Fcom ACK from slave(8)
		const uint8_t C2 = 0x00 | ((D1 >> 4) & 0x0F);				// Icom Blank (0) + eeprom address(word address) D1
		const uint8_t C3 = ((D1 << 4) & 0xF0) | 0x08;				//                  Fcom ACK from slave(8)
		const uint8_t C4 = 0x60 | ((I2C_address >> 4) & 0x0F);		// Icom Start (6) + I2C_address D2 (0xC1)
		const uint8_t C5 = (((I2C_address | 0x01) << 4) & 0xF0) | 0x08;	//                  Fcom ACK from slave(8)
		
		//current_ic = MAX_PACK_NUM
		for (uint8_t current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
		{
			//Communication control bits and communication data bytes. Refer to the data sheet.    
			//(Write operation to set the word address)
			bms_ic[current_ic].com.tx_data[0] = C0; // Icom Start (6) + I2C_address D0 (C0)
			bms_ic[current_ic].com.tx_data[1] = C1; //                  Fcom ACK from slave(8)
			bms_ic[current_ic].com.tx_data[2] = C2; // Icom Blank (0) + eeprom address(word address) D1 (0x90)
			bms_ic[current_ic].com.tx_data[3] = C3; //                  Fcom ACK from slave(8)
			bms_ic[current_ic].com.tx_data[4] = C4; // Icom Start (6) + I2C_address D2 (0xC1)(Read operation)
			bms_ic[current_ic].com.tx_data[5] = C5; //                  Fcom ACK from slave(8)
		}
	
		LTC6811SPI::LTC681x_wrcomm(LTC6811SPI::TOTAL_IC, bms_ic); // write to comm register 

		LTC6811SPI::LTC681x_stcomm(3); // data length=3 // initiates communication between master and the I2C slave

		//current_ic = MAX_PACK_NUM
		for (uint8_t current_ic = 0U; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
		{ 
			//Communication control bits and communication data bytes. Refer to the data sheet.       	
			bms_ic[current_ic].com.tx_data[0] = 0x0F; // Icom Blank (0) + data D0 (FF)
			bms_ic[current_ic].com.tx_data[1] = 0xF0; //                  Fcom master ACK(0)
			bms_ic[current_ic].com.tx_data[2] = 0x0F; // Icom Blank (0) + data D1 (FF)
			bms_ic[current_ic].com.tx_data[3] = 0xF0; //                  Fcom master ACK(0)
			bms_ic[current_ic].com.tx_data[4] = 0x0F; // Icom Blank (0) + data D2 (FF)
			bms_ic[current_ic].com.tx_data[5] = 0xF9; //                  Fcom master NACK + Stop(9)
		}  

		LTC6811SPI::LTC681x_wrcomm(LTC6811SPI::TOTAL_IC, bms_ic); // write to comm register
	
		LTC6811SPI::LTC681x_stcomm(3); // data length=1 // initiates communication between master and the I2C slave

		error = LTC6811SPI::LTC681x_rdcomm(LTC6811SPI::TOTAL_IC, bms_ic); // read from comm register                

		const uint8_t heartbeat_idx = D1 & 0xF0;	//heartbeat_idx

//		heartbeat_CH[0] = { 0 };
		//current_ic = MAX_PACK_NUM
		for (uint16_t current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
		{
			if (heartbeat_idx == HEARTBEAT)
			{
//				tmp[0] = (uint8_t)(((bms_ic[current_ic].com.rx_data[0] << 4) & 0xF0)
//						| ((bms_ic[current_ic].com.rx_data[1] >> 4) & 0x0F));
				heartbeat_CH[1] = static_cast<uint8_t>(((bms_ic[current_ic].com.rx_data[2] << 4U) & 0xF0U)
						| ((bms_ic[current_ic].com.rx_data[3] >> 4U) & 0x0FU));
				heartbeat_CH[2] = static_cast<uint8_t>(((bms_ic[current_ic].com.rx_data[4] << 4U) & 0xF0U)
						| ((bms_ic[current_ic].com.rx_data[5] >> 4U) & 0x0FU));
			
				heartbeat_CH[0] = (bms_ic[current_ic].com.rx_data[1] >> 4U) & 0x0FU;
//				heartbeat_CH[1] = tmp1;
//				heartbeat_CH[2] = tmp2;
				

				BatterySys::pack[current_ic].PackHeartbeat[0]  = heartbeat_CH[0];
				BatterySys::pack[current_ic].PackHeartbeat[1]  = heartbeat_CH[1];
				BatterySys::pack[current_ic].PackHeartbeat[2]  = heartbeat_CH[2];
			}	
			
			if (D1 == REDLED_VERSION)
			{
//				tmp[0] = (uint8_t)(((bms_ic[current_ic].com.rx_data[0] << 4) & 0xF0)
//						| ((bms_ic[current_ic].com.rx_data[1] >> 4) & 0x0F));
				heartbeat_CH[1] = static_cast<uint8_t>(((bms_ic[current_ic].com.rx_data[2] << 4U) & 0xF0U) | ((bms_ic[current_ic].com.rx_data[3] >> 4U) & 0x0FU));

//				heartbeat_CH[2] = static_cast<uint8_t>(((bms_ic[current_ic].com.rx_data[4] << 4U) & 0xF0U) | ((bms_ic[current_ic].com.rx_data[5] >> 4U) & 0x0FU));
				heartbeat_CH[2] = ((bms_ic[current_ic].com.rx_data[4] & 0x0F) | (bms_ic[current_ic].com.rx_data[5] & 0xF0)) ;
				
				heartbeat_CH[0] = (bms_ic[current_ic].com.rx_data[1] >> 4U) & 0x0FU;
				
//				SlaveTempbyte[19] = major_number & 0x0F;
//				SlaveTempbyte[20] = ((middle_number & 0x0F) << 4) | (minor_number & 0x0F);
				uint16_t BMU_SW_version = (uint16_t)(((uint16_t)heartbeat_CH[2] << 8) | heartbeat_CH[1]);
//				BatterySys::pack[current_ic].HWVer = HW_version;  
				BatterySys::pack[current_ic].SWVer = BMU_SW_version;  
				
				BatterySys::pack[current_ic].PackHeartbeat[0]  = heartbeat_CH[0];
//				BatterySys::pack[current_ic].PackHeartbeat[1]  = heartbeat_CH[1];
//				BatterySys::pack[current_ic].PackHeartbeat[2]  = heartbeat_CH[2];
			}	
			
		}
		

		
	}
	
	//LTC681x_heartbeat_commmand
	void LTC6811SPI::LTC681x_heartbeat_commmand(uint8_t I2C_address)
	{
		//LTC681x_heartbeat_commmand
		uint8_t heartbeat_command = static_cast<uint8_t>((LTC6811SPI::heartbeat_value & 0x0FU) | 0x90U);
		
		if (BatterySys::SendBMUVerFlag == 1)
		{
			heartbeat_command = REDLED_VERSION;
		
		}
		LTC6811SPI::LTC6811_i2c_commmand(I2C_address, heartbeat_command);
		
		if (BatterySys::SendBMUVerFlag == 1)
		{
			BatterySys::CANSendBMUVer_Flag = 1;
			BatterySys::SendBMUVerFlag = 0;
		}
		
		
		LTC6811SPI::heartbeat_value++;
		if (LTC6811SPI::heartbeat_value > 14U)
		{
			LTC6811SPI::heartbeat_value = 0U;
		}
	}
	

//	void LTC6811SPI::VoltageTemperature_measurement_switch()
//	{
//		switch (sw) 
//		{
//			case 0: //
//			{		
////				LTC6811SPI::Wakeup_IDLE(LTC6811SPI::TOTAL_IC, PackConst::WAKEUP_IDLE_Volt_DELAY);
//				ADBMS6815::adBmsWakeupIc(TOTAL_IC);
//				LTC6811SPI::Write_S_Control(0); //Balance OFF
//				wait_ms(1);
//				ADBMS6815::Start_ADC_Measurement();
//				wait_ms(3);	//7 kHz Conversion Time 2.3 ms
////				ADBMS6815::adBmsPollAdc(PLADC);
//				ADBMS6815::adBms6815_read_cell_voltages(TOTAL_IC, &IC[0]);
//				if (Balance::PassiveBalanceCommand_ON_flag == 1)
//				{
//					LTC6811SPI::Write_S_Control(1);
//				}
//				else
//				{
//					LTC6811SPI::Write_S_Control(0);
//				}		
//
////				LTC6811SPI::Read_Cell_Voltage();	
//			
//	
//				sw = 1;
//			}
//			break;
//			
//			case 1: //1pack ~ms
//			{			
////				LTC6811SPI::Wakeup_IDLE(LTC6811SPI::TOTAL_IC, PackConst::WAKEUP_IDLE_Temp_DELAY);
////				ADBMS6815::adBmss6815_Adax(ADBMS6815::MODE_7KHz_TO_3KHz, ADBMS6815::GPIOS_VREF2_VREF3); //GPIO1 GPIO6
////				ADBMS6815::adBmsWakeupIc(TOTAL_IC);
//				ADBMS6815::adBms6815_start_aux_voltage_conversion(TOTAL_IC, &IC[0]);
//				wait_ms(4);	//7 kHz Conversion Time 3131 μs
//				ADBMS6815::adBms6815_read_aux_voltages(TOTAL_IC, &IC[0]);
////				error = LTC6811SPI::LTC6811_rdaux(REG_ALL, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back  aux registers ALL ADBMS6815 pag.36
//		
//				for (int current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++) 
//				{				
////					PackTemp_CH[0] = bms_ic[current_ic].aux.a_codes[0] / 10;	//GPIO1 PackTemp Busbar+
////					ADCPackTemp_CH[0] = ADCC::Volt_temperatureTab(PackTemp_CH[0]);
////					BatterySys::pack[current_ic].packTemp[0] = ADCPackTemp_CH[0];								
////					
////					Temp_CH[1] = bms_ic[current_ic].aux.a_codes[6] / 10;	//GPIO6 Temp 2			
////					ADCTemp_CH[1] =  ADCC::Volt_temperatureTab(Temp_CH[1]);
////					BatterySys::pack[current_ic].cellTemp[1] = ADCTemp_CH[1];
//					for (int i = 0; i < 8; i++)
//					{
//						Temp_CH[i] = bms_ic[current_ic].aux.a_codes[i] / 10; //GPIO6 Temp 2			
//						ADCTemp_CH[i] =  ADCC::Volt_temperatureTab(Temp_CH[i]);
//						BatterySys::pack[current_ic].cellTemp[i] = ADCTemp_CH[i];
//					}
//				}
//
//				sw = 0;
//			}
//			break;
//			
//			case 2: //ms
//			{	
//				LTC6811SPI::Wakeup_IDLE(LTC6811SPI::TOTAL_IC, PackConst::WAKEUP_IDLE_Temp_DELAY);
//				LTC6811SPI::SwitchADG729_Channel(ADG729_address2, S1A_S1B);
//				wait_us(100);
//				LTC6811SPI::LTC6811_adax(LTC6811SPI::ADC_CONVERSION_MODE, AUX_CH_GPIO2); //GPIO2 GPIO7
//				wait_us(800);	//7 kHz Conversion Time 788 μs
//				error = LTC6811SPI::LTC6811_rdaux(REG_1, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back aux registers 1
//				error = LTC6811SPI::LTC6811_rdaux(REG_3, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back aux registers 3
////				error = LTC6811SPI::LTC6811_rdaux(REG_ALL, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back  aux registers ALL ADBMS1818 pag.63
//
//				for (int current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++) 
//				{
//					Temp_CH[5] = bms_ic[current_ic].aux.a_codes[1] / 10; //GPIO2 Temp 6			
//					ADCTemp_CH[5] = ADCC::Volt_temperatureTab(Temp_CH[5]);
//					BatterySys::pack[current_ic].cellTemp[5] = ADCTemp_CH[5];
//					
//					Temp_CH[8] = bms_ic[current_ic].aux.a_codes[7] / 10; //GPIO7 Temp 9		
//					ADCTemp_CH[8] =  ADCC::Volt_temperatureTab(Temp_CH[8]);
//					BatterySys::pack[current_ic].cellTemp[8] = ADCTemp_CH[8];	
//				}
//				LTC6811SPI::SwitchADG729_Channel(ADG729_address2, 0x00);
//				sw = 3;
//			}
//			break;
//
//			case 3: //ms
//			{					
//				LTC6811SPI::Wakeup_IDLE(LTC6811SPI::TOTAL_IC, PackConst::WAKEUP_IDLE_Temp_DELAY);	
//				LTC6811SPI::SwitchADG729_Channel(ADG729_address1, S2A_S2B);
//				wait_us(100);
//				LTC6811SPI::LTC6811_adax(LTC6811SPI::ADC_CONVERSION_MODE, AUX_CH_GPIO1); //GPIO1 GPIO6
//				wait_us(800); //7 kHz Conversion Time 788 μs
//				error = LTC6811SPI::LTC6811_rdaux(REG_1, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back aux registers 1
//				error = LTC6811SPI::LTC6811_rdaux(REG_3, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back aux registers 3
////				error = LTC6811SPI::LTC6811_rdaux(REG_ALL, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back  aux registers ALL ADBMS1818 pag.63
//	
//				for (int current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++) 
//				{						
//					PackTemp_CH[1] = bms_ic[current_ic].aux.a_codes[0] / 10;	//GPIO1 PackTemp Busbar-
//					ADCPackTemp_CH[1] = ADCC::Volt_temperatureTab(PackTemp_CH[1]);
//					BatterySys::pack[current_ic].packTemp[1] = ADCPackTemp_CH[1];
//										
//					Temp_CH[2] = bms_ic[current_ic].aux.a_codes[6] / 10;	//GPIO6 Temp 3			
//					ADCTemp_CH[2] = ADCC::Volt_temperatureTab(Temp_CH[2]);
//					BatterySys::pack[current_ic].cellTemp[2] = ADCTemp_CH[2];
//				}
//				LTC6811SPI::SwitchADG729_Channel(ADG729_address1, 0x00);			
//				sw = 4;
//			}
//			break;
//			
//			case 4: //ms
//			{	
//				LTC6811SPI::Wakeup_IDLE(LTC6811SPI::TOTAL_IC, PackConst::WAKEUP_IDLE_Temp_DELAY);	
//				LTC6811SPI::SwitchADG729_Channel(ADG729_address2, S2A_S2B);
//				wait_us(100);
//				LTC6811SPI::LTC6811_adax(LTC6811SPI::ADC_CONVERSION_MODE, AUX_CH_GPIO2); //GPIO2 GPIO7
//				wait_us(800); //7 kHz Conversion Time 788 μs
//				error = LTC6811SPI::LTC6811_rdaux(REG_1, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back aux registers 1
//				error = LTC6811SPI::LTC6811_rdaux(REG_3, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back aux registers 3
////				error = LTC6811SPI::LTC6811_rdaux(REG_ALL, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back  aux registers ALL ADBMS1818 pag.63
//	
//				for (int current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++) 
//				{
//					Temp_CH[6] = bms_ic[current_ic].aux.a_codes[1] / 10; //GPIO2 Temp 7			
//					ADCTemp_CH[6] =  ADCC::Volt_temperatureTab(Temp_CH[6]);
//					BatterySys::pack[current_ic].cellTemp[6] = ADCTemp_CH[6];
//					
//					Temp_CH[9] = bms_ic[current_ic].aux.a_codes[7] / 10; //GPIO7 Temp 10		
//					ADCTemp_CH[9] = ADCC::Volt_temperatureTab(Temp_CH[9]);
//					BatterySys::pack[current_ic].cellTemp[9] = ADCTemp_CH[9];
//				}
//				LTC6811SPI::SwitchADG729_Channel(ADG729_address2, 0x00);
//				sw = 5;
//			}
//			break;
//			
//			case 5: //ms
//			{			
//				LTC6811SPI::Wakeup_IDLE(LTC6811SPI::TOTAL_IC, PackConst::WAKEUP_IDLE_Temp_DELAY);
//				LTC6811SPI::SwitchADG729_Channel(ADG729_address1, S3A_S3B);
//				wait_us(100);
//				LTC6811SPI::LTC6811_adax(LTC6811SPI::ADC_CONVERSION_MODE, AUX_CH_GPIO1); //GPIO1 GPIO6
//				wait_us(800); //7 kHz Conversion Time 788 μs
//				error = LTC6811SPI::LTC6811_rdaux(REG_1, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back aux registers 1
//				error = LTC6811SPI::LTC6811_rdaux(REG_3, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back aux registers 3
////				error = LTC6811SPI::LTC6811_rdaux(REG_ALL, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back  aux registers ALL ADBMS1818 pag.63
//
//				for (int current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++) 
//				{
//					Temp_CH[0] = bms_ic[current_ic].aux.a_codes[0] / 10;	//GPIO1 Temp 1
//					ADCTemp_CH[0] = ADCC::Volt_temperatureTab(Temp_CH[0]);
//					BatterySys::pack[current_ic].cellTemp[0] = ADCTemp_CH[0];
//						
//					Temp_CH[3] = bms_ic[current_ic].aux.a_codes[6] / 10;	//GPIO6 Temp 4			
//					ADCTemp_CH[3] = ADCC::Volt_temperatureTab(Temp_CH[3]);
//					BatterySys::pack[current_ic].cellTemp[3] = ADCTemp_CH[3];
//				}
//				LTC6811SPI::SwitchADG729_Channel(ADG729_address1, 0x00);
//				sw = 6;
//			}
//			break;
//			
//			case 6: //ms
//			{
//				LTC6811SPI::Wakeup_IDLE(LTC6811SPI::TOTAL_IC, PackConst::WAKEUP_IDLE_Temp_DELAY);
//				LTC6811SPI::SwitchADG729_Channel(ADG729_address2, S3A_S3B);
//				wait_us(100);
//				LTC6811SPI::LTC6811_adax(LTC6811SPI::ADC_CONVERSION_MODE, AUX_CH_GPIO2); //GPIO2 GPIO7
//				wait_us(800); //7 kHz Conversion Time 788 μs
//				error = LTC6811SPI::LTC6811_rdaux(REG_1, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back aux registers 1
//				error = LTC6811SPI::LTC6811_rdaux(REG_3, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back aux registers 3
////				error = LTC6811SPI::LTC6811_rdaux(REG_ALL, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back  aux registers ALL ADBMS1818 pag.63
//
//				for (int current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++) 
//				{					
//					Temp_CH[7] = bms_ic[current_ic].aux.a_codes[1] / 10; //GPIO2 Temp 8			
//					ADCTemp_CH[7] = ADCC::Volt_temperatureTab(Temp_CH[7]);
//					BatterySys::pack[current_ic].cellTemp[7] = ADCTemp_CH[7];
//					
//					PackTemp_CH[2] = bms_ic[current_ic].aux.a_codes[7] / 10; //GPIO7 PackTemp AMB		
//					ADCPackTemp_CH[2] = ADCC::Volt_temperatureTab(PackTemp_CH[2]);
//					BatterySys::pack[current_ic].packTemp[2] = ADCPackTemp_CH[2];
//				}
//				LTC6811SPI::SwitchADG729_Channel(ADG729_address2, 0x00);
//				sw = 7;
//			}
//			break;
//			
//			case 7: //ms
//			{					
//				LTC6811SPI::Wakeup_IDLE(LTC6811SPI::TOTAL_IC, PackConst::WAKEUP_IDLE_Temp_DELAY);	
//
//				LTC6811SPI::SwitchADG729_Channel(ADG729_address1, S4A_S4B);
//				wait_us(100);
//				LTC6811SPI::LTC6811_adax(LTC6811SPI::ADC_CONVERSION_MODE, AUX_CH_GPIO1); //GPIO1 GPIO6
//				wait_us(800); //7 kHz Conversion Time 788 μs
//		
//				error = LTC6811SPI::LTC6811_rdaux(REG_3, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back aux registers 3
////				error = LTC6811SPI::LTC6811_rdaux(REG_ALL, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back  aux registers ALL ADBMS1818 pag.63
//	
//				for (int current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++) 
//				{					
//					Temp_CH[4] = bms_ic[current_ic].aux.a_codes[6] / 10;	//GPIO6 Temp 5			
//					ADCTemp_CH[4] = ADCC::Volt_temperatureTab(Temp_CH[4]);
//					BatterySys::pack[current_ic].cellTemp[4] = ADCTemp_CH[4];					
//				}			
//				LTC6811SPI::SwitchADG729_Channel(ADG729_address1, 0x00);
//				sw = 8;
//			}
//			break;
//			
//			case 8: //ms
//			{				
//				LTC6811SPI::Wakeup_IDLE(LTC6811SPI::TOTAL_IC, PackConst::WAKEUP_IDLE_Temp_DELAY);
//				
//				bool GPIO_GLED = true;
//				bool GPIO_RLED = false;
//				if (Input_GPIO9 == 1)
//				{
//					GPIO_GLED = true;
//					GPIO_RLED = false;
//				}
//				else
//				{
//					GPIO_GLED = false;
//					GPIO_RLED = true;
//				}
//				
//				if (heartbeatSwitch == 0)
//				{			
//					//Gpio 8 G LED ON
//					bool GPIOBITS_A[5] = { true, true, GPIO_RLED, true, true }; //!< GPIO Pin Control // Gpio 1,2,3,4,5
//					bool GPIOBITS_B[4] = { true, true, GPIO_GLED, true }; //!< GPIO Pin Control // Gpio 6,7,8,9
//
//					for (uint8_t current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
//					{
//						ADBMS181x_set_cfgr_gpio(current_ic, bms_ic, GPIOBITS_A);
//						ADBMS1818_set_cfgrb_gpio_b(current_ic, bms_ic, GPIOBITS_B);
//					}
//					LTC6811SPI::ADBMS181x_wrcfg(LTC6811SPI::TOTAL_IC, bms_ic);
//					LTC6811SPI::ADBMS181x_wrcfgb(LTC6811SPI::TOTAL_IC, bms_ic);
//					heartbeatSwitch = 1;
//				}
//				else if (heartbeatSwitch == 1)
//				{
//					heartbeatSwitch = 2;
//					// Read Configuration Register
//					error = LTC6811SPI::ADBMS181x_rdcfg(LTC6811SPI::TOTAL_IC, bms_ic);
//					error = LTC6811SPI::ADBMS1818_rdcfgb(LTC6811SPI::TOTAL_IC, bms_ic);
//
//					for (uint8_t current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
//					{
//						Configuration_RegisterA = bms_ic[current_ic].config.rx_data[0];
//						Configuration_RegisterB = bms_ic[current_ic].configb.rx_data[0];
//						Input_GPIO9 = (Configuration_RegisterB & 0xFB) >> 3;					
//					}
//
//				}
//				else if (heartbeatSwitch == 2)
//				{
//					//Gpio 8  G LED OFF
////					GPIO_GLED = !GPIO_GLED;
//					bool GPIOBITS_A[5] = { true, true, false, true, true }; //!< GPIO Pin Control // Gpio 1,2,3,4,5
//					bool GPIOBITS_B[4] = { true, true, false, true }; //!< GPIO Pin Control // Gpio 6,7,8,9
//
//					for (uint8_t current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
//					{
//						ADBMS181x_set_cfgr_gpio(current_ic, bms_ic, GPIOBITS_A);
//						ADBMS1818_set_cfgrb_gpio_b(current_ic, bms_ic, GPIOBITS_B);
//					}
//					LTC6811SPI::ADBMS181x_wrcfg(LTC6811SPI::TOTAL_IC, bms_ic);
//					LTC6811SPI::ADBMS181x_wrcfgb(LTC6811SPI::TOTAL_IC, bms_ic);
//					heartbeatSwitch = 3;
//				}
//				else if (heartbeatSwitch == 3)
//				{
//					heartbeatSwitch = 0;
////				   // Read Configuration Register
////					error = LTC6811SPI::ADBMS181x_rdcfg(LTC6811SPI::TOTAL_IC, bms_ic);
//////					check_error(error);
////					error = LTC6811SPI::ADBMS1818_rdcfgb(LTC6811SPI::TOTAL_IC, bms_ic);
//////					check_error(error);
//////					print_rxconfig();
//////					print_rxconfigb();
////					for (uint8_t current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
////					{
////						Configuration_RegisterA = bms_ic[current_ic].config.rx_data[0];
////						Configuration_RegisterB = bms_ic[current_ic].configb.rx_data[0];
//////						int mask = 1 << 3; // 位元位置從 0 開始計算，所以這裡是第 3 個位元
//////						Temp_CH[15] = (Temp_CH[17] & mask) >> 3;
////						Input_GPIO9 = (Configuration_RegisterB & 0xFB) >> 3;
////
////						
////					}
//
//				}
//				sw = 0;
//			}
//			break;		
//									
//		    default:
//			{
//				sw = 0;
//			}
//			break;
//		}
//
//	}
	
	
	void LTC6811SPI::LTC6811Process()
	{
		if (LTC6811SPI::BMSLTC6811VoltTemp_Flag == 1)
		{	
			Green_LED = 1;
		
			if ((Balance::BCU_SET_Manual_Balance_flag == 0) && (Balance::BCU_SET_Semi_Automatic_Balance_flag == 0))	
			{
				Balance::PassiveBalancejudge(); //max_volt - min_volt >= Passive_IV
			}
			else if ((Balance::BCU_SET_Manual_Balance_flag == 1) || (Balance::BCU_SET_Semi_Automatic_Balance_flag == 1))	
			{
				Balance::PassiveBalanceCommand_ON_flag = 1;
				Balance::PassiveBalancejudge_flag = 0;
			}
			else
			{
				
			}

			LTC6811SPI::VoltageTemperature_measurement_switch();
					
			LTC6811SPI::Update_PACK_Voltage();
			LTC6811SPI::BMSLTC6811VoltTemp_Flag = 0;	
			Green_LED = 0;
	
		}
	}
	
	//Starts cell voltage overlap conversion
	void LTC6811SPI::StartOverlapConversion()
	{
		LTC681x_adol(MD_7KHZ_3KHZ, DCP_DISABLED);
	}


	
	//case 17: Run ADC Overlap self test
	void LTC6811SPI::ADCOverlap_self_test()
	{	
		error = static_cast<int8_t>(LTC6811SPI::LTC6811_run_adc_overlap(LTC6811SPI::TOTAL_IC, bms_ic));
		if (error == 0) 
		{
			Overlap_Test_Flag = 0;		//PASS
		}
		else 
		{
			Overlap_Test_Flag = 1;		//FAIL
		}

	}
	
	//case 7: Start Status ADC Measurement
	//case 8: Read Status registers
	void LTC6811SPI::Read_Status_registers()
	{
	
//		LTC6811SPI::wakeup_sleep(LTC6811SPI::TOTAL_IC);
		LTC6811SPI::LTC681x_adstat(LTC6811SPI::ADC_CONVERSION_MODE, LTC6811SPI::STAT_CH_TO_CONVERT);
//		LTC6811SPI::LTC681x_pollAdc();
		
//		LTC6811SPI::wakeup_sleep(LTC6811SPI::TOTAL_IC);
		error = LTC6811SPI::LTC6811_rdstat(0U, LTC6811SPI::TOTAL_IC, bms_ic); // Set to read back all aux registers
		LTC6811SPI::Check_error(error);
		
		BatterySys::pBattSys->m_sysVolt = 0U;
		//current_ic = MAX_PACK_NUM
		for (uint16_t current_ic = 0U; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
		{
			if (bms_ic[current_ic].stat.stat_codes[0] == 0xFFFFU)
			{
				bms_ic[current_ic].stat.stat_codes[0] = 0U;
			}
			BatterySys::pBattSys->m_sysVolt += bms_ic[current_ic].stat.stat_codes[0] / 10U * 20U;
						
		}

	}

	
	//case 13: Enable a discharge transistor
	void LTC6811SPI::Enable_discharge_transistor(int Cell)
	{
//		LTC6811SPI::wakeup_sleep(LTC6811SPI::TOTAL_IC);
//		LTC6811SPI::LTC681x_adcv(LTC6811SPI::ADC_CONVERSION_MODE, LTC6811SPI::ADC_DCP, LTC6811SPI::CELL_CH_TO_CONVERT);
//		conv_time = LTC6811SPI::LTC681x_pollAdc();
		
//		LTC6811SPI::wakeup_sleep(LTC6811SPI::TOTAL_IC);
		LTC6811SPI::LTC6811_set_discharge(Cell, LTC6811SPI::TOTAL_IC, bms_ic);
		LTC6811SPI::LTC6811_wrcfg(LTC6811SPI::TOTAL_IC, bms_ic);
		
//		wakeup_idle(TOTAL_IC);
		error = LTC6811_rdcfg(TOTAL_IC, bms_ic);
		Check_error(error);
		//current_ic = MAX_PACK_NUM
		for (uint16_t current_ic = 0U; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
		{
		
		}

		
//	case 21: // Enable a discharge transistor
//
//		wakeup_sleep(TOTAL_IC);
//		LTC6811_set_discharge(readIC, TOTAL_IC, bms_ic);
//		LTC6811_wrcfg(TOTAL_IC, bms_ic);   
//		print_config();
//		wakeup_idle(TOTAL_IC);
//		error = LTC6811_rdcfg(TOTAL_IC, bms_ic);
//		check_error(error);
//		print_rxconfig();
//		break;
//      
//	case 22: // Clear all discharge transistors
//		wakeup_sleep(TOTAL_IC);
//		LTC6811_clear_discharge(TOTAL_IC, bms_ic);
//		LTC6811_wrcfg(TOTAL_IC, bms_ic);    
//		print_config();
//		wakeup_idle(TOTAL_IC);
//		error = LTC6811_rdcfg(TOTAL_IC, bms_ic);
//		check_error(error);
//		print_rxconfig();
//		break;
	}
	
	
//	case 26: // Write and read S Control Register Group
	void LTC6811SPI::Write_S_Control(bool_t Command_ON)
	{

		/**************************************************************************************
		   S pin control. 
		   1)Ensure that the pwm is set according to the requirement using the previous case.
		   2)Choose the value depending on the required number of pulses on S pin. 
		   Refer to the data sheet. 
		  ***************************************************************************************/
		for (uint8_t current_ic = 0; current_ic < TOTAL_IC; current_ic++)
		{									
			if (Command_ON == 1)
			{
				bms_ic[current_ic].sctrl.tx_data[0] = (uint8_t)((BatterySys::pack[current_ic].cellBalance[1] << 4) & 0xF0)
													| (BatterySys::pack[current_ic].cellBalance[0] & 0x0F); // No. of high pulses on S pin 2 and 1
				bms_ic[current_ic].sctrl.tx_data[1] = (uint8_t)((BatterySys::pack[current_ic].cellBalance[3] << 4) & 0xF0)
													| (BatterySys::pack[current_ic].cellBalance[2] & 0x0F); // No. of high pulses on S pin 4 and 3
				bms_ic[current_ic].sctrl.tx_data[2] = (uint8_t)((BatterySys::pack[current_ic].cellBalance[5] << 4) & 0xF0)
													| (BatterySys::pack[current_ic].cellBalance[4] & 0x0F); 	// No. of high pulses on S pin 6 and 5		
				bms_ic[current_ic].sctrl.tx_data[3] = (uint8_t)((BatterySys::pack[current_ic].cellBalance[7] << 4) & 0xF0)
													| (BatterySys::pack[current_ic].cellBalance[6] & 0x0F); // No. of high pulses on S pin 8 and 7
				bms_ic[current_ic].sctrl.tx_data[4] = (uint8_t)((BatterySys::pack[current_ic].cellBalance[9] << 4) & 0xF0)
													| (BatterySys::pack[current_ic].cellBalance[8] & 0x0F); // No. of high pulses on S pin 10 and 9
				bms_ic[current_ic].sctrl.tx_data[5] = (uint8_t)((BatterySys::pack[current_ic].cellBalance[11] << 4) & 0xF0)
													| (BatterySys::pack[current_ic].cellBalance[10] & 0x0F);  // No. of high pulses on S pin 12 and 11
								
			
			}	
			else //if (Command_ON == 0)
			{
				bms_ic[current_ic].sctrl.tx_data[0] = 0x00; // No. of high pulses on S pin 2 and 1
				bms_ic[current_ic].sctrl.tx_data[1] = 0x00; // No. of high pulses on S pin 4 and 3
				bms_ic[current_ic].sctrl.tx_data[2] = 0x00; // No. of high pulses on S pin 6 and 5
				bms_ic[current_ic].sctrl.tx_data[3] = 0x00; // No. of high pulses on S pin 8 and 7
				bms_ic[current_ic].sctrl.tx_data[4] = 0x00; // No. of high pulses on S pin 10 and 9
				bms_ic[current_ic].sctrl.tx_data[5] = 0x00; // No. of high pulses on S pin 12 and 11
	
			}
		}

		LTC681x_wrsctrl(TOTAL_IC, 0, bms_ic);
	
		// Start S Control pulsing
		LTC681x_stsctrl();
		
//		for (uint8_t current_ic = 0; current_ic < TOTAL_IC; current_ic++)
//		{
//			bms_ic[current_ic].sctrlb.tx_data[3] = (uint8_t)((BatterySys::pack[current_ic].cellBalance[13] << 4) & 0xF0)
//													| (BatterySys::pack[current_ic].cellBalance[12] & 0x0F); // No. of high pulses on S pin 14 and 13
//			bms_ic[current_ic].sctrlb.tx_data[4] = (uint8_t)((BatterySys::pack[current_ic].cellBalance[15] << 4) & 0xF0)
//													| (BatterySys::pack[current_ic].cellBalance[14] & 0x0F); // No. of high pulses on S pin 16 and 15
//			bms_ic[current_ic].sctrlb.tx_data[5] = 0x00; // No. of high pulses on S pin 18 and 17
//		}	
//		ADBMS1818_wrpsb(TOTAL_IC, bms_ic); //  Write PWM/S control register group B
	
	}
	
	
	
	//Update Pack Total Voltage:
	void LTC6811SPI::Update_PACK_Voltage()
	{	
		//current_ic = MAX_PACK_NUM
		for (uint8_t current_ic = 0; current_ic < TOTAL_IC; current_ic++)
		{		
			BatterySys::pack[current_ic].packVolt = 0;
			//MAX_CELL_NUM
			for (uint16_t i = 0; i < PackConst::MAX_CELL_NUM; i++)
			{
				BatterySys::pack[current_ic].packVolt += BatterySys::pack[current_ic].cellVolt[i]; //Update Pack Total Voltage:			
			}
		}
		
		BatterySys::pBattSys->m_sysVolt = 0U;
		//MAX_PACK_NUM
		for (uint16_t i = 0U; i < PackConst::MAX_PACK_NUM; i++)
		{
			BatterySys::pBattSys->m_sysVolt += BatterySys::pack[i].packVolt; //total Sum
		}
		Balance::Average_voltReading = static_cast<uint16_t>(BatterySys::pBattSys->m_sysVolt
										/ ((PackConst::MAX_CELL_NUM)*(PackConst::MAX_PACK_NUM)));

		BatterySys::m_voltReading = static_cast<uint16_t>((BatterySys::pBattSys->m_sysVolt) / 100U); //convert to 0.1V
	}
	
	
} //namespace bmstech

		
