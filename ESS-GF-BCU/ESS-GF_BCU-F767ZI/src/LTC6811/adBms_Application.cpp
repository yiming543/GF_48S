#include <mbed.h>
#include "LTC6811_daisy.h"
#include "BatterySys.h"
#include "Balance.h"
#include "ADC_Temperature.h"

#include "ADBMS6815_daisy.h"
#include "adBms6815CmdList.h"
#include "SEGGER_RTT.h"

extern DigitalOut Green_LED;	//extern
//extern DigitalOut Blue_LED;
//extern DigitalOut Red_LED;

extern bmstech::ADBMS6815::cell_asic IC[bmstech::LTC6811SPI::TOTAL_IC];		
//
//extern DigitalOut CS; // ssel

namespace bmstech
{	

	void ADBMS6815::VoltageTemperature_measurement_switch()
	{
		switch (LTC6811SPI::sw) 
		{
			case 0: //isoSPI 4.15ms *2pack
			{		
				ADBMS6815::adBmsWakeupIdle(TOTAL_IC);
				
//				ADBMS6815::adBms6815_clear_dcc_discharge(TOTAL_IC, &IC[0]);
				
				ADBMS6815::Start_ADC_Measurement();
				ADBMS6815::adBmsPollAdc(PLADC);
				ADBMS6815::adBms6815_read_cell_voltages(TOTAL_IC, &IC[0]);	
	
//				if (Balance::PassiveBalanceCommand_ON_flag == 1)
//				{
//					ADBMS6815::adBms6815_set_dcc_discharge(TOTAL_IC, &IC[0]);
//				}
	
				ADBMS6815::printVoltages(TOTAL_IC, &IC[0], Cell);
				
				LTC6811SPI::sw = 1;
				
			}
			break;
			
			case 1: //isoSPI ~2.78ms *2pack
			{			
//				ADBMS6815::adBmsWakeupIdle(TOTAL_IC);
				
				BatterySys::pack[0].channel = 4;
				ADBMS6815::adBms6815_start_aux_voltage_conversion(TOTAL_IC, &IC[0], SGM4518_CHANNEL_4);
				ADBMS6815::adBms6815_read_aux_voltages(TOTAL_IC, &IC[0]);
				ADBMS6815::printVoltages(TOTAL_IC, &IC[0], Aux);

				LTC6811SPI::sw = 2;
			}
			break;
			
			case 2: //isoSPI ~2.78ms *2pack
			{			
//				ADBMS6815::adBmsWakeupIdle(TOTAL_IC);
				
				BatterySys::pack[0].channel = 5;
				ADBMS6815::adBms6815_start_aux_voltage_conversion(TOTAL_IC, &IC[0], SGM4518_CHANNEL_5);
				ADBMS6815::adBms6815_read_aux_voltages(TOTAL_IC, &IC[0]);
				ADBMS6815::printVoltages(TOTAL_IC, &IC[0], Aux);

				LTC6811SPI::sw = 3;
			}
			break;
			
			case 3: //isoSPI ~2.78ms *2pack
			{			
//				ADBMS6815::adBmsWakeupIdle(TOTAL_IC);
				
				BatterySys::pack[0].channel = 6;
				ADBMS6815::adBms6815_start_aux_voltage_conversion(TOTAL_IC, &IC[0], SGM4518_CHANNEL_6);
				ADBMS6815::adBms6815_read_aux_voltages(TOTAL_IC, &IC[0]);
				ADBMS6815::printVoltages(TOTAL_IC, &IC[0], Aux);

				LTC6811SPI::sw = 4;
			}
			break;		
			
			case 4: //isoSPI ~2.78ms *2pack
			{			
//				ADBMS6815::adBmsWakeupIdle(TOTAL_IC);
				
				BatterySys::pack[0].channel = 7;
				ADBMS6815::adBms6815_start_aux_voltage_conversion(TOTAL_IC, &IC[0], SGM4518_CHANNEL_7);
				ADBMS6815::adBms6815_read_aux_voltages(TOTAL_IC, &IC[0]);
				ADBMS6815::printVoltages(TOTAL_IC, &IC[0], Aux);

				LTC6811SPI::sw = 5;
			}
			break;			
			
			case 5: //isoSPI ~2.78ms *2pack
			{			
//				ADBMS6815::adBmsWakeupIdle(TOTAL_IC);
				
				BatterySys::pack[0].channel = 3;
				ADBMS6815::adBms6815_start_aux_voltage_conversion(TOTAL_IC, &IC[0], SGM4518_CHANNEL_3);
				ADBMS6815::adBms6815_read_aux_voltages(TOTAL_IC, &IC[0]);
				ADBMS6815::printVoltages(TOTAL_IC, &IC[0], Aux);

				LTC6811SPI::sw = 6;
			}
			break;		
			
			case 6: //isoSPI ~2.78ms *2pack
			{			
//				ADBMS6815::adBmsWakeupIdle(TOTAL_IC);
				
				BatterySys::pack[0].channel = 2;
				ADBMS6815::adBms6815_start_aux_voltage_conversion(TOTAL_IC, &IC[0], SGM4518_CHANNEL_2);
				ADBMS6815::adBms6815_read_aux_voltages(TOTAL_IC, &IC[0]);
				ADBMS6815::printVoltages(TOTAL_IC, &IC[0], Aux);

				LTC6811SPI::sw = 0;
			}
			break;		
			
						
		    default:
			{
				LTC6811SPI::sw = 0;
			}
			break;
		}

	}
	
	
	void ADBMS6815::ADBMS6815Process()
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

