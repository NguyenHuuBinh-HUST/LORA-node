#include <stdlib.h>
#include <limits.h>
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* =========DEFINE DELAY ========================== */
#define SYSTEM_US_TICKS		(SystemCoreClock / 1000000)//cycles per microsecond

/* =========DEFINE PARAMETER======================= */
static const unsigned int CRC_Tab16[] =
{
    0X0000, 0X1189, 0X2312, 0X329B, 0X4624, 0X57AD, 0X6536, 0X74BF,
    0X8C48, 0X9DC1, 0XAF5A, 0XBED3, 0XCA6C, 0XDBE5, 0XE97E, 0XF8F7,
    0X1081, 0X0108, 0X3393, 0X221A, 0X56A5, 0X472C, 0X75B7, 0X643E,
    0X9CC9, 0X8D40, 0XBFDB, 0XAE52, 0XDAED, 0XCB64, 0XF9FF, 0XE876,
    0X2102, 0X308B, 0X0210, 0X1399, 0X6726, 0X76AF, 0X4434, 0X55BD,
    0XAD4A, 0XBCC3, 0X8E58, 0X9FD1, 0XEB6E, 0XFAE7, 0XC87C, 0XD9F5,
    0X3183, 0X200A, 0X1291, 0X0318, 0X77A7, 0X662E, 0X54B5, 0X453C,
    0XBDCB, 0XAC42, 0X9ED9, 0X8F50, 0XFBEF, 0XEA66, 0XD8FD, 0XC974,
    0X4204, 0X538D, 0X6116, 0X709F, 0X0420, 0X15A9, 0X2732, 0X36BB,
    0XCE4C, 0XDFC5, 0XED5E, 0XFCD7, 0X8868, 0X99E1, 0XAB7A, 0XBAF3,
    0X5285, 0X430C, 0X7197, 0X601E, 0X14A1, 0X0528, 0X37B3, 0X263A,
    0XDECD, 0XCF44, 0XFDDF, 0XEC56, 0X98E9, 0X8960, 0XBBFB, 0XAA72,
    0X6306, 0X728F, 0X4014, 0X519D, 0X2522, 0X34AB, 0X0630, 0X17B9,
    0XEF4E, 0XFEC7, 0XCC5C, 0XDDD5, 0XA96A, 0XB8E3, 0X8A78, 0X9BF1,
    0X7387, 0X620E, 0X5095, 0X411C, 0X35A3, 0X242A, 0X16B1, 0X0738,
    0XFFCF, 0XEE46, 0XDCDD, 0XCD54, 0XB9EB, 0XA862, 0X9AF9, 0X8B70,
    0X8408, 0X9581, 0XA71A, 0XB693, 0XC22C, 0XD3A5, 0XE13E, 0XF0B7,
    0X0840, 0X19C9, 0X2B52, 0X3ADB, 0X4E64, 0X5FED, 0X6D76, 0X7CFF,
    0X9489, 0X8500, 0XB79B, 0XA612, 0XD2AD, 0XC324, 0XF1BF, 0XE036,
    0X18C1, 0X0948, 0X3BD3, 0X2A5A, 0X5EE5, 0X4F6C, 0X7DF7, 0X6C7E,
    0XA50A, 0XB483, 0X8618, 0X9791, 0XE32E, 0XF2A7, 0XC03C, 0XD1B5,
    0X2942, 0X38CB, 0X0A50, 0X1BD9, 0X6F66, 0X7EEF, 0X4C74, 0X5DFD,
    0XB58B, 0XA402, 0X9699, 0X8710, 0XF3AF, 0XE226, 0XD0BD, 0XC134,
    0X39C3, 0X284A, 0X1AD1, 0X0B58, 0X7FE7, 0X6E6E, 0X5CF5, 0X4D7C,
    0XC60C, 0XD785, 0XE51E, 0XF497, 0X8028, 0X91A1, 0XA33A, 0XB2B3,
    0X4A44, 0X5BCD, 0X6956, 0X78DF, 0X0C60, 0X1DE9, 0X2F72, 0X3EFB,
    0XD68D, 0XC704, 0XF59F, 0XE416, 0X90A9, 0X8120, 0XB3BB, 0XA232,
    0X5AC5, 0X4B4C, 0X79D7, 0X685E, 0X1CE1, 0X0D68, 0X3FF3, 0X2E7A,
    0XE70E, 0XF687, 0XC41C, 0XD595, 0XA12A, 0XB0A3, 0X8238, 0X93B1,
    0X6B46, 0X7ACF, 0X4854, 0X59DD, 0X2D62, 0X3CEB, 0X0E70, 0X1FF9,
    0XF78F, 0XE606, 0XD49D, 0XC514, 0XB1AB, 0XA022, 0X92B9, 0X8330,
    0X7BC7, 0X6A4E, 0X58D5, 0X495C, 0X3DE3, 0X2C6A, 0X1EF1, 0X0F78,
};

