#include <mbed.h>
#include "LTC6811_daisy.h"
#include "PackConst.h"
//#include "ADBMS1818_daisy.h"

namespace bmstech
{
	
	/* Helper function to initialize register limits. */
	void LTC6811SPI::ADBMS1818_init_reg_limits(uint8_t total_ic, //Number of ICs in the system
		Cell_asic *ic // A two dimensional array that will store the data
		)
	{

		
		for (uint8_t cic = 0; cic < total_ic; cic++)
		{
			ic[cic].ic_reg.cell_channels = PackConst::MAX_CELL_NUM; 
			ic[cic].ic_reg.stat_channels = 4;
			ic[cic].ic_reg.aux_channels = 12;	//ic[cic].ic_reg.aux_channels = 9;
			ic[cic].ic_reg.num_cv_reg = 6; 
			ic[cic].ic_reg.num_gpio_reg = 4;    //ic[cic].ic_reg.num_gpio_reg = 4;    
			ic[cic].ic_reg.num_stat_reg = 2;     
		} 
	}
	
	/* Helper Function to reset PEC counters */
	void LTC6811SPI::ADBMS1818_reset_crc_count(uint8_t total_ic, //Number of ICs in the system
		Cell_asic *ic //A two dimensional array that stores the data
		)
	{
		for (int current_ic = 0; current_ic < total_ic; current_ic++)
		{
			ic[current_ic].crc_count.pec_count = 0;
			ic[current_ic].crc_count.cfgr_pec = 0;
			for (int i = 0; i < 6; i++)
			{
				ic[current_ic].crc_count.cell_pec[i] = 0;
		
			}
			for (int i = 0; i < 4; i++)
			{
				ic[current_ic].crc_count.aux_pec[i] = 0;
			}
			for (int i = 0; i < 2; i++)
			{
				ic[current_ic].crc_count.stat_pec[i] = 0;
			}
		}
	}
	
