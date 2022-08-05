/**
  ******************************************************************************
  * @file    hi_boot_sap_interface.h
  * @author  AMG/IPC Application Team
  * @brief   This code include all functionalities of the interface between
  *              Bootstrap Handler and SAP levels.
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HI_BOOT_SAP_INTERFACE_H
#define HI_BOOT_SAP_INTERFACE_H

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_boot_drv HIF BOOT DRV
  * @{
  */

/** @defgroup Boot_SAP_Interface BOOT SAP Interface
  * @brief    Interface between Bootstrap Layer and upper layers
  * @details  This module contains the implementation of the interface between the Bootstrap Handler Application and upper layers.
  * @{
  */
#include "hi_adp_sap_interface.h"
#include "hi_adp_lbp_message_catalog.h"
#include "hi_g3lib_attributes.h"


/** @defgroup BOOT_SAP_DEFINE BOOT SAP Interface Definition
  * @brief    Bootstrap Layer Interface Definition
  * @details  These values are used in the BOOT SAP Interface
  * @{
  */

#define BOOT_EAP_PSK_KEY_LEN            ((uint8_t) 0x10)  /**< @brief Length of the EAP-PSK Pre Shared Key */
/**
  * @}
  */


/** @struct BOOT_States_t
  * @brief    enumeration for the various states the boostrap process can take for the device
  * @brief    Bootstrap Layer Interface Len Values
  * @details  These values are used to handle SAP Primitives parameters
  * @{
  */
typedef enum
{
  BOOT_DEV_STATE_IDLE = 0,    /**< @brief The device is not connected and not starting a bootstrap process */
  BOOT_DEV_STATE_WAITING,     /**< @brief The device has started a bootstrap process and it is waiting the proper time to perfom active scan */
  BOOT_DEV_STATE_DISCOVERING, /**< @brief The device is perfoming the active scan */
  BOOT_DEV_STATE_SORTING,     /**< @brief The device is sorting the received PAN descriptors */
  BOOT_DEV_STATE_JOINING,     /**< @brief The device has performed the active scan and it is waiting the proper time to join the network */
  BOOT_DEV_STATE_CONNECTED,   /**< @brief The device is connected */
} BOOT_States_t;
/**
  * @}
  */

/** @struct BOOT_CBKResult_t
  * @brief    Bootstrap Layer Interface Callback Return Values
  * @details  These values are used as return value from the External Callbacks called by the Bootstrap Layer
  * @{
  */
typedef enum
{
    BOOT_CBK_SUCCESS = 0,             /**< @brief The requested operation was completed successfully. */
    BOOT_CBK_EUI64_BLACKLISTED,       /**< @brief The Node is not allowed to join the PAN.  */
    BOOT_CBK_IDP_BLACKLISTED,         /**< @brief The Node is not allowed to join the PAN.  */
    BOOT_CBK_NODE_TABLE_FULL,         /**< @brief The Nodes Table is full and new joining attempts cannot be handled.  */
    BOOT_CBK_NODE_UNKNOWN             /**< @brief The Node cannot be found in the Nodes Table and its request cannot be handled.  */
} BOOT_CBKResult_t;
/**
  * @}
  */

/** @struct BOOT_StartType_t
  * @brief The Bootstrap Layer SAP Start Type Values
  * @details These values are given as parameter to BOOT-SERVER-START.request or BOOT-DEVICE-START.request commands to decide how the bootstrap procedure should start
  * @{
  */
typedef enum
{
    BOOT_START_NORMAL = 0,    /**< @brief Normal bootstrap type: create a new PAN (for PAN Coordinator) or discover and connect to existing PAN (for Device) */
    BOOT_START_FAST_RESTORE   /**< @brief Fast Restore bootstrap type: Fast Restore algorithm restores network information of the nodes if the restart happens when the node was already in the network */
} BOOT_StartType_t;
/**
  * @}
  */

#pragma pack(1)

/**
  * @brief  BOOT-SERVER-START.Request parameters
  */
typedef struct
{
    uint16_t ServerAddress;      /**< @brief The short address used by the server */
    uint16_t PANId;              /**< @brief The PAN ID */
    BOOT_StartType_t ReqType;    /**< @brief The type of BOOT-SERVER-START.request (#BOOT_StartType_t) */
} BOOT_ServerStartRequest_t;

/**
  * @brief  BOOT-SERVER-START.Confirm parameters
  */
typedef struct
{
    uint8_t Status; /**< @brief Result of the operation */
} BOOT_ServerStartConfirm_t;

/**
  * @brief  BOOT-SERVER-STOP.Confirm parameters
  */
typedef struct
{
    uint8_t Status; /**< @brief Result of the operation */
} BOOT_ServerStopConfirm_t;

/**
  * @brief  BOOT-SERVER-KICK.Request parameters
  */
typedef struct
{
    uint8_t ExtendedAddress[MAC_ADDR64_SIZE]; /**< @brief The extended address of the node to be kicked */
    uint16_t ShortAddress;                    /**< @brief The 16-bit address of the node to be kicked */
} BOOT_ServerKickRequest_t;

/**
  * @brief  BOOT-SERVER-KICK.Confirm parameters
  */
typedef struct
{
    uint8_t Status; /**< @brief Result of the operation */
} BOOT_ServerKickConfirm_t;

/**
  * @brief  BOOT-SERVER-JOIN.Indication parameters
  */
