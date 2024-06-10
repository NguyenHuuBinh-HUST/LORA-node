#ifndef SPI1_DRIVER_H
#define SPI1_DRIVER_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

void SPI1_Init(void);
void SPI1_SendByte(uint8_t* txData, uint8_t* rxData);

#ifdef __cplusplus
}
#endif

#endif
