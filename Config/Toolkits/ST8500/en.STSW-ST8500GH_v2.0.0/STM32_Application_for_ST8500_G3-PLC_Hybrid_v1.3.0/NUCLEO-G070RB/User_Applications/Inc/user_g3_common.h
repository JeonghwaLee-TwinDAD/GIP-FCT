/**
  ******************************************************************************
  * @file    user_g3_common.h
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

#ifndef G3_USER_COMMON_H
#define G3_USER_COMMON_H


/* Include files -------------------------------------------------------------*/
#include <hi_msgs_impl.h>
#include <hi_ip_sap_interface.h>
#include <hif_g3_common.h>

/** @addtogroup USER_DRIVER G3 USER DRIVER
  * @{
  */

/** @defgroup g3_user_common G3 USER COMMON
  * @{
  */

/** @defgroup g3_user_common_Constants G3 USER COMMON Constants
  * @{
  */

//#define TU_WO_SERIAL_TERMINAL

/* Exported constants --------------------------------------------------------*/
#define USEREVT_G3_PANSORTIND_RCVED_Pos                        ((uint32_t)(0U))
#define USEREVT_G3_BOOTSERVERJOININD_RCVED_Pos                 ((uint32_t)(1U))
#define USEREVT_G3_BOOTDEVICESTARTCNF_RCVED_Pos                ((uint32_t)(2U))
#define USEREVT_G3_ICMPECHOREQIND_RCVED_Pos                    ((uint32_t)(3U))
#define USEREVT_G3_UDPCONNSETCNF_RCVED_Pos                     ((uint32_t)(4U))
#define USEREVT_G3_UDPDATACNF_RCVED_Pos                        ((uint32_t)(5U))
#define USEREVT_G3_UDPDATAIND_RCVED_Pos                        ((uint32_t)(6U))
#define USEREVT_G3_BOOTDEVICELEAVE_RCVED_Pos                   ((uint32_t)(7U))
#define USEREVT_G3_BOOTSERVERSTOP_RCVED_Pos                    ((uint32_t)(8U))
#define USEREVT_G3_SYSTEMINFO_RCVED_Pos                        ((uint32_t)(9U))     /* corresponds to the reception of ST8500 HOSTIF-DBGTOOL.Confirm message */
#define USEREVT_LAST_POSITION                 (USEREVT_G3_SYSTEMINFO_RCVED_Pos) /* To update when appending new events! */

#define USEREVT_G3_PANSORTIND_RCVED                  (0x1UL << USEREVT_G3_PANSORTIND_RCVED_Pos)
#define USEREVT_G3_BOOTSERVERJOININD_RCVED    (0x1UL << USEREVT_G3_BOOTSERVERJOININD_RCVED_Pos)
#define USEREVT_G3_BOOTDEVICESTARTCNF_RCVED  (0x1UL << USEREVT_G3_BOOTDEVICESTARTCNF_RCVED_Pos)
#define USEREVT_G3_ICMPECHOREQIND_RCVED          (0x1UL << USEREVT_G3_ICMPECHOREQIND_RCVED_Pos)
#define USEREVT_G3_UDPCONNSETCNF_RCVED            (0x1UL << USEREVT_G3_UDPCONNSETCNF_RCVED_Pos)
#define USEREVT_G3_UDPDATACNF_RCVED                  (0x1UL << USEREVT_G3_UDPDATACNF_RCVED_Pos)
#define USEREVT_G3_UDPDATAIND_RCVED                  (0x1UL << USEREVT_G3_UDPDATAIND_RCVED_Pos)
#define USEREVT_G3_BOOTDEVICELEAVE_RCVED        (0x1UL << USEREVT_G3_BOOTDEVICELEAVE_RCVED_Pos)
#define USEREVT_G3_BOOTSERVERSTOP_RCVED          (0x1UL << USEREVT_G3_BOOTSERVERSTOP_RCVED_Pos)
#define USEREVT_G3_SYSTEMINFO_RCVED                  (0x1UL << USEREVT_G3_SYSTEMINFO_RCVED_Pos)


