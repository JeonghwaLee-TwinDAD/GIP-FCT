/**
  ******************************************************************************
  * @file    user_g3_common.c
  * @author  AMG/IPC Application Team
  * @brief   This file contains code that can be used by both g3 and user tasks.
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


/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>
#include <task_comm.h>
#include <cmsis_os2.h>
#include <assert.h>
#include <user_g3_common.h>
#include <user_task.h>
#include <hi_g3lib_sap_interface.h>
#include <g3_app_attrib_tbl.h>
#include <hi_ip_sap_interface.h>
#include <hi_msgs_impl.h>
#include <g3_app_config.h>
#include <hif_g3_common.h>
#include <g3_hif.h>
#include <user_term_rxtx.h>
#include <user_if_fsm.h>
#include <utils.h>
#include <stm32g0xx_hal.h>
#include <mem_pool.h>

/* Private constants and macros-----------------------------------------------*/
#define COORD_CONNECTION_ID 1U
#define DEVICE_CONNECTION_ID 2U

#define USER_UDP_HANDLE 5U
#define USER_ICMP_ECHO_HANDLE 1U

#define COORD_LOCAL_PORT 0x1111
#define COORD_REMOTE_PORT 0x2222

#define DEVICE_LOCAL_PORT 0x2222
#define DEVICE_REMOTE_PORT 0x1111

#define RAISE_USER_EVENT(eVENT)           (UserG3_CommonEvt.UserEvents |= eVENT)

/* Local types ---------------------------------------------------------------*/
typedef enum
{
  CONF_ST_WAIT_FOR_DEFAULT_START_CNF,
  CONF_ST_WAIT_FOR_UDP_CONN_SET_CNF,
  CONF_ST_WAIT_FOR_UDP_DATA_CNF,
  CONF_ST_WAIT_FOR_ICMP_ECHO_CNF,
  CONF_ST_WAIT_FOR_DEV_LEAVE_CNF,
  CONF_ST_WAIT_FOR_COORD_LEAVE_CNF,
  CONF_ST_CNT
} user_conf_state_t;

typedef enum
{
  CONF_EV_NONE,
  READY_TO_START_CONNECTION,
  READY_TO_START_UDP_TRANSFER,
  DEVICE_NEED_COORD_ADDR_BEFORE_STARTING_CONNECTION,
  DEVICE_READY_TO_LEAVE_CONNECTION,
  COORD_READY_TO_LEAVE_CONNECTION,
  CONF_EV_CNT
} user_conf_event_t;

typedef enum
{
  NO_ICMPECHOREQIND_RCVED,
  WAITING_FOR_ICMPECHOREQIND,
  ICMPECHOREQIND_RCVED
} user_IcmpEchoReqIndStatus_t;

static struct
{
  user_conf_state_t curr_state;
  user_conf_event_t curr_event;
  uint16_t user_set_evts_cnt;
} user_conf_fsm_info;

typedef struct
{
  bool WaitForTermFsmIcmpEchoReqCnf;
  user_IcmpEchoReqIndStatus_t IcmpEchoReqIndSts;
  bool WaitForTermFsmUDpDataCnf; /* Workaround for spurious UDP-Data.Cnf reception issue, meanwhile investigation are led ST8500 side. AT_TEAM: to be removed afterwards */
} UserG3_CommonCtx_t;

