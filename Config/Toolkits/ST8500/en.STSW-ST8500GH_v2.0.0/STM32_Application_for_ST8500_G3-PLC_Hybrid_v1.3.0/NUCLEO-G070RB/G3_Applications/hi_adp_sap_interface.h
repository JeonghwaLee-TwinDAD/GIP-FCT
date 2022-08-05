/**
  ******************************************************************************
  * @file    hi_adp_sap_interface.h
  * @author  AMG/IPC Application Team
  * @brief   Header file for the interface between ADP and SAP levels
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
#ifndef HI_ADP_SAP_INTERFACE_H
#define HI_ADP_SAP_INTERFACE_H

#include <stdint.h>

#include "hi_adp_loadng_message_catalog.h"
#include "hi_adp_pib_attributes.h"
#include "hi_g3lib_attributes.h"

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_adp_drv HIF ADP DRV
  * @{
  */

/** @defgroup ADP_SAP_Interface ADP-SAP Interface
  * @brief    Interface between ADP and upper layers
  * @details  This module contains the implementation of the interface between the 6LoWPAN based ADP and upper layers.
  * @{
  */

/** @defgroup ADP_SAP_IF_TypesDefinitions ADP-SAP Interface Types Definitions
  * @brief  ADP Sap Interface types definitions
  * @{
  */

/* ADP Commands parameters */
#define ADP_MAX_SDU_SIZE                ((uint16_t)1600) /**< @brief ADP<->IPv6 Layer Primitive maximum size of supported IPv6 packet  */
#define ADP_MAX_CTRL_PKT_SIZE           ((uint16_t)400)  /**< @brief ADP<->IPv6 Layer Primitive maximum size of supported internal/control packet  */
#define ADP_MAX_NUM_PANDESCR            ((uint8_t)64)    /**< @brief ADP<->IPv6 Layer Primitive maximum number of supported PAN Descriptors  */
#define ADP_MAX_STR_LEN                 ((uint8_t)128)   /**< @brief ADP<->IPv6 Layer Primitive maximum length of the string returned by ADPM-NETWORK-STATUS.Indication */
#define ADP_MAX_NUM_BC0                 ((uint8_t)6)     /**< @brief ADP<->IPv6 Layer Primitive maximum number of BC0 header(s) */
#define ADP_RF_CHANNEL                  ((uint8_t)0x01)

#define ADP_SECURIY_ENABLED             ((uint8_t)0x01)   /**< @brief ADP<->IPv6 Layer Primitive the frame was received with a security level greater or equal to adpSecurityLevel */
#define ADP_SECURIY_DISABLED            ((uint8_t)0x00)   /**< @brief ADP<->IPv6 Layer Primitive the frame was received with a security level lower than adpSecurityLevel */
#define ADP_DISCOVERY_ROUTE_FALSE       ((uint8_t)0x00)   /**< @brief ADP<->IPv6 Layer Primitive no route discovery is performed */
#define ADP_DISCOVERY_ROUTE_TRUE        ((uint8_t)0x01)   /**< @brief ADP<->IPv6 Layer Primitive a route discovery procedure will be performed prior to sending the frame if a route to the destination is not available in the routing table */
#define ADP_QOS_STANDARD_PRIORITY       ((uint8_t)0x00)   /**< @brief ADP<->IPv6 Layer Primitive quality of service (QoS) of the frame to send: standard priority */
#define ADP_QOS_HIGH_PRIORITY           ((uint8_t)0x01)   /**< @brief ADP<->IPv6 Layer Primitive quality of service (QoS) of the frame to send: high priority */

#define ADP_SHORT_ADDR_MULTICAST_MASK   ((uint16_t)0x8000) /**< @brief ADP short address multicast bit mask */

#pragma pack(1)

/**
  * @brief  ADPD-DATA.Request parameters
  */
typedef struct
{
  uint16_t NsduLength;             /**< @brief The Length of the IPv6 Packet  */
  uint8_t  NsduHandle;             /**< @brief The Handle of the IPv6 Packet  */
  uint8_t  DiscoverRoute;          /**< @brief Discovery Route Flag: 0x01 to start a discovery route if route not present in routing table, 0x00 in the other cases */
  uint8_t  QualityOfService;       /**< @brief The Quality of service: 0x00 for normal priority, 0x01 for high priority  */
  uint8_t  Nsdu[ADP_MAX_SDU_SIZE]; /**< @brief The IPv6 Packet  */

} ADP_AdpdDataRequest_t;
#define ADP_DATA_REQ_MIN_LEN  ((uint16_t)sizeof(ADP_AdpdDataRequest_t) - ADP_MAX_SDU_SIZE)  /**< @brief Minimum length of one ADP Data Request (no payload) */

