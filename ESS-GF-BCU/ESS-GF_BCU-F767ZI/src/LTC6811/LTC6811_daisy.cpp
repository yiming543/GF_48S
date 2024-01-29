//File
#include <mbed.h>
#include "LTC6811_daisy.h"
#include "PackConst.h"
#include "ADBMS1818_daisy.h"

namespace bmstech
{
	DigitalOut CS(SPI4_CSB); // ssel
	SPI spi4(SPI4_MOSI, SPI4_MISO, SPI4_SCK); // mosi, miso, sclk
	

	void LTC6811SPI::SPI_init()
	{
		CS = 1;							//high as init for disable SPI
			
		spi4.format(8, 3);
		spi4.frequency(LTC6811_SPI_BAUD); //100K 200K 700K 900K 1000K	
//		CS = 0;
//		wait_us(10); //   
//		CS = 1;	
	}
	
	//spi_read_byte
	uint8_t LTC6811SPI::SPI_read_byte(const uint8_t tx_dat)
	{
//		uint8_t data = (uint8_t)spi4.write(0xFF);
//		return (data);
		return static_cast<uint8_t>(spi4.write(tx_dat));
	}
	
	/*Writes an array of bytes out of the SPI port*/
	void LTC6811SPI::SPI_write_array(uint16_t len,uint8_t data[])
	{
		for (uint16_t i = 0; i < len; i++) //
		{
			spi4.write(static_cast<uint8_t>(data[i]));
		}
	}
	
	//array of data to be written on SPI port
	void LTC6811SPI::SPI_write_read(uint8_t tx_Data[], uint16_t tx_len, uint8_t *rx_data, uint16_t rx_len)
	{
		//tx_len
		for (uint16_t i = 0; i < tx_len; i++)
		{
			spi4.write(tx_Data[i]);
		}
//		wait_us(10);
		//rx_len
		for (uint16_t i = 0; i < rx_len; i++)
		{
			rx_data[i] = (uint8_t)spi4.write(0xFF);
		}

	}
	
	void LTC6811SPI::Wakeup_IDLE(uint8_t total_ic)
	{
		//MAX_PACK_NUM
		for (uint16_t i = 0; i < total_ic; i++)
		{
			spi4.lock();
			CS = 0;
			SPI_read_byte(0xff);
			CS = 1;
			spi4.unlock();
		}
	}
	

	
	void LTC6811SPI::Wakeup_IDLE(uint8_t total_ic, uint16_t delay_time)
	{
		//MAX_PACK_NUM
		for (uint16_t i = 0; i < total_ic; i++)
		{
			spi4.lock();
			CS = 0;
			//			wait_ms(2); //Guarantees the isoSPI will be in ready mode
			wait_us(delay_time); //   
			SPI_read_byte(0xff);

			CS = 1;
			spi4.unlock();
		}

	}

	//Generic wakeup commannd to wake the LTC6813 from sleep
	void LTC6811SPI::Wakeup_SLEEP(uint8_t total_ic)
	{
		//MAX_PACK_NUM
		for (uint16_t i = 0; i < total_ic; i++)
		{
			spi4.lock();
			CS = 0;
			wait_us(800); // Guarantees the LTC6813 will be in standby
			CS = 1;
			spi4.unlock();
			wait_us(10);	//
		}
	}
	
	/*  Calculates  and returns the CRC15  */
	//Number of bytes that will be used to calculate a PEC
	//Array of data that will be used to calculate  a PEC
	uint16_t LTC6811SPI::PEC15_CALC(uint8_t len, uint8_t *data)
	{

		uint16_t remainder = 16U; //initialize the PEC
		//len
		for (uint8_t i = 0U; i < len; i++)
		{
			// loops for each byte in data array
			uint16_t addr = ((remainder >> 7U) ^ data[i]) & 0xffU; //calculate PEC table address
			remainder = (remainder << 8U) ^ crc15Table[addr];
			//        remainder = (remainder<<8)^pgm_read_word_near(crc15Table+addr);
		
		}
		return (remainder * 2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
	}

	//Generic function to write 68xx commands. Function calculated PEC for tx_cmd data
	void LTC6811SPI::CMD_6811(uint8_t tx_cmd[2])
	{
		uint8_t cmd[4];//
	
		//    uint8_t md_bits;

		cmd[0] =  tx_cmd[0];
		cmd[1] =  tx_cmd[1];
		//
		const uint16_t cmd_pec = PEC15_CALC(2, cmd);
		cmd[2] = static_cast<uint8_t>((cmd_pec >> 8));
		cmd[3] = static_cast<uint8_t>(cmd_pec);
		
		spi4.lock();
		CS = 0;
		SPI_write_array(4, cmd);
		CS = 1;
		spi4.unlock();

	}
	
	
	//Generic function to write 68xx commands and write payload data. Function calculated PEC for tx_cmd data
	void LTC6811SPI::WRITE_6811(uint8_t total_ic, uint8_t tx_cmd[2], uint8_t data[])
	{
		const uint8_t BYTES_IN_REG = 6;//
		uint16_t CMD_LEN = 4 + (8*total_ic);//
		
//		uint8_t *cmd;	//JY 20200706
//		uint8_t cmd[384];	//JY 20200706
//		cmd = (uint8_t *)malloc(CMD_LEN*sizeof(uint8_t));	//JY 20200706
		cmd[0] = tx_cmd[0];
		cmd[1] = tx_cmd[1];
		uint16_t cmd_pec = PEC15_CALC(2, cmd);//
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);
		uint16_t cmd_index = 4;//
		//MAX_PACK_NUM
		for (uint8_t current_ic = total_ic; current_ic > 0; current_ic--)
		{
			// executes for each LTC681x in daisy chain, this loops starts with
		    // the last IC on the stack. The first configuration written is
		    // received by the last IC in the daisy chain
			for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
			{
				cmd[cmd_index] = data[((current_ic - 1) * 6) + current_byte];
				cmd_index = cmd_index + 1;
			}

			// calculating the PEC for each Iss configuration register data
			uint16_t data_pec = PEC15_CALC(BYTES_IN_REG, &data[(current_ic - 1) * 6]);

			cmd[cmd_index] = (uint8_t)(data_pec >> 8);
			cmd[cmd_index + 1] = (uint8_t)data_pec;
			cmd_index = cmd_index + 2;
		}
		spi4.lock();
		CS = 0;
		SPI_write_array(CMD_LEN, cmd);
		CS = 1;
		spi4.unlock();

//		free(cmd);	//JY 20200706
	}

	//Generic function to write 68xx commands and read data. Function calculated PEC for tx_cmd data
	int8_t LTC6811SPI::READ_6811(uint8_t total_ic, uint8_t tx_cmd[2], uint8_t *rx_data)
	{
		uint8_t BYTES_IN_REG = 8U;	//init
		uint8_t cmd[4];		//init
//		uint8_t data[384];
		int8_t pec_error = 0;

		// data = (uint8_t *) malloc((8*total_ic)*sizeof(uint8_t)); // This is a problem because it can fail

		cmd[0] = tx_cmd[0];
		cmd[1] = tx_cmd[1];
		uint16_t cmd_pec = PEC15_CALC(2U, cmd);//
		cmd[2] = (uint8_t)(cmd_pec >> 8U);
		cmd[3] = (uint8_t)(cmd_pec);
	
		spi4.lock();
		CS = 0;
		//Read the configuration data of all ICs on the daisy chain into
		SPI_write_read(cmd, 4U, data, (BYTES_IN_REG*total_ic));

		CS = 1;		
		spi4.unlock();

		//MAX_PACK_NUM
		for (uint8_t current_ic = 0U; current_ic < total_ic; current_ic++)
		{
			//executes for each LTC681x in the daisy chain and packs the data
		    //into the r_comm array as well as check the received Config data
		    //for any bit errors
			for (uint8_t current_byte = 0U; current_byte < BYTES_IN_REG; current_byte++)
			{
				rx_data[(current_ic * 8) + current_byte] = data[current_byte + (current_ic*BYTES_IN_REG)];
			}
			uint16_t received_pec = (rx_data[(current_ic * 8) + 6] << 8) + rx_data[(current_ic * 8) + 7];//
			uint16_t data_pec = PEC15_CALC(6, &rx_data[current_ic * 8]);//
			if (received_pec != data_pec) 
			{
				pec_error = -1;
			}
		}


		return (pec_error);
	}
	
	//Starts cell voltage conversion
	//MD:ADC Mode//DCP:Discharge Permit//CH:Cell Channels to be measured
	void LTC6811SPI::LTC681x_adcv(uint8_t MD, uint8_t DCP, uint8_t CH)
	{
		uint8_t cmd[4] = {0U};//
		uint8_t md_bits = 0U;//

		md_bits = (MD & 0x02U) >> 1U;
		cmd[0] = md_bits + 0x02U;
		md_bits = (MD & 0x01U) << 7U;
		cmd[1] =  md_bits + 0x60U + (DCP << 4U) + CH;
	
		CMD_6811(cmd);
	
	}
	
