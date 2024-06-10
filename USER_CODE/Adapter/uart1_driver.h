#ifndef UART1_DRIVER_H
#define UART1_DRIVER_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

void HW_UART3_Init(void);
void HW_UART3_DeInit(void);
void UART3_Send(const char*, int);

#ifdef __cplusplus
}
#endif

#endif
