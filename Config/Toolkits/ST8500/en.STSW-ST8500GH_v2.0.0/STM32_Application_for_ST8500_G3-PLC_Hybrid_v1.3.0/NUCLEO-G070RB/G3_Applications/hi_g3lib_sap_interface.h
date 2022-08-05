/**
  ******************************************************************************
  * @file    hi_g3lib_sap_interface.h
  * @author  AMG/IPC Application Team
  * @brief   Header file for the G3_LIB-Sap Interface
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
#ifndef HI_G3LIB_SAP_INTERFACE_H
#define HI_G3LIB_SAP_INTERFACE_H

#include <stdint.h>
#include "hi_g3lib_attributes.h"
#include "hi_g3lib_trace_modules.h"
#include <hi_adp_loadng_message_catalog.h>

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_g3lib_drv HIF G3LIB DRV
  * @{
  */

/** @defgroup G3_LIB G3_LIB
  * @brief    G3 LIB Management Module
  * @{
  */

/** @defgroup G3_LIB_Sap_Interface G3_LIB-SAP Interface
  * @brief This module defines the SAP between the G3_LIB and the APPLICATION Layers
  * @{
  */

/** @defgroup G3_LIB_SAP_IF_TypesDefinitions G3_LIB-SAP Interface Types Definitions
  * @brief  G3_LIB Sap Interface types definitions
  * @{
  */

#pragma pack(1)

/**
  * @brief  Structure of the G3LIB-SET.Request command
  */
typedef struct
{
    G3_LIB_PIB_t Attribute;          /**< @brief The attribute information */
} G3_LIB_SetAttributeRequest_t;

/**
  * @brief  Structure of the G3LIB-SET.Confirm command
  */
typedef struct
{
    uint8_t  State;                 /**< @brief The result of the operation  */
    G3_LIB_PIB_ID_t Attribute;       /**< @brief The ID+Index */
} G3_LIB_SetAttributeConfirm_t;

/**
  * @brief  Structure of the G3LIB-GET.Request command
  */
typedef struct
{
    G3_LIB_PIB_ID_t Attribute;      /**< @brief The ID+Index */
} G3_LIB_GetAttributeRequest_t;

/**
  * @brief  Structure of the G3LIB-GET.Confirm command
  */
typedef struct
{
    uint8_t  State;                 /**< @brief The result of the operation  */
    G3_LIB_PIB_t Attribute;          /**< @brief The attribute information */
} G3_LIB_GetAttributeConfirm_t;

/**
  * @brief  Structure of the G3LIB-TEST-MODE.Request command
  */
typedef struct
{
    uint8_t ID;                     /**< @brief The ID of the Test Mode to set  */
} G3_LIB_TestModeRequest_t;

/**
  * @brief  Structure of the G3LIB-TEST-MODE.Confirm command
  */
typedef struct
{
    uint8_t Status;                 /**< @brief The State of the G3LIB-TEST-MODE operation  */
} G3_LIB_TestModeConfirm_t;

/**
  * @brief  Structure of the G3LIB-SWRESET.Request command
  */
typedef struct
{
    uint8_t Bandplan;       /**< @brief The version of the G3 Library (#G3_LIB_G3PLC_CENA or #G3_LIB_G3PLC_FCC)  */
    uint8_t DeviceType;     /**< @brief The G3 Device type (#ADP_DEVICETYPE_DEVICE or #ADP_DEVICETYPE_COORD)  */
    uint8_t Mode;           /**< @brief The G3 Mode (PHY, MAC...)  */
} G3_LIB_SWResetRequest_t;

/**
  * @brief  Structure of the G3LIB-SWRESET.Confirm command
  */
typedef struct
{
    uint8_t Status;                 /**< @brief The State of the G3LIB-SWRESET operation  */
    uint8_t Bandplan;        /**< @brief The version of the G3 Library (#G3_LIB_G3PLC_CENA or #G3_LIB_G3PLC_FCC)  */
    uint8_t DeviceType;             /**< @brief The G3 Device type (#ADP_DEVICETYPE_DEVICE or #ADP_DEVICETYPE_COORD)  */
    uint8_t Mode;           /**< @brief The G3 Mode (PHY, MAC...)  */
} G3_LIB_SWResetConfirm_t;

/**
  * @}
  */

/**
  * @brief  Type of notification events
  * @details  These values are used to signal the type of the event occurred in the library
  * @{
  */
