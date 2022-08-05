/**
  ******************************************************************************
  * @file    g3_app_keep_alive.c
  * @author  AMG/IPC Application Team
  * @brief   Keep-Alive implementation
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

#include <g3_app_keep_alive.h>
#include <hi_boot_sap_interface.h>
#include <hi_ip_sap_interface.h>
#include <hi_g3lib_sap_interface.h>
#include <hif_g3_common.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <timed_events.h>
#include <task_comm.h>
#include <hi_msgs_impl.h>
#include <limits.h>
#include <soft_timer.h>
#include <g3_hif.h>
#include <utils.h>
#include <user_g3_common.h>

#define KEEP_ALIVE_DEV_LAST_SEEN_TIMEOUT    (15U * TICKS_IN_A_SEC)

#define KEEP_ALIVE_DEV_SIDE_TIMEOUT         (10U * TICKS_IN_A_MIN)

#define KEEP_ALIVE_CHECK_TIMEOUT_DELAY      (5U * TICKS_IN_A_SEC)

#define KA_PAN_PING_TIMEOUT                 (2U * TICKS_IN_A_MIN)

#define DEVICE_TABLE_BITS_IN_FLAG           32U

#define DEVICE_TABLE_INFO_FLAGS_CNT         ((G3_APP_MAX_DEVICES_IN_PAN / DEVICE_TABLE_BITS_IN_FLAG) + \
                    (uint32_t)(!!(G3_APP_MAX_DEVICES_IN_PAN % DEVICE_TABLE_BITS_IN_FLAG)))

#define KA_EV_PANC_RECEIVED_ECHO_CNF_MASK (1<<KA_EV_PANC_RECEIVED_ECHO_CNF)
#define KA_EV_PANC_RECEIVED_ECHO_IND_MASK (1<<KA_EV_PANC_RECEIVED_ECHO_IND)

static void g3_app_ka_reset_fsm(void);
static void g3_app_ka_ping_stop(void);
static void g3_app_ka_panc_check_timeout(void *unused);
static void g3_app_ka_dev_check_timeout(void *unused);

typedef enum {
    KA_EV_PANC_NO_EVENT = 0,
    KA_EV_PANC_GET_SHORT_ADDR,
    KA_EV_PANC_GET_PAN_ID,
    KA_EV_PANC_SEND_ECHO_REQ,
    KA_EV_PANC_RECEIVED_ECHO_CNF,
    KA_EV_PANC_RECEIVED_ECHO_IND,
    KA_EV_PANC_RECEIVED_KICK_CNF,
    KA_EV_PANC_CNT
} ka_panc_event_t;

typedef enum {
    KA_ST_PANC_GET_SHORT_ADDR = 0,
    KA_ST_PANC_GET_PAN_ID,
    KA_ST_PANC_SEND_ECHO_REQ,
    KA_ST_PANC_WAIT_FOR_ECHO_CNF,
    KA_ST_PANC_WAIT_FOR_ECHO_IND,
    KA_ST_PANC_CNT
} ka_panc_state_t;

typedef struct {
    uint16_t short_addr;
    uint8_t  extended_addr[MAC_ADDR64_SIZE];
    uint64_t last_seen_timeout;
} ka_device_data_t;

typedef struct {
    uint16_t    devices_cnt;
    uint32_t    device_used_flag[DEVICE_TABLE_INFO_FLAGS_CNT];
} ka_device_table_info_t;

typedef struct {
    ka_device_table_info_t  info;
    ka_device_data_t        dt[G3_APP_MAX_DEVICES_IN_PAN];
    ka_device_data_t        *dt_ord_by_short_addr[G3_APP_MAX_DEVICES_IN_PAN];
} ka_device_table_t;

static struct {
    plcType_t ka_board_type;
    bool ka_board_type_set;
    bool ka_board_info_set;
    uint8_t timed_evts_idx_used;
    uint16_t pan_id;
    uint16_t short_addr;
} ka_info;

static struct {
    bool currently_checking_device;
    uint16_t ka_panc_ping_dst_short_addr;
    ka_panc_state_t ka_panc_state;
    ka_panc_event_t ka_panc_event;
    ka_device_table_t device_table;
    uint8_t ping_handle;
    uint16_t ka_panc_ev_received;
    BOOT_ServerKickRequest_t boot_kick_req; // this should later be changed to use
                                            // some form of generic buffering
} ka_panc_info;

static struct {
    bool     ka_dev_timeout_active;
    uint64_t ka_dev_timeout;
} ka_dev_info;

static task_comm_info_t *g3_task_comm_info;

static const uint8_t g3_app_ka_data[] = {0x00, 0x01, 0x02, 0x03, 0x04};

// Start of table handling code

static bool find_free_device_table_slot(const ka_device_table_t *device_table,
                                        uint16_t *free_slot_index)
{
    const uint16_t num_of_flags = DEVICE_TABLE_INFO_FLAGS_CNT;

    for (uint32_t i = 0; i < num_of_flags; ++i) {
        for (uint32_t j = 0; j < DEVICE_TABLE_BITS_IN_FLAG; ++j) {
            if (!(device_table->info.device_used_flag[i] & (1U << j))) {
                *free_slot_index = ((i * DEVICE_TABLE_BITS_IN_FLAG) + j);
                return true;
            }
        }
    }

    return false;
}

#ifndef NDEBUG
/* This function is called in an assert */
static bool allocate_device_table_slot(ka_device_table_t *device_table,
                                       uint16_t free_slot_index)
{
    const uint16_t i = free_slot_index / DEVICE_TABLE_BITS_IN_FLAG;
    const uint16_t j = free_slot_index % DEVICE_TABLE_BITS_IN_FLAG;

    if (device_table->info.device_used_flag[i] & (1U << j)) {
        return false;
    } else {
        uint32_t flag = device_table->info.device_used_flag[i];
        flag |= (1U << j);
        device_table->info.device_used_flag[i] = flag;
        return true;
    }
}
#endif /* NDEBUG */

