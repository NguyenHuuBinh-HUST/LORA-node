#include "pinmap.h"

static GPIO_Pin __PIN_MAP[] =
{
/* DES					MAP 					MASK		*/
/* MDM_LED			-	0		*/				{GPIOB, GPIO_PIN_9},
/* ALERT_SPK		-	3		*/				{GPIOB, GPIO_PIN_6},
/* LCD_RS	  		- 4		*/				{GPIOA, GPIO_PIN_12},
/* LCD_EN	  		- 5		*/				{GPIOA, GPIO_PIN_11},
/* LCD_D4	  		- 6		*/				{GPIOA, GPIO_PIN_8},
/* LCD_D5	  		- 7		*/				{GPIOC, GPIO_PIN_9},
/* LCD_D6	  		- 8		*/				{GPIOB, GPIO_PIN_8},
/* LCD_D7	  		- 9		*/				{GPIOB, GPIO_PIN_7},
/* SD_CD	  		- 10	*/				{GPIOC, GPIO_PIN_6},
/* SD_CS	  		- 11	*/				{GPIOB, GPIO_PIN_12},
/* LCD_LI	  		- 12	*/				{GPIOB, GPIO_PIN_0},
/* FLASH_CS	  	- 13	*/				{GPIOC, GPIO_PIN_4},
/* MDM_PWR	  	- 14	*/				{GPIOA, GPIO_PIN_0},
/* MDM_RESET  	- 15	*/				{GPIOA, GPIO_PIN_1},
/* MDM_RST  		- 16	*/				{GPIOD, GPIO_PIN_2},
/* MDM_DTR  		- 17	*/				{GPIOD, GPIO_PIN_2},
/* SD_WP  			- 18	*/				//{GPIOD, GPIO_PIN_2},
};

GPIO_Pin* HAL_Pin_Map() {
    return __PIN_MAP;
}

