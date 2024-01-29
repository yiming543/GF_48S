//File
#pragma once
#include <mbed.h>

#include "PackConst.h"

namespace bmstech
{

//#define ISOSPI_COMMUNICATION_COUNTER 5
//#define MAX_ISOSPI_COMMUNICATION_COUNTER 5
//#define LTC6811_SPI_BAUD	(700000) //100K 200K 700K 900K 1000K
//#define BMU_ADDRESS 0xC0
//#define TEMP_CH_IDX1 0x81
//#define TEMP_CH_IDX2 0x82
//#define TEMP_CH_IDX3 0x83
//#define TEMP_CH_IDX4 0x84
//#define TEMP_CH_IDX5 0x85
//#define TEMP_CH_IDX6 0x86
const uint8_t ISOSPI_COMMUNICATION_COUNTER = 5U;
const uint8_t MAX_ISOSPI_COMMUNICATION_COUNTER = 5U;
const uint32_t LTC6811_SPI_BAUD = 1000000;	//100K 200K 700K 900K 1000K 1700K
const uint8_t BMU_ADDRESS = 0xC0;
const uint8_t TEMP_CH_IDX1 = 0x81;
const uint8_t TEMP_CH_IDX2 = 0x82;
const uint8_t TEMP_CH_IDX3 = 0x83;
const uint8_t TEMP_CH_IDX4 = 0x84;
const uint8_t TEMP_CH_IDX5 = 0x85;
const uint8_t TEMP_CH_IDX6 = 0x86;
const uint8_t HEARTBEAT = 0x90;
const uint8_t REDLED_VERSION = 0x9F;
//#define HEARTBEAT 0x90
//#define REDLED_VERSION 0x9F
//#define Temp_address_D2 0xAE
//#define MD_422HZ_1KHZ 0
//#define MD_27KHZ_14KHZ 1
//#define MD_7KHZ_3KHZ 2
//#define MD_26HZ_2KHZ 3
//
//#define ADC_OPT_ENABLED 1
//#define ADC_OPT_DISABLED 0
//#define CELL_CH_ALL 0

const uint8_t CELL_CH_ALL = 0;
//const uint8_t MD_422HZ_1KHZ = 0U;
//const uint8_t MD_27KHZ_14KHZ = 1U;
const uint8_t MD_7KHZ_3KHZ = 2;
//const uint8_t MD_26HZ_2KHZ = 3U;
//const uint8_t ADC_OPT_ENABLED = 1U;
//const uint8_t ADC_OPT_DISABLED = 0U;

#define ADG729_address1  0x88
#define ADG729_address2  0x8C
#define S1A_S1B  0x11
#define S2A_S2B  0x22
#define S3A_S3B  0x44
#define S4A_S4B  0x88

#define SELFTEST_1 1
#define SELFTEST_2 2

#define AUX_CH_ALL 0
#define AUX_CH_GPIO1 1
#define AUX_CH_GPIO2 2
#define AUX_CH_GPIO3 3
#define AUX_CH_GPIO4 4
#define AUX_CH_GPIO5 5
#define AUX_CH_VREF2 6
	
#define REG_ALL 0
#define REG_1 1
#define REG_2 2
#define REG_3 3
#define REG_4 4
#define REG_5 5
#define REG_6 6

#define STAT_CH_ALL 0
#define STAT_CH_SOC 1
#define STAT_CH_ITEMP 2
#define STAT_CH_VREGA 3
#define STAT_CH_VREGD 4

#define DCP_DISABLED 0
#define DCP_ENABLED 1

#define PULL_UP_CURRENT 1
#define PULL_DOWN_CURRENT 0



//#define NUM_RX_BYT 8
//#define CELL 1
//#define AUX 2
//#define STAT 3
//#define CFGR 0
//#define CFGRB 4
//#define CS_PIN 10	
	
#define UI_BUFFER_SIZE 64
#define SERIAL_TERMINATOR '\n'

#define ENABLED 1
#define DISABLED 0

#define DATALOG_ENABLED 1
#define DATALOG_DISABLED 0
	
#define MAX_VOLT_ERROR_COUNTER 128
	
