//File=======   2022-01   Truewin ================================
//Arthor:
//Date: 2022-06-20
//Application:  ESS
//Version:
//History:
#include <mbed.h>
#include "ADC_Temperature.h"
#include "BmsEngine.h"

extern AnalogIn temperature1;   //An analog input
extern AnalogIn temperature2;   //An analog input
extern AnalogIn temperature3;   //An analog input
extern AnalogIn temperature4;   //An analog input
extern AnalogIn temperature5;   //An analog input
extern AnalogIn temperature6;   //An analog input
extern AnalogIn temperature7;   //An analog input
extern AnalogIn temperature8;   //An analog input

namespace bmstech
{

    const uint16_t TEMPERATURE_TAB[146] = {                                 //NTSE0103FZ117HA  for 12bit
    3894U ,3883U ,3872U ,3860U ,3847U ,3835U ,3821U ,3807U ,3793U ,3778U ,3763U ,3747U ,3730U ,3712U ,3695U ,          //-40~-26
    3676U ,3657U ,3637U ,3616U ,3595U ,3573U ,3550U ,3527U ,3503U ,3478U ,3453U ,3427U ,3400U ,3373U ,3345U ,          //-25~-11
    3316U ,3287U ,3257U ,3226U ,3195U ,3164U ,3131U ,3099U ,3065U ,3032U ,2997U ,2963U ,2927U ,2892U ,2856U ,          //-10~4
    2819U ,2782U ,2745U ,2708U ,2670U ,2632U ,2593U ,2555U ,2516U ,2477U ,2438U ,2399U ,2360U ,2321U ,2281U ,          //5~19
    2242U ,2203U ,2164U ,2125U ,2086U ,2048U ,2009U ,1971U ,1933U ,1895U ,1858U ,1821U ,1784U ,1748U ,1712U ,          //20~34
    1677U ,1641U ,1607U ,1573U ,1539U ,1506U ,1473U ,1441U ,1409U ,1378U ,1347U ,1317U ,1287U ,1258U ,1230U ,          //35~49
    1202U ,1174U ,1147U ,1120U ,1095U ,1069U ,1044U ,1020U ,996U   ,972U  ,949U  ,927U  ,905U  ,884U  ,863U  ,          //50~64
     842U , 822U  ,803U  ,783U  ,765U  ,746U  ,729U  ,711U  ,694U  ,677U  ,661U  ,645U  ,630U  ,615U  ,600U  ,          //65~79
     586U , 572U  ,558U  ,545U  ,532U  ,519U  ,507U  ,495U  ,483U  ,471U  ,460U  ,449U  ,439U  ,428U  ,418U  ,          //80~94
     408U , 399U  ,390U  ,381U  ,372U  ,363U  ,355U  ,347U  ,339U  ,331U  ,323U   };                                //95~105

//  const uint16_t RT_Table[166] = {                                                                    //TTF3A103F34D1BY for 5000mV
//  4750, 4737, 4723, 4709, 4694, 4679, 4662, 4645, 4628, 4609, 4590, 4570, 4550, 4528, 4506,           //-40~-26
//  4484, 4460, 4435, 4410, 4384, 4358, 4330, 4302, 4272, 4242, 4212, 4180, 4148, 4115, 4081,           //-25~-11
//  4046, 4010, 3974, 3937, 3899, 3861, 3822, 3782, 3741, 3700, 3659, 3616, 3573, 3530, 3486,           //-10~4
//  3441, 3396, 3351, 3305, 3259, 3213, 3166, 3119, 3071, 3024, 2976, 2929, 2881, 2833, 2785,           //5~19
//  2737, 2690, 2642, 2594, 2547, 2500, 2453, 2407, 2360, 2314, 2269, 2223, 2179, 2134, 2090,           //20~34
//  2047, 2004, 1962, 1920, 1879, 1838, 1798, 1759, 1720, 1682, 1644, 1607, 1571, 1535, 1500,           //35~49
//  1466, 1432, 1399, 1366, 1334, 1303, 1273, 1243, 1214, 1185, 1157, 1130, 1103, 1077, 1051,           //50~64
//  1026, 1002,  978,  955,  932,  910,  888,  867,  846,  826,  806,  787,  768,  750,  732,           //65~79
//   714,  698,  681,  665,  649,  634,  619,  604,  590,  576,  562,  549,  536,  524,  512,           //80~94
//   500,  488,  477,  466,  455,  445,  434,  424,  415,  405,  396,  387,  378,  370,  361,           //95~109
//   353,  345,  338,  330,  323,  316,  309,  302,  295,  289,  282,  276,  270,  264,  259, 253 };    //110~125                           
    
//  const uint16_t RT_Table[166] = {                                                                    //TTF3A103F34D1BY for 4955mV
//      4707, 4694, 4681, 4667, 4652, 4637, 4620, 4604, 4586, 4568, 4549, 4529, 4509, 4488, 4466,       //-40~-26
//      4443, 4420, 4396, 4371, 4345, 4318, 4291, 4263, 4234, 4204, 4174, 4142, 4110, 4077, 4044,       //-25~-11       
//      4009, 3974, 3938, 3902, 3864, 3826, 3787, 3748, 3708, 3667, 3626, 3584, 3541, 3498, 3454,       //-10~4 
//      3410, 3366, 3321, 3275, 3230, 3184, 3137, 3091, 3044, 2997, 2950, 2902, 2855, 2808, 2760,       //5~19  
//      2713, 2665, 2618, 2571, 2524, 2478, 2431, 2385, 2339, 2293, 2248, 2203, 2159, 2115, 2072,       //20~34     
//      2029, 1986, 1944, 1903, 1862, 1822, 1782, 1743, 1704, 1666, 1629, 1593, 1557, 1521, 1486,       //35~49         
//      1452, 1419, 1386, 1354, 1322, 1292, 1261, 1232, 1203, 1174, 1147, 1120, 1093, 1067, 1042,       //50~64     
//      1017,  993,  969,  946,  923,  901,  880,  859,  838,  818,  799,  780,  761,  743,  725,       //65~79     
//       708,  691,  675,  659,  643,  628,  613,  599,  585,  571,  557,  544,  532,  519,  507,       //80~94         
//       495,  484,  473,  462,  451,  441,  430,  421,  411,  402,  392,  383,  375,  366,  358,       //95~109           
//       350,  342,  335,  327,  320,  313,  306,  299,  292,  286,  280,  274,  268,  262,  256, 251 };    //110~125 

//  const uint16_t RT_Table[166] = {                                                                    //TTF3A103F34D1BY for 2999mV
//      2849, 2841, 2833, 2825, 2816, 2806, 2796, 2786, 2776, 2765, 2753, 2741, 2729, 2716, 2703,       //-40~-26
//      2689, 2675, 2660, 2645, 2630, 2614, 2597, 2580, 2563, 2545, 2526, 2507, 2488, 2468, 2448,       //-25~-11
//      2427, 2405, 2384, 2361, 2339, 2316, 2292, 2268, 2244, 2219, 2194, 2169, 2143, 2117, 2091,       //-10~4
//      2064, 2037, 2010, 1982, 1955, 1927, 1899, 1871, 1842, 1814, 1785, 1757, 1728, 1699, 1671,       //5~19
//      1642, 1613, 1585, 1556, 1528, 1500, 1471, 1443, 1416, 1388, 1361, 1334, 1307, 1280, 1254,       //20~34
//      1228, 1202, 1177, 1152, 1127, 1103, 1079, 1055, 1032, 1009,  986,  964,  942,  921,  900,       //35~49
//       879,  859,  839,  820,  800,  782,  763,  746,  728,  711,  694,  678,  662,  646,  630,       //50~64
//       615,  601,  587,  573,  559,  546,  533,  520,  507,  495,  483,  472,  461,  450,  439,       //65~79
//       429,  418,  408,  399,  389,  380,  371,  362,  354,  345,  337,  329,  322,  314,  307,       //80~94
//       300,  293,  286,  279,  273,  267,  261,  255,  249,  243,  238,  232,  227,  222,  217,       //95~109
//       212,  207,  202,  198,  194,  189,  185,  181,  177,  173,  169,  166,  162,  159,  155, 152};    //110~125