	/* Helper function to initialize CFG variables */
	void LTC6811SPI::ADBMS1818_init_cfg(uint8_t total_ic, //Number of ICs in the system
		Cell_asic *ic //A two dimensional array that stores the data
		)
	{
		
		for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++)  
		{
			for (int j = 0; j < 6; j++)
			{
				ic[current_ic].config.tx_data[j] = 0;
			}
		}
	}
	
	

	/* Helper Function to initialize the CFGRB data structures */
	void LTC6811SPI::ADBMS1818_init_cfgb(uint8_t total_ic, Cell_asic *ic)
	{		
		//	/*************************************************************************
		//	Set configuration register. Refer to the data sheet
		//	**************************************************************************/
		static bool REFON = true; //!< Reference Powered Up Bit
		bool ADCOPT = false; //!< ADC Mode option bit
		bool GPIOBITS_A[5] = { true, true, true, true, true }; //!< GPIO Pin Control // Gpio 1,2,3,4,5
		bool GPIOBITS_B[4] = { true, true, true, true }; //!< GPIO Pin Control // Gpio 6,7,8,9
		uint16_t UV = UV_THRESHOLD; //!< Under voltage Comparison Voltage
		uint16_t OV = OV_THRESHOLD; //!< Over voltage Comparison Voltage
		bool DCCBITS_A[12] = { false, false, false, false, false, false, false, false, false, false, false, false }; //!< Discharge cell switch //Dcc 1,2,3,4,5,6,7,8,9,10,11,12
		bool DCCBITS_B[7] = { false, false, false, false, false, false, false }; //!< Discharge cell switch //Dcc 0,13,14,15
		bool DCTOBITS[4] = { true, false, true, false }; //!< Discharge time value //Dcto 0,1,2,3  // Programed for 4 min
		/*Ensure that Dcto bits are set according to the required discharge time. Refer to the data sheet */
		bool FDRF = false; //!< Force Digital Redundancy Failure Bit
		bool DTMEN = true; //!< Enable Discharge Timer Monitor
		bool PSBITS[2] = { false, false }; //!< Digital Redundancy Path Selection//ps-0,1
		for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++)
		{
			for (int j = 0; j < 6; j++)
			{
				ic[current_ic].configb.tx_data[j] = 0;
			}  
		}
		
		for (uint8_t current_ic = 0; current_ic < LTC6811SPI::TOTAL_IC; current_ic++)
		{
			ADBMS1818_set_cfgr(current_ic, ic, REFON, ADCOPT, GPIOBITS_A, DCCBITS_A, DCTOBITS, UV, OV);
			ADBMS1818_set_cfgrb(current_ic, ic, FDRF, DTMEN, PSBITS, GPIOBITS_B, DCCBITS_B);
		}
		
	}
	
	/* Helper function to set CFGR variable */
	void LTC6811SPI::ADBMS1818_set_cfgr(uint8_t nIC, // Current IC
		Cell_asic *ic, // A two dimensional array that stores the data
		bool refon, // The REFON bit
		bool adcopt, // The ADCOPT bit
		bool gpio[5], // The GPIO bits
		bool dcc[12], // The DCC bits
		bool dcto[4], // The Dcto bits
		uint16_t uv, // The UV value
		uint16_t  ov // The OV value
		)
	{
		ADBMS181x_set_cfgr_refon(nIC, ic, refon);
		ADBMS181x_set_cfgr_adcopt(nIC, ic, adcopt);
		ADBMS181x_set_cfgr_gpio(nIC, ic, gpio);
		ADBMS181x_set_cfgr_dis(nIC, ic, dcc);
		ADBMS181x_set_cfgr_dcto(nIC, ic, dcto);
		ADBMS181x_set_cfgr_uv(nIC, ic, uv);
		ADBMS181x_set_cfgr_ov(nIC, ic, ov);
	}
	
	/* Helper Function to set the configuration register B */ 
	void LTC6811SPI::ADBMS1818_set_cfgrb(uint8_t nIC, Cell_asic *ic, bool fdrf, bool dtmen, bool ps[2], bool gpiobits[4], bool dccbits[7])
	{
		ADBMS1818_set_cfgrb_fdrf(nIC, ic, fdrf);
		ADBMS1818_set_cfgrb_dtmen(nIC, ic, dtmen);
		ADBMS1818_set_cfgrb_ps(nIC, ic, ps);
		ADBMS1818_set_cfgrb_gpio_b(nIC, ic, gpiobits);
		ADBMS1818_set_cfgrb_dcc_b(nIC, ic, dccbits);
	}

	/* Helper function to set the REFON bit */
	void LTC6811SPI::ADBMS181x_set_cfgr_refon(uint8_t nIC, Cell_asic *ic, bool refon)
	{
		if (refon) ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0] | 0x04;
		else ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0] & 0xFB;
	}

	/* Helper function to set the ADCOPT bit */
	void LTC6811SPI::ADBMS181x_set_cfgr_adcopt(uint8_t nIC, Cell_asic *ic, bool adcopt)
	{
		if (adcopt) ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0] | 0x01;
		else ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0] & 0xFE;
	}

	/* Helper function to set GPIO bits */
	void LTC6811SPI::ADBMS181x_set_cfgr_gpio(uint8_t nIC, Cell_asic *ic, bool gpio[5])
	{
		for (int i = 0; i < 5; i++)
		{
			if (gpio[i])ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0] | (0x01 << (i + 3));
			else ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]&(~(0x01 << (i + 3)));
		}
	}

	/* Helper function to control discharge */
	void LTC6811SPI::ADBMS181x_set_cfgr_dis(uint8_t nIC, Cell_asic *ic, bool dcc[12])
	{
		for (int i = 0; i < 8; i++)
		{
			if (dcc[i])ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4] | (0x01 << i);
			else ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4]& (~(0x01 << i));
		}
		for (int i = 0; i < 4; i++)
		{
			if (dcc[i + 8])ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5] | (0x01 << i);
			else ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]&(~(0x01 << i));
		}
	}

	/* Helper function to control discharge time value */
	void LTC6811SPI::ADBMS181x_set_cfgr_dcto(uint8_t nIC, Cell_asic *ic, bool dcto[4])
	{  
		for (int i = 0; i < 4; i++)
		{
			if (dcto[i])ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5] | (0x01 << (i + 4));
			else ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]&(~(0x01 << (i + 4)));
		} 
	}
	
	/* Helper Function to set UV value in CFG register */
	void LTC6811SPI::ADBMS181x_set_cfgr_uv(uint8_t nIC, Cell_asic *ic, uint16_t uv)
	{
		uint16_t tmp = (uv / 16) - 1;
		ic[nIC].config.tx_data[1] = 0x00FF & tmp;
		ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2] & 0xF0;
		ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2] | ((0x0F00 & tmp) >> 8);
	}

	/* Helper function to set OV value in CFG register */
	void LTC6811SPI::ADBMS181x_set_cfgr_ov(uint8_t nIC, Cell_asic *ic, uint16_t ov)
	{
		uint16_t tmp = (ov / 16);
		ic[nIC].config.tx_data[3] = 0x00FF & (tmp >> 4);
		ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2] & 0x0F;
		ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2] | ((0x000F & tmp) << 4);
	}

	/* Helper function to set the FDRF bit */
	void LTC6811SPI::ADBMS1818_set_cfgrb_fdrf(uint8_t nIC, Cell_asic *ic, bool fdrf) 
	{
		if (fdrf) ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] | 0x40;
		else ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] & 0xBF;
	}

	/* Helper function to set the DTMEN bit */
	void LTC6811SPI::ADBMS1818_set_cfgrb_dtmen(uint8_t nIC, Cell_asic *ic, bool dtmen) 
	{
		if (dtmen) ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] | 0x08;
		else ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] & 0xF7;
	}
	
	/* Helper function to set the PATH SELECT bit */
	void LTC6811SPI::ADBMS1818_set_cfgrb_ps(uint8_t nIC, Cell_asic *ic, bool ps[]) 
	{
		for (int i = 0; i < 2; i++)
		{
			if (ps[i])ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] | (0x01 << (i + 4));
			else ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]&(~(0x01 << (i + 4)));
		}
	}

	/*  Helper function to set the gpio bits in configb b register  */
	void LTC6811SPI::ADBMS1818_set_cfgrb_gpio_b(uint8_t nIC, Cell_asic *ic, bool gpiobits[]) 
	{
		for (int i = 0; i < 4; i++)
		{
			if (gpiobits[i])ic[nIC].configb.tx_data[0] = ic[nIC].configb.tx_data[0] | (0x01 << i);
			else ic[nIC].configb.tx_data[0] = ic[nIC].configb.tx_data[0]&(~(0x01 << i));
		}
	}

	/*  Helper function to set the dcc bits in configb b register */ 
	void LTC6811SPI::ADBMS1818_set_cfgrb_dcc_b(uint8_t nIC, Cell_asic *ic, bool dccbits[]) 
	{
		for (int i = 0; i < 7; i++)
		{ 
			if (i == 0)
			{
				if (dccbits[i])ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] | 0x04;
				else ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] & 0xFB;
			}
			if (i > 0 and i < 5)
			{	
				if (dccbits[i])ic[nIC].configb.tx_data[0] = ic[nIC].configb.tx_data[0] | (0x01 << (i + 3));
				else ic[nIC].configb.tx_data[0] = ic[nIC].configb.tx_data[0]&(~(0x01 << (i + 3)));
			}
			if (i > 4 and i < 7)
			{
				if (dccbits[i])ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] | (0x01 << (i - 5));
				else ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]&(~(0x01 << (i - 5)));	
			}
		}
	}
	
