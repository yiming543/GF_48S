//File
#include <mbed.h>
#include "SOC.h"
#include "PackConst.h"

#include "BatterySys.h"
#include "CY15B256J_I2C.h"

namespace bmstech
{ 
	const  uint16_t SOC_TAB[101] = {												//3.3257v=100% 	¬KÄõ100Ah
	2994U, 3055U, 3102U, 3139U, 3166U, 3176U, 3181U, 3183U, 3185U, 3186U, 3187U,     	//0~10
	3189U, 3193U, 3201U, 3208U, 3213U, 3218U, 3224U, 3229U, 3232U, 3236U,				//11~20
    3239U, 3242U, 3244U, 3247U, 3251U, 3254U, 3257U, 3260U, 3263U, 3266U,				//21~30
	3270U, 3273U, 3274U, 3275U, 3275U, 3275U, 3275U, 3276U, 3276U, 3276U,				//31~40
	3276U, 3276U, 3277U, 3277U, 3277U, 3277U, 3277U, 3278U, 3278U, 3278U,				//41~50
	3278U, 3278U, 3279U, 3279U, 3280U, 3280U, 3281U, 3282U, 3283U, 3285U,				//51~60
	3286U, 3290U, 3296U, 3309U, 3315U, 3316U, 3317U, 3317U, 3318U, 3318U,				//61~70
	3318U, 3319U, 3319U, 3319U, 3319U, 3319U, 3319U, 3320U, 3320U, 3320U,				//71~80
	3320U, 3320U, 3320U, 3320U, 3321U, 3321U, 3321U, 3321U, 3321U, 3322U,				//81~90
	3322U, 3322U, 3322U, 3322U, 3322U, 3323U, 3323U, 3324U, 3324U, 3325U  };			//91~100
		
	//soc_ocv
	uint8_t SOC::SOC_OCV(const uint16_t min_value)
	{ 
		uint8_t soc_persent = 0U; 	//init
//		unsigned long temp3 = 0U, temp4 = 0U;		//init
//		uint64_t temp3 = 0U;		//init
//		uint64_t temp4 = 0U;        //init

//		uint32_t total_soc = 190000U;     //106080       //2.6AH*0.85   = 2.21AH       2.21AH*48p = 106.08AH
//																				//2.6Ah*0.8=2.08Ah   2.08Ah*48p=99.84Ah
//		total_soc = total_soc * 3600U*100U;
		//SOC_Tab[0]
		if (min_value <= SOC_TAB[0])
		{
			soc_persent = 0U;
		}   //SOC_Tab[100]    
		else if (min_value >= SOC_TAB[100])
		{
			soc_persent = 100U;
		}     
		else
		{
			for (uint8_t s = 1U; s <= 100U; s++)//
			{
				if (min_value < SOC_TAB[s])
				{
					soc_persent = s - 1U;
					break;
				}
			}
		}
  
		if (soc_persent >= 100U)
		{
			soc_persent = 100U;
		}
		if (soc_persent <= 0U)
		{
			soc_persent = 0U;
		}
  
//		temp3 = soc_persent;
//		temp4 = total_soc / 100U;
//		temp3 = temp4*temp3;

		//  temp3 = (unsigned double long)(temp3 * total_soc);
		//  temp3 = (temp3 / 100);
		//  temp3 = ((unsigned long long)(total_soc * soc_persent)/(unsigned long long)100);
		//  static_soc = temp3;
        //  dynamic_soc = temp3;   //SOC to Capacity
  
		return (soc_persent);
	} 
		

	
	//==============================================================================
	// SOC correction Current limit Function
	// Discharge Current <= 10 A and continue 5 sec  , DCI  event occure
	// Charge  Current <= 10 A and continue 5 sec  , CCI  event occure
	//==============================================================================

	
	
	//==============================================================================
	// SOH Calsulate Function
	// SOH = FCC/DC *100%
	// Temp>10 SOC<10% and Charge
	// LMDupdate = LMD/(1-SOC)
	// if (LMDupdate<(FCC*0.95)) FCC=FCC*0.95
	// else FCC=LMDupdate
	//==============================================================================


	
	//==============================================================================
	// SOH Update Action Function
	// SOC < 10 % and MinVolt < 3000mV ,SOH Update
	// SOH = FCC / DC
	// SOC = RMC / FCC
	//==============================================================================
	void SOC::SOHUpdate_Action()
	{
	
		if (SOC::SOHupdate_Start_Flag == 1)
		{
		
			SOC::SOHupdate_Start_Flag = 0;
			SOC::FCCcoulombCount_Flag = 1;
		}
		
		if (SOC::FCCcoulombCount_Flag == 1) 
		{
			if ((BmsEngine::m_soc  < SOH_USOC) && (BatterySys::MinVolt < SOH_MIN_Volt))
			{
				SOC::SOHupdate_END_Flag = 1;
				SOC::FCCcoulombCount_Flag = 0;				
			}
		}
	
	}

} //namespace bmstech
