/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "st8500_handler.h"
#include <g3_task.h>
#include <g3_app_config.h>
#include <user_g3_common.h>
#include <user_task.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SEND_QUEUE_MAX_ELEMS    1U
#define RECV_QUEUE_MAX_ELEMS    4U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
task_comm_info_t g3_task_comm_info;
/* USER CODE END Variables */
/* Definitions for default_task */
osThreadId_t default_taskHandle;
const osThreadAttr_t default_task_attributes = {
  .name = "default_task",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for g3_task */
osThreadId_t g3_taskHandle;
const osThreadAttr_t g3_task_attributes = {
  .name = "g3_task",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for user_task */
osThreadId_t user_taskHandle;
const osThreadAttr_t user_task_attributes = {
  .name = "user_task",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for send_op_begin_queue */
osMessageQueueId_t send_op_begin_queue;
const osMessageQueueAttr_t send_op_begin_queue_attributes = {
  .name = "send_op_begin_queue"
};
/* Definitions for recv_op_begin_queue */
osMessageQueueId_t recv_op_begin_queue;
const osMessageQueueAttr_t recv_op_begin_queue_attributes = {
  .name = "recv_op_begin_queue"
};
/* Definitions for g3_to_user_queue */
osMessageQueueId_t g3_to_user_queueHandle;
const osMessageQueueAttr_t g3_to_user_queue_attributes = {
  .name = "g3_to_user_queue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

static void show_device_type(void)
{
#ifdef DEVICE_TYPE_DEVICE
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
#elif defined(DEVICE_TYPE_COORD)
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
#endif
}

/* USER CODE END FunctionPrototypes */

void start_default_task(void *argument);
void start_g3_task(void *argument);
void user_task_run(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of send_op_begin_queue */
  g3_task_comm_info.send_op_begin_queue = osMessageQueueNew (2, sizeof(task_comm_msg_t), &send_op_begin_queue_attributes);

  /* creation of recv_op_begin_queue */
  g3_task_comm_info.recv_op_begin_queue = osMessageQueueNew (4, sizeof(task_comm_msg_t), &recv_op_begin_queue_attributes);

  /* creation of g3_to_user_queue */
  g3_to_user_queueHandle = osMessageQueueNew (4, sizeof(g3_user_msg_t), &g3_to_user_queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of default_task */
  default_taskHandle = osThreadNew(start_default_task, NULL, &default_task_attributes);

  /* creation of g3_task */
  g3_taskHandle = osThreadNew(start_g3_task, NULL, &g3_task_attributes);

  /* creation of user_task */
  user_taskHandle = osThreadNew(user_task_run, NULL, &user_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_start_default_task */
/**
  * @brief  Function implementing the default_task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_start_default_task */
void start_default_task(void *argument)
{
  /* USER CODE BEGIN start_default_task */
  uint32_t tick= osKernelGetSysTimerCount();
  uint32_t timeout = 0;

  if (ST8500_GetBootMode() == ST8500_BOOT_FROM_UART) {
    HAL_GPIO_WritePin(ST8500_BOOT_W_GPIO_Port, ST8500_BOOT_W_Pin, GPIO_PIN_RESET);
  } else {
    HAL_GPIO_WritePin(ST8500_BOOT_W_GPIO_Port, ST8500_BOOT_W_Pin, GPIO_PIN_SET);
  }

  show_device_type();

  osDelay(10*osKernelGetTickFreq()/1000); //just delay for 100 millisec
  
  /* LED blinking every 500 ms == 500 000 usec */
  timeout = 500000U * (osKernelGetSysTimerFreq() / 1000000u);

  /* Infinite loop */
  for(;;)
  {
    if((osKernelGetSysTimerCount() - tick) >= timeout)
    {
        HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
        tick = osKernelGetSysTimerCount();
    }
  }
  /* USER CODE END start_default_task */
}

/* USER CODE BEGIN Header_start_g3_task */
/**
* @brief Function implementing the g3_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_g3_task */
void start_g3_task(void *argument)
{
  /* USER CODE BEGIN start_g3_task */
 /*  g3_task context initialization */
  task_comm_init(&g3_task_comm_info, SEND_QUEUE_MAX_ELEMS, RECV_QUEUE_MAX_ELEMS);
  g3_task_init(); 

  g3_app_task();
  /* USER CODE END start_g3_task */
}

/* USER CODE BEGIN Header_user_task_run */
/**
* @brief Function implementing the user_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_user_task_run */
void user_task_run(void *argument)
{
  /* USER CODE BEGIN user_task_run */

  /*  g3_task context initialization */
  USER_TaskInit(&g3_task_comm_info);
  
  USER_AppTask();
  /* USER CODE END user_task_run */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
