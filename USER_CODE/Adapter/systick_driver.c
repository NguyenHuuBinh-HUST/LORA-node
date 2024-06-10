#include "common.h"
#include "systick_driver.h"

extern void xPortSysTickHandler(void);
extern SystemInformation systemInfo;


static volatile uint32_t system_millis = 0;
static volatile uint32_t system_uptime = 0;

volatile unsigned long ulHighFrequencyTimerTicks;

void System1MsTick(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
		system_millis++;
		ulHighFrequencyTimerTicks++;
    if(!(system_millis % 1000)) 
			{
				system_uptime++; 
				systemInfo.SystemUptime = system_uptime;
			}
}

uint32_t GetSystem1MsTick(void)
{
    return system_millis;
}

uint32_t HAL_Timer_Get_Milli_Seconds(void)
{
    //return GetSystem1MsTick();
	return HAL_GetTick();
}

uint32_t HAL_Timer_Get_Uptime_Status(void)
{
	return system_uptime;
}