        const uint16_t RT_Table_T1[166] = { //ADBMS 6815 voltage
        25551,25343,25128,24907,24680,24448,24207,23963,23710,23452,    //-40~-31
        23187,22918,22645,22363,22078,21788,21493,21195,20892,20584,    //-30~-21
        20274,19960,19644,19325,19004,18681,18357,18030,17705,17377,    //-20~-11
        17050,16724,16397,16072,15747,15426,15104,14786,14471,14157,    //-10~-09
        13845,13538,13235,12934,12638,12343,12055,11771,11492,11218,    //  0~ 09
        10947,10683,10423,10164,9915 ,9671 ,9431 ,9197 ,8967 ,8742 ,    // 10~ 19
        8520 ,8309 ,8097 ,7892 ,7693 ,7500 ,7311 ,7127 ,6947 ,6773 ,    // 20~ 29
        6602 ,6437 ,6276 ,6119 ,5967 ,5819 ,5676 ,5536 ,5401 ,5269 ,    // 30~ 39
        5142 ,5018 ,4897 ,4780 ,4667 ,4557 ,4451 ,4347 ,4247 ,4149 ,    // 40~ 49
        4055 ,3964 ,3875 ,3789 ,3706 ,3625 ,3547 ,3471 ,3398 ,3326 ,    // 50~ 59
        3257 ,3190 ,3125 ,3062 ,3001 ,2942 ,2885 ,2829 ,2775 ,2724 ,    // 60~ 69
        2672 ,2624 ,2576 ,2530 ,2485 ,2442 ,2400 ,2359 ,2319 ,2281 ,    // 70~ 79
        2244 ,2208 ,2173 ,2139 ,2106 ,2074 ,2043 ,2013 ,1984 ,1956 ,    // 80~ 89
        1928 ,1902 ,1876 ,1851 ,1826 ,1803 ,1780 ,1758 ,1736 ,1715 ,    // 90~ 99
        1694 ,1675 ,1655 ,1636 ,1618 ,1601 ,1583 ,1566 ,1550 ,1535 ,    //100~109
        1519 ,1504 ,1489 ,1475 ,1462 ,1448 ,1435 ,1423 ,1410 ,1398 ,    //110~119
        1386 ,1375 ,1364 ,1353 ,1343 ,1333 };                           //120~125