/* Local variables -----------------------------------------------------------*/
user_info_t UserInfo;
static const uint8_t icmp_echo_req_data[] = {0xA, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
static User_Term_CmdBody_t data_to_send; // To be reworked
static task_comm_info_t *g3_task_comm_info;
UserG3_CommonCtx_t UserG3_CommonCtx;

/* Private function prototypes -----------------------------------------------*/
static user_conf_state_t UserG3_DefaultFnc(void);
static void UserG3_HandleBootServerJoinInd(const g3_user_msg_t *msg);
static void UserG3_HandleUdpDataInd(const g3_user_msg_t *msg);
static void UserG3_HandleDeviceStartCnf(const g3_user_msg_t *msg);
static void UserG3_HandleConnSetCnf(const g3_user_msg_t *msg);
static void UserG3_HandleUdpDataCnf(const g3_user_msg_t *msg);
static uint16_t UserG3_MacGetAddrLen(const MAC_DeviceAddress_t *addr);
static void UserG3_HandleIcmpEchoReqInd(const g3_user_msg_t *msg);
static void UserG3_HandleIcmpEchoCnf(const g3_user_msg_t *msg);
static void UserG3_HandleDbgToolCnf(const g3_user_msg_t * msg);
static user_conf_state_t UserG3_SendBootDeviceLeaveReq(void);
static user_conf_state_t UserG3_SendConnSetReq(void);
static user_conf_state_t UserG3_SendIcmpEchoReq(void);
static user_conf_state_t UserG3_SendUdpDataReq(void);

/* Exported variables --------------------------------------------------------*/ 
UserG3_CommonEventData_t UserG3_CommonEvt;

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Function thet puts a message in the given queue
  * @param  QueueId Queue in which the message should be put
  * @param  Msg Hand on message to send
  * @param  WaitTime Waiting time on OS side
  * @retval None
  */
void UserG3_SendMessage(osMessageQueueId_t QueueId, g3_user_msg_t * Msg, uint8_t WaitTime)
{
  osMessageQueuePut(QueueId, Msg, NULL, WaitTime); 
}

/**
  * @brief  Function thet checks if a message is received in the given queue
  * @param  QueueId Queue from which the message should be got
  * @param  Buff Ptr to the message to send
  * @param  WaitTime Waiting time on OS side
  * @retval Status Status of the OS receiving function
  */
bool UserG3_ReceiveMessage(osMessageQueueId_t QueueId, g3_user_msg_t *Buff, uint8_t WaitTime)
{
  osStatus_t tmp;
  tmp = osMessageQueueGet(QueueId, Buff, NULL, WaitTime);
  if (tmp == osOK)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**
  * @brief  Function that initializes the User task variables
  * @param  g3_task_comm_info_ Ptr to the G3 task structure
  * @retval None
  */
void UserG3_InitVars(task_comm_info_t *g3_task_comm_info_)
{
  g3_task_comm_info = g3_task_comm_info_;
    
 /* Init the UserInfo.UserCoordInfo variables*/
  UserInfo.UserCoordInfo.DestShortAddress = 0U;
  UserInfo.UserCoordInfo.PanId = 0U;
  UserInfo.UserCoordInfo.ConnId = 0U;
  UserInfo.UserCoordInfo.RemotePort = 0U;
  UserInfo.UserCoordInfo.LocalPort = 0U;
  UserInfo.UserCoordInfo.IcmpEchoHandle = 0U;

  /* Init the UserInfo.UserDeviceInfo variables*/
  UserInfo.UserDeviceInfo.DestShortAddress = 0U;
  UserInfo.UserDeviceInfo.PanId = 0U;
  UserInfo.UserDeviceInfo.ConnId = 0U;
  UserInfo.UserDeviceInfo.RemotePort = 0U;
  UserInfo.UserDeviceInfo.LocalPort = 0U;
  
  /* Init UserG3_CommonEvt struct */
  memset(&UserG3_CommonEvt, 0U, sizeof(UserG3_CommonEventData_t));

  /* Init the variables for the Terminal */
  UserG3_CommonCtx.WaitForTermFsmIcmpEchoReqCnf = false;
  UserG3_CommonCtx.IcmpEchoReqIndSts = NO_ICMPECHOREQIND_RCVED;
  UserG3_CommonCtx.WaitForTermFsmUDpDataCnf = false;  /* Workaround for spurious UDP-Data.Cnf reception issue, meanwhile investigation are led ST8500 side. AT_TEAM: to be removed afterwards */
}

/**
  * @brief This function sends a G3ICMP-ECHO.Request to the ST8500 in case the board is the Coordinator
  * @retval None
  */
void UserG3_LaunchUdpConnSet(void)
{
  if  (dev_type == DEV_TYPE_COORD)
  {
    /* It is assumed that G3BOOT-SRV-JOIN.Ind has been already received by Coord at that level, proceed with ICMP-ECHO.Ind */
    UserG3_SendIcmpEchoReq();
    UserG3_CommonCtx.WaitForTermFsmIcmpEchoReqCnf = true;
  }
  else /* (dev_type == DEV_TYPE_DEVICE) */
  {
    if (UserG3_CommonCtx.IcmpEchoReqIndSts == ICMPECHOREQIND_RCVED)
    {
      /* ICMP-ECHO.Ind message already received, proceed with a Connection Set Request */
      UserG3_SendConnSetReq();
    }
    else
    {
      /* No ICMP-ECHO.Ind message received so far, initiate a waiting phase for it... */
      UserG3_CommonCtx.IcmpEchoReqIndSts = WAITING_FOR_ICMPECHOREQIND;
    }
  }
}

/**
  * @brief This function prepares the buffer with the data to send before sending a G3UDP-DATA.Request message to ST8500
  * @param  DataToSend Ptr to the buffer containing the data to send
  * @param  DataLength Number of data to send
  * @retval None
  */
void UserG3_StartUdpDataTransfer(void * DataToSend, uint32_t DataLength)
{
  uint32_t i;
  for (i=0; i<TERM_CMD_PAYLD_MAX_SIZE;i++)
  {
    data_to_send.Payload[i] = *((uint8_t *)DataToSend+i);
  }
  data_to_send.Length = DataLength;

  UserG3_SendUdpDataReq();
}

/**
  * @brief  Function that launches the starting time in order to measure the time
  * @retval None
  */
void UserG3_StartTimeCounter(void)
{
  UserG3_CommonEvt.LoopbackStartTime = HAL_GetTick();
}

/**
  * @brief  Function that computes the loopback duration
  * @retval None
  */
void UserG3_StopTimecounter(void)
{
  UserG3_CommonEvt.LoopbackEndTime = HAL_GetTick();
}

/**
  * @brief  Function that handles the messages coming from the G3 task
  * @param  msg Ptr to the received message
  * @retval None
  */
void UserG3_MsgHandler(const g3_user_msg_t *msg)
{  
  switch (msg->cmd_id) {
    
  case USER_HI_HWRESET_REQ:
    break;
    
  case USER_G3LIB_SWRESET_CNF:
    break;
    
  case USER_BOOT_DEVICE_PANSORT_IND:
    RAISE_USER_EVENT(USEREVT_G3_PANSORTIND_RCVED);
    break;
    
  case USER_HI_RFCONFIGSET_CNF:    
    break;

  case USER_BOOT_DEVICE_START_CNF:   
    UserG3_HandleDeviceStartCnf(msg);
    RAISE_USER_EVENT(USEREVT_G3_BOOTDEVICESTARTCNF_RCVED);
    break;
    
  case USER_BOOT_SERVER_JOIN_IND:
    UserG3_HandleBootServerJoinInd(msg);  
#ifdef TU_WO_SERIAL_TERMINAL    
    user_conf_fsm_info.curr_event = DEVICE_NEED_COORD_ADDR_BEFORE_STARTING_CONNECTION;
#endif /* TU_WO_SERIAL_TERMINAL */    
    RAISE_USER_EVENT(USEREVT_G3_BOOTSERVERJOININD_RCVED);
    break;
    
  case USER_BOOT_SERVER_GETPSK_IND:
    break;
    
  case USER_BOOT_SERVER_SETPSK_CNF:
    break;

  case USER_ICMP_ECHO_REQ_IND:
    if(dev_type == DEV_TYPE_DEVICE)
    {
      UserG3_HandleIcmpEchoReqInd(msg);

#ifdef TU_WO_SERIAL_TERMINAL       
      user_conf_fsm_info.curr_event = READY_TO_START_CONNECTION;
#else /* TU_WO_SERIAL_TERMINAL */
      if (UserG3_CommonCtx.IcmpEchoReqIndSts == NO_ICMPECHOREQIND_RCVED)
      {
        /* ICMP-ECHO.Ind message is received when Device does not wait for it yet: update status */
        UserG3_CommonCtx.IcmpEchoReqIndSts = ICMPECHOREQIND_RCVED;
      }
      else if (UserG3_CommonCtx.IcmpEchoReqIndSts == WAITING_FOR_ICMPECHOREQIND)
      {
        /* ICMP-ECHO.Ind message is received while Device was expecting it to proceed with a Connection Set Request */
        UserG3_CommonCtx.IcmpEchoReqIndSts = ICMPECHOREQIND_RCVED;
        UserG3_SendConnSetReq();
      }
      else {}
#endif /* TU_WO_SERIAL_TERMINAL */
    }
    RAISE_USER_EVENT(USEREVT_G3_ICMPECHOREQIND_RCVED);
    break;    
    
  case USER_ICMP_ECHO_CNF:
     UserG3_HandleIcmpEchoCnf(msg);
     
#ifdef TU_WO_SERIAL_TERMINAL       
     if ((dev_type == DEV_TYPE_COORD) 
         && (UserInfo.UserCoordInfo.IcmpEchoHandle == USER_ICMP_ECHO_HANDLE) 
           && (user_conf_fsm_info.curr_state == CONF_ST_WAIT_FOR_ICMP_ECHO_CNF))
     {    
       user_conf_fsm_info.curr_event = READY_TO_START_CONNECTION;
     }
#else /* TU_WO_SERIAL_TERMINAL */
     if (UserG3_CommonCtx.WaitForTermFsmIcmpEchoReqCnf)
     {
       UserG3_CommonCtx.WaitForTermFsmIcmpEchoReqCnf = false;
       UserG3_SendConnSetReq();
     }
#endif /* TU_WO_SERIAL_TERMINAL */
    break;
    
  case USER_UDP_CONN_SET_CNF:
#ifdef TU_WO_SERIAL_TERMINAL          
    if (user_conf_fsm_info.curr_state == CONF_ST_WAIT_FOR_UDP_CONN_SET_CNF)
    {
      user_conf_fsm_info.curr_event = READY_TO_START_UDP_TRANSFER;
      UserG3_HandleConnSetCnf(msg);
    }  
#else /* TU_WO_SERIAL_TERMINAL */   
    user_conf_fsm_info.curr_event = READY_TO_START_UDP_TRANSFER;

    UserG3_HandleConnSetCnf(msg);
#endif /* TU_WO_SERIAL_TERMINAL */
    RAISE_USER_EVENT(USEREVT_G3_UDPCONNSETCNF_RCVED);
    break;
    
  case USER_UDP_DATA_CNF:
#ifdef TU_WO_SERIAL_TERMINAL              
    if (user_conf_fsm_info.curr_state == CONF_ST_WAIT_FOR_UDP_DATA_CNF)
    {
      UserG3_HandleUdpDataCnf(msg);
    }    
#else /* TU_WO_SERIAL_TERMINAL */    
    if (UserG3_CommonCtx.WaitForTermFsmUDpDataCnf)  /* Workaround for spurious UDP-Data.Cnf reception issue, meanwhile investigation are led ST8500 side. AT_TEAM: to be removed afterwards */
    {
      UserG3_CommonCtx.WaitForTermFsmUDpDataCnf = false;
      UserG3_HandleUdpDataCnf(msg);
      RAISE_USER_EVENT(USEREVT_G3_UDPDATACNF_RCVED);
    }

#endif /* TU_WO_SERIAL_TERMINAL */
    break;
    
  case USER_UDP_DATA_IND:
    UserG3_HandleUdpDataInd(msg);

    if(dev_type == DEV_TYPE_DEVICE)
    {     
      user_conf_fsm_info.curr_event = DEVICE_READY_TO_LEAVE_CONNECTION;
    }
    else if (dev_type == DEV_TYPE_COORD)
    {  
      user_conf_fsm_info.curr_event = COORD_READY_TO_LEAVE_CONNECTION;       
    }
    RAISE_USER_EVENT(USEREVT_G3_UDPDATAIND_RCVED);
    break;

  case USER_BOOT_DEVICE_LEAVE_CNF:
    user_conf_fsm_info.curr_event = CONF_EV_NONE;  
    break;

  case HIF_BOOT_SERVER_STOP_CNF:
    user_conf_fsm_info.curr_event = CONF_EV_NONE;  
    break;

  case USER_HI_TOOLS_CNF:
    UserG3_HandleDbgToolCnf(msg);
    RAISE_USER_EVENT(USEREVT_G3_SYSTEMINFO_RCVED); 
    break;

  default:
    break;
  }
  
  /* Msg now consumed by User task:
     Msg data content stored in MemPool can now be freed  */
   assert(msg->hif_msg_handle != NULL);
   MEM_FREE(msg->hif_msg_handle);
}

/**
  * @brief  Function that captures the real size of the AdpMetaData structure (variable depending on BCO number)
  * @param  data_ind Ptr to IP_DataIndication_t structure 
  * @retval Length of the AdpMetaData structure
  */
uint16_t UserG3_IpDataGetMetaDataLen(const IP_DataIndication_t *data_ind)
{
    const IP_AdpMetaData_t *metadata = &data_ind->MetaData;
    uint16_t len = sizeof(*metadata);

    len -= sizeof(metadata->PreviousHop);

    len += UserG3_MacGetAddrLen(&metadata->PreviousHop);

    len -= sizeof(metadata->BC0List);
    len += sizeof(metadata->BC0List[0]) * metadata->BC0Num;

    return len;
}

/**
  * @brief  Function that returns a pointer on the data in the G3UDP-DATA.Indication message 
  * @param  data_ind Ptr to IP_DataIndication_t structure 
  * @retval Pointer to the data part of the G3UDP-DATA.Indication message
  */
IP_UdpDataIndication_t *UserG3_UdpDataIndGetData(const IP_DataIndication_t *data_ind)
{
    const uint8_t *ind = (uint8_t *)data_ind;

    return (IP_UdpDataIndication_t *)(ind + UserG3_IpDataGetMetaDataLen(data_ind));
}

/**
  * @brief  Function that returns a pointer on the source address in the G3ICMP-ECHOREQ.Indication or G3UDP-DATA.Indication messages 
  * @param  data_ind Ptr to IP_DataIndication_t structure 
  * @retval Pointer to the source address
  */
ip6_addr_t *UserG3_IpDataIndGetSourceAddressPtr(const IP_DataIndication_t *data_ind)
{
  const uint8_t *ind = (uint8_t *)data_ind;

  return (ip6_addr_t *)(ind + (sizeof(data_ind->IcmpDataIndication.PrevHopMedia) + UserG3_IpDataGetMetaDataLen(data_ind)));
}

/**
  * @brief  Function that provides board system information (ST8500 FW version, PLC info,..) 
  * @note function handles the preparation and sending of USER_HI_TOOLS_REQ message to ST8500.
  * @retval None
  */
void UserG3_SystemInfoRequest(void)
{
  static hi_dbgtool_req_t dbgtool_data_req;
  uint16_t len;

  len = hi_dbgtool_req_set(&dbgtool_data_req, HI_TOOL_INFO, 0);

  task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, USER_HI_TOOLS_REQ, &dbgtool_data_req, len, NO_WAIT);
}


/* Private functions --------------------------------------------------------*/

/**
  * @brief  Function that returns the length of the MAC_DeviceAddress_t that is variable depending on the Mode field
  * @param  addr Pointer to the MAC_DeviceAddress_t structure 
  * @retval Length of the MAC_DeviceAddress_t structure
  */
static uint16_t UserG3_MacGetAddrLen(const MAC_DeviceAddress_t *addr)
{
    uint16_t len = sizeof(addr->Mode) +
        sizeof(addr->PanID);

    if (2U == addr->Mode)
        len += sizeof(addr->Short);
    else if (3U == addr->Mode)
        len += sizeof(addr->Extended);

    return len;
}

/**
  * @brief  Function that sets the USER task current state to CONF_EV_NONE
  * @retval None
*/
static user_conf_state_t UserG3_DefaultFnc(void)
{
  user_conf_fsm_info.curr_event = CONF_EV_NONE;
  
  return user_conf_fsm_info.curr_state;
}

/**
  * @brief  Function that handles the USER_BOOT_SERVER_JOIN_IND coming from the G3 task
  * @param  msg Ptr to the received message
  * @retval None
  */
static void UserG3_HandleBootServerJoinInd(const g3_user_msg_t *msg)
{
  const BOOT_ServerJoinIndication_t *srv_join_ind;
  srv_join_ind = (BOOT_ServerJoinIndication_t*)msg->buff;
  uint16_t short_addr = srv_join_ind->ShortAddress;
    
  UserInfo.UserCoordInfo.DestShortAddress = short_addr;
}

/**
  * @brief  Function that handles the USER_BOOT_DEVICE_START_CNF coming from the G3 task
  * @param  msg Ptr to the received message
  * @retval None
  */
static void UserG3_HandleDeviceStartCnf(const g3_user_msg_t *msg)
{
  const BOOT_DeviceStartConfirm_t *dev_start_cnf;
  dev_start_cnf = (BOOT_DeviceStartConfirm_t*)msg->buff;
  
  if (!dev_start_cnf->Status) 
  {
    UserInfo.UserDeviceInfo.PanId = dev_start_cnf->PANId;
  }
}

/**
  * @brief  Function that handles the USER_ICMP_ECHO_CNF coming from the G3 task
  * @param  msg Ptr to the received message
  * @retval None
  */
static void UserG3_HandleIcmpEchoCnf(const g3_user_msg_t *msg)
{
  const IP_G3IcmpDataConfirm_t *icmp_echo_cnf;
  icmp_echo_cnf = (IP_G3IcmpDataConfirm_t*)msg->buff;
  
  if (!icmp_echo_cnf->Status) 
  {
    UserInfo.UserCoordInfo.IcmpEchoHandle = icmp_echo_cnf->Handle;
  }
}

/**
  * @brief  Function that handles the USER_ICMP_ECHO_REQ_IND coming from the G3 task
  * @param  msg Ptr to the received message
  * @retval None
  */
static void UserG3_HandleIcmpEchoReqInd(const g3_user_msg_t *msg)
{
  const IP_DataIndication_t *icmp_echo_ind;
  icmp_echo_ind = (IP_DataIndication_t *)msg->buff;
  uint16_t src_pan_id, src_short_addr;
  
  const ip6_addr_t * p_source_address = UserG3_IpDataIndGetSourceAddressPtr(icmp_echo_ind);
  
  /* Get back the Coordinator Short Address */
  hi_ipv6_get_saddr_panid(p_source_address, &src_pan_id, &src_short_addr);
  
  UserInfo.UserDeviceInfo.DestShortAddress = src_short_addr;
}

/**
  * @brief  Function that handles the USER_UDP_CONN_SET_CNF coming from the G3 task
  * @param  msg Ptr to the received message
  * @retval None
  */
static void UserG3_HandleConnSetCnf(const g3_user_msg_t *msg)
{
#ifndef NDEBUG
  const IP_UdpConnSetConfirm_t *conn_set_cnf;
  conn_set_cnf = (IP_UdpConnSetConfirm_t*)msg->buff;

  assert(conn_set_cnf->Status == G3_SUCCESS); 
#endif /* NDEBUG */
}

/**
  * @brief  Function that handles the USER_UDP_DATA_CNF coming from the G3 task
  * @param  msg Ptr to the received message
  * @retval None
  */
static void UserG3_HandleUdpDataCnf(const g3_user_msg_t *msg)
{
#ifndef NDEBUG
  const IP_G3UdpDataConfirm_t *udp_data_cnf;
  udp_data_cnf = (IP_G3UdpDataConfirm_t*)msg->buff;
  
  assert(udp_data_cnf->Status == G3_SUCCESS);
#endif /* NDEBUG */
}

/**
  * @brief  Function that handles the USER_UDP_DATA_IND coming from the G3 task
  * @param  msg Ptr to the received message
  * @retval None
  */
static void UserG3_HandleUdpDataInd(const g3_user_msg_t *msg)
{
#ifdef TU_WO_SERIAL_TERMINAL              
  static volatile uint8_t data_0;
  static volatile uint8_t data_1;
  static volatile uint8_t data_2;
  static volatile uint8_t data_3;
#endif /* TU_WO_SERIAL_TERMINAL */
  
  const IP_DataIndication_t *ip_udp_ind;
  ip_udp_ind = (IP_DataIndication_t*)msg->buff;
  const IP_UdpDataIndication_t *udp_data_ind = UserG3_UdpDataIndGetData(ip_udp_ind); 
  
#ifdef TU_WO_SERIAL_TERMINAL              
  data_0 = udp_data_ind->Data[0];
  data_1 = udp_data_ind->Data[1];
  data_2 = udp_data_ind->Data[2];
  data_3 = udp_data_ind->Data[3];
#endif /* TU_WO_SERIAL_TERMINAL */
  
  UserG3_CommonEvt.UdpData.Length = (uint32_t)udp_data_ind->DataLen;
  UserG3_CommonEvt.UdpData.pPayload = (uint8_t *)udp_data_ind->Data;
}

/**
  * @brief  Function that handles the USER_HI_TOOLS_CNF coming from the G3 task
  * @param  msg Ptr to the received message
  * @retval None
  */
static void UserG3_HandleDbgToolCnf(const g3_user_msg_t * msg)
{
  memcpy(&(UserG3_CommonEvt.PlatformInfo), msg->buff, sizeof(hi_rdbg_conf_t));
}

/**
  * @brief  Function that handles the preparation and sending of HIF_ICMP_ECHO_REQ message to ST8500
  * @retval None
  */
static user_conf_state_t UserG3_SendIcmpEchoReq(void)
{
  ip6_addr_t ip_dst_addr;
  uint16_t len;
  static IP_G3IcmpDataRequest_t icmp_data_req;
  
  /* The HIF_ICMP_ECHO_REQ is sent by the coordinator so that the device goes a HIF_ICMP_ECHO_IND, a thus the coord MAC short address */
  UserInfo.Handle = USER_ICMP_ECHO_HANDLE;

  /* Get the Coord PAN Id */
  UserInfo.UserCoordInfo.PanId = g3_app_attrib_tbl_get_coord_pan_id();
  
  /* Compute the IPv6 remote address, from the short address */
  hi_ipv6_set_ipaddr(&ip_dst_addr, UserInfo.UserCoordInfo.PanId, UserInfo.UserCoordInfo.DestShortAddress); 
  
  len = hi_ipv6_echo_req_set(&icmp_data_req, &ip_dst_addr, UserInfo.Handle,
                             sizeof(icmp_echo_req_data), icmp_echo_req_data);
  
  //    hif_uart_send_message(HIF_ICMP_ECHO_REQ, &icmp_data_req, req_len);
  
  task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_ICMP_ECHO_REQ, &icmp_data_req, len, NO_WAIT);
  
  return CONF_ST_WAIT_FOR_ICMP_ECHO_CNF;

}

