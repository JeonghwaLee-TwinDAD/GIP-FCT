/**
  ******************************************************************************
  * @file    hi_adp_pib_types.h
  * @author  AMG/IPC Application Team
  * @brief   Header file for the LOADng layer
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
#ifndef HI_ADP_PIB_TYPES_H
#define HI_ADP_PIB_TYPES_H

#include <stdint.h>

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_adp_drv HIF ADP DRV
  * @{
  */

/** @addtogroup ADP_PIB_Attributes_TypesDefinitions
  * @{
  */

/** @defgroup Internal_TypesDefinitions Internal_TypesDefinitions
  * @brief Definitions of Internal Types
  * @{
  */

  // interface macros
#define ADP_CONTEXT_MAX_LEN ((uint8_t)16)    /**< @brief Maximum context length is 128 bits (16 bytes) */
#define ADP_CONTEXT_TBL_LEN ((uint8_t)16)    /**< @brief Number of entries in the Context Table */

#define ADP_ROUTING_ENTRY_TIME_UPDATE     ((uint16_t)15) /**< @brief TTL in Routing Table is expressed in multiple of 15 seconds */
#define ADP_ROUTING_ENTRY_TIME_SCALE      ((uint16_t)4)  /**< @brief Number of 15 seconds Ticks to reach 1 minute (60 secs / ADP_ROUTING_ENTRY_TIME_UPDATE) */

#define ADP_BROADCAST_TBL_LEN ((uint16_t)64)  /**< @brief Number of entries in the Broadcast Table  */

#define ADP_BLACKLISTEDNEIGH_TBL_LEN ((uint16_t)64)  /**< @brief Number of entry in the Blacklisted Neighbour Table  */

#define ADP_GROUP_TBL_LEN ((uint16_t)64)    /**< @brief Number of entries in the Multicast Group Table */
#define ADP_GROUP_ENTRY_EMPTY_VALUE    ((uint16_t)0xFFFF)    /**< @brief Value of Multicast Group empty entry */

#define ADP_DESTADDR_TBL_LEN ((uint16_t)16)    /**< @brief Number of entries in the Destination Address Table */
#define ADP_DESTADDR_ENTRY_EMPTY_VALUE ((uint16_t)0xFFFF)    /**< @brief Value of Destination Address empty entry */

#define ADP_PREFIX_TBL_LEN ((uint8_t)4)  /**< @brief Number of entries in the Prefix Table */
#define ADP_PREFIX_MAX_LEN ((uint8_t)16) /**< @brief Maximum prefix length is 128 bits (16 bytes) */
#define ADP_PREFIX_LT_GRANULARITY    ((uint32_t)4) /**< @brief The granularity of PreferredLifetime and ValidLifetime in seconds */

#define ADP_MAX_HOPS_SUPPORTED ((uint8_t)15) /**< @brief Maximum Number of Hops */

#define ADP_MAX_ACTIVEKEYINDEX  ((uint8_t)1) /**< @brief Maximum index for the Key Index (it could be 0 or 1) */

#define ADP_DEVICETYPE_UNKNOWN  ((uint8_t)2) /**< @brief Unknow type of device */
#define ADP_DEVICETYPE_COORD    ((uint8_t)1) /**< @brief Device is a G3 Coordinator */
#define ADP_DEVICETYPE_DEVICE   ((uint8_t)0) /**< @brief Device is a G3 Device */

// interface typedef

#pragma pack(1)

/**
  * @brief  ADP PAN Descriptor
  */
typedef struct
{
    uint16_t PanID;          /**< @brief The 16-bit PAN identifier */
    uint8_t  LinkQuality;    /**< @brief The 8-bit Link Quality of LBA */
    uint16_t LBAAddress;     /**< @brief The 16 bit short address of a device in this PAN to be used as the LBA by the associating device */
    uint16_t RouteCost;      /**< @brief The estimated route cost */
    uint8_t  MediaType;      /**< @brief The MediaType */
} ADP_PanDescriptor_t;

/**
  * @brief  ADP Context Information Table Entry compliant with RFC 6775
  */