	//Starts cell voltage and SOC conversion
	void LTC6811SPI::LTC681x_adcvsc(uint8_t MD, uint8_t DCP)//MD:ADC Mode//DCP:Discharge Permit
	{
		uint8_t cmd[4];//
		uint8_t md_bits = 0U;//
		md_bits = (MD & 0x02U) >> 1U;
		cmd[0] = md_bits | 0x04U;
		md_bits = (MD & 0x01U) << 7U;
		cmd[1] =  md_bits | 0x60U | (DCP << 4U) | 0x07U;
		CMD_6811(cmd);

	}
	

	// Starts cell voltage  and GPIO 1&2 conversion
	void LTC681x_adcvax(uint8_t MD,uint8_t DCP)//ADC Mode//Discharge Permit
	{
		uint8_t cmd[4];//
		uint8_t md_bits = 0U;//
		md_bits = (MD & 0x02U) >> 1U;
		cmd[0] = md_bits | 0x04U;
		md_bits = (MD & 0x01U) << 7U;
		cmd[1] =  md_bits | ((DCP & 0x01U) << 4U) + 0x6FU;
		LTC6811SPI::CMD_6811(cmd);
	}
	
	//Starts cell voltage overlap conversion
	void LTC6811SPI::LTC681x_adol(uint8_t MD, uint8_t DCP)//ADC Mode//Discharge Permit
	{
		uint8_t cmd[4];//
		uint8_t md_bits = 0U;//
		md_bits = (MD & 0x02U) >> 1U;
		cmd[0] = md_bits + 0x02U;
		md_bits = (MD & 0x01U) << 7U;
		cmd[1] =  md_bits + (DCP << 4U) + 0x01U;
		LTC6811SPI::CMD_6811(cmd);
	}
	
	//Starts cell voltage self test conversion
	void LTC681x_cvst(uint8_t MD,uint8_t ST)//ADC Mode//Self Test
	{
		uint8_t cmd[2];//
		uint8_t md_bits = 0U;//

		md_bits = (MD & 0x02U) >> 1U;
		cmd[0] = md_bits + 0x02U;
		md_bits = (MD & 0x01U) << 7U;
		cmd[1] =  md_bits + ((ST) << 5U) + 0x07U;
		LTC6811SPI::CMD_6811(cmd);

	}
	
	//Start an Auxiliary Register Self Test Conversion
	void LTC681x_axst(uint8_t MD,uint8_t ST)//ADC Mode//Self Test
	{
		uint8_t cmd[4];//
		uint8_t md_bits = 0U;//

		md_bits = (MD & 0x02U) >> 1U;
		cmd[0] = md_bits + 0x04U;
		md_bits = (MD & 0x01U) << 7U;
		cmd[1] =  md_bits + ((ST & 0x03U) << 5U) + 0x07U;
		LTC6811SPI::CMD_6811(cmd);

	}
	
	//Start a Status Register Self Test Conversion
	void LTC6811SPI::LTC681x_statst(uint8_t MD, uint8_t ST)//ADC Mode//Self Test
	{
		uint8_t cmd[2];//
		uint8_t md_bits = 0U;//

		md_bits = (MD & 0x02U) >> 1U;
		cmd[0] = md_bits + 0x04U;
		md_bits = (MD & 0x01U) << 7U;
		cmd[1] =  md_bits + ((ST & 0x03U) << 5U) + 0x0FU;
		CMD_6811(cmd);

	}
		 
	//Start a GPIO and Vref2 Conversion
	void LTC6811SPI::LTC6811_adax(uint8_t MD,uint8_t CHG)//ADC Mode//GPIO Channels to be measured)
	{
		uint8_t cmd[4];//
		uint8_t md_bits;//

		md_bits = (MD & 0x02) >> 1;
		cmd[0] = md_bits + 0x04;
		md_bits = (MD & 0x01) << 7;
		cmd[1] = md_bits + 0x60 + CHG;
		CMD_6811(cmd);

	}
	
	
	//Start an GPIO Redundancy test
	void LTC6811SPI::LTC681x_adaxd(uint8_t MD, uint8_t CHG)//ADC Mode//GPIO Channels to be measured)
	{
		uint8_t cmd[4];//
		uint8_t md_bits = 0U;//

		md_bits = (MD & 0x02U) >> 1U;
		cmd[0] = md_bits + 0x04U;
		md_bits = (MD & 0x01U) << 7U;
		cmd[1] = md_bits + CHG;
		CMD_6811(cmd);
	}
	
	//Start a Status ADC Conversion
	void LTC6811SPI::LTC6811_ADCstat(uint8_t MD, uint8_t CHST)//ADC Mode//GPIO Channels to be measured
	{
		uint8_t cmd[4];//
		uint8_t md_bits = 0U;//

		md_bits = (MD & 0x02U) >> 1U;
		cmd[0] = md_bits + 0x04U;
		md_bits = (MD & 0x01U) << 7U;
		cmd[1] = md_bits + 0x68U + CHST;
		CMD_6811(cmd);
	}
	
	// Start a Status register redundancy test Conversion	
	void LTC6811SPI::LTC681x_adstatd(uint8_t MD,uint8_t CHST)//ADC Mode//GPIO Channels to be measured
	{
		uint8_t cmd[2];//
		uint8_t md_bits = 0U;//

		md_bits = (MD & 0x02U) >> 1U;
		cmd[0] = md_bits + 0x04U;
		md_bits = (MD & 0x01U) << 7U;
		cmd[1] = md_bits + 0x08U + CHST;
		CMD_6811(cmd);

	}