		//!<precomputed CRC15 Table
static const uint16_t crc15Table[256] = {0x0U ,0xc599U, 0xceabU, 0xb32U, 0xd8cfU, 0x1d56U, 0x1664U, 0xd3fdU, 0xf407U, 0x319eU, 0x3aacU,
						0xff35U, 0x2cc8U, 0xe951U, 0xe263U, 0x27faU, 0xad97U, 0x680eU, 0x633cU, 0xa6a5U, 0x7558U, 0xb0c1U,
						0xbbf3U, 0x7e6aU, 0x5990U, 0x9c09U, 0x973bU, 0x52a2U, 0x815fU, 0x44c6U, 0x4ff4U, 0x8a6dU, 0x5b2eU,
						0x9eb7U, 0x9585U, 0x501cU, 0x83e1U, 0x4678U, 0x4d4aU, 0x88d3U, 0xaf29U, 0x6ab0U, 0x6182U, 0xa41bU,
						0x77e6U, 0xb27fU, 0xb94dU, 0x7cd4U, 0xf6b9U, 0x3320U, 0x3812U, 0xfd8bU, 0x2e76U, 0xebefU, 0xe0ddU,
						0x2544U, 0x2beU,  0xc727U, 0xcc15U, 0x98cU,  0xda71U, 0x1fe8U, 0x14daU, 0xd143U, 0xf3c5U, 0x365cU,
						0x3d6eU, 0xf8f7U, 0x2b0aU, 0xee93U, 0xe5a1U, 0x2038U,  0x7c2U, 0xc25bU, 0xc969U, 0xcf0U,  0xdf0dU,
						0x1a94U, 0x11a6U, 0xd43fU, 0x5e52U, 0x9bcbU, 0x90f9U, 0x5560U, 0x869dU, 0x4304U, 0x4836U, 0x8dafU,
						0xaa55U, 0x6fccU, 0x64feU, 0xa167U, 0x729aU, 0xb703U, 0xbc31U, 0x79a8U, 0xa8ebU, 0x6d72U, 0x6640U,
						0xa3d9U, 0x7024U, 0xb5bdU, 0xbe8fU, 0x7b16U, 0x5cecU, 0x9975U, 0x9247U, 0x57deU, 0x8423U, 0x41baU,
						0x4a88U, 0x8f11U, 0x57cU,  0xc0e5U, 0xcbd7U,  0xe4eU, 0xddb3U, 0x182aU, 0x1318U, 0xd681U, 0xf17bU,
						0x34e2U, 0x3fd0U, 0xfa49U, 0x29b4U, 0xec2dU, 0xe71fU, 0x2286U, 0xa213U, 0x678aU, 0x6cb8U, 0xa921U,
						0x7adcU, 0xbf45U, 0xb477U, 0x71eeU, 0x5614U, 0x938dU, 0x98bfU, 0x5d26U, 0x8edbU, 0x4b42U, 0x4070U,
						0x85e9U, 0xf84U,  0xca1dU, 0xc12fU, 0x4b6U,  0xd74bU, 0x12d2U, 0x19e0U, 0xdc79U, 0xfb83U, 0x3e1aU, 0x3528U,
						0xf0b1U, 0x234cU, 0xe6d5U, 0xede7U, 0x287eU, 0xf93dU, 0x3ca4U, 0x3796U, 0xf20fU, 0x21f2U, 0xe46bU, 0xef59U,
						0x2ac0U, 0xd3aU,  0xc8a3U, 0xc391U, 0x608U,  0xd5f5U, 0x106cU, 0x1b5eU, 0xdec7U, 0x54aaU, 0x9133U, 0x9a01U,
						0x5f98U, 0x8c65U, 0x49fcU, 0x42ceU, 0x8757U, 0xa0adU, 0x6534U, 0x6e06U, 0xab9fU, 0x7862U, 0xbdfbU, 0xb6c9U,
						0x7350U, 0x51d6U, 0x944fU, 0x9f7dU, 0x5ae4U, 0x8919U, 0x4c80U, 0x47b2U, 0x822bU, 0xa5d1U, 0x6048U, 0x6b7aU,
						0xaee3U, 0x7d1eU, 0xb887U, 0xb3b5U, 0x762cU, 0xfc41U, 0x39d8U, 0x32eaU, 0xf773U, 0x248eU, 0xe117U, 0xea25U,
						0x2fbcU, 0x846U,  0xcddfU, 0xc6edU, 0x374U,  0xd089U, 0x1510U, 0x1e22U, 0xdbbbU,  0xaf8U, 0xcf61U, 0xc453U,
						0x1caU,  0xd237U, 0x17aeU, 0x1c9cU, 0xd905U, 0xfeffU, 0x3b66U, 0x3054U, 0xf5cdU, 0x2630U, 0xe3a9U, 0xe89bU,
						0x2d02U, 0xa76fU, 0x62f6U, 0x69c4U, 0xac5dU, 0x7fa0U, 0xba39U, 0xb10bU, 0x7492U, 0x5368U, 0x96f1U, 0x9dc3U,
						0x585aU, 0x8ba7U, 0x4e3eU, 0x450cU, 0x8095U													   
					    };
	
