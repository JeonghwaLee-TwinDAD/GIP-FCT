/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32g0xx_hal.h"

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
#define BOOT2_Pin GPIO_PIN_1
#define BOOT2_GPIO_Port GPIOF
#define ST8500_BOOT_R_Pin GPIO_PIN_2
#define ST8500_BOOT_R_GPIO_Port GPIOC
#define STM32_MODE_Pin GPIO_PIN_3
#define STM32_MODE_GPIO_Port GPIOC
#define HOST_TX_Pin GPIO_PIN_2
#define HOST_TX_GPIO_Port GPIOA
#define HOST_RX_Pin GPIO_PIN_3
#define HOST_RX_GPIO_Port GPIOA
#define BOOT0_Pin GPIO_PIN_1
#define BOOT0_GPIO_Port GPIOB
#define ST8500_LPMODE_Pin GPIO_PIN_11
#define ST8500_LPMODE_GPIO_Port GPIOB
#define PLC_RST_Pin GPIO_PIN_12
#define PLC_RST_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_8
#define LED3_GPIO_Port GPIOA
#define PLC_TX_Pin GPIO_PIN_9
#define PLC_TX_GPIO_Port GPIOA
#define LED0_Pin GPIO_PIN_6
#define LED0_GPIO_Port GPIOC
#define PLC_RX_Pin GPIO_PIN_10
#define PLC_RX_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_8
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_9
#define LED2_GPIO_Port GPIOC
#define ST8500_BOOT_W_Pin GPIO_PIN_6
#define ST8500_BOOT_W_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* STM32 Fw version in the vMAJOR.MINOR.SUB format */
#define FW_VERSION_MAJOR                      (1U)
#define FW_VERSION_MINOR                      (3U)
#define FW_VERSION_SUB                        (0U)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
