/**
  ******************************************************************************
  * @file           : task_comm.h
  * @brief          : Task Communication header
  *
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

#ifndef TASK_COMM_H_
#define TASK_COMM_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <g3_msg_id.h>
#include <cmsis_os2.h>

extern osMessageQueueId_t send_op_begin_queue;
extern osMessageQueueId_t recv_op_begin_queue;

typedef struct {
    osMessageQueueId_t send_op_begin_queue;
    osMessageQueueId_t recv_op_begin_queue;
    uint16_t send_max_elems;
    uint16_t recv_max_elems;
} task_comm_info_t;

#define NO_WAIT         0U
#define WAIT_FOREVER    osWaitForever

typedef struct {
    g3_msg_ids_t msg_id;
    uint8_t cmd_id;
    void *buff;
    size_t size;
} task_comm_msg_t;

typedef task_comm_msg_t msg_t;

void task_comm_init(task_comm_info_t *task_comm_info, uint16_t send_max_elems, uint16_t recv_max_elems);

bool task_comm_put_send_buffer(task_comm_info_t *task_comm_info, g3_msg_ids_t msg_id, uint8_t cmd_id,
                               void * const buff, size_t size, uint32_t wait_time);
bool task_comm_get_send_buffer(task_comm_info_t *task_comm_info, task_comm_msg_t *msg, uint32_t wait_time);

bool task_comm_put_recv_buffer(task_comm_info_t *task_comm_info, g3_msg_ids_t msg_id, uint8_t cmd_id,
                               void * const buff, size_t size, uint32_t wait_time);
bool task_comm_get_recv_buffer(task_comm_info_t *task_comm_info, task_comm_msg_t *msg, uint32_t wait_time);

#endif