/* =========INITIAL SYSTEM PARAMETER================== */
SystemTime timeSystemCr;
ServerConfig serverCfg;
Cycle_TimeData timeDataCtl;
AES_Params aesConfig;
SystemInformation systemInfo;


/* ================================================ */

void HAL_Make_First_System_Parameter(void)
{

	
	//AES Key Define
	aesConfig.AES_IV[0]  = 'B';    aesConfig.AES_A[0]  = 'T';    aesConfig.AES_B[0]  = 'C';    
	aesConfig.AES_IV[1]  = 'K';    aesConfig.AES_A[1]  = 'H';    aesConfig.AES_B[1]  = 'o';
	aesConfig.AES_IV[2]  = 'R';    aesConfig.AES_A[2]  = 'A';    aesConfig.AES_B[2]  = 'p';
	aesConfig.AES_IV[3]  = 'E';    aesConfig.AES_A[3]  = 'N';    aesConfig.AES_B[3]  = 'y';
	aesConfig.AES_IV[4]  = 'S';    aesConfig.AES_A[4]  = 'H';    aesConfig.AES_B[4]  = 'r';
	aesConfig.AES_IV[5]  = '_';    aesConfig.AES_A[5]  = 'N';    aesConfig.AES_B[5]  = 'i';
	aesConfig.AES_IV[6]  = 'S';    aesConfig.AES_A[6]  = 'T';    aesConfig.AES_B[6]  = 'g';
	aesConfig.AES_IV[7]  = 'E';    aesConfig.AES_A[7]  = 'D';    aesConfig.AES_B[7]  = 'h';
	aesConfig.AES_IV[8]  = 'C';    aesConfig.AES_A[8]  = 'O';    aesConfig.AES_B[8]  = 't';
	aesConfig.AES_IV[9]  = 'R';    aesConfig.AES_A[9]  = 'T';    aesConfig.AES_B[9]  = '@';
	aesConfig.AES_IV[10] = 'E';    aesConfig.AES_A[10] = 'C';    aesConfig.AES_B[10] = '2';
	aesConfig.AES_IV[11] = 'T';    aesConfig.AES_A[11] = 'O';    aesConfig.AES_B[11] = '0';
	aesConfig.AES_IV[12] = '_';    aesConfig.AES_A[12] = 'M';    aesConfig.AES_B[12] = '1';
	aesConfig.AES_IV[13] = 'K';    aesConfig.AES_A[13] = ':';    aesConfig.AES_B[13] = '6';
	aesConfig.AES_IV[14] = 'E';    aesConfig.AES_A[14] = ')';    aesConfig.AES_B[14] = 'V';
	aesConfig.AES_IV[15] = 'Y';    aesConfig.AES_A[15] = ')';    aesConfig.AES_B[15] = 'N';
	
	//System Time
	timeSystemCr.Day   = 8;
	timeSystemCr.Month = 9;
	timeSystemCr.Year  = 2018;
	timeSystemCr.Hour  = 5;
	timeSystemCr.Min   = 10;
	timeSystemCr.Sec   = 0;
	
	//System Infomations
	systemInfo.SystemUptime       = 0;
	systemInfo.CPU_Temp           = 0;
	systemInfo.VBAT_Value         = 0;
	systemInfo.SDCard_State       = 0;
	systemInfo.SDCard_AllSize     = 0;
	systemInfo.SDCard_FreeSize    = 0;
	systemInfo.SDCard_LogFile     = 0;
	systemInfo.Flag_Change_Server = 0;
	systemInfo.Flag_Sensor_OverRange = 0;
	systemInfo.Cycle_MDM_Led      = 0;
	systemInfo.GSM_RSSI           = 0;
	systemInfo.CryptBase1         = 2111;
	systemInfo.CryptBase2         = 1990;
	
	//System Config
	Set_Default_System_Config();
}

