/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32wbxx_hal.h"
#include "app_conf.h"
#include "app_entry.h"
#include "app_common.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bmi270.h"
#include "bme280.h"
#include "gps.h"
#include "flash.h"
#include "sensordata.h"
#include "oled_display.h"
#include "usbd_cdc_if.h"
#include "usbd_cdc.h"
#include <string.h>
#include "custom_stm.h"
#include "custom_app.h"
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
#define BAT_Pin GPIO_PIN_0
#define BAT_GPIO_Port GPIOA
#define BTN_YELLOW_Pin GPIO_PIN_4
#define BTN_YELLOW_GPIO_Port GPIOA
#define BTN_YELLOW_EXTI_IRQn EXTI4_IRQn
#define BTN_BLUE_Pin GPIO_PIN_5
#define BTN_BLUE_GPIO_Port GPIOA
#define BTN_BLUE_EXTI_IRQn EXTI9_5_IRQn
#define BTN_1_Pin GPIO_PIN_2
#define BTN_1_GPIO_Port GPIOB
#define BTN_1_EXTI_IRQn EXTI2_IRQn
#define LED_R_Pin GPIO_PIN_0
#define LED_R_GPIO_Port GPIOB
#define LED_G_Pin GPIO_PIN_1
#define LED_G_GPIO_Port GPIOB
#define LED_B_Pin GPIO_PIN_4
#define LED_B_GPIO_Port GPIOB
#define GNSS_RX_Pin GPIO_PIN_5
#define GNSS_RX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
void sendBLE();
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