			ADBMS6815::VoltageTemperature_measurement_switch();		
			LTC6811SPI::Update_PACK_Voltage();
			LTC6811SPI::BMSLTC6811VoltTemp_Flag = 0;	
			Green_LED = 0;
	
		}
	}
	


	
//	//Update Pack Total Voltage:
//	void LTC6811SPI::Update_PACK_Voltage()
//	{	
//		//current_ic = MAX_PACK_NUM
//		for (uint8_t current_ic = 0; current_ic < TOTAL_IC; current_ic++)
//		{		
//			BatterySys::pack[current_ic].packVolt = 0;
//			//MAX_CELL_NUM
//			for (uint16_t i = 0; i < PackConst::MAX_CELL_NUM; i++)
//			{
//				BatterySys::pack[current_ic].packVolt += BatterySys::pack[current_ic].cellVolt[i]; //Update Pack Total Voltage:			
//			}
//		}
//		
//		BatterySys::pBattSys->m_sysVolt = 0U;
//		//MAX_PACK_NUM
//		for (uint16_t i = 0U; i < PackConst::MAX_PACK_NUM; i++)
//		{
//			BatterySys::pBattSys->m_sysVolt += BatterySys::pack[i].packVolt; //total Sum
//		}
//		Balance::Average_voltReading = static_cast<uint16_t>(BatterySys::pBattSys->m_sysVolt)
//										/ ((PackConst::MAX_CELL_NUM)*(PackConst::MAX_PACK_NUM));
//
//		BatterySys::m_voltReading = static_cast<uint16_t>((BatterySys::pBattSys->m_sysVolt) / 100U); //convert to 0.1V
//	}
	
	/**
	*******************************************************************************
	* @brief Read Device SID.
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_read_device_sid(uint8_t tIC, cell_asic *ic)
	{
		//adBmsWakeupIc(tIC);
		adBmsReadData(tIC, &ic[0], RDSID, SID, NONE);
//		printDeviceSID(tIC, &ic[0], SID);
	}
	
	/**
	*******************************************************************************
	* @brief Set and reset the gpio pins(to drive output on gpio pins)
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_set_reset_gpio_pins(uint8_t tIC, cell_asic *ic)
	{
//		int gpio;
//#ifdef MBED
//		pc.printf("Please Enter: \n1:Set\n2:Reset\n");
//		pc.scanf("%d", &gpio);
//#else
//		printf("Please Enter: \n1:Set\n2:Reset\n");
//		scanf("%d", &gpio);
//#endif
		//adBmsWakeupIc(tIC);
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			ic[cic].tx_cfga.refon = PWR_UP;
			ic[cic].tx_cfga.gpo = 0x7F; /* All gpios pull down disable */
//			ic[cic].tx_cfga.gpo = 0x00; /* All gpios pin pull-down enabled. */
			
