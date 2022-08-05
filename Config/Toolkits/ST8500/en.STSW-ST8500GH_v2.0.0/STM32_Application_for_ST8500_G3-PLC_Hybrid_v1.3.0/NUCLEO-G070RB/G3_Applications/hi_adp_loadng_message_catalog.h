/**
  ******************************************************************************
  * @file    hi_adp_loadng_message_catalog.h
  * @author  AMG/IPC Application Team
  * @brief   Header file to define the packets used in the LOADng layer
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
#ifndef HI_ADP_LOADNG_MESSAGECATALOG_H
#define HI_ADP_LOADNG_MESSAGECATALOG_H

#include <stdint.h>
#include "hi_adp_pib_types.h"

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_adp_drv HIF ADP DRV
  * @{
  */

/** @defgroup LOADng_MessageCatalog LOADng_MessageCatalog
  * @brief LOADng Message Catalog
  * @brief Definition of the messages used by LOADng protocol
  * @{
  */

/** @defgroup LOADng_ExportedMacros LOADng_ExportedMacros
  * @brief Definitions for LOADng protocol
  * @{
  */

// interface macros
#define LOADng_TYPE_RREQ   ((uint8_t)0)    /**< @brief Type field RREQ: 0 */
#define LOADng_TYPE_RREP   ((uint8_t)1)    /**< @brief Type field RREP: 1 */
#define LOADng_TYPE_RERR   ((uint8_t)2)    /**< @brief Type field RERR: 2 */
#define LOADng_TYPE_PREQ   ((uint8_t)252)  /**< @brief Type field PREQ: 252 */
#define LOADng_TYPE_PREP   ((uint8_t)253)  /**< @brief Type field PREP: 253 */
#define LOADng_TYPE_RLCREQ ((uint8_t)254)  /**< @brief Type field RLCREQ: 254 */
#define LOADng_TYPE_RLCREP ((uint8_t)255)  /**< @brief Type field RLCREP: 255 */

#define LOADng_ROUTE_REPAIR_FLAG_SET     ((uint8_t)0x01) /**< @brief Route Repair Flag used for local repair procedure in RREQ and RREP: set the bit */
#define LOADng_ROUTE_REPAIR_FLAG_NOTSET  ((uint8_t)0x00) /**< @brief Route Repair Flag used for local repair procedure in RREQ and RREP: unset the bit */
#define LOADng_UNICAST_RREQ_FLAG_SET     ((uint8_t)0x01) /**< @brief Unicast RREQ Flag used for local repair procedure in RREQ: set the bit */
#define LOADng_UNICAST_RREQ_FLAG_NOTSET  ((uint8_t)0x00) /**< @brief Unicast RREQ Flag used for local repair procedure in RREQ: unset the bit */

#define LOADng_G3_METRIC_TYPE       ((uint8_t)0x0F) /**< @brief G3 Metric Type used in RREQ and RREP */
#define LOADng_CAPACITY_METRIC_TYPE ((uint8_t)0x0E) /**< @brief Capacity Metric Type used in RREQ and RREP */
#define LOADng_HOPCNT_METRIC_TYPE   ((uint8_t)0x00) /**< @brief HOP_COUNT Metric Type used in RREQ and RREP (not used in G3) */

#define LOADng_HOPCNT_DIST        ((uint16_t)0x0001) /**< @brief HOP_COUNT Metric DIST value is 1 (not used in G3) */
#define LOADng_HOPCNT_MAXDIST     ((uint8_t)0x0F) /**< @brief HOP_COUNT Metric MAX_DIST value is 15 (not used in G3) */

#define LOADng_NOAVLROUTE_ERRORCODE ((uint8_t)0x00) /**< @brief No Available Route Error Code used in RERR */

#define LOADng_PATH_DISCOVERY_SUCCESS               ((uint8_t)0x00) /**< @brief Success of Path Discovery in PREP */
#define LOADng_PATH_DISCOVERY_INCOMPLETE            ((uint8_t)0x01) /**< @brief Incomplete Path Discovery in PREP */
#define LOADng_PATH_DISCOVERY_ALREADY_IN_PROGRESS   ((uint8_t)0x02) /**< @brief Path Discovery Already in Progress */
#define LOADng_PATH_DISCOVERY_TIMEOUT               ((uint8_t)0x03) /**< @brief Path Discovery operation timed out */

#define LOADng_MAX_HOP_COUNT    ((uint8_t)0x0F) /**< @brief Define the Maximum hop count that can be encoded in the message (HopCount field is 4 bits) */

  // interface typedef

/**
  * @brief  RREQ and RREP Message Format
  */