        const uint16_t RT_Table_T4[166] = { //ADBMS 6815 voltage
        28832,28771,28706,28638,28568,28495,28419,28340,28257,28171,    //-40~-31
        28081,27989,27893,27793,27690,27583,27472,27357,27238,27116,    //-30~-21
        26989,26859,26724,26585,26442,26295,26144,25988,25829,25665,    //-20~-11
        25497,25325,25149,24968,24784,24596,24403,24207,24008,23805,    //-10~-09
        23597,23387,23173,22956,22736,22512,22286,22057,21826,21593,    //  0~ 09
        21356,21119,20880,20635,20393,20151,19904,19660,19412,19163,    // 10~ 19
        18913,18667,18416,18166,17917,17672,17424,17179,16933,16689,    // 20~ 29
        16446,16205,15966,15727,15492,15257,15025,14796,14569,14344,    // 30~ 39
        14123,13903,13686,13472,13262,13053,12849,12646,12447,12251,    // 40~ 49
        12059,11870,11684,11501,11321,11145,10972,10802,10636,10472,    // 50~ 59
        10311,10155,10000,9849 ,9702 ,9558 ,9417 ,9278 ,9143 ,9012 ,    // 60~ 69
        8881 ,8756 ,8632 ,8510 ,8393 ,8277 ,8164 ,8055 ,7946 ,7842 ,    // 70~ 79
        7739 ,7640 ,7542 ,7447 ,7354 ,7262 ,7174 ,7088 ,7003 ,6923 ,    // 80~ 89
        6841 ,6764 ,6687 ,6613 ,6539 ,6470 ,6400 ,6334 ,6268 ,6205 ,    // 90~ 99
        6141 ,6081 ,6021 ,5964 ,5906 ,5853 ,5797 ,5746 ,5695 ,5646 ,    //100~109
        5597 ,5550 ,5503 ,5459 ,5414 ,5372 ,5330 ,5290 ,5250 ,5211 ,    //110~119
        5173 ,5136 ,5101 ,5066 ,5030 ,4998 };                           //120~125

//  int16_t ADCC::Volt_temperatureTab(const uint16_t result_TH)
//  {        
//      if (result_TH >= RT_Table[165])
//      {   
//          for (uint16_t i = 0; i <= 165; i++) //TEMPERATURE_TAB
//          {
//              if (result_TH > RT_Table[i])
//              {
//                  //Temporary storage
//                  const int16_t j = (10*(result_TH - RT_Table[i])) / (RT_Table[i - 1] - RT_Table[i]);
//                
//                  if (i == 0)
//                  {
//                      Temp_TH = -400;
//                  }
//                  else
//                  {
//                      Temp_TH = (i * 10) - j - 400;
//                  }
//                  //                i = 166;
//                  break;  
//              }
//          }
//      }
//      else
//      {
//          Temp_TH = 1250;
//      }       
//
//      return Temp_TH;
//      
//  }
    
    
    
    
    