//			gpio = 1;//test
//			if (gpio == 1)
//			{
//#ifdef MBED
//				pc.printf("IC[%d]: All GPIOs Pin pull-down disabled\n", (cic + 1));
//#else
//				printf("IC[%d]: All GPIOs Pin pull-down disabled\n", (cic + 1));
//#endif
//				ic[cic].tx_cfga.gpo = 0x7F; /* All gpios pull down disable */
//			}
//			else if (gpio == 2)
//			{
//#ifdef MBED
//				pc.printf("IC[%d]: All GPIOs pin pull-down enabled\n", (cic + 1));
//#else
//				printf("IC[%d]: All GPIOs pin pull-down enabled\n", (cic + 1));
//#endif
//				ic[cic].tx_cfga.gpo = 0x00; /* All gpios pin pull-down enabled. */
//			}
		}
		//adBmsWakeupIc(tIC);
		adBmsWriteData(tIC, &ic[0], WRCFGA, Config, A);
		adBmsReadData(tIC, &ic[0], RDCFGA, Config, A);
//		printWriteConfig(tIC, &ic[0], Config, A);
//		printReadConfig(tIC, &ic[0], Config, A);
	}
	
	
	/**
	*******************************************************************************
	* @brief Soft Reset
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_soft_reset(uint8_t tIC)
	{
		adBmsWakeupIc(tIC);
		spiSendCmd(SRST);
#ifdef MBED
		pc.printf("Soft Reset done\n\n");
#else
		printf("Soft Reset done\n\n");
#endif
	}
	
	/**
	*******************************************************************************
	* @brief Reset command counter
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_reset_cmd_count(uint8_t tIC)
	{
		//adBmsWakeupIc(tIC);
		spiSendCmd(RSTCC);
#ifdef MBED
		pc.printf("Command Counter Reset done\n\n");
#else
//		printf("Command Counter Reset done\n\n");
#endif
	}
	
	/**
*******************************************************************************
* @brief Clear all flags
*******************************************************************************
*/
	void ADBMS6815::adBms6815_clear_all_flags(uint8_t tIC, cell_asic *ic)
	{
		/* CLRFLAG Data Format */
		/* CL_VAOV, CL_VAUV, CL_VDOV, CL_VDUV, CL_AED, CL_AMED, CL_ED, CL_MED */
		/* CL_REDF, CL_COMP, CL_SLEEP, CL_TMODE , ---, CL_THSD , CL_CPCHK, CL_OSCCHK */
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			ic[cic].clrflg_data.cl_med = CL_FLAG_SET;
			ic[cic].clrflg_data.cl_ed = CL_FLAG_SET;
			ic[cic].clrflg_data.cl_amed = CL_FLAG_SET;
			ic[cic].clrflg_data.cl_aed = CL_FLAG_SET;
			ic[cic].clrflg_data.cl_vduv = CL_FLAG_SET;
			ic[cic].clrflg_data.cl_vdov = CL_FLAG_SET;
			ic[cic].clrflg_data.cl_vauv = CL_FLAG_SET;
			ic[cic].clrflg_data.cl_vaov = CL_FLAG_SET;

			ic[cic].clrflg_data.cl_oscchk = CL_FLAG_SET;
			ic[cic].clrflg_data.cl_cpchk = CL_FLAG_SET;
			ic[cic].clrflg_data.cl_thsd = CL_FLAG_SET;
			ic[cic].clrflg_data.cl_tmode = CL_FLAG_SET;
			ic[cic].clrflg_data.cl_sleep = CL_FLAG_SET;
			ic[cic].clrflg_data.cl_comp = CL_FLAG_SET;
			ic[cic].clrflg_data.cl_redf = CL_FLAG_SET;
		}
		adBmsWakeupIc(tIC);
		adBmsWriteData(tIC, &ic[0], CLRFLAG, Clrflag, NONE);
		adBmsReadData(tIC, &ic[0], RDSTATC, Status, C);