static bool free_device_table_slot(ka_device_table_t *device_table,
                                   uint16_t slot_index)
{
    const uint16_t i = slot_index / DEVICE_TABLE_BITS_IN_FLAG;
    const uint16_t j = slot_index % DEVICE_TABLE_BITS_IN_FLAG;

    if (!(device_table->info.device_used_flag[i] & (1U << j))) {
        return false;
    } else {
        uint32_t flag = device_table->info.device_used_flag[i];
        flag &= ~(1U << j);
        device_table->info.device_used_flag[i] = flag;
        return true;
    }
}

static int compare_short_addr(const void *elem1, const void *elem2)
{
    ka_device_data_t *dev_data1 = *(ka_device_data_t **)elem1;
    ka_device_data_t *dev_data2 = *(ka_device_data_t **)elem2;

    if (dev_data1->short_addr < dev_data2->short_addr)
        return -1;
    else if (dev_data1->short_addr > dev_data2->short_addr)
        return 1;
    else
        return 0;
}

static void g3_app_ka_device_add_ordered(ka_device_table_t *device_table,
                                         ka_device_data_t *device_data,
                                         uint16_t index, uint16_t devices_cnt)
{
    const uint16_t new_elems_cnt = devices_cnt + 1;

    assert(index < G3_APP_MAX_DEVICES_IN_PAN);

    device_table->dt_ord_by_short_addr[index] = device_data;
    qsort(device_table->dt_ord_by_short_addr, new_elems_cnt,
          sizeof(device_data), compare_short_addr);
}

static void g3_app_ka_device_create_ordered(ka_device_table_t *device_table)
{
    const uint16_t num_of_flags = DEVICE_TABLE_INFO_FLAGS_CNT;
    uint16_t ord_idx = 0;
    ka_device_data_t **dev_table_ordered = NULL;
    int (*cmp_fnc)(const void *, const void *);

    dev_table_ordered = device_table->dt_ord_by_short_addr;
    cmp_fnc = compare_short_addr;

    assert(dev_table_ordered != NULL);

    for (uint32_t i = 0; i < num_of_flags; ++i) {
        for (uint32_t j = 0; j < DEVICE_TABLE_BITS_IN_FLAG; ++j) {
            if (device_table->info.device_used_flag[i] & (1U << j)) {
                const uint16_t orig_table_idx = ((i * DEVICE_TABLE_BITS_IN_FLAG) + j);
                dev_table_ordered[ord_idx++] = &device_table->dt[orig_table_idx];

                if (ord_idx >= device_table->info.devices_cnt)
                    goto sort_table;
            }
        }
    }

sort_table:
    qsort(dev_table_ordered, device_table->info.devices_cnt,
          sizeof(*dev_table_ordered), cmp_fnc);
}

static void g3_app_ka_device_add_data(ka_device_table_t *device_table,
                                      uint16_t free_slot_index,
                                      uint16_t device_short_addr,
                                      const uint8_t extended_addr[MAC_ADDR64_SIZE],
                                      uint32_t timeout)
{
    ka_device_data_t *device_data = &device_table->dt[free_slot_index];

    device_data->short_addr = device_short_addr;
    memcpy(device_data->extended_addr, extended_addr,
           sizeof(device_data->extended_addr));
    device_data->last_seen_timeout = st_getsystime() + timeout;

    g3_app_ka_device_add_ordered(device_table, device_data,
                                 device_table->info.devices_cnt,
                                 device_table->info.devices_cnt);

    device_table->info.devices_cnt++;
}

static bool g3_app_ka_device_add(ka_device_table_t *device_table,
                                 uint16_t short_addr,
                                 const uint8_t extended_addr[MAC_ADDR64_SIZE],
                                 uint32_t timeout)
{
    uint16_t free_slot_idx;

    if (!find_free_device_table_slot(device_table, &free_slot_idx))
        return false;

    assert(allocate_device_table_slot(device_table, free_slot_idx));

    g3_app_ka_device_add_data(device_table, free_slot_idx, short_addr,
                              extended_addr, timeout);

    return true;
}

