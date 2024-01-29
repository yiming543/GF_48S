#include <mbed.h>
#include "LTC6811_daisy.h"
#include "PackConst.h"
#include "ADBMS6815_daisy.h"
#include "adBms6815CmdList.h"


//extern bmstech::ADBMS6815::Cell_asic bms6815_ic[bmstech::PackConst::MAX_PACK_NUM];

namespace bmstech
{
	extern DigitalOut CS; // ssel
	extern SPI spi4; // mosi, miso, sclk
	
	
	/**
	*******************************************************************************
	* Function: Pec15_Calc
	* @brief CRC15 Pec Calculation Function
	*
	* @details This function calculates and return the CRC15 value
	*
	* Parameters:
	* @param [in]   Len Data length
	*
	* @param [in] *data    Data pointer
	*
	* @return CRC15_Value
	*
	*******************************************************************************
	*/
	uint16_t ADBMS6815::Pec15_Calc
	(
		uint8_t len, /* Number of bytes that will be used to calculate a PEC */
		uint8_t *data /* Array of data that will be used to calculate  a PEC */
		)
	{
		uint16_t remainder, addr;
		remainder = 16; /* initialize the PEC */
		for (uint8_t i = 0; i < len; i++) /* loops for each byte in data array */
		{
			addr = (((remainder >> 7) ^ data[i]) & 0xff); /* calculate PEC table address */
			remainder = ((remainder << 8) ^ crc15Table[addr]);
		}
		return (remainder * 2);/* The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2 */
	}

	uint16_t ADBMS6815::pec10_calc(bool rx_cmd, int len, uint8_t *data)
	{
		uint16_t remainder = 16; /* PEC_SEED;   0000010000 */
		uint16_t polynom = 0x8F; /* x10 + x7 + x3 + x2 + x + 1 <- the CRC15 polynomial         100 1000 1111   48F */

		/* Perform modulo-2 division, a byte at a time. */
		for (uint8_t pbyte = 0; pbyte < len; ++pbyte)
		{
			/* Bring the next byte into the remainder. */
			remainder ^= (uint16_t)(data[pbyte] << 2);
			/* Perform modulo-2 division, a bit at a time.*/
			for (uint8_t bit_ = 8; bit_ > 0; --bit_)
			{
				/* Try to divide the current data bit. */
				if ((remainder & 0x200) > 0)//equivalent to remainder & 2^14 simply check for MSB
				{
					remainder = (uint16_t)((remainder << 1));
					remainder = (uint16_t)(remainder ^ polynom);
				}
				else
				{
					remainder = (uint16_t)(remainder << 1);
				}
			}
		}
		if (rx_cmd == true)
		{
			remainder ^= (uint16_t)((data[len] & 0xFC) << 2);
			/* Perform modulo-2 division, a bit at a time */
			for (uint8_t bit_ = 6; bit_ > 0; --bit_)
			{
				/* Try to divide the current data bit */
				if ((remainder & 0x200) > 0)//equivalent to remainder & 2^14 simply check for MSB
				{
					remainder = (uint16_t)((remainder << 1));
					remainder = (uint16_t)(remainder ^ polynom);
				}
				else
				{
					remainder = (uint16_t)((remainder << 1));
				}
			}
		}
		return ((uint16_t)(remainder & 0x3FF));
	}
	
	
		
	void ADBMS6815::spiSendCmd(uint8_t tx_cmd[2])
	{
		uint8_t cmd[4];
		uint16_t cmd_pec;
		cmd[0] = tx_cmd[0];
		cmd[1] =  tx_cmd[1];
		cmd_pec = LTC6811SPI::PEC15_CALC(2, cmd);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);

		spi4.lock();
		CS = 0;		
		ADBMS6815::spiWriteBytes(4, &cmd[0]);	
		CS = 1;
		spi4.unlock();
	}
	
	//*******************************************************************************
	//* Function : adBms6815_Adcv
	//* @ brief ADCV Command.
	//*
	//* @ details Send adcv command to start cell voltage conversion.
	//*
	//* Parameters :
	//*
	//* @ param[in]  MD       Enum type adc mode
	//*
	//* @ param[in]  DCP      Enum type discharge permit bit
	//*
	//* @ param[in]  CHCELL   Enum type cell channels to be measured
	//*
	//* @ return None
	//*
	//*******************************************************************************
	void ADBMS6815::adBms6815_Adcv(
                ADC_MODE MD, //ADC Mode
	DCC_BIT DCP, //Discharge Permit
	CH CHCELL //Cell Channels to be measured
		)
	{
		uint8_t cmd[2];
		uint8_t md_bits;
		md_bits = (MD & 0x02) >> 1;
		cmd[0] = md_bits + 0x02;
		md_bits = (MD & 0x01) << 7;
		cmd[1] =  md_bits + 0x60 + (DCP << 4) + CHCELL;
		ADBMS6815::spiSendCmd(cmd);
	}
	
	
	/**
	 *******************************************************************************
	 * Function: spiWriteBytes
	 * @brief Writes an array of bytes out of the SPI port.
	 *
	 * @details This function wakeup bms ic in IsoSpi mode send dumy byte data in spi line..
	 *
	 * @param [in]  size            Numberof bytes to be send on the SPI line
	 *
	 * @param [in]  *tx_Data    Tx data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	*/
	void ADBMS6815::spiWriteBytes
	(
	uint16_t size,                     /*Option: Number of bytes to be written on the SPI port*/
		uint8_t *tx_data                       /*Array of bytes to be written on the SPI port*/
		)
	{
		uint8_t rx_data[size];
		spi4.write((char *)tx_data, size, (char *)rx_data, size);
	}

	/**
	 *******************************************************************************
	 * Function: spiWriteReadBytes
	 * @brief Writes and read a set number of bytes using the SPI port.
	 *
	 * @details This function writes and read a set number of bytes using the SPI port.
	 *
	 * @param [in]  *tx_data    Tx data pointer
	 *
	 * @param [in]  *rx_data    Rx data pointer
	 *
	 * @param [in]  size            Data size
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void spiWriteReadBytes
	(
	uint8_t *tx_data,                   /*array of data to be written on SPI port*/
		uint8_t *rx_data,                   /*Input: array that will store the data read by the SPI port*/
		uint16_t size                           /*Option: number of bytes*/
		)
	{
		uint16_t data_size = (4 + size);
		uint8_t cmd[data_size];
		memcpy(&cmd[0], &tx_data[0], 4); /* dst, src, size */
		spi4.write((char *)cmd, data_size, (char *)cmd, data_size);
		memcpy(&rx_data[0], &cmd[4], size); /* dst, src, size */
	}

	/**
	 *******************************************************************************
	 * Function: spiReadBytes
	 * @brief Read number of bytes using the SPI port.
	 *
	 * @details This function Read a set number of bytes using the SPI port.
	 *
	 * @param [in]  size            Data size
	 *
	 * @param [in]  *rx_data    Rx data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void spiReadBytes(uint16_t size, uint8_t *rx_data)
	{
		rx_data[0] = spi4.write(0xff);
		uint8_t tx_data[size];
		for (uint8_t i = 0; i < size; i++)
		{
			tx_data[i] = 0xFF;
		}
		spi4.write((char *)tx_data, size, (char *)rx_data, size);
	}
	
	void ADBMS6815::Start_ADC_Measurement()
	{
		ADBMS6815::adBms6815_Adcv(MODE_26Hz_TO_2KHz, DCC_BIT_CLR, ALL_CELLS);
	}
	
	/**
	*******************************************************************************
	* @brief ADCV Read Cell voltages
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_read_cell_voltages(uint8_t tIC, cell_asic *ic)
	{
		//adBmsWakeupIc(tIC);
		adBmsReadData(tIC, &ic[0], RDCVA, Cell, A);
		adBmsReadData(tIC, &ic[0], RDCVB, Cell, B);
		adBmsReadData(tIC, &ic[0], RDCVC, Cell, C);
		adBmsReadData(tIC, &ic[0], RDCVD, Cell, D);

//		printf("Read cell voltages:\n");
//		printVoltages(tIC, &ic[0], Cell);
		
	}

	/**
	*******************************************************************************
	* Function: adBms6815_Cvow
	* @brief Cvow cell open wire Conversion.
	*
	* @details Start cell open wire Conversion.
	*
	* Parameters:
	*
	* @param [in]  MD      Enum type adc mode
	*
	* @return None
	*
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_Cvow(
	                    ADC_MODE MD //ADC Mode
	                      )
	{
		uint8_t cmd[2];
		uint8_t md_bits;
		md_bits = (MD & 0x02) >> 1;
		cmd[0] = md_bits + 0x02;
		md_bits = (MD & 0x01) << 7;
		cmd[1] = md_bits + 0x08;
		spiSendCmd(cmd);
	}
	
	/**
	*******************************************************************************
	* @brief Start AUX Voltages Measurement
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_start_aux_voltage_conversion(uint8_t tIC, cell_asic *ic, uint8_t channel)
	{
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			/* Init config A */
			ic[cic].tx_cfga.refon = PWR_UP;
			ic[cic].tx_cfga.gpo = channel;
		}
		
		adBmsWakeupIdle(tIC);
		
		adBmsWriteData(tIC, &ic[0], WRCFGA, Config, A);
		wait_us(900);//wait mul stable 1
		wait_us(900);//wait mul stable 2
		wait_us(900);//wait mul stable 3
		wait_us(900);//wait mul stable 4
		wait_us(900);//wait mul stable 5
		wait_us(900);//wait mul stable 6
		wait_us(900);//wait mul stable 7
		wait_us(900);//wait mul stable 8
		wait_us(900);//wait mul stable 9
		wait_us(900);//wait mul stable 10
		wait_us(900);//wait mul stable 11
		wait_us(900);//wait mul stable 12
		wait_us(900);//wait mul stable 13
		wait_us(900);//wait mul stable 14
		adBmsWakeupIdle(tIC);

