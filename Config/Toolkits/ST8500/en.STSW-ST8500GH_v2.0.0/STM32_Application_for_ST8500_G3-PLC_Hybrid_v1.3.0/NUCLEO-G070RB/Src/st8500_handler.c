/**
  ******************************************************************************
  * File Name          : st8500_handler.c
  * Description        : Handler functions of ST8500
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

/* Includes ------------------------------------------------------------------*/
#include "st8500_handler.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private application code --------------------------------------------------*/
/**
  * @brief  This function get the STM32 boot mode
  * @retval bootmode (default or pass through)
  */
stm32_boot_mode_t STM32_GetBootMode(void)
{
  stm32_boot_mode_t bootmode;

  bootmode = (stm32_boot_mode_t)HAL_GPIO_ReadPin(STM32_MODE_GPIO_Port, STM32_MODE_Pin);

  return bootmode;
}

/**
  * @brief  This function send the ST8500 boot mode
  * @retval bootmode (boot from Uart or boot from attached sflash)
  */
st8500_boot_mode_t ST8500_GetBootMode(void)
{
  st8500_boot_mode_t bootmode;

  bootmode = (st8500_boot_mode_t)HAL_GPIO_ReadPin(ST8500_BOOT_R_GPIO_Port, ST8500_BOOT_R_Pin);

  return bootmode;
}

/**
  * @brief  This function asserts the ST8500 reset pin
  * @retval None
  */
void ST8500_assert_reset(void)
{
  HAL_GPIO_WritePin(PLC_RST_GPIO_Port, PLC_RST_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(ST8500_LPMODE_GPIO_Port, ST8500_LPMODE_Pin, GPIO_PIN_RESET);
}

/**
  * @brief  This function releases the ST8500 reset pin
  * @retval None
  */
void ST8500_release_reset(void)
{
  HAL_GPIO_WritePin(PLC_RST_GPIO_Port, PLC_RST_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ST8500_LPMODE_GPIO_Port, ST8500_LPMODE_Pin, GPIO_PIN_SET);
}

/**
  * @brief  This function activate the pass though
  * @retval None
  */
void STM32_pass_through_activate(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // Disable all uarts
    // disable USART1
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_TC);
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
    HAL_NVIC_DisableIRQ(USART1_IRQn);

    // disable USART2
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_TC);
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
    HAL_NVIC_DisableIRQ(USART2_IRQn);

    // Set UART1 IOs in GPIO mode
    // RX line PA10 in input
    // TX line PA9 in output
    GPIO_InitStruct.Pin = PLC_RX_Pin;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    HAL_GPIO_Init(PLC_RX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PLC_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(PLC_TX_GPIO_Port, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);

    HAL_GPIO_EXTI_IRQHandler(PLC_RX_Pin);

    HAL_NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

    // Set UART3 IOs in GPIO mode
    // RX line PA3 in input
    // TX line PA2 in output
    GPIO_InitStruct.Pin = HOST_RX_Pin;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    HAL_GPIO_Init(HOST_RX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = HOST_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(HOST_TX_GPIO_Port, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);

    HAL_GPIO_EXTI_IRQHandler(HOST_RX_Pin);

    HAL_NVIC_ClearPendingIRQ(EXTI2_3_IRQn);
    HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

    HAL_GPIO_WritePin(HOST_TX_GPIO_Port, HOST_TX_Pin, HAL_GPIO_ReadPin(PLC_RX_GPIO_Port, PLC_RX_Pin));
    HAL_GPIO_WritePin(PLC_TX_GPIO_Port, PLC_TX_Pin, HAL_GPIO_ReadPin(HOST_RX_GPIO_Port, HOST_RX_Pin));
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