/**
  * @brief  ADPD-ROUTEOVER-DATA.Request parameters
  */
typedef struct
{
  uint16_t            NsduLength;             /**< @brief The Length of the IPv6 Packet  */
  uint8_t             NsduHandle;             /**< @brief The Handle of the IPv6 Packet  */
  MAC_DeviceAddress_t NextHop;                /**< @brief The MAC address of the next node the packet shall be sent to  */
  uint8_t             AckRequired;            /**< @brief It allows the upper layers to handle MAC layer acknowledgement: 0x00 if the device shall not
                                                          request the acknowledgement, 0x01 if the device shall request the acknowledgement */
  uint8_t             QualityOfService;       /**< @brief The Quality of service: 0x00 for normal priority, 0x01 for high priority  */
  uint8_t  MediaType;                         /**< @brief The channel over which the frame has to betransmitted to thenext hop */
  uint8_t             Nsdu[ADP_MAX_SDU_SIZE]; /**< @brief The IPv6 Packet  */
} ADP_AdpdRODataRequest_t;

#define ADP_RO_DATA_MIN_ADDR_LEN ((uint16_t) sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t))   /**< @brief Minimum length of Next Hop destination address (Mode + PanId + Short address) */
#define ADP_RO_DATA_REQ_MIN_LEN  ((uint16_t) sizeof(ADP_AdpdRODataRequest_t) - (uint16_t)sizeof(MAC_DeviceAddress_t) + ADP_RO_DATA_MIN_ADDR_LEN - ADP_MAX_SDU_SIZE) /**< @brief Minimum length of ADPD-RO-DATA Request */
#define ADP_RO_DATA_REQ_MAX_LEN  (uint16_t) sizeof(ADP_AdpdRODataRequest_t) /**< @brief Maximim length of ADPD-RO-DATA Request */

/**
  * @brief  ADPD-DATA.Confirm parameters
  */
typedef struct
{
  uint8_t  Status;     /**< @brief The Status of the operation (result)  */
  uint8_t  NsduHandle; /**< @brief The Handle specified in the correspondent .Request  */
  uint8_t  NexthopMedia; /**< @brief The Next Hop Media Type */
} ADP_AdpdDataConfirm_t;

/**
  * @brief  ADPD-DATA.Indication parameters
  */
typedef struct
{
  uint16_t NsduLength;                 /**< @brief The Length of the IPv6 Packet  */
  MAC_DeviceAddress_t PreviousHop;     /**< @brief The MAC address of the previous hop  */
  uint8_t  LQI;                        /**< @brief The Link Quality Indicator  */
  uint8_t  PhaseDifferential;          /**< @brief The phase difference in multiples of 60 degrees of the last received MAC frame  */
  uint8_t  PayloadModType;             /**< @brief The modulation type used in the last received MAC frame  */
  uint8_t  PayloadModScheme;           /**< @brief The payload modulation scheme used in the last received MAC frame */
  uint8_t  ToneMap[PHY_TONEMAP_SIZE];  /**< @brief The Tone map parameter used in the last received MAC frame */
  uint8_t  BC0Num;                     /**< @brief Number of the BC0 headers of the 6LoWPAN broadcast corresponding to the received IPv6 multicast packet */
  uint8_t  BC0List[ADP_MAX_NUM_BC0];   /**< @brief List of BC0 headers of the 6LoWPAN broadcast packets corresponding to the received IPv6 multicast packet */
  uint8_t  PrevhopMedia;               /**< @brief The MediaType */
  uint8_t  Nsdu[ADP_MAX_SDU_SIZE];     /**< @brief The IPv6 Packet  */
} ADP_AdpdDataIndication_t;

/**
  * @brief  ADPM-ROUTE-DISCOVERY.Request parameters
  */
typedef struct
{
  uint16_t DstAddr;     /**< @brief The Final Destination Short Address  */
  uint8_t  MaxHops;     /**< @brief The maximum number of hops  */
  uint16_t NextHop;     /**< @brief 0xFFFF for normal operation  */
  uint8_t  MediaType;   /**< @brief The Media Type  */

} ADP_AdpmRouteDiscoveryRequest_t;

/**
  * @brief  ADPM-ROUTE-DISCOVERY.Confirm parameters
  */