//		printStatus(tIC, &ic[0], Status, C);
	}
	
	/**
	*******************************************************************************
	* @brief Reset PEC error flags
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_reset_pec_error_flag(uint8_t tIC, cell_asic *ic)
	{
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			ic[cic].cccrc.cfgr_pec      = 0;
			ic[cic].cccrc.cell_pec      = 0;
			ic[cic].cccrc.aux_pec       = 0;
			ic[cic].cccrc.stat_pec      = 0;
			ic[cic].cccrc.comm_pec      = 0;
			ic[cic].cccrc.dcell_pec     = 0;
			ic[cic].cccrc.pwm_pec       = 0;
			ic[cic].cccrc.sid_pec       = 0;
		}
#ifdef MBED
		pc.printf("Reset PEC Error Flag done\n\n");
#else
//		printf("Reset PEC Error Flag done\n\n");
#endif
	}
	
	/**
	*******************************************************************************
	* Function: printVoltages
	* @brief Print volatges in I/O terminal.
	*
	* @details This function Print volatges into I/O terminal.
	*
	* Parameters:
	* @param [in]   tIC      Total IC
	*
	* @param [in]  *ic      cell_asic stucture pointer
	*
	* @param [in]  TYPE   Enum type of resistor
	*
	* @return None
	*
	*******************************************************************************
	*/
	void ADBMS6815::printVoltages(uint8_t tIC, cell_asic *IC, TYPE type)
	{
		float voltage;
		uint16_t temp;
		uint8_t channel = 0;
		uint8_t index_Start = 0;
		uint8_t index_Stop = PackConst::MAX_CELL_NUM;
		
		if ((type == Cell) || (type == D_volt))
		{
			channel = CELL;
			index_Start = 0;
			index_Stop = PackConst::MAX_CELL_NUM;
		}
		else if (type == Aux)
		{ 
			channel = AUX; 
			index_Start = 0;
			//index_Stop = 8;
			index_Stop = AUX;
		}
		
		for (uint16_t current_ic = 0; current_ic < tIC; current_ic++)
		{
//			printf("IC%d:", (ic + 1));
			for (uint16_t i = index_Start; i < index_Stop; i++)	
			{
				//cell_channels	
				if (type == Cell)
				{
					temp = IC[current_ic].cell.c_codes[i]; 
					LTC6811SPI::Volt_CH[i] = temp * 0.1;
					BatterySys::pack[current_ic].cellVolt[i] = LTC6811SPI::Volt_CH[i];	
//					BatterySys::pack[current_ic].cellVolt[i] = IC[current_ic].cell.c_codes[i]; 

				}
				else if (type == D_volt)
				{
					temp = IC[current_ic].dcell.dc_codes[i]; 
//					voltage = getCdVoltage(temp); 
				}
				else if (type == Aux)
				{ 					
					//					temp = IC[current_ic].aux.a_codes[i]; 
					if (BatterySys::pack[0].channel == 4) //T1
					{
						BatterySys::pack[current_ic].cellTemp[0] = IC[current_ic].aux.a_codes[1] / 10 ;	//GPIO2
					}
					else if (BatterySys::pack[0].channel == 5) //T2
					{
						BatterySys::pack[current_ic].cellTemp[1] = IC[current_ic].aux.a_codes[1] / 10;	//GPIO2
					}
					else if (BatterySys::pack[0].channel == 6) //T3
					{
						BatterySys::pack[current_ic].cellTemp[2] = IC[current_ic].aux.a_codes[1] / 10;	//GPIO2
					}
					else if (BatterySys::pack[0].channel == 7) //T4
					{
						BatterySys::pack[current_ic].cellTemp[3] = IC[current_ic].aux.a_codes[1] / 10;	//GPIO2
					}
					else if (BatterySys::pack[0].channel == 3) //T5
					{
						BatterySys::pack[current_ic].cellTemp[4] = IC[current_ic].aux.a_codes[1] / 10;	//GPIO2
					}
					else if (BatterySys::pack[0].channel == 2) //T6
					{
						BatterySys::pack[current_ic].cellTemp[5] = IC[current_ic].aux.a_codes[1] / 10;	//GPIO2
					}
					
//					LTC6811SPI::Temp_CH[i - 1] = IC[current_ic].aux.a_codes[i] / 10; //GPIO6 Temp 2			
//					LTC6811SPI::ADCTemp_CH[i - 1] =  ADCC::Volt_temperatureTab(LTC6811SPI::Temp_CH[i - 1]);
//					BatterySys::pack[current_ic].cellTemp[i - 1] = LTC6811SPI::ADCTemp_CH[i - 1];
					//					voltage = getVoltage(temp); 
				}
		
				
			}
		}
	}
	
	/**
	*******************************************************************************
	* @brief Enable Mute
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_enable_mute(uint8_t tIC, cell_asic *ic)
	{
		adBmsWakeupIc(tIC);
		spiSendCmd(MUTE);
		adBmsReadData(tIC, &ic[0], RDCFGB, Config, B);
#ifdef MBED
		pc.printf("Received Configuration.After send MUTE Command\n");
#else
		printf("Received Configuration.After send MUTE Command\n");
#endif
//		printReadConfig(tIC, &ic[0], Config, B);
	}

	/**
	*******************************************************************************
	* @brief Disable Mute
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_disable_mute(uint8_t tIC, cell_asic *ic)
	{
		adBmsWakeupIc(tIC);
		spiSendCmd(UNMUTE);
		adBmsReadData(tIC, &ic[0], RDCFGB, Config, B);
#ifdef MBED
		pc.printf("Received Configuration.After send UNMUTE Command\n");
#else
		printf("Received Configuration.After send UNMUTE Command\n");
#endif
//		printReadConfig(tIC, &ic[0], Config, B);
	}
	
	/**
	*******************************************************************************
	* @brief Cell CVOW command open wire test sm 0.14 manual
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_cvow_cell_openwire(uint8_t tIC, cell_asic *ic)
	{
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			ic[cic].tx_cfga.cvmin = CVMIN_DEFAULT; /* Default value = 0            */
			ic[cic].tx_cfga.soakon = SOAKON_CLR; /* Default value = 0            */
			ic[cic].tx_cfga.owrng = SHORT; /* Default value = 0            */
			ic[cic].tx_cfga.owc = OWC0; /* Default value = 0            */
		}
		/* Write/Read config A*/
		adBmsWakeupIc(tIC);
		adBmsWriteData(tIC, &ic[0], WRCFGA, Config, A);
		adBmsReadData(tIC, &ic[0], RDCFGA, Config, A);
		adBmsWakeupIc(tIC);
