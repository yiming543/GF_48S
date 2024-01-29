//File
#include <mbed.h>
#include "BatterySys.h"
#include "BmsEngine.h"

#include "VersionInfo.h"

#include "Balance.h"
#include "CY15B256J_I2C.h"
#include "SOC.h"


namespace bmstech
{
//	BatterySys *BatterySys::pBatterySys = 0;

//	BatterySys::BatterySys():

//	BatterySys::BatterySys(CAN* can1, const int bdCan1, CAN* can2, const int bdCan2, BmsEngine* pBms, DS1302* pRtc)
//		: p_canCm(can1)
//		, m_baudCm(bdCan1)
//		, p_canMbu(can2)
//		, m_baudMbu(bdCan2)
//		, p_bmsParam(pBms)
//		, p_rtc(pRtc)
//	{
//		p_canCm->reset();                                             //DO NOT reset() 2017/10/17
//		p_canMbu->reset();
//
//		p_canCm->attach(&cm_canMessageParse, CAN::RxIrq);
//		p_canMbu->attach(&mbu_canMessageParse, CAN::RxIrq);
//		p_canCm->frequency(m_baudCm);
//		p_canMbu->frequency(m_baudMbu);
//	}

#if 0
	BatterySys::BatterySys(CAN* can1, const int bdCan1, CAN* can2, const int bdCan2, BmsEngine* pBms)
		: p_canCm(can1)
		, m_baudCm(bdCan1)
		, p_canMbu(can2)
		, m_baudMbu(bdCan2)
		, p_bmsParam(pBms)
	{
		__disable_irq();

		//m_totalVolt
		//		canCurrError.canError_Chksum

		p_canMbu->reset();
		p_canMbu->frequency(m_baudMbu);
		//p_canMbu->attach(&mbu_canMessageParse, CAN::RxIrq);
		p_canMbu->attach(pBattSys->mbu_canMessageParse, CAN::RxIrq);

		wait_ms(100);                                             //100ms delay OK (2019/4/10 50ms is not so good)

		p_canCm->reset();          //reset()

		p_canCm->frequency(m_baudCm);
		//Hook-up CAN Rx ISR to static message parser
		p_canCm->attach(pBattSys->cm_canMessageParse, CAN::RxIrq);


		wait_ms(200);
		//wait_ms(10); //wait => hang up

		__enable_irq();
		//wait_ms(500);
	}
#endif
	BatterySys::BatterySys(CAN* can1, const int bdCan1, CAN* can2, const int bdCan2, BmsEngine* pBms)
		: p_canCm(can1)
		, m_baudCm(bdCan1)
		, p_canMbu(can2)
		, m_baudMbu(bdCan2)
		, p_bmsParam(pBms)
	{
		//2019/4/24: move setting code to init() function which is called in main();
		//============================================================================
		//__disable_irq();

		//m_totalVolt
		//		canCurrError.canError_Chksum
		//p_canCm->reset();    //reset()

		//p_canCm->frequency(m_baudCm);
		//p_canCm->attach(pBattSys->cm_canMessageParse, CAN::RxIrq);      //Hook-up CAN Rx ISR to static message parser

		//wait_ms(100);  //100ms delay OK (2019/4/10 50ms is not so good)

		//p_canMbu->reset();
		//p_canMbu->frequency(m_baudMbu);

		//p_canMbu->attach(pBattSys->mbu_canMessageParse, CAN::RxIrq);
		//wait_ms(10); //wait => hang up

		//wait_ms(100);
		//__enable_irq();
		//wait_ms(500);
		//wait_ms(100);
	}
	//HAL_CAN_DISABLE_IT
	BatterySys::~BatterySys()
	{
	}


	void BatterySys::Init() const
	{
		__disable_irq();


		p_canMbu->reset();
		p_canCm->reset();

		p_canMbu->frequency(m_baudMbu);
		p_canCm->frequency(m_baudCm);
		p_canMbu->mode(CAN::Normal);
		p_canCm->mode(CAN::Normal);
		wait_ms(20);
		p_canMbu->attach(pBattSys->Mbu_canMessageParse, CAN::RxIrq);
		//Hook-up CAN Rx ISR to static message parser
		p_canCm->attach(pBattSys->BMU_CANMessageParse, CAN::RxIrq);

		wait_ms(100);

		__enable_irq();
	}

