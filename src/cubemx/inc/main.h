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
#define BOARD_BUTTON_Pin GPIO_PIN_0
#define BOARD_BUTTON_GPIO_Port GPIOA
#define BOARD_BUTTON_EXTI_IRQn EXTI0_IRQn
#define USART_PA2_TX_Pin GPIO_PIN_2
#define USART_PA2_TX_GPIO_Port GPIOA
#define USART_PA3_RX_Pin GPIO_PIN_3
#define USART_PA3_RX_GPIO_Port GPIOA
#define RELAY8_Pin GPIO_PIN_4
#define RELAY8_GPIO_Port GPIOA
#define RELAY7_Pin GPIO_PIN_5
#define RELAY7_GPIO_Port GPIOA
#define RELAY6_Pin GPIO_PIN_6
#define RELAY6_GPIO_Port GPIOA
#define RELAY5_Pin GPIO_PIN_7
#define RELAY5_GPIO_Port GPIOA
#define RELAY4_Pin GPIO_PIN_0
#define RELAY4_GPIO_Port GPIOB
#define RELAY3_Pin GPIO_PIN_1
#define RELAY3_GPIO_Port GPIOB
#define RELAY2_Pin GPIO_PIN_2
#define RELAY2_GPIO_Port GPIOB
#define RELAY1_Pin GPIO_PIN_10
#define RELAY1_GPIO_Port GPIOB
#define SWITCH9_Pin GPIO_PIN_12
#define SWITCH9_GPIO_Port GPIOB
#define SWITCH10_Pin GPIO_PIN_13
#define SWITCH10_GPIO_Port GPIOB
#define SWITCH11_Pin GPIO_PIN_14
#define SWITCH11_GPIO_Port GPIOB
#define SWITCH12_Pin GPIO_PIN_15
#define SWITCH12_GPIO_Port GPIOB
#define SWITCH1_Pin GPIO_PIN_15
#define SWITCH1_GPIO_Port GPIOA
#define SWITCH2_Pin GPIO_PIN_3
#define SWITCH2_GPIO_Port GPIOB
#define SWITCH3_Pin GPIO_PIN_4
#define SWITCH3_GPIO_Port GPIOB
#define SWITCH4_Pin GPIO_PIN_5
#define SWITCH4_GPIO_Port GPIOB
#define SWITCH5_Pin GPIO_PIN_6
#define SWITCH5_GPIO_Port GPIOB
#define SWITCH6_Pin GPIO_PIN_7
#define SWITCH6_GPIO_Port GPIOB
#define SWITCH7_Pin GPIO_PIN_8
#define SWITCH7_GPIO_Port GPIOB
#define SWITCH8_Pin GPIO_PIN_9
#define SWITCH8_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
