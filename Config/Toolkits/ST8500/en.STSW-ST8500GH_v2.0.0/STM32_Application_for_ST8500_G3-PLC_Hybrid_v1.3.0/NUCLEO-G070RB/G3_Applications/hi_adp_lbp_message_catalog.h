/**
  ******************************************************************************
  * @file    hi_adp_lbp_message_catalog.h
  * @author  AMG/IPC Application Team
  * @brief   Header file to define the packets used in the LBP layer
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
#ifndef HI_ADP_LBP_MESSAGE_CATALOG_H
#define HI_ADP_LBP_MESSAGE_CATALOG_H

#include <stdint.h>
#include "hi_mac_constants.h"

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_adp_drv HIF ADP DRV
  * @{
  */


/** @defgroup LBP_MessageCatalog Message Catalog
  * @brief Definition of the messages used by LOADng protocol
  * @{
  */

/** @defgroup LBP_TypesDefinitions Definition of types
  * @brief Definition of macros and types used in messages for LBP
  * @{
  */

  // interface macros
#define LBP_UINT16_BYTEREVERSE(a) ( ((a >> 8) & 0x00FF) + ((a << 8 ) & 0xFF00) )  /**< @brief Macro to swap 16 bits */

#define ADP_LBP_MESSAGE_FROM_LBD  ((uint8_t) 0x00)  /**< @brief Type of LBP message: from LBD */
#define ADP_LBP_MESSAGE_TO_LBD    ((uint8_t) 0x01)  /**< @brief Type of LBP message: to LBD */

/* Identifies the LBP message codes for messages directed to the LBD */
#define ADP_LBP_ACCEPTED          ((uint8_t) 0x01)  /**< @brief Authentication succeeded with delivery of device specific information (DSI) to the LBD */
#define ADP_LBP_CHALLENGE         ((uint8_t) 0x02)  /**< @brief Authentication in progress. PAN specific information (PSI) may be delivered to the LBD */
#define ADP_LBP_DECLINE           ((uint8_t) 0x03)  /**< @brief Authentication failed */
#define ADP_LBP_ACK               ((uint8_t) 0x05)  /**< @brief The LBS acknowledges the notification */

/* Identifies the LBP message codes for messages directed to the LBS */
#define ADP_LBP_JOINING           ((uint8_t) 0x01)  /**< @brief The LBD requests joining a PAN and provides the necessary authentication material */
#define ADP_LBP_NOTIFICATION      ((uint8_t) 0x05)  /**< @brief The LBD notifies the LBS of its presence within the PAN */

/* Identifies the LBP message codes for messages directed in both directions */
#define ADP_LBP_KICK              ((uint8_t) 0x04)  /**< @brief KICK frame is used by a PAN coordinator to force a device to lose its MAC address, or by any device to inform the coordinator that it left the PAN. */

#define ADP_CONFIG_PARAMS_BUF_LEN ((uint8_t) 0x80)  /**< @brief Implementation defined buffer length for Configuration Parameters */

/* EAP message types */
#define ADP_EAP_REQUEST           ((uint8_t) 0x01) /**< @brief Request (sent to the peer = LBD) */
#define ADP_EAP_RESPONSE          ((uint8_t) 0x02) /**< @brief Response (sent by the peer) */
#define ADP_EAP_SUCCESS           ((uint8_t) 0x03) /**< @brief Success (sent to the peer) */
#define ADP_EAP_FAILURE           ((uint8_t) 0x04) /**< @brief Failure (sent to the peer) */

#define ADP_EAP_PSK_IANA_TYPE     ((uint8_t) 0x2F) /**< @brief EAP Method Type: EAP-PSK */

/* T-subfield types */
#define ADP_EAP_PSK_T0            ((uint8_t) 0x00) /**< @brief 0 The first EAP-PSK message */
#define ADP_EAP_PSK_T1            ((uint8_t) 0x01) /**< @brief 1 The second EAP-PSK message */
#define ADP_EAP_PSK_T2            ((uint8_t) 0x02) /**< @brief 2 The third EAP-PSK message */
#define ADP_EAP_PSK_T3            ((uint8_t) 0x03) /**< @brief 3 The forth EAP-PSK message */

/* Message Length macros */
#define ADP_EAP_MACP_SEED_MAX_LEN     ((uint8_t) 2*(ADP_EAP_PSK_RAND_LEN + ADP_EAP_PSK_ID_MAX_LEN))  /**< @brief MACP SEED maximum size */
#define ADP_EAP_MACS_SEED_MAX_LEN     ((uint8_t) ADP_EAP_PSK_RAND_LEN + ADP_EAP_PSK_ID_MAX_LEN)      /**< @brief MCAS SEED maximum size */
#define ADP_EAP_MAC_LEN               ((uint8_t) 0x10)   /**< @brief MAC size */
#define ADP_EAP_PSK_KEY_LEN           ((uint8_t) 0x10)   /**< @brief PSK Key size */
#define ADP_EAP_PSK_MSG_NONCE_LEN     ((uint8_t) 0x04)   /**< @brief PSK MSG NONCE size */
// Since EAX uses a 16-byte nonce, the Nonce is padded with 96 zero bits
#define ADP_EAP_PSK_EAX_NONCE_LEN     ((uint8_t) 0x10)   /**< @brief PSK EAX NONCE size */
#define ADP_EAP_PSK_TAG_LEN           ((uint8_t) 0x10)   /**< @brief PSK TAG size */
#define ADP_EAP_PSK_RAND_LEN          ((uint8_t) 0x10)   /**< @brief PSK RAND size */
#define ADP_EAP_PSK_ID_MAX_LEN        ((uint8_t) 0x24)   /**< @brief PSK IDs (ID_P, ID_S) maximum size */