/**
  * @brief  Function that handles the preparation and sending of HIF_UDP_CONN_SET_REQ message to ST8500
  * @retval None
  */
static user_conf_state_t UserG3_SendConnSetReq(void)
{
  static IP_UdpConnSetRequest_t udp_conn_set_req;
  ip6_addr_t ip_dst_addr;
  uint16_t len;
  
  user_conf_fsm_info.curr_event = CONF_EV_NONE;
  
  if(dev_type == DEV_TYPE_COORD)
  {  
    /* Get the Coord PAN Id */
    UserInfo.UserCoordInfo.PanId = g3_app_attrib_tbl_get_coord_pan_id();
    
    /* Compute the IPv6 remote address, from the short address */
    hi_ipv6_set_ipaddr(&ip_dst_addr, UserInfo.UserCoordInfo.PanId, UserInfo.UserCoordInfo.DestShortAddress); 
    
    /* Set the ConnId, RemotePort and LocalPort */
    UserInfo.UserCoordInfo.ConnId = COORD_CONNECTION_ID;
    UserInfo.UserCoordInfo.RemotePort = COORD_REMOTE_PORT;
    UserInfo.UserCoordInfo.LocalPort = COORD_LOCAL_PORT;
    UserInfo.UserCoordInfo.DestIpv6Address = ip_dst_addr;
    
    len = hi_ipv6_udp_conn_req_set(&udp_conn_set_req, UserInfo.UserCoordInfo.ConnId, &ip_dst_addr, UserInfo.UserCoordInfo.RemotePort, UserInfo.UserCoordInfo.LocalPort);      
  }
  else if (dev_type == DEV_TYPE_DEVICE)
  {  
    /* Compute the IPv6 remote address, from the short address */
    hi_ipv6_set_ipaddr(&ip_dst_addr, UserInfo.UserDeviceInfo.PanId, UserInfo.UserDeviceInfo.DestShortAddress); 
    
    /* Set the ConnId, RemotePort and LocalPort */
    UserInfo.UserDeviceInfo.ConnId = DEVICE_CONNECTION_ID;
    UserInfo.UserDeviceInfo.RemotePort = DEVICE_REMOTE_PORT;
    UserInfo.UserDeviceInfo.LocalPort = DEVICE_LOCAL_PORT;
    UserInfo.UserDeviceInfo.DestIpv6Address = ip_dst_addr;
    
    len = hi_ipv6_udp_conn_req_set(&udp_conn_set_req, UserInfo.UserDeviceInfo.ConnId, &ip_dst_addr, UserInfo.UserDeviceInfo.RemotePort, UserInfo.UserDeviceInfo.LocalPort);     
  }
  
  /* Send the message to ST8500 */
  task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_UDP_CONN_SET_REQ, &udp_conn_set_req, len, NO_WAIT);
  
  return CONF_ST_WAIT_FOR_UDP_CONN_SET_CNF;
}