//		adBms6815_Cvow(ADC_CONVERSION_MODE);
		adBms6815_Cvow(MODE_27KHz_TO_14KHz);
		
		adBmsPollAdc(PLADC);
		adBmsReadData(tIC, &ic[0], RDCDA, D_volt, A);
		adBmsReadData(tIC, &ic[0], RDCDB, D_volt, B);
		adBmsReadData(tIC, &ic[0], RDCDC, D_volt, C);
		adBmsReadData(tIC, &ic[0], RDCDD, D_volt, D);
		adBms6815_cell_open_detection_condition_check(tIC, &ic[0]);
//		printWriteConfig(tIC, &ic[0], Config, A);
//		printReadConfig(tIC, &ic[0], Config, A);
		printCellOpenWireTestResult(tIC, ic);
	}
	
	/**
	*******************************************************************************
	* Function: printCellOpenWireTestResult
	* @brief Print Cell open wire test result.
	*
	* @details This function print cell open wire result.
	*
	* Parameters:
	* @param [in]   tIC      Total IC
	*
	* @param [in]  *IC      cell_asic stucture pointer
	*
	* @return None
	*
	*******************************************************************************
	*/
	void ADBMS6815::printCellOpenWireTestResult(uint8_t tIC, cell_asic *IC)
	{
//		uint8_t total_cell = CELL;
		uint8_t total_cell = PackConst::MAX_PACK_NUM;
		printf("Cell Open Wire Test\n");
		for (uint8_t ic = 0; ic < tIC; ic++)
		{
			printf("IC%d:\n", (ic + 1));
			for (uint8_t cell = 0; cell < total_cell; cell++)
			{
				printf("CELL%d: ", (cell + 1));
				if (IC[ic].diag_result.cellopen_wire[cell] == 1)
				{
					printf("Digital redundancy fault was detected\n");
				}
				else if (IC[ic].diag_result.cellopen_wire[cell] == 2)
				{
					printf("Low cell voltage was detected \n");
				}
				else if (IC[ic].diag_result.cellopen_wire[cell] == 3)
				{
//					printf("C%d or C%d is open\n", (cell + 1), cell);
					SEGGER_RTT_printf(0, "C%d or C%d is open \r\n", (cell + 1), cell);
					LTC6811SPI::communication_Volt_counter_Flag = 1;
					
					
				}
				else if (IC[ic].diag_result.cellopen_wire[cell] == 4)
				{
					printf("Latent fault occurred\n");
				}
				else if (IC[ic].diag_result.cellopen_wire[cell] == 5)
				{
//					printf("Cell is close\n");
				
					SEGGER_RTT_printf(0, "Cell is close \r\n");
					
					
				}
			}
			printf("\n\n");
		}
	}

	/**
	*******************************************************************************
	* @brief Cell open wire detection condtion check
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_cell_open_detection_condition_check(uint8_t tIC, cell_asic *ic)
	{
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			for (uint8_t cell = 0; cell < PackConst::MAX_PACK_NUM; cell++)
			{
				if ((ic[cic].dcell.dc_codes[cell] & 0xFFF0) == 0x7FA0)
				{
					ic[cic].diag_result.cellopen_wire[cell] = 1; /* Digital redundancy fault is detected.*/
				}
				else if (ic[cic].dcell.dc_codes[cell] == 0x7FC0)
				{
					ic[cic].diag_result.cellopen_wire[cell] = 2; /* Open wire detection is not possible, Low cell voltage condition.*/
				}
				else if (ic[cic].dcell.dc_codes[cell] > OPENWIRE_VTH_SPF)
				{
					ic[cic].diag_result.cellopen_wire[cell] = 3; /* Cx or C(x - 1) is open.*/
				}
				else if (ic[cic].dcell.dc_codes[cell] < OPENWIRE_VTH_LF)
				{
					ic[cic].diag_result.cellopen_wire[cell] = 4; /* Latent fault occurred in the squeeze current sources.*/
				}
				else
				{
					ic[cic].diag_result.cellopen_wire[cell] = 5; /* Cell Close */
				}
			}
		}
	}
	
	/**
	*******************************************************************************
	* @brief Aux open wire test sm 0.14 manual
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_aux_openwire(uint8_t tIC, cell_asic *ic)
	{
		uint8_t data_size = AUX;
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			ic[cic].tx_cfga.refon = PWR_UP;
			ic[cic].tx_cfga.adcopt = ADCOPT_OFF;
			ic[cic].tx_cfga.gpo = 0x7F; /* All gpios pull down disable      */
			ic[cic].tx_cfga.soakon = SOAKON_SET; /* Set default value = 0            */
			ic[cic].tx_cfga.owrng = SHORT; /* Set default value = 0            */
			ic[cic].tx_cfga.owa = OWA0; /* Set default value = 0            */
		}
		/* Write/Read config A*/
		adBmsWakeupIc(tIC);
		adBmsWriteData(tIC, &ic[0], WRCFGA, Config, A);
		adBmsReadData(tIC, &ic[0], RDCFGA, Config, A);

		/* Read Adax gpios voltages */
		adBmsWakeupIc(tIC);
		adBmss6815_Adax(ADBMS6815::MODE_7KHz_TO_3KHz, GPIOS_VREF2_VREF3);

		adBmsPollAdc(PLADC);