    class LTC6811SPI 
    {
    public:
	       	    
	    static const uint8_t ADC_CONVERSION_MODE = MD_7KHZ_3KHZ; //MD_7KHZ_3KHZ; //MD_26HZ_2KHZ;//MD_7KHZ_3KHZ;
	    static const uint8_t ADC_DCP = DCP_DISABLED; //DCP_DISABLED; // See LTC6811_daisy.h for Options
	    static const uint8_t CELL_CH_TO_CONVERT = CELL_CH_ALL; // See LTC6811_daisy.h for Options
	    static const uint8_t AUX_CH_TO_CONVERT = AUX_CH_ALL; // See LTC6811_daisy.h for Options
	    static const uint8_t STAT_CH_TO_CONVERT = STAT_CH_ALL; // See LTC6811_daisy.h for Options

	    static const uint16_t MEASUREMENT_LOOP_TIME = 500U; //milliseconds(mS)

	    //Under Voltage and Over Voltage Thresholds
	    static const uint16_t OV_THRESHOLD = 36500; // Over voltage threshold ADC Code. LSB = 0.0001
	    static const uint16_t UV_THRESHOLD = 20000; // Under voltage threshold ADC Code. LSB = 0.0001

	    //Loop Measurement Setup These Variables are ENABLED or DISABLED Remember ALL CAPS
	    static const uint8_t WRITE_CONFIG = DISABLED; // This is ENABLED or DISABLED
	    static const uint8_t READ_CONFIG = DISABLED; // This is ENABLED or DISABLED
	    static const uint8_t MEASURE_CELL = ENABLED; // This is ENABLED or DISABLED
	    static const uint8_t MEASURE_AUX = DISABLED; // This is ENABLED or DISABLED
	    static const uint8_t MEASURE_STAT = DISABLED; //This is ENABLED or DISABLED
	    static const uint8_t PRINT_PEC = DISABLED; //This is ENABLED or DISABLED

	    static int8_t error;
	    static uint32_t conv_time;
	    
	    static int8_t error_message;	//TUV rule
	    
	    static uint16_t Volt_PEC_Err;
	    static uint16_t communication_Volt_counter;
	    static bool_t communication_Volt_counter_Flag;
	    static bool_t communication_Temp_counter_Flag;
	    
	    static uint16_t Volt_error_counter; 
	    static bool_t Volt_PEC_Err_Flag;
	    static uint16_t com_PEC_Err;
	  
	    static uint16_t com_error_counter;
	    static uint16_t com_PEC_Err_Flag;
	    
	    static bool_t Overlap_Test_Flag;
	    
	    
	    static uint16_t cell_codes[PackConst::MAX_PACK_NUM][12];


	    typedef struct 
	    {
		    uint16_t c_codes[PackConst::MAX_CELL_NUM]; //!< Cell Voltage Codes	//12-->18
		    uint8_t pec_match[6]; //!< If a PEC error was detected during most recent read cmd
	    } CV;

