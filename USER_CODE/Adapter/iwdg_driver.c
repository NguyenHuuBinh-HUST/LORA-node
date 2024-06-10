#include "common.h"
#include "iwdg_driver.h"
#include "iwdg.h"

extern IWDG_HandleTypeDef hiwdg;

/*
* IWDG using clock from LSI
* IWDG counter clock Frequency = LSIFreq / PrescaleValue (40k / 128 ~ 312Hz)
*  => Counter Reload Value: 4096
*  => WDG timeout: 4096 / 312 ~ 13 seconds
*/

void HW_IWDG_Init(void)
{
	MX_IWDG_Init();
}

void IWDG_Start(void)
{
	__HAL_IWDG_START(&hiwdg);
}

void IWDG_Reload(void)
{
	HAL_IWDG_Refresh(&hiwdg);
}
