/**
  ******************************************************************************
  * @file    hi_ip_pib_types.h
  * @author  AMG/IPC Application Team
  * @brief   Header file for Types definition used in IP PIB
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
#ifndef HI_IP_PIB_TYPES_H
#define HI_IP_PIB_TYPES_H

#include <stdint.h>
#include "hi_adp_pib_types.h"

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_ip_drv HIF IP DRV
  * @{
  */

/** @addtogroup IP_PIB_Attributes_TypesDefinitions
  * @{
  */

/** @defgroup Internal_TypesDefinitions Internal_TypesDefinitions
  * @brief Definitions of Internal Types
  * @{
  */

  // interface macros
#define IP_MULTICAST_TBL_LEN      ADP_GROUP_TBL_LEN  /**< @brief Number of entry in the Multicast Address Table  */

// interface typedef
#define IP_MAX_SDU_SIZE                 ((uint16_t)1500)

#define IP_IPV6_ADDR128_UINT8_LEN       16   /**< @brief Length (in bytes) of a 128 bits IPv6 address */
#define IP_IPV6_ADDR128_UINT16_LEN      8    /**< @brief Length (in words) of a 128 bits IPv6 address */

typedef struct
{
    uint16_t Address[IP_IPV6_ADDR128_UINT16_LEN];    /**< @brief The Source Address */
} IP_Addr128_t;

/**
  * @brief  IP Multicast Addresses Table Entry
  */

  // interface global variables
extern IP_Addr128_t        IP_MulticastTable[IP_MULTICAST_TBL_LEN]; /**< @brief Multicast Address Table */


/**
  * @brief  UDP Connection Table Entry
  */
typedef struct
{
    uint16_t      DstAddr;       /**< @brief The Final Destination Address */
    uint16_t      SrcAddr ;      /**< @brief The Source Address */
    uint16_t      DstPanId;      /**< @brief The Destination PAN Id */
    uint16_t      SrcPanId;      /**< @brief The Source PAN Id */
    uint16_t      SrcPortId;     /**< @brief The Source Port Id */
    uint16_t      DstPortId;     /**< @brief The Destination Port Id */
    IP_Addr128_t  ipv6SrcAddr;   /**< @brief The IPv6 Source Address */
    IP_Addr128_t  ipv6DstAddr;   /**< @brief The IPv6 Destination Address */
} IP_UdpConnectionTableEntry_t;

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
#endif //HI_IP_PIB_TYPES_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