#define ADP_EAP_PSK_MSG_MIN_LEN        ((uint16_t) ADP_EAP_HEADER_SIZE + ADP_EAP_PSK_HEADER_SIZE)  /**< @brief PSK MSG minimum size */
#define ADP_EAP_PSK_FIRST_MSG_MIN_LEN  ((uint16_t) ADP_EAP_PSK_MSG_MIN_LEN + ADP_EAP_PSK_RAND_LEN) /**< @brief PSK First message minimum size */
#define ADP_EAP_PSK_SECOND_MSG_MIN_LEN ((uint16_t) ADP_EAP_PSK_MSG_MIN_LEN + sizeof(EAP_PSK_Second_Msg_t) - ADP_EAP_PSK_ID_MAX_LEN) /**< @brief PSK Second message minimum size */
#define ADP_EAP_PSK_THIRD_MSG_CLEAR    ((uint16_t) ADP_EAP_PSK_MSG_MIN_LEN + ADP_EAP_PSK_RAND_LEN + ADP_EAP_MAC_LEN + ADP_EAP_PSK_MSG_NONCE_LEN + ADP_EAP_PSK_TAG_LEN) /**< @brief Unencrypted part of the message: RAND_S (16-byte) + MAC_S (16-byte) + Nonce (4-byte) + Tag (16-byte) */
#define ADP_EAP_PSK_AUTH_HEADER_LEN    ((uint16_t) ADP_EAP_PSK_MSG_MIN_LEN + ADP_EAP_PSK_RAND_LEN) /**< @brief As per RFC 4764 - 3.3 (EAP Code + Id + Length + Type + Flags + RandS) */
#define ADP_EAP_PSK_FOURTH_MSG_CLEAR   ((uint16_t) ADP_EAP_PSK_MSG_MIN_LEN + ADP_EAP_PSK_RAND_LEN + ADP_EAP_PSK_MSG_NONCE_LEN + ADP_EAP_PSK_TAG_LEN) /**< @brief Unencrypted part of the message: RAND_S (16-byte) + Nonce (4-byte) + Tag (16-byte) */

/* P-Channel result field */
#define ADP_PCHANNEL_RESULT_CONTINUE      ((uint8_t) 0x01) /**< @brief P-Channel result field: Continue */
#define ADP_PCHANNEL_RESULT_DONE_SUCCESS  ((uint8_t) 0x02) /**< @brief P-Channel result field: Success  */
#define ADP_PCHANNEL_RESULT_DONE_FAILURE  ((uint8_t) 0x03) /**< @brief P-Channel result field: Failure  */


#define ADP_CONFIG_PARAMS_EXT_TYPE        ((uint8_t) 0x02) /**< @brief G3 EAP-PSK Extension */

  // interface typedef

/**
  * @brief  LoWPAN bootstrapping protocol (LBP) header format
  */
typedef struct
{
  uint8_t  Reserved_1   : 4;        /**< @brief Reserved by ITU-T, set to 0 by the sender and ignored by the receiver     */
  uint8_t  Code         : 3;        /**< @brief Identifies the message code (JOINING, ACCEPTED, CHALLENGE, DECLINE, KICK) */
  uint8_t  T            : 1;        /**< @brief Identifies the type of message (0: Message from LBD, 1: Message to LBD)   */
  uint8_t  Reserved_2;              /**< @brief Reserved by ITU-T, set to 0 by the sender and ignored by the receiver     */
  uint8_t  A_LBD[MAC_ADDR64_SIZE];  /**< @brief Indicates the EUI-64 address of the bootstrapping device (LBD).           */

} LBP_Header_t;

#define ADP_LBP_HEADER_SIZE ((uint16_t) sizeof(LBP_Header_t))  /**< @brief LBP Header size */


/**
  * @brief  LoWPAN bootstrapping protocol (LBP) message format
  */
typedef struct
{
  LBP_Header_t Header;                                         /**< @brief Headers */
  uint8_t      Bootstrapping_Data[ADP_CONFIG_PARAMS_BUF_LEN];  /**< @brief Contains additional information elements */

} LBP_Message_t;


/**
  * @brief  EAP-PSK PCHANNEL field Format
  */
