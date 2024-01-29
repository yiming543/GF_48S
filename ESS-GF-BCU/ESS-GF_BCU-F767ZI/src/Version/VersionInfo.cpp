//File
#include <cstring>
#include <cstdlib>

#include "VersionInfo.h"
#include "SEGGER_RTT.h"

#include "BmsEngine.h"
          
#include "PackConst.h"
////#include "stm32fxx_STLparam.h"
//#if !defined(RTT_ENABLE)
//#define RTT_ENABLE 1
//#endif

uint32_t VersionInfo::mainThreadCount = 0;//
uint32_t VersionInfo::bcuThreadCount = 0;//
uint32_t VersionInfo::canTickSec = 0;//
uint32_t VersionInfo::packetErrRate = 0; // unit:PPM
//
const char* VersionInfo::bcuBoardName = BCU_BOARD_NAME;


//version info:
//==========================================================
static char versionString[] =
		"BCU_VERSION_MAJOR" "." "BCU_VERSION_MINOR" "." "BCU_VERSION_PATCH" "." "BCU_VERSION_BUILD";
static char timestamp[] = __DATE__ " " __TIME__;	//timestamp

string verStr;//
const char *versionPtr;	//versionPtr

//version2String
const char *Version2String(int major, int minor, int patch)
{
	verStr =  ( to_string(major) + "." + to_string(minor) + "." + to_string(patch) );
	return( verStr.c_str() );
	
	//versionPtr = verStr.c_str();
	//return (versionPtr);	
}


//#if defined(RTT_ENABLE)
//dumpVersionInfo
void DumpVersionInfo()
{
	SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
	SEGGER_RTT_WriteString(0, "\r\n==============================================\r\n");
	SEGGER_RTT_printf(0, "CPU SystemCoreClock is %d MHz\r\n", SystemCoreClock/1000000U);
	SEGGER_RTT_printf(0, "Compile Date & Time: %s \r\n", timestamp);
	SEGGER_RTT_printf(0, "BCU ID: %x \r\n", bmstech::BmsEngine::m_bcuId);
	SEGGER_RTT_printf(0, "MAX_PACK_NUM: %d \r\n", bmstech::PackConst::MAX_PACK_NUM);
	SEGGER_RTT_WriteString(0, "==============================================\r\n");

	SEGGER_RTT_printf(0, "BCU Board Name: %s \r\n", VersionInfo::bcuBoardName);
	SEGGER_RTT_printf(0, "Mbed Version: %s \r\n",
			Version2String(MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION));
	SEGGER_RTT_printf(0, "BMU Protocol Version: %s \r\n",
			Version2String(BMU_PROTOCOL_VER_MAJOR, BMU_PROTOCOL_VER_MINOR, BMU_PROTOCOL_VER_PATCH));
	SEGGER_RTT_printf(0, "BCU Hardware Version: %s \r\n",
			Version2String(BCU_HW_VERSION_MAJOR, BCU_HW_VERSION_MINOR, BCU_HW_VERSION_PATCH));
	SEGGER_RTT_printf(0, "BCU Firmware Version: %s \r\n",
			Version2String(BCU_FW_VERSION_MAJOR, BCU_FW_VERSION_MINOR, BCU_FW_VERSION_PATCH));

	SEGGER_RTT_printf(0, "Build Number: %08u SY:%u\r\n", VersionInfo::BUILDNUMBER, BCU_START_YEAR);  //auto build number
	SEGGER_RTT_printf(0, "Epoch buildTimeStamp: %u \r\n", VersionInfo::BUILDTIMESTAMP); //epoch timestamp
	SEGGER_RTT_WriteString(0, "==============================================\r\n\n");

	printf("\r\n==============================================\r\n");
	printf(" CPU SystemCoreClock is %lu MHz\r\n", SystemCoreClock / 1000000U);

	printf(" BCU Firmware Version: %x.%x.%x \r\n",
			(char)BCU_FW_VERSION_MAJOR, (char)BCU_FW_VERSION_MINOR,
			(char)BCU_FW_VERSION_PATCH);
	printf(" BCU ID: %x \r\n", bmstech::BmsEngine::m_bcuId);
	printf(" MAX_PACK_NUM: %d \r\n", bmstech::PackConst::MAX_PACK_NUM);

	printf("==============================================\r\n");
	SEGGER_RTT_SetTerminal(0);     // Select terminal 0
}

//dumCanErrorRate
void DumCanErrorRate()
{
	SEGGER_RTT_printf(0, "CAN Error Rate(PPM): %u \r\n", VersionInfo::packetErrRate); // 
}
//#endif

