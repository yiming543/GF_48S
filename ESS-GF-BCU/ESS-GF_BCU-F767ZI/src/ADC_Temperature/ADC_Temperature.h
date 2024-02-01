//File=======   2022-01   Truewin ================================
#pragma once
#include <mbed.h>


namespace bmstech
{

#define AVERAGE_NUMBER	(18)
#define TEMP_RISE (100)
#define TEMP_COUNT (3)
	
#define ADC_12bit 4095
#define ADC_Volt 4950

    class ADCC 
    {
    public:
	    static int16_t Temp_TH;
	    static uint16_t result_TH[AVERAGE_NUMBER];
	    static int16_t Tempchannels[8];
	    static int16_t TempTxCH[8];
	    static int16_t Temp_previous[8];
	    static bool_t Temp_abnormal[8];

	    static uint16_t TempCount[8];    
	    static uint32_t result_Temp_SUM;
	    static uint16_t Max_Temp_sort;
	    static uint16_t Min_Temp_sort;
	    
	    static bool_t ADC_T1000ms_Flag;
	    
	    static int16_t Adc_temperatureTab(const uint16_t result_TH);
	    static void Temperature_measurement();
	    
	    static int16_t Volt_temperatureTab_T1(const uint16_t result_TH);
	    static int16_t Volt_temperatureTab_T4(const uint16_t result_TH);


	    static uint16_t Maximum(uint16_t* temp);
	    
	    static uint16_t Minimum(uint16_t* temp);
	    // Do nothing
	    ~ADCC() {
		    // Do nothing
	    }
        private:

    };
	
} //namespace bmstech