typedef struct
{
  uint8_t  Type;              /**< @brief Specifies the type of the message */
  uint16_t Dst;               /**< @brief Destination address of RREQ or RREP */
  uint16_t Orig;              /**< @brief Originator address of RREQ or RREP */
  uint16_t SeqNum;            /**< @brief Refers to RREQ.seq-num or RREP.seq-num */
  uint8_t  MetricType    : 4; /**< @brief Refers to RREQ.metric-type or RREP.metric-type */
  uint8_t  Flag_Rsv      : 2; /**< @brief Shall be cleared on transmission and shall be ignored upon reception */
  uint8_t  Flag_UR       : 1; /**< @brief The RREQ message is forwarded in unicast along an already installed route towards RREQ.destination if such a valid route exists */
  uint8_t  Flag_RR       : 1; /**< @brief When set the RREQ message is used within a local repair procedure */
  uint16_t RouteCost;         /**< @brief Cumulative link cost along the route towards the destination. Refers to RREQ.route-metric or RREP.route-metric */
  uint8_t  HopCount      : 4; /**< @brief Number of hops of the route. Refers to RREQ.hop-count or RREP.hop-count */
  uint8_t  HopLimit      : 4; /**< @brief Maximum number of hops of the route. Refers to RREQ.hop-limit or RREP.hop-limit */
  uint8_t  Rsv           : 4; /**< @brief Reserved bits, shall be set to 0 by the sender and ignored by the receiver */
  uint8_t  WeakLinkCount : 4; /**< @brief Total number of weak links which the message has traversed from RREQ.originator or RREP.originator */

} LOADng_RREQ_RREP_t;

/**
  * @brief  RERR Message Format
  */
typedef struct
{
  uint8_t  Type;         /**< @brief Specifies the type of the message */
  uint16_t Dst;          /**< @brief Destination address of RERR packet */
  uint16_t Orig;         /**< @brief Originator address of RERR packet */
  uint8_t  ErrCode;      /**< @brief Error Code */
  uint16_t UnreachAddr;  /**< @brief Refers to RERR.unreachableAddress */
  uint8_t  Rsv      : 4; /**< @brief Reserved bits, shall be set to 0 by the sender and ignored by the receiver */
  uint8_t  HopLimit : 4; /**< @brief Refers to RERR.hop-limit  */

} LOADng_RERR_t;

/**
  * @brief  PREQ/PREP Single Hop Information
  */
typedef struct
{
    uint16_t Address;     /**< @brief Short Address */
    union {
      uint8_t Res_MNS;
      struct {
        uint8_t  Reserved     : 6; /**< @brief Reserved (set to 0) */
        uint8_t  MediaType    : 1; /**< @brief (0: PLC, 1: RF) */
        uint8_t  MNS          : 1; /**< @brief MNS */
      };
    };
    uint8_t  LinkCost;    /**< @brief Link Cost */
} LOADng_PREX_Hop_Info_t;

/**
  * @brief  PREQ Message Format
  */
typedef struct
{
    struct
    {
        uint8_t  Type;     /**< @brief Type of message (PREQ) */
        uint16_t Dst;      /**< @brief Destination Short Address */
        uint16_t Orig;     /**< @brief Originator Short Address */
        uint32_t ExtData;  /**< @brief Additional Info */
    } StaticInfo;  /**< @brief Static information structure (fixed size) */
    LOADng_PREX_Hop_Info_t HopsInfo[ADP_MAX_HOPS_SUPPORTED * 2]; /**< @brief List of hops information (variable size) */
} LOADng_PREQ_t;

#define LOADNG_PREQ_STATICINFO_SIZE (sizeof(((LOADng_PREQ_t *)0)->StaticInfo))   /**< @brief Size of the fixed part of PREQ messages */

/**
  * @brief  PREP Message Format
  */
typedef struct
{
    struct
    {
        uint8_t  Type;                 /**< @brief Type of message (PREP) */
        uint16_t Dst;                  /**< @brief Destination Short Address */
        uint16_t ExpectedOrig;         /**< @brief Expected originator (is the Dst short address of the PREQ) */
        uint16_t ExtData;              /**< @brief Additional Info */
        uint16_t Orig;                 /**< @brief Originator Short Address */
    } StaticInfo; /**< @brief Static information structure (fixed size)  */
    LOADng_PREX_Hop_Info_t HopsInfo[ADP_MAX_HOPS_SUPPORTED * 2]; /**< @brief List of hops information (variable size) */
} LOADng_PREP_t;

#define LOADNG_PREP_STATICINFO_SIZE (sizeof(((LOADng_PREP_t *)0)->StaticInfo)) /**< @brief Size of the fixed part of PREP messages */

/**
  * @brief  RLCREQ Message Format
  */
typedef struct
{
    uint8_t  Type;           /**< @brief Specifies the type of the message */
    uint16_t Dst;            /**< @brief Destination address of the RLCREQ packet */
    uint16_t Orig;           /**< @brief Originator address of the RLCREQ packet */
    uint8_t  Reserved   : 4; /**< @brief Reserved by ITU-T (shall be set to 0) */
    uint8_t  MetricType : 4; /**< @brief Metric type used for routing and shall be set to adpMetricType */
} LOADng_RLCREQ_t;

/**
  * @brief  RLCREP Message Format
  */
typedef struct
{
    uint8_t  Type;           /**< @brief Specifies the type of the message */
    uint16_t Dst;            /**< @brief Destination address of the RLCREP packet */
    uint16_t Orig;           /**< @brief Originator address of the RLCREP packet */
    uint8_t  Reserved   : 4; /**< @brief Reserved by ITU-T (shall be set to 0)  */
    uint8_t  MetricType : 4; /**< @brief Metric type used for routing and shall be set to adpMetricType */
    uint8_t  LinkCost;       /**< @brief Link cost from the destination to the originator of the RLCREP packet */
} LOADng_RLCREP_t;


  // interface global variables

/**
  * @}
  */

/** @defgroup LOADng_Functions LOADng_Functions
  * @brief LOADng interface Functions
  * @{
  */

    // interface function prototypes

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
#endif //HI_ADP_LOADNG_MESSAGECATALOG_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
