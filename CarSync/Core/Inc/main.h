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
#include "stm32f1xx_hal.h"

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
#define Run_Pin GPIO_PIN_13
#define Run_GPIO_Port GPIOC
#define Error_Pin GPIO_PIN_14
#define Error_GPIO_Port GPIOC
#define Flash_Pin GPIO_PIN_15
#define Flash_GPIO_Port GPIOC
#define Buzzer_Pin GPIO_PIN_0
#define Buzzer_GPIO_Port GPIOA
#define SW_Blu_Pin GPIO_PIN_1
#define SW_Blu_GPIO_Port GPIOA
#define BUTTON_Pin GPIO_PIN_2
#define BUTTON_GPIO_Port GPIOA
#define LORA_RST_Pin GPIO_PIN_3
#define LORA_RST_GPIO_Port GPIOA
#define LORA_NSS_Pin GPIO_PIN_4
#define LORA_NSS_GPIO_Port GPIOA
#define LORA_SCK_Pin GPIO_PIN_5
#define LORA_SCK_GPIO_Port GPIOA
#define LORA_MISO_Pin GPIO_PIN_6
#define LORA_MISO_GPIO_Port GPIOA
#define LORA_MOSI_Pin GPIO_PIN_7
#define LORA_MOSI_GPIO_Port GPIOA
#define LORA_INT_Pin GPIO_PIN_0
#define LORA_INT_GPIO_Port GPIOB
#define LORA_INT_EXTI_IRQn EXTI0_IRQn
#define RESET_Pin GPIO_PIN_1
#define RESET_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define LCD_TX_Pin GPIO_PIN_10
#define LCD_TX_GPIO_Port GPIOB
#define LCD_RX_Pin GPIO_PIN_11
#define LCD_RX_GPIO_Port GPIOB
#define SIM_PWRKEY_Pin GPIO_PIN_12
#define SIM_PWRKEY_GPIO_Port GPIOB
#define GPS_EN_Pin GPIO_PIN_13
#define GPS_EN_GPIO_Port GPIOB
#define USART1_DTR_Pin GPIO_PIN_14
#define USART1_DTR_GPIO_Port GPIOB
#define USART1_RI_Pin GPIO_PIN_15
#define USART1_RI_GPIO_Port GPIOB
#define USART1_DCD_Pin GPIO_PIN_8
#define USART1_DCD_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