//	/* Helper function that increments PEC counters */
//	void LTC6811SPI::ADBMS181x_check_pec(uint8_t total_ic, //Number of ICs in the system
//		uint8_t reg, //Type of Register
//		Cell_asic *ic //A two dimensional array that stores the data
//		)
//	{
//		switch (reg)
//		{
//		case CFGR:
//			for (int current_ic = 0; current_ic < total_ic; current_ic++)
//			{
//				ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].config.rx_pec_match;
//				ic[current_ic].crc_count.cfgr_pec = ic[current_ic].crc_count.cfgr_pec + ic[current_ic].config.rx_pec_match;
//			}
//			break;
//
//		case CFGRB:
//			for (int current_ic = 0; current_ic < total_ic; current_ic++)
//			{
//				ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].configb.rx_pec_match;
//				ic[current_ic].crc_count.cfgr_pec = ic[current_ic].crc_count.cfgr_pec + ic[current_ic].configb.rx_pec_match;
//			}
//			break;
//		case CELL:
//			for (int current_ic = 0; current_ic < total_ic; current_ic++)
//			{
//				for (int i = 0; i < ic[0].ic_reg.num_cv_reg; i++)
//				{
//					ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].cells.pec_match[i];
//					ic[current_ic].crc_count.cell_pec[i] = ic[current_ic].crc_count.cell_pec[i] + ic[current_ic].cells.pec_match[i];
//				}
//			}
//			break;
//		case AUX:
//			for (int current_ic = 0; current_ic < total_ic; current_ic++)
//			{
//				for (int i = 0; i < ic[0].ic_reg.num_gpio_reg; i++)
//				{
//					ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + (ic[current_ic].aux.pec_match[i]);
//					ic[current_ic].crc_count.aux_pec[i] = ic[current_ic].crc_count.aux_pec[i] + (ic[current_ic].aux.pec_match[i]);
//				}
//			}
//
//			break;
//		case STAT:
//			for (int current_ic = 0; current_ic < total_ic; current_ic++)
//			{
//
//				for (int i = 0; i < ic[0].ic_reg.num_stat_reg - 1; i++)
//				{
//					ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].stat.pec_match[i];
//					ic[current_ic].crc_count.stat_pec[i] = ic[current_ic].crc_count.stat_pec[i] + ic[current_ic].stat.pec_match[i];
//				}
//			}
//			break;
//		default:
//			break;
//		}
//	}

	/* Write the ADBMS181x CFGRA */
	void LTC6811SPI::ADBMS181x_wrcfg(uint8_t total_ic, //The number of ICs being written to
		Cell_asic ic[]  // A two dimensional array of the configuration data that will be written
	   )
	{
		uint8_t cmd[2] = { 0x00, 0x01 };
		uint8_t write_buffer[384]; //uint8_t write_buffer[256];
		uint8_t write_count = 0;
		uint8_t c_ic = 0;
	
		for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++)
		{
			if (ic->isospi_reverse == false)
			{
				c_ic = current_ic;
			}
			else
			{
				c_ic = total_ic - current_ic - 1;
			}
		
			for (uint8_t data = 0; data < 6; data++)
			{
				write_buffer[write_count] = ic[c_ic].config.tx_data[data];
				write_count++;
			}
		}
		WRITE_6811(total_ic, cmd, write_buffer);
	}

	/* Write the ADBMS181x CFGRB */
	void LTC6811SPI::ADBMS181x_wrcfgb(uint8_t total_ic, //The number of ICs being written to
		Cell_asic ic[] // A two dimensional array of the configuration data that will be written
	   )
	{
		uint8_t cmd[2] = { 0x00, 0x24 };
		uint8_t write_buffer[384];	//uint8_t write_buffer[256];
		uint8_t write_count = 0;
		uint8_t c_ic = 0;
	
		for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++)
		{
			if (ic->isospi_reverse == false)
			{
				c_ic = current_ic;
			}
			else
			{
				c_ic = total_ic - current_ic - 1;
			}
		
			for (uint8_t data = 0; data < 6; data++)
			{
				write_buffer[write_count] = ic[c_ic].configb.tx_data[data];
				write_count++;
			}
		}
		WRITE_6811(total_ic, cmd, write_buffer);
	}
	
	/* Reads configuration registers of a ADBMS1818 daisy chain */
	int8_t LTC6811SPI::ADBMS1818_rdcfg(uint8_t total_ic, //Number of ICs in the system
		Cell_asic *ic //A two dimensional array that the function stores the read configuration data.
	   )
	{
		int8_t pec_error = 0;
		pec_error = ADBMS181x_rdcfg(total_ic, ic);
		return (pec_error);
	}

	/* Reads configuration b registers of a ADBMS1818 daisy chain */
	int8_t LTC6811SPI::ADBMS1818_rdcfgb(uint8_t total_ic, //Number of ICs in the system
		Cell_asic *ic //A two dimensional array that the function stores the read configuration data.
	   )
	{
		int8_t pec_error = 0;
		pec_error = ADBMS181x_rdcfgb(total_ic, ic);
		return (pec_error);
	}
	
