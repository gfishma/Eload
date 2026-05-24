/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#define PE5_GPIO_Port GPIOE
#define PE6_Pin GPIO_PIN_6
#define PE6_GPIO_Port GPIOE
#define LCD_PWR_Pin GPIO_PIN_0
#define LCD_PWR_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_1
#define LCD_RST_GPIO_Port GPIOC
#define IN_PWM1_Pin GPIO_PIN_0
#define IN_PWM1_GPIO_Port GPIOA
#define IN_PWM2_Pin GPIO_PIN_2
#define IN_PWM2_GPIO_Port GPIOA
#define OUT_PWM2_Pin GPIO_PIN_6
#define OUT_PWM2_GPIO_Port GPIOA
#define LCD_CS_Pin GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOB
#define IN_PWM3_Pin GPIO_PIN_6
#define IN_PWM3_GPIO_Port GPIOC
#define IN_PWM4_Pin GPIO_PIN_8
#define IN_PWM4_GPIO_Port GPIOC
#define LCD_WR_Pin GPIO_PIN_11
#define LCD_WR_GPIO_Port GPIOC
#define OUT_PWM1_Pin GPIO_PIN_9
#define OUT_PWM1_GPIO_Port GPIOB
#define LED_B_Pin GPIO_PIN_0
#define LED_B_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
#define  PE1     HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_1)? 0:1
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
