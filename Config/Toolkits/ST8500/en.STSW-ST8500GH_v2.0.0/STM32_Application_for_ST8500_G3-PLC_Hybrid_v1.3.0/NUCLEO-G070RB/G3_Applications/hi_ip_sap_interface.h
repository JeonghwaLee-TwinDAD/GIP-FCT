/**
  ******************************************************************************
  * @file    hi_ip_sap_interface.h
  * @author  AMG/IPC Application Team
  * @brief   Header file for all functionalities of the interface between
  *          Ip and SAP levels.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HI_IP_SAP_INTERFACE_H
#define HI_IP_SAP_INTERFACE_H

#include <stdint.h>
#include "hi_ip_pib_attributes.h"
#include "hi_mac_pib_types.h"
#include "hi_adp_sap_interface.h"
#include "hi_msgs_impl.h"

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_phy_drv HIF PHY DRV
  * @{
  */

/** @defgroup IP_SAP_IF_TypesDefinitions IP-SAP Interface Types Definitions
  * @brief  IP Sap Interface types definitions
  * @{
  */

/* IP Commands parameters */

/**
  * @brief    ADP<->IPv6 Layer Interface Enumeration Values (Results and Status fields)
  * @details  These values are used to signal the results of ADP-SAP interface operations
  * @{
  */

#define IP_MAX_SDU_SIZE                 ((uint16_t)1500)

#pragma pack(1)

/**
  * @}
  */

/**
  * @brief    ADP<->IPv6 Layer Interface Enumeration Values (Results and Status fields)
  * @details  These values are used to signal the results of ADP-SAP interface operations
  * @{
  */
/**
  * @brief Meta Data received from ADP Layer
  */
typedef struct
{
  MAC_DeviceAddress_t  PreviousHop;      /**< @brief The MAC address of the previous hop  */
  uint8_t  LQI;                          /**< @brief The Link Quality Indicator  */
  uint8_t  PhaseDifferential;            /**< @brief The phase difference in multiples of 60 degrees of the last received MAC frame  */
  uint8_t  PayloadModType;               /**< @brief The modulation type used in the last received MAC frame  */
  uint8_t  PayloadModScheme;             /**< @brief The payload modulation scheme used in the last received MAC frame */
  uint8_t  ToneMap[PHY_TONEMAP_SIZE];    /**< @brief The Tone map parameter used in the last received MAC frame */
  uint8_t  BC0Num;                       /**< @brief Number of the BC0 headers of the 6LoWPAN broadcast corresponding to the received IPv6 multicast packet */
  uint8_t  BC0List[ADP_MAX_NUM_BC0];     /**< @brief List of BC0 headers of the 6LoWPAN broadcast packets corresponding to the received IPv6 multicast packet */
} IP_AdpMetaData_t;

/**
  * @brief  ICMP-DATA.Request parameters
  */
typedef struct
{
  ip6_addr_t  DestAddress;       /**< @brief The destination address for the ICMPv6 Packet  */
  uint8_t   Handle;                /**< @brief The Handle of the ICMPv6 Packet  */
  uint16_t  DataLen;               /**< @brief The Length of the ICMPv6 Packet  */
  uint8_t   Data[IP_MAX_SDU_SIZE]; /**< @brief The ICMPv6 Packet  */
} IP_G3IcmpDataRequest_t;

/**< @brief Minimum length of one ADP Data Request (no payload) */
#define G3_ICMP_DATA_REQ_MIN_LEN            ((uint16_t)sizeof(IP_G3IcmpDataRequest_t) - IP_MAX_SDU_SIZE)
#define G3_ICMP_DATA_REQ_CURR_LEN(data_req) (sizeof(IP_G3IcmpDataRequest_t) -   \
    sizeof((data_req)->Data) + (data_req)->DataLen)

/**
  * @brief  ICMP-DATA.Confirm parameters
  */
typedef struct
{
  uint8_t       Status;                /**< @brief The Status of the operation */
  uint8_t       Handle;                /**< @brief The Handle of the UDP transmission */
  uint8_t       NextHopMedium;         /**< @brief The MediaType */
} IP_G3IcmpDataConfirm_t;


/**
  * @brief  ICMP-DATA.Indication parameters
  */
typedef struct
{
  uint8_t          PrevHopMedia;          /**< @brief The MediaType */
  ip6_addr_t       SourceAddress;         /**< @brief The IPv6 source address of the ICMPv6 Packet  */
  uint16_t         DataLen;               /**< @brief The Length of the ICMP Echo Reply payload  */
  uint8_t          Data[IP_MAX_SDU_SIZE]; /**< @brief The ICMP Echo Reply payload  */

} IP_IcmpDataIndication_t;

/**< @brief Minimum length of one IP Data Indication (no payload) */
#define ICMP_DATA_IND_MIN_LEN  ((uint16_t)sizeof(IP_IcmpDataIndication_t) - IP_MAX_SDU_SIZE)