//		wait_ms(4);
//		adBmsWakeupIc(tIC);
			
		adBmsReadData(tIC, &ic[0], RDAUXA, Aux, A);
		adBmsReadData(tIC, &ic[0], RDAUXB, Aux, B);
		adBmsReadData(tIC, &ic[0], RDAUXC, Aux, C);
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			memcpy(&ic[cic].ax_opwire.adax_gpio[0], &ic[cic].aux.a_codes[0], data_size); /* dst, src, size */
		}

		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			ic[cic].tx_cfga.refon = PWR_UP;
			ic[cic].tx_cfga.adcopt = ADCOPT_OFF;
			ic[cic].tx_cfga.gpo = 0x7F; /* All gpios pull down disable  */
			ic[cic].tx_cfga.soakon = SOAKON_SET; /* Enable soakon delay          */
			ic[cic].tx_cfga.owrng = SHORT; /* Soak time rang               */
			ic[cic].tx_cfga.owa = OWA6; /* Owa values                   */
		}
		/* Write/Read config A*/
		adBmsWakeupIc(tIC);
		adBmsWriteData(tIC, &ic[0], WRCFGA, Config, A);
		adBmsReadData(tIC, &ic[0], RDCFGA, Config, A);

		/* Read axow pup up gpios voltages */
		adBmsWakeupIc(tIC);
