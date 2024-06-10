#include "spi1_driver.h"
#include "spi.h"

extern SPI_HandleTypeDef hspi1;

void SPI1_Init(void)
{
	MX_SPI1_Init();
}

void SPI1_SendByte(uint8_t* txData, uint8_t* rxData)
{
	/* Loop while SPI bus ready */
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
	//HAL_SPI_Transmit(&hspi1, txData, 1, 2);
	HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 1, 5);
}