	// Start an open wire Conversion
	void LTC6811SPI::LTC681x_adow(uint8_t MD, uint8_t PUP)//ADC Mode//Discharge Permit
	{
		uint8_t cmd[2];//
		uint8_t md_bits = 0U;//
		md_bits = (MD & 0x02U) >> 1U;
		cmd[0] = md_bits + 0x02U;
		md_bits = (MD & 0x01U) << 7U;
		cmd[1] =  md_bits + 0x28U + (PUP << 6U); //+ CH;
		CMD_6811(cmd);
	}
	

	
	// Reads the raw cell voltage register data
	//Determines which cell voltage register is read back//the number of ICs in the//An array of the unparsed cell codes
	void LTC6811SPI::LTC681x_rdcv_reg(uint8_t reg,uint8_t total_ic,uint8_t *data)
	{
		uint8_t REG_LEN = 8U; //number of bytes in each ICs register + 2 bytes for the PEC
		uint8_t cmd[4];//

		if (reg == 1U) {
			//1: RDCVA
			cmd[1] = 0x04U;
			cmd[0] = 0x00U;
		}
		else if (reg == 2U) {
			//2: RDCVB
			cmd[1] = 0x06U;
			cmd[0] = 0x00U;
		}
		else if (reg == 3U) {
			//3: RDCVC
			cmd[1] = 0x08U;
			cmd[0] = 0x00U;
		}
		else if (reg == 4U) {
			//4: RDCVD
			cmd[1] = 0x0AU;
			cmd[0] = 0x00U;
		}
		else if (reg == 5U) {
			//4: RDCVE
			cmd[1] = 0x09U;
			cmd[0] = 0x00U;
		}
		else if (reg == 6U) {
			//4: RDCVF
			cmd[1] = 0x0BU;
			cmd[0] = 0x00U;
		}

		//
		uint16_t cmd_pec = PEC15_CALC(2U, cmd);
		cmd[2] = (uint8_t)(cmd_pec >> 8U);
		cmd[3] = (uint8_t)(cmd_pec);

		spi4.lock();
		CS = 0;

		SPI_write_read(cmd, 4U, data, (REG_LEN*total_ic));
		CS = 1;
		spi4.unlock();

	}
	
//	//helper function that parses voltage measurement registers
//	int8_t LTC6811SPI::Parse_cells(uint8_t current_ic, uint8_t cell_reg,
//			uint8_t cell_data[], uint16_t *cell_codes, uint8_t *ic_pec)
//	{
//		const uint8_t BYT_IN_REG = 6U;		//const
//		const uint8_t CELL_IN_REG = 3U;	//const
//		int8_t pec_error = 0;	//
//
//		uint16_t data_counter = current_ic*NUM_RX_BYT; //data counter
//		//
//		for (uint8_t current_cell = 0U; current_cell < CELL_IN_REG; current_cell++)
//		{
//			// This loop parses the read back data into cell voltages, it
//		    // loops once for each of the 3 cell voltage codes in the register
//			//Each cell code is received as two bytes and is combined to
//			uint16_t parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1U] << 8U);
//
//			// create the parsed cell voltage code
//			cell_codes[current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;
//			data_counter = data_counter + 2; //Because cell voltage codes are two bytes the data counter
//			//must increment by two for each parsed cell code
//		}
//
//		//The received PEC for the current_ic is transmitted as the 7th and 8th
//		uint16_t received_pec = (cell_data[data_counter] << 8) | cell_data[data_counter + 1];
//
//		//after the 6 cell voltage data bytes
//		uint16_t data_pec = PEC15_CALC(BYT_IN_REG, &cell_data[(current_ic) * NUM_RX_BYT]);
//
//		if (received_pec != data_pec) 
//		{
//			pec_error = 1; //The pec_error variable is simply set negative if any PEC errors
//			ic_pec[cell_reg - 1] = 1;
//		}
//		else 
//		{
//			ic_pec[cell_reg - 1] = 0;
//		}
//		data_counter = data_counter + 2;
//		return (pec_error);
//	}
//	
	
	
	
	
	//Read the raw data from the ltc6811 cell voltage register
	void LTC6811SPI::LTC6811_rdcv_reg_AMS(uint8_t reg, //Determines which cell voltage register is read back
		uint8_t total_ic, //the number of ICs in the
		uint8_t *data //An array of the unparsed cell codes
	   )
	{
		uint8_t REG_LEN = 8U; //number of bytes in each ICs register + 2 bytes for the PEC
		uint8_t cmd[4];//
		uint16_t cmd_pec = 0U;//

		if (reg == 1U)     //1: RDCVA
		{
			cmd[1] = 0x04U;
			cmd[0] = 0x00U;
		}
		else if (reg == 2U) //2: RDCVB
		{
			cmd[1] = 0x06U;
			cmd[0] = 0x00U;
		}
		else if (reg == 3U) //3: RDCVC
		{
			cmd[1] = 0x08U;
			cmd[0] = 0x00U;
		}
		else if (reg == 4U) //4: RDCVD
		{
			cmd[1] = 0x0AU;
			cmd[0] = 0x00U;
		}
		else if (reg == 5U) //4: RDCVE
		{
			cmd[1] = 0x09U;
			cmd[0] = 0x00U;
		}
		else if (reg == 6U) //4: RDCVF
		{
			cmd[1] = 0x0BU;
			cmd[0] = 0x00U;
		}


		cmd_pec = PEC15_CALC(2U, cmd);
		cmd[2] = (uint8_t)(cmd_pec >> 8U);
		cmd[3] = (uint8_t)(cmd_pec);

		spi4.lock();
		CS = 0; //output_low(LTC6811_CS);
		SPI_write_read(cmd, 4, data, (REG_LEN*total_ic));
		CS = 1; //output_high(LTC6811_CS);
		spi4.unlock();

	}
	// Reads and parses the ltc6811 cell voltage registers.
//	uint8_t LTC6811SPI::LTC6811_rdcv_AMS(uint8_t reg, // Controls which cell voltage register is read back.
//		uint8_t total_ic, // the number of ICs in the system
//		uint16_t cell_codes[][12] // Array of the parsed cell codes
//	   )
//	{
//
////		const uint8_t NUM_RX_BYT = 8;
//		uint8_t BYT_IN_REG = 6U;
//		uint8_t CELL_IN_REG = 3U;//
//		uint8_t NUM_CV_REG = 4U;//
//
////		uint8_t *cell_data;
//		uint8_t pec_error = 0U;
//		uint16_t parsed_cell = 0U;//
//		uint16_t received_pec = 0U;//
//		uint16_t data_pec = 0U;//
//		uint16_t data_counter = 0U; //data counter
////		cell_data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));
//
//		if (reg == 0U)
//		{
//			//executes once for each of the ltc6811 cell voltage registers
//			for (uint8_t cell_reg = 1U; cell_reg < NUM_CV_REG + 1; cell_reg++)
//			{
//				data_counter = 0U;
//				LTC6811_rdcv_reg_AMS(cell_reg, total_ic, cell_data); //Reads a single Cell voltage register
//				// executes for every ltc6811 in the daisy chain
//				for (uint8_t current_ic = 0U; current_ic < total_ic; current_ic++)
//				{
//					// current_ic is used as the IC counter
//					// This loop parses the read back data into cell voltages, it
//					for (uint8_t current_cell = 0U; current_cell < CELL_IN_REG; current_cell++)
//					{
//						// loops once for each of the 3 cell voltage codes in the register
//						//Each cell code is received as two bytes and is combined to
//						parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);
//
//						// create the parsed cell voltage code
//
//						cell_codes[current_ic][current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;
//						data_counter = data_counter + 2; //Because cell voltage codes are two bytes the data counter
//						//must increment by two for each parsed cell code
//					}
//					//The received PEC for the current_ic is transmitted as the 7th and 8th
//					received_pec = (cell_data[data_counter] << 8) + cell_data[data_counter + 1];
//
//					//after the 6 cell voltage data bytes
//					data_pec = PEC15_CALC(BYT_IN_REG, &cell_data[current_ic * NUM_RX_BYT]);
//					if (received_pec != data_pec)
//					{
//						pec_error = -1; //The pec_error variable is simply set negative if any PEC errors
//						//are detected in the serial data
//					}
//					data_counter = data_counter + 2; //Because the transmitted PEC code is 2 bytes long the data_counter
//					//must be incremented by 2 bytes to point to the next ICs cell voltage data
//				}
//			}
//		}
//
//		else
//		{
//
//			LTC6811_rdcv_reg_AMS(reg, total_ic, cell_data);
//			// executes for every ltc6811 in the daisy chain
//			for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++)
//			{
//				// current_ic is used as the IC counter
//				// This loop parses the read back data into cell voltages, it
//				for (uint8_t current_cell = 0; current_cell < CELL_IN_REG; current_cell++)
//
//				{
//					// loops once for each of the 3 cell voltage codes in the register
//					//Each cell code is received as two bytes and is combined to
//					parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);
//
//					// create the parsed cell voltage code
//
//					cell_codes[current_ic][current_cell + ((reg - 1) * CELL_IN_REG)] = 0x0000FFFF & parsed_cell;
//					data_counter = data_counter + 2; //Because cell voltage codes are two bytes the data counter
//					//must increment by two for each parsed cell code
//				}
//				//The received PEC for the current_ic is transmitted as the 7th and 8th
//				received_pec = (cell_data[data_counter] << 8) + cell_data[data_counter + 1];
//
//				//after the 6 cell voltage data bytes
//				data_pec = PEC15_CALC(BYT_IN_REG, &cell_data[current_ic * NUM_RX_BYT]);
//				if (received_pec != data_pec)
//				{
//					pec_error = -1; //The pec_error variable is simply set negative if any PEC errors
//					//are detected in the serial data
//				}
//				data_counter = data_counter + 2; //Because the transmitted PEC code is 2 bytes long the data_counter
//				//must be incremented by 2 bytes to point to the next ICs cell voltage data
//			}
//		}
//
//
////		free(cell_data);
//		return (pec_error);
//	}
//	


	
	
	//Reads and parses the LTC681x cell voltage registers.
	// Controls which cell voltage register is read back.// the number of ICs in the system
//	// Array of the parsed cell codes
//	uint8_t LTC6811SPI::LTC681x_rdcv(uint8_t reg,uint8_t total_ic,Cell_asic ic[])
//	{
//		int8_t pec_error = 0;//init
////		uint8_t *cell_data	//JY 20200706
//		uint8_t c_ic = 0U;
//		
////		cell_data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));	//JY 20200706
////		uint8_t cell_data[384];	//JY 20200706
//		
//		if (reg == 0U)
//		{
//			for (uint8_t cell_reg = 1U; cell_reg < ic[0].ic_reg.num_cv_reg + 1; cell_reg++) //
//			{
//				//executes once for each of the LTC6811 cell voltage registers
//				LTC681x_rdcv_reg(cell_reg, total_ic, cell_data);
//				for (uint16_t current_ic = 0U; current_ic < total_ic; current_ic++) //
//				{
//					if (ic->isospi_reverse == false) 
//					{
//						c_ic = current_ic;
//					}
//					else 
//					{
//						c_ic = total_ic - current_ic - 1;
//					}
//					pec_error = pec_error +
//							Parse_cells(current_ic,cell_reg,cell_data,&ic[c_ic].cells.c_codes[0],&ic[c_ic].cells.pec_match[0]);
//				}
//			}
//		}
//
//		else 
//		{
//			LTC681x_rdcv_reg(reg, total_ic, cell_data);
//			//MAX_PACK_NUM
//			for (uint16_t current_ic = 0U; current_ic < total_ic; current_ic++)
//			{
//				if (ic->isospi_reverse == false) 
//				{
//					c_ic = current_ic;
//				}
//				else 
//				{
//					c_ic = total_ic - current_ic - 1;
//				}
//				pec_error = pec_error +
//						Parse_cells(current_ic,	reg, &cell_data[8*c_ic], &ic[c_ic].cells.c_codes[0], &ic[c_ic].cells.pec_match[0]);
//			}
//		}
//		LTC681x_check_pec(total_ic, CELL, ic);
////		free(cell_data);	//JY 20200706
//		return (pec_error);
//	}

	
	// Reads and parses the LTC6811 cell voltage registers.
	uint8_t LTC6811SPI::LTC6811_rdcv(uint8_t reg, // Controls which cell voltage register is read back.
		uint8_t total_ic, // the number of ICs in the system
		Cell_asic ic[] // Array of the parsed cell codes
	   )
	{

		int8_t pec_error = 0;	//
		pec_error = LTC681x_rdcv(reg, total_ic, ic);
		return (pec_error);
	}

//	//Helper function to set CFGR variable
//	void LTC6811SPI::LTC681x_set_cfgr(uint8_t nIC, cell_asic ic[],
//			bool refon, bool adcopt, bool gpio[5], bool dcc[12], bool dtco[4])
//	{
//		LTC681x_set_cfgr_refon(nIC, ic, refon);
//		LTC681x_set_cfgr_adcopt(nIC, ic, adcopt);
//		LTC681x_set_cfgr_gpio(nIC, ic, gpio);
//		LTC681x_set_cfgr_dis(nIC, ic, dcc);
//		
//		LTC681x_set_cfgr_dcto(nIC, ic, dtco);
//		LTC681x_set_cfgr_uv(nIC, ic, UV_THRESHOLD);
//		LTC681x_set_cfgr_ov(nIC, ic, OV_THRESHOLD);
//		
//		
//	}
	