typedef enum
{
    RTEError = 0,         /**< @brief  Error event reported from the Real Time Engine */
    GMKUpdate,            /**< @brief  Update of the GMK */
    ContextTableUpdate,   /**< @brief  Context Information Table Update */
    GroupTableUpdate,     /**< @brief  Group Table Update */
    ActiveKeyIndexUpdate, /**< @brief  Active Key Index Update */
    ShortAddrUpdate,      /**< @brief  MAC Short Address (16-bits) Update */
    PANIdUpdate,          /**< @brief  MAC PAN ID Update */
    ToneMapTx,            /**< @brief  Transmission of ToneMap Response message */
    ToneMapRx,            /**< @brief  Reception of ToneMap Response message */
    RouteUpdate,          /**< @brief  Creation or Update of one route */
    PathRequestRx,        /**< @brief  Reception or a Path Request message for the node */
    ThermalEvent,         /**< @brief  Thermal event occurred */
    SurgeEvent,           /**< @brief  Surge event occurred */
    RTEWarning,           /**< @brief  Warning event reported from the Real Time Engine */
    RTEPollingError,      /**< @brief  Real Time Engine <-> Cortex polling failed */
    BootMaxRetries,       /**< @brief  Bootstrap application reaches max retires for the specific agent */
    RTEBandMismatch,      /**< @brief  Bandplan configured in RTE is not the expected one */
    PHYQualityRX,         /**< @brief  PHY Quality information on the RX frames */
    PHYQualityTX,         /**< @brief  PHY Quality information on the TX frames */
} G3_LIB_EventIndicationType_t;

/**
  * @}
  */

/**
  * @brief  Structure of the G3LIB-EVENT.Indication command
  * @details The event notification indicates to the external application that a specific event based on Type (#G3_LIB_EventIndicationType_t) occurs in the library
  */
typedef struct
{
    G3_LIB_EventIndicationType_t Type; /**< @brief  The type of the event. Based on the type defined in G3_LIB_EventIndicationType_t information can vary */
    union
    {
        struct{
            uint8_t KeyIndex;
            uint8_t Key[MAC_KEY_SIZE];
        } GMKUpdateInfo;  /**< @brief struct to contain information for type GMKUpdate */
        struct{
            uint8_t ContextIndex;
            ADP_ContextInformationEntry_t Context;
        } ContextTableUdateInfo;  /**< @brief struct to contain information for type ContextTableUpdate */
        struct{
            uint8_t  GroupIndex;
            uint16_t Group;
        } GroupTableUpdateInfo;  /**< @brief struct to contain information for type GroupTableUpdate */
        struct{
            uint8_t ActiveKeyIndex;
        } ActiveKeyIndexUpdateInfo; /**< @brief struct to contain information for type ActiveKeyIndexUpdate */
        struct{
            uint16_t ShortAddr;
        } ShortAddrUpdateInfo; /**< @brief struct to contain information for type ShortAddrUpdate */
        struct{
            uint16_t PANId;
        } PANIdUpdateInfo; /**< @brief struct to contain information for type PANIdUpdate */
        struct{
            MAC_DeviceAddress_t Address; /**< @brief The address of the node that has sent to us the Tone Map (Rx event) or to which we are sending the Tone Map (Tx Event) */
            union{
                MAC_ToneMapRespPay_CEN_t CEN_ToneMapRspPayload;
                MAC_ToneMapRespPay_FCC_t FCC_ToneMapRspPayload;
            };
        } ToneMapTxRxInfo;   /**< @brief struct that contains the transmitted or received ToneMap message */
        struct{
            ADP_RoutingTableEntry_t RoutingTableEntry; /**< @brief  The full routing entry related to this event */
            uint8_t                 Originator; /**< @brief  0 if the event is generated for a request initiated by the node itself, 1 if the event is genereated from a request coming from the endpoint */
        } RouteUpdateInfo;
        struct{
            uint16_t      PREQLen;
            LOADng_PREQ_t PREQ;
        } PathRequestRxInfo;
        struct{
            uint8_t ThermalEventType;    /**< @brief  The type of the ThermalEvent - 0: no thermal warn 1: LD thermal warn 2: SoC thermal warn 3: LD+SoC thermal warn */
        } ThermaEventInfo;
        struct{
            ADP_PanDescriptor_t PanDescriptor;
        } BootMaxRetriesInfo;
        struct{
            uint8_t  RTEBand;
        } RTEBandMismatch;
        struct
        {
          MAC_DeviceAddress_t      SrcAddr;   /**< @brief The Source Address of the node that has sent the packet   */
          PHY_IndParams_t          PhyParams; /**< @brief Channel quality parameters estimated by the PHY Layer */
        } PHYQualityRx;
        struct
        {
          MAC_DeviceAddress_t  DstAddr;           /**< @brief The Destination Address of the node to which the packet is sent */
          uint8_t SignalPow[PHY_NUM_OF_CARRIERS]; /**< @brief Signal power of subcarrier 0, ..., 71 at the output of the device (in dB�V) */
        } PHYQualityTx;
    } Info; /**< @brief Union of different information for the notification event */
} G3_LIB_EventIndication_t;

/**
  * @brief  Structure of the G3LIB-TRACE.Indication command
  * @details The event notification indicates the presence of trace information
  */
typedef struct
{
    G3LIB_TracePrio_t Prio;
    G3LIB_TraceType_t Type;
    G3LIB_TraceModule_t Module;
    uint32_t Id;
    uint8_t *Data;
    uint16_t Size;
} G3_LIB_BinaryTrace_t;

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
#endif //HI_G3LIB_SAP_INTERFACE_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