    //NTC Table
    int16_t ADCC::Adc_temperatureTab(const uint16_t result_TH)
    {        
        if (result_TH >= TEMPERATURE_TAB[145])
        {   
            for (int16_t i = 0; i <= 145; i++)  //TEMPERATURE_TAB
            {
                if (result_TH > TEMPERATURE_TAB[i])
                {
                    //Temporary storage
                    const int16_t j = (10*(result_TH - TEMPERATURE_TAB[i])) / (TEMPERATURE_TAB[i - 1] - TEMPERATURE_TAB[i]);
                
                    if (i == 0)
                    {
                        Temp_TH = -400;
                    }
                    else
                    {
                        Temp_TH = (i * 10) - j - 400;
                    }
                    //                i = 166;
                    break;  
                }
            }
        }
        else
        {
            Temp_TH = 1050;
        }       

        return Temp_TH;
        
    }
    
    int16_t ADCC::Volt_temperatureTab_T1(const uint16_t result_TH)
    {
        int16_t Temp_TH = 0;
        uint16_t low, mid, high;
        low = 0;
        uint16_t TEMP_TAB_SIZE = 166;
        
        high = (TEMP_TAB_SIZE - 1);
        mid = (low + high) / 2;
        if (result_TH >= RT_Table_T1[high])
        {
            if (result_TH <= RT_Table_T1[low])
            {
                if (result_TH >= RT_Table_T1[mid])
                {
                    //low ~ mid
                    high = mid;
                }
                else
                {
                    //mid ~ high
                    low = mid;
                }
             
                for (uint16_t i = low; i <= high; i++)
                { 
                    if (result_TH > RT_Table_T1[i])
                    {
                        uint16_t j = (uint16_t)(10 * (result_TH - RT_Table_T1[i]))
                            / (RT_Table_T1[i - 1] - RT_Table_T1[i]);
                        Temp_TH = (int16_t)((i * 10) - j - 400);
                        break;
                    }
                }
            }
            else
            {
                Temp_TH = -400;
            }
        }
        else
        {
            Temp_TH = 1250;
        }
        return Temp_TH;
    }
	
	int16_t ADCC::Volt_temperatureTab_T4(const uint16_t result_TH)
	{
		int16_t Temp_TH = 0;
		uint16_t low, mid, high;
		low = 0;
		uint16_t TEMP_TAB_SIZE = 166;
        
		high = (TEMP_TAB_SIZE - 1);
		mid = (low + high) / 2;
		if (result_TH >= RT_Table_T4[high])
		{
			if (result_TH <= RT_Table_T4[low])
			{
				if (result_TH >= RT_Table_T4[mid])
				{
					//low ~ mid
					high = mid;
				}
				else
				{
					//mid ~ high
					low = mid;
				}
             
				for (uint16_t i = low; i <= high; i++)
				{ 
					if (result_TH > RT_Table_T4[i])
					{
						uint16_t j = (uint16_t)(10 * (result_TH - RT_Table_T4[i]))
						    / (RT_Table_T4[i - 1] - RT_Table_T4[i]);
						Temp_TH = (int16_t)((i * 10) - j - 400);
						break;
					}
				}
			}
			else
			{
				Temp_TH = -400;
			}
		}
		else
		{
			Temp_TH = 1250;
		}
		return Temp_TH;
	}
    
