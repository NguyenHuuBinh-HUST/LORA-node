#include "task_InterBoard.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "Misc.h"

#define SEND_UPDATE_TIME   2000

typedef struct BoardInfo_s
{
	uint8_t BoardCMD[10];
	uint8_t Battery;       // %
	uint8_t Temp;          // oC
}BoardInfo_t;

BoardInfo_t BoardInfo;

uint8_t board_rx_data[2];
uint8_t board_rx_buffer[256];
uint8_t board_rx_index = 0;
uint8_t board_rx_cplt = 0;

uint8_t board_tx_buffer[256];

void Task_Send_BoardInfo(void *params)
{
	srand1(411); //seed
	
	while(1)
	{
		memcpy((char *)BoardInfo.BoardCMD, "Hello", 5);
		BoardInfo.Battery = randr(60,90);
		BoardInfo.Temp =  randr(20,35);
		
		memset(board_tx_buffer, 0, 256);
		sprintf((char *)board_tx_buffer,"@|%s|%d|%d|$", (char *)BoardInfo.BoardCMD, \
		                                            BoardInfo.Battery,  \
											                          BoardInfo.Temp);
											
		HAL_UART_Transmit(&huart6, board_tx_buffer, strlen((char *) board_tx_buffer), 1000);
		vTaskDelay(SEND_UPDATE_TIME);
	}
}

void Board_Rx_Init(void)
{
	MX_USART6_UART_Init();
	
	HAL_UART_Receive_DMA(&huart6, board_rx_data, 1);
}

Std_ReturnType Get_External_Data(uint8_t * rx_buffer, uint8_t rx_len, uint32_t timeout)
{
	
	return STD_OK;
}