/* Exported types ------------------------------------------------------------*/

/* Application G3 Cmd_Id */
typedef enum
{
  USER_HI_HWRESET_REQ          = HIF_HI_HWRESET_REQ,
  USER_HI_HWRESET_CNF          = HIF_HI_HWRESET_CNF,
  USER_HI_MODE_SET_REQ         = HIF_HI_MODE_SET_REQ,
  USER_HI_MODE_SET_CNF         = HIF_HI_MODE_SET_CNF,
  USER_HI_MODE_GET_REQ         = HIF_HI_MODE_GET_REQ,
  USER_HI_MODE_GET_CNF         = HIF_HI_MODE_GET_CNF,
  USER_HI_BAUDRATE_SET_REQ     = HIF_HI_BAUDRATE_SET_REQ,
  USER_HI_BAUDRATE_SET_CNF     = HIF_HI_BAUDRATE_SET_CNF,
  USER_HI_RESETSTATE_REQ       = HIF_HI_RESETSTATE_REQ,
  USER_HI_RESETSTATE_CNF       = HIF_HI_RESETSTATE_CNF,
  USER_HI_TEST_SET_REQ         = HIF_HI_TEST_SET_REQ,
  USER_HI_TEST_SET_CNF         = HIF_HI_TEST_SET_CNF,
  USER_HI_TEST_GET_REQ         = HIF_HI_TEST_GET_REQ,
  USER_HI_TEST_GET_CNF         = HIF_HI_TEST_GET_CNF,
  USER_HI_SFLASH_REQ           = HIF_HI_SFLASH_REQ,
  USER_HI_SFLASH_CNF           = HIF_HI_SFLASH_CNF,
  USER_HI_NVRAM_REQ            = HIF_HI_NVRAM_REQ,
  USER_HI_NVRAM_CNF            = HIF_HI_NVRAM_CNF,
  USER_HI_TRACE_REQ            = HIF_HI_TRACE_REQ,
  USER_HI_TRACE_CNF            = HIF_HI_TRACE_CNF,
  USER_HI_TOOLS_REQ            = HIF_HI_TOOLS_REQ,
  USER_HI_TOOLS_CNF            = HIF_HI_TOOLS_CNF,
  USER_HI_SEC_INIT_REQ         = HIF_HI_SEC_INIT_REQ,
  USER_HI_SEC_INIT_CNF         = HIF_HI_SEC_INIT_CNF,
  USER_HI_RFCONFIGSET_REQ      = HIF_HI_RFCONFIGSET_REQ,
  USER_HI_RFCONFIGSET_CNF      = HIF_HI_RFCONFIGSET_CNF,
  USER_HI_RFCONFIGGET_REQ      = HIF_HI_RFCONFIGGET_REQ,
  USER_HI_RFCONFIGGET_CNF      = HIF_HI_RFCONFIGGET_CNF,
  USER_HI_OTP_REQ              = HIF_HI_OTP_REQ,
  USER_HI_OTP_CNF              = HIF_HI_OTP_CNF,
  USER_HI_READY_IND            = HIF_HI_READY_IND,

  USER_G3LIB_GET_REQ           = HIF_G3LIB_GET_REQ,
  USER_G3LIB_GET_CNF           = HIF_G3LIB_GET_CNF,
  USER_G3LIB_SET_REQ           = HIF_G3LIB_SET_REQ,
  USER_G3LIB_SET_CNF           = HIF_G3LIB_SET_CNF,
  USER_G3LIB_SWRESET_REQ       = HIF_G3LIB_SWRESET_REQ,
  USER_G3LIB_SWRESET_CNF       = HIF_G3LIB_SWRESET_CNF,
  USER_G3LIB_TESTMODE_REQ      = HIF_G3LIB_TESTMODE_REQ,
  USER_G3LIB_TESTMODE_CNF      = HIF_G3LIB_TESTMODE_CNF,
  USER_G3LIB_EVENT_NOTE_IND    = HIF_G3LIB_EVENT_NOTE_IND,
  USER_G3LIB_MULTIPLEGET_REQ   = HI_G3LIB_MULTIPLEGET_REQ,
  USER_G3LIB_MULTIPLEGET_CNF   = HI_G3LIB_MULTIPLEGET_CNF,

  USER_PD_DATA_REQ             = HIF_PD_DATA_REQ,
  USER_PD_DATA_CNF             = HIF_PD_DATA_CNF,
  USER_PD_DATA_IND             = HIF_PD_DATA_IND,
  USER_PD_ACK_REQ              = HIF_PD_ACK_REQ,
  USER_PD_ACK_CNF              = HIF_PD_ACK_CNF,
  USER_PD_ACK_IND              = HIF_PD_ACK_IND,
  USER_PHY_RF_REQ              = HIF_PHY_RF_REQ,
  USER_PHY_RF_CNF              = HIF_PHY_RF_CNF,
  USER_PHY_RF_IND              = HIF_PHY_RF_IND,
  USER_PLME_GET_CNF            = HIF_PLME_GET_CNF, 
  USER_PLME_SET_TRX_STATE_REQ  = HIF_PLME_SET_TRX_STATE_REQ,
  USER_PLME_SET_TRX_STATE_CNF  = HIF_PLME_SET_TRX_STATE_CNF,
  USER_PLME_CS_REQ             = HIF_PLME_CS_REQ,
  USER_PLME_CS_CNF             = HIF_PLME_CS_CNF,

  USER_MCPS_DATA_REQ           = HIF_MCPS_DATA_REQ,
  USER_MCPS_DATA_CNF           = HIF_MCPS_DATA_CNF,
  USER_MCPS_DATA_IND           = HIF_MCPS_DATA_IND,
  USER_MLME_BEACON_NOTIFY_IND  = HIF_MLME_BEACON_NOTIFY_IND,
  USER_MLME_GET_REQ            = HIF_MLME_GET_REQ, 
  USER_MLME_GET_CNF            = HIF_MLME_GET_CNF, 
  USER_MLME_SET_REQ            = HIF_MLME_SET_REQ, 
  USER_MLME_SET_CNF            = HIF_MLME_SET_CNF,
  USER_MLME_RESET_REQ          = HIF_MLME_RESET_REQ,
  USER_MLME_RESET_CNF          = HIF_MLME_RESET_CNF,
  USER_MLME_SCAN_REQ           = HIF_MLME_SCAN_REQ,
  USER_MLME_SCAN_CNF           = HIF_MLME_SCAN_CNF,
  USER_MLME_COMM_STATUS_IND    = HIF_MLME_COMM_STATUS_IND,
  USER_MLME_START_REQ          = HIF_MLME_START_REQ,
  USER_MLME_START_CNF          = HIF_MLME_START_CNF,

  USER_ADPD_DATA_REQ           = HIF_ADPD_DATA_REQ,
  USER_ADPD_DATA_CNF           = HIF_ADPD_DATA_CNF,
  USER_ADPD_DATA_IND           = HIF_ADPD_DATA_IND,
  USER_ADPM_DISCOVERY_REQ      = HIF_ADPM_DISCOVERY_REQ,
  USER_ADPM_DISCOVERY_CNF      = HIF_ADPM_DISCOVERY_CNF,
  USER_ADPM_NTWSTART_REQ       = HIF_ADPM_NTWSTART_REQ,
  USER_ADPM_NTWSTART_CNF       = HIF_ADPM_NTWSTART_CNF,
  USER_ADPM_NTWJOIN_REQ        = HIF_ADPM_NTWJOIN_REQ,
  USER_ADPM_NTWJOIN_CNF        = HIF_ADPM_NTWJOIN_CNF,
  USER_ADPM_NTWLEAVE_REQ       = HIF_ADPM_NTWLEAVE_REQ,
  USER_ADPM_NTWLEAVE_CNF       = HIF_ADPM_NTWLEAVE_CNF,
  USER_ADPM_NTWLEAVE_IND       = HIF_ADPM_NTWLEAVE_IND,
  USER_ADPM_RESET_REQ          = HIF_ADPM_RESET_REQ,
  USER_ADPM_RESET_CNF          = HIF_ADPM_RESET_CNF,
  USER_ADPM_GET_REQ            = HIF_ADPM_GET_REQ, 
  USER_ADPM_GET_CNF            = HIF_ADPM_GET_CNF, 
  USER_ADPM_SET_REQ            = HIF_ADPM_SET_REQ, 
  USER_ADPM_SET_CNF            = HIF_ADPM_SET_CNF, 
  USER_ADPM_NTWSTATUS_IND      = HIF_ADPM_NTWSTATUS_IND,
  USER_ADPM_ROUTEDISCO_REQ     = HIF_ADPM_ROUTEDISCO_REQ,
  USER_ADPM_ROUTEDISCO_CNF     = HIF_ADPM_ROUTEDISCO_CNF,
  USER_ADPM_PATHDISCO_REQ      = HIF_ADPM_PATHDISCO_REQ,
  USER_ADPM_PATHDISCO_CNF      = HIF_ADPM_PATHDISCO_CNF,
  USER_ADPM_LBP_REQ            = HIF_ADPM_LBP_REQ,
  USER_ADPM_LBP_CNF            = HIF_ADPM_LBP_CNF,
  USER_ADPM_LBP_IND            = HIF_ADPM_LBP_IND,
  USER_ADPD_ROUTEOVER_REQ      = HIF_ADPD_ROUTEOVER_REQ,
  USER_ADPD_ROUTEDEL_REQ       = HIF_ADPD_ROUTEDEL_REQ,
  USER_ADPD_ROUTEDEL_CNF       = HIF_ADPD_ROUTEDEL_CNF,

  USER_BOOT_SERVER_START_REQ   = HIF_BOOT_SERVER_START_REQ,
  USER_BOOT_SERVER_START_CNF   = HIF_BOOT_SERVER_START_CNF,
  USER_BOOT_SERVER_STOP_REQ    = HIF_BOOT_SERVER_STOP_REQ,
  USER_BOOT_SERVER_STOP_CNF    = HIF_BOOT_SERVER_STOP_CNF,
  USER_BOOT_SERVER_LEAVE_IND   = HIF_BOOT_SERVER_LEAVE_IND,
  USER_BOOT_SERVER_KICK_REQ    = HIF_BOOT_SERVER_KICK_REQ,
  USER_BOOT_SERVER_KICK_CNF    = HIF_BOOT_SERVER_KICK_CNF,
  USER_BOOT_SERVER_JOIN_IND    = HIF_BOOT_SERVER_JOIN_IND,
  USER_BOOT_DEVICE_START_REQ   = HIF_BOOT_DEVICE_START_REQ,
  USER_BOOT_DEVICE_START_CNF   = HIF_BOOT_DEVICE_START_CNF,
  USER_BOOT_DEVICE_LEAVE_REQ   = HIF_BOOT_DEVICE_LEAVE_REQ,
  USER_BOOT_DEVICE_LEAVE_CNF   = HIF_BOOT_DEVICE_LEAVE_CNF,
  USER_BOOT_DEVICE_LEAVE_IND   = HIF_BOOT_DEVICE_LEAVE_IND,

  USER_BOOT_DEVICE_PANSORT_IND = HIF_BOOT_DEVICE_PANSORT_IND,
  USER_BOOT_DEVICE_PANSORT_REQ = HIF_BOOT_DEVICE_PANSORT_REQ,
  USER_BOOT_DEVICE_PANSORT_CNF = HIF_BOOT_DEVICE_PANSORT_CNF,
  USER_BOOT_SERVER_GETPSK_IND  = HIF_BOOT_SERVER_GETPSK_IND,
  USER_BOOT_SERVER_SETPSK_REQ  = HIF_BOOT_SERVER_SETPSK_REQ,
  USER_BOOT_SERVER_SETPSK_CNF  = HIF_BOOT_SERVER_SETPSK_CNF,

  USER_UDP_DATA_REQ            = HIF_UDP_DATA_REQ,
  USER_UDP_DATA_CNF            = HIF_UDP_DATA_CNF,
  USER_UDP_DATA_IND            = HIF_UDP_DATA_IND,
  USER_UDP_CONN_SET_REQ        = HIF_UDP_CONN_SET_REQ,
  USER_UDP_CONN_SET_CNF        = HIF_UDP_CONN_SET_CNF,
  USER_UDP_CONN_GET_REQ        = HIF_UDP_CONN_GET_REQ,
  USER_UDP_CONN_GET_CNF        = HIF_UDP_CONN_GET_CNF,
  USER_ICMP_ECHO_REQ           = HIF_ICMP_ECHO_REQ,
  USER_ICMP_ECHO_CNF           = HIF_ICMP_ECHO_CNF,
  USER_ICMP_ECHO_IND           = HIF_ICMP_ECHO_IND,
  USER_ICMP_ECHO_REQ_IND       = HIF_ICMP_ECHO_REQ_IND,
  
  USER_ERROR_IND               = HIF_ERROR_IND,
} g3_cmdIdIf_t;

typedef struct
{
    uint8_t cmd_id;
    void * buff;
    size_t size;
    void * hif_msg_handle;
} g3_user_msg_t;


typedef struct
{
   uint16_t DestShortAddress;
   ip6_addr_t DestIpv6Address;
   uint16_t PanId;
   uint8_t  ConnId;
   uint16_t RemotePort;
   uint16_t LocalPort;
   uint8_t IcmpEchoHandle;
} user_coord_info_t;

typedef struct
{
   uint16_t DestShortAddress;
   ip6_addr_t DestIpv6Address;
   uint16_t PanId;
   uint8_t  ConnId;
   uint16_t RemotePort;
   uint16_t LocalPort;
} user_device_info_t;

typedef struct
{
  uint16_t Handle;
  user_coord_info_t UserCoordInfo; 
  user_device_info_t UserDeviceInfo;  
} user_info_t;

typedef struct
{
  uint32_t Length;
  uint8_t * pPayload;
} UserG3_UdpData_t;

typedef struct
{
  uint32_t UserEvents;
  UserG3_UdpData_t UdpData;
  uint32_t LoopbackStartTime;
  uint32_t LoopbackEndTime;
  hi_rdbg_conf_t PlatformInfo;
} UserG3_CommonEventData_t;


/**
  * @}
  */


