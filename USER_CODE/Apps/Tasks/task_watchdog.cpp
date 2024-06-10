#include "common.h"
#include "iwdg_driver.h"
#include "task_watchdog.h"


extern uint16_t time_Check_Lora;
//extern uint16_t time_Check_Sim;
void task_sysWatchDog(void *params)
{
	HW_IWDG_Init();
//	IWDG_Start();
//	IWDG_Reload();
	while(1)
	{
		vTaskDelay(7000);
		IWDG_Reload();
		time_Check_Lora++;
	//	time_Check_Sim++;
		if(time_Check_Lora >  50)
		{
			HAL_NVIC_SystemReset();
		}
	}
}
