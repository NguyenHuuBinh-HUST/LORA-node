#ifndef PINMAP_LIB_H
#define PINMAP_LIB_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

//PinMap Construct
typedef struct GPIO_Pin{
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pinx;
} GPIO_Pin;

typedef struct GPIO_Pin GPIO_Pin;

GPIO_Pin* HAL_Pin_Map(void);

//DEFINE PIN MAP
#define MDM_PWR          14 //Out
#define MDM_RESET        15 //Out
#define MDM_RTS          16 //Out
#define MDM_DTR          17 //Out
#define MDM_LED			  		0 //Out
#define sFLASH_CE        13 //Out
#define SD_Detector      10 //Input
#define SD_WriteProtect  18 //Input
#define SD_CE            11 //Out - SD Select
#define SD_LED			  1 //Out
#define ALERT_LI		  2 //Out
#define ALERT_SPK		  3 //Out
#define LCD_LI			 12 //Out
#define LCD_RS			  4 //Out
#define LCD_EN			  5 //Out
#define LCD_D4			  6 //Out
#define LCD_D5			  7 //Out
#define LCD_D6			  8 //Out
#define LCD_D7			  9 //Out


#ifdef __cplusplus
}
#endif

#endif
