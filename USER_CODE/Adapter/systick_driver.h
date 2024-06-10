#ifndef SYSTICK_DRIVER_H
#define SYSTICK_DRIVER_H

#include <stdint.h>

/***
	Edit in stm32fxxx_it.c - At SysTick_Handler() -> System1MsTick
***/

#ifdef __cplusplus
extern "C" {
#endif

void System1MsTick(void);

uint32_t HAL_Timer_Get_Milli_Seconds(void);

uint32_t HAL_Timer_Get_Uptime_Status(void);

#ifdef __cplusplus
}
#endif

#endif
