/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define NSS_Pin GPIO_PIN_0
#define NSS_GPIO_Port GPIOB
#define RESET_Pin GPIO_PIN_1
#define RESET_GPIO_Port GPIOB
#define DIO0_Pin GPIO_PIN_2
#define DIO0_GPIO_Port GPIOB
#define DIO0_EXTI_IRQn EXTI2_IRQn

/* USER CODE BEGIN Private defines */
#define MAX_LORA_PAYLOAD 255
#define LED_BLINK_TIME 1000
#define RX_BUFFER_SIZE 255
/* USER CODE END Private defines */

#ifdef __cplusplus
}

/* USER CODE BEGIN Private defines */
extern uint8_t cdcRxBuffer[RX_BUFFER_SIZE];
extern volatile uint32_t cdcRxLength;
extern volatile uint8_t cdcDataReceived;
/* USER CODE END Private defines */
#endif

#endif /* __MAIN_H */