//		adBms6815_Axow(ADC_CONVERSION_MODE, PUP_UP, AUX_CH_TO_CONVERT);
		adBms6815_Axow(MODE_7KHz_TO_3KHz, PUP_UP, GPIOS_VREF2_VREF3);
		adBmsPollAdc(PLADC);
//		wait_ms(4);
//		adBmsWakeupIc(tIC);
		adBmsReadData(tIC, &ic[0], RDAUXA, Aux, A);
		adBmsReadData(tIC, &ic[0], RDAUXB, Aux, B);
		adBmsReadData(tIC, &ic[0], RDAUXC, Aux, C);
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			memcpy(&ic[cic].ax_opwire.axow_pu[0], &ic[cic].aux.a_codes[0], data_size); /* dst, src, size */
		}
		/* Calculate Axow delta PU voltages */
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			for (uint8_t gpio = 1; gpio < (data_size - 1); gpio++)
			{
				ic[cic].ax_opwire.delta_pu[gpio] = (ic[cic].ax_opwire.axow_pu[gpio] - ic[cic].ax_opwire.adax_gpio[gpio]);
			}
		}

		/* Read axow pup down gpios voltages */
		adBmsWakeupIc(tIC);
//		adBms6815_Axow(ADC_CONVERSION_MODE, PUP_DOWN, AUX_CH_TO_CONVERT);
		adBms6815_Axow(MODE_7KHz_TO_3KHz, PUP_DOWN, GPIOS_VREF2_VREF3);
		adBmsPollAdc(PLADC);
