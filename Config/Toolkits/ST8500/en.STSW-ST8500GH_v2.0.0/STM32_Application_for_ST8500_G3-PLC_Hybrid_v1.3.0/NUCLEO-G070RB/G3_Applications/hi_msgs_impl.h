/**
  ******************************************************************************
  * @file    hi_ipv6_msgs.h
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

#ifndef HI_MSGS_IMPL_H_
#define HI_MSGS_IMPL_H_

#include <hi_adp_pib_types.h>
#include <hi_boot_sap_interface.h>
#include <stdint.h>
#include <task_comm.h>

// Rdbg definitions
#define PE_VERSION_BAND_MASK            0xE0000000
#define PE_VERSION_PROFILE_MASK         0x10000000
#define PE_VERSION_MASK                 0x0FFFFFFF
#define PE_VERSION_TYPE_MASK            0x0F000000
#define PE_VERSION_MAJOR_MASK           0x00FF0000
#define PE_VERSION_MINOR_MASK           0x0000FF00
#define PE_VERSION_PATCH_MASK           0x000000FF

#define PE_VERSION_BAND_SHIFT           29
#define PE_VERSION_PROFILE_SHIFT        28
#define PE_VERSION_TYPE_SHIFT           24
#define PE_VERSION_MAJOR_SHIFT          16
#define PE_VERSION_MINOR_SHIFT          8
#define PE_VERSION_PATCH_SHIFT          0

#define PLAT_TYPE_INVALID              ((uint8_t)0x00)
#define PLAT_TYPE_STCOMET_STCOM        ((uint8_t)0x01)
#define PLAT_TYPE_ST8500               ((uint8_t)0x02)
#define PLAT_TYPE_UNSPEC               ((uint8_t)0xFF)

#define HI_CFG_PHY_MODE                 ((uint8_t)0x0)
#define HI_CFG_MAC_MODE                 ((uint8_t)0x1)
#define HI_CFG_ADP_MODE                 ((uint8_t)0x2)
#define HI_CFG_ADP_BOOT_MODE            ((uint8_t)0x3)
#define HI_CFG_IPV6_BOOT_MODE           ((uint8_t)0x5)

#define IP6_HTONS(n) (uint16_t)((((uint16_t) (n)) << 8) | (((uint16_t) (n)) >> 8))
#define IP6_HTONL(n) (((uint32_t)IP6_HTONS(n) << 16) | IP6_HTONS((uint32_t)(n) >> 16))

#pragma pack(1)

typedef struct {
    uint32_t RadioBaseFreq;      /* Specify the base carrier frequency (in Hz) */
    uint8_t  RadioModSelect;     /* Specify the modulation */
    uint32_t RadioDataRate;      /* Specify the datarate expressed in bps */
    uint32_t RadioFreqDeviation; /* Specify the frequency deviation expressed in Hz */
    uint32_t RadioBandwidth;     /* Specify the channel filter bandwidth expressed in Hz */
    uint8_t  RadioCsBlanking;    /* Prevent data to be received if the RSSI level on the air is below the RSSI threshold */
    uint32_t RadioXtalFreq;      /* Set the XTAL frequency */
    uint8_t  RadioRssiGain;      /* Set the RSSI filter gain */
    uint8_t  PowerdBm;           /* Power Amplifier value to write expressed in dBm (0 is -31 dBm, 45 is 14 dBm). In case of FEM this value is used to set the raw register */
    uint8_t  PktCRCMode;         /* Set the CRC type */
    uint8_t  PktEnFEC;           /* Enable the FEC/Viterbi */
    uint8_t  PktEnWhitening;     /* Enable the data whitening */
    uint8_t  PktEnInterleaving;  /* Enable the interleaving of 802.15.4g packet */
    uint8_t  IrqGpioPin;         /* Specify the S2LP GPIO pin used to route the interrupt flag */
    uint8_t  MCUClkEnable;       /* Enable the S2LP Clock Output (0x00 Disabled, 0x01 Enabled) */
    uint8_t  MCUClkGpioPin;      /* Specify the S2LP GPIO pin used to export S2LP Clock to MCU */
    uint8_t  MCUClkXORatio;      /* Set the S2LP clock output XO prescaler */
    uint8_t  MCUClkRCORatio;     /* Set the S2LP Clock Output RCO prescaler */
    uint8_t  MCUClkClockCycles;  /* Set the S2LP ExtraClockCycles */
    uint8_t  ExtSmpsEnable;      /* Enable the external SMPS (0x00 Disabled, 0x01 Enabled) */
    uint8_t  FEMEnabled;         /* Enable the S2LP GPIO signals to control the Front End Module (0x00 Disabled, 0x01 Enabled) */
    uint8_t  FEMGpioPinCSD;      /* Specify the S2LP GPIO pin to control CSD */
    uint8_t  FEMGpioPinCPS;      /* Specify the S2LP GPIO pin to control CPS */
    uint8_t  FEMGpioPinCTX;      /* Specify the S2LP GPIO pin to control CTX */
    uint8_t  FEMTxBypassEn;      /* Enable the FEM TX Bypass Mode (0x00 Disabled, 0x01 Enabled) */
} s2lp_configdata_t;