typedef struct
{
  uint8_t  Status; /**< @brief The result: SUCCESS, INVALID_REQUEST, ROUTE_ERROR */
  ADP_RoutingTableEntry_t RouteEntry; /**< @brief The routing entry updated with the Route Discovery */
} ADP_AdpmRouteDiscoveryConfirm_t;

/**
  * @brief  ADPM-PATH-DISCOVERY.Request parameters
  */
typedef struct
{
    uint16_t DstAddr;  /**< @brief The Final Destination Short Address  */
    uint8_t  PathMetricType; /**< @brief Type of the Metric */
} ADP_AdpmPathDiscoveryRequest_t;

/**
  * @brief  ADPM-PATH-DISCOVERY.Confirm parameters
  */
typedef struct
{
  uint16_t DstAddr;          /**< @brief The Final Destination Short Address  */
  uint8_t  Result;           /**< @brief Result: TRUE (0x00) if success, FALSE (0x01) if destination cannot be reached */
  union {
    uint16_t MetricType_Res;
    struct {
      uint16_t MetricType : 4;   /**< @brief Type of the Metric  */
      uint16_t Reserved   : 12;  /**< @brief Reserved (set to 0)  */
    };
  };
  uint16_t Orig;             /**< @brief Originator */
  LOADng_PREX_Hop_Info_t HopsInfo[ADP_MAX_HOPS_SUPPORTED * 2]; /**< @brief Information on the hops */
} ADP_AdpmPathDiscoveryConfirm_t;


#define ADP_PATHDISC_CONF_MIN_LEN  ((uint16_t)(sizeof(ADP_AdpmPathDiscoveryConfirm_t) - \
    sizeof(((ADP_AdpmPathDiscoveryConfirm_t *)0)->HopsInfo)))  /**< @brief Minimum length of one ADP Path Discovery Request (no hops) */
#define ADP_PATHDISC_CONF_LEN(hops_info_len) ((uint16_t)(ADP_PATHDISC_CONF_MIN_LEN + (hops_info_len))) /**< @brief Minimum length of one ADP Path Discovery Request (hops info length) */

/**
* @brief  ADPM-NETWORK-JOIN.Request parameters
*/
typedef struct
{
    uint16_t PANId;       /**< @brief The 16-bit PAN identifier of the network to join  */
    uint16_t LBAAddress;  /**< @brief The 16-bit short address of the device acting as a LoWPAN bootstrap agent  */
    uint8_t  MediaType;   /**< @brief The Media Type */
} ADP_AdpmNetworkJoinRequest_t;

/**
* @brief  ADPM-NETWORK-JOIN.Confirm parameters
*/
typedef struct
{
    uint8_t  Status;          /**< @brief The result of the attempt to join the network */
    uint16_t NetworkAddress;  /**< @brief The 16-bit network address that was allocated to the device  */
    uint16_t PANId;           /**< @brief TThe 16-bit address of the PAN of which the device is now a member  */
} ADP_AdpmNetworkJoinConfirm_t;

/**
* @brief  ADPM-NETWORK-LEAVE.Confirm parameters
* @details The ADPM-NETWORK-LEAVE.Request has no parameters
*/
typedef struct
{
    uint8_t Status;           /**< @brief The result of the leaving operation */
} ADP_AdpmNetworkLeaveConfirm_t;

/**
* @brief  ADPM-NETWORK-LEAVE.Indication parameters
*/
typedef struct
{
    uint8_t Unused;           /**< @brief Place holder, since this message doesn't have any parameters anymore*/
} ADP_AdpmNetworkLeaveIndication_t;

/**
* @brief  ADPM-DISCOVERY.Request parameters
*/
typedef struct
{
    uint8_t Duration;       /**< @brief The duration in seconds of the discovery procedure  */
} ADP_AdpmDiscoveryRequest_t;

/**
* @brief  ADPM-DISCOVERY.Confirm parameters
*/
typedef struct
{
    uint8_t Status;       /**< @brief The status of the discovery operation */
    uint8_t PANCount;     /**< @brief The number of returned PAN Descriptors  */
    ADP_PanDescriptor_t PANDescriptor[ADP_MAX_NUM_PANDESCR]; /**< @brief The array of returned PAN Descriptors */
} ADP_AdpmDiscoveryConfirm_t;

/**
  * @brief  ADPM-NETWORK-START.Request parameters
  */
typedef struct
{
    uint16_t PanId; /**< @brief The Pan ID to be used for the new PAN */
} ADP_AdpmNetworkStartRequest_t;

/**
  * @brief  ADPM-NETWORK-START.Confirm parameters
  */
typedef struct
{
    uint8_t Status; /**< @brief The status of the request */
} ADP_AdpmNetworkStartConfirm_t;