static bool g3_app_ka_device_remove(ka_device_table_t *device_table,
                                    ka_device_data_t *device_data)
{
    uint16_t device_table_slot;

    assert(device_data != NULL);

    device_table_slot = device_data - device_table->dt;

    memset(device_data, 0, sizeof(*device_data));
    memset(device_table->dt_ord_by_short_addr, 0,
           sizeof(device_table->dt_ord_by_short_addr));

    free_device_table_slot(device_table, device_table_slot);

    device_table->info.devices_cnt--;

    if (device_table->info.devices_cnt)
        g3_app_ka_device_create_ordered(device_table);

    return true;
}

static ka_device_data_t *g3_app_ka_device_find(ka_device_table_t *device_table,
                                               uint16_t device_short_addr)
{
    ka_device_data_t key = {.short_addr = device_short_addr,
        .extended_addr = {0,},
        .last_seen_timeout = 0};
    ka_device_data_t *key_ptr = &key;

    if (!device_table->info.devices_cnt)
        return NULL;

    ka_device_data_t **device_data_ptr = bsearch(&key_ptr,
                                            device_table->dt_ord_by_short_addr,
                                            device_table->info.devices_cnt,
                                            sizeof(key_ptr), compare_short_addr);

    if (!device_data_ptr)
        return NULL;

    return *device_data_ptr;
}

static ka_device_data_t *g3_app_ka_device_find_by_ext_addr(ka_device_table_t *device_table,
                                               const uint8_t ext_addr[MAC_ADDR64_SIZE])
{
    const uint16_t table_len = device_table->info.devices_cnt;
    ka_device_data_t **dt = device_table->dt_ord_by_short_addr;
    ka_device_data_t *device_data = NULL;

    for (uint16_t i = 0; i < table_len; i++) {
        if (!memcmp(dt[i]->extended_addr, ext_addr, MAC_ADDR64_SIZE)) {
            device_data = dt[i];
            break;
        }
    }

    return device_data;
}

static void g3_app_ka_device_update_ext_addr(ka_device_data_t *device_data,
                                             const uint8_t ext_addr[MAC_ADDR64_SIZE])
{
    memcpy(device_data->extended_addr, ext_addr, MAC_ADDR64_SIZE);
}

static void g3_app_ka_device_update_timeout(ka_device_table_t *device_table,
                                            ka_device_data_t *device_data,
                                            uint32_t timeout)
{
    device_data->last_seen_timeout = st_getsystime() + timeout;
}

static ka_device_data_t *g3_app_ka_find_expired_timeout(ka_device_table_t *device_table)
{
    ka_device_data_t *min_timeout_dev = device_table->dt_ord_by_short_addr[0];
    uint64_t min_timeout = min_timeout_dev->last_seen_timeout;

    for (uint16_t i = 1; i < device_table->info.devices_cnt; i++) {
        ka_device_data_t *tmp_dev = device_table->dt_ord_by_short_addr[i];

        if (min_timeout > tmp_dev->last_seen_timeout) {
            min_timeout_dev = tmp_dev;
            min_timeout = tmp_dev->last_seen_timeout;
        }
    }

    if (!st_is_timeout_expired(min_timeout))
        return NULL;
    else
        return min_timeout_dev;
}

// End of table handling code

static void g3_app_ka_set_pan_id(uint16_t pan_id)
{
    ka_info.pan_id = pan_id;
}

static void g3_app_ka_set_short_addr(uint16_t short_addr)
{
    ka_info.short_addr = short_addr;
}

// Start of KA FSM code

static ka_panc_state_t g3_app_ka_fsm_default(ka_panc_event_t event)
{
    ka_panc_info.ka_panc_event = KA_EV_PANC_NO_EVENT;

    return ka_panc_info.ka_panc_state;
}

static ka_panc_state_t g3_app_ka_fsm_get_short_addr(ka_panc_event_t event)
{
    uint16_t len;
    static G3_LIB_GetAttributeRequest_t get_attr_req;

    ka_panc_info.ka_panc_event = KA_EV_PANC_NO_EVENT;

    len = hi_g3lib_attrget_req_set(&get_attr_req, MAC_PANID_ID, 0);

//    hif_uart_send_message(HIF_G3LIB_GET_REQ, &get_attr_req, len);

    task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_G3LIB_GET_REQ, &get_attr_req, len, NO_WAIT);

    return KA_ST_PANC_GET_PAN_ID;
}

static ka_panc_state_t g3_app_ka_fsm_get_pan_id(ka_panc_event_t event)
{
    ka_panc_info.ka_panc_event = KA_EV_PANC_NO_EVENT;

    ka_panc_info.ka_panc_ev_received = 0U;

    return KA_ST_PANC_SEND_ECHO_REQ;
}

