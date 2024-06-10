#include "usart.h"
#include "uart2_driver.h"

/* -----------------------------------------------------------------------*/
/* DEFINE VARIABLE CONFIG ------------------------------------------------*/
/* -----------------------------------------------------------------------*/
#define MDM_HARDWARE_FLOW_CONTROL_RTS_CTS 0
#define UART2_TIME_SEND_DATA							5000

/* RX Data from UART Interrupt */
char UART2_RX_Data;

/* Uart for modem */
extern UART_HandleTypeDef huart2;

/* -----------------------------------------------------------------------*/
/* FUNCTIONS -------------------------------------------------------------*/
/* -----------------------------------------------------------------------*/
void HW_UART2_Init(void)
{	
	MX_USART2_UART_Init();
	
	//Start UART
	__HAL_UART_FLUSH_DRREGISTER(&huart2);
	//Start Receive Interrupt
	//HAL_UART_Receive_IT(&huart2, (uint8_t *)&UART2_RX_Data, 1);
	//Start Receive DMA
	HAL_UART_Receive_DMA(&huart2, (uint8_t *)&UART2_RX_Data, 1);
}

void HW_UART2_DeInit(void)
{
	HAL_UART_DeInit(&huart2);
}

void UART2_Send(const char* buf, int len)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)buf, len, UART2_TIME_SEND_DATA);
}