	    //! AUX Reg Voltage Data
	    typedef struct 
	    {
//		    uint16_t a_codes[9]; //!< Aux Voltage Codes
		    uint16_t a_codes[12]; //!< Aux Voltage Codes
		    uint8_t pec_match[4]; //!< If a PEC error was detected during most recent read cmd
	    } AX;

	    typedef struct 
	    {
		    uint16_t stat_codes[4]; //!< A two dimensional array of the stat voltage codes.
		    uint8_t flags[3]; //!< byte array that contains the uv/ov flag data
		    uint8_t mux_fail[1]; //!< Mux self test status flag
		    uint8_t thsd[1]; //!< Thermal shutdown status
		    uint8_t pec_match[2]; //!< If a PEC error was detected during most recent read cmd
	    } ST;

	    typedef struct 
	    {
		    uint8_t tx_data[6];
		    uint8_t rx_data[8];
		    uint8_t rx_pec_match; //!< If a PEC error was detected during most recent read cmd
	    } IC_register;

	    typedef struct 
	    {
		    uint16_t pec_count;
		    uint16_t cfgr_pec;
		    uint16_t cell_pec[6];
		    uint16_t aux_pec[4];
		    uint16_t stat_pec[2];
	    } PEC_counter;

	    typedef struct 
	    {
		    uint8_t cell_channels;
		    uint8_t stat_channels;
		    uint8_t aux_channels;
		    uint8_t num_cv_reg;
		    uint8_t num_gpio_reg;
		    uint8_t num_stat_reg;
	    } Register_cfg;
	    
	    typedef struct 
	    {
		    IC_register config;
		    IC_register configb;
		    CV   cells;
		    AX   aux;
		    ST   stat;
		    IC_register  com;
		    IC_register pwm;
		    IC_register pwmb;
		    IC_register sctrl;
		    IC_register sctrlb;
		    uint8_t sid[6];	//20230801
		    bool isospi_reverse;
		    PEC_counter crc_count;
		    Register_cfg ic_reg;
		    long system_open_wire;
	    } Cell_asic;
	    

	
//	    static  const int TOTAL_IC = PackConst::MAX_PACK_NUM; //!<number of ICs in the daisy chain
	    static  const uint8_t TOTAL_IC = PackConst::MAX_PACK_NUM; //!<number of ICs in the daisy chain
	    static uint8_t sw;
	    static uint8_t heartbeatSwitch;
	    
	    static uint8_t	heartbeat_value;
	    
	    static uint16_t Temp_CH[10];
	    static int16_t ADCTemp_CH[10];
	    static uint16_t Volt_CH[PackConst::MAX_CELL_NUM];
	    static int16_t PackTemp_CH[4];
	    static int16_t ADCPackTemp_CH[4];
	    
	    static uint16_t Configuration_RegisterA;
	    static uint16_t Configuration_RegisterB;
	    static bool_t Input_GPIO9;

	   
	    static uint8_t heartbeat_CH[4];
	    
	    static bool_t BMSLTC6811VoltTemp_Flag;
	    
	    static uint8_t cell_data[384]; //JY 20200706
	    static uint8_t read_buffer[384];	//LTC681x_rdcomm
	    static uint8_t data[384];	//read_6811
	    static uint8_t cmd[384];    //write_6811
	    static uint8_t write_buffer[384];	//LTC681x_wrcfg
//	    static int16_t pec15Table[256];
//	    static int16_t CRC15_POLY = 0x4599;

	    static void SPI_init();

	    // Option: Number of bytes to be written on the SPI port
	    //Array of bytes to be written on the SPI port
	    static void SPI_write_array(uint16_t len, uint8_t data[]);


	    static void SPI_write_read(uint8_t tx_Data[],//array of data to be written on SPI port
								   uint16_t tx_len, //length of the tx data arry
								   uint8_t *rx_data,//Input: array that will store the data read by the SPI port
								   uint16_t rx_len //Option: number of bytes to be read from the SPI port
								   );

	    //name conflicts with linduino also needs to take a byte as a parameter
	    static uint8_t SPI_read_byte(const uint8_t tx_dat);


