/**
  ******************************************************************************
  * @file    g3_app_config.c
  * @author  AMG/IPC Application Team
  * @brief   This file contains the implementation of g3 config functionalities
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
  
#include <g3_app_config.h>
#include <task_comm.h>
#include <hi_msgs_impl.h>
#include <g3_app_keep_alive.h>
#include <g3_app_attrib_tbl.h>
#include <hi_g3lib_sap_interface.h>
#include <hif_g3_common.h>
#include <g3_hif.h>
#include <utils.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define PROT_CENA_BANDPLAN  0U

typedef enum {
    CONF_EV_NONE,
    CONF_EV_RECEIVED_SWRESET_CNF,
    CONF_EV_RECEIVED_RFCONFIGSET_CNF,
    CONF_EV_RECEIVED_SRVSTOP_CNF,
    CONF_EV_RECEIVED_SRVSTART_CNF,
    CONF_EV_RECEIVED_G3LIBSET_CNF,
    CONF_EV_CNT
} conf_event_t;

typedef enum {
    CONF_ST_WAIT_FOR_SWRESET_CNF,
    CONF_ST_WAIT_FOR_RFCONFIGSET_CNF,
    CONF_ST_WAIT_FOR_DEFAULT_SRVSTART_CNF,
    CONF_ST_WAIT_FOR_SRVSTOP_CNF,
    CONF_ST_WAIT_FOR_SRVSTART_CNF,
    CONF_ST_WAIT_FOR_G3LIBSET_CNF,
    CONF_ST_CNT
} conf_state_t;

static task_comm_info_t *g3_task_comm_info;

static struct {
    conf_state_t curr_state;
    conf_event_t curr_event;
    uint16_t g3lib_set_evts_cnt;
} g3_conf_fsm_info;


static bool g3_conf_set_attr(void)
{
    static G3_LIB_SetAttributeRequest_t attr_data;
    bool ret = g3_app_attrib_tbl_get_next_data(&attr_data.Attribute);
    uint16_t len;

    if (ret) {
        g3_conf_fsm_info.g3lib_set_evts_cnt++;

        len = sizeof(attr_data.Attribute) - sizeof(attr_data.Attribute.Value) + attr_data.Attribute.Len;

//        hif_uart_send_message(HIF_G3LIB_SET_REQ, &attr_data, len);

        task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_G3LIB_SET_REQ, &attr_data, len, NO_WAIT);
    }

    return ret;
}

static conf_state_t g3_conf_fsm_default_fnc(void)
{
    g3_conf_fsm_info.curr_event = CONF_EV_NONE;

    return g3_conf_fsm_info.curr_state;
}

static conf_state_t g3_conf_fsm_handle_swreset_cnf(void)
{
    static hif_rfconfigset_req_t rfconfig_req;

    g3_conf_fsm_info.curr_event = CONF_EV_NONE;

    rfconfig_req.data = (s2lp_configdata_t) {
#ifdef RF_IS_868
        .RadioBaseFreq = 863100000U,
        .RadioModSelect = 0x00U,
        .RadioDataRate = 50000U,
        .RadioFreqDeviation = 12500U,
        .RadioBandwidth = 100000U,
        .RadioCsBlanking = 0x01U,
        .RadioXtalFreq = 50000000U,
        .RadioRssiGain = 14U,
        .PowerdBm = 35U,
        .PktCRCMode = 0xA0U,
        .PktEnFEC = 0x00U,
        .PktEnWhitening = 0x01U,
        .PktEnInterleaving = 0x01U,
        .IrqGpioPin = 0x00U,
        .MCUClkEnable = 0x00U,
        .MCUClkGpioPin = 0xFFU,
        .MCUClkXORatio = 0xFFU,
        .MCUClkRCORatio = 0xFFU,
        .MCUClkClockCycles = 0xFFU,
        .ExtSmpsEnable = 0x00U,
        .FEMEnabled = 0x00U,
        .FEMGpioPinCSD = 0xFFU,
        .FEMGpioPinCPS = 0xFFU,
        .FEMGpioPinCTX = 0xFFU,
        .FEMTxBypassEn = 0x00U
#else /* RF_IS_915 */
        .RadioBaseFreq = 915000000U,
        .RadioModSelect = 0x00U,
        .RadioDataRate = 50000U,
        .RadioFreqDeviation = 12500U,
        .RadioBandwidth = 100000U,
        .RadioCsBlanking = 0x01U,
        .RadioXtalFreq = 50000000U,
        .RadioRssiGain = 14U,
        .PowerdBm = 45U,                        /* Modified for 915 */
        .PktCRCMode = 0xA0U,
        .PktEnFEC = 0x00U,
        .PktEnWhitening = 0x01U,
        .PktEnInterleaving = 0x01U,
        .IrqGpioPin = 0x03U,                    /* Modified for 915 */
        .MCUClkEnable = 0x00U,
        .MCUClkGpioPin = 0xFFU,
        .MCUClkXORatio = 0xFFU,
        .MCUClkRCORatio = 0xFFU,
        .MCUClkClockCycles = 0xFFU,
        .ExtSmpsEnable = 0x00U,
        .FEMEnabled = 0x01U,                    /* Modified for 915 */
        .FEMGpioPinCSD = 0x00U,                 /* Modified for 915 */
        .FEMGpioPinCPS = 0x01U,                 /* Modified for 915 */
        .FEMGpioPinCTX = 0x02U,                 /* Modified for 915 */
        .FEMTxBypassEn = 0x00U
