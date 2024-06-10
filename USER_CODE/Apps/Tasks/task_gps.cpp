#include "task_gps.h"
#include "GPS.h"

void Task_GetGPS(void * params)
{
	GPS_Init();
	
	while(1)
	{
		GPS_Process();
		vTaskDelay(100);
	}	
}