/**
  * @brief  Function that handles the preparation and sending of HIF_UDP_DATA_REQ message to ST8500
  * @retval None
  */
static user_conf_state_t UserG3_SendUdpDataReq(void)
{
  static IP_G3UdpDataRequest_t udp_data_req;
  uint16_t len;
  user_conf_state_t output_value;
 
#ifdef TU_WO_SERIAL_TERMINAL
  uint32_t i;
  if(dev_type == DEV_TYPE_COORD)
  {
    osDelay(1000*osKernelGetTickFreq()/1000);
    
    for (i=0; i<TERM_CMD_PAYLD_MAX_SIZE;i++)
    {
      data_to_send.Payload[i] = i+5;
    }
    
    data_to_send.Length = TERM_CMD_PAYLD_MAX_SIZE;
     
    output_value = CONF_ST_WAIT_FOR_COORD_LEAVE_CNF;
  }  
  else /* dev_type == DEV_TYPE_DEVICE */
  {
    osDelay(3000*osKernelGetTickFreq()/1000);
    
    for (i=0; i<TERM_CMD_PAYLD_MAX_SIZE;i++)
    {
      data_to_send.Payload[i] = i;
    }
    
    data_to_send.Length = TERM_CMD_PAYLD_MAX_SIZE;
    
    output_value = CONF_ST_WAIT_FOR_DEV_LEAVE_CNF;
  }
#else /* TU_WO_SERIAL_TERMINAL */  
  output_value = CONF_ST_WAIT_FOR_DEFAULT_START_CNF;
#endif /* TU_WO_SERIAL_TERMINAL */   
  
  
  UserInfo.Handle = USER_UDP_HANDLE;
  if(dev_type == DEV_TYPE_COORD)
  {
    len = hi_ipv6_udp_data_req_set(&udp_data_req,&UserInfo.UserCoordInfo.DestIpv6Address,UserInfo.Handle++,UserInfo.UserCoordInfo.RemotePort,data_to_send.Length,(uint8_t*)&data_to_send.Payload,UserInfo.UserCoordInfo.ConnId);
  }
  else
  {
    len = hi_ipv6_udp_data_req_set(&udp_data_req,&UserInfo.UserDeviceInfo.DestIpv6Address,UserInfo.Handle++,UserInfo.UserDeviceInfo.RemotePort,data_to_send.Length,(uint8_t*)&data_to_send.Payload,UserInfo.UserDeviceInfo.ConnId);
  }
  /* Send the message to ST8500 */
  task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_UDP_DATA_REQ, &udp_data_req, len, NO_WAIT);

  UserG3_CommonCtx.WaitForTermFsmUDpDataCnf = true;   /* Workaround for spurious UDP-Data.Cnf reception issue, meanwhile investigation are led ST8500 side. AT_TEAM: to be removed afterwards */
  
  return output_value;
}