#endif
    };

    task_comm_put_send_buffer(g3_task_comm_info, G3_PROT,
                              HIF_HI_RFCONFIGSET_REQ, &rfconfig_req,
                              sizeof(rfconfig_req), NO_WAIT);

    return CONF_ST_WAIT_FOR_RFCONFIGSET_CNF;
}

static conf_state_t g3_conf_fsm_handle_rfconfigset_cnf(void)
{
    bool g3libset_conf_expected;

    g3_conf_fsm_info.curr_event = CONF_EV_NONE;

    if (DEV_TYPE_COORD == dev_type)
        return CONF_ST_WAIT_FOR_DEFAULT_SRVSTART_CNF;

    g3libset_conf_expected = g3_conf_set_attr();

    if (g3libset_conf_expected)
        return CONF_ST_WAIT_FOR_G3LIBSET_CNF;

    g3_app_keep_alive_start();

    return CONF_ST_WAIT_FOR_SWRESET_CNF;
}

static conf_state_t g3_conf_fsm_handle_default_srvstart_cnf(void)
{
    // we use a fake buffer address here to be able to check for errors (buff == NULL)
    task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_BOOT_SERVER_STOP_REQ, utils_get_fake_buff_addr(), 0, NO_WAIT);

    return CONF_ST_WAIT_FOR_SRVSTOP_CNF;
}

static conf_state_t g3_conf_fsm_handle_srvstop_cnf(void)
{
    static BOOT_ServerStartRequest_t srv_start_req;

    g3_conf_fsm_info.curr_event = CONF_EV_NONE;

    srv_start_req.ReqType = BOOT_START_NORMAL;
    srv_start_req.PANId = g3_app_attrib_tbl_get_coord_pan_id();
    srv_start_req.ServerAddress = g3_app_attrib_tbl_get_coord_short_addr();

    task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_BOOT_SERVER_START_REQ, &srv_start_req, sizeof(srv_start_req), NO_WAIT);

    return CONF_ST_WAIT_FOR_SRVSTART_CNF;
}

static conf_state_t g3_conf_fsm_handle_srvstart_cnf(void)
{
    g3_conf_fsm_info.curr_event = CONF_EV_NONE;

    g3_app_keep_alive_start();

    return CONF_ST_WAIT_FOR_SWRESET_CNF;
}

static conf_state_t g3_conf_fsm_handle_g3libset_cnf(void)
{
    bool g3libset_conf_expected = g3_conf_set_attr();

    g3_conf_fsm_info.curr_event = CONF_EV_NONE;

    if (!g3libset_conf_expected || !g3_conf_fsm_info.g3lib_set_evts_cnt) {
        g3_app_keep_alive_start();

        return CONF_ST_WAIT_FOR_SWRESET_CNF;
    }

    return g3_conf_fsm_info.curr_state;
}

typedef conf_state_t g3_conf_fsm_func(void);

static g3_conf_fsm_func *g3_conf_fsm_func_tbl[CONF_ST_CNT][CONF_EV_CNT] = {
    {g3_conf_fsm_default_fnc, g3_conf_fsm_handle_swreset_cnf, g3_conf_fsm_default_fnc,            g3_conf_fsm_default_fnc,        g3_conf_fsm_default_fnc,                 g3_conf_fsm_default_fnc},
    {g3_conf_fsm_default_fnc, g3_conf_fsm_default_fnc,        g3_conf_fsm_handle_rfconfigset_cnf, g3_conf_fsm_default_fnc,        g3_conf_fsm_default_fnc,                 g3_conf_fsm_default_fnc},
    {g3_conf_fsm_default_fnc, g3_conf_fsm_default_fnc,        g3_conf_fsm_default_fnc,            g3_conf_fsm_default_fnc,        g3_conf_fsm_handle_default_srvstart_cnf, g3_conf_fsm_default_fnc},
    {g3_conf_fsm_default_fnc, g3_conf_fsm_default_fnc,        g3_conf_fsm_default_fnc,            g3_conf_fsm_handle_srvstop_cnf, g3_conf_fsm_default_fnc,                 g3_conf_fsm_default_fnc},
    {g3_conf_fsm_default_fnc, g3_conf_fsm_default_fnc,        g3_conf_fsm_default_fnc,            g3_conf_fsm_default_fnc,        g3_conf_fsm_handle_srvstart_cnf,         g3_conf_fsm_default_fnc},
    {g3_conf_fsm_default_fnc, g3_conf_fsm_default_fnc,        g3_conf_fsm_default_fnc,            g3_conf_fsm_default_fnc,        g3_conf_fsm_default_fnc,                 g3_conf_fsm_handle_g3libset_cnf}
};