static ka_panc_state_t g3_app_ka_fsm_send_echo_req(ka_panc_event_t event)
{
    ip6_addr_t ip_dst_addr;
    uint16_t req_len;
    static IP_G3IcmpDataRequest_t icmp_data_req;

    ka_panc_info.ka_panc_event = KA_EV_PANC_NO_EVENT;

    hi_ipv6_set_ipaddr(&ip_dst_addr, ka_info.pan_id, ka_panc_info.ka_panc_ping_dst_short_addr);
    req_len = hi_ipv6_echo_req_set(&icmp_data_req, &ip_dst_addr, ka_panc_info.ping_handle++,
                                   sizeof(g3_app_ka_data), g3_app_ka_data);

//    hif_uart_send_message(HIF_ICMP_ECHO_REQ, &icmp_data_req, req_len);

    task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_ICMP_ECHO_REQ, &icmp_data_req, req_len, NO_WAIT);

    st_set_timer(SOFT_TIMER_ID_G3APP_KA_PAN, KA_PAN_PING_TIMEOUT);

    return KA_ST_PANC_WAIT_FOR_ECHO_CNF;
}

static ka_panc_state_t g3_app_ka_fsm_wait_for_kick_cnf(ka_panc_event_t event)
{
    ka_panc_info.ka_panc_event = KA_EV_PANC_NO_EVENT;

    ka_panc_info.currently_checking_device = false;
    ka_panc_info.ka_panc_ping_dst_short_addr = 0xFFFF;

    return KA_ST_PANC_SEND_ECHO_REQ;
}


static ka_panc_state_t g3_app_ka_fsm_wait_for_echo_cnf_or_echo_ind(ka_panc_event_t event)
{
    ka_panc_state_t output_state = KA_ST_PANC_CNT;
    ka_panc_info.ka_panc_event = KA_EV_PANC_NO_EVENT;

    if (event == KA_EV_PANC_RECEIVED_ECHO_CNF)
    {
      ka_panc_info.ka_panc_ev_received |= 1<<KA_EV_PANC_RECEIVED_ECHO_CNF;
    }
    else if (event == KA_EV_PANC_RECEIVED_ECHO_IND)
    {
      ka_panc_info.ka_panc_ev_received |= 1<<KA_EV_PANC_RECEIVED_ECHO_IND;
    }
    else
    {
    }
    
    if ((ka_panc_info.ka_panc_ev_received & KA_EV_PANC_RECEIVED_ECHO_CNF_MASK)
        && (ka_panc_info.ka_panc_ev_received & KA_EV_PANC_RECEIVED_ECHO_IND_MASK))
    {
      ka_panc_info.currently_checking_device = false;
      ka_panc_info.ka_panc_ping_dst_short_addr = 0xFFFF;
      ka_panc_info.ka_panc_ev_received = 0U;
      output_state = KA_ST_PANC_SEND_ECHO_REQ;
    }
    else if ((ka_panc_info.ka_panc_ev_received & KA_EV_PANC_RECEIVED_ECHO_CNF_MASK) == KA_EV_PANC_RECEIVED_ECHO_CNF_MASK)
    {
      output_state = KA_ST_PANC_WAIT_FOR_ECHO_IND;
    }
    else if ((ka_panc_info.ka_panc_ev_received & KA_EV_PANC_RECEIVED_ECHO_IND_MASK) == KA_EV_PANC_RECEIVED_ECHO_IND_MASK)
    {
      output_state = KA_ST_PANC_WAIT_FOR_ECHO_CNF;
    }
    else
    {
    }
        
    return output_state;
}


typedef ka_panc_state_t g3_app_ka_panc_fsm_func(ka_panc_event_t);

static g3_app_ka_panc_fsm_func *ka_panc_fsm_func_tbl[KA_ST_PANC_CNT][KA_EV_PANC_CNT] = {
    {g3_app_ka_fsm_default, g3_app_ka_fsm_get_short_addr, g3_app_ka_fsm_default, g3_app_ka_fsm_default, g3_app_ka_fsm_default, g3_app_ka_fsm_default, g3_app_ka_fsm_default},
    {g3_app_ka_fsm_default, g3_app_ka_fsm_default, g3_app_ka_fsm_get_pan_id, g3_app_ka_fsm_default, g3_app_ka_fsm_default, g3_app_ka_fsm_default, g3_app_ka_fsm_default},
    {g3_app_ka_fsm_default, g3_app_ka_fsm_default, g3_app_ka_fsm_default, g3_app_ka_fsm_send_echo_req, g3_app_ka_fsm_default,           g3_app_ka_fsm_default,                        g3_app_ka_fsm_default},
    {g3_app_ka_fsm_default, g3_app_ka_fsm_default,        g3_app_ka_fsm_default,    g3_app_ka_fsm_default,       g3_app_ka_fsm_wait_for_echo_cnf_or_echo_ind, g3_app_ka_fsm_wait_for_echo_cnf_or_echo_ind,  g3_app_ka_fsm_wait_for_kick_cnf},
    {g3_app_ka_fsm_default, g3_app_ka_fsm_default,        g3_app_ka_fsm_default,    g3_app_ka_fsm_default,       g3_app_ka_fsm_default,                       g3_app_ka_fsm_wait_for_echo_cnf_or_echo_ind,  g3_app_ka_fsm_wait_for_kick_cnf}
};

// End of KA FSM code

// Start of MSG handling code

static void g3_app_ka_handle_g3lib_get_attr_cnf(const void *msg)
{
    const G3_LIB_GetAttributeConfirm_t *get_attr = msg;
    uint16_t pan_id, short_addr;

    if (get_attr->State) {
        g3_app_ka_reset_fsm();
        return;
    }

    if (MAC_PANID_ID == get_attr->Attribute.Attribute.ID) {
        memcpy(&pan_id, get_attr->Attribute.Value, sizeof(pan_id));
        g3_app_ka_set_pan_id(pan_id);
    } else if (MAC_SHORTADDRESS_ID == get_attr->Attribute.Attribute.ID) {
        memcpy(&short_addr, get_attr->Attribute.Value, sizeof(short_addr));
        g3_app_ka_set_short_addr(short_addr);
    }
}

static void g3_app_ka_handle_srv_join_ind(ka_device_table_t *device_table,
                                          const void *msg)
{
    const BOOT_ServerJoinIndication_t *srv_join_ind = msg;
    uint16_t short_addr = srv_join_ind->ShortAddress;
    const uint8_t *ext_addr = srv_join_ind->ExtendedAddress;

    if (!device_table->info.devices_cnt) {
        g3_app_ka_device_add(device_table, short_addr, ext_addr,
                             KEEP_ALIVE_DEV_LAST_SEEN_TIMEOUT);
    } else {
        ka_device_data_t *device_data = g3_app_ka_device_find(device_table,
                                                              short_addr);
        if (!device_data) {
            if (device_table->info.devices_cnt >= G3_APP_MAX_DEVICES_IN_PAN)
                return;

            g3_app_ka_device_add(device_table, short_addr, ext_addr,
                                 KEEP_ALIVE_DEV_LAST_SEEN_TIMEOUT);
        } else {
            if (short_addr == ka_panc_info.ka_panc_ping_dst_short_addr) {
                g3_app_ka_ping_stop();
            }

            g3_app_ka_device_update_ext_addr(device_data, ext_addr);
            g3_app_ka_device_update_timeout(device_table, device_data,
                                            KEEP_ALIVE_DEV_LAST_SEEN_TIMEOUT);
        }
    }
}

static IP_IcmpDataIndication_t *icmp_data_ind_get_data(const IP_DataIndication_t *data_ind)
{
    const uint8_t *ind = (uint8_t *)data_ind;

    return (IP_IcmpDataIndication_t *)(ind + UserG3_IpDataGetMetaDataLen(data_ind));
}

static void g3_app_ka_handle_device_start_cnf(const void *msg)
{
    const BOOT_DeviceStartConfirm_t *dev_start = msg;

    if (!dev_start->Status) {
        ka_dev_info.ka_dev_timeout_active = true;
        g3_app_ka_set_pan_id(dev_start->PANId);
        g3_app_ka_set_short_addr(dev_start->NetworkAddress);
        ka_dev_info.ka_dev_timeout = st_getsystime() + KEEP_ALIVE_DEV_SIDE_TIMEOUT;

        timed_events_add(g3_app_ka_dev_check_timeout, NULL,
                         TIMED_EVENTS_ABS_TIME(KEEP_ALIVE_CHECK_TIMEOUT_DELAY),
                         KEEP_ALIVE_CHECK_TIMEOUT_DELAY, &ka_info.timed_evts_idx_used);
    }
}

static void g3_app_ka_handle_device_leave_cnf(const void *msg)
{
    uint16_t len;
    const BOOT_DeviceLeaveConfirm_t *dev_leave = msg;
    static BOOT_DeviceStartRequest_t dev_start_req;

    if (!dev_leave->Status) {
        ka_dev_info.ka_dev_timeout_active = false;
        timed_events_remove(ka_info.timed_evts_idx_used);

        len = hi_boot_dev_start_req_set(&dev_start_req, BOOT_START_NORMAL);

//        hif_uart_send_message(HIF_BOOT_DEVICE_START_REQ, &dev_start_req, len);

        task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_BOOT_DEVICE_START_REQ, &dev_start_req, len, NO_WAIT);
    }
}

static void g3_app_ka_handle_echo_ind(ka_device_table_t *device_table,
                                      const void *msg)
{
    const IP_DataIndication_t *echo_ind = msg;
    const IP_IcmpDataIndication_t *icmp_data_ind = icmp_data_ind_get_data(echo_ind);
    uint16_t ping_dst_addr, ping_dst_panc_id;

    hi_ipv6_get_saddr_panid(&icmp_data_ind->SourceAddress, &ping_dst_panc_id,
                            &ping_dst_addr);

    if (ping_dst_addr == ka_panc_info.ka_panc_ping_dst_short_addr) {
        ka_device_data_t *device_data = g3_app_ka_device_find(device_table,
                                                              ping_dst_addr);
        if (device_data) {
            g3_app_ka_device_update_timeout(device_table, device_data,
                                            KEEP_ALIVE_DEV_LAST_SEEN_TIMEOUT);
        }
        // what do we do in the else clause here? do we insert it into
        // the table?
#if 0
        else {
        }
#endif
    }
    // do we need to handle the else case here?
    // this is the case where we receive and echo-ind
    // unrelated to the current ping target
    // do we handle the case where the coord receives a ping from a device?
#if 0
    else {
    }
#endif
}

static void g3_app_ka_panc_handle_udp_data_ind(ka_device_table_t *device_table,
                                              const void *msg)
{
    const IP_DataIndication_t *ip_udp_ind = msg;
    const IP_UdpDataIndication_t *udp_data_ind = UserG3_UdpDataIndGetData(ip_udp_ind);
    uint16_t src_panc_id, src_short_addr;

    hi_ipv6_get_saddr_panid(&udp_data_ind->SourceAddress, &src_panc_id,
                            &src_short_addr);

    ka_device_data_t *device_data = g3_app_ka_device_find(device_table,
                                                          src_short_addr);

    if (device_data) {
        g3_app_ka_device_update_timeout(device_table, device_data,
                                        KEEP_ALIVE_DEV_LAST_SEEN_TIMEOUT);
    }
}

static void g3_app_ka_panc_handle_srv_leave_ind(ka_device_table_t *device_table,
                                              const void *msg)
{
    const BOOT_ServerLeaveIndication_t *srv_leave_ind = msg;

    ka_device_data_t *device_data = g3_app_ka_device_find_by_ext_addr(device_table, srv_leave_ind->ExtendedAddress);

    if (device_data) {
        if (ka_panc_info.currently_checking_device &&
            (device_data->short_addr == ka_panc_info.ka_panc_ping_dst_short_addr)) {
            g3_app_ka_ping_stop();
        }

        g3_app_ka_device_remove(device_table, device_data);
    }
}

static void g3_app_ka_dev_handle_udp_data_ind(const void *msg)
{
    const IP_DataIndication_t *udp_data_ind = msg;
    uint16_t src_pan_id, src_short_addr;

    const ip6_addr_t * p_source_address = UserG3_IpDataIndGetSourceAddressPtr(udp_data_ind);
  
    hi_ipv6_get_saddr_panid(p_source_address, &src_pan_id, &src_short_addr);

    // Is it from the Pan Coordinator?
    if (!src_short_addr) {
        ka_dev_info.ka_dev_timeout = st_getsystime() + KEEP_ALIVE_DEV_SIDE_TIMEOUT;
    }
}

static void g3_app_ka_dev_handle_echo_ind(const void *msg)
{
    const IP_DataIndication_t *icmp_echo_ind = msg;
    uint16_t src_pan_id, src_short_addr;

    const ip6_addr_t * p_source_address = UserG3_IpDataIndGetSourceAddressPtr(icmp_echo_ind);
  
    hi_ipv6_get_saddr_panid(p_source_address, &src_pan_id, &src_short_addr);

    // Is it from the Pan Coordinator?
    if (!src_short_addr) {
        ka_dev_info.ka_dev_timeout = st_getsystime() + KEEP_ALIVE_DEV_SIDE_TIMEOUT;
    }
}

static void g3_app_handle_tools_cnf(const void *msg)
{
    uint16_t len;
    static G3_LIB_GetAttributeRequest_t get_attrib_req;
    const hi_rdbg_conf_t *hi_tools = msg;

    if (!(hi_tools->g3_info & PE_VERSION_PROFILE_MASK)) {
        ka_info.ka_board_type = PLC_G3_DEVICE;
    } else {
        ka_info.ka_board_type = PLC_G3_COORD;
    }

    ka_info.ka_board_type_set = true;

    len = hi_g3lib_attrget_req_set(&get_attrib_req, MAC_SHORTADDRESS_ID, 0);

//    hif_uart_send_message(HIF_G3LIB_GET_REQ, &get_attrib_req, len);

    task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_G3LIB_GET_REQ, &get_attrib_req, len, NO_WAIT);

    timed_events_add(g3_app_ka_panc_check_timeout, NULL,
                     TIMED_EVENTS_ABS_TIME(KEEP_ALIVE_CHECK_TIMEOUT_DELAY),
                     KEEP_ALIVE_CHECK_TIMEOUT_DELAY, &ka_info.timed_evts_idx_used);
}

#if 0
static void g3_app_handle_srv_start_cnf(const void *msg)
{
    uint16_t len;
    const BOOT_ServerStartConfirm_t *srv_start_conf = msg;
    static G3_LIB_GetAttributeRequest_t get_attrib_req;

    assert(!srv_start_conf->Status);

    len = hi_g3lib_attrget_req_set(&get_attrib_req, MAC_SHORTADDRESS_ID, 0);

//    hif_uart_send_message(HIF_G3LIB_GET_REQ, &get_attrib_req, len);

    task_comm_put_send_buffer(G3_PROT, HIF_G3LIB_GET_REQ, &get_attrib_req, len, NO_WAIT);

    timed_events_add(g3_app_ka_panc_check_timeout, NULL,
                     TIMED_EVENTS_ABS_TIME(KEEP_ALIVE_CHECK_TIMEOUT_DELAY),
                     KEEP_ALIVE_CHECK_TIMEOUT_DELAY, &ka_info.timed_evts_idx_used);
}
#endif

// End of MSG handling code

static void g3_app_ka_panc_kick_dev(uint16_t short_addr)
{
    uint16_t len;

    ka_device_data_t *dev_data = g3_app_ka_device_find(&ka_panc_info.device_table,
                                                       short_addr);

    assert(dev_data != NULL);

    if (!dev_data)
        return;

    len = hi_boot_srv_kick_req_set(&ka_panc_info.boot_kick_req, dev_data->short_addr,
                                   dev_data->extended_addr);

//    hif_uart_send_message(HIF_BOOT_SERVER_KICK_REQ, &ka_panc_info.boot_kick_req, len);

    task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_BOOT_SERVER_KICK_REQ, &ka_panc_info.boot_kick_req, len, NO_WAIT);

    if (ka_panc_info.currently_checking_device && (short_addr == ka_panc_info.ka_panc_ping_dst_short_addr)) {
            g3_app_ka_ping_stop();
    }

    g3_app_ka_device_remove(&ka_panc_info.device_table, dev_data);
}

static void g3_app_ka_ping_start(uint16_t short_addr)
{
    ka_panc_info.currently_checking_device = true;
    ka_panc_info.ka_panc_ping_dst_short_addr = short_addr;
    ka_panc_info.ka_panc_event = KA_EV_PANC_SEND_ECHO_REQ;
}

static void g3_app_ka_ping_stop(void)
{
    ka_panc_info.currently_checking_device = false;
    ka_panc_info.ka_panc_ping_dst_short_addr = 0xFFFF;
    ka_panc_info.ka_panc_event = KA_EV_PANC_NO_EVENT;
    ka_panc_info.ka_panc_state = KA_ST_PANC_SEND_ECHO_REQ;
}

static void g3_app_ka_ping_cnf_received(void)
{
    ka_panc_info.ka_panc_event = KA_EV_PANC_RECEIVED_ECHO_CNF;
}

static void g3_app_ka_ping_ind_received(void)
{
    ka_panc_info.ka_panc_event = KA_EV_PANC_RECEIVED_ECHO_IND;
    st_reset_timer(SOFT_TIMER_ID_G3APP_KA_PAN);
}

static void g3_app_ka_kick_cnf_received(void)
{
    ka_panc_info.ka_panc_event = KA_EV_PANC_RECEIVED_KICK_CNF;
}

static void g3_app_ka_g3lib_get_cnf_received(const void *msg)
{
    const G3_LIB_GetAttributeConfirm_t *get_attr = msg;

    if (MAC_PANID_ID == get_attr->Attribute.Attribute.ID)
        ka_panc_info.ka_panc_event = KA_EV_PANC_GET_PAN_ID;
    else if (MAC_SHORTADDRESS_ID == get_attr->Attribute.Attribute.ID)
        ka_panc_info.ka_panc_event = KA_EV_PANC_GET_SHORT_ADDR;
}

static void g3_app_ka_reset_fsm(void)
{
    ka_panc_info.ka_panc_event = KA_EV_PANC_NO_EVENT;
    ka_panc_info.ka_panc_state = KA_ST_PANC_GET_SHORT_ADDR;
}

static void g3_app_ka_panc_handle_timeout(void)
{
    g3_app_ka_panc_kick_dev(ka_panc_info.ka_panc_ping_dst_short_addr);
}

bool g3_app_ka_msg_needed(const msg_t *msg)
{
    switch (msg->cmd_id) {
#if 0
    case HIF_BOOT_SERVER_START_CNF:
#endif
    case HIF_BOOT_SERVER_JOIN_IND:
    case HIF_ICMP_ECHO_CNF:
    case HIF_ICMP_ECHO_IND:
    case HIF_ICMP_ECHO_REQ_IND:
    case HIF_UDP_DATA_IND:
    case HIF_BOOT_DEVICE_LEAVE_CNF:
    case HIF_BOOT_DEVICE_LEAVE_IND:
    case HIF_BOOT_DEVICE_START_CNF:
    case HIF_BOOT_SERVER_KICK_CNF:
    case HIF_BOOT_SERVER_LEAVE_IND:
    case HIF_HI_TOOLS_CNF:
    case HIF_G3LIB_GET_CNF:
        return true;
    default:
        break;
    }

    return false;
}