	//Helper function to set CFGR variable
	void LTC6811SPI::LTC681x_set_cfgr1(uint8_t nIC, Cell_asic ic[],	bool refon, bool adcopt, bool gpio[5])
	{
		LTC681x_set_cfgr_refon(nIC, ic, refon);
		LTC681x_set_cfgr_adcopt(nIC, ic, adcopt);
		LTC681x_set_cfgr_gpio(nIC, ic, gpio);
	}
	
	//Helper function to set CFGR variable
	void LTC6811SPI::LTC681x_set_cfgr2(uint8_t nIC, Cell_asic ic[], bool dcc[12], bool dtco[4])
	{
		LTC681x_set_cfgr_dis(nIC, ic, dcc);
		
		LTC681x_set_cfgr_dcto(nIC, ic, dtco);
		LTC681x_set_cfgr_uv(nIC, ic, UV_THRESHOLD);
		LTC681x_set_cfgr_ov(nIC, ic, OV_THRESHOLD);
		
		
	}
	
	
	//Helper function to intialize CFG variables.
	void LTC6811SPI::LTC681x_init_cfg(uint8_t total_ic, Cell_asic ic[])
	{
		bool_t REFON = true;	//init
		bool_t ADCOPT = false;	//init
		bool_t gpioBits[5] = { true, true, true, true, true };	//init
		bool_t dccBits[12] = { false, false, false, false, false, false, false, false, false, false, false, false };	//init
		bool_t dctoBits[4] = { false, false, false, false };	//init
		//MAX_PACK_NUM
		for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++)
		{
			for (uint8_t j = 0U; j < 6; j++) //
			{
				ic[current_ic].config.tx_data[j] = 0U;
				ic[current_ic].configb.tx_data[j] = 0U;
			}
//			LTC681x_set_cfgr(current_ic, ic, REFON, ADCOPT, gpioBits, dccBits, dctoBits);
			LTC681x_set_cfgr1(current_ic, ic, REFON, ADCOPT, gpioBits);
			LTC681x_set_cfgr2(current_ic, ic, dccBits, dctoBits);

		}
	}

	//Helper function to set the REFON bit
	void LTC6811SPI::LTC681x_set_cfgr_refon(uint8_t nIC, Cell_asic ic[], bool refon)
	{
		if (refon)
		{
			ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0] | 0x04U;
		}
		else 
		{
			ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0] & 0xFBU;
		}
	}

	//Helper function to set the adcopt bit
	void LTC6811SPI::LTC681x_set_cfgr_adcopt(uint8_t nIC, Cell_asic ic[], bool adcopt)
	{
		if (adcopt)
		{
			ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0] | 0x01U;
		}
		else
		{
			ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0] & 0xFEU;
		}
	}

	//Helper function to set GPIO bits
	void LTC6811SPI::LTC681x_set_cfgr_gpio(uint8_t nIC, Cell_asic ic[], bool gpio[5])
	{
		for (int8_t i = 0; i < 5U; i++)
		{
			if (gpio[i])
			{
				ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0] | (0x01U << (i + 3U));
			}
			else
			{
				ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]&(~(0x01U << (i + 3U)));
			}
		}
	}

	//Helper function to control discharge
	void LTC6811SPI::LTC681x_set_cfgr_dis(uint8_t nIC, Cell_asic ic[], bool dcc[12])
	{
		//
		for (int8_t i = 0; i < 8; i++)
		{
			if (dcc[i])
			{
				ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4] | (0x01U << i);
			}
			else 
			{
				ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4]& (~(0x01U << i));
			}
		}
		//
		for (int8_t i = 0; i < 4; i++)
		{
			if (dcc[i + 8])
			{
				ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5] | (0x01U << i);
			}
			else 
			{
				ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]&(~(0x01U << i));
			}
		}
	}

	//Helper Function to set uv value in CFG register
	void LTC6811SPI::LTC681x_set_cfgr_uv(uint8_t nIC, Cell_asic ic[], uint16_t uv)
	{
		const uint16_t tmp = (uv / 16U) - 1U;	//init
		ic[nIC].config.tx_data[1] = 0x00FFU & tmp;
		ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2] & 0xF0U;
		ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2] | ((0x0F00U & tmp) >> 8U);
	}

	//helper function to set OV value in CFG register
	void LTC6811SPI::LTC681x_set_cfgr_ov(uint8_t nIC, Cell_asic ic[], uint16_t ov)
	{
		uint16_t tmp = ov / 16;	//init
		ic[nIC].config.tx_data[3] = 0x00FF & (tmp >> 4U);
		ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2] & 0x0FU;
		ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2] | ((0x000F & tmp) << 4U);
	}

	//Helper Function to reset PEC counters
	void LTC6811SPI::LTC681x_reset_crc_count(uint8_t total_ic, Cell_asic ic[])
	{
		//MAX_PACK_NUM
		for (uint16_t current_ic = 0U; current_ic < total_ic; current_ic++) {
			ic[current_ic].crc_count.pec_count = 0U;
			ic[current_ic].crc_count.cfgr_pec = 0U;
			//
			for (uint16_t i = 0U; i < 6U; i++)
			{
				ic[current_ic].crc_count.cell_pec[i] = 0U;

			}
			//
			for (uint16_t i = 0U; i < 4U; i++)
			{
				ic[current_ic].crc_count.aux_pec[i] = 0U;
			}
			//
			for (uint16_t i = 0U; i < 2U; i++)
			{
				ic[current_ic].crc_count.stat_pec[i] = 0U;
			}
		}
	}
	
	//
	void LTC6811SPI::LTC6811_init_reg_limits(uint8_t total_ic, Cell_asic ic[])
	{
		//MAX_PACK_NUM
		for (uint8_t cic = 0U; cic < total_ic; cic++)
		{
			ic[cic].ic_reg.cell_channels = 12U;
			ic[cic].ic_reg.stat_channels = 4U;
			ic[cic].ic_reg.aux_channels = 6U;
			ic[cic].ic_reg.num_cv_reg = 4U;
			ic[cic].ic_reg.num_gpio_reg = 2U;
			ic[cic].ic_reg.num_stat_reg = 3U;
		}
	}
	
	
	//Helper function that increments PEC counters
//	void LTC6811SPI::LTC681x_check_pec(uint8_t total_ic, uint8_t reg, Cell_asic ic[])
//	{
//		switch (reg) {
//		case CFGR:
//			//
//			for (uint16_t current_ic = 0U; current_ic < total_ic; current_ic++) {
//				ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].config.rx_pec_match;
//				ic[current_ic].crc_count.cfgr_pec = ic[current_ic].crc_count.cfgr_pec + ic[current_ic].config.rx_pec_match;
//			}
//			break;
//
//		case CFGRB:
//			//
//			for (uint16_t current_ic = 0U; current_ic < total_ic; current_ic++) {
//				ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].configb.rx_pec_match;
//				ic[current_ic].crc_count.cfgr_pec = ic[current_ic].crc_count.cfgr_pec + ic[current_ic].configb.rx_pec_match;
//			}
//			break;
//		case CELL:
//			//
//			for (uint16_t current_ic = 0U; current_ic < total_ic; current_ic++) {
//				//
//				for (uint16_t i = 0U; i < ic[0].ic_reg.num_cv_reg; i++) {
//					ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].cells.pec_match[i];
//					ic[current_ic].crc_count.cell_pec[i] = ic[current_ic].crc_count.cell_pec[i] + ic[current_ic].cells.pec_match[i];
//				}
//			}
//			break;
//		case AUX:
//			//
//			for (uint16_t current_ic = 0; current_ic < total_ic; current_ic++) {
//				//
//				for (uint16_t i = 0; i < ic[0].ic_reg.num_gpio_reg; i++) {
//					ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + (ic[current_ic].aux.pec_match[i]);
//					ic[current_ic].crc_count.aux_pec[i] = ic[current_ic].crc_count.aux_pec[i] + (ic[current_ic].aux.pec_match[i]);
//				}
//			}
//
//			break;		
//			
//		case STAT:
//			//
//			for (uint16_t current_ic = 0U; current_ic < total_ic; current_ic++) {
//				//
//				for (uint16_t i = 0U; i < ic[0].ic_reg.num_stat_reg - 1; i++) {
//					ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].stat.pec_match[i];
//					ic[current_ic].crc_count.stat_pec[i] = ic[current_ic].crc_count.stat_pec[i] + ic[current_ic].stat.pec_match[i];
//				}
//			}
//			break;
//		default:
//			break;
//		}
//	}
	
		/*
	 This command will write the configuration registers of the LTC6811-1s
	 connected in a daisy chain stack. The configuration is written in descending
	 order so the last device's configuration is written first.
	*/
	void LTC6811SPI::LTC6811_wrcfg(uint8_t total_ic, //The number of ICs being written to
		Cell_asic ic[] //A two dimensional array of the configuration data that will be written
	   )
	{
		LTC681x_wrcfg(total_ic, ic);
	}

	//Write the LTC681x CFGRA
	//The number of ICs being written to
	void LTC6811SPI::LTC681x_wrcfg(uint8_t total_ic, Cell_asic ic[])
	{
		uint8_t cmd[2] = { 0x00, 0x01 };	//init
//		uint8_t write_buffer[384];
		uint16_t write_count = 0U;
		uint8_t c_ic = 0U;	//init
		//MAX_PACK_NUM
		for (uint8_t current_ic = 0U; current_ic < total_ic; current_ic++)
		{
			if (ic->isospi_reverse == true) 
			{
				c_ic = current_ic;
			}
			else 
			{
				c_ic = total_ic - current_ic - 1;
			}

			for (uint8_t data = 0U; data < 6U; data++) //
			{
				write_buffer[write_count] = ic[c_ic].config.tx_data[data];
				write_count++;
			}
		}
		WRITE_6811(total_ic, cmd, write_buffer);
	}

	
	//Reads configuration registers of a LTC6811 daisy chain
	//Number of ICs in the system//A two dimensional array that the function stores the read configuration data.
	int8_t LTC6811SPI::LTC6811_rdcfg(uint8_t total_ic, Cell_asic ic[])
	{
		
		int8_t pec_error = LTC681x_rdcfg(total_ic, ic);//
		return (pec_error);
	}
	
	//Read CFGA
	//Number of ICs in the system
