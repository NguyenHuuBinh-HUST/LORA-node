#ifndef SST25VF_SPI_H
#define SST25VF_SPI_H

#include "common.h"

#define sFLASH_PAGESIZE					0x1000		/* 4096 bytes */
#define sFLASH_PAGECOUNT                512         /* 2MByte storage */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* High level functions. */
void sFLASH_Init(void);
void sFLASH_EraseSector(uint32_t SectorAddr);
void sFLASH_EraseBulk(void);
void sFLASH_WriteBuffer(const uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite);
void sFLASH_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
uint32_t sFLASH_ReadID(void);

/* Flash Self Test Routine */
int sFLASH_SelfTest(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif
