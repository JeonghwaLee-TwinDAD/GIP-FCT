/**
  ******************************************************************************
  * @file    hif_g3_common.h
  * @author  AMG/IPC Application Team
  * @brief   Header file to define the interface for the Common HI functions
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

#ifndef HIF_G3_COMMON_H
#define HIF_G3_COMMON_H

/*******************************************************************************
* INCLUDE FILES:
*******************************************************************************/

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_g3_common HIF G3 COMMON
  * @{
  */

/** @defgroup hif_g3_common_Constants HIF G3 COMMON Constants
  * @{
  */


/*******************************************************************************
* MACROS & CONSTANTS:
*******************************************************************************/

#ifndef HI_GENERIC_HDL_H
/*******************************************************************************
* TYPES:
*******************************************************************************/
#define HI_TIMEOUT_HWRESET      800 /* 8s */
#define HI_TIMEOUT_NOPE         100 /* 1s */

// HOSTIF TOOL PAYLOAD
#define HI_TOOL_INFO            0x00
#define HI_TOOL_UART_NUM        0x01
#define HI_TOOL_LED_NUM         0x02
#define HI_TOOL_RF_NUM          0x03
#define HI_TOOL_PLC_MODE        0x10
#define HI_TOOL_PLC_STATE       0x11
#define HI_TOOL_RF_CONFIG       0x12
#define HI_TOOL_BAUDRATE        0x20
#define HI_TOOL_RESET_STATE     0x21
#define HI_TOOL_PLC_PERF        0x30
#define HI_TOOL_SFL_ERASE       0x40
#define HI_TOOL_SFL_READ        0x41
#define HI_TOOL_SFL_WRITE       0x42
#define HI_TOOL_NVM_RD          0x43
#define HI_TOOL_NVM_WR          0x44
#define HI_TOOL_NVM_SV          0x45
#define HI_TOOL_NVM_ER          0x46
#define HI_TRACE_DISABLE        0x50
#define HI_TRACE_ENABLE         0x51
#define HI_TRACE_START          0x52
#define HI_TRACE_STOP           0x53
#define HI_TRACE_DUMP           0x54
#define HI_TRACE_FILTER         0x55
#define HI_TRACE_TRIGGER        0x56
#define HI_TOOL_SECINIT         0x60
#define HI_TOOL_SECCFG          0x61
#define HI_TOOL_OTP_RD          0x70
#define HI_TOOL_OTP_WR          0x71
#define HI_TOOL_OTP_LOCK        0x72
#define HI_TOOL_RF_GET_CONF     0x80
#define HI_TOOL_RF_SET_CONF     0x81

// HOSTIF G3LIB PAYLOAD
#define HI_G3LIB_GET_REQ        0x00
#define HI_G3LIB_SET_REQ        0x01
#define HI_G3LIB_SWRESET_REQ    0x02
#define HI_G3LIB_TESTMODE_REQ   0x03
#define HI_G3LIB_GET_REQ_NEW    0x04
#define HI_G3LIB_SET_REQ_NEW    0x05

// APPLICATION CMD ID

typedef enum {
/*  HI_STATE_GET_REQ      = 0x00,*/ /*deleted */

  HIF_HI_HWRESET_REQ        = 0x00,
  HIF_HI_HWRESET_CNF        = 0x01,
  HIF_HI_MODE_SET_REQ       = 0x02,
  HIF_HI_MODE_SET_CNF       = 0x03,
  HIF_HI_MODE_GET_REQ       = 0x04,
  HIF_HI_MODE_GET_CNF       = 0x05,
  HIF_HI_BAUDRATE_SET_REQ   = 0x06,
  HIF_HI_BAUDRATE_SET_CNF   = 0x07,
  HIF_HI_RESETSTATE_REQ     = 0x08,
  HIF_HI_RESETSTATE_CNF     = 0x09,
  HIF_HI_TEST_SET_REQ       = 0x0A,
  HIF_HI_TEST_SET_CNF       = 0x0B,
  HIF_HI_TEST_GET_REQ       = 0x0C,
  HIF_HI_TEST_GET_CNF       = 0x0D,
  HIF_HI_SFLASH_REQ         = 0x0E,
  HIF_HI_SFLASH_CNF         = 0x0F,
  HIF_HI_NVRAM_REQ          = 0x10,
  HIF_HI_NVRAM_CNF          = 0x11,
  HIF_HI_TRACE_REQ          = 0x12,
  HIF_HI_TRACE_CNF          = 0x13,
  HIF_HI_TOOLS_REQ          = 0x14,
  HIF_HI_TOOLS_CNF          = 0x15,
  HIF_HI_SEC_INIT_REQ       = 0x16,
  HIF_HI_SEC_INIT_CNF       = 0x17,
  HIF_HI_RFCONFIGSET_REQ    = 0x18,
  HIF_HI_RFCONFIGSET_CNF    = 0x19,
  HIF_HI_RFCONFIGGET_REQ    = 0x1A,
  HIF_HI_RFCONFIGGET_CNF    = 0x1B,
  HIF_HI_OTP_REQ            = 0x1D,
  HIF_HI_OTP_CNF            = 0x1E,
  HIF_HI_READY_IND          = 0x1F,

  HIF_G3LIB_GET_REQ         = 0x20,
  HIF_G3LIB_GET_CNF         = 0x21,
  HIF_G3LIB_SET_REQ         = 0x22,
  HIF_G3LIB_SET_CNF         = 0x23,
  HIF_G3LIB_SWRESET_REQ     = 0x24,
  HIF_G3LIB_SWRESET_CNF     = 0x25,
  HIF_G3LIB_TESTMODE_REQ    = 0x26,
  HIF_G3LIB_TESTMODE_CNF    = 0x27,
  HIF_G3LIB_EVENT_NOTE_IND  = 0x28,
  HI_G3LIB_MULTIPLEGET_REQ  = 0x29,
  HI_G3LIB_MULTIPLEGET_CNF  = 0x2A,

  HIF_PD_DATA_REQ           = 0x40,
  HIF_PD_DATA_CNF           = 0x41,
  HIF_PD_DATA_IND           = 0x42,
  HIF_PD_ACK_REQ            = 0x43,
  HIF_PD_ACK_CNF            = 0x44,
  HIF_PD_ACK_IND            = 0x45,
  HIF_PHY_RF_REQ            = 0x46,
  HIF_PHY_RF_CNF            = 0x47,
  HIF_PHY_RF_IND            = 0x48,
  HIF_PLME_GET_CNF          = 0x49, /* not implemented */
  HIF_PLME_SET_TRX_STATE_REQ= 0x4A,
  HIF_PLME_SET_TRX_STATE_CNF= 0x4B,
  HIF_PLME_CS_REQ           = 0x4C,
  HIF_PLME_CS_CNF           = 0x4D,

  HIF_MCPS_DATA_REQ         = 0x60,
  HIF_MCPS_DATA_CNF         = 0x61,
  HIF_MCPS_DATA_IND         = 0x62,
  HIF_MLME_BEACON_NOTIFY_IND= 0x63,
  HIF_MLME_GET_REQ          = 0x64, /* not implemented */
  HIF_MLME_GET_CNF          = 0x65, /* not implemented */
  HIF_MLME_SET_REQ          = 0x66, /* not implemented */
  HIF_MLME_SET_CNF          = 0x67, /* not implemented */
  HIF_MLME_RESET_REQ        = 0x68,
  HIF_MLME_RESET_CNF        = 0x69,
  HIF_MLME_SCAN_REQ         = 0x6A,
  HIF_MLME_SCAN_CNF         = 0x6B,
  HIF_MLME_COMM_STATUS_IND  = 0x6C,
  HIF_MLME_START_REQ        = 0x6D,
  HIF_MLME_START_CNF        = 0x6E,

  HIF_ADPD_DATA_REQ         = 0x80,
  HIF_ADPD_DATA_CNF         = 0x81,
  HIF_ADPD_DATA_IND         = 0x82,
  HIF_ADPM_DISCOVERY_REQ    = 0x83,
  HIF_ADPM_DISCOVERY_CNF    = 0x84,
  HIF_ADPM_NTWSTART_REQ     = 0x85,
  HIF_ADPM_NTWSTART_CNF     = 0x86,
  HIF_ADPM_NTWJOIN_REQ      = 0x87,
  HIF_ADPM_NTWJOIN_CNF      = 0x88,
  HIF_ADPM_NTWLEAVE_REQ     = 0x89,
  HIF_ADPM_NTWLEAVE_CNF     = 0x8A,
  HIF_ADPM_NTWLEAVE_IND     = 0x8B,
  HIF_ADPM_RESET_REQ        = 0x8C,
  HIF_ADPM_RESET_CNF        = 0x8D,
  HIF_ADPM_GET_REQ          = 0x8E, /* not implemented */
  HIF_ADPM_GET_CNF          = 0x8F, /* not implemented */
  HIF_ADPM_SET_REQ          = 0x90, /* not implemented */
  HIF_ADPM_SET_CNF          = 0x91, /* not implemented */
  HIF_ADPM_NTWSTATUS_IND    = 0x92,
  HIF_ADPM_ROUTEDISCO_REQ   = 0x93,
  HIF_ADPM_ROUTEDISCO_CNF   = 0x94,
  HIF_ADPM_PATHDISCO_REQ    = 0x95,
  HIF_ADPM_PATHDISCO_CNF    = 0x96,
  HIF_ADPM_LBP_REQ          = 0x97,
  HIF_ADPM_LBP_CNF          = 0x98,
  HIF_ADPM_LBP_IND          = 0x99,
  HIF_ADPD_ROUTEOVER_REQ    = 0x9A,
  HIF_ADPD_ROUTEDEL_REQ     = 0x9B,
  HIF_ADPD_ROUTEDEL_CNF     = 0x9C,

  HIF_BOOT_SERVER_START_REQ = 0xB0,
  HIF_BOOT_SERVER_START_CNF = 0xB1,
  HIF_BOOT_SERVER_STOP_REQ  = 0xB2,
  HIF_BOOT_SERVER_STOP_CNF  = 0xB3,
  HIF_BOOT_SERVER_LEAVE_IND = 0xB4,
  HIF_BOOT_SERVER_KICK_REQ  = 0xB5,
  HIF_BOOT_SERVER_KICK_CNF  = 0xB6,
  HIF_BOOT_SERVER_JOIN_IND  = 0xB7,
  HIF_BOOT_DEVICE_START_REQ = 0xB8,
  HIF_BOOT_DEVICE_START_CNF = 0xB9,
  HIF_BOOT_DEVICE_LEAVE_REQ = 0xBA,
  HIF_BOOT_DEVICE_LEAVE_CNF = 0xBB,
  HIF_BOOT_DEVICE_LEAVE_IND = 0xBC,

  HIF_BOOT_DEVICE_PANSORT_IND = 0xC1,
  HIF_BOOT_DEVICE_PANSORT_REQ = 0xC2,
  HIF_BOOT_DEVICE_PANSORT_CNF = 0xC3,
  HIF_BOOT_SERVER_GETPSK_IND = 0xC4,
  HIF_BOOT_SERVER_SETPSK_REQ = 0xC5,
  HIF_BOOT_SERVER_SETPSK_CNF = 0xC6,

  HIF_UDP_DATA_REQ          = 0xD0,
  HIF_UDP_DATA_CNF          = 0xD1,
  HIF_UDP_DATA_IND          = 0xD2,
  HIF_UDP_CONN_SET_REQ      = 0xD3,
  HIF_UDP_CONN_SET_CNF      = 0xD4,
  HIF_UDP_CONN_GET_REQ      = 0xD5,
  HIF_UDP_CONN_GET_CNF      = 0xD6,
  HIF_ICMP_ECHO_REQ         = 0xD7,
  HIF_ICMP_ECHO_CNF         = 0xD8,
  HIF_ICMP_ECHO_IND         = 0xD9,
  HIF_ICMP_ECHO_REQ_IND     = 0xDF,
  HIF_ERROR_IND         = 0xFF,

} hif_cmdIdHostif_t;

/**
  * @}
  */


#endif /* HI_GENERIC_HDL_H */
/*******************************************************************************
* FUNCTIONS:
*******************************************************************************/

/** @defgroup hif_g3_common_Public_Functions HIF G3 COMMON Public Functions
  * @{
  */

/**
  * @}
  */

#endif /* HIF_G3_COMMON_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