    //Temperature_measurement
    void ADCC::Temperature_measurement()
    {
        if (ADCC::ADC_T1000ms_Flag == 1)
        {
            result_Temp_SUM = 0;
            for (uint8_t i = 0; i < AVERAGE_NUMBER; i++)    //Average_number
            {
                ADCC::result_TH[i] = static_cast<uint16_t>(temperature1.read_u16() >> 4); //CH1 BCU PCS+ Temperature
                result_Temp_SUM += ADCC::result_TH[i];
            }
            ADCC::Max_Temp_sort = Maximum(ADCC::result_TH);
            ADCC::Min_Temp_sort = Minimum(ADCC::result_TH);
        
            Tempchannels[0] = Adc_temperatureTab((result_Temp_SUM - Max_Temp_sort - Min_Temp_sort) >> 4);


            if (BmsEngine::m_bcuState != BCUStateType_t::BCU_INIT) //BCU_INT        
            {
                if (Tempchannels[0] > Temp_previous[0])
                {
                    if ((Tempchannels[0] - Temp_previous[0]) >= TEMP_RISE)
                    {
                        //                  Temp_abnormal[0] = 1;
                        TempCount[0]++;
                        if (TempCount[0] == TEMP_COUNT)
                        {
                            Temp_abnormal[0] = 1;
                            TempCount[0] = 0;
                        }                   
                    }   
                    else
                    {
                        TempCount[0] = 0;                   
                    }
                }
            }
            Temp_previous[0] = Tempchannels[0];
        
            result_Temp_SUM = 0U;
            for (uint8_t i = 0; i < AVERAGE_NUMBER; i++)    //Average_number
            {
                ADCC::result_TH[i] = temperature2.read_u16() >> 4; //CH2    BCU PCS- Temperature
                result_Temp_SUM += ADCC::result_TH[i];
            }
            ADCC::Max_Temp_sort = Maximum(ADCC::result_TH);
            ADCC::Min_Temp_sort = Minimum(ADCC::result_TH);
            Tempchannels[1] = Adc_temperatureTab((result_Temp_SUM - Max_Temp_sort - Min_Temp_sort) >> 4);
        
            if (BmsEngine::m_bcuState != BCUStateType_t::BCU_INIT) //BCU_INT        
            {
                if (Tempchannels[1] > Temp_previous[1])
                {
                    if ((Tempchannels[1] - Temp_previous[1]) >= TEMP_RISE)
                    {
                        //                  Temp_abnormal[0] = 1;
                        TempCount[1]++;
                        if (TempCount[1] == TEMP_COUNT)
                        {
                            Temp_abnormal[1] = 1;
                            TempCount[1] = 0;
                        }                   
                    }   
                    else
                    {
                        TempCount[1] = 0;                   
                    }
                }
            }
            Temp_previous[1] = Tempchannels[1];
        
            result_Temp_SUM = 0;
            for (uint8_t i = 0; i < AVERAGE_NUMBER; i++)    //Average_number
            {
                ADCC::result_TH[i] = temperature3.read_u16() >> 4; //CH3    BCU HVDC+ Temperature
                result_Temp_SUM += ADCC::result_TH[i];
            }
            ADCC::Max_Temp_sort = Maximum(ADCC::result_TH);
            ADCC::Min_Temp_sort = Minimum(ADCC::result_TH);
            Tempchannels[2] = Adc_temperatureTab((result_Temp_SUM - Max_Temp_sort - Min_Temp_sort) >> 4);
        
            if (BmsEngine::m_bcuState != BCUStateType_t::BCU_INIT) //BCU_INT        
            {
                if (Tempchannels[2] > Temp_previous[2])
                {
                    if ((Tempchannels[2] - Temp_previous[2]) >= TEMP_RISE)
                    {
                        //                  Temp_abnormal[0] = 1;
                        TempCount[2]++;
                        if (TempCount[2] == TEMP_COUNT)
                        {
                            Temp_abnormal[2] = 1;
                            TempCount[2] = 0;
                        }                   
                    }   
                    else
                    {
                        TempCount[2] = 0;                   
                    }
                }
            }
            Temp_previous[2] = Tempchannels[2];
        
            result_Temp_SUM = 0;
            for (uint8_t i = 0; i < AVERAGE_NUMBER; i++)    //Average_number
            {
                ADCC::result_TH[i] = temperature4.read_u16() >> 4; //CH4    BCU HVDC- Temperature
                result_Temp_SUM += ADCC::result_TH[i];
            }
            ADCC::Max_Temp_sort = Maximum(ADCC::result_TH);
            ADCC::Min_Temp_sort = Minimum(ADCC::result_TH);
            Tempchannels[3] = Adc_temperatureTab((result_Temp_SUM - Max_Temp_sort - Min_Temp_sort) >> 4);
        
            if (BmsEngine::m_bcuState != BCUStateType_t::BCU_INIT) //BCU_INT        
            {
                if (Tempchannels[3] > Temp_previous[3])
                {
                    if ((Tempchannels[3] - Temp_previous[3]) >= TEMP_RISE)
                    {
                        //                  Temp_abnormal[0] = 1;
                        TempCount[3]++;
                        if (TempCount[3] == TEMP_COUNT)
                        {
                            Temp_abnormal[3] = 1;
                            TempCount[3] = 0;
                        }                   
                    }   
                    else
                    {
                        TempCount[3] = 0;                   
                    }
                }
            }
            Temp_previous[3] = Tempchannels[3];
        
            result_Temp_SUM = 0;
            for (uint8_t i = 0; i < AVERAGE_NUMBER; i++)    //Average_number
            {
                ADCC::result_TH[i] = temperature5.read_u16() >> 4; //CH5    BCU Fuse Temperature
                result_Temp_SUM += ADCC::result_TH[i];
            }
            ADCC::Max_Temp_sort = Maximum(ADCC::result_TH);
            ADCC::Min_Temp_sort = Minimum(ADCC::result_TH);
            Tempchannels[4] = Adc_temperatureTab((result_Temp_SUM - Max_Temp_sort - Min_Temp_sort) >> 4);
        
            if (BmsEngine::m_bcuState != BCUStateType_t::BCU_INIT) //BCU_INT        
            {
                if (Tempchannels[4] > Temp_previous[4])
                {
                    if ((Tempchannels[4] - Temp_previous[4]) >= TEMP_RISE)
                    {
                        //                  Temp_abnormal[0] = 1;
                        TempCount[4]++;
                        if (TempCount[4] == TEMP_COUNT)
                        {
                            Temp_abnormal[4] = 1;
                            TempCount[4] = 0;
                        }                   
                    }   
                    else
                    {
                        TempCount[4] = 0;                   
                    }
                }
            }
            Temp_previous[4] = Tempchannels[4];
        
            result_Temp_SUM = 0;
            for (uint8_t i = 0; i < AVERAGE_NUMBER; i++)    //Average_number
            {
                ADCC::result_TH[i] = temperature6.read_u16() >> 4; //CH6    BCU TA Temperature
                result_Temp_SUM += ADCC::result_TH[i];
            }
            ADCC::Max_Temp_sort = Maximum(ADCC::result_TH);
            ADCC::Min_Temp_sort = Minimum(ADCC::result_TH);
            Tempchannels[5] = Adc_temperatureTab((result_Temp_SUM - Max_Temp_sort - Min_Temp_sort) >> 4);
                
            result_Temp_SUM = 0;
            for (uint8_t i = 0; i < AVERAGE_NUMBER; i++)    //Average_number
            {
                ADCC::result_TH[i] = temperature7.read_u16() >> 4; //CH7
                result_Temp_SUM += ADCC::result_TH[i];
            }
            ADCC::Max_Temp_sort = Maximum(ADCC::result_TH);
            ADCC::Min_Temp_sort = Minimum(ADCC::result_TH);
            Tempchannels[6] = Adc_temperatureTab((result_Temp_SUM - Max_Temp_sort - Min_Temp_sort) >> 4);
        
            result_Temp_SUM = 0;
            for (uint8_t i = 0; i < AVERAGE_NUMBER; i++)    //Average_number
            {
                ADCC::result_TH[i] = temperature8.read_u16() >> 4; //CH8
                result_Temp_SUM += ADCC::result_TH[i];
            }
            ADCC::Max_Temp_sort = Maximum(ADCC::result_TH);
            ADCC::Min_Temp_sort = Minimum(ADCC::result_TH);
            Tempchannels[7] = Adc_temperatureTab((result_Temp_SUM - Max_Temp_sort - Min_Temp_sort) >> 4);
        
            for (uint8_t i = 0; i < 8; i++)     //Temporary storage
            {
                TempTxCH[i] = Tempchannels[i];
            }
            
            ADCC::ADC_T1000ms_Flag = 0;
        }
    }
    
    //Sort Maximum
    uint16_t ADCC::Maximum(uint16_t* temp)
    {
        uint16_t Max = temp[0]; //init
        for (uint8_t i = 0; i < AVERAGE_NUMBER; i++)    //Average_number
        {
            if (Max < temp[i])
            {
                Max = temp[i];
            }
        }
        return Max;
    }
    
    //Sort Minimum
    uint16_t ADCC::Minimum(uint16_t* temp)
    { 
        uint16_t Min = temp[0]; //init
        for (uint8_t i = 0U; i < static_cast<uint8_t>(AVERAGE_NUMBER); i++) //Average_number
        {
            if (Min > temp[i])
            {
                Min = temp[i];
            }
        }
        return Min;
    }
          
} //namespace bmstech
