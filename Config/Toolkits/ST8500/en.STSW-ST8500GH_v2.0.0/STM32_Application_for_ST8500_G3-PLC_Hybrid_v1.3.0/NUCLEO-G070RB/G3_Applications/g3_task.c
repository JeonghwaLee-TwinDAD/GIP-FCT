/**
  ******************************************************************************
  * @file    g3_task.c
  * @author  AMG/IPC Application Team
  * @brief   This file contains code that implements the g3 task
  @verbatim
  @endverbatim

  ******************************************************************************
  * @attention
  *
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2021 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include <string.h>
#include <hi_msgs_impl.h>
#include <hi_ip_sap_interface.h>
#include <g3_task.h>
#include <timed_events.h>
#include <stdbool.h>
#include <g3_app_keep_alive.h>
#include <soft_timer.h>
#include <g3_app_config.h>
#include <g3_app_boot.h>
#include <g3_app_pansort.h>
#include <g3_app_boot_tbl.h>
#include <g3_hif.h>
#include <user_task.h>
#include <task_comm.h>
#include <cmsis_os2.h>
#include <hif_g3_common.h>
#include <st8500_handler.h>
#include <assert.h>
#include <mem_pool.h>

// #define G3_APP_WAIT_FOR_MSG_DELAY  1U
static inline void g3_send_msg_to_user_task(uint8_t cmd_id, void * buff, uint8_t size, void * hif_msg_handle);

void g3_task_init(void)
{
    st_timer_init();
    timed_events_init();
    g3_hif_uart_init(&g3_task_comm_info);

    g3_app_conf_init(&g3_task_comm_info);
    g3_app_pansort_init(&g3_task_comm_info);
    g3_app_boot_init(&g3_task_comm_info);
    g3_app_boot_tbl_init();
    g3_app_keep_alive_init(&g3_task_comm_info);

}

void g3_app_start(void)
{
    msg_t msg;

    memset(&msg, 0, sizeof(msg));

    task_comm_put_recv_buffer(&g3_task_comm_info, G3_INIT, 0, NULL, 0, NO_WAIT);
}

static void g3_app_msg_handler(const msg_t *msg)
{
    bool forward_msg_to_user_task = false;
    const void *msg_payload = hif_rx_get_msg_payload(msg->buff, NULL);

    if (g3_app_conf_msg_needed(msg)) {
        g3_app_conf_msg_handler(msg->cmd_id, msg_payload);
        forward_msg_to_user_task = true;
    }

    if (g3_app_pansort_msg_needed(msg)) {
        g3_app_pansort_msg_handler(msg->cmd_id, msg_payload);
        forward_msg_to_user_task = true;
    }

    if (g3_app_boot_msg_needed(msg)) {
        g3_app_boot_msg_handler(msg->cmd_id, msg_payload);
        forward_msg_to_user_task = true;
    }

    if (g3_app_ka_msg_needed(msg)) {
        g3_app_ka_msg_handler(msg->cmd_id, msg_payload);
        forward_msg_to_user_task = true;
    }

    if (!forward_msg_to_user_task) {
        /* Msg now consumed by G3 App task and no forward needed:
           Msg data content stored in MemPool can now be freed    */
        assert(msg->buff != NULL);
        MEM_FREE(msg->buff);
    } else {
        /* Msg now forwarded to User task. Its content is thus still
           needed: then defer MemPool free to User Task           */
        g3_send_msg_to_user_task(msg->cmd_id, (void *)msg_payload, sizeof(msg_payload), msg->buff);
    }
}

void g3_app_task(void)
{
    msg_t msg, tx_msg;

    ST8500_release_reset();

    do {
        msg.cmd_id = 0xFF;

        task_comm_get_recv_buffer(&g3_task_comm_info, &msg, 1);

    } while (msg.cmd_id != HIF_HI_HWRESET_CNF);

    g3_app_conf_start();

    while (1) {

        if (hif_uart_is_free() && task_comm_get_send_buffer(&g3_task_comm_info, &tx_msg, NO_WAIT)) {
            if (!hif_uart_send_message(tx_msg.cmd_id, tx_msg.buff, tx_msg.size)) {
                task_comm_put_send_buffer(&g3_task_comm_info, G3_PROT, tx_msg.cmd_id, tx_msg.buff, tx_msg.size, NO_WAIT);
            }
        }

        timed_events_exec();

        if (task_comm_get_recv_buffer(&g3_task_comm_info, &msg, NO_WAIT)) {
            g3_app_msg_handler(&msg);
        }

        g3_app_conf();
        g3_app_keep_alive();
    }
}

task_comm_info_t *g3_app_get_task_comm(void)
{
    return &g3_task_comm_info;
}

static inline void g3_send_msg_to_user_task(uint8_t cmd_id, void * buff, uint8_t size, void * hif_msg_handle)
{
  g3_user_msg_t msg;

  msg.cmd_id = cmd_id;
  msg.buff = buff;
  msg.size = size;
  msg.hif_msg_handle = hif_msg_handle;

  UserG3_SendMessage(g3_to_user_queueHandle, &msg, NO_WAIT);
}

