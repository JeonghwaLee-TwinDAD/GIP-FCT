/**
  ******************************************************************************
  * @file           : g3_plc_rf_task.c
  * @brief          : G3-PLC+RF (Hybrid) task
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
#include "g3_hif.h"
#include "main.h" //todo: for led blinking make it in the default task

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
#define G3__HIF_MSG_QUEUE_OBJECTS 10    // number of Message Queue Objects
osMessageQueueId_t g3_hif_msg_queue;    // message queue id for the G3 host interface

g3_hi_msg_t rx_msg;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
/**
* @brief Function initializing the g3_task thread internal data.
* @retval None
*/
void init_g3_task(void)
{
    g3_hif_msg_queue = osMessageQueueNew(G3__HIF_MSG_QUEUE_OBJECTS, sizeof(g3_hi_msg_t), NULL);

    g3_hif_uart_init();
}

/**
* @brief Function implementing the g3_task thread.
* @param argument: Not used
* @retval None
*/
void start_g3_task(void *argument)
{
    osStatus_t status;

    init_g3_task();

    /* Infinite loop */
    for(;;)
    {
        osDelay(100*osKernelGetTickFreq()/1000); //millisec
        HAL_GPIO_TogglePin(GPIOC, LED1_Pin);
        status = osMessageQueueGet(g3_hif_msg_queue, &rx_msg, NULL, 0U);   // wait for message
        //status = osMessageQueueGet(g3_hif_msg_queue, &rx_msg, NULL, osWaitForever);   // wait for message with infinite delay
        if (status == osOK) {
            HAL_GPIO_TogglePin(GPIOC, LED2_Pin);
        }
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