//		wait_ms(4);
//		adBmsWakeupIc(tIC);
		adBmsReadData(tIC, &ic[0], RDAUXA, Aux, A);
		adBmsReadData(tIC, &ic[0], RDAUXB, Aux, B);
		adBmsReadData(tIC, &ic[0], RDAUXC, Aux, C);
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			memcpy(&ic[cic].ax_opwire.axow_pd[0], &ic[cic].aux.a_codes[0], data_size); /* dst, src, size */
		}
		/* Calculate axow delta PD voltages */
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			for (uint8_t gpio = 1; gpio < (data_size - 1); gpio++)
			{
				if ((ic[cic].ax_opwire.axow_pd[0] - ic[0].ax_opwire.adax_gpio[gpio]) > 0)
				{
					ic[cic].ax_opwire.delta_pd[gpio] = (ic[cic].ax_opwire.axow_pd[gpio] - ic[cic].ax_opwire.adax_gpio[gpio]);
				}
				else
				{
					ic[cic].ax_opwire.delta_pd[gpio] = (ic[cic].ax_opwire.adax_gpio[gpio] - ic[cic].ax_opwire.axow_pd[gpio]);
				}
			}
		}
		adBms6815_aux_open_detection_condtion_check(tIC, ic);
		printAuxOpenWireTestResult(tIC, ic);
	}

	void ADBMS6815::adBms6815_aux_open_detection_condtion_check(uint8_t tIC, cell_asic *ic)
	{
		uint8_t data_size = AUX;
		uint16_t ref1 = 27500, ref2 = 26000, ref3 = 300; /* 27500 = 2.75 volt, 26000 = 2.6 volt and 300 = 30mV*/
		float delta1 = 0.025557399, delta2 = 0.921838959, delta;
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			delta = 0.0;
			for (uint8_t gpio = 1; gpio < (data_size - 1); gpio++)
			{
				/* GPIO open detection check */
				delta = (ic[cic].ax_opwire.adax_gpio[gpio] / (float)ic[cic].ax_opwire.adax_gpio[0]);
				if (delta < delta1)
				{
					ic[cic].diag_result.auxopen_wire[gpio] = 1;
				}
				else
				{
					if (delta > delta2)
					{
						ic[cic].diag_result.auxopen_wire[gpio] = 2;
					}
					else
					{
						if (ic[cic].ax_opwire.adax_gpio[gpio] >= ref1)
						{
							ic[cic].ax_opwire.delta_volt[gpio] = ic[cic].ax_opwire.delta_pd[gpio];
						}
						else
						{
							ic[cic].ax_opwire.delta_volt[gpio] = ic[cic].ax_opwire.delta_pu[gpio];
						}

						if (ic[cic].ax_opwire.delta_volt[gpio] > ref2)
						{
							ic[cic].diag_result.auxopen_wire[gpio] = 3;
						}
						else if (ic[cic].ax_opwire.delta_volt[gpio] < ref3)
						{
							ic[cic].diag_result.auxopen_wire[gpio] = 4;
						}
						else
						{
							ic[cic].diag_result.auxopen_wire[gpio] = 5;
						}
					}
				}
			}
		}
	}
	
	/**
	*******************************************************************************
	* Function: printAuxOpenWireTestResult
	* @brief Print Aux open wire test result.
	*
	* @details This function print aux open wire path.
	*
	* Parameters:
	* @param [in]   tIC      Total IC
	*
	* @param [in]  *IC      cell_asic stucture pointer
	*
	* @return None
	*
	*******************************************************************************
	*/
	void ADBMS6815::printAuxOpenWireTestResult(uint8_t tIC, cell_asic *IC)
	{
		uint8_t data_size = AUX;
		printf("Aux Open Wire Test\n");
		for (uint8_t ic = 0; ic < tIC; ic++)
		{
			printf("IC%d:\n", (ic + 1));
			for (uint8_t gpio = 1; gpio < (data_size - 1); gpio++)
			{
				printf("GPIO%d: ", gpio);
				if (IC[ic].diag_result.auxopen_wire[gpio] == 1)
				{
					printf("Open at 1\n");
				}
				else if (IC[ic].diag_result.auxopen_wire[gpio] == 2)
				{
					printf("Open at 2 or 4\n");
				}
				else if (IC[ic].diag_result.auxopen_wire[gpio] == 3)
				{
					printf("Open at 3 \n");
				}
				else if (IC[ic].diag_result.auxopen_wire[gpio] == 4)
				{
					printf("Stuck on/off current source 5, 6, 7 and 8\n");
				}
				else if (IC[ic].diag_result.auxopen_wire[gpio] == 5)
				{
					printf("No Fault\n");
				}
			}
			printf("\n\n");
		}
	}

	/**
*******************************************************************************
* @brief Enable DCC discharge.
*******************************************************************************
*/
	void ADBMS6815::adBms6815_set_dcc_discharge(uint8_t tIC, cell_asic *ic)
	{
		//int spin_no = 0;
		//#ifdef MBED
		//		pc.printf("Please enter the Spin number(1-%d)\n", CELL);
		//		pc.scanf("%d", &spin_no);
		//#else
		//		printf("Please enter the Spin number(1-%d)\n", CELL);
		//		scanf("%d", &spin_no);
		//#endif
		//		while (spin_no > CELL)
		//		{
		//#ifdef MBED
		//			pc.printf("Please enter correct Spin number\n");
		//			pc.scanf("%d", &spin_no);
		//#else
		//			printf("Please enter correct Spin number\n");
		//			scanf("%d", &spin_no);
		//#endif
		//		}
		//#ifdef MBED
		//		pc.printf("Enter spin no:%d\n", spin_no);
		//#else
		//		printf("Enter spin no:%d\n", spin_no);
		//#endif
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			/* Init config B */
			ic[cic].tx_cfgb.dcc = 0;
			for (uint8_t i = 0; i < CELL; i++)
			{
				if((uint8_t) BatterySys::pack[cic].cellBalance[i] !=0)
				{
					ic[cic].tx_cfgb.dcc |= 1 << i;
				}
			}
		}
		
//		adBmsWakeupIdle(tIC);
		adBmsWriteData(tIC, &ic[0], WRCFGB, Config, B);
		adBmsReadData(tIC, &ic[0], RDCFGB, Config, B);
		//printWriteConfig(tIC, &ic[0], Config, B);
		//printReadConfig(tIC, &ic[0], Config, B);
	}

	/**
	*******************************************************************************
	* @brief Clear DCC discharge.
	*******************************************************************************
	*/
	void ADBMS6815::adBms6815_clear_dcc_discharge(uint8_t tIC, cell_asic *ic)
	{
		for (uint8_t cic = 0; cic < tIC; cic++)
		{
			/* Init config B */
			ic[cic].tx_cfgb.dcc = 0;
		}
		adBmsWakeupIdle(tIC);
		
		adBmsWriteData(tIC, &ic[0], WRCFGB, Config, B);
//		adBmsReadData(tIC, &ic[0], RDCFGB, Config, B);
	}
	
	
} //namespace bmstech

		