typedef struct
{
    uint8_t  ExtendedAddress[MAC_ADDR64_SIZE]; /**< @brief The Node extended address (EUI-64)of the node that has joint the network */
    uint16_t ShortAddress;                     /**< @brief The assigned 16-bit Short Addresss */
} BOOT_ServerJoinIndication_t;

/**
  * @brief  BOOT-SERVER-LEAVE.Indication parameters
  */
typedef struct
{
    uint8_t ExtendedAddress[MAC_ADDR64_SIZE]; /**< @brief The extended address (EUI-64)of the node leaving the network*/
} BOOT_ServerLeaveIndication_t;

/**
  * @brief  BOOT-SERVER-GETPSK.Indication parameters
  */
typedef struct
{
    uint8_t ExtendedAddress[MAC_ADDR64_SIZE]; /**< @brief The extended address (EUI-64) of the node joining the network */
    uint8_t IdPLength;                        /**< @brief The Length of the EAP peer NAI (IdP) */
    uint8_t IdP[ADP_EAP_PSK_ID_MAX_LEN];      /**< @brief The EAP peer NAI (IdP) */
} BOOT_ServerGetPSKIndication_t;

/**
  * @brief  BOOT-SERVER-SETPSK.Request parameters
  */
typedef struct
{
    uint8_t  ExtendedAddress[MAC_ADDR64_SIZE];  /**< @brief The extended address (EUI-64) of the node joining the network */
    uint8_t  PreSharedKey[ADP_EAP_PSK_KEY_LEN]; /**< @brief The PSK to be used for EAP-PSK exchange with the peer */
    uint16_t ShortAddress;                      /**< @brief The short address to be assigned to the peer */
} BOOT_ServerSetPSKRequest_t;

/**
  * @brief  BOOT-SERVER-SETPSK.Confirm parameters
  */
typedef struct
{
    uint8_t Status; /**< @brief Result of the operation */
} BOOT_ServerSetPSKConfirm_t;

/**
  * @brief  BOOT-DEVICE-START.Request parameters
  */
typedef struct
{
    BOOT_StartType_t ReqType;            /**< @brief The type of BOOT-DEVICE-START.request (#BOOT_StartType_t) */
} BOOT_DeviceStartRequest_t;

/**
  * @brief  BOOT-DEVICE-START.Confirm parameters
  */
typedef struct
{
    uint8_t Status;          /**< @brief Result of the operation */
    uint16_t NetworkAddress; /**< @brief The short address used by the node  */
    uint16_t PANId;          /**< @brief The PAN ID */
} BOOT_DeviceStartConfirm_t;

/**
  * @brief  BOOT-DEVICE-PANSORT.Indication parameters
  */
typedef struct
{
    uint8_t PANCount;                                        /**< @brief The number of received PAN Descriptors  */
    ADP_PanDescriptor_t PANDescriptor[ADP_MAX_NUM_PANDESCR]; /**< @brief The array of returned PAN Descriptors */
} BOOT_DevicePANSortIndication_t;

#define BOOT_DEVICEPANSORT_IND_MIN_LEN        ((uint16_t)(sizeof(BOOT_DevicePANSortIndication_t) - \
    sizeof(((BOOT_DevicePANSortIndication_t *)0)->PANDescriptor)))  /**< @brief Minimum length of BOOT-DEVICE-PANSORT.Indication */
#define BOOT_DEVICEPANSORT_IND_LEN(pan_count) ((uint16_t)(BOOT_DEVICEPANSORT_IND_MIN_LEN + (pan_count * sizeof(ADP_PanDescriptor_t)))) /**< @brief Minimum length of one ADP Path Discovery Request (hops info length) */

/**
  * @brief  BOOT-DEVICE-PANSORT.Request parameters
  */
typedef struct
{
    uint8_t PANCount;                                         /**< @brief The number of selected PAN Descriptors  */
    ADP_PanDescriptor_t PANDescriptor[ADP_MAX_NUM_PANDESCR]; /**< @brief The array of selected and ordered PAN Descriptors */
} BOOT_DevicePANSortRequest_t;

/**
  * @brief  BOOT-DEVICE-PANSORT.Confirm parameters
  */
typedef struct
{
    uint8_t Status; /**< @brief Result of the operation */
} BOOT_DevicePANSortConfirm_t;

/**
  * @brief  BOOT-DEVICE-LEAVE.Confirm parameters
  */
typedef struct
{
    uint8_t Status; /**< @brief Result of the operation */
} BOOT_DeviceLeaveConfirm_t;

/**
  * @brief  BOOT-GET.Request parameters
  */
typedef struct
{
    G3_LIB_PIB_ID_t Attribute;      /**< @brief The ID+Index */
} BOOT_Get_Request_t;

/**
  * @brief  BOOT-GET.Confirm parameters
  */
typedef struct
{
    uint8_t Status;                 /**< @brief The result of the operation  */
    G3_LIB_PIB_t Attribute;         /**< @brief The attribute information */
} BOOT_Get_Confirm_t;

/**
  * @brief  BOOT-SET.Request parameters
  */
typedef struct
{
    G3_LIB_PIB_t Attribute;        /**< @brief The attribute information */
} BOOT_Set_Request_t;

/**
  * @brief  BOOT-SET.Confirm parameters
  */
typedef struct
{
    uint8_t  Status;                  /**< @brief The status of the request */
    G3_LIB_PIB_ID_t Attribute;       /**< @brief The ID+Index */
} BOOT_Set_Confirm_t;

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

#endif // HI_BOOT_SAP_INTERFACE_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/