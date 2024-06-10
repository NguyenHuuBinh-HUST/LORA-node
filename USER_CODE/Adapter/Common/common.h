#ifndef COMMON_LIB_H
#define COMMON_LIB_H

#if   defined ( __CC_ARM )
  #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler          */
  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */
  #define __STATIC_INLINE  static __inline
#elif defined ( __ICCARM__ )
  #define __ASM            __asm                                      /*!< asm keyword for IAR Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
  #define __STATIC_INLINE  static inline
#elif defined ( __GNUC__ )
  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
  #define __STATIC_INLINE  static inline
#endif

#include "version.h"

#include <stdint.h>
#include <stddef.h>

#include "stm32f4xx_hal.h"
//#include "core_cmFunc.h"
	
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#ifdef __cplusplus
extern "C" {
#endif

// ======Padding in struct = 0 =================	
//typedef struct __attribute__ ((packed)) _hbDATA
//{
//...
//} HB_DATA;
// =============================================
	
#pragma pack(push,1)
//======SENSOR DATA=============================
typedef struct
{
	volatile uint32_t PH;
	volatile uint32_t Oxy;
	volatile uint32_t Temp;
	volatile uint32_t Salt;
  volatile uint32_t NH3;
  volatile uint32_t H2S;
}SensorData;

//======ACCEPT RANGE VALUE======================
typedef struct
{
  uint32_t PH_High;
	uint32_t PH_Low;
	uint32_t Oxy_Low;
	uint32_t Temp_High;
	uint32_t Temp_Low;
  uint32_t Salt_High;
	uint32_t Salt_Low;
	uint32_t NH3_High;
	uint32_t NH3_Low;
	uint32_t H2S_High;
	uint32_t H2S_Low;
}SensorRange;

//======SYSTEM TIME=============================
typedef struct
{
  uint8_t Hour;
  uint8_t Min;
  uint8_t Sec;
  uint8_t Day;
  uint8_t Month;
  uint16_t Year;
}SystemTime;

//======SERVER PARAMS CONFIG====================
typedef struct
{
  uint32_t IP_Server;
  uint16_t Port;
	char IP_Gen[16];
  uint32_t Password;
  unsigned char ClientID[16]; //Can be IMEI...
	uint8_t Enable_GPRS;
	uint8_t Enable_Log2SD;
	uint32_t Revision_Firmware;
	char admin_num[32]; // Phone number Admin
	uint32_t PIN;
	uint8_t Protocol;
	uint8_t Mode;
	uint32_t Cycle_SendData;
}ServerConfig;

//======SETUP TIME PROCESSING===================
typedef struct
{
  uint32_t Cycle_GetADC;
  uint32_t Cycle_SendGPRS_Data;
  uint32_t Cycle_Log2SDCARD;
}Cycle_TimeData;

//======AES PARAMS CONFIG=======================
typedef struct
{
  uint8_t AES_Key[16];
  uint8_t AES_IV[16];
  uint8_t AES_A[16];
  uint8_t AES_B[16];
}AES_Params;

//======SYSTEM INFORMATION======================
typedef struct
{
  uint16_t CPU_Temp;     // oC
  uint16_t VBAT_Value;   // mV
  uint32_t SystemUptime; // Second
  uint8_t  SDCard_State; 
	uint32_t SDCard_AllSize;
	uint32_t SDCard_FreeSize;
	uint16_t SDCard_LogFile;
  uint8_t Flag_Change_Server;
  uint8_t Flag_Sensor_OverRange;
	uint32_t Cycle_MDM_Led;
	int GSM_RSSI;
	//Encrypt password
	uint32_t CryptBase1;
	uint32_t CryptBase2;
}SystemInformation;

//==============================================
typedef struct
{
  ServerConfig Flash_ServerConfig;
  Cycle_TimeData Flash_CycleTimeData;
	SensorRange Flash_SensorRange;
}Flash_Config_Store;

typedef struct
{
  char IMEI[15];
  char Command;
  SystemTime Time;
  char Data[1];
}DataPackage;
//================================

#pragma pack(pop)

//======COMMON FUNCTION=========================

void HAL_Make_First_System_Parameter(void);

void Set_Default_System_Config(void);

void AES_B_Generator(void);

uint16_t HAL_Core_Compute_CRC16(const uint8_t* pBuffer, uint32_t bufferSize);

void HAL_Delay_ms(uint32_t);

void HAL_Core_System_Reset(void);

void Error_Handler(void);
	
//void CRC_ResetDR(void);

//uint32_t HAL_Core_Compute_CRC32(const uint8_t *, uint32_t);
 
#ifdef __cplusplus
}
#endif

#endif