//	int8_t LTC6811SPI::LTC681x_rdcfg(uint8_t total_ic,Cell_asic ic[])
//	{
//		uint8_t cmd[2] = { 0x00U, 0x02U };//
//		uint8_t read_rdcfg_buffer[384] = { 0U };//
////		uint8_t read_buffer[384];
//
//		uint8_t c_ic = 0U;//
//		int8_t pec_error = READ_6811(total_ic, cmd, read_rdcfg_buffer);//
//		//MAX_PACK_NUM
//		for (uint8_t current_ic = 0U; current_ic < total_ic; current_ic++) {
//			if (ic->isospi_reverse == false) {
//				c_ic = current_ic;
//			}
//			else {
//				c_ic = total_ic - current_ic - 1;
//			}
//			//
//			for (uint16_t byte = 0U; byte < 8; byte++) {
//				ic[c_ic].config.rx_data[byte] = read_rdcfg_buffer[byte + (8U*current_ic)];
//			}
//			uint16_t calc_pec = PEC15_CALC(6, &read_rdcfg_buffer[8U*current_ic]);//
//			uint16_t data_pec = read_rdcfg_buffer[7U + (8*current_ic)] | (read_rdcfg_buffer[6U + (8U*current_ic)] << 8U);//
//			if (calc_pec != data_pec) {
//				ic[c_ic].config.rx_pec_match = 1U;
//			}
//			else ic[c_ic].config.rx_pec_match = 0U;
//		}
//		LTC681x_check_pec(total_ic, CFGR, ic);
//		return (pec_error);
//	}
//	
	//Check_error
	void LTC6811SPI::Check_error(int8_t error)
	{
		if (error == -1) 
		{
			error = 1;
			LTC6811SPI::error_message = error;

			//printf("A PEC error was detected in the received data \r\n");
		}
	}
	
	/*
	The function reads a single GPIO voltage register and stores thre read data
	in the *data point as a byte array. This function is rarely used outside of
	the LTC6811_rdaux() command.
	*/
	void LTC6811SPI::LTC681x_rdaux_reg(uint8_t reg, //Determines which GPIO voltage register is read back
		uint8_t total_ic, //The number of ICs in the system
		uint8_t *data //Array of the unparsed auxiliary codes
	   )
	{
		uint8_t REG_LEN = 8U; // number of bytes in the register + 2 bytes for the PEC
		uint8_t cmd[4];//
		uint16_t cmd_pec = 0U;//


		if (reg == 1U) {
			//Read back auxiliary group A
			cmd[1] = 0x0CU;
			cmd[0] = 0x00U;
		}
		else if (reg == 2U) {
			//Read back auxiliary group B
			cmd[1] = 0x0eU;
			cmd[0] = 0x00U;
		}
		else if (reg == 3U) {
			//Read back auxiliary group C
			cmd[1] = 0x0DU;
			cmd[0] = 0x00U;
		}
		else if (reg == 4U) {
			//Read back auxiliary group D
			cmd[1] = 0x0FU;
			cmd[0] = 0x00U;
		}
		else {
			//Read back auxiliary group A
			cmd[1] = 0x0CU;
			cmd[0] = 0x00U;
		}

		cmd_pec = PEC15_CALC(2U, cmd);
		cmd[2] = (uint8_t)(cmd_pec >> 8U);
		cmd[3] = (uint8_t)(cmd_pec);

		spi4.lock();
		CS = 0;
		SPI_write_read(cmd, 4U, data, (REG_LEN*total_ic));
		CS = 1;
		spi4.unlock();

	}
	
	/*
	The function is used to read the  parsed GPIO codes of the LTC6811. 
	This function will send the requested read commands parse the data 
	and store the gpio voltages in a_codes variable
	*/
	int8_t LTC6811SPI::LTC6811_rdaux(uint8_t reg, //Determines which GPIO voltage register is read back.
		uint8_t total_ic,//The number of ICs in the system
		Cell_asic *ic//A two dimensional array of the gpio voltage codes.
		)
	{
		int8_t pec_error = 0;	//init
		LTC681x_rdaux(reg, total_ic, ic);
		return (pec_error);
	}
	
	/*
	The function is used
	to read the  parsed GPIO codes of the LTC6811. This function will send the requested
	read commands parse the data and store the gpio voltages in aux_codes variable
	*/
//	int8_t LTC6811SPI::LTC681x_rdaux(uint8_t reg, //Determines which GPIO voltage register is read back.
//		uint8_t total_ic,//the number of ICs in the system
//		Cell_asic ic[]//A two dimensional array of the gpio voltage codes.
//	   )
//	{
////		uint8_t *data;
//		int8_t pec_error = 0;
//		uint8_t c_ic = 0;	//init
//		uint8_t data_rdaux[384] = { 0 };
//		
//		if (reg == 0) 
//		{
//			for (uint8_t gpio_reg = 1; gpio_reg < ic[0].ic_reg.num_gpio_reg + 1; gpio_reg++) 
//			{
//				//executes once for each of the LTC6811 aux voltage registers
//				LTC681x_rdaux_reg(gpio_reg, total_ic, data_rdaux); //Reads the raw auxiliary register data into the data[] array
//				//
//				for (int current_ic = 0; current_ic < total_ic; current_ic++) 
//				{
//					if (ic->isospi_reverse == false) 
//					{
//						c_ic = current_ic;
//					}
//					else 
//					{
//						c_ic = total_ic - current_ic - 1;
//					}
//					pec_error = Parse_cells(current_ic,	gpio_reg, data_rdaux, &ic[c_ic].aux.a_codes[0],	&ic[c_ic].aux.pec_match[0]);
//
//				}
//			}
//		}
//		else 
//		{
//			LTC681x_rdaux_reg(reg, total_ic, data_rdaux);
//			//
//			for (int current_ic = 0; current_ic < total_ic; current_ic++) 
//			{
//				if (ic->isospi_reverse == false) 
//				{
//					c_ic = current_ic;
//				}
//				else 
//				{
//					c_ic = total_ic - current_ic - 1;
//				}
//				pec_error = Parse_cells(current_ic,	reg, data_rdaux, &ic[c_ic].aux.a_codes[0], &ic[c_ic].aux.pec_match[0]);
//			}
//
//		}
//		LTC681x_check_pec(total_ic, AUX, ic);
//
//		return (pec_error);
//	}

	//Start a Status ADC Conversion
	void LTC6811SPI::LTC681x_adstat(uint8_t MD,uint8_t CHST)//ADC Mode//GPIO Channels to be measured
	{
		uint8_t cmd[4];//
		uint8_t md_bits = 0U;//

		md_bits = (MD & 0x02U) >> 1U;
		cmd[0] = md_bits + 0x04U;
		md_bits = (MD & 0x01U) << 7U;
		cmd[1] = md_bits + 0x68U + CHST;
		spi4.lock();
		CS = 0;
		CMD_6811(cmd);
		CS = 1;
		spi4.unlock();
	}

	/*
	 Reads and parses the LTC6811 stat registers.
	 The function is used
	 to read the  parsed stat codes of the LTC6811. This function will send the requested
	 read commands parse the data and store the stat voltages in stat_codes variable
	*/
	int8_t LTC6811SPI::LTC6811_rdstat(uint8_t reg, //Determines which Stat  register is read back.
		uint8_t total_ic,//the number of ICs in the system
		Cell_asic ic[])
	{
		int8_t pec_error = 0;	//init
		pec_error = LTC681x_rdstat(reg, total_ic, ic);
		return (pec_error);
	}
	
	// Reads and parses the LTC681x stat registers.