	//CM Can bus:
	//================================================================
	//Receiveing CM CAN Bus messages (ISR)
	//================================================================
#if 1
	void BatterySys::BMU_CANMessageParse(void)
	{
		//canCmCounter++;
		//pBattSys->canCmError.canTotalCount++;

		pBattSys->p_canCm->read(pBattSys->canCmRxMsg);	//read can message to clear Rx interrupt

		uint32_t pid = 0;//
		uint16_t address = 0;//
		uint16_t maddress = 0;//

		int16_t temp[6];//
		int32_t curr = 0;//
		int32_t tempAmp = 0;//
		int i = 0;//

		//this->canCmRxMsg.id

		//0x000003C2: Honeywell CSNV500M Hall current sensor 
		if ((pBattSys->canCmRxMsg.id & 0x00000FFF) == 0x3C2)
		{
			//canCurrCounter++;
			pBattSys->canCurrError.canTotalCount++;

			//Ip value: 80000000h=0mA  7FFFFFFFh=-1mA  80000001h=1mA, bit0~bit31 (big-endian)
			//CRC8: bit56~bit63 (8 bits)
			//TODO: Add Hall sensor can messgae parser here:
			//
			curr = pBattSys->canCmRxMsg.data[0];
			curr <<= 8;
			curr |= pBattSys->canCmRxMsg.data[1];
			curr <<= 8;
			curr |= pBattSys->canCmRxMsg.data[2];
			curr <<= 8;
			curr |= pBattSys->canCmRxMsg.data[3];

			rawCurrent = curr;

#if defined (HALLSENSOR_CRC_TEST)
			uint8_t crc8 = pBattSys->canCmRxMsg.data[7];//TEST
#else
			uint8_t crc8 = get_CRC8_Poly_0X07((uint8_t*)pBattSys->canCmRxMsg.data);	
#endif //(BCU_DeBugMode)	
			
			//pBattSys->canMbuRxMsg.data[7];
			if (crc8 != pBattSys->canCmRxMsg.data[7]) //CRC8 mismatch
			{
				pBattSys->canCurrError.canError_Chksum++;
			}
			else //CRC8 Matched
			{
				canCurrCounter++;
				BatterySys::CurrCountFlag = 1;
			
				//Moving average
				for (i = 0 ; i < (MV_NUMBER - 1) ; i++) //c[15]<-c[14], c[14]<-c[13]...,c[1]<-c[0]
				{
					current[(MV_NUMBER - 1) - i] = current[(MV_NUMBER - 1) - (i + 1)];
				}

				//convert to 2's complement: reverse MSB (32-bit)
				//===============================================
				if (curr & 0x80000000) //Positive current
				{
					tempAmp = (curr & 0x7FFFFFFF);                                                 // truncate MSB
				}
				else //negative current
				{
					tempAmp = (curr | 0x80000000);
				}

				//check amo range: rated 500A => +-600A (unit:1mA)
				if ((tempAmp <= MAX_CURRENT) && (tempAmp >= MIN_CURRENT))
				{
					current[0] = tempAmp;
					rawCurrent = current[0];			
				
					totalCurrent = 0;
					for ( i = 0; i < MV_NUMBER; i++)
					{
						current[i + 1] = current[i];
						totalCurrent += current[i];
					}				
					totalCurrent >>= MV_AVG;                                               //averaging	
					
					//(pBattSys->p_bmsParam)->ampReading_mA =  totalCurrent;
					BmsEngine::ampReading_mA = totalCurrent;
//					BatterySys::m_ampReading =  totalCurrent / 100;    //unit: 100mA (0.1A)  
					int16_t m_ampread =  totalCurrent / 100;     //unit: 100mA (0.1A)  
			
//					BatterySys::m_ampReading = m_ampread;  	
					//~����
//					if(((pBattSys->p_bmsParam)->ampReading_mA & 0x80000000) != 0)                    //Discharge Mode
					if ((BmsEngine::ampReading_mA & 0x80000000) != 0)                    //Discharge Mode
					{
						BatterySys::m_ampReading = (0xFFFFFFFF - m_ampread) + 1;  		//32-bit 2� Complement Value
					}
					else if ((BmsEngine::ampReading_mA & 0x80000000) == 0)                   //charge Mode
					{
						BatterySys::m_ampReading = (m_ampread  ^ 0xFFFFFFFF) + 1; 
					}	
	
				} // in range
				else
				{
					pBattSys->canCurrError.sensor_OverRange++;
				}
			} //CRC8

			return;                                             //end of hall sensor message parse
		}

		//CM Message Parse:
		canCmCounter++;
		pBattSys->canCmError.canTotalCount++;			
		
		
		//0x0A00003x: Temperature message
//		if((pBattSys->canCmRxMsg.id & 0x0F0000F0) == 0x0A000030)
//		{
//			pid = (static_cast<uint32_t>(pBattSys->canCmRxMsg.id) & static_cast<uint32_t>(0x0000ff00)) >> 8;
//			//check valid ID:
//			if((pid >= PacketConst::MIN_PACK_ID) && (pid <= PacketConst::MAX_PACK_ID))
//			{
//				uint8_t idx = (pBattSys->canCmRxMsg.id & 0x0000000F);
//				int16_t cellTemp1 = static_cast<int16_t>(pBattSys->canCmRxMsg.data[0])
//									| static_cast<int16_t>((pBattSys->canCmRxMsg.data[1]&0x0F) << 8);
//				int16_t cellTemp2 = static_cast<int16_t>((pBattSys->canCmRxMsg.data[1]>>4)&0x0F)
//									| static_cast<int16_t>((pBattSys->canCmRxMsg.data[2]) <<4);
//				int16_t cellTemp3 = static_cast<int16_t>(pBattSys->canCmRxMsg.data[3])
//									| static_cast<int16_t>((pBattSys->canCmRxMsg.data[4]&0x0F) << 8);
//				int16_t cellTemp4 = static_cast<int16_t>((pBattSys->canCmRxMsg.data[4]>>4)&0x0F)
//									| static_cast<int16_t>((pBattSys->canCmRxMsg.data[5]) <<4);
//				
//				#if defined(USING_SAE_J1850_CRC8)
//				
//				#if defined (BCU_DeBugMode)
//					uint8_t crc8 = pBattSys->canCmRxMsg.data[7];//TEST
//				#else
//					uint8_t crc8 = get_SAE_CRC8((uint8_t*)pBattSys->canCmRxMsg.data, 7);
//				#endif //(BCU_DeBugMode)	
//				
//				////uint8_t crc8 = get_SAE_CRC8((uint8_t*)pBattSys->canCmRxMsg.data, 7);
//				////uint8_t crc8 = pBattSys->canCmRxMsg.data[7]; //TEST**************check CRC error
//				#endif
//				if (crc8 != pBattSys->canCmRxMsg.data[7]) //check CRC error
//				{
//						pBattSys->canCmError.canError_Chksum++;
//				}
//				else //CRC Check OK:
//				{
//						//valid packet:
//					pBattSys->m_canCmFlag = true;
//
//					//pBatterySys->pack[pid - 1].timeoutCount = 0; //reset timeCount
//					//pack[pid - 1].timeoutCount = 0; //reset timeCount
//					if(idx < 3)
//					{
//						pack[pid - 1].timeoutCount[idx] = 0;                                                //reset timeoutCount
//						pack[pid - 1].btimeout[idx] = false;
//						pack[pid - 1].bexist = true;
//
////						pack[pid - 1].cellTemp[idx * 4] = cellTemp1;                                               
////						pack[pid - 1].cellTemp[idx * 4 + 1] = cellTemp2;
////						pack[pid - 1].cellTemp[idx * 4 + 2] = cellTemp3;
////						pack[pid - 1].cellTemp[idx * 4 + 3] = cellTemp4;
//											
//						if ((cellTemp1 & 0x800) == 0)                    
//							pack[pid - 1].cellTemp[idx * 4] =  cellTemp1;   		
//						else if ((cellTemp1 & 0x800) != 0)                  
//							pack[pid - 1].cellTemp[idx * 4] = cellTemp1|0xF000;    		//12bit-->16bit 
//						
//						if ((cellTemp2 & 0x800) == 0)                    
//							pack[pid - 1].cellTemp[idx * 4 + 1] =  cellTemp2;   		
//						else if ((cellTemp2 & 0x800) != 0)                  
//							pack[pid - 1].cellTemp[idx * 4 + 1] = cellTemp2 | 0xF000;    		
//						
//						if ((cellTemp3 & 0x800) == 0)                    
//							pack[pid - 1].cellTemp[idx * 4 + 2] =  cellTemp3;   		
//						else if ((cellTemp3 & 0x800) != 0)                  
//							pack[pid - 1].cellTemp[idx * 4 + 2] = cellTemp3 | 0xF000;    	
//						
//						if ((cellTemp4 & 0x800) == 0)                    
//							pack[pid - 1].cellTemp[idx * 4 + 3] =  cellTemp4;   		
//						else if ((cellTemp4 & 0x800) != 0)                  
//							pack[pid - 1].cellTemp[idx * 4 + 3] = cellTemp4 | 0xF000;    	
//					
//					}
//					else if (idx ==0x0F)
//					{
//						pack[pid - 1].timeoutCount[idx] = 0;                                                 //reset timeoutCount
//						pack[pid - 1].btimeout[idx] = false;
//						pack[pid - 1].bexist = true;
//
//						pack[pid - 1].packTemp[0] = cellTemp1;                                               
//						pack[pid - 1].packTemp[1] = cellTemp2;
//						pack[pid - 1].packTemp[2] = cellTemp3;
//						pack[pid - 1].packTemp[3] = cellTemp4;
//					}
//					else pBattSys->canCmError.canError_Index++;
//				}
//			}
//			else pBattSys->canCmError.canError_Pid++;
//		}
//
//		
//		
//		//0x0A00002x: Cell voltage message
//		else if((pBattSys->canCmRxMsg.id & 0x0F0000F0) == 0x0A000020)
//		{
//			pid = (static_cast<uint32_t>(pBattSys->canCmRxMsg.id) & static_cast<uint32_t>(0x0000ff00)) >> 8;
//
//			//check valid ID:
//			if((pid >= PacketConst::MIN_PACK_ID) && (pid <= PacketConst::MAX_PACK_ID))
//			{
//				//uint8_t idx = pBattSys->canCmRxMsg.data[4]; //0/1/2/3: Cell Index
//				uint8_t idx = (pBattSys->canCmRxMsg.id & 0x0000000F);
//				uint16_t cellVolt1 = static_cast<uint16_t>(pBattSys->canCmRxMsg.data[0])
//										| static_cast<uint16_t>((pBattSys->canCmRxMsg.data[1]&0x1F) << 8); 	// |0x1F
//				uint16_t cellVolt2 = static_cast<uint16_t>(pBattSys->canCmRxMsg.data[2])
//										| static_cast<uint16_t>((pBattSys->canCmRxMsg.data[3]&0x1F) << 8);
//				uint16_t cellVolt3 = static_cast<uint16_t>(pBattSys->canCmRxMsg.data[4])
//										| static_cast<uint16_t>((pBattSys->canCmRxMsg.data[5]&0x1F) << 8);
//
//				// pBattSys->Status_balance1 = static_cast<uint16_t>(pBattSys->canCmRxMsg.data[1] & 0xC0) >>6
//				uint8_t Status_balance1 = static_cast<uint16_t>(pBattSys->canCmRxMsg.data[1] & 0xE0) >>5;
//				uint8_t Status_balance2 = static_cast<uint16_t>(pBattSys->canCmRxMsg.data[3] & 0xE0) >>5;
//				uint8_t Status_balance3 = static_cast<uint16_t>(pBattSys->canCmRxMsg.data[5] & 0xE0) >>5;
//
//#if defined(USING_SAE_J1850_CRC8)
//				
//				#if defined (BCU_DeBugMode)
//					uint8_t crc8 = pBattSys->canCmRxMsg.data[7]; //TEST
//				#else
//					uint8_t crc8 = get_SAE_CRC8((uint8_t*)pBattSys->canCmRxMsg.data, 7);
//				#endif //(BCU_DeBugMode)	
//				////uint8_t crc8 = get_SAE_CRC8((uint8_t*)pBattSys->canCmRxMsg.data, 7);
//				////uint8_t crc8 = pBattSys->canCmRxMsg.data[7]; //TEST**************check CRC error
//#else
//				dataBuf[7] = 0;                                              //check sum
//				for(int i = 0 ; i < CM_CHKSUM_INDEX ; i++) dataBuf[7] += dataBuf[i];
//#endif
//
//				if(crc8 != pBattSys->canCmRxMsg.data[7]) //check CRC error
//				{
//					pBattSys->canCmError.canError_Chksum++;
//				}
//				else //CRC OK:
//				{
//					//valid packet:
//					pBattSys->m_canCmFlag = true;
//
//					//pack[pid - 1].timeoutCount = 0; //reset timeoutCount
//					//pack[pid - 1].btimeout = false;
//
//					if(idx < 4)
//					{
//						pack[pid - 1].timeoutCount[idx] = 0;                                              //reset timeoutCount
//						pack[pid - 1].btimeout[idx] = false;
//						pack[pid - 1].bexist = true;
//
//						pack[pid - 1].cellVolt[idx * 3] = cellVolt1;                                              //unit:1mV
//						pack[pid - 1].cellVolt[idx * 3 + 1] = cellVolt2;
//						pack[pid - 1].cellVolt[idx * 3 + 2] = cellVolt3;
//
//						pack[pid - 1].cellBalance[idx * 3] = Status_balance1;
//						pack[pid - 1].cellBalance[idx * 3 + 1] = Status_balance2;
//						pack[pid - 1].cellBalance[idx * 3 + 2] = Status_balance3;
//
//						//Update Pack Total Voltage:
//						pack[pid - 1].packVolt = 0;                                               //scratchpad
//
//						for(int i = 0 ; i < PackConst::MAX_CELL_NUM ; i++)
//						{
//							pack[pid - 1].packVolt += pack[pid - 1].cellVolt[i];
//							
//						}
//						
//
//						pBattSys->m_sysVolt = 0;
//						for (int i = 0; i < PackConst::MAX_PACK_NUM; i++)
//						{
//							pBattSys->m_sysVolt += pack[i].packVolt;                                                //total Sum
//						}
//
//						BatterySys::m_voltReading = (pBattSys->m_sysVolt) / 100;   	     //convert to 0.1V
//						Balance::Average_voltReading = (pBattSys->m_sysVolt - MaxVolt - MinVolt)
//														/ ((PackConst::MAX_CELL_NUM)*(PackConst::MAX_PACK_NUM) - 2);
//						
//					}
//					else pBattSys->canCmError.canError_Index++;
//				}
//			}
//			else pBattSys->canCmError.canError_Pid++;
//		}
	
		
		//0x0A00006x: CAN_ERROR_CNT
//		else if ((pBattSys->canCmRxMsg.id & 0x0F0000F0) == 0x0A000060)
//		{
//			pid = (static_cast<uint32_t>(pBattSys->canCmRxMsg.id) & static_cast<uint32_t>(0x0000ff00)) >> 8;
//
//			//check valid ID:
//			if ((pid >= PacketConst::MIN_PACK_ID) && (pid <= PacketConst::MAX_PACK_ID))
//			{
//				//0/1/2/3/4: CAN_ERROR_Count Index
//				uint8_t idx = (pBattSys->canCmRxMsg.id & 0x0000000F);
//				uint32_t CAN_ERROR_Count1 = static_cast<int32_t>((pBattSys->canCmRxMsg.data[0]) << 16)
//											| static_cast<int32_t>((pBattSys->canCmRxMsg.data[1]) << 8)
//											| static_cast<int32_t>((pBattSys->canCmRxMsg.data[2]));
//				uint32_t CAN_ERROR_Count2 = static_cast<int32_t>((pBattSys->canCmRxMsg.data[4]) << 16)
//											| static_cast<int32_t>((pBattSys->canCmRxMsg.data[5]) << 8)
//											| static_cast<int32_t>((pBattSys->canCmRxMsg.data[6]));
//
//				#if defined(USING_SAE_J1850_CRC8)
//				////uint8_t crc8 = get_SAE_CRC8((uint8_t*)pBattSys->canCmRxMsg.data, 7);
//				uint8_t crc8 = pBattSys->canCmRxMsg.data[7]; //TEST**************check CRC error
//				#else
//				dataBuf[7] = 0;                                              //check sum
//				for(int i = 0 ; i < CM_CHKSUM_INDEX ; i++) dataBuf[7] += dataBuf[i];
//				#endif
//
//				if(crc8 != pBattSys->canCmRxMsg.data[7]) //check CRC error
//				{
//					pBattSys->canCmError.canError_Chksum++;
//				}
//				else //CRC OK:
//				{
//					//valid packet:
//					pBattSys->m_canCmFlag = true;
//
//					if (idx < 5)
//					{
//						pack[pid - 1].cellErrCNT[idx * 2] = CAN_ERROR_Count1;                                              
//						pack[pid - 1].cellErrCNT[idx *2 + 1] = CAN_ERROR_Count2;			
//					                                     
//					}
//					else pBattSys->canCmError.canError_Index++;
//				}
//			}
//			else pBattSys->canCmError.canError_Pid++;
//		}
		

		
		//0x0A000001: Bmu Version
//		else if((pBattSys->canCmRxMsg.id & 0x0F0000FF) == 0x0A000001)
//		{
//			pid = (static_cast<uint32_t>(pBattSys->canCmRxMsg.id) & static_cast<uint32_t>(0x0000ff00)) >> 8;
//			//check valid ID:
//			if((pid >= PacketConst::MIN_PACK_ID) && (pid <= PacketConst::MAX_PACK_ID))
//			{
//				uint16_t HW_version = static_cast<uint16_t>((pBattSys->canCmRxMsg.data[1]) << 8)
//											| static_cast<uint16_t>(pBattSys->canCmRxMsg.data[0]);
//				uint16_t SW_version = static_cast<uint16_t>((pBattSys->canCmRxMsg.data[3]) << 8)
//											| static_cast<uint16_t>(pBattSys->canCmRxMsg.data[2]);
//				uint16_t Protocol_version = static_cast<uint16_t>((pBattSys->canCmRxMsg.data[5]) << 8)
//											| static_cast<uint16_t>(pBattSys->canCmRxMsg.data[4]);
//				uint8_t CCS811_FW_version = (pBattSys->canCmRxMsg.data[6]);
//
//			#if defined(USING_SAE_J1850_CRC8)				
//				#if defined (BCU_DeBugMode)
//					uint8_t crc8 = pBattSys->canCmRxMsg.data[7];   //TEST
//				#else
//					uint8_t crc8 = get_SAE_CRC8((uint8_t*)pBattSys->canCmRxMsg.data, 7);
//				#endif //(BCU_DeBugMode)
//				
//			#else
//				dataBuf[7] = 0;                                                //check sum
//				for(int i = 0 ; i < CM_CHKSUM_INDEX ; i++) dataBuf[7] += dataBuf[i];
//			#endif
//
//				if(crc8 != pBattSys->canCmRxMsg.data[7]) //check CRC error
//				{
//					pBattSys->canCmError.canError_Chksum++;
//				}
//				else //CRC OK:
//				{
//					//valid packet:
//					//pBattSys->m_canCmFlag = true;
//					BatterySys::pack[pid - 1].HWVer = HW_version;  
//					BatterySys::pack[pid - 1].SWVer = SW_version;  
////					BatterySys::pack[pid - 1].ProtocolVer = Protocol_version;  
////					BatterySys::pack[pid - 1].HWCCS811FWVer = CCS811_FW_version;  
//				}
//			}
//			else pBattSys->canCmError.canError_Pid++;
//		}

	} //canMessageParse

#endif

