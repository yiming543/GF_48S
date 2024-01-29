//File
#include <mbed.h>
#include "Balance.h"
#include "BatterySys.h"


namespace bmstech
{

	//PassiveBalancejudge
	void Balance::PassiveBalancejudge()
	{
		if ((BmsEngine::m_bcuState == BCUStateType_t::BCU_STANDBY)\
		 || (BmsEngine::m_bcuState == BCUStateType_t::BCU_CHARGE)\
		 || (BmsEngine::m_bcuState == BCUStateType_t::BCU_FAULT)\
		 || (BmsEngine::m_bcuState == BCUStateType_t::BCU_DRIVE))
		{
			Balance::PassiveBalanceProcess(BatterySys::MaxVolt, BatterySys::MinVolt);	
			if (PassiveBalancejudge_flag == 1)
			{                               
//				if ((BatterySys::MaxVolt > PASSIVE_MINVOLT) && (BatterySys::MaxVolt < PASSIVE_MAXVOLT))
//				{
					if (Balance::Average_voltReading < PASSIVE_MINVOLT)		//20220912 JY
					{
						Balance::Average_voltReading = PASSIVE_MINVOLT;
					}
					
					if (Balance::Average_voltReading > ((BatterySys::MaxVolt + BatterySys::MinVolt) / 2))	//20220920 JY
					{
						Balance::BalanceVoltageSort((Balance::Average_voltReading - OFFSET_VOLT));
					}
					else
					{
						Balance::BalanceVoltageSort(Balance::Average_voltReading + OFFSET_VOLT);
					}
					
					PassiveBalanceCommand_ON_flag = 1;
//				}			
			}
			else
			{
				PassiveBalanceCommand_ON_flag = 0;
				PassiveBalancejudge_flag = 0;
				//MAX_PACK_NUM
				for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)
				{
					//MAX_CELL_NUM
					for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)
					{
						BatterySys::pack[x].cellBalance[y] = 0;
					}
				}
				
			}
		}
		else
		{
			PassiveBalanceCommand_ON_flag = 0;
			PassiveBalancejudge_flag = 0;
			//MAX_PACK_NUM
			for (uint16_t x = 0; x < PackConst::MAX_PACK_NUM; x++)
			{
				//MAX_CELL_NUM
				for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)
				{
					BatterySys::pack[x].cellBalance[y] = 0;
				}
			}
		}
	}
	
	//BalanceVoltageSort
	void Balance::BalanceVoltageSort(const uint16_t AVG_volt)
	{	
		//MAX_PACK_NUM
		for (uint16_t x = 0U; x < PackConst::MAX_PACK_NUM; x++)
		{
			//MAX_CELL_NUM
			for (uint16_t y = 0U; y < PackConst::MAX_CELL_NUM; y++)
			{
				if (BatterySys::pack[x].cellVolt[y] > AVG_volt)			//AVG
				{
					BatterySys::pack[x].cellBalance[y] = 1U;
				}
				else
				{
					BatterySys::pack[x].cellBalance[y] = 0U;
				}
				
			}
		}
		
			
	}

	//CellVolt_max
	uint16_t Balance::CellVolt_max(uint8_t packid)
	{	
		if (packid <= 0U)
		{
			packid = 1U;
		}
		uint16_t Max = BatterySys::pack[packid - 1U].cellVolt[0];	//init
		//MaxCell_num = 0;
		for (uint8_t s = 0U; s < PackConst::MAX_CELL_NUM; s++)
		{
			if (Max <= BatterySys::pack[packid - 1U].cellVolt[s])
			{
				Max = BatterySys::pack[packid - 1U].cellVolt[s];
				BatterySys::pack[packid - 1U].packMaxVolt = Max;
				BatterySys::pack[packid - 1U].packMaxVolt_NUM = s;
			}
		}
		return Max;
	}

	//CellVolt_max