/**
  * @brief  UDP-DATA.Confirm and ICMP-DATA.Confirm parameters
  */
typedef struct
{
  uint8_t  Status;  /**< @brief The Status of the last Request operation */
  uint8_t  Handle;  /**< @brief The Handle specified in the correspondent .Request  */

} IP_DataConfirm_t;

/**
  * @brief  UDP-CONNECTION-SET.Request
  */
typedef struct
{
  uint8_t       ConnId;          /**< @brief The Index of the UDP connection to set */
  ip6_addr_t    RemoteAddress;   /**< @brief The remote IPv6 address (if set to all 0s it accepts UPD datagrams from any address)  */
  uint16_t      RemotePort;      /**< @brief The remote UDP port (if set to 0 it accepts UPD datagrams on any port)  */
  uint16_t      LocalPort;       /**< @brief The local UDP port  */

} IP_UdpConnSetRequest_t;

/**
  * @brief  UDP-CONNECTION-SET.Confirm
  */
typedef struct
{
  uint8_t       Status;          /**< @brief The Status of the operation */

} IP_UdpConnSetConfirm_t;

/**
  * @brief  UDP-CONNECTION-GET.Confirm
  */
typedef struct
{
  uint8_t       ConnId;          /**< @brief The Index of the UDP connection to get */

} IP_UdpConnGetRequest_t;

/**
  * @brief  UDP-CONNECTION-GET.Request
  */
typedef struct
{
  uint8_t       Status;          /**< @brief The Status of the operation */
  uint8_t       ConnId;          /**< @brief The Index of the UDP connection to get */
  ip6_addr_t    RemoteAddress;   /**< @brief The remote IPv6 address (if set to all 0s it accepts UPD datagrams from any address)  */
  uint16_t      RemotePort;      /**< @brief The remote UDP port (if set to 0 it accepts UPD datagrams on any port)  */
  uint16_t      LocalPort;       /**< @brief The local UDP port  */

} IP_UdpConnGetConfirm_t;

/**
  * @brief  UDP-DATA.Request parameters
  */
typedef struct
{
  uint8_t       Handle;                /**< @brief The Handle of the UDP transmission */
  uint8_t       ConnId;                /**< @brief The ID of the UDP connection to be used to send this datagram */
  ip6_addr_t    DestAddress;           /**< @brief The IPv6 destination address (if set to all 0s the remote address of the UDP connection is used)  */
  uint16_t      DestPort;              /**< @brief The ID of the UDP connection to be used to send this datagram */
  uint16_t      DataLen;               /**< @brief The Length of the UDP payload  */
  uint8_t       Data[IP_MAX_SDU_SIZE]; /**< @brief The UDP payload  */

} IP_G3UdpDataRequest_t;

/**< @brief Minimum length of one ADP Data Request (no payload) */
#define G3_UDP_DATA_REQ_MIN_LEN  ((uint16_t)sizeof(IP_G3UdpDataRequest_t) - IP_MAX_SDU_SIZE)

#define G3_UDP_DATA_REQ_CURR_LEN(data_req) (sizeof(IP_G3UdpDataRequest_t) -   \
    sizeof((data_req)->Data) + (data_req)->DataLen)



/**
  * @brief  UDP-DATA.Confirm parameters
  */
typedef struct
{
  uint8_t       Status;                /**< @brief The Status of the operation */
  uint8_t       Handle;                /**< @brief The Handle of the UDP transmission */
  uint8_t       NextHopMedium;         /**< @brief The MediaType */
} IP_G3UdpDataConfirm_t;

/**
  * @brief  UDP-DATA.Indication parameters
  */
typedef struct
{
  uint8_t          PrevHopMedia;            /**< @brief The MediaType */
  uint8_t          ConnId;                  /**< @brief The ID of the UDP connection  */
  ip6_addr_t       SourceAddress;           /**< @brief The source IPv6 address */
  uint16_t         SourcePort;              /**< @brief The source UDP port */
  ip6_addr_t       DestAddress;             /**< @brief The destination IPv6 address */
  uint16_t         DestPort;                /**< @brief The destination UDP port */
  uint16_t         DataLen;                 /**< @brief The Length of the UDP payload  */
  uint8_t          Data[IP_MAX_SDU_SIZE];   /**< @brief The UDP payload  */

} IP_UdpDataIndication_t;

/**< @brief Minimum length of one IP Data Indication (no payload) */
#define UDP_DATA_IND_MIN_LEN  ((uint16_t)sizeof(IP_UdpDataIndication_t) - IP_MAX_SDU_SIZE)

typedef struct
{
  IP_AdpMetaData_t MetaData;
  union
  {
    IP_IcmpDataIndication_t IcmpDataIndication;   /**< @brief The received ICMP Packet  */
    IP_UdpDataIndication_t  UdpDataIndication;    /**< @brief The received UDP Packet  */
  };
} IP_DataIndication_t;

#pragma pack()

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif // HI_IP_SAP_INTERFACE_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