//	int8_t LTC6811SPI::LTC681x_rdstat(uint8_t reg, //Determines which Stat  register is read back.
//		uint8_t total_ic,//the number of ICs in the system
//		Cell_asic ic[])
//	{
//		uint8_t BYT_IN_REG = 6U;	//const
//		uint8_t GPIO_IN_REG = 3U;	//const
//
////		uint8_t *data;
//		uint8_t data_counter = 0U;
//		int8_t pec_error = 0;	//init
//		uint16_t parsed_stat = 0U;	//init
//		uint16_t received_pec = 0U;	//init
//		uint16_t data_pec = 0U;	//init
//		uint8_t c_ic = 0U;	//init
////		data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));
//		
//		uint8_t data_rdstat[384] = { 0 };	//init
//		
//		if (reg == 0U) {
//			//
//			for (uint8_t stat_reg = 1U; stat_reg < 3U; stat_reg++) {
//				//executes once for each of the LTC6811 stat voltage registers
//				data_counter = 0;
//				LTC681x_rdstat_reg(stat_reg, total_ic, data_rdstat); //Reads the raw statiliary register data into the data[] array
//				//
//				for (uint8_t current_ic = 0U; current_ic < total_ic; current_ic++) {
//					// executes for every LTC6811 in the daisy chain
//					if (ic->isospi_reverse == false) {
//						c_ic = current_ic;
//					}
//					else {
//						c_ic = total_ic - current_ic - 1;
//					}
//					// current_ic is used as the IC counter
//					if (stat_reg == 1) {
//						//
//						for (uint8_t current_gpio = 0; current_gpio < GPIO_IN_REG; current_gpio++) {
//							// This loop parses the read back data into GPIO voltages, it
//						    // loops once for each of the 3 gpio voltage codes in the register
//							//Each gpio codes is received as two bytes and is combined to
//							parsed_stat = data_rdstat[data_counter] + (data_rdstat[data_counter + 1] << 8);
//
//							ic[c_ic].stat.stat_codes[current_gpio] = parsed_stat;
//							data_counter = data_counter + 2; //Because gpio voltage codes are two bytes the data counter
//
//						}
//					}
//					else if (stat_reg == 2U) {
//						//Each gpio codes is received as two bytes and is combined to
//						parsed_stat = data_rdstat[data_counter] + (data_rdstat[data_counter + 1] << 8);
//
//						data_counter = data_counter + 2;
//						ic[c_ic].stat.stat_codes[3] = parsed_stat;
//						ic[c_ic].stat.flags[0] = data_rdstat[data_counter++];
//						ic[c_ic].stat.flags[1] = data_rdstat[data_counter++];
//						ic[c_ic].stat.flags[2] = data_rdstat[data_counter++];
//						ic[c_ic].stat.mux_fail[0] = (data_rdstat[data_counter] & 0x02) >> 1;
//						ic[c_ic].stat.thsd[0] = data_rdstat[data_counter++] & 0x01;
//					}
//					//The received PEC for the current_ic is transmitted as the 7th and 8th
//					received_pec = (data_rdstat[data_counter] << 8) + data_rdstat[data_counter + 1];
//
//					//after the 6 gpio voltage data bytes
//					data_pec = PEC15_CALC(BYT_IN_REG, &data_rdstat[current_ic*NUM_RX_BYT]);
//
//					if (received_pec != data_pec) {
//						pec_error = -1; //The pec_error variable is simply set negative if any PEC errors
//						ic[c_ic].stat.pec_match[stat_reg - 1] = 1;
//						//are detected in the received serial data
//					}
//					else {
//						ic[c_ic].stat.pec_match[stat_reg - 1] = 0;
//					}
//
//					data_counter = data_counter + 2; //Because the transmitted PEC code is 2 bytes long the data_counter
//					//must be incremented by 2 bytes to point to the next ICs gpio voltage data
//				}
//
//
//			}
//
//		}
//		else {
//
//			LTC681x_rdstat_reg(reg, total_ic, data_rdstat);
//			//
//			for (uint16_t current_ic = 0U; current_ic < total_ic; current_ic++) {
//				// executes for every LTC6811 in the daisy chain
//			    // current_ic is used as an IC counter
//				if (ic->isospi_reverse == false) {
//					c_ic = current_ic;
//				}
//				else {
//					c_ic = total_ic - current_ic - 1;
//				}
//				if (reg == 1U) {
//					//
//					for (uint8_t current_gpio = 0U; current_gpio < GPIO_IN_REG; current_gpio++) {
//						// This loop parses the read back data into GPIO voltages, it
//					    // loops once for each of the 3 gpio voltage codes in the register
//						//Each gpio codes is received as two bytes and is combined to
//						parsed_stat = data_rdstat[data_counter] + (data_rdstat[data_counter + 1] << 8);
//
//						// create the parsed gpio voltage code
//
//						ic[c_ic].stat.stat_codes[current_gpio] = parsed_stat;
//						data_counter = data_counter + 2; //Because gpio voltage codes are two bytes the data counter
//						//must increment by two for each parsed gpio voltage code
//
//					}
//				}
//				else if (reg == 2U) {
//					//Each gpio codes is received as two bytes and is combined to
//					parsed_stat = data_rdstat[data_counter++] + (data_rdstat[data_counter++] << 8U);
//
//					ic[c_ic].stat.stat_codes[3] = parsed_stat;
//					ic[c_ic].stat.flags[0] = data_rdstat[data_counter++];
//					ic[c_ic].stat.flags[1] = data_rdstat[data_counter++];
//					ic[c_ic].stat.flags[2] = data_rdstat[data_counter++];
//					ic[c_ic].stat.mux_fail[0] = (data_rdstat[data_counter] & 0x02U) >> 1U;
//					ic[c_ic].stat.thsd[0] = data_rdstat[data_counter++] & 0x01U;
//				}
//
//				//The received PEC for the current_ic is transmitted as the 7th and 8th
//				received_pec = (data_rdstat[data_counter] << 8U) + data_rdstat[data_counter + 1U];
//
//				//after the 6 gpio voltage data bytes
//				data_pec = PEC15_CALC(BYT_IN_REG, &data_rdstat[current_ic*NUM_RX_BYT]);
//				if (received_pec != data_pec) {
//					pec_error = -1; //The pec_error variable is simply set negative if any PEC errors
//					ic[c_ic].stat.pec_match[reg - 1] = 1;
//
//				}
//
//				data_counter = data_counter + 2;
//			}
//		}
//		LTC681x_check_pec(total_ic, STAT, ic);
////		free(data);
//		return (pec_error);
//	}
	
	/*
	The function reads a single stat  register and stores the read data
	in the *data point as a byte array. This function is rarely used outside of
	the LTC6811_rdstat() command.
	*/
	void LTC6811SPI::LTC681x_rdstat_reg(uint8_t reg, //Determines which stat register is read back
		uint8_t total_ic, //The number of ICs in the system
		uint8_t *data //Array of the unparsed stat codes
	   )
	{
		uint8_t REG_LEN = 8U; // number of bytes in the register + 2 bytes for the PEC
		uint8_t cmd[4];//
		uint16_t cmd_pec =0U;//


		if (reg == 1U) {
			//Read back statiliary group A
			cmd[1] = 0x10U;
			cmd[0] = 0x00U;
		}
		else if (reg == 2U) {
			//Read back statiliary group B
			cmd[1] = 0x12U;
			cmd[0] = 0x00U;
		}

		else {
			//Read back statiliary group A
			cmd[1] = 0x10U;
			cmd[0] = 0x00U;
		}

		cmd_pec = PEC15_CALC(2, cmd);
		cmd[2] = (uint8_t)(cmd_pec >> 8U);
		cmd[3] = (uint8_t)(cmd_pec);

		spi4.lock();
		CS = 0;
		SPI_write_read(cmd, 4, data, (REG_LEN*total_ic));
		CS = 1;
		spi4.unlock();

	}
	
	// Runs the Digital Filter Self Test
	int16_t LTC6811SPI::LTC6811_run_cell_adc_st(uint8_t adc_reg, uint8_t total_ic, Cell_asic ic[])
	{
		int16_t error = 0;	//init
		error = LTC681x_run_cell_adc_st(adc_reg, total_ic, ic);
		return (error);
	}

	// Runs the Digital Filter Self Test
