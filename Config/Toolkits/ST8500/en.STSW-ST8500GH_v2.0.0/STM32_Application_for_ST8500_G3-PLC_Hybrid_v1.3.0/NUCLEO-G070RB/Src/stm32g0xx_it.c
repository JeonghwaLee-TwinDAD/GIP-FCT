/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g0xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <g3_hif.h>
#include <stm32g0xx_ll_usart.h> /* if not already, file has to be added in corresponding \Drivers\STM32Yyxx_HAL_Driver */
#include <user_term_rxtx.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim6;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_IRQn 0 */

  /* USER CODE END TIM6_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_IRQn 1 */

  /* USER CODE END TIM6_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE)) {
        g3_hif_uart_rx_handler(LL_USART_ReceiveData8(USART1));
    }

    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC)) {
        uint8_t tx_data;

        if (g3_hif_uart_tx_handler(&tx_data) == false) {
            /* For STM32 F4: __HAL_UART_CLEAR_FLAG(&huart1, UART_CLEAR_TC); RNS team*/
            __HAL_UART_CLEAR_FLAG(&huart1, UART_CLEAR_TCF);
        } else {
            LL_USART_TransmitData8(USART1, tx_data);
        }
    }
  /* USER CODE END USART1_IRQn 0 */
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
  User_Term_UsartIsr();

  /* USER CODE END USART2_IRQn 0 */
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE BEGIN 1 */

void EXTI2_3_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(HOST_RX_Pin) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(HOST_RX_Pin);

        if ((HOST_RX_GPIO_Port->IDR & HOST_RX_Pin) != GPIO_PIN_RESET) {
            PLC_TX_GPIO_Port->BSRR = PLC_TX_Pin;
        } else {
            PLC_TX_GPIO_Port->BSRR = PLC_TX_Pin << 16U;
        }
    }
}

void EXTI4_15_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(PLC_RX_Pin) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(PLC_RX_Pin);

        if ((PLC_RX_GPIO_Port->IDR & PLC_RX_Pin) != GPIO_PIN_RESET) {
            HOST_TX_GPIO_Port->BSRR = HOST_TX_Pin;
        } else {
            HOST_TX_GPIO_Port->BSRR = HOST_TX_Pin << 16U;
        }
    }
}

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