//	uint16_t Balance::CellVolt_max(uint8_t packid, uint8_t mode)
//	{
//		uint16_t Max = BatterySys::pack[packid - 1].cellVolt[0];	//init
//		//MAX_CELL_NUM
//		for (uint8_t s = 0U; s < PackConst::MAX_CELL_NUM; s++)
//		{
//			if (Max <= BatterySys::pack[packid - 1].cellVolt[s])
//			{
//				Max = BatterySys::pack[packid - 1].cellVolt[s];
//				Cellnum = s;
//			}
//		}
//
//		if (mode == 1)
//		{
//			return Max;
//		}
//		else
//		{
//			return Cellnum;
//		}
//	}

	//PackVolt_max
	uint16_t Balance::PackVolt_max()
	{
		uint16_t maxv = BatterySys::pack[1].packMaxVolt;	//init
		uint16_t maxm = 0U;
		uint16_t maxc = 0U;	//init
		maxc = BatterySys::pack[0].packMaxVolt_NUM;
		for (uint16_t i = 0U; i < PackConst::MAX_PACK_NUM; i++)   //PackConst::MAX_PACK_NUM+1
		{
			if (maxv <= BatterySys::pack[i].packMaxVolt)
			{
				maxv = BatterySys::pack[i].packMaxVolt;
				maxm = i;
				maxc = BatterySys::pack[i].packMaxVolt_NUM;
			}
		}
		BatterySys::MaxVolt = maxv;		
		BatterySys::MaxVoltModule = maxm + 1U;
		BatterySys::MaxVoltCH = maxc + 1U;
		
		return (BatterySys::MaxVoltModule);
	}
	
	//CellVolt_min
	uint16_t Balance::CellVolt_min(uint8_t packid)
	{
		if (packid <= 0U)
		{
			packid = 1U;
		}
		uint16_t Min = BatterySys::pack[packid - 1U].cellVolt[0];	//init
		//MinCell_num = 0;
		for (uint8_t s = 0U; s < PackConst::MAX_CELL_NUM; s++)
		{
			if (Min >= BatterySys::pack[packid - 1U].cellVolt[s])
			{
				Min = BatterySys::pack[packid - 1U].cellVolt[s];
				BatterySys::pack[packid - 1U].packMinVolt = Min;
				BatterySys::pack[packid - 1U].packMinVolt_NUM = s;
				//MinCell_num = s;
			}
		}
		return Min;
	}

	//CellVolt_min
//	uint16_t Balance::CellVolt_min(uint8_t packid, uint8_t mode)
//	{
//		uint16_t Min = BatterySys::pack[packid - 1].cellVolt[0];	//init
//		//MAX_CELL_NUM
//		for (uint8_t s = 0U; s < PackConst::MAX_CELL_NUM; s++)
//		{
//			if (Min >= BatterySys::pack[packid - 1].cellVolt[s])
//			{
//				Min = BatterySys::pack[packid - 1].cellVolt[s];
//				Cellnum = s;
//			}
//		}
//		if (mode == 1)
//		{
//			return Min;
//		}
//		else
//		{
//			return Cellnum;
//		}
//	}

	//PackVolt_min
	uint16_t Balance::PackVolt_min()
	{
		uint16_t minv = BatterySys::pack[1].packMinVolt;	//init
		uint16_t minm = 0U;
		uint16_t minc = 0U;	//init
		minc = BatterySys::pack[0].packMinVolt_NUM;
		for (uint16_t i =0U; i < PackConst::MAX_PACK_NUM ; i++)		//PackConst::MAX_PACK_NUM+1
		{
			if (minv >= BatterySys::pack[i].packMinVolt)
			{
				minv = BatterySys::pack[i].packMinVolt;
				minm = i;
				minc = BatterySys::pack[i].packMinVolt_NUM;
			}
		}
		BatterySys::MinVolt = minv;		
		BatterySys::MinVoltModule = minm + 1U;
		BatterySys::MinVoltCH = minc + 1U;
		return (BatterySys::MinVoltModule);
	}
	
	//Temperature Max Check Function
	int16_t Balance::CellTemp_max(uint8_t packid)
	{
		if (packid <= 0U)
		{
			packid = 1U;
		}
		int16_t Max = BatterySys::pack[packid - 1U].cellTemp[0];	//init
		//MaxTemp_num = 0;
//		for (uint8_t s = 0U ; s < PackConst::MAX_CELL_NUM ; s++)
		for (uint8_t s = 0U; s < PackConst::MAX_TEMP_NUM; s++)	//20230921
		{
			if (Max <= BatterySys::pack[packid - 1U].cellTemp[s])
			{
				Max = BatterySys::pack[packid - 1U].cellTemp[s];
				//MaxTemp_num = s;
				BatterySys::pack[packid - 1U].packMaxTemp = Max;
				BatterySys::pack[packid - 1U].packMaxTemp_NUM = s;
		
			}
		}
		return Max;
	}
	
	//CellTemp_max