/* Exported vars -------------------------------------------------------------*/
extern osMessageQueueId_t g3_to_user_queueHandle;
extern osMessageQueueId_t user_to_g3_queueHandle;
extern UserG3_CommonEventData_t UserG3_CommonEvt;

/* Exported functions ------------------------------------------------------- */

/** @defgroup g3_user_common_Public_Functions G3 USER COMMON Public Functions
  * @{
  */
void UserG3_SendMessage(osMessageQueueId_t QueueId, g3_user_msg_t * Msg, uint8_t WaitTime);
bool UserG3_ReceiveMessage(osMessageQueueId_t QueueId, g3_user_msg_t *Buff, uint8_t WaitTime);
void UserG3_LaunchUdpConnSet(void);
void UserG3_StartUdpDataTransfer(void * DataToSend, uint32_t DataLength);
void UserG3_MsgHandler(const g3_user_msg_t *msg);
void UserG3_AppConf(void);
void UserG3_InitVars(task_comm_info_t *g3_task_comm_info_);
IP_UdpDataIndication_t *UserG3_UdpDataIndGetData(const IP_DataIndication_t *data_ind);
uint16_t UserG3_IpDataGetMetaDataLen(const IP_DataIndication_t *data_ind);
ip6_addr_t *UserG3_IpDataIndGetSourceAddressPtr(const IP_DataIndication_t *data_ind);
void UserG3_StartTimeCounter(void);
void UserG3_StopTimecounter(void);
void UserG3_SystemInfoRequest(void);


/**
  * @}
  */

#endif /* G3_USER_COMMON_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