	//return 0: means no timeout pack at all
//	int BatterySys::cm_canCheckTimeout(void)
//	{
//		int timeoutNum = 0;
//
//		for (int i = 0; i < PackConst::MAX_PACK_NUM; i++)
//		{
//			for (int j = 0; j < PacketConst::CELL_VOLT_NUM; j++)
//				if (pBattSys->pack[i].timeoutCount[j]) 
//					timeoutNum++;
//		}
//
//		return (timeoutNum);
//	}

	//CM Can bus commands (from BCU to CMs)
	//================================================================
	int BatterySys::BCU_CANStartAppCommand()
	{
		//Extended ID
		//ID field: (Slave_ID <<16) | 0x09 (CMD_ExcuteApp: 0x0800_3000)
		//Slave_ID=0: Broadcasting
		const char CMD[8] = { 0x08, 0x00, 0x30, 0x00, 0xFF, 0x00, 0xFF, 0x00 };//
		return (pBattSys->p_canCm->write(CANMessage(0x0009, CMD, 8, CANData, CANExtended)));
	}
	
	//setBCU Version Information
	void BatterySys::BCU_canVersion_Information()
	{
		char cmd[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };//
		uint32_t id = BCU_INFO_HW_FW_VERSION_BASE | ((pBattSys->p_bmsParam->m_bcuId) << 16);//

		cmd[0] = (char)(BCU_FW_VERSION_MAJOR);
		cmd[1] = (char)(BCU_FW_VERSION_MINOR);
		cmd[2] = (char)(BCU_FW_VERSION_PATCH);
		cmd[3] = (char)(BCU_HW_VERSION_MAJOR);
		cmd[4] = (char)(BCU_HW_VERSION_MINOR);
		cmd[5] = (char)(BCU_HW_VERSION_PATCH);
		cmd[6] = (0x00);

		//return (pBattSys->p_canMbu->write(CANMessage(0x0001, cmd, 8, CANData, CANExtended)));
#if defined(USING_SAE_J1850_CRC8)
		cmd[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(cmd), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0;
		for (int i = 0 ; i < BCU_STATUS_CHKSUM_INDEX ; i++)
		{
			dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i];
		}
#endif
		//
		int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, cmd, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
		if (canEmsRet == 0)	
		{
			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
			//while(true)
			{
				wait_us(600);
				//
				int canEmsRet = pBattSys->p_canMbu->write(CANMessage(id, cmd, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
				if (canEmsRet == 1)
					break;
			}
		}

	}

	//setBMU Version Information
	void BatterySys::BMU_Ver_CANCommand()
	{
		char cmd[8] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };//
		uint32_t id = BMU_INFO_HW_FW_VERSION_BASE | ((pBattSys->p_bmsParam->m_bcuId) << 16); //
		
		cmd[0] = (char)(0xFF);
		cmd[1] = (char)(0x00);
		cmd[2] = (char)(0x00);
		cmd[3] = (char)(0x00);
		cmd[4] = (char)(0x00);
		cmd[5] = (char)(0x00);
		cmd[6] = (char)(0x00);

#if defined(USING_SAE_J1850_CRC8)
		cmd[7] = Get_SAE_CRC8(reinterpret_cast<uint8_t *>(cmd), BCU_STATUS_CHKSUM_INDEX);
#else
		dataBuf[BCU_STATUS_CHKSUM_INDEX] = 0;
		for (int i = 0 ; i < BCU_STATUS_CHKSUM_INDEX ; i++)
		{
			dataBuf[BCU_STATUS_CHKSUM_INDEX] += dataBuf[i];
		}
#endif
		//
		int canCmsRet = pBattSys->p_canCm->write(CANMessage(id, cmd, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
		if (canCmsRet == 0)	
		{
			for (int i = 0; i < BCU_CAN_REPEAT_NUM; i++)//
			//while(true)
			{
				wait_us(600);
				//
				int canCmsRet = pBattSys->p_canCm->write(CANMessage(id, cmd, BCU_STATUS_PACKET_LENGTH, CANData, CANExtended));
				if (canCmsRet == 1)
					break;
			}
		}
	}
	






} //namespace 