typedef struct {
    s2lp_configdata_t data;
} hif_rfconfigset_req_t;

typedef struct {
    uint8_t status;
} hif_rfconfigset_cnf_t;

typedef struct {
  uint32_t                  g3_info;
  uint32_t                  libVersion;
  uint32_t                  rteVersion;
  uint32_t                  eui64[2];
  uint32_t                  sflashid;
  uint8_t                   usartnum;
  uint8_t                   ledconf;
  uint8_t                   platform;
  uint8_t                   rfconf;
  uint8_t                   plcmode;
  uint16_t                  panid;
  uint16_t                  shortaddr;
} hi_rdbg_conf_t;

typedef struct {
  uint8_t   control;
  uint32_t  info;
} hi_dbgtool_req_t;

typedef union {
  uint8_t  u8[16];
  uint16_t u16[8];
} ip6_addr_t;

typedef enum {
  PLC_NONE = 0,
  PLC_G3_COORD,
  PLC_G3_DEVICE,
} plcType_t;

#pragma pack()

void hi_ipv6_set_ipaddr(ip6_addr_t *addr, uint16_t pan_id, uint16_t short_addr);
void hi_ipv6_get_saddr_panid(const ip6_addr_t *addr, uint16_t *pan_id,
                             uint16_t *short_addr);
uint16_t hi_ipv6_echo_req_set(void *msg_, const ip6_addr_t *dst_addr,
                              uint8_t handle, uint16_t data_len,
                              const uint8_t *data);
uint16_t hi_boot_srv_kick_req_set(void *msg_, uint16_t short_addr,
                                  const uint8_t *ext_addr);

uint16_t hi_g3lib_attrget_req_set(void *msg_, uint32_t id, uint16_t idx);

uint16_t hi_boot_srv_setpsk_req_set(void *msg_, const uint8_t *ext_addr,
                                    const uint8_t *psk, const uint16_t short_addr);

uint16_t hi_boot_dev_pansort_req_set(void *msg_, uint8_t pan_cnt, const ADP_PanDescriptor_t *pan_descr);

uint16_t hi_boot_dev_start_req_set(void *msg_, BOOT_StartType_t start_type);

void hi_g3lib_swresetreq_set(void *msg_, uint8_t prot_version, uint8_t device_type, uint8_t mode);

void hi_rfconfigsetreq_set(void *msg_, const s2lp_configdata_t *data);

uint16_t hi_dbgtool_req_set(void *msg_, uint8_t control, uint32_t info);

uint16_t hi_ipv6_udp_conn_req_set(void *msg_, uint8_t conn_id, const ip6_addr_t *remote_addr,
                          uint16_t remote_port, uint16_t local_port);

uint16_t hi_ipv6_udp_conn_req_get(void *msg_, uint8_t conn_id);

uint16_t hi_ipv6_udp_data_req_set(void *msg_, const ip6_addr_t *dst_addr,
                          uint8_t handle, uint16_t dst_port, uint16_t data_len,
                          const uint8_t *data, uint8_t conn_id);

#endif