typedef struct
{
    union
    {
      struct
      {
      uint8_t  CID : 4;       /**< @brief Corresponds to the 4-bit context information used for source and destination addresses (SCI, DCI) */
      uint8_t  C   : 1;       /**< @brief Indicates if the context is valid for use in compression */
      uint8_t  Res : 3;       /**< @brief Unused bits */
      };
      uint8_t CID_C_Res;
    };
    uint16_t ValidLifeTime; /**< @brief Remaining time in minutes during which the context information table is considered valid */
    uint8_t  ContextLen;    /**< @brief Indicates the length of the carried context (up to 128-bit contexts may be carried) */
    uint8_t  Context[ADP_CONTEXT_MAX_LEN]; /**< @brief Corresponds to the carried context used for compression/decompression purposes */

} ADP_ContextInformationEntry_t;

/**
  * @brief  ADP Broadcast Log Table Entry
  */
typedef struct
{
    uint16_t SrcAddr;    /**< @brief The 16-bit source address of a broadcast packet. This is the address of the broadcast initiator */
    uint8_t  SeqNum;     /**< @brief The sequence number contained in the BC0 header */
    uint16_t ValidTime;  /**< @brief Remaining time in minutes until when this entry in the broadcast log table is considered valid */

} ADP_BroadcastTableEntry_t;

/**
  * @brief  ADP Blacklisted Table Entry
  */
typedef struct
{
    uint16_t Addr;        /**< @brief The 16-bit address of the blacklisted neighbour */
    uint16_t ValidTime;   /**< @brief Remaining time in minutes until when this entry in the blacklisted neighbour table is considered valid */
    uint8_t MediaType;  /**< @brief The channel to be used to transmit to the next hop */
} ADP_BlackListedNeighTableEntry_t;

/**
  * @brief  ADP Prefix Table Entry compliant with RFC 4861
  */
typedef struct
{
    uint8_t  PrefixLength;   /**< @brief Number of bits in the prefix that are valid. The value ranges from 0 to 128 */
    uint8_t  L : 1;          /**< @brief On-link flag */
    uint8_t  A : 1;          /**< @brief Autonomous address-configuration flag */
    uint32_t ValidLifetime;  /**< @brief Length of time in seconds during which the prefix is valid for the purpose of on-link determination */
    uint32_t PreferredLifetime; /**< @brief Length of time in seconds during which addresses generated from the prefix remain preferred */
    uint8_t  Prefix[ADP_PREFIX_MAX_LEN]; /**< @brief IPv6 address or a prefix of an IPv6 address */

} ADP_PrefixTableEntry_t;

#pragma pack()

/**
  * @brief  ADP Group Table Entry definition
  */
typedef uint16_t ADP_GroupTableEntry_t;

/**
  * @brief  ADP Destination Address Table Entry definition
  */
typedef uint16_t ADP_DestinationAddressEntry_t;


/**
  * @brief ADP exported Table Attributes
  */
extern ADP_ContextInformationEntry_t    ADP_ContextInformationTable[ADP_CONTEXT_TBL_LEN]; /**< @brief Context Information Table */
extern ADP_BroadcastTableEntry_t        ADP_BroadcastTable[ADP_BROADCAST_TBL_LEN]; /**< @brief Broadcast Log Table */
extern ADP_BlackListedNeighTableEntry_t ADP_BlackListedNeighTable[ADP_BLACKLISTEDNEIGH_TBL_LEN]; /**< @brief Blacklisted Neighbour Table */
extern ADP_GroupTableEntry_t            ADP_GroupTable[ADP_GROUP_TBL_LEN]; /**< @brief Multicast Group Table */
extern ADP_PrefixTableEntry_t           ADP_PrefixTable[ADP_PREFIX_TBL_LEN]; /**< @brief Prefix Table */
extern ADP_DestinationAddressEntry_t    ADP_DestinationAddressTable[ADP_DESTADDR_TBL_LEN]; /**< @brief Destination Address Table */

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

#endif //HI_ADP_PIB_TYPES_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
