/**
  ******************************************************************************
  * @file    hi_ipv6_msgs.c
  * @author  AMG/IPC Application Team
  * @brief   HI IPv6 messages
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

#include <hi_msgs_impl.h>
#include <hi_ip_sap_interface.h>
#include <hi_boot_sap_interface.h>
#include <hi_g3lib_sap_interface.h>
#include <stdlib.h>
#include <utils.h>
#include <hif_g3_common.h>
#include <string.h>

void hi_ipv6_set_ipaddr(ip6_addr_t *addr, uint16_t pan_id, uint16_t short_addr)
{
    uint8_t prefix[] = {0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t interface_id[] = {0x00, 0x00, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x00};

    interface_id[0] = (pan_id >> 8) & 0xFFU;
    interface_id[1] = pan_id & 0xFFU;
    interface_id[6] = (short_addr >> 8) & 0xFFU;
    interface_id[7] = short_addr & 0xFFU;

    for (int i = 0; i < sizeof(prefix); i++)
        addr->u8[i] = prefix[i];

    for (int i = 0; i < sizeof(interface_id); i++)
        addr->u8[i + sizeof(prefix)] = interface_id[i];
}

void hi_ipv6_get_saddr_panid(const ip6_addr_t *addr, uint16_t *pan_id,
                             uint16_t *short_addr)
{
    const uint16_t vec_elems_cnt = sizeof(addr->u16) / sizeof(addr->u16[0]);
    const uint16_t interface_idx = vec_elems_cnt / 2;

    *pan_id = IP6_HTONS(addr->u16[interface_idx]);
    *short_addr = IP6_HTONS(addr->u16[vec_elems_cnt - 1]);
}

uint16_t hi_ipv6_echo_req_set(void *msg_, const ip6_addr_t *dst_addr,
                          uint8_t handle, uint16_t data_len,
                          const uint8_t *data)
{
    IP_G3IcmpDataRequest_t *msg = (IP_G3IcmpDataRequest_t *)msg_;

    msg->DestAddress = *dst_addr;
    msg->Handle = handle;
    msg->DataLen = data_len;

    if (data_len <= sizeof((IP_G3IcmpDataRequest_t *)0)->Data)
        memcpy(msg->Data, data, data_len);
    else
        UTILS_HANDLE_ERROR_AND_RETURN_VAL(NULL,
            "hi_ipv6_echo_request: data_len value too high", 0);

    return G3_ICMP_DATA_REQ_CURR_LEN(msg);
}

uint16_t hi_boot_srv_kick_req_set(void *msg_, uint16_t short_addr,
                                  const uint8_t *ext_addr)
{
    BOOT_ServerKickRequest_t *msg = msg_;

    msg->ShortAddress = short_addr;
    memcpy(msg->ExtendedAddress, ext_addr, sizeof(msg->ExtendedAddress));

    return sizeof(*msg);
}

uint16_t hi_g3lib_attrget_req_set(void *msg_, uint32_t id, uint16_t idx)
{
    G3_LIB_GetAttributeRequest_t *msg = msg_;

    msg->Attribute.ID = id;
    msg->Attribute.Index = idx;

    return sizeof(*msg);
}

uint16_t hi_boot_srv_setpsk_req_set(void *msg_, const uint8_t *ext_addr,
                                    const uint8_t *psk, const uint16_t short_addr)
{
    BOOT_ServerSetPSKRequest_t *msg = msg_;

    memcpy(msg->ExtendedAddress, ext_addr, sizeof(msg->ExtendedAddress));
    memcpy(msg->PreSharedKey, psk, sizeof(msg->PreSharedKey));
    msg->ShortAddress = short_addr;

    return sizeof(*msg);
}

uint16_t hi_boot_dev_pansort_req_set(void *msg_, uint8_t pan_cnt, const ADP_PanDescriptor_t *pan_descr)
{
    BOOT_DevicePANSortRequest_t *msg = msg_;

    msg->PANCount = pan_cnt;

    memcpy(msg->PANDescriptor, pan_descr, sizeof(msg->PANDescriptor[0]) * pan_cnt);

    return (sizeof(msg->PANCount) + (sizeof(msg->PANDescriptor[0]) * pan_cnt));
}

uint16_t hi_boot_dev_start_req_set(void *msg_, BOOT_StartType_t start_type)
{
    BOOT_DeviceStartRequest_t *msg = msg_;

    msg->ReqType = start_type;

    return sizeof(*msg);
}

void hi_g3lib_swresetreq_set(void *msg_, uint8_t prot_version, uint8_t device_type, uint8_t mode)
{
    G3_LIB_SWResetRequest_t *msg = (G3_LIB_SWResetRequest_t *)msg_;

    msg->Bandplan = prot_version;
    msg->DeviceType = device_type;
    msg->Mode = mode;
}

void hi_rfconfigsetreq_set(void *msg_, const s2lp_configdata_t *data)
{
    hif_rfconfigset_req_t *msg = (hif_rfconfigset_req_t *)msg_;

    msg->data = *data;
}

uint16_t hi_dbgtool_req_set(void *msg_, uint8_t control, uint32_t info)
{
    hi_dbgtool_req_t *msg = msg_;

    msg->control = control;
    msg->info = info;

    if (control == 0)
        return (sizeof(*msg) - sizeof(msg->info));
    else
        return sizeof(*msg);
}


uint16_t hi_ipv6_udp_conn_req_set(void *msg_, uint8_t conn_id, const ip6_addr_t *remote_addr,
                          uint16_t remote_port, uint16_t local_port)
{
    IP_UdpConnSetRequest_t *msg = (IP_UdpConnSetRequest_t *)msg_;

    msg->ConnId = conn_id;
    msg->RemoteAddress = *remote_addr;
    msg->RemotePort = remote_port;
    msg->LocalPort = local_port;
    
    return (uint16_t)sizeof(*msg); //returns the frame size (IP_UdpConnSetRequest_t size)
}




uint16_t hi_ipv6_udp_conn_req_get(void *msg_, uint8_t conn_id)
{
  IP_UdpConnGetRequest_t *msg = (IP_UdpConnGetRequest_t *)msg_;

   msg->ConnId = conn_id;

   return (uint16_t)sizeof(*msg);
}

                                    
uint16_t hi_ipv6_udp_data_req_set(void *msg_, const ip6_addr_t *dst_addr,
                          uint8_t handle, uint16_t dst_port, uint16_t data_len,
                          const uint8_t *data, uint8_t conn_id)
{
    IP_G3UdpDataRequest_t *msg = (IP_G3UdpDataRequest_t *)msg_;

    msg->DestAddress = *dst_addr;
    msg->DestPort = dst_port;
    msg->Handle = handle;
    msg->DataLen = data_len;
    msg->ConnId = conn_id;

    if (data_len <= sizeof((IP_G3UdpDataRequest_t *)0)->Data)
        memcpy(msg->Data, data, data_len);
    else
        UTILS_HANDLE_ERROR_AND_RETURN_VAL(NULL,
            "hi_ipv6_udp_data_req: data_len value too high", 0);

    return G3_UDP_DATA_REQ_CURR_LEN(msg);
}
