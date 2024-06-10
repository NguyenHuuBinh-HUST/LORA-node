#include "Serial_Debug.h"

PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&DEBUG_Port, (uint8_t*)&ch,1,100);
  return ch;
}
