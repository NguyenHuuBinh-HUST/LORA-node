#ifndef UART2_DRIVER_H
#define UART2_DRIVER_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

void HW_UART2_Init(void);
void HW_UART2_DeInit(void);
void UART2_Send(const char*, int);

#ifdef __cplusplus
}
#endif



#endif
