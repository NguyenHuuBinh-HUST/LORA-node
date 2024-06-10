/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USART2_RI_Pin GPIO_PIN_4
#define USART2_RI_GPIO_Port GPIOA
#define SPI1_S_NSS_Pin GPIO_PIN_4
#define SPI1_S_NSS_GPIO_Port GPIOC
#define SPI2_S_NSS_Pin GPIO_PIN_12
#define SPI2_S_NSS_GPIO_Port GPIOB
#define USART4_RST_Pin GPIO_PIN_15
#define USART4_RST_GPIO_Port GPIOA
#define LoRa_LED_Pin GPIO_PIN_4
#define LoRa_LED_GPIO_Port GPIOB
#define SPI2_IO0_Pin GPIO_PIN_7
#define SPI2_IO0_GPIO_Port GPIOB
#define SPI2_IO1_Pin GPIO_PIN_8
#define SPI2_IO1_GPIO_Port GPIOB
#define SPI2_RST_Pin GPIO_PIN_9
#define SPI2_RST_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
