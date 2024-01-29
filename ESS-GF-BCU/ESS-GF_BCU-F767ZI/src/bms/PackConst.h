//File
#pragma once


namespace bmstech
{
#pragma region PackConst
	
//#define	Auto_MODE			//UL TEST

//Close RUN:// (DeBug Mode OPEN)
#define HALLSENSOR_CRC_TEST		//DeBug CRC Check  CRC Close

//#define DORUNTIMECHECKS		//DeBug CRC Check  RUN://
	
	
#define EMS_BCU_CANBUS      //BSU BCU CAN
#define HALLSENSOR_BCU_CANBUS	//HALL SENSOR CAN
#define BMU_BCU_ISOSPI	//isoSPI
#define BCU_TEMP_PROTECTION	//BCU_TEMP_PROTECTION
	
	class PackConst
	{
	public:
	//Pack:
		//
		static const uint16_t MAX_PACK_NUM = 4;        //Battery Bank battery pack number: 22 for eFerry, 12S48P for ESS 
//		static const uint16_t WAKEUP_IDLE_Volt_DELAY = 700; //600 300;  //38 pack
//		static const uint16_t WAKEUP_IDLE_Temp_DELAY = 900; //900 38 pack 20230508 
//		static const uint16_t WAKEUP_IDLE_Volt_DELAY = 300; //400 38 pack 20230605
//		static const uint16_t WAKEUP_IDLE_Temp_DELAY = 500; //700 38 pack 20230605
		static const uint16_t WAKEUP_IDLE_Volt_DELAY = 400; //38 pack 20230609
//		static const uint16_t WAKEUP_IDLE_Temp_DELAY = 700; //38 pack 20230609
//		static const uint16_t WAKEUP_IDLE_Temp_DELAY = 2100; //1 pack 20230609
		static const uint16_t WAKEUP_IDLE_Temp_DELAY = 300; //1 pack 20230609
		static const uint16_t WAKEUP_IDLE_DELAY = 700; //550;  //38 pack


		
		static const uint16_t MIN_PACK_NUM = 1; //Pack ID: from 1~21
		static const uint16_t MAX_CELL_NUM = 12; //10S 2023-08-15 JY
		static const uint16_t MAX_TEMP_NUM = 6;	//6 2023-09-21 JY
		static const uint8_t MAX_BALANCE_NUM = 12;		//12S 2023-09-21 JY
		static const uint16_t FUNC_ID_NUM = 21; //CM function id total numnber: 1,2,3,8,9, 0x40~0x4D
	};

#pragma endregion

#pragma region PacketConst
	/// <summary>
	/// PacketConst: bmsTech CM CAN Packet & Pack Information Definition
	/// </summary>
	class PacketConst
	{
	public:
		static const uint16_t CAN_PACKET_TIMEOUT = 10; //0.5 second tick: 10 (5) sec

													//CAN Packet:
		static const uint16_t CELL_VOLT_NUM = 3; // 4;          	//CAN Packet ID for cell voltage: 0~3 (toetal 4)
//		static const uint16_t CELL_TEMP_NUM = 10; //CAN Packet: cell temperature number (battery cell)
		static const uint16_t CELL_TEMP_NUM = 16; //CAN Packet: cell temperature number (battery cell)
		static const uint16_t BIC_TEMP_NUM = 4; //CAN Packet: BIC temperature number

		static const uint16_t BIC_TEMP_ID = 0;
		static const uint16_t BAT_TEMP_ID = 1;
		static const uint16_t CHKSUM_INDEX = 5;

		static const uint16_t MAX_PACK_ID = PackConst::MAX_PACK_NUM;
		static const uint16_t MIN_PACK_ID = 1;

		//MBU-BCU:
		static const uint16_t MAX_BCU_ID = 22; // JY 20220928 initial:25
		static const uint16_t MIN_BCU_ID = 1;
	};

#pragma endregion
}