/**
  * @brief  Function that handles the preparation and sending of HIF_BOOT_DEVICE_LEAVE_REQ message to ST8500
  * @retval None
  */
static user_conf_state_t UserG3_SendBootDeviceLeaveReq(void)
{
  /* We use a fake buffer address here to be able to check for errors (buff == NULL) */
  task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_BOOT_DEVICE_LEAVE_REQ, utils_get_fake_buff_addr(), 0, NO_WAIT);
   
  return CONF_ST_WAIT_FOR_DEFAULT_START_CNF;  
}

/**
  * @brief  Function that handles the preparation and sending of HIF_BOOT_SERVER_STOP_REQ message to ST8500
  * @retval None
  */
static user_conf_state_t UserG3_SendBootServerStopReq(void)
{
  /* We use a fake buffer address here to be able to check for errors (buff == NULL) */
  task_comm_put_send_buffer(g3_task_comm_info, G3_PROT, HIF_BOOT_SERVER_STOP_REQ, utils_get_fake_buff_addr(), 0, NO_WAIT);
   
  return CONF_ST_WAIT_FOR_DEFAULT_START_CNF;
}

/* This declaration should be here and not at the beginning of the file or else the functions are not known */
typedef user_conf_state_t user_conf_fsm_func(void);

static user_conf_fsm_func *user_conf_fsm_func_tbl[CONF_ST_CNT][CONF_EV_CNT] = {
    {UserG3_DefaultFnc, UserG3_SendConnSetReq, UserG3_DefaultFnc, UserG3_SendIcmpEchoReq, UserG3_DefaultFnc, UserG3_DefaultFnc},
    {UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_SendUdpDataReq, UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_DefaultFnc},
    {UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_DefaultFnc},
    {UserG3_DefaultFnc, UserG3_SendConnSetReq, UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_DefaultFnc},
    {UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_SendBootDeviceLeaveReq, UserG3_DefaultFnc},
    {UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_DefaultFnc, UserG3_SendBootServerStopReq}
};

/**
  * @brief  Function that handles the state of the User task FSM
  * @retval None
  */
void UserG3_AppConf(void)
{
  user_conf_fsm_info.curr_state = user_conf_fsm_func_tbl[user_conf_fsm_info.curr_state][user_conf_fsm_info.curr_event]();  
}