	    static void Wakeup_IDLE(uint8_t total_ic); //!< number of ICs in the daisy chain
	    
	    static void Wakeup_IDLE(uint8_t total_ic, uint16_t delay_time);
		    
	    /*!  Wake the LTC6813 from the sleep state */
	    static void Wakeup_SLEEP(uint8_t total_ic); //!< number of ICs in the daisy chain
	   
	    static 	uint16_t PEC15_CALC(uint8_t len, uint8_t *data);
	    static void CMD_6811(uint8_t tx_cmd[2]);
	    static void WRITE_6811(uint8_t total_ic, uint8_t tx_cmd[2], uint8_t data[]);
	    static int8_t READ_6811(uint8_t total_ic, uint8_t tx_cmd[2], uint8_t *rx_data);
	    static void LTC681x_adcv(uint8_t MD, uint8_t DCP, uint8_t CH);
	    static void LTC681x_adcvsc(uint8_t MD, uint8_t DCP);
	    static void LTC681x_adcvax(uint8_t MD, uint8_t DCP);
//	    static uint8_t LTC6811_poll_adc();
	
	    static uint32_t LTC681x_pollAdc();
//	    static uint32_t LTC6811_pollAdc_finished();
		static void LTC6811_ADCstat(uint8_t MD, uint8_t CHST);
	    static void LTC6811_adax(uint8_t MD, uint8_t CHG);
	    static void LTC681x_adaxd(uint8_t MD, uint8_t CHG);
//	    static void LTC6811_rdcv_reg(uint8_t reg, uint8_t total_ic, uint8_t *data);
	    static void LTC681x_init_cfg(uint8_t total_ic, Cell_asic ic[]);
	    
	    //static void LTC681x_set_cfgr(uint8_t nIC, cell_asic ic[], bool refon, bool adcopt, bool gpio[5], bool dcc[12]);

//	    static void LTC681x_set_cfgr(uint8_t nIC, cell_asic ic[]
//									, bool refon, bool adcopt, bool gpio[5], bool dcc[12], bool dtco[4]);

	    static void LTC681x_set_cfgr1(uint8_t nIC, Cell_asic ic[], bool refon, bool adcopt, bool gpio[5]);
	    static void LTC681x_set_cfgr2(uint8_t nIC, Cell_asic ic[], bool dcc[12], bool dtco[4]);
	    
	    static void LTC681x_set_cfgr_refon(uint8_t nIC, Cell_asic ic[], bool refon);
	    static void LTC681x_set_cfgr_adcopt(uint8_t nIC, Cell_asic ic[], bool adcopt);
	    static void LTC681x_set_cfgr_gpio(uint8_t nIC, Cell_asic ic[], bool gpio[5]);
	    static void LTC681x_set_cfgr_dis(uint8_t nIC, Cell_asic ic[], bool dcc[12]);
	    static void LTC681x_set_cfgr_uv(uint8_t nIC, Cell_asic ic[], uint16_t uv);
	    static void LTC681x_set_cfgr_ov(uint8_t nIC, Cell_asic ic[], uint16_t ov);
	    static void LTC681x_adstatd(uint8_t MD, uint8_t CHST);
	    static void LTC681x_adow(uint8_t MD, uint8_t PUP);
	    static void LTC681x_adol(uint8_t MD, uint8_t DCP);
	    static void LTC681x_reset_crc_count(uint8_t total_ic, Cell_asic ic[]);
	    static void LTC6811_init_reg_limits(uint8_t total_ic, Cell_asic ic[]);
	    static void LTC681x_wrcfg(uint8_t total_ic, Cell_asic ic[]);
	    static void LTC6811_wrcfg(uint8_t total_ic, //The number of ICs being written to
		    Cell_asic ic[] //A two dimensional array of the configuration data that will be written
		   );
	    static int8_t LTC6811_rdcfg(uint8_t total_ic, Cell_asic ic[]);
	    static int8_t LTC681x_rdcfg(uint8_t total_ic, Cell_asic ic[]);
	    static void LTC681x_check_pec(uint8_t total_ic, uint8_t reg, Cell_asic ic[]);
	    static void Check_error(int8_t error);

	    
	    
	    
	    static uint8_t LTC6811_rdcv_AMS(uint8_t reg, // Controls which cell voltage register is read back.
		    uint8_t total_ic, // the number of ICs in the system
		    uint16_t cell_codes[][12] // Array of the parsed cell codes
		   );
	    static void LTC6811_rdcv_reg_AMS(uint8_t reg, //Determines which cell voltage register is read back
		    uint8_t total_ic, //the number of ICs in the
		    uint8_t *data //An array of the unparsed cell codes
		   );
	    
	    
	    