//	int16_t Balance::CellTemp_max(uint8_t packid, uint8_t mode)
//	{
//		int16_t Max = BatterySys::pack[packid - 1].cellTemp[0];	//init
//		//MAX_CELL_NUM
//		for (uint8_t s = 0U; s < PackConst::MAX_CELL_NUM; s++)
//		{
//			if (Max <= BatterySys::pack[packid - 1].cellTemp[s])
//			{
//				Max = BatterySys::pack[packid - 1].cellTemp[s];
//				Cellnum = s;
//			}
//		}
//		if (mode == 1)
//		{
//			return Max;
//		}
//		else
//		{
//			return Cellnum;
//		}
//	}

	//PackTemp_max
	uint16_t Balance::PackTemp_max()
	{
		int16_t maxt = BatterySys::pack[1].packMaxTemp;	//init
		uint16_t maxm = 0U;	//init
		uint16_t maxc = 0U;
		maxc = BatterySys::pack[0].packMaxTemp_NUM;
		for (uint16_t i = 0U; i < PackConst::MAX_PACK_NUM ; i++)   //PackConst::MAX_PACK_NUM+1
		{
			if (maxt <= BatterySys::pack[i].packMaxTemp)
			{
				maxt = BatterySys::pack[i].packMaxTemp;
				maxm = i;
				maxc = BatterySys::pack[i].packMaxTemp_NUM;
			}
		}
		BatterySys::MaxTemp = maxt;		
		BatterySys::MaxTempModule = maxm + 1U;
		BatterySys::MaxTempCH = maxc + 1U;
		return (BatterySys::MaxTempModule);
		
	}
	
	//CellTemp_min
	int16_t Balance::CellTemp_min(uint8_t packid)
	{
		if (packid <= 0U)
		{
			packid = 1U;
		}
		int16_t Min = BatterySys::pack[packid - 1U].cellTemp[0];	//init
		//MAX_CELL_NUM
		for (uint8_t s = 0U; s < PackConst::MAX_TEMP_NUM; s++)	//20230921
//			for (uint8_t s = 0U; s < PackConst::MAX_CELL_NUM; s++)
		{
			if (Min >= BatterySys::pack[packid - 1U].cellTemp[s])
			{
				Min = BatterySys::pack[packid - 1U].cellTemp[s];
				//Cellnum = s;
				BatterySys::pack[packid - 1U].packMinTemp = Min;
				BatterySys::pack[packid - 1U].packMinTemp_NUM = s;
			}
		}	
			return Min;
	}

	//CellTemp_min
//	int16_t Balance::CellTemp_min(uint8_t packid, uint8_t mode)
//	{
//		int16_t Min = BatterySys::pack[packid - 1].cellTemp[0];	//init
//		//MAX_CELL_NUM
//		for (uint16_t s = 0U; s < PackConst::MAX_CELL_NUM; s++)
//		{
//			if (Min >= BatterySys::pack[packid - 1].cellTemp[s])
//			{
//				Min = BatterySys::pack[packid - 1].cellTemp[s];
//				Cellnum = s;
//			}
//		}
//
//		if (mode == 1)
//		{
//			return Min;
//		}
//		else
//		{
//			return Cellnum;
//		}
//	}

	//PackTemp_min
	uint16_t Balance::PackTemp_min()
	{
		int16_t mint = BatterySys::pack[1].packMinTemp;	//init
		uint16_t minm = 0U;
		uint16_t minc = 0U;	//init
		minc = BatterySys::pack[0].packMinTemp_NUM;
		for (uint16_t i = 0U; i < PackConst::MAX_PACK_NUM ; i++)		//PackConst::MAX_PACK_NUM+1
		{
			if (mint >= BatterySys::pack[i].packMinTemp)
			{
				mint = BatterySys::pack[i].packMinTemp;
				minm = i;
				minc = BatterySys::pack[i].packMinTemp_NUM;
			}
		}
		BatterySys::MinTemp = mint;		
		BatterySys::MinTempModule = minm + 1U;
		BatterySys::MinTempCH = minc + 1U;
		return (BatterySys::MinTempModule);

	}

	//PassiveBalanceProcess
	void Balance::PassiveBalanceProcess(const uint16_t max_volt, const uint16_t min_volt)
	{
		if ((max_volt >= PASSIVE_MINVOLT) && (max_volt <= PASSIVE_MAXVOLT))
		{
			if ((max_volt - min_volt) >= PASSIVE_IV)
			{
				PassiveBalancejudge_flag = 1;				
			}
			else
			{
				PassiveBalancejudge_flag = 0;
			}
		}
		else
		{
			PassiveBalancejudge_flag = 0;
		}
	}
/*	void Balance::ActiveBalanceProcess(uint16_t max_volt, uint16_t min_volt, uint16_t avg_volt)
	{
		if ((min_volt >= Active_minVolt)&&(max_volt <= Active_maxVolt))
		{
			if ((max_volt - avg_volt) >= Active_IV)
			{
				ActiveBalanceDSG_flag = 1;						
			}
			else
			{
				ActiveBalanceDSG_flag = 0;
			}
			
			if ((avg_volt - min_volt) >= Active_IV)
			{
				ActiveBalanceCHG_flag = 1;						
			}
			else
			{
				ActiveBalanceCHG_flag = 0;
			}
		}
		else
		{
			ActiveBalanceDSG_flag = 0;
			ActiveBalanceCHG_flag = 0;
		}
	}*/
}

