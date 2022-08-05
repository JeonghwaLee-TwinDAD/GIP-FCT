/**
  ******************************************************************************
  * @file    g3_app_pansort.c
  * @author  AMG/IPC Application Team
  * @brief   Pansort implementation
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

#include <g3_app_pansort.h>
#include <hi_boot_sap_interface.h>
#include <hi_msgs_impl.h>
#include <hif_g3_common.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static task_comm_info_t *g3_task_comm_info;

// We return the values with inverted signs, since we want to invert the sort
static int pan_descr_compare(const void *a_, const void *b_)
{
    const ADP_PanDescriptor_t *a = a_;
    const ADP_PanDescriptor_t *b = b_;

    if (a->RouteCost > b->RouteCost)
        return -1;
    else if (a->RouteCost < b->RouteCost)
        return 1;
    else
        return 0;
}

static void g3_app_pansort_handle_pansort_ind(const void *msg)
{
    uint16_t len;
    static BOOT_DevicePANSortRequest_t pan_sort_req;
    const BOOT_DevicePANSortIndication_t *pansort_ind = msg;

    len = hi_boot_dev_pansort_req_set(&pan_sort_req, pansort_ind->PANCount, pansort_ind->PANDescriptor);

    if (!pan_sort_req.PANCount)
        return;

    qsort(pan_sort_req.PANDescriptor, pan_sort_req.PANCount, sizeof(pan_sort_req.PANDescriptor[0]),
          pan_descr_compare);

    task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_BOOT_DEVICE_PANSORT_REQ, &pan_sort_req, len, NO_WAIT);
}

static void g3_app_pansort_handle_pansort_cnf(const void *msg)
{
#ifndef NDEBUG
    const BOOT_DevicePANSortConfirm_t *pansort_conf = msg;
#endif /* NDEBUG */
    assert(!pansort_conf->Status);
}

bool g3_app_pansort_msg_needed(const msg_t *msg)
{
    switch (msg->cmd_id) {
    case HIF_BOOT_DEVICE_PANSORT_IND:
    case HIF_BOOT_DEVICE_PANSORT_CNF:
        return true;
    }

    return false;
}

void g3_app_pansort_msg_handler(uint8_t cmd_id, const void *msg)
{
    switch (cmd_id) {
    case HIF_BOOT_DEVICE_PANSORT_IND:
        g3_app_pansort_handle_pansort_ind(msg);
        break;
    case HIF_BOOT_DEVICE_PANSORT_CNF:
        g3_app_pansort_handle_pansort_cnf(msg);
        break;
    default:
        break;
    }
}

void g3_app_pansort_init(task_comm_info_t *g3_task_comm_info_)
{
    g3_task_comm_info = g3_task_comm_info_;
}