/**
  * @brief  ADPM-NETWORK-STATUS.Indication parameters
  */
typedef struct
{
    uint8_t   Status;                            /**< @brief The communications status  */
    MAC_DeviceAddress_t SrcAddr;                 /**< @brief The individual device address of the entity from which the frame causing the error originated  */
    MAC_DeviceAddress_t DstAddr;                 /**< @brief The individual device address of the device for which the frame was intended */
    uint8_t   SecurityLevel;                     /**< @brief The security level used by the originator of the received frame */
    uint8_t   KeyIdMode;                         /**< @brief The mode used to identify the key used by the originator of the received frame  */
    uint8_t   KeyIndex;                          /**< @brief The index of the key used by the originator of the received frame  */
    uint8_t   SNRCarriers[PHY_NUM_OF_CARRIERS];  /**< @brief The SNR measurements of each carrier */
    uint8_t   Lqi;                               /**< @brief The Link Quality Indicator */
} ADP_AdpmNetworkStatusIndication_t;

/**
  * @brief  ADPM-LBP.Request parameters
  */
typedef struct
{
    MAC_DeviceAddress_t DstAddr; /**< @brief The address of the destination device */
    uint16_t NsduLength;         /**< @brief The length of the NSDU */
    uint8_t  Nsdu[ADP_MAX_CTRL_PKT_SIZE]; /**< @brief The NSDU */
    uint8_t  NsduHandle;         /**< @brief The handle of this request */
    uint8_t  MaxHops;            /**< @brief The maximum number of hops */
    uint8_t  DiscoveryRoute;     /**< @brief A flag used to activate the route discovery */
    uint8_t  QoS;                /**< @brief The QoS of the NSDU */
    uint8_t  SecurityEnabled;    /**< @brief A flag used to activate the security */
    uint8_t  MediaType;          /**< @brief The MediaType */
} ADP_AdpmLbpRequest_t;

#define ADP_LBP_MIN_DST_ADDR_LEN ((uint16_t) sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t))   /**< @brief Minimum length of LBP destination address (Mode + PanId + Short address) */
#define ADP_LBP_REQ_MIN_LEN  ((uint16_t)sizeof(ADP_AdpmLbpRequest_t) - (uint16_t)sizeof(MAC_DeviceAddress_t) + ADP_LBP_MIN_DST_ADDR_LEN - ADP_MAX_CTRL_PKT_SIZE) /**< @brief Minimum length of LBP Request */
#define ADP_LBP_REQ_MAX_LEN  (uint16_t)sizeof(ADP_AdpmLbpRequest_t) /**< @brief Maximim length of LBP Request */

/**
  * @brief  ADPM-LBP.Confirm parameters
  */
typedef struct
{
    uint8_t  Status;     /**< @brief The status of the request */
    uint8_t  NsduHandle; /**< @brief The handle used for the request*/
    uint8_t  MediaType;  /**< @brief The Media Type */
} ADP_AdpmLbpConfirm_t;

/**
  * @brief  ADPM-LBP.Indication  parameters
  */
typedef struct
{
    MAC_DeviceAddress_t SrcAddr;   /**< @brief The address of the source device */
    uint16_t NSDULen;              /**< @brief The length of the NSDU */
    uint8_t  Nsdu[ADP_MAX_CTRL_PKT_SIZE]; /**< @brief The NSDU */
    uint8_t  LinkQualityIndicator; /**< @brief The LQI of the current NSDU */
    uint8_t  SecurityEnabled;      /**< @brief A flag that indicates if the NSDU was received with or without the security */
    uint8_t  MediaType; /**< @brief MediaType */
} ADP_AdpmLbpIndication_t;

/**
  * @brief  ADPM-RESET.Confirm parameters
  * @details Note that ADPM-RESET.Request has no parameters
  */
typedef struct
{
    uint8_t Status; /**< @brief The status of the requested reset */
} ADP_AdpmResetConfirm_t;


/**
  * @brief  ADPM-ROUTE-DELETE.Reqeust parameters
  */
typedef struct
{
    uint16_t FinalDestinationAddr; /**< @brief The final destination address */
} ADP_AdpmRouteDeleteRequest_t;

/**
  * @brief  ADPM-ROUTE-DELETE.Confirm parameters
  */
typedef struct
{
    uint8_t  Status;  /**< @brief The status of the requested reset */
    uint16_t FinalDestinationAddr; /**< @brief The final destination address */
} ADP_AdpmRouteDeleteConfirm_t;

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

#endif // HI_ADP_SAP_INTERFACE_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