	    static uint8_t LTC6811_rdcv(uint8_t reg, // Controls which cell voltage register is read back.
		    uint8_t total_ic, // the number of ICs in the system
		    Cell_asic ic[] // Array of the parsed cell codes
		   );
	    static void LTC681x_rdcv_reg(uint8_t reg, uint8_t total_ic, uint8_t *data);
	    static uint8_t LTC681x_rdcv(uint8_t reg, uint8_t total_ic, Cell_asic ic[]);
	    static int8_t Parse_cells(uint8_t current_ic, uint8_t cell_reg
	    		, uint8_t cell_data[], uint16_t *cell_codes, uint8_t *ic_pec);
	   
	    static void LTC681x_rdaux_reg(uint8_t reg, //Determines which GPIO voltage register is read back
		    uint8_t total_ic, //The number of ICs in the system
		    uint8_t *data //Array of the unparsed auxiliary codes
		   );
	    static int8_t LTC6811_rdaux(uint8_t reg, //Determines which GPIO voltage register is read back.
		    uint8_t total_ic,//The number of ICs in the system
		    Cell_asic *ic//A two dimensional array of the gpio voltage codes.
		    );
	    static int8_t LTC681x_rdaux(uint8_t reg, //Determines which GPIO voltage register is read back.
		    uint8_t total_ic,//the number of ICs in the system
		    Cell_asic ic[]//A two dimensional array of the gpio voltage codes.
		   );
	    static void LTC681x_adstat(uint8_t MD, uint8_t CHST);
		    
		static int8_t LTC6811_rdstat(uint8_t reg, //Determines which Stat  register is read back.
			    uint8_t total_ic,//the number of ICs in the system
			    Cell_asic ic[]);
	    static int8_t LTC681x_rdstat(uint8_t reg, //Determines which Stat  register is read back.
		    uint8_t total_ic,//the number of ICs in the system
		    Cell_asic ic[]);
	    static void LTC681x_rdstat_reg(uint8_t reg, //Determines which stat register is read back
		    uint8_t total_ic, //The number of ICs in the system
		    uint8_t *data //Array of the unparsed stat codes
		   );
	    static int16_t LTC6811_run_cell_adc_st(uint8_t adc_reg, uint8_t total_ic, Cell_asic ic[]);
	    static int16_t LTC681x_run_cell_adc_st(uint8_t adc_reg, uint8_t total_ic, Cell_asic ic[]);
	    static uint16_t LTC681x_st_lookup(uint8_t MD, uint8_t ST);
	    static void LTC681x_clrcell();
	    static void LTC681x_clraux();
	    static  void LTC681x_clrstat();

	    static  void LTC681x_statst(uint8_t MD, uint8_t ST);
	    static  void LTC6811_set_discharge(int Cell, uint8_t total_ic, Cell_asic ic[]);
	    static  void Clear_discharge(uint8_t total_ic, Cell_asic ic[]);
	    static  void LTC681x_diagn();
	    static  uint16_t LTC6811_run_adc_overlap(uint8_t total_ic, Cell_asic ic[]);
	    static  uint16_t LTC681x_run_adc_overlap(uint8_t total_ic, Cell_asic ic[]);
	    static  int16_t LTC6811_run_adc_redundancy_st(uint8_t adc_mode, uint8_t adc_reg, uint8_t total_ic, Cell_asic ic[]);
	    static  int16_t LTC681x_run_adc_redundancy_st(uint8_t adc_mode, uint8_t adc_reg, uint8_t total_ic, Cell_asic ic[]);
	    static  void LTC681x_stcomm(uint8_t len);
	    static  void LTC681x_wrcomm(uint8_t total_ic, Cell_asic ic[]);
	    static  int8_t LTC681x_rdcomm(uint8_t total_ic, Cell_asic ic[]);
	    static  void LTC681x_run_openwire(uint8_t total_ic, Cell_asic ic[]);
	    