//	/* Read the ADBMS181x CFGA */
//	int8_t LTC6811SPI::ADBMS181x_rdcfg(uint8_t total_ic, //Number of ICs in the system
//		Cell_asic ic[] // A two dimensional array that the function stores the read configuration data.
//	   )
//	{
//		uint8_t cmd[2] = { 0x00, 0x02 };
//		uint8_t read_buffer[384];
//		int8_t pec_error = 0;
//		uint16_t data_pec;
//		uint16_t calc_pec;
//		uint8_t c_ic = 0;
//	
//		pec_error = READ_6811(total_ic, cmd, read_buffer);
//	
//		for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++)
//		{
//			if (ic->isospi_reverse == false)
//			{
//				c_ic = current_ic;
//			}
//			else
//			{
//				c_ic = total_ic - current_ic - 1;
//			}
//		
//			for (int byte = 0; byte < 8; byte++)
//			{
//				ic[c_ic].config.rx_data[byte] = read_buffer[byte + (8*current_ic)];
//			}
//		
//			calc_pec = PEC15_CALC(6, &read_buffer[8*current_ic]);
//			data_pec = read_buffer[7 + (8*current_ic)] | (read_buffer[6 + (8*current_ic)] << 8);
//			if (calc_pec != data_pec)
//			{
//				ic[c_ic].config.rx_pec_match = 1;
//			}
//			else ic[c_ic].config.rx_pec_match = 0;
//		}
//		ADBMS181x_check_pec(total_ic, CFGR, ic);
//	
//		return (pec_error);
//	}

