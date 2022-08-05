/**
  ******************************************************************************
  * @file    g3_app_boot.c
  * @author  AMG/IPC Application Team
  * @brief   Boot implementation
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

#include <g3_app_boot.h>
#include <g3_app_boot_tbl.h>
#include <hi_boot_sap_interface.h>
#include <hi_msgs_impl.h>
#include <hif_g3_common.h>
#include <g3_hif.h>

static task_comm_info_t *g3_task_comm_info;

static void g3_app_boot_handle_srv_getpsk_ind(const void *msg)
{
    const uint8_t default_psk[] = DEFAULT_PSK;
    static BOOT_ServerSetPSKRequest_t setpsk_req;
    const g3_app_boot_data_t *boot_data;
    uint16_t len;
    const BOOT_ServerGetPSKIndication_t *getpsk_ind = msg;

    boot_data = g3_app_boot_tbl_find_elem(getpsk_ind->ExtendedAddress);

    if (!boot_data) {
        boot_data = g3_app_boot_temp_tbl_find_elem(getpsk_ind->ExtendedAddress);

        if (!boot_data)
            g3_app_boot_data_tbl_store_elem(g3_app_boot_get_next_short_addr(), getpsk_ind->ExtendedAddress,
                                            default_psk);

        boot_data = g3_app_boot_temp_tbl_find_elem(getpsk_ind->ExtendedAddress);

        if (!boot_data)
            return;
    }

    len = hi_boot_srv_setpsk_req_set(&setpsk_req, boot_data->ext_addr,
                                         boot_data->psk, boot_data->short_addr);

//    hif_uart_send_message(HIF_BOOT_SERVER_SETPSK_REQ, &setpsk_req, len);

    task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_BOOT_SERVER_SETPSK_REQ, &setpsk_req, len, NO_WAIT);
}

static void g3_app_boot_handle_srv_setpsk_cnf(const void *msg)
{
    // Should we retry here, if the operation failed?
}

bool g3_app_boot_msg_needed(const msg_t *msg)
{
    switch (msg->cmd_id) {
    case HIF_BOOT_SERVER_GETPSK_IND:
    case HIF_BOOT_SERVER_SETPSK_CNF:
        return true;
    }

    return false;
}

void g3_app_boot_msg_handler(uint8_t cmd_id, const void *msg)
{
    switch (cmd_id) {
    case HIF_BOOT_SERVER_GETPSK_IND:
        g3_app_boot_handle_srv_getpsk_ind(msg);
        break;
    case HIF_BOOT_SERVER_SETPSK_CNF:
        g3_app_boot_handle_srv_setpsk_cnf(msg);
        break;
    default:
        break;
    }
}

void g3_app_boot_init(task_comm_info_t *g3_task_comm_info_)
{
    g3_task_comm_info = g3_task_comm_info_;
}
