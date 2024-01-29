//File
#pragma once

#include <string>
#include "mbed.h"
#include "compile_time.h"

#define BCU_HW_VERSION_MAJOR	1	//2-digit 1.0.0: NUCLEO F756ZG Board
#define BCU_HW_VERSION_MINOR	1	//2-digit 
#define BCU_HW_VERSION_PATCH	0	//2-digit 

#define BCU_FW_VER				0x01	//2-digit  0x05:    0.5 version @
#define BCU_FW_VERSION_MAJOR	0	//2-digit  0x05:    0.5 version @
#define BCU_FW_VERSION_MINOR	0	//2-digit
#define BCU_FW_VERSION_PATCH	1	//2-digit

#define BCU_VERSION_BUILD	1	//8-digit  2020-01-01 mm-dd-hh-mm 04021530 (uint32-t)

#define BMU_PROTOCOL_VER_MAJOR	1	// BMU (CM) CAN Protocol for Pack Module 
#define BMU_PROTOCOL_VER_MINOR	0
#define BMU_PROTOCOL_VER_PATCH	0	 

#define BSU_PROTOCOL_VER_MAJOR	1	// for eferry MBU CAN
#define BSU_PROTOCOL_VER_MINOR	0
#define BSU_PROTOCOL_VER_PATCH	5	//

//Automatic build number in compile_time.h:
#define BCU_ENCODE_VERSION(major, minor, patch) ((major)*10000 + (minor)*100 + (patch))

#define BCU_BOARD_NAME			"NUCLEO STM32F767ZI_BOARD"			//JY 20220607
#define BCU_HW_VERSION		BCU_ENCODE_VERSION(BCU_HW_VERSION_MAJOR, BCU_HW_VERSION_MINOR, BCU_HW_VERSION_PATCH)
#define BCU_FW_VERSION		BCU_ENCODE_VERSION(BCU_FW_VERSION_MAJOR, BCU_FW_VERSION_MINOR, BCU_FW_VERSION_PATCH)
#define EMS_PROTOCOL_VERSION BCU_ENCODE_VERSION(BSU_PROTOCOL_VER_MAJOR, BSU_PROTOCOL_VER_MINOR, BSU_PROTOCOL_VER_PATCH)
#define BMU_PROTOCOL_VERSION BCU_ENCODE_VERSION(BMU_PROTOCOL_VER_MAJOR, BMU_PROTOCOL_VER_MINOR, BMU_PROTOCOL_VER_PATCH)

#define BCU_BUILD_NUMBER		__AUTO_BUILD_NUMBER__	//compile_time.h
#define BCU_BUILD_TIMESTAMP		__TIME_UNIX__			//compile_time.h



//static char version[] = BCU_VERSION_MAJOR "." BCU_VERSION_MINOR "." BCU_VERSION_PATCH "." BCU_VERSION_BUILD;
//static char timestamp[] = __DATE__ " " __TIME__;

//#define MBED_ENCODE_VERSION(major, minor, patch) ((major)*10000 + (minor)*100 + (patch))
//#define MBED_VERSION MBED_ENCODE_VERSION(MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION)

/* Some test definition here */
#define DEFINED_BUT_NO_VALUE
#define DEFINED_INT 3
#define DEFINED_STR "ABC"

/* definition to expand macro then apply to pragma message */
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

#define STRING2(x) #x
#define STRING(x) STRING2(x)


/* Some example here */
//#pragma message(VAR_NAME_VALUE(NOT_DEFINED))
//#pragma message(VAR_NAME_VALUE(DEFINED_BUT_NO_VALUE))
//#pragma message(VAR_NAME_VALUE(DEFINED_INT))

class VersionInfo
{
public:

	static const uint32_t MBEDVERSION = MBED_VERSION;
	static const uint32_t EMSPROTOCOLVERSION = EMS_PROTOCOL_VERSION; 
	static const uint32_t BMUPROTOCOLVERSION = BMU_PROTOCOL_VERSION;
	static const uint32_t BCUHARDWAREVERSION = BCU_HW_VERSION;
	static const uint32_t BCUFIRMWAREVERSION = BCU_FW_VERSION;

	static const char* bcuBoardName; //initializer in implementation file
	
	static const uint32_t BUILDNUMBER = BCU_BUILD_NUMBER;
	static const uint32_t BUILDTIMESTAMP = BCU_BUILD_TIMESTAMP;

	static uint32_t mainThreadCount;
	static uint32_t bcuThreadCount;

	static uint32_t canTickSec;
	static uint32_t packetErrRate; //unit:PPM
};

void DumpVersionInfo();
void DumCanErrorRate();
