#include "usart.h"
#include "main.h"
#ifndef TASK_LORA_H

#define TASK_LORA_H

#define Eb_HUARTTypeDef    UART_HandleTypeDef

#define Eb_GPIOTypeDef     GPIO_TypeDef

#define Eb_Stt_Port        GPIOB

#define Eb_Stt_Pin         GPIO_PIN_3

#define EB_RX_BUFFER_SIZE  100
typedef enum LoRaSV_PROCESS_STATE_s
{
	LoRaSV_STATE_SLEEP = 0x00, \
	LoRaSV_STATE_TX, \
	LoRaSV_STATE_RX
	
}LoRaSV_PROCESS_STATE_t;

typedef struct
{
	Eb_GPIOTypeDef  *gpio_port;
	uint16_t            gpio_pin;
} Eb_Gpio_t;

typedef struct 
{
	Eb_HUARTTypeDef *huart;
	Eb_Gpio_t       stt_pin;
	
	uint8_t		rxBuffer[EB_RX_BUFFER_SIZE];
	uint16_t	rxIndex;
	uint8_t		rxTmp[2];	
	uint8_t   cpltUartdata;
	uint16_t  dataLen;
} ExtBoard_t;
void Task_LoRa(void *params);
#endif