static void g3_app_conf_handle_swreset_cnf(const void *buff)
{
#ifndef NDEBUG
    const G3_LIB_SWResetConfirm_t *sw_reset_conf = buff;
#endif /* NDEBUG */
    g3_conf_fsm_info.curr_event = CONF_EV_RECEIVED_SWRESET_CNF;

    assert(sw_reset_conf->Status == G3_SUCCESS);
}

static void g3_app_conf_handle_rfconfigset_cnf(const void *buff)
{
#ifndef NDEBUG
    const hif_rfconfigset_cnf_t *rfconfigset_conf = buff;
#endif /* NDEBUG */
    g3_conf_fsm_info.curr_event = CONF_EV_RECEIVED_RFCONFIGSET_CNF;

    assert(rfconfigset_conf->status == G3_SUCCESS);
}

static void g3_app_conf_handle_srvstop_cnf(const void *buff)
{
#ifndef NDEBUG
    const BOOT_ServerStopConfirm_t *srv_stop_conf = buff;
#endif /* NDEBUG */
    g3_conf_fsm_info.curr_event = CONF_EV_RECEIVED_SRVSTOP_CNF;

    assert(srv_stop_conf->Status == G3_SUCCESS);
}

static void g3_app_conf_handle_srvstart_cnf(const void *buff)
{
#ifndef NDEBUG
    const BOOT_ServerStartConfirm_t *srv_start_conf = buff;
#endif /* NDEBUG */
    g3_conf_fsm_info.curr_event = CONF_EV_RECEIVED_SRVSTART_CNF;

    assert(srv_start_conf->Status == G3_SUCCESS);
}

static void g3_app_conf_handle_g3libset_cnf(const void *buff)
{
#ifndef NDEBUG
    const G3_LIB_SetAttributeConfirm_t *set_attr_conf = buff;
#endif /* NDEBUG */
    g3_conf_fsm_info.g3lib_set_evts_cnt--;
    g3_conf_fsm_info.curr_event = CONF_EV_RECEIVED_G3LIBSET_CNF;

    assert(set_attr_conf->State == G3_SUCCESS);
}

bool g3_app_conf_msg_needed(const msg_t *msg)
{
    switch (msg->cmd_id) {
    case HIF_HI_HWRESET_CNF:
    case HIF_G3LIB_SWRESET_CNF:
    case HIF_HI_RFCONFIGSET_CNF:
    case HIF_G3LIB_SET_CNF:
    case HIF_BOOT_SERVER_STOP_CNF:
    case HIF_BOOT_SERVER_START_CNF:
    case HIF_UDP_CONN_SET_CNF:
    case HIF_UDP_DATA_CNF:
        return true;
    }

    return false;
}

void g3_app_conf_msg_handler(uint8_t cmd_id, const void *msg)
{
    switch (cmd_id) {
    case HIF_HI_HWRESET_CNF:
        break;
    case HIF_G3LIB_SWRESET_CNF:
        g3_app_conf_handle_swreset_cnf(msg);
        break;
    case HIF_HI_RFCONFIGSET_CNF:
        g3_app_conf_handle_rfconfigset_cnf(msg);
        break;
    case HIF_BOOT_SERVER_STOP_CNF:
        g3_app_conf_handle_srvstop_cnf(msg);
        break;
    case HIF_BOOT_SERVER_START_CNF:
        g3_app_conf_handle_srvstart_cnf(msg);
        break;
    case HIF_G3LIB_SET_CNF:
        g3_app_conf_handle_g3libset_cnf(msg);
        break;
    default:
        break;
    }
}

void g3_app_conf_init(task_comm_info_t *g3_task_comm_info_)
{
    g3_task_comm_info = g3_task_comm_info_;
    memset(&g3_conf_fsm_info, 0, sizeof(g3_conf_fsm_info));
    g3_app_attrib_tbl_init_default();
}

void g3_app_conf_start(void)
{
    static G3_LIB_SWResetRequest_t sw_reset_req;

    hi_g3lib_swresetreq_set(&sw_reset_req,
                            PROT_CENA_BANDPLAN, dev_type, HI_CFG_IPV6_BOOT_MODE);

//    hif_uart_send_message(HIF_G3LIB_SWRESET_REQ, &sw_reset_req, sizeof(sw_reset_req));

    task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_G3LIB_SWRESET_REQ, &sw_reset_req, sizeof(sw_reset_req), NO_WAIT);
}

// TODO: add timeout handling
void g3_app_conf(void)
{
    g3_conf_fsm_info.curr_state = g3_conf_fsm_func_tbl[g3_conf_fsm_info.curr_state][g3_conf_fsm_info.curr_event]();
}
