#include "usart.h"
#include "uart1_driver.h"

#define UART3_TIME_SEND_DATA              500

/* RX Data from UART Interrupt */
char UART3_RX_Data;

/* Uart for modem */
extern UART_HandleTypeDef huart3;

/* USART1 init function */
void HW_UART3_Init(void)
{
	MX_USART3_UART_Init();

  //Start UART
  __HAL_UART_FLUSH_DRREGISTER(&huart3);
  //Start Receive DMA
  HAL_UART_Receive_DMA(&huart3, (uint8_t *)&UART3_RX_Data, 1);
}

void HW_UART3_DeInit(void)
{
  HAL_UART_DeInit(&huart3);
}

void UART3_Send(const char* buf, int len)
{
	HAL_UART_Transmit(&huart3, (uint8_t *)buf, len, UART3_TIME_SEND_DATA);
}