//		ADBMS6815::adBmss6815_Adax(ADBMS6815::MODE_7KHz_TO_3KHz, ADBMS6815::GPIOS_VREF2_VREF3);
		ADBMS6815::adBmss6815_Adax(ADBMS6815::MODE_26Hz_TO_2KHz, ADBMS6815::GPIOS_VREF2_VREF3);
		adBmsPollAdc(PLADC);
//		pladc_count = adBmsPollAdc(PLADC);
//#ifdef MBED
//		pc.printf("Aux conversion completed\n");
//#else
//		printf("Aux conversion completed\n");
//#endif
//		printPollAdcConvTime(pladc_count);
	}
	
	/**
	*******************************************************************************
	* @brief Read AUX Voltages
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_read_aux_voltages(uint8_t tIC, cell_asic *ic)
	{
//		adBmsWakeupIdle(tIC);
		adBmsReadData(tIC, &ic[0], RDAUXA, Aux, A);
		adBmsReadData(tIC, &ic[0], RDAUXB, Aux, B);
		adBmsReadData(tIC, &ic[0], RDAUXC, Aux, C);
//		printf("Read Aux voltages:\n");
//		printVoltages(tIC, &ic[0], Aux);
	}
	
	
	/**
	*******************************************************************************
	* Function: spiReadData
	* @brief Spi Read Bms Data
	*
	* @details This function send bms command in spi line and read command corrospond data byte.
	*
	* Parameters:
	* @param [in]   tIC     Total IC
	*
	* @param [in]  tx_cmd   Tx command bytes
	*
	* @param [in]  *rx_data Rx data pointer
	*
	* @param [in]  *pec_error Pec error pointer
	*
	* @param [in]  *cmd_cntr command counter pointer
	*
	* @return None
	*
	*******************************************************************************
	*/
	void ADBMS6815::spiReadData
	(
	uint8_t tIC,
		uint8_t tx_cmd[2],
		uint8_t *rx_data,
		uint8_t *pec_error,
		uint8_t *cmd_cntr,
		uint8_t regData_size)
	{
		uint8_t *data, *copyArray, src_address = 0;
		uint16_t cmd_pec, received_pec, calculated_pec;
		uint8_t BYTES_IN_REG = regData_size;
		uint8_t RX_BUFFER = (regData_size * tIC);

		data = (uint8_t *)calloc(RX_BUFFER, sizeof(uint8_t));
		copyArray = (uint8_t *)calloc(BYTES_IN_REG, sizeof(uint8_t));
		if ((data == NULL) || (copyArray == NULL))
		{
#ifdef MBED
			pc.printf(" Failed to allocate spi read data memory \n");
#else
			printf(" Failed to allocate spi read data memory \n");
#endif
			exit(0);
		}
		else
		{
			uint8_t cmd[4];
			cmd[0] = tx_cmd[0];
			cmd[1] = tx_cmd[1];
			cmd_pec = Pec15_Calc(2, cmd);
			cmd[2] = (uint8_t)(cmd_pec >> 8);
			cmd[3] = (uint8_t)(cmd_pec);
			spi4.lock();
			CS = 0;	
			spiWriteReadBytes(&cmd[0], &data[0], RX_BUFFER); /* Read the configuration data of all ICs on the daisy chain into readdata array */
			CS = 1;
			spi4.unlock();
			for (uint8_t current_ic = 0; current_ic < tIC; current_ic++)     /* executes for each ic in the daisy chain and packs the data */
			{
				/* Into the r_comm array as well as check the received data for any bit errors */
				for (uint8_t current_byte = 0; current_byte < (BYTES_IN_REG - 2); current_byte++)
				{
					rx_data[(current_ic*BYTES_IN_REG) + current_byte] = data[current_byte + (current_ic*BYTES_IN_REG)];
				}
				/* Get command counter value */
				cmd_cntr[current_ic] = (data[(current_ic * BYTES_IN_REG) + (BYTES_IN_REG - 2)] >> 2);
				/* Get received pec value from ic*/
				received_pec = (uint16_t)(((data[(current_ic * BYTES_IN_REG) + (BYTES_IN_REG - 2)] & 0x03) << 8) | data[(current_ic * BYTES_IN_REG) + (BYTES_IN_REG - 1)]);
				/* Copy each ic correspond data + pec value for calculate data pec */
				memcpy(&copyArray[0], &data[src_address], BYTES_IN_REG);
				src_address = ((current_ic + 1) * (regData_size));
				/* Calculate data pec */
				calculated_pec = (uint16_t)pec10_calc(true, (BYTES_IN_REG - 2), &copyArray[0]);
				/* Match received pec with calculated pec */
				if (received_pec == calculated_pec){ pec_error[current_ic] = 0; }/* If no error is there value set to 0 */
				else{ pec_error[current_ic] = 1; }                               /* If error is there value set to 1 */
			}
		}
		free(data);
		free(copyArray);
	}
	
	/**
	*******************************************************************************
	* Function: spiWriteData
	* @brief Spi Write Bms Data
	*
	* @details This function write the data into bms ic.
	*
	* Parameters:
	* @param [in]   tIC             Total IC
	*
	* @param [in]  tx_cmd           Tx command bytes
	*
	* @param [in]  *data            Data pointer
	*
	* @param [in]  tx_size          Tx data size
	*
	* @return None
	*
	*******************************************************************************
	*/
	void ADBMS6815::spiWriteData(uint8_t tIC, uint8_t tx_cmd[2], uint8_t *data, uint8_t data_size)
	{
		uint8_t BYTES_IN_REG = data_size;
		uint8_t CMD_LEN = 4 + ((data_size + 2) * tIC);
		uint16_t data_pec, cmd_pec;
		uint8_t *cmd, *copyArray, src_address = 0;
		uint8_t cmd_index;
		copyArray = (uint8_t *)calloc(data_size, sizeof(uint8_t));
		cmd = (uint8_t *)calloc(CMD_LEN, sizeof(uint8_t));
		if ((cmd == NULL) || (copyArray == NULL))
		{
#ifdef MBED
			pc.printf(" Failed to allocate cmd array memory \n");
#else
			printf(" Failed to allocate cmd array memory \n");
#endif
			exit(0);
		}
		else
		{
			cmd[0] = tx_cmd[0];
			cmd[1] = tx_cmd[1];
			cmd_pec = ADBMS6815::Pec15_Calc(2, cmd);
			cmd[2] = (uint8_t)(cmd_pec >> 8);
			cmd[3] = (uint8_t)(cmd_pec);
			cmd_index = 4;
			/* executes for each LTC68xx, this loops starts with the last IC on the stack */
			for (uint8_t current_ic = tIC; current_ic > 0; current_ic--)
			{
				src_address = ((current_ic - 1) * data_size);
				/* The first configuration written is received by the last IC in the daisy chain */
				for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
				{
					cmd[cmd_index] = data[((current_ic - 1)*data_size) + current_byte];
					cmd_index = cmd_index + 1;
				}
				/* Copy each ic correspond data + pec value for calculate data pec */
				memcpy(&copyArray[0], &data[src_address], data_size); /* dst, src, size */
				/* calculating the PEC for each Ics configuration register data */
				data_pec = (uint16_t)pec10_calc(true, BYTES_IN_REG, &copyArray[0]);
				cmd[cmd_index] = (uint8_t)(data_pec >> 8);
				cmd_index = cmd_index + 1;
				cmd[cmd_index] = (uint8_t)data_pec;
				cmd_index = cmd_index + 1;
			}
		
			spi4.lock();
			CS = 0;		
			spiWriteBytes(CMD_LEN, &cmd[0]);
			CS = 1;
			spi4.unlock();
			
		}
		free(copyArray);
		free(cmd);
	}

	/**
	*******************************************************************************
	* Function: adBmsReadData
	* @brief Adbms Read Data From Bms ic.
	*
	* @details This function send bms command, read payload data parse into function and check pec error.
	*
	* Parameters:
	* @param [in]   tIC      Total IC
	*
	* @param [in]  *ic      cell_asic stucture pointer
	*
	* @param [in]  cmd_arg   command bytes
	*
	* @param [in]  TYPE   Enum type of resistor
	*
	* @param [in]  GRP   Enum type of resistor group
	*
	* @return None
	*
	*******************************************************************************
	*/
	void ADBMS6815::adBmsReadData(uint8_t tIC, cell_asic *ic, uint8_t cmd_arg[2], TYPE type, GRP group)
	{
		uint16_t rBuff_size;
		uint16_t regData_size;
		if (group == ALL_GRP)
		{
			if (type == Rdcvall){rBuff_size = RDCVALL_SIZE; regData_size = RDCVALL_SIZE; }
			else if (type == Rdcdall){rBuff_size = RDCDALL_SIZE; regData_size = RDCDALL_SIZE; }
			else if (type == Rdasall){rBuff_size = RDASALL_SIZE; regData_size = RDASALL_SIZE; }
			else{printf("Read All cmd wrong type select \n"); }
		}
		else{rBuff_size = (tIC * RX_DATA); regData_size = RX_DATA; }
		uint8_t *read_buffer, *pec_error, *cmd_count;
		read_buffer = (uint8_t *)calloc(rBuff_size, sizeof(uint8_t));
		pec_error = (uint8_t *)calloc(tIC, sizeof(uint8_t));
		cmd_count = (uint8_t *)calloc(tIC, sizeof(uint8_t));
		if ((pec_error == NULL) || (cmd_count == NULL) || (read_buffer == NULL))
		{
#ifdef MBED
			pc.printf(" Failed to allocate memory \n");
#else
			printf(" Failed to allocate memory \n");
#endif
			exit(0);
		}
		else
		{
			spiReadData(tIC, &cmd_arg[0], &read_buffer[0], &pec_error[0], &cmd_count[0], regData_size);
			switch (type)
			{
			case Config:
				adBms6815ParseConfig(tIC, ic, group, &read_buffer[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					ic[cic].cccrc.cfgr_pec = pec_error[cic];
					ic[cic].cccrc.cmd_cntr = cmd_count[cic];
				}
				break;

			case Cell:
				adBms6815ParseCell(tIC, ic, group, &read_buffer[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					ic[cic].cccrc.cell_pec = pec_error[cic];
					ic[cic].cccrc.cmd_cntr = cmd_count[cic];
				}
				break;

			case D_volt:
				adBms6815ParseDCell(tIC, ic, group, &read_buffer[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					ic[cic].cccrc.dcell_pec = pec_error[cic];
					ic[cic].cccrc.cmd_cntr = cmd_count[cic];
				}
				break;

			case Aux:
				adBms6815ParseAux(tIC, ic, group, &read_buffer[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					ic[cic].cccrc.aux_pec = pec_error[cic];
					ic[cic].cccrc.cmd_cntr = cmd_count[cic];
				}
				break;

			case Status:
				adBms6815ParseStatus(tIC, ic, group, &read_buffer[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					ic[cic].cccrc.stat_pec = pec_error[cic];
					ic[cic].cccrc.cmd_cntr = cmd_count[cic];
				}
				break;

			case Comm:
				adBms6815ParseComm(tIC, ic, &read_buffer[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					ic[cic].cccrc.comm_pec = pec_error[cic];
					ic[cic].cccrc.cmd_cntr = cmd_count[cic];
				}
				break;

			case Pwm:
				adBms6815ParsePwm(tIC, ic, &read_buffer[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					ic[cic].cccrc.pwm_pec = pec_error[cic];
					ic[cic].cccrc.cmd_cntr = cmd_count[cic];
				}
				break;

			case SID:
				adBms6815ParseSID(tIC, ic, &read_buffer[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					ic[cic].cccrc.sid_pec = pec_error[cic];
					ic[cic].cccrc.cmd_cntr = cmd_count[cic];
				}
				break;

			case Rdcvall:
				adBms6815ParseCell(tIC, ic, group, &read_buffer[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					ic[cic].cccrc.cell_pec = pec_error[cic];
					ic[cic].cccrc.cmd_cntr = cmd_count[cic];
				}
				break;

			case Rdcdall:
				adBms6815ParseDCell(tIC, ic, group, &read_buffer[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					ic[cic].cccrc.dcell_pec = pec_error[cic];
					ic[cic].cccrc.cmd_cntr = cmd_count[cic];
				}
				break;

			case Rdasall:
				/* 36 byte + 2 byte pec = 18 byte aux data + 18 byte status A,B and C data */
				adBms6815ParseAux(tIC, ic, group, &read_buffer[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					ic[cic].cccrc.aux_pec = pec_error[cic];
					ic[cic].cccrc.cmd_cntr = cmd_count[cic];
				}
				/* Status A base address read_buffer [18] */
				adBms6815ParseStatus(tIC, ic, group, &read_buffer[18]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					ic[cic].cccrc.stat_pec = pec_error[cic];
					ic[cic].cccrc.cmd_cntr = cmd_count[cic];
				}
				break;
			default:
				break;
			}
		}
		free(read_buffer);
		free(pec_error);
		free(cmd_count);
	}

	
	/**
	 *******************************************************************************
	 * Function: adBms6815ParseConfiga
	 * @brief Parse the recived Configuration register A data
	 *
	 * @details This function Parse the recived Configuration register A data.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  *data                   Data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	*/
	void ADBMS6815::adBms6815ParseConfiga(uint8_t tIC, cell_asic *ic, uint8_t *data)
	{
		uint8_t address = 0;
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			memcpy(&ic[curr_ic].configa.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
			address = ((curr_ic + 1) * (RX_DATA));

			ic[curr_ic].rx_cfga.comm_bk = (ic[curr_ic].configa.rx_data[0] & 0x01);
			ic[curr_ic].rx_cfga.mcal   = (ic[curr_ic].configa.rx_data[0] & 0x02) >> 1;
			ic[curr_ic].rx_cfga.cvmin   = (ic[curr_ic].configa.rx_data[0] & 0x0C) >> 2;
			ic[curr_ic].rx_cfga.ps   = (ic[curr_ic].configa.rx_data[0] & 0x30) >> 4;
			ic[curr_ic].rx_cfga.adcopt   = (ic[curr_ic].configa.rx_data[0] & 0x40) >> 6;
			ic[curr_ic].rx_cfga.refon   = (ic[curr_ic].configa.rx_data[0] & 0x80) >> 7;

			ic[curr_ic].rx_cfga.flag_d  = (ic[curr_ic].configa.rx_data[1] & 0xFF);

			ic[curr_ic].rx_cfga.soakon   = (ic[curr_ic].configa.rx_data[2] & 0x80) >> 7;
			ic[curr_ic].rx_cfga.owrng    = (((ic[curr_ic].configa.rx_data[2] & 0x40) >> 6));
			ic[curr_ic].rx_cfga.owa      = ((ic[curr_ic].configa.rx_data[2] & 0x38) >> 3);
			ic[curr_ic].rx_cfga.owc      = ((ic[curr_ic].configa.rx_data[2] & 0x07));

			ic[curr_ic].rx_cfga.gpo        = ((ic[curr_ic].configa.rx_data[3] & 0x7F));
			ic[curr_ic].rx_cfga.gpi        = ((ic[curr_ic].configa.rx_data[4] & 0x7F));

			ic[curr_ic].rx_cfga.rev   = ((ic[curr_ic].configa.rx_data[5] & 0xF0) >> 4);
		}
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815ParseConfigb
	 * @brief Parse the recived Configuration register B data
	 *
	 * @details This function Parse the recived Configuration register B data.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  *data                   Data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815ParseConfigb(uint8_t tIC, cell_asic *ic, uint8_t *data)
	{
		uint8_t address = 0;
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			memcpy(&ic[curr_ic].configb.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
			address = ((curr_ic + 1) * (RX_DATA));

			ic[curr_ic].rx_cfgb.vuv = ((ic[curr_ic].configb.rx_data[0])  | ((ic[curr_ic].configb.rx_data[1] & 0x0F) << 8));
			ic[curr_ic].rx_cfgb.vov  = (ic[curr_ic].configb.rx_data[2] << 4) | ((ic[curr_ic].configb.rx_data[1] & 0xF0) >> 4);
			ic[curr_ic].rx_cfgb.dtmen = (((ic[curr_ic].configb.rx_data[3] & 0x80) >> 7));
			ic[curr_ic].rx_cfgb.dtrng = ((ic[curr_ic].configb.rx_data[3] & 0x40) >> 6);
			ic[curr_ic].rx_cfgb.dcto   = ((ic[curr_ic].configb.rx_data[3] & 0x3F));
			ic[curr_ic].rx_cfgb.dcc = ((ic[curr_ic].configb.rx_data[4]) | ((ic[curr_ic].configb.rx_data[5] & 0x0F) << 8));
			ic[curr_ic].rx_cfgb.mute_st = ((ic[curr_ic].configb.rx_data[5] & 0x80) >> 7);
		}
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815ParseConfig
	 * @brief Parse the recived Configuration register A & B data
	 *
	 * @details This function Parse the recived Configuration register A & B data.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  grp                     Enum type register group
	 *
	 * @param [in]  *data                   Data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815ParseConfig(uint8_t tIC, cell_asic *ic, GRP grp, uint8_t *data)
	{
		switch (grp)
		{
		case A:
			adBms6815ParseConfiga(tIC, &ic[0], &data[0]);
			break;

		case B:
			adBms6815ParseConfigb(tIC, &ic[0], &data[0]);
			break;

		default:
			break;
		}
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815ParseCell
	 * @brief Parse cell voltages
	 *
	 * @details This function Parse the recived cell register A,B,C and D data.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  grp                     Enum type register group
	 *
	 * @param [in]  *cv_data                Cell voltage data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815ParseCell(uint8_t tIC, cell_asic *ic, GRP grp, uint8_t *cv_data)
	{
		uint8_t *data, data_size, address = 0;
		if (grp == ALL_GRP){data_size = RDCVALL_SIZE; }
		else {data_size = RX_DATA; }
		data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
		if (data == NULL)
		{
			printf(" Failed to allocate parse cell memory \n");
			exit(0);
		}
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			memcpy(&data[0], &cv_data[address], data_size); /* dst , src , size */
			address = ((curr_ic + 1) * (data_size));
			switch (grp)
			{
			case A: /* Cell Register group A */
				ic[curr_ic].cell.c_codes[0] = (data[0] + (data[1] << 8));
				ic[curr_ic].cell.c_codes[1] = (data[2] + (data[3] << 8));
				ic[curr_ic].cell.c_codes[2] = (data[4] + (data[5] << 8));
				break;

			case B: /* Cell Register group B */
				ic[curr_ic].cell.c_codes[3] = (data[0] + (data[1] << 8));
				ic[curr_ic].cell.c_codes[4] = (data[2] + (data[3] << 8));
				ic[curr_ic].cell.c_codes[5] = (data[4] + (data[5] << 8));
				break;

			case C: /* Cell Register group C */
				ic[curr_ic].cell.c_codes[6] = (data[0] + (data[1] << 8));
				ic[curr_ic].cell.c_codes[7] = (data[2] + (data[3] << 8));
				ic[curr_ic].cell.c_codes[8] = (data[4] + (data[5] << 8));
				break;

			case D: /* Cell Register group D */
				ic[curr_ic].cell.c_codes[9] =  (data[0] + (data[1] << 8));
				ic[curr_ic].cell.c_codes[10] = (data[2] + (data[3] << 8));
				ic[curr_ic].cell.c_codes[11] = (data[4] + (data[5] << 8));
				break;

			case ALL_GRP: /* Cell Register group ALL */
				ic[curr_ic].cell.c_codes[0] = (data[0] + (data[1] << 8));
				ic[curr_ic].cell.c_codes[1] = (data[2] + (data[3] << 8));
				ic[curr_ic].cell.c_codes[2] = (data[4] + (data[5] << 8));
				ic[curr_ic].cell.c_codes[3] = (data[6] + (data[7] << 8));
				ic[curr_ic].cell.c_codes[4] = (data[8] + (data[9] << 8));
				ic[curr_ic].cell.c_codes[5] = (data[10] + (data[11] << 8));
				ic[curr_ic].cell.c_codes[6] = (data[12] + (data[13] << 8));
				ic[curr_ic].cell.c_codes[7] = (data[14] + (data[15] << 8));
				ic[curr_ic].cell.c_codes[8] = (data[16] + (data[17] << 8));
				ic[curr_ic].cell.c_codes[9] =  (data[18] + (data[19] << 8));
				ic[curr_ic].cell.c_codes[10] = (data[20] + (data[21] << 8));
				ic[curr_ic].cell.c_codes[11] = (data[22] + (data[23] << 8));
				break;

			default:
				break;
			}
		}
		free(data);
	}
	
	
	/**
	 *******************************************************************************
	 * Function: adBms6815ParseDCell
	 * @brief Parse DC voltages
	 *
	 * @details This function Parse the recived DC register A,B,C and D data.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  grp                     Enum type register group
	 *
	 * @param [in]  *dcv_data               Dc voltage data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	*/
	void ADBMS6815::adBms6815ParseDCell(uint8_t tIC, cell_asic *ic, GRP grp, uint8_t *dcv_data)
	{
		uint8_t *data, data_size, address = 0;
		if (grp == ALL_GRP){data_size = RDCDALL_SIZE; }
		else {data_size = RX_DATA; }
		data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
		if (data == NULL)
		{
#ifdef MBED
			pc.printf(" Failed to allocate parse dcell memory \n");
#else
			printf(" Failed to allocate parse dcell memory \n");
#endif
			exit(0);
		}
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			memcpy(&data[0], &dcv_data[address], data_size); /* dst , src , size */
			address = ((curr_ic + 1) * (data_size));
			switch (grp)
			{
			case A: /* Cell Register group A */
				ic[curr_ic].dcell.dc_codes[0] = (data[0] + (data[1] << 8));
				ic[curr_ic].dcell.dc_codes[1] = (data[2] + (data[3] << 8));
				ic[curr_ic].dcell.dc_codes[2] = (data[4] + (data[5] << 8));
				break;

			case B: /* Cell Register group B */
				ic[curr_ic].dcell.dc_codes[3] = (data[0] + (data[1] << 8));
				ic[curr_ic].dcell.dc_codes[4] = (data[2] + (data[3] << 8));
				ic[curr_ic].dcell.dc_codes[5] = (data[4] + (data[5] << 8));
				break;

			case C: /* Cell Register group C */
				ic[curr_ic].dcell.dc_codes[6] = (data[0] + (data[1] << 8));
				ic[curr_ic].dcell.dc_codes[7] = (data[2] + (data[3] << 8));
				ic[curr_ic].dcell.dc_codes[8] = (data[4] + (data[5] << 8));
				break;

			case D: /* Cell Register group D */
				ic[curr_ic].dcell.dc_codes[9] =  (data[0] + (data[1] << 8));
				ic[curr_ic].dcell.dc_codes[10] = (data[2] + (data[3] << 8));
				ic[curr_ic].dcell.dc_codes[11] = (data[4] + (data[5] << 8));
				break;

			case ALL_GRP: /* Cell Register group ALL */
				ic[curr_ic].dcell.dc_codes[0] = (data[0] + (data[1] << 8));
				ic[curr_ic].dcell.dc_codes[1] = (data[2] + (data[3] << 8));
				ic[curr_ic].dcell.dc_codes[2] = (data[4] + (data[5] << 8));
				ic[curr_ic].dcell.dc_codes[3] = (data[6] + (data[7] << 8));
				ic[curr_ic].dcell.dc_codes[4] = (data[8] + (data[9] << 8));
				ic[curr_ic].dcell.dc_codes[5] = (data[10] + (data[11] << 8));
				ic[curr_ic].dcell.dc_codes[6] = (data[12] + (data[13] << 8));
				ic[curr_ic].dcell.dc_codes[7] = (data[14] + (data[15] << 8));
				ic[curr_ic].dcell.dc_codes[8] = (data[16] + (data[17] << 8));
				ic[curr_ic].dcell.dc_codes[9] =  (data[18] + (data[19] << 8));
				ic[curr_ic].dcell.dc_codes[10] = (data[20] + (data[21] << 8));
				ic[curr_ic].dcell.dc_codes[11] = (data[22] + (data[23] << 8));
				break;

			default:
				break;
			}
		}
		free(data);
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815ParseAux
	 * @brief Parse aux voltages
	 *
	 * @details This function Parse the recived aux voltages register A,B and C data.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  grp                     Enum type register group
	 *
	 * @param [in]  *aux_data               Aux voltages data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815ParseAux(uint8_t tIC, cell_asic *ic, GRP grp, uint8_t *aux_data)
	{
		uint8_t *data, data_size, address = 0;
		if (grp == ALL_GRP){data_size = (RDASALL_SIZE - 18); }
		else {data_size = RX_DATA; }
		data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
		if (data == NULL)
		{
#ifdef MBED
			pc.printf(" Failed to allocate parse aux memory \n");
#else
			printf(" Failed to allocate parse aux memory \n");
#endif
			exit(0);
		}
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			memcpy(&data[0], &aux_data[address], data_size); /* dst , src , size */
			address = ((curr_ic + 1) * (data_size));
			switch (grp)
			{
			case A: /* Aux Register group A */
				ic[curr_ic].aux.a_codes[0] = (data[0] + (data[1] << 8));
				ic[curr_ic].aux.a_codes[1] = (data[2] + (data[3] << 8));
				ic[curr_ic].aux.a_codes[2] = (data[4] + (data[5] << 8));
				break;

			case B: /* Aux Register group B */
				ic[curr_ic].aux.a_codes[3] = (data[0] + (data[1] << 8));
				ic[curr_ic].aux.a_codes[4] = (data[2] + (data[3] << 8));
				ic[curr_ic].aux.a_codes[5] = (data[4] + (data[5] << 8));
				break;

			case C: /* Aux Register group C */
				ic[curr_ic].aux.a_codes[6] = (data[0] + (data[1] << 8));
				ic[curr_ic].aux.a_codes[7] = (data[2] + (data[3] << 8));
				ic[curr_ic].aux.a_codes[8] = (data[4] + (data[5] << 8));
				break;

			case ALL_GRP: /* Aux Register group ALL */
				ic[curr_ic].aux.a_codes[0] = (data[0] + (data[1] << 8));
				ic[curr_ic].aux.a_codes[1] = (data[2] + (data[3] << 8));
				ic[curr_ic].aux.a_codes[2] = (data[4] + (data[5] << 8));
				ic[curr_ic].aux.a_codes[3] = (data[6] + (data[7] << 8));
				ic[curr_ic].aux.a_codes[4] = (data[8] + (data[9] << 8));
				ic[curr_ic].aux.a_codes[5] = (data[10] + (data[11] << 8));
				ic[curr_ic].aux.a_codes[6] = (data[12] + (data[13] << 8));
				ic[curr_ic].aux.a_codes[7] = (data[14] + (data[15] << 8));
				ic[curr_ic].aux.a_codes[8] = (data[16] + (data[17] << 8));

			default:
				break;
			}
		}
		free(data);
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815ParseStatusA
	 * @brief Parse status A register data
	 *
	 * @details This function Parse the recived status A register data.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  *data                   data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815ParseStatusA(uint8_t tIC, cell_asic *ic, uint8_t *data)
	{
		uint8_t address = 0;
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			memcpy(&ic[curr_ic].stat.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
			address = ((curr_ic + 1) * (RX_DATA));
			ic[curr_ic].stata.sc   = (ic[curr_ic].stat.rx_data[0] | (ic[curr_ic].stat.rx_data[1] << 8));
			ic[curr_ic].stata.itmp = (ic[curr_ic].stat.rx_data[2] | (ic[curr_ic].stat.rx_data[3] << 8));
			ic[curr_ic].stata.va   = (ic[curr_ic].stat.rx_data[4] | (ic[curr_ic].stat.rx_data[5] << 8));
		}
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815ParseStatusB
	 * @brief Parse status B register data
	 *
	 * @details This function Parse the recived status B register data.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  *data                   data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815ParseStatusB(uint8_t tIC, cell_asic *ic, uint8_t *data)
	{
		uint8_t address = 0;
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			memcpy(&ic[curr_ic].stat.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
			address = ((curr_ic + 1) * (RX_DATA));

			ic[curr_ic].statb.vd   = (ic[curr_ic].stat.rx_data[0] + (ic[curr_ic].stat.rx_data[1] << 8));
			ic[curr_ic].statb.c1uv = (ic[curr_ic].stat.rx_data[2] & 0x01);
			ic[curr_ic].statb.c1ov = ((ic[curr_ic].stat.rx_data[2] & 0x02) >> 1);
			ic[curr_ic].statb.c2uv = ((ic[curr_ic].stat.rx_data[2] & 0x04) >> 2);
			ic[curr_ic].statb.c2ov = ((ic[curr_ic].stat.rx_data[2] & 0x08) >> 3);
			ic[curr_ic].statb.c3uv = ((ic[curr_ic].stat.rx_data[2] & 0x10) >> 4);
			ic[curr_ic].statb.c3ov = ((ic[curr_ic].stat.rx_data[2] & 0x20) >> 5);
			ic[curr_ic].statb.c4uv = ((ic[curr_ic].stat.rx_data[2] & 0x40) >> 6);
			ic[curr_ic].statb.c4ov = ((ic[curr_ic].stat.rx_data[2] & 0x80) >> 7);

			ic[curr_ic].statb.c5uv = (ic[curr_ic].stat.rx_data[3] & 0x01);
			ic[curr_ic].statb.c5ov = ((ic[curr_ic].stat.rx_data[3] & 0x02) >> 1);
			ic[curr_ic].statb.c6uv = ((ic[curr_ic].stat.rx_data[3] & 0x04) >> 2);
			ic[curr_ic].statb.c6ov = ((ic[curr_ic].stat.rx_data[3] & 0x08) >> 3);
			ic[curr_ic].statb.c7uv = ((ic[curr_ic].stat.rx_data[3] & 0x10) >> 4);
			ic[curr_ic].statb.c7ov = ((ic[curr_ic].stat.rx_data[3] & 0x20) >> 5);
			ic[curr_ic].statb.c8uv = ((ic[curr_ic].stat.rx_data[3] & 0x40) >> 6);
			ic[curr_ic].statb.c8ov = ((ic[curr_ic].stat.rx_data[3] & 0x80) >> 7);

			ic[curr_ic].statb.c9uv =  (ic[curr_ic].stat.rx_data[4] & 0x01);
			ic[curr_ic].statb.c9ov =  ((ic[curr_ic].stat.rx_data[4] & 0x02) >> 1);
			ic[curr_ic].statb.c10uv = ((ic[curr_ic].stat.rx_data[4] & 0x04) >> 2);
			ic[curr_ic].statb.c10ov = ((ic[curr_ic].stat.rx_data[4] & 0x08) >> 3);
			ic[curr_ic].statb.c11uv = ((ic[curr_ic].stat.rx_data[4] & 0x10) >> 4);
			ic[curr_ic].statb.c11ov = ((ic[curr_ic].stat.rx_data[4] & 0x20) >> 5);
			ic[curr_ic].statb.c12uv = ((ic[curr_ic].stat.rx_data[4] & 0x40) >> 6);
			ic[curr_ic].statb.c12ov = ((ic[curr_ic].stat.rx_data[4] & 0x80) >> 7);
			ic[curr_ic].statb.oc_cntr   = ic[curr_ic].stat.rx_data[5];
		}
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815ParseStatusC
	 * @brief Parse status C register data
	 *
	 * @details This function Parse the recived status C register data.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  *data                   data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815ParseStatusC(uint8_t tIC, cell_asic *ic, uint8_t *data)
	{
		uint8_t address = 0;
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			memcpy(&ic[curr_ic].stat.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
			address = ((curr_ic + 1) * (RX_DATA));
			ic[curr_ic].statc.otp_med    =  (ic[curr_ic].stat.rx_data[0] & 0x01);
			ic[curr_ic].statc.otp_ed     =  ((ic[curr_ic].stat.rx_data[0] & 0x02) >> 1);
			ic[curr_ic].statc.a_otp_med  =  ((ic[curr_ic].stat.rx_data[0] & 0x04) >> 2);
			ic[curr_ic].statc.a_otp_ed   =  ((ic[curr_ic].stat.rx_data[0] & 0x08) >> 3);
			ic[curr_ic].statc.vd_uvlo    =  ((ic[curr_ic].stat.rx_data[0] & 0x10) >> 4);
			ic[curr_ic].statc.va_ovhi    =  ((ic[curr_ic].stat.rx_data[0] & 0x20) >> 5);
			ic[curr_ic].statc.va_uvlo    =  ((ic[curr_ic].stat.rx_data[0] & 0x40) >> 6);
			ic[curr_ic].statc.va_ovhi    =  ((ic[curr_ic].stat.rx_data[0] & 0x80) >> 7);
			ic[curr_ic].statc.oscchk     =  (ic[curr_ic].stat.rx_data[1] & 0x01);
			ic[curr_ic].statc.cpchk      =  ((ic[curr_ic].stat.rx_data[1] & 0x02) >> 1);
			ic[curr_ic].statc.thsd       =  ((ic[curr_ic].stat.rx_data[1] & 0x04) >> 2);
			ic[curr_ic].statc.muxfail    =  ((ic[curr_ic].stat.rx_data[1] & 0x08) >> 3);
			ic[curr_ic].statc.tmodechk   =  ((ic[curr_ic].stat.rx_data[1] & 0x10) >> 4);
			ic[curr_ic].statc.sleep      =  ((ic[curr_ic].stat.rx_data[1] & 0x20) >> 5);
			ic[curr_ic].statc.compchk    =  ((ic[curr_ic].stat.rx_data[1] & 0x40) >> 6);
			ic[curr_ic].statc.redfail    =  ((ic[curr_ic].stat.rx_data[1] & 0x80) >> 7);

			ic[curr_ic].statc.adol1      =  ((ic[curr_ic].stat.rx_data[2]) | (ic[curr_ic].stat.rx_data[3] << 8));
			ic[curr_ic].statc.adol2      =  ((ic[curr_ic].stat.rx_data[4]) | (ic[curr_ic].stat.rx_data[5] << 8));
		}
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815ParseStatus
	 * @brief Parse status register data
	 *
	 * @details This function Parse the recived status register data.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  grp                     Enum typ register group
	 *
	 * @param [in]  *data                   data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815ParseStatus(uint8_t tIC, cell_asic *ic, GRP grp, uint8_t *data)
	{
		switch (grp)
		{
		case A:
			adBms6815ParseStatusA(tIC, &ic[0], &data[0]);
			break;

		case B:
			adBms6815ParseStatusB(tIC, &ic[0], &data[0]);
			break;

		case C:
			adBms6815ParseStatusC(tIC, &ic[0], &data[0]);
			break;

		case ALL_GRP:
			/* Status A base address data[0] index */
			adBms6815ParseStatusA(tIC, &ic[0], &data[0]);
			/* Status B base address data[6] index */
			adBms6815ParseStatusB(tIC, &ic[0], &data[6]);
			/* Status C base address data[12] index */
			adBms6815ParseStatusC(tIC, &ic[0], &data[12]);
			break;

		default:
			break;
		}
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815ParseComm
	 * @brief Parse comm register
	 *
	 * @details This function Parse the recived comm register data.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  *data                   data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815ParseComm(uint8_t tIC, cell_asic *ic, uint8_t *data)
	{
		uint8_t address = 0;
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			memcpy(&ic[curr_ic].com.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
			address = ((curr_ic + 1) * (RX_DATA));
			ic[curr_ic].comm.icomm[0] = ((ic[curr_ic].com.rx_data[0] & 0xF0) >> 4);
			ic[curr_ic].comm.fcomm[0] = (ic[curr_ic].com.rx_data[0] & 0x0F);
			ic[curr_ic].comm.data[0] = (ic[curr_ic].com.rx_data[1]);
			ic[curr_ic].comm.icomm[1] = ((ic[curr_ic].com.rx_data[2] & 0xF0) >> 4);
			ic[curr_ic].comm.data[1] = (ic[curr_ic].com.rx_data[3]);
			ic[curr_ic].comm.fcomm[1] = (ic[curr_ic].com.rx_data[2] & 0x0F);
			ic[curr_ic].comm.icomm[2] = ((ic[curr_ic].com.rx_data[4] & 0xF0) >> 4);
			ic[curr_ic].comm.data[2] = (ic[curr_ic].com.rx_data[5]);
			ic[curr_ic].comm.fcomm[2] = (ic[curr_ic].com.rx_data[4] & 0x0F);
		}
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815ParseSID
	 * @brief Parse SID register
	 *
	 * @details This function Parse the recived sid register data.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  *data                   data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815ParseSID(uint8_t tIC, cell_asic *ic, uint8_t *data)
	{
		uint8_t address = 0;
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			memcpy(&ic[curr_ic].rsid.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
			address = ((curr_ic + 1) * (RX_DATA));
			ic[curr_ic].sid.sid[0] = ic[curr_ic].rsid.rx_data[0];
			ic[curr_ic].sid.sid[1] = ic[curr_ic].rsid.rx_data[1];
			ic[curr_ic].sid.sid[2] = ic[curr_ic].rsid.rx_data[2];
			ic[curr_ic].sid.sid[3] = ic[curr_ic].rsid.rx_data[3];
			ic[curr_ic].sid.sid[4] = ic[curr_ic].rsid.rx_data[4];
			ic[curr_ic].sid.sid[5] = ic[curr_ic].rsid.rx_data[5];
		}
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815ParsePwm
	 * @brief Parse PWM register
	 *
	 * @details This function Parse the recived pwm register data.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  *data                   data pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815ParsePwm(uint8_t tIC, cell_asic *ic, uint8_t *data)
	{
		uint8_t address = 0;
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			memcpy(&ic[curr_ic].pwm.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
			address = ((curr_ic + 1) * (RX_DATA));
			ic[curr_ic].Pwm.pwm[0] = (ic[curr_ic].pwm.rx_data[0] & 0x0F);
			ic[curr_ic].Pwm.pwm[1] = ((ic[curr_ic].pwm.rx_data[0] & 0xF0) >> 4);
			ic[curr_ic].Pwm.pwm[2] = (ic[curr_ic].pwm.rx_data[1] & 0x0F);
			ic[curr_ic].Pwm.pwm[3] = ((ic[curr_ic].pwm.rx_data[1] & 0xF0) >> 4);
			ic[curr_ic].Pwm.pwm[4] = (ic[curr_ic].pwm.rx_data[2] & 0x0F);
			ic[curr_ic].Pwm.pwm[5] = ((ic[curr_ic].pwm.rx_data[2] & 0xF0) >> 4);
			ic[curr_ic].Pwm.pwm[6] = (ic[curr_ic].pwm.rx_data[3] & 0x0F);
			ic[curr_ic].Pwm.pwm[7] = ((ic[curr_ic].pwm.rx_data[3] & 0xF0) >> 4);
			ic[curr_ic].Pwm.pwm[8] = (ic[curr_ic].pwm.rx_data[4] & 0x0F);
			ic[curr_ic].Pwm.pwm[9] = ((ic[curr_ic].pwm.rx_data[4] & 0xF0) >> 4);
			ic[curr_ic].Pwm.pwm[10] = (ic[curr_ic].pwm.rx_data[5] & 0x0F);
			ic[curr_ic].Pwm.pwm[11] = ((ic[curr_ic].pwm.rx_data[5] & 0xF0) >> 4);
		}
	}
	
	/**
	*******************************************************************************
	* Function: adBms6815_Adax
	* @brief ADAX Start a GPIO and Vref2 Conversion.
	*
	* @details Send Aux command to starts auxiliary conversion.
	*
	* Parameters:
	*
	* @param [in]  MD    Enum type adc mode
	*
	* @param [in]  CHGPIO    Enum type gpio channel to be measure
	*
	* @return None
	*
	*******************************************************************************
	*/		
	void ADBMS6815::adBmss6815_Adax(
                    ADC_MODE MD, //ADC Mode
					CHG CHGPIO //GPIO Channels to be measure
					)
	{
		uint8_t cmd[2];
		uint8_t md_bits;
		md_bits = (MD & 0x02) >> 1;
		cmd[0] = md_bits + 0x04;
		md_bits = (MD & 0x01) << 7;
		cmd[1] = md_bits + CHGPIO;
		spiSendCmd(cmd);
	}
	
	/**
	*******************************************************************************
	* Function: adBms6815_Axow
	* @brief AXOW GPIO open wire aux volatges measure.
	*
	* @details Send command to starts open wire auxiliary conversion.
	*
	* Parameters:
	*
	* @param [in]  MD      Enum type adc mode
	*
	*@param [in]  PUP      Enum type pull Down/Up current during aux conversion
	*
	* @param [in] CHGPIO   Enum type gpio channel to be measure
	*
	* @return None
	*
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_Axow(
	                    ADC_MODE MD,          //ADC Mode
		PUP_BIT PUP,          // Pull Down/Up current during aux conversion
		CHG CHGPIO            //GPIO Channels to be measure
		  )
	{
		uint8_t cmd[2];
		uint8_t md_bits;
		md_bits = (MD & 0x02) >> 1;
		cmd[0] =  (md_bits + 0x04);
		md_bits = (MD & 0x01) << 7;
		cmd[1] =  md_bits + 0x10 + (PUP << 6) + CHGPIO;
		spiSendCmd(cmd);
	}
	
	
	/* Send poll adc command and retun adc conversion count */
	uint32_t ADBMS6815::adBmsPollAdc(uint8_t tx_cmd[2])
	{
		uint32_t pladc_count = 0;
		uint8_t cmd[4];
		uint16_t cmd_pec;
		uint8_t read_data = 0x00;
		uint8_t SDO_Line = 0xFF;
		cmd[0] = tx_cmd[0];
		cmd[1] = tx_cmd[1];
		cmd_pec = Pec15_Calc(2, cmd);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);
//		startTimer();
		spi4.lock();
		CS = 0;		
		spiWriteBytes(4, &cmd[0]);
		do {
			spiReadBytes(1, &read_data);
		} while (!(read_data == SDO_Line));
		CS = 1;
		spi4.unlock();
//		pladc_count = getTimCount();
//		stopTimer();
		return (pladc_count);
	}
	
	/**
*******************************************************************************
* @brief Set configuration register A. Refer to the data sheet
*        Set configuration register B. Refer to the data sheet
*******************************************************************************
*/
	void ADBMS6815::adBms6815_init_configA_B(uint8_t tIC, cell_asic *ic)
	{
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			/* Init config A */
			ic[cic].tx_cfga.refon = PWR_UP;
			//References Powered Up.
			//1:References remain powered up until watchdog timeout.
			//0:References shut down after conversions
			ic[cic].tx_cfga.adcopt = ADCOPT_ON;//ADC_OPT; //2K Hz
			//ADC Mode Option Bit.
			//0:Selects 27 kHz, 7 kHz, 422 Hz,or 26 Hz mode with the MD[1 : 0] bits in the ADC conversion commands(default).
			//1:Selects 14 kHz, 3 kHz, 1 kHz, or 2 kHz mode with the MD[1 : 0] bits in the ADC conversion commands.
			ic[cic].tx_cfga.mcal = MCAL_OFF; //MCAL;
			ic[cic].tx_cfga.gpo = 0x7F; /* All gpios pull down disable      */
			ic[cic].tx_cfga.soakon = 0; /* Set default value = 0            */
			ic[cic].tx_cfga.owrng = 0; /* Set default value = 0            */
			ic[cic].tx_cfga.owa = 0; /* Set default value = 0            */

			/* Init config B */
			ic[cic].tx_cfgb.vov = SetOverVoltageThreshold(OV_THRESHOLD);
			ic[cic].tx_cfgb.vuv = SetUnderVoltageThreshold(UV_THRESHOLD);
		}
		//adBmsWakeupIc(tIC);
		adBmsWriteData(tIC, &ic[0], WRCFGA, Config, A);
		adBmsWriteData(tIC, &ic[0], WRCFGB, Config, B);
	}
	
	/**
	 *******************************************************************************
	 * Function: SetOverVoltageThreshold
	 * @brief Set Over Voltage Threshold.
	 *
	 * @details This function Set Over Voltage Threshold.
	 *
	 * Parameters:
	 *
	 * @param [in]  voltage      Over voltage
	 *
	 * @return OverVoltage_value
	 *
	 *******************************************************************************
	*/
	uint16_t ADBMS6815::SetOverVoltageThreshold(float voltage)
	{
		uint16_t vov_value;
		vov_value = (uint16_t)(voltage / 0.0016); /*(Comparison Voltage = VOV • 16 • 0.0001)*/
		return vov_value;
	}
	
	/**
	 *******************************************************************************
	 * Function: SetUnderVoltageThreshold
	 * @brief Set Under Voltage Threshold.
	 *
	 * @details This function Set Under Voltage Threshold.
	 *
	 * Parameters:
	 *
	 * @param [in]  voltage      Under voltage
	 *
	 * @return UnderVoltage_value
	 *
	 *******************************************************************************
	*/
	uint16_t ADBMS6815::SetUnderVoltageThreshold(float voltage)
	{
		uint16_t vuv_value;
		vuv_value = (uint16_t)(voltage / 0.0016);
		return vuv_value;
	}
	
	/**
	*******************************************************************************
	* Function: adBmsWriteData
	* @brief Adbms Write Data into Bms ic.
	*
	* @details This function write the data into bms ic.
	*
	* Parameters:
	* @param [in]   tIC      Total IC
	*
	* @param [in]  *ic      cell_asic stucture pointer
	*
	* @param [in]  cmd_arg   command bytes
	*
	* @param [in]  TYPE   Enum type of resistor
	*
	* @param [in]  GRP   Enum type of resistor group
	*
	* @return None
	*
	*******************************************************************************
	*/
	void ADBMS6815::adBmsWriteData(uint8_t tIC, cell_asic *ic, uint8_t cmd_arg[2], TYPE type, GRP group)
	{
		uint8_t data_len, write_size;
		if (type == Clrflag)
		{
			data_len = CLR_FLAG, write_size = (CLR_FLAG * tIC);
		}
		else
		{
			data_len = TX_DATA, write_size = (TX_DATA * tIC);
		}
		uint8_t *write_buffer = (uint8_t *)calloc(write_size, sizeof(uint8_t));
		if (write_buffer == NULL)
		{
#ifdef MBED
			pc.printf(" Failed to allocate write_buffer array memory \n");
#else
			printf(" Failed to allocate write_buffer array memory \n");
#endif
			exit(0);
		}
		else
		{
			switch (type)
			{
			case Config:
				switch (group)
				{
				case A:
					adBms6815CreateConfiga(tIC, &ic[0]);
					for (uint8_t cic = 0; cic < tIC; cic++)
					{
						for (uint8_t data = 0; data < data_len; data++)
						{
							write_buffer[(cic * data_len) + data] = ic[cic].configa.tx_data[data];
						}
					}
					break;
				case B:
					adBms6815CreateConfigb(tIC, &ic[0]);
					for (uint8_t cic = 0; cic < tIC; cic++)
					{
						for (uint8_t data = 0; data < data_len; data++)
						{
							write_buffer[(cic * data_len) + data] = ic[cic].configb.tx_data[data];
						}
					}
					break;
				}
				break;

			case Comm:
				adBms6815CreateComm(tIC, &ic[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					for (uint8_t data = 0; data < data_len; data++)
					{
						write_buffer[(cic * data_len) + data] = ic[cic].com.tx_data[data];
					}
				}
				break;

			case Pwm:
				adBms6815CreatePwm(tIC, &ic[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					for (uint8_t data = 0; data < data_len; data++)
					{
						write_buffer[(cic * data_len) + data] = ic[cic].pwm.tx_data[data];
					}
				}
				break;

			case Clrflag:
				adBms6815CreateClrFlagData(tIC, &ic[0]);
				for (uint8_t cic = 0; cic < tIC; cic++)
				{
					for (uint8_t data = 0; data < data_len; data++)
					{
						write_buffer[(cic * data_len) + data] = ic[cic].flag_tx.data[data];
					}
				}
				break;

			default:
				break;
			}
		}
		spiWriteData(tIC, cmd_arg, &write_buffer[0], data_len);
		free(write_buffer);
	}
	
	/**
	 *******************************************************************************
	 * Function: adBmsWakeupIc
	 * @brief Wakeup bms ic using chip select
	 *
	 * @details This function wakeup thr bms ic using chip select.
	 *
	 * @param [in]  total_ic    Total_ic
	 *
	 * @return None
	 *
	 *******************************************************************************
	*/
	void ADBMS6815::adBmsWakeupIc(uint8_t total_ic)
	{
		for (uint8_t ic = 0; ic < total_ic; ic++)
		{
			spi4.lock();
			CS = 0;		
//			wait_us(100);
			wait_ms(4);
			CS = 1;
			spi4.unlock();
//			wait_us(1);
			wait_ms(4);
		}
	}
	
	
	void ADBMS6815::adBmsWakeupIdle(uint8_t total_ic)
	{
		uint8_t read_data = 0x00;
		//MAX_PACK_NUM
		for (uint16_t i = 0; i < total_ic; i++)
		{
			spi4.lock();
			CS = 0;
			spiReadBytes(1, &read_data);
			CS = 1;
			spi4.unlock();
			wait_us(600);
		}
	}
	
	/**
	 *******************************************************************************
	 * Function: adBms6815CreateConfiga
	 * @brief Create the configation A write buffer
	 *
	 * @details This function create the configation A write buffer.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	*/
	void ADBMS6815::adBms6815CreateConfiga(uint8_t tIC, cell_asic *ic)
	{
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			ic[curr_ic].configa.tx_data[0] = (((ic[curr_ic].tx_cfga.refon & 0x01) << 7) | (ic[curr_ic].tx_cfga.adcopt << 6) | (ic[curr_ic].tx_cfga.ps << 4) | (ic[curr_ic].tx_cfga.cvmin << 2) | (ic[curr_ic].tx_cfga.mcal << 1) | ic[curr_ic].tx_cfga.comm_bk);
			ic[curr_ic].configa.tx_data[1] = (ic[curr_ic].tx_cfga.flag_d & 0xFF);
			ic[curr_ic].configa.tx_data[2] = (((ic[curr_ic].tx_cfga.soakon & 0x01) << 7) | ((ic[curr_ic].tx_cfga.owrng & 0x1) << 6) | ((ic[curr_ic].tx_cfga.owa & 0x07) << 3) | (ic[curr_ic].tx_cfga.owc & 0x07));
			ic[curr_ic].configa.tx_data[3] = ((ic[curr_ic].tx_cfga.gpo & 0xFF));
			ic[curr_ic].configa.tx_data[4] = ((ic[curr_ic].tx_cfga.gpi & 0xFF));
			ic[curr_ic].configa.tx_data[5] = ((ic[curr_ic].tx_cfga.rev & 0x0F) << 4);
		}
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815CreateConfigb
	 * @brief Create the configation B write buffer
	 *
	 * @details This function create the configation B write buffer.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815CreateConfigb(uint8_t tIC, cell_asic *ic)
	{
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			ic[curr_ic].configb.tx_data[0] = ((ic[curr_ic].tx_cfgb.vuv));
			ic[curr_ic].configb.tx_data[1] = (((ic[curr_ic].tx_cfgb.vov & 0x00F) << 4) | ((ic[curr_ic].tx_cfgb.vuv) >> 8));
			ic[curr_ic].configb.tx_data[2] = ((ic[curr_ic].tx_cfgb.vov >> 4) & 0x0FF);
			ic[curr_ic].configb.tx_data[3] = (((ic[curr_ic].tx_cfgb.dtmen & 0x01) << 7) | ((ic[curr_ic].tx_cfgb.dtrng & 0x01) << 6) | (ic[curr_ic].tx_cfgb.dcto & 0x3F));
			ic[curr_ic].configb.tx_data[4] = ((ic[curr_ic].tx_cfgb.dcc & 0xFF));
			ic[curr_ic].configb.tx_data[5] = (((ic[curr_ic].tx_cfgb.mute_st & 0x01) << 7) | (ic[curr_ic].tx_cfgb.dcc >> 8));
		}
	}
	
	/**
	 *******************************************************************************
	 * Function: adBms6815CreateClrFlagData
	 * @brief Create the clear flag write buffer.
	 *
	 * @details This function create the clear flag write buffer.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	*/
	void ADBMS6815::adBms6815CreateClrFlagData(uint8_t tIC, cell_asic *ic)
	{
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			ic[curr_ic].flag_tx.data[0] = (((ic[curr_ic].clrflg_data.cl_vaov << 7) | (ic[curr_ic].clrflg_data.cl_vauv << 6) | (ic[curr_ic].clrflg_data.cl_vdov << 5) | (ic[curr_ic].clrflg_data.cl_vduv << 4)
			                                | (ic[curr_ic].clrflg_data.cl_aed << 3) | (ic[curr_ic].clrflg_data.cl_amed << 2) | (ic[curr_ic].clrflg_data.cl_ed << 1) | ic[curr_ic].clrflg_data.cl_med));
			ic[curr_ic].flag_tx.data[1] = (((ic[curr_ic].clrflg_data.cl_redf << 7) | (ic[curr_ic].clrflg_data.cl_comp << 6) | (ic[curr_ic].clrflg_data.cl_sleep << 5) | (ic[curr_ic].clrflg_data.cl_tmode << 4)
			                                | (ic[curr_ic].clrflg_data.cl_thsd << 2) | (ic[curr_ic].clrflg_data.cl_cpchk << 1) | ic[curr_ic].clrflg_data.cl_oscchk));
		}
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815CreateComm
	 * @brief Create the configation comm write buffer
	 *
	 * @details This function create the configation comm write buffer.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815CreateComm(uint8_t tIC, cell_asic *ic)
	{
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			ic[curr_ic].com.tx_data[0] = ((ic[curr_ic].comm.icomm[0] & 0x0F)  << 4  | (ic[curr_ic].comm.fcomm[0]   & 0x0F));
			ic[curr_ic].com.tx_data[1] = ((ic[curr_ic].comm.data[0]));
			ic[curr_ic].com.tx_data[2] = ((ic[curr_ic].comm.icomm[1] & 0x0F)  << 4) | (ic[curr_ic].comm.fcomm[1]   & 0x0F);
			ic[curr_ic].com.tx_data[3] = ((ic[curr_ic].comm.data[1]));
			ic[curr_ic].com.tx_data[4] = ((ic[curr_ic].comm.icomm[2] & 0x0F)  << 4  | (ic[curr_ic].comm.fcomm[2]   & 0x0F));
			ic[curr_ic].com.tx_data[5] = ((ic[curr_ic].comm.data[2]));
		}
	}

	/**
	 *******************************************************************************
	 * Function: adBms6815CreatePwm
	 * @brief Create the configation pwm write buffer
	 *
	 * @details This function create the configation pwm write buffer.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::adBms6815CreatePwm(uint8_t tIC, cell_asic *ic)
	{
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			ic[curr_ic].pwm.tx_data[0] = ((ic[curr_ic].Pwm.pwm[1] & 0x0F) << 4 | (ic[curr_ic].Pwm.pwm[0] & 0x0F));
			ic[curr_ic].pwm.tx_data[1] = ((ic[curr_ic].Pwm.pwm[3] & 0x0F) << 4 | (ic[curr_ic].Pwm.pwm[2] & 0x0F));
			ic[curr_ic].pwm.tx_data[2] = ((ic[curr_ic].Pwm.pwm[5] & 0x0F) << 4 | (ic[curr_ic].Pwm.pwm[4] & 0x0F));
			ic[curr_ic].pwm.tx_data[3] = ((ic[curr_ic].Pwm.pwm[7] & 0x0F) << 4 | (ic[curr_ic].Pwm.pwm[6] & 0x0F));
			ic[curr_ic].pwm.tx_data[4] = ((ic[curr_ic].Pwm.pwm[9] & 0x0F) << 4 | (ic[curr_ic].Pwm.pwm[8] & 0x0F));
			ic[curr_ic].pwm.tx_data[5] = ((ic[curr_ic].Pwm.pwm[11] & 0x0F) << 4 | (ic[curr_ic].Pwm.pwm[10] & 0x0F));
		}
	}

	/**
	 *******************************************************************************
	 * Function: ConfigA_Flag
	 * @brief Config A Flag bit.
	 *
	 * @details This function Set configuration A flag bit.
	 *
	 * Parameters:
	 *
	 * @param [in]  flag_d      Enum type flag bit
	 *
	 * @param [in]  flag       Enum type set or clr flag
	 *
	 * @return Flagbit_Value
	 *
	 *******************************************************************************
	*/
	uint8_t ADBMS6815::ConfigA_Flag(FLAG_D flag_d, CFGA_FLAG flag)
	{
		uint8_t flag_value;
		if (flag == FLAG_SET)
		{
			flag_value = (1 << flag_d);
		}
		else
		{
			flag_value = (0 << flag_d);
		}
		return (flag_value);
	}

	/**
	 *******************************************************************************
	 * Function: ConfigA_Gpo
	 * @brief Config A Gpio Pull High/Low.
	 *
	 * @details This function Set configuration A gpio as pull high/Low.
	 *
	 * Parameters:
	 *
	 * @param [in]  gpo      Enum type GPO Pin
	 *
	 * @param [in]  stat     Enum type CFGAGPO set Low or High
	 *
	 * @return Gpio_value
	 *
	 *******************************************************************************
	 */
	uint16_t ADBMS6815::ConfigA_Gpo(GPO gpo, CFGA_GPO stat)
	{
		uint16_t gpovalue;
		if (stat == GPO_SET)
		{
			gpovalue = (1 << gpo);
		}
		else
		{
			gpovalue = (0 << gpo);
		}
		return (gpovalue);
	}

	/**
	 *******************************************************************************
	 * Function: ConfigB_DccBit
	 * @brief Config B DCC bit.
	 *
	 * @details This function configure config B DCC bit.
	 *
	 * Parameters:
	 *
	 * @param [in]  dcc      Enum type DCC bit
	 *
	 * @param [in]  dccbit   Enum type DCC bit set or clr.
	 *
	 * @return DccBit_value
	 *
	 *******************************************************************************
	 */
	uint16_t ADBMS6815::ConfigB_DccBit(DCC dcc, DCC_BIT dccbit)
	{
		uint16_t dccvalue;
		if (dccbit == DCC_BIT_SET)
		{
			dccvalue = (1 << dcc);
		}
		else
		{
			dccvalue = (0 << dcc);
		}
		return (dccvalue);
	}

	/**
	 *******************************************************************************
	 * Function: SetPwmDutyCycle
	 * @brief Set PWM discharges duty cycle.
	 *
	 * @details This function Set PWM discharges duty cycle.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  duty_cycle              Enum type PWM duty cycle value
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::SetPwmDutyCycle(uint8_t tIC, cell_asic *ic, PWM_DUTY duty_cycle)
	{
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			for (uint8_t pwmc = 0; pwmc < PWM; pwmc++)
			{
				ic[curr_ic].Pwm.pwm[pwmc] = duty_cycle;
			}
		}
	}

	/**
	 *******************************************************************************
	 * Function: SetConfigB_DischargeTimeOutValue
	 * @brief Set Config B Discharge Time Out Value.
	 *
	 * @details This function Set configuration B Discharge Time Out Value with short or long range.
	 *
	 * Parameters:
	 *
	 * @param [in]  tIC                     Total IC
	 *
	 * @param [in]  *ic                     cell_asic ic structure pointer
	 *
	 * @param [in]  timer_rang              Enum type time range
	 *
	 * @param [in]  timeout_value           Enum type time out value
	 *
	 * @return None
	 *
	 *******************************************************************************
	 */
	void ADBMS6815::SetConfigB_DischargeTimeOutValue(uint8_t tIC, cell_asic *ic, DTRNG timer_rang, DCTO timeout_value)
	{
		for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
		{
			ic[curr_ic].tx_cfgb.dtrng = timer_rang;
			if (timer_rang == RANG_0_TO_63_MIN)
			{
				ic[curr_ic].tx_cfgb.dcto = timeout_value;
			}
			else if (timer_rang == RANG_0_TO_16_8_HR)
			{
				ic[curr_ic].tx_cfgb.dcto = timeout_value;
			}
		}
	}
	
	

} //namespace bmstech