//	/* Reads the ADBMS181x CFGB */
//	int8_t LTC6811SPI::ADBMS181x_rdcfgb(uint8_t total_ic, //Number of ICs in the system
//		Cell_asic ic[] // A two dimensional array that the function stores the read configuration data.
//	   )
//	{
//		uint8_t cmd[2] = { 0x00, 0x26 };
//		uint8_t read_buffer[384];
//		int8_t pec_error = 0;
//		uint16_t data_pec;
//		uint16_t calc_pec;
//		uint8_t c_ic = 0;
//	
//		pec_error = READ_6811(total_ic, cmd, read_buffer);
//	
//		for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++)
//		{
//			if (ic->isospi_reverse == false)
//			{
//				c_ic = current_ic;
//			}
//			else
//			{
//				c_ic = total_ic - current_ic - 1;
//			}
//		
//			for (int byte = 0; byte < 8; byte++)
//			{
//				ic[c_ic].configb.rx_data[byte] = read_buffer[byte + (8*current_ic)];
//			}
//		
//			calc_pec = PEC15_CALC(6, &read_buffer[8*current_ic]);
//			data_pec = read_buffer[7 + (8*current_ic)] | (read_buffer[6 + (8*current_ic)] << 8);
//			if (calc_pec != data_pec)
//			{
//				ic[c_ic].configb.rx_pec_match = 1;
//			}
//			else ic[c_ic].configb.rx_pec_match = 0;
//		}
//		ADBMS181x_check_pec(total_ic, CFGRB, ic);
//	
//		return (pec_error);
//	}
	
	

} //namespace bmstech