//	int16_t LTC6811SPI::LTC681x_run_cell_adc_st(uint8_t adc_reg, uint8_t total_ic, Cell_asic ic[])
//	{
//		int16_t error = 0;//
//		uint16_t expected_result = 0U;//
//		//
//		for (uint16_t self_test = 1U; self_test < 3U; self_test++)
//		{
//			expected_result = LTC6811SPI::LTC681x_st_lookup(2, self_test);
//			Wakeup_IDLE(total_ic);
//			switch (adc_reg) {
//			case CELL:
//				Wakeup_IDLE(total_ic);
//				LTC681x_clrcell();
//				LTC681x_cvst(2, self_test);
//				LTC681x_pollAdc(); //this isn't working
//				Wakeup_IDLE(total_ic);
//				error = LTC681x_rdcv(0, total_ic, ic);
//				//
//				for (uint16_t cic = 0U; cic < total_ic; cic++) {
//					//
//					for (int channel = 0; channel < ic[cic].ic_reg.cell_channels; channel++) {
//						if (ic[cic].cells.c_codes[channel] != expected_result) {
//							error = error + 1;
//						}
//					}
//				}
//				break;
//			case AUX:
//				error = 0;
//				Wakeup_IDLE(total_ic);
//				LTC681x_clraux();
//				LTC681x_axst(2, self_test);
//				LTC681x_pollAdc();
//				wait_ms(10);
//				Wakeup_IDLE(total_ic);
//				LTC681x_rdaux(0, total_ic, ic);
//				//
//				for (uint16_t cic = 0U; cic < total_ic; cic++) {
//					//
//					for (int channel = 0; channel < ic[cic].ic_reg.aux_channels; channel++) {
//						if (ic[cic].aux.a_codes[channel] != expected_result) {
//							error = error + 1;
//						}
//					}
//				}
//				break;
//			case STAT:
//				Wakeup_IDLE(total_ic);
//				LTC681x_clrstat();
//				LTC681x_statst(2, self_test);
//				LTC681x_pollAdc();
//				Wakeup_IDLE(total_ic);
//				error = LTC681x_rdstat(0, total_ic, ic);
//				//
//				for (uint16_t cic = 0U; cic < total_ic; cic++) {
//					//
//					for (uint16_t channel = 0U; channel < ic[cic].ic_reg.stat_channels; channel++) {
//						if (ic[cic].stat.stat_codes[channel] != expected_result) {
//							error = error + 1;
//						}
//					}
//				}
//				break;
//
//			default:
//				error = -1;
//				break;
//			}
//		}
//		return (error);
//	}
	//Looks up the result pattern for digital filter self test
	uint16_t LTC6811SPI::LTC681x_st_lookup(uint8_t MD,uint8_t ST)//ADC Mode//Self Test
	{
		uint16_t test_pattern = 0U;	//init
		if (MD == 1) {
			if (ST == 1) {
				test_pattern = 0x9565U;
			}
			else {
				test_pattern = 0x6A9AU;
			}
		}
		else {
			if (ST == 1) {
				test_pattern = 0x9555U;
			}
			else {
				test_pattern = 0x6AAAU;
			}
		}
		return (test_pattern);
	}

	/*
	The command clears the cell voltage registers and intiallizes
	all values to 1. The register will read back hexadecimal 0xFF
	after the command is sent.
	*/
	void LTC6811SPI::LTC681x_clrcell()
	{
		uint8_t cmd[2] = { 0x07U, 0x11U };	//init
		CMD_6811(cmd);
	}
	
	/*
	The command clears the Auxiliary registers and initializes
	all values to 1. The register will read back hexadecimal 0xFF
	after the command is sent.
	*/
	void LTC6811SPI::LTC681x_clraux()
	{
		uint8_t cmd[2] = { 0x07U, 0x12U };	//init
		CMD_6811(cmd);
	}
	
	/*
	The command clears the Stat registers and intiallizes
	all values to 1. The register will read back hexadecimal 0xFF
	after the command is sent.
	*/
	void LTC6811SPI::LTC681x_clrstat()
	{
		uint8_t cmd[2] = { 0x07U, 0x13U };	//init
		CMD_6811(cmd);
	}
	
	//This function will block operation until the ADC has finished it's conversion
	uint32_t LTC6811SPI::LTC681x_pollAdc()
	{
		uint32_t counter = 0U;	//init
		uint8_t finished = 0U;	//init
		uint8_t current_time = 0U;	//init
		uint8_t cmd[4];	//init

		cmd[0] = 0x07U;
		cmd[1] = 0x14U;
		uint16_t cmd_pec = PEC15_CALC(2U, cmd);//
		cmd[2] = (uint8_t)(cmd_pec >> 8U);
		cmd[3] = (uint8_t)(cmd_pec);

		spi4.lock();
		CS = 0;
		SPI_write_array(4U, cmd);

		while ((counter < 200000)&&(finished == 0)) 
		{
			current_time = SPI_read_byte(0xff);
			if (current_time > 0) 
			{
				finished = 1;
			}
			else 
			{
				counter = counter + 10;
			}
		}
		
		CS = 1;
		spi4.unlock();

		return (counter);
	}
	
	//Helper function to set discharge bit in CFG register
	void LTC6811SPI::LTC6811_set_discharge(int Cell, uint8_t total_ic, Cell_asic ic[])
	{
		for (uint16_t i = 0U; i < total_ic; i++)//
		{
			ic[i].config.tx_data[4] = ic[i].config.tx_data[4] | (Cell & 0x00FFU);
			ic[i].config.tx_data[5] = ic[i].config.tx_data[5] & 0x0F | Cell >> 8 & 0x000F;
			
			ic[i].config.tx_data[5] = (ic[i].config.tx_data[5] ) | ((0U <<4U) & 0xF0U);
			
			
//			if (Cell < 9)
//			{
//				ic[i].config.tx_data[4] = ic[i].config.tx_data[4] | (1 << (Cell - 1));
//			}
//			else if (Cell < 13)
//			{
//				ic[i].config.tx_data[5] = ic[i].config.tx_data[5] | (1 << (Cell - 9));
//			}
		}
	}

	//Clears all of the DCC bits in the configuration registers
	void LTC6811SPI::Clear_discharge(uint8_t total_ic, Cell_asic ic[])
	{
		for (uint16_t i = 0U; i < total_ic; i++) //
		{
			ic[i].config.tx_data[4] = 0U;
			ic[i].config.tx_data[5] = 0U;
		}
	}
	
	//Starts the Mux Decoder diagnostic self test
	void LTC6811SPI::LTC681x_diagn()
	{
		uint8_t cmd[2] = { 0x07U, 0x15U };//
		CMD_6811(cmd);
	}
	
	// Runs the ADC overlap test for the IC
	uint16_t LTC6811SPI::LTC6811_run_adc_overlap(uint8_t total_ic, Cell_asic ic[])
	{
		const uint16_t error = 0U;//
		LTC681x_run_adc_overlap(total_ic, ic);
		return (error);
	}
	// Runs the ADC overlap test for the IC
	uint16_t LTC6811SPI::LTC681x_run_adc_overlap(uint8_t total_ic, Cell_asic ic[])
	{
		uint16_t error = 0U;//
		int32_t measure_delta = 0;//
		int16_t failure_pos_limit = 20;//
		int16_t failure_neg_limit = -20;//

//		LTC681x_adol(MD_7KHZ_3KHZ, DCP_DISABLED);
//		LTC681x_pollAdc();

		error = LTC681x_rdcv(0, total_ic, ic);
		for (uint16_t cic = 0U; cic < total_ic; cic++) //
		{
			measure_delta = (int32_t)ic[cic].cells.c_codes[6] - (int32_t)ic[cic].cells.c_codes[7];
			if ((measure_delta > failure_pos_limit) || (measure_delta < failure_neg_limit))
			{
				error = error | (1 << (cic - 1));
			}
		}
		return (error);
	}
	
	//runs the redundancy self test
	int16_t LTC6811SPI::LTC6811_run_adc_redundancy_st(uint8_t adc_mode, uint8_t adc_reg, uint8_t total_ic, Cell_asic ic[])
	{
		int16_t error = 0;//
		LTC681x_run_adc_redundancy_st(adc_mode, adc_reg, total_ic, ic);
		return (error);
	}
	//runs the redundancy self test
