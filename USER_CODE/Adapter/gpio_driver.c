#include "gpio_driver.h"
#include "gpio.h"
/* -----------------------------------------------------------------------*/
/* DEFINE VARIABLE CONFIG ------------------------------------------------*/
/* -----------------------------------------------------------------------*/


/* -----------------------------------------------------------------------*/
/* HARDWARE LAYER --------------------------------------------------------*/
/* -----------------------------------------------------------------------*/
void HW_GPIO_Init(void)
{
  MX_GPIO_Init();
}

/* -----------------------------------------------------------------------*/
/* DRIVER LAYER ----------------------------------------------------------*/
/* -----------------------------------------------------------------------*/
void GPIO_Write(uint16_t pin, uint8_t value)
{
	GPIO_Pin * PIN_MAP = HAL_Pin_Map();
	HAL_GPIO_WritePin(PIN_MAP[pin].GPIOx, PIN_MAP[pin].GPIO_Pinx, value ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void GPIO_Toggle(uint16_t pin)
{
  GPIO_Pin * PIN_MAP = HAL_Pin_Map();
	HAL_GPIO_TogglePin(PIN_MAP[pin].GPIOx, PIN_MAP[pin].GPIO_Pinx);
}

uint8_t GPIO_Read(uint16_t pin)
{
  GPIO_Pin * PIN_MAP = HAL_Pin_Map();
  return HAL_GPIO_ReadPin(PIN_MAP[pin].GPIOx, PIN_MAP[pin].GPIO_Pinx);
}