typedef struct
{
  uint8_t  Nonce[ADP_EAP_PSK_MSG_NONCE_LEN];       /**< @brief Nonce */
  uint8_t  Tag[ADP_EAP_PSK_TAG_LEN];               /**< @brief Tag */
  uint8_t  Reserved    :5;                         /**< @brief Reserved field, which is set to zero on emission and ignored on reception */
  uint8_t  E           :1;                         /**< @brief A 1-bit extension flag E, which is set to 1 if an EAP-PSK Extension is present */
  uint8_t  R           :2;                         /**< @brief A 2-bit result indication flag R */
  uint8_t  EXT_Type;                               /**< @brief The EXT_Type subfield indicates the type of the extension */
  uint8_t  EXT_Payload[ADP_CONFIG_PARAMS_BUF_LEN]; /**< @brief The EXT_Payload subfield consists of the payload of the extension */

} EAP_PSK_PCHANNEL_t;


/**
  * @brief  EAP-PSK First Message Format
  */
typedef struct
{
  uint8_t          RAND_S[ADP_EAP_PSK_RAND_LEN];  /**< @brief A 16-byte random number */
  uint8_t          ID_S[ADP_EAP_PSK_ID_MAX_LEN];  /**< @brief A field that conveys the server's NAI: ID_S  */

} EAP_PSK_First_Msg_t;


/**
  * @brief  EAP-PSK Second Message Format
  */
typedef struct
{
  uint8_t          RAND_S[ADP_EAP_PSK_RAND_LEN];  /**< @brief The 16-byte random number sent by the server in the first EAP-PSK message */
  uint8_t          RAND_P[ADP_EAP_PSK_RAND_LEN];  /**< @brief A 16-byte random number */
  uint8_t          MAC_P[ADP_EAP_MAC_LEN];        /**< @brief A 16-byte MAC */
  uint8_t          ID_P[ADP_EAP_PSK_ID_MAX_LEN];  /**< @brief A field that conveys the peer's NAI: ID_P  */

} EAP_PSK_Second_Msg_t;


/**
  * @brief  EAP-PSK Third Message Format
  */
typedef struct
{
  uint8_t            RAND_S[ADP_EAP_PSK_RAND_LEN]; /**< @brief The 16-byte random number sent by the server in the first EAP-PSK message */
  uint8_t            MAC_S[ADP_EAP_MAC_LEN];       /**< @brief A 16-byte MAC */
  EAP_PSK_PCHANNEL_t PCHANNEL;                     /**< @brief A variable length field that constitutes the protected channel */

} EAP_PSK_Third_Msg_t;


/**
  * @brief  EAP-PSK Fourth Message Format
  */
typedef struct
{
  uint8_t            RAND_S[ADP_EAP_PSK_RAND_LEN]; /**< @brief The 16-byte random number sent by the server in the first EAP-PSK message */
  EAP_PSK_PCHANNEL_t PCHANNEL;                     /**< @brief A variable length field that constitutes the protected channel */

} EAP_PSK_Fourth_Msg_t;


/**
  * @brief  EAP Header Format
  */
typedef struct
{
  uint8_t  Type;           /**< @brief This field indicates the Type of Request or Response. */
  uint8_t  Reserved  :6;   /**< @brief Reserved subfield that is set to zero on transmission and ignored on reception */
  uint8_t  T         :2;   /**< @brief Indicates the type of EAP-PSK message */

} EAP_PSK_Header_t;

#define ADP_EAP_PSK_HEADER_SIZE     ((uint16_t) sizeof(EAP_PSK_Header_t)) /**< @brief EAP PSK Header size */


/**
  * @brief  EAP Header Format
  */
typedef struct
{
  uint8_t  Reserved :2;   /**< @brief Two bits set as 0 */
  uint8_t  Code     :6;   /**< @brief The Code field identifies the Type of EAP packet (Request, Response, Success, Failure)     */
  uint8_t  Identifier;    /**< @brief The Identifier field aids in matching Responses with Requests. */
  uint16_t Length;        /**< @brief The Length field indicates the length, in octets, of the EAP packet including the Code, Identifier, Length, and Data fields.   */

} EAP_Header_t;

#define ADP_EAP_HEADER_SIZE     ((uint16_t) sizeof(EAP_Header_t))  /**< @brief EAP Header size */


/**
  * @brief  EAP Packet Format
  */
typedef struct
{
  EAP_Header_t     EAP_Header;             /**< @brief Header */
  EAP_PSK_Header_t EAP_PSK_Header;          /**< @brief PSK Header */
  union {
      EAP_PSK_First_Msg_t  EAP_PSK_First;  /**< @brief First Message */
      EAP_PSK_Second_Msg_t EAP_PSK_Second; /**< @brief Second Message */
      EAP_PSK_Third_Msg_t  EAP_PSK_Third;  /**< @brief Third Message */
      EAP_PSK_Fourth_Msg_t EAP_PSK_Fourth; /**< @brief Fourth Message */
  };

} EAP_Packet_t;



  // interface global variables

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

#endif //HI_ADP_LBP_MESSAGE_CATALOG_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