void Set_Default_System_Config()
{
	
	//Server configuration
	serverCfg.IP_Server = (((uint32_t)(202))<<24) | (((uint32_t)(191))<<16) | (((uint32_t)(56))<< 8) | (((uint32_t)(104))<< 0);
	serverCfg.Port      = 5544;
//	serverCfg.IP_Server = (((uint32_t)(3))<<24) | (((uint32_t)(131))<<16) | (((uint32_t)(147))<< 8) | (((uint32_t)(49))<< 0);
//	serverCfg.Port      = 17273;
	serverCfg.Password  = 12345;
	strcpy(serverCfg.IP_Gen,"205.205.204.6");
	serverCfg.ClientID[0]  = '3';				serverCfg.admin_num[0]  = '+';
	serverCfg.ClientID[1]  = '5';				serverCfg.admin_num[1]  = '8';
	serverCfg.ClientID[2]  = '3';				serverCfg.admin_num[2]  = '4';
	serverCfg.ClientID[3]  = '3';				serverCfg.admin_num[3]  = '9';
	serverCfg.ClientID[4]  = '0';				serverCfg.admin_num[4]  = '2';
	serverCfg.ClientID[5]  = '1';				serverCfg.admin_num[5]  = '1';
	serverCfg.ClientID[6]  = '0';				serverCfg.admin_num[6]  = '2';
	serverCfg.ClientID[7]  = '5';				serverCfg.admin_num[7]  = '3';
	serverCfg.ClientID[8]  = '5';				serverCfg.admin_num[8]  = '9';
	serverCfg.ClientID[9]  = '8';				serverCfg.admin_num[9]  = '3';
	serverCfg.ClientID[10] = '0';				serverCfg.admin_num[10] = '4';
	serverCfg.ClientID[11] = '2';				serverCfg.admin_num[11] = '0';
	serverCfg.ClientID[12] = '3';				serverCfg.admin_num[12] = '\0';
	serverCfg.ClientID[13] = '6';
	serverCfg.ClientID[14] = '2';
	serverCfg.ClientID[15] = '\0';
	serverCfg.Enable_GPRS  = 1;
	serverCfg.Enable_Log2SD = 1;
	serverCfg.Revision_Firmware  = 150;
	serverCfg.PIN = 1234;
	serverCfg.Mode = 1;
	serverCfg.Cycle_SendData = 2000;
	//Time process data(ms)
	timeDataCtl.Cycle_GetADC     = 5000;
	timeDataCtl.Cycle_Log2SDCARD = 10000;
	timeDataCtl.Cycle_SendGPRS_Data = 60000;
}

void AES_B_Generator(void)
{
	int i;
	for(i = 0; i < 16; i++)
    {
         aesConfig.AES_B[i] = (rand() % 100)+1;          
    }
}

uint16_t HAL_Core_Compute_CRC16(const uint8_t* pBuffer, uint32_t bufferSize)
{
	uint16_t  fcs  =  0xffff;         //  initialization
    while(bufferSize>0){
        fcs = (fcs >> 8) ^ CRC_Tab16[(fcs ^ *pBuffer) & 0xff];
        bufferSize--;
        pBuffer++;
    }
    return  ~fcs;
}

void HAL_Delay_ms(uint32_t nTime)
{
#ifdef USING_RTOS
	vTaskDelay(nTime);
#else
	HAL_Delay(nTime);
#endif
}

//void HAL_Delay_us(uint32_t nTime)
//{
//	volatile uint32_t DWT_START = DWT->CYCCNT;
//	volatile uint32_t DWT_TOTAL = (SYSTEM_US_TICKS * nTime);
//	// keep DWT_TOTAL from overflowing (max 59.652323s w/72MHz SystemCoreClock)
//	if(nTime > (UINT_MAX / SYSTEM_US_TICKS))
//	{
//		nTime = (UINT_MAX / SYSTEM_US_TICKS);
//	}
//	while((DWT->CYCCNT - DWT_START) < DWT_TOTAL)
//  {
//    //KICK_WDT();
//  }
//}

void HAL_Core_System_Reset(void)
{
	NVIC_SystemReset();
}

//void Error_Handler(void)
//{
//	
//}

///**
// * @brief  Computes the 32-bit CRC of a given buffer of byte data.
// * @param  pBuffer: pointer to the buffer containing the data to be computed
// * @param  BufferSize: Size of the buffer to be computed
// * @retval 32-bit CRC
// */
//uint32_t HAL_Core_Compute_CRC32(const uint8_t *pBuffer, uint32_t bufferSize)
//{
//	/* Hardware CRC32 calculation */
//	uint32_t i, j;
//	uint32_t Data;

//	CRC_ResetDR();

//	i = bufferSize >> 2;

//	while (i--)
//	{
//		Data = *((uint32_t *)pBuffer);
//		pBuffer += 4;

//		Data = __RBIT(Data);//reverse the bit order of input Data
//		CRC->DR = Data;
//	}

//	Data = CRC->DR;
//	Data = __RBIT(Data);//reverse the bit order of output Data

//	i = bufferSize & 3;

//	while (i--)
//	{
//		Data ^= (uint32_t)*pBuffer++;

//		for (j = 0 ; j < 8 ; j++)
//		{
//			if (Data & 1)
//				Data = (Data >> 1) ^ 0xEDB88320;
//			else
//				Data >>= 1;
//		}
//	}

//	Data ^= 0xFFFFFFFF;

//	return Data;
//}

///**
//  * @brief  Resets the CRC Data register (DR).
//  * @param  None
//  * @retval None
//  */
//void CRC_ResetDR(void)
//{
//  /* Reset CRC generator */
//  CRC->CR = CRC_CR_RESET;
//}

