#ifndef TASK_CELLULAR_H
#define TASK_CELLULAR_H

#include "main.h"
#include "usart.h"

#define Eb_HUARTTypeDef    UART_HandleTypeDef
	
#define Eb_GPIOTypeDef     GPIO_TypeDef

#define Eb_Stt_Port        GPIOB

#define Eb_Stt_Pin         GPIO_PIN_3

#define EB_RX_BUFFER_SIZE  100

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
typedef struct
{
	uint8_t version[1];
	uint8_t contype[1];
	uint8_t devEUI[8];
	uint8_t arg_1[2];
	uint8_t arg_2[2];
	uint8_t arg_3[2];
	uint8_t day[1];
	uint8_t month[1];
	uint8_t year[1];
	uint8_t hh[1];
	uint8_t mm[1];
	uint8_t ss[1];
	uint8_t latitude[4];
	uint8_t longtitude[4];
	uint8_t altitude[2];
}sendMsg;
void task_Cellular(void *params);
void task_Command_From_Server(void *params);
void task_SMS_From_Server(void *params);

#endif
