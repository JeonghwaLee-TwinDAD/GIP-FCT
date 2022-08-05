/**
  ******************************************************************************
  * @file           : st8500_handler.h
  * @brief          : Header for st8500_handler.c file.
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
#ifndef __ST8500HANDLER_H
#define __ST8500HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef enum {
  STM32_BOOT_DEFAULT = 0,
  STM32_BOOT_PASSTHROUGH
} stm32_boot_mode_t;

typedef enum {
  ST8500_BOOT_FROM_UART = 0,
  ST8500_BOOT_FROM_FLASH
} st8500_boot_mode_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
stm32_boot_mode_t STM32_GetBootMode(void);
st8500_boot_mode_t ST8500_GetBootMode(void);
void ST8500_assert_reset(void);
void ST8500_release_reset(void);
void STM32_pass_through_activate(void);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __ST8500HANDLER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