static void g3_app_ka_panc_msg_handler(uint8_t cmd_id, const void *msg)
{
    ka_device_table_t *device_table = &ka_panc_info.device_table;

    switch (cmd_id) {
#if 0
    case HIF_BOOT_SERVER_START_CNF:
        g3_app_handle_srv_start_cnf(msg);
        break;
#endif
    case HIF_BOOT_SERVER_JOIN_IND:
        g3_app_ka_handle_srv_join_ind(device_table, msg);
        break;
    case HIF_ICMP_ECHO_CNF:
        g3_app_ka_ping_cnf_received();
        break;
    case HIF_ICMP_ECHO_IND:
        g3_app_ka_handle_echo_ind(device_table, msg);
        g3_app_ka_ping_ind_received();
        break;
    case HIF_UDP_DATA_IND:
        g3_app_ka_panc_handle_udp_data_ind(device_table, msg);
        break;
    case HIF_BOOT_SERVER_KICK_CNF:
        g3_app_ka_kick_cnf_received();
        break;
    case HIF_BOOT_SERVER_LEAVE_IND:
        g3_app_ka_panc_handle_srv_leave_ind(device_table, msg);
        break;
    case HIF_G3LIB_GET_CNF:
        g3_app_ka_handle_g3lib_get_attr_cnf(msg);
        g3_app_ka_g3lib_get_cnf_received(msg);
        break;
    default:
        break;
    }
}

static void g3_app_ka_dev_msg_handler(uint8_t cmd_id, const void *msg)
{
    switch (cmd_id) {
    case HIF_UDP_DATA_IND:
        g3_app_ka_dev_handle_udp_data_ind(msg);
        break;
    case HIF_ICMP_ECHO_REQ_IND:
        g3_app_ka_dev_handle_echo_ind(msg);
        break;
    case HIF_BOOT_DEVICE_LEAVE_CNF:
        g3_app_ka_handle_device_leave_cnf(msg);
        break;
    case HIF_BOOT_DEVICE_START_CNF:
        g3_app_ka_handle_device_start_cnf(msg);
        break;
    default:
        break;
    }
}

void g3_app_ka_msg_handler(uint8_t cmd_id, const void *msg)
{
    if (!ka_info.ka_board_type_set) {
        if (HIF_HI_TOOLS_CNF == cmd_id) {
            g3_app_handle_tools_cnf(msg);
        }

        return;
    }

    if (PLC_G3_COORD == ka_info.ka_board_type)
        g3_app_ka_panc_msg_handler(cmd_id, msg);
    else if (PLC_G3_DEVICE == ka_info.ka_board_type)
        g3_app_ka_dev_msg_handler(cmd_id, msg);
}

static void g3_app_ka_panc_check_timeout(void *unused)
{
    if (!ka_panc_info.currently_checking_device &&
        ka_panc_info.device_table.info.devices_cnt) {
        ka_device_data_t *device_data =
            g3_app_ka_find_expired_timeout(&ka_panc_info.device_table);

        if (device_data) {
            g3_app_ka_ping_start(device_data->short_addr);
        }
    }
}

static void g3_app_ka_dev_check_timeout(void *unused)
{
    if (ka_dev_info.ka_dev_timeout_active &&
        st_is_timeout_expired(ka_dev_info.ka_dev_timeout)) {
//        hif_uart_send_message(HIF_BOOT_DEVICE_LEAVE_REQ, NULL, 0);

        // we use a fake buffer address here to be able to check for errors (buff == NULL)
        task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_BOOT_DEVICE_LEAVE_REQ, utils_get_fake_buff_addr(), 0, NO_WAIT);

        ka_dev_info.ka_dev_timeout_active = false;
    }
}

void g3_app_keep_alive_init(task_comm_info_t *g3_task_comm_info_)
{
    g3_task_comm_info = g3_task_comm_info_;
    memset(&ka_info, 0, sizeof(ka_info));
    memset(&ka_panc_info, 0, sizeof(ka_panc_info));
    ka_panc_info.ka_panc_ping_dst_short_addr = 0xFFFF;
    memset(&ka_dev_info, 0, sizeof(ka_dev_info));
}

void g3_app_keep_alive_start(void)
{
    static hi_dbgtool_req_t dbgtool_req;

    uint16_t len = hi_dbgtool_req_set(&dbgtool_req, HI_TOOL_INFO, 0);

//    hif_uart_send_message(HIF_HI_TOOLS_REQ, &dbgtool_req, len);

    task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_HI_TOOLS_REQ, &dbgtool_req, len, NO_WAIT);
}

void g3_app_keep_alive(void)
{
    if (ka_info.ka_board_type_set &&
        (PLC_G3_COORD == ka_info.ka_board_type)) {

        if (st_is_timer_expired(SOFT_TIMER_ID_G3APP_KA_PAN)) {
            g3_app_ka_panc_handle_timeout();
            st_reset_timer(SOFT_TIMER_ID_G3APP_KA_PAN);
        }

        ka_panc_info.ka_panc_state =
            ka_panc_fsm_func_tbl[ka_panc_info.ka_panc_state][ka_panc_info.ka_panc_event](ka_panc_info.ka_panc_event);
    }
}