	    static void LTC6811_Voltage();
	    static void Start_ADC_Measurement(); 
	    static void Read_Cell_Voltage();

	    static void LTC6811_temperature(uint8_t I2C_address, uint8_t D1);
	    static void LTC6811_temperature_test(uint8_t I2C_address, uint8_t D1);
	    static void VoltageTemperature_measurement_switch();


	    
	    static void LTC6811Process();
	    
	    
	    static void SwitchADG729_Channel(uint8_t I2C_address, uint8_t D1);
	    
	    static void MuxDecoder_Self_Test();
	    static void ADCOverlap_self_test();
	    static void LTC6811_i2c_commmand(uint8_t I2C_address, uint8_t D1);
	    static void LTC681x_heartbeat_commmand(uint8_t I2C_address);
	    static void Read_Status_registers();
	    static void Read_AUX_Voltage_Registers();
	    static void Enable_discharge_transistor(int Cell);
	    static void LTC681x_set_cfgr_dcto(uint8_t nIC, Cell_asic *ic, bool dcto[4]);
	    static void LTC681x_wrsctrl(uint8_t total_ic, // Number of ICs in the daisy chain
		    uint8_t sctrl_reg, // The Sctrl Register to be written A or B
		    Cell_asic *ic  // A two dimensional array that stores the data to be written
		   );
	    static void LTC681x_stsctrl();
	    static void Write_S_Control(bool_t Command_ON);
	    static void LTC681x_wrpwm(uint8_t total_ic, // Number of ICs in the daisy chain
		    uint8_t pwmReg, // The PWM Register to be written A or B
		    Cell_asic ic[] // A two dimensional array that stores the data to be written
		   );
	    static void Write_pwm_configuration(int pwmReg);
	    // static void write_i2c_communication();
	    static void Update_PACK_Voltage();
	    static void StartOverlapConversion();
	    
	    /* Helper function to initialize register limits. */
	    static void ADBMS1818_init_reg_limits(uint8_t total_ic, //Number of ICs in the system
		    Cell_asic *ic // A two dimensional array that will store the data
		    );
	    
	    /* Helper Function to reset PEC counters */
	    static void ADBMS1818_reset_crc_count(uint8_t total_ic, //Number of ICs in the system
		    Cell_asic *ic //A two dimensional array that stores the data
		    );
	    
	    /* Helper function to initialize CFG variables */
	    static void ADBMS1818_init_cfg(uint8_t total_ic, //Number of ICs in the system
		    Cell_asic *ic //A two dimensional array that stores the data
		    );
	    
	    /* Helper function to set CFGR variable */
	    static void ADBMS1818_set_cfgr(uint8_t nIC, // Current IC
		    Cell_asic *ic, // A two dimensional array that stores the data
		    bool refon, // The REFON bit
		    bool adcopt, // The ADCOPT bit
		    bool gpio[5], // The GPIO bits
		    bool dcc[12], // The DCC bits
		    bool dcto[4], // The Dcto bits
		    uint16_t uv, // The UV value
		    uint16_t  ov // The OV value
		    );
	    
	    /* Helper Function to set the configuration register B */ 
	    static void ADBMS1818_set_cfgrb(uint8_t nIC, Cell_asic *ic, bool fdrf, bool dtmen, bool ps[2], bool gpiobits[4], bool dccbits[7]);
	    
	    /* Helper Function to initialize the CFGRB data structures */
	    static void ADBMS1818_init_cfgb(uint8_t total_ic, Cell_asic *ic);
	    
	    /* Helper function to set the REFON bit */
	    static void ADBMS181x_set_cfgr_refon(uint8_t nIC, Cell_asic *ic, bool refon);
	    
