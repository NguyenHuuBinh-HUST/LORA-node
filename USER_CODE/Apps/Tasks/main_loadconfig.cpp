#include <string.h>
#include "common.h"

#include "main_loadconfig.h"


Flash_Config_Store flashData;
extern ServerConfig serverCfg;
extern Cycle_TimeData timeDataCtl;

void Start_Init_ExtFlash(void)
{
	flashExtMem.Start();
}

void Check_Flash_Config_Saved(void)
{
	//Clear cache
	memset(&flashData, 0, sizeof(flashData));
	//Load data saved from flash
	flashExtMem.read(&flashData, 0, sizeof(flashData));
	//Compare firmware revision
	//If saved data older firmware
	//=> Clear all, update new data
	if((serverCfg.Revision_Firmware > flashData.Flash_ServerConfig.Revision_Firmware) || (flashData.Flash_ServerConfig.Revision_Firmware > 10000)) //When flash is empty, loaded value = 2^32 (0xffffffff)
	{
		//Increase firmware revision to lastest
		serverCfg.Revision_Firmware++;
		flashData.Flash_CycleTimeData = timeDataCtl;
		flashData.Flash_ServerConfig  = serverCfg;

		//Erase all data in flash
		flashExtMem.eraseAll();
		flashExtMem.write(&flashData, 0, sizeof(flashData));
	}
	memset(&flashData, 0, sizeof(flashData));
	Read_Flash_Config_Saved();
}

void Read_Flash_Config_Saved(void)
{
	memset(&flashData, 0, sizeof(flashData));
	flashExtMem.read(&flashData, 0, sizeof(flashData));
	timeDataCtl = flashData.Flash_CycleTimeData;
	serverCfg   = flashData.Flash_ServerConfig;

	memset(&flashData, 0, sizeof(flashData));
}

void Write_Flash_New_Config(void)
{
	memset(&flashData, 0, sizeof(flashData));
	flashData.Flash_CycleTimeData = timeDataCtl;
	flashData.Flash_ServerConfig  = serverCfg;

	flashExtMem.erasePage(0);
	flashExtMem.write(&flashData, 0, sizeof(flashData));
	memset(&flashData, 0, sizeof(flashData));
}
