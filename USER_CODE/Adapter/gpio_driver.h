#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include "common.h"
#include "pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

void HW_GPIO_Init(void);
void GPIO_Write(uint16_t, uint8_t);
void GPIO_Toggle(uint16_t);
uint8_t GPIO_Read(uint16_t);

#ifdef __cplusplus
}
#endif

#endif