	    /* Helper function to set the ADCOPT bit */
	    static void ADBMS181x_set_cfgr_adcopt(uint8_t nIC, Cell_asic *ic, bool adcopt);
	    
	    /* Helper function to set GPIO bits */
	    static void ADBMS181x_set_cfgr_gpio(uint8_t nIC, Cell_asic *ic, bool gpio[5]);
	    
	    /* Helper function to control discharge */
	    static void ADBMS181x_set_cfgr_dis(uint8_t nIC, Cell_asic *ic, bool dcc[12]);
	    
	    /* Helper function to control discharge time value */
	    static void ADBMS181x_set_cfgr_dcto(uint8_t nIC, Cell_asic *ic, bool dcto[4]);
	    
	    /* Helper Function to set UV value in CFG register */
	    static void ADBMS181x_set_cfgr_uv(uint8_t nIC, Cell_asic *ic, uint16_t uv);
	    
	    /* Helper function to set OV value in CFG register */
	    static void ADBMS181x_set_cfgr_ov(uint8_t nIC, Cell_asic *ic, uint16_t ov);
	    
	    /* Helper function to set the FDRF bit */
	    static void ADBMS1818_set_cfgrb_fdrf(uint8_t nIC, Cell_asic *ic, bool fdrf);
	    
	    /* Helper function to set the DTMEN bit */
	    static void ADBMS1818_set_cfgrb_dtmen(uint8_t nIC, Cell_asic *ic, bool dtmen);
	    
	    /* Helper function to set the PATH SELECT bit */
	    static void ADBMS1818_set_cfgrb_ps(uint8_t nIC, Cell_asic *ic, bool ps[]);
	    
	    /*  Helper function to set the gpio bits in configb b register  */
	    static void ADBMS1818_set_cfgrb_gpio_b(uint8_t nIC, Cell_asic *ic, bool gpiobits[]);
	    
	    /*  Helper function to set the dcc bits in configb b register */ 
	    static void ADBMS1818_set_cfgrb_dcc_b(uint8_t nIC, Cell_asic *ic, bool dccbits[]);
	    
	    /* Helper function that increments PEC counters */
	    static void ADBMS181x_check_pec(uint8_t total_ic, //Number of ICs in the system
		    uint8_t reg, //Type of Register
		    Cell_asic *ic //A two dimensional array that stores the data
		    );
	    
	    /* Write the ADBMS181x CFGRA */
	    static void ADBMS181x_wrcfg(uint8_t total_ic, //The number of ICs being written to
		    Cell_asic ic[]  // A two dimensional array of the configuration data that will be written
		   );
	    
	    /* Write the ADBMS181x CFGRB */
	    static void ADBMS181x_wrcfgb(uint8_t total_ic, //The number of ICs being written to
		    Cell_asic ic[] // A two dimensional array of the configuration data that will be written
		   );
	    
	    /* Reads configuration registers of a ADBMS1818 daisy chain */
	    static int8_t ADBMS1818_rdcfg(uint8_t total_ic, //Number of ICs in the system
		    Cell_asic *ic //A two dimensional array that the function stores the read configuration data.
		   );
	    
	 	    /* Reads configuration b registers of a ADBMS1818 daisy chain */
	    static int8_t ADBMS1818_rdcfgb(uint8_t total_ic, //Number of ICs in the system
		    Cell_asic *ic //A two dimensional array that the function stores the read configuration data.
		   );   

	    /* Read the ADBMS181x CFGA */
	    static int8_t ADBMS181x_rdcfg(uint8_t total_ic, //Number of ICs in the system
		    Cell_asic ic[] // A two dimensional array that the function stores the read configuration data.
		   );
	    
	    /* Reads the ADBMS181x CFGB */
	    static int8_t ADBMS181x_rdcfgb(uint8_t total_ic, //Number of ICs in the system
		    Cell_asic ic[] // A two dimensional array that the function stores the read configuration data.
		   );
	    
	 
	    
	    // Do nothing
	    ~LTC6811SPI() {
		    // Do nothing
	    }
        private:
	
    };

} //namespace bmstech