//	int16_t LTC6811SPI::LTC681x_run_adc_redundancy_st(uint8_t adc_mode, uint8_t adc_reg, uint8_t total_ic, Cell_asic ic[])
//	{
//		int16_t error = 0;//
//		for (uint16_t self_test = 1U; self_test < 3U; self_test++) //
//		{
//			Wakeup_IDLE(total_ic);
//			switch (adc_reg) 
//			{
//			case AUX:
//				LTC681x_clraux();
//				LTC681x_adaxd(adc_mode, AUX_CH_ALL);
//				LTC681x_pollAdc();
//				Wakeup_IDLE(total_ic);
//				error = LTC681x_rdaux(0U, total_ic, ic);
//				//
//				for (uint16_t cic = 0U; cic < total_ic; cic++) {
//					//
//					for (uint16_t channel = 0U; channel < ic[cic].ic_reg.aux_channels; channel++) {
//						if (ic[cic].aux.a_codes[channel] >= 65280U) {
//							error = error + 1;
//						}
//					}
//				}
//				break;
//			case STAT:
//				LTC681x_clrstat();
//				LTC681x_adstatd(adc_mode, STAT_CH_ALL);
//				LTC681x_pollAdc();
//				Wakeup_IDLE(total_ic);
//				error = LTC681x_rdstat(0, total_ic, ic);
//				//
//				for (uint16_t cic = 0U; cic < total_ic; cic++) {
//					//
//					for (uint16_t channel = 0U; channel < ic[cic].ic_reg.stat_channels; channel++) {
//						if (ic[cic].stat.stat_codes[channel] >= 65280U) {
//							error = error + 1;
//						}
//					}
//				}
//				break;
//
//			default:
//				error = -1;
//				break;
//			}
//		}
//		return (error);
//	}
	
	
	/* Shifts data in COMM register out over LTC681x SPI/I2C port */
	void LTC6811SPI::LTC681x_stcomm(uint8_t len) //Length of data to be transmitted 
	{
		uint8_t cmd[4];//

		cmd[0] = 0x07;
		cmd[1] = 0x23;
		uint16_t cmd_pec = PEC15_CALC(2, cmd);//
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);
		
		spi4.lock();
		CS = 0;

		SPI_write_array(4, cmd);

		for (uint16_t i = 0; i < len * 3; i++)//
		{
			SPI_read_byte(0xFF);
		}

		CS = 1;	
		spi4.unlock();
	}

	
	//Writes the comm register
	//The number of ICs being written to
	void LTC6811SPI::LTC681x_wrcomm(uint8_t total_ic, Cell_asic ic[])
	{
		uint8_t cmd[2] = { 0x07, 0x21 };//
//		uint8_t write_buffer[384] = { 0 };
		uint8_t write_buffer_wrcomm[384] = { 0 };//
		uint16_t write_count = 0;//
		uint8_t c_ic = 0;//
		//
		for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++)
		{
			if (ic->isospi_reverse == true) 
			{
				c_ic = current_ic;
			}
			else 
			{
				c_ic = total_ic - current_ic - 1;
			}
			//
			for (uint8_t data = 0; data < 6; data++)
			{
				write_buffer_wrcomm[write_count] = ic[c_ic].com.tx_data[data];
				write_count++;
			}
		}
		WRITE_6811(total_ic, cmd, write_buffer_wrcomm);
	}

	
	//Reads COMM registers of a LTC6811 daisy chain
	//Number of ICs in the system
	int8_t LTC6811SPI::LTC681x_rdcomm(uint8_t total_ic, Cell_asic ic[])
	{
		uint8_t cmd[2] = { 0x07, 0x22 };//
//		uint8_t read_buffer[384];
	
		uint8_t c_ic = 0U;//
		int8_t pec_error = READ_6811(total_ic, cmd, read_buffer);//
		//
		for (uint8_t current_ic = 0U; current_ic < total_ic; current_ic++)
		{
			if (ic->isospi_reverse == false)
			{
				c_ic = current_ic;
			}
			else 
			{
				c_ic = total_ic - current_ic - 1U;
			}
			//
			for (uint8_t byte = 0U; byte < 8U; byte++)
			{
				ic[c_ic].com.rx_data[byte] = read_buffer[byte + (8U*current_ic)];
			}
			uint16_t calc_pec = PEC15_CALC(6U, &read_buffer[8*current_ic]);//
			uint16_t data_pec = read_buffer[7U + (8U*current_ic)] | (read_buffer[6U + (8U*current_ic)] << 8U);//
			if (calc_pec != data_pec) 
			{
				ic[c_ic].com.rx_pec_match = 1;
			}
			else
				ic[c_ic].com.rx_pec_match = 0;
		}
		return (pec_error);
	}
	
	//Runs the datasheet algorithm for open wire
	void LTC6811SPI::LTC681x_run_openwire(uint8_t total_ic, Cell_asic ic[])
	{
		uint16_t OPENWIRE_THRESHOLD = 4000U;//
		uint8_t  N_CHANNELS = ic[0].ic_reg.cell_channels;//

		Cell_asic pullUp_cell_codes[total_ic];//
		Cell_asic pullDwn_cell_codes[total_ic];//
		Cell_asic openWire_delta[total_ic];//
		int8_t error = 0;//

		Wakeup_SLEEP(total_ic);
		LTC681x_adow(MD_7KHZ_3KHZ, PULL_UP_CURRENT);
		LTC681x_pollAdc();
		Wakeup_IDLE(total_ic);
		LTC681x_adow(MD_7KHZ_3KHZ, PULL_UP_CURRENT);
		LTC681x_pollAdc();
		Wakeup_IDLE(total_ic);
		error = LTC681x_rdcv(0, total_ic, pullUp_cell_codes);

		Wakeup_IDLE(total_ic);
		LTC681x_adow(MD_7KHZ_3KHZ, PULL_DOWN_CURRENT);
		LTC681x_pollAdc();
		Wakeup_IDLE(total_ic);
		LTC681x_adow(MD_7KHZ_3KHZ, PULL_DOWN_CURRENT);
		LTC681x_pollAdc();
		Wakeup_IDLE(total_ic);
		error = LTC681x_rdcv(0, total_ic, pullDwn_cell_codes);

		//
		for (uint16_t cic = 0U; cic < total_ic; cic++) {
			ic[cic].system_open_wire = 0;
			//
			for (uint16_t cell = 0U; cell < N_CHANNELS; cell++) {
				if (pullDwn_cell_codes[cic].cells.c_codes[cell] > pullUp_cell_codes[cic].cells.c_codes[cell]) {
					openWire_delta[cic].cells.c_codes[cell] = pullDwn_cell_codes[cic].cells.c_codes[cell]
															- pullUp_cell_codes[cic].cells.c_codes[cell];
				}
				else {
					openWire_delta[cic].cells.c_codes[cell] = 0U;
				}

			}
		}
		//
		for (uint16_t cic = 0U; cic < total_ic; cic++) {
			//
			for (uint16_t cell = 1U; cell < N_CHANNELS; cell++) {

				if (openWire_delta[cic].cells.c_codes[cell] > OPENWIRE_THRESHOLD) {
					ic[cic].system_open_wire += (1 << cell);

				}
			}
			if (pullUp_cell_codes[cic].cells.c_codes[0] == 0U) {
				ic[cic].system_open_wire += 1;
			}
			if (pullUp_cell_codes[cic].cells.c_codes[N_CHANNELS - 1U] == 0U) {
				ic[cic].system_open_wire += (1U << (N_CHANNELS));
			}
		}
	}


	/* Helper function to control discharge time value */
	void LTC6811SPI::LTC681x_set_cfgr_dcto(uint8_t nIC, Cell_asic *ic, bool dcto[4])
	{
		for (uint8_t i = 0U; i < 4U; i++)//
		{
			//
			if (dcto[i])ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5] | (0x01U << (i + 4U));
			else
				ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]&(~(0x01U << (i + 4U)));
		}
	}
	
	/*  Write the LTC681x Sctrl register */
	void LTC6811SPI::LTC681x_wrsctrl(uint8_t total_ic, // Number of ICs in the daisy chain
		uint8_t sctrl_reg, // The Sctrl Register to be written A or B
		Cell_asic *ic  // A two dimensional array that stores the data to be written
	   )
	{
		uint8_t cmd[2];//
		uint8_t write_buffer_wrsctrl[384] = { 0U };//
		uint16_t write_count = 0U;//
		uint8_t c_ic = 0U;//
		if (sctrl_reg == 0U)
		{
			cmd[0] = 0x00U;
			cmd[1] = 0x14U;
		}
		else
		{
			cmd[0] = 0x00U;
			cmd[1] = 0x1CU;
		}
    
		//MAX_PACK_NUM
		for (uint8_t current_ic = 0U; current_ic < total_ic; current_ic++)
		{
			if (ic->isospi_reverse == false){c_ic = current_ic; }
			else{c_ic = total_ic - current_ic - 1U; }
		
			//
			for (uint8_t data = 0U; data < 6U; data++)
			{
				write_buffer_wrsctrl[write_count] = ic[c_ic].sctrl.tx_data[data];
				write_count++;
			}
		}
		WRITE_6811(total_ic, cmd, write_buffer_wrsctrl);
	}		
	
	/* 
	Start Sctrl data communication       
	This command will start the sctrl pulse communication over the spins
	*/
	void LTC6811SPI::LTC681x_stsctrl()
	{
		uint8_t cmd[4];//
    
		cmd[0] = 0x00U;
		cmd[1] = 0x19U;
		uint16_t cmd_pec = PEC15_CALC(2U, cmd);//
		cmd[2] = (uint8_t)(cmd_pec >> 8U);
		cmd[3] = (uint8_t)(cmd_pec);
    
		spi4.lock();
		CS = 0;
		SPI_write_array(4, cmd);          
		CS = 1;
		spi4.unlock();
	}
	
	/* Writes the pwm register */
	void LTC6811SPI::LTC681x_wrpwm(uint8_t total_ic, // Number of ICs in the daisy chain
		uint8_t pwmReg, // The PWM Register to be written A or B
		Cell_asic ic[] // A two dimensional array that stores the data to be written
	   )
	{
		uint8_t cmd[2];//
		uint8_t write_buffer_wrpwm[384] = { 0U };	//init
		uint16_t write_count = 0U;	//init
		uint8_t c_ic = 0U;	//init
		if (pwmReg == 0U)	//Write PWM Register Group
		{
			cmd[0] = 0x00U;
			cmd[1] = 0x20U;
		}
		else				//Write PWM/S Control Register Group B*
		{
			cmd[0] = 0x00U;
			cmd[1] = 0x1CU;
		}
	
		//MAX_PACK_NUM
		for (uint8_t current_ic = 0U; current_ic < total_ic; current_ic++)
		{
			if (ic->isospi_reverse == false)
			{
				c_ic = current_ic;
			}
			else
			{
				c_ic = total_ic - current_ic - 1U;
			}
			//
			for (uint8_t data = 0U; data < 6U; data++)
			{
				write_buffer_wrpwm[write_count] = ic[c_ic].pwm.tx_data[data];
				write_count++;
			}
		}
		WRITE_6811(total_ic, cmd, write_buffer_wrpwm);
	}


} //namespace bmstech
