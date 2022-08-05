/**
  ******************************************************************************
  * @file    hi_mac_message_catalog.h
  * @author  AMG/IPC Application Team
  * @brief   Header file with the definitions of the MAC PDUs
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
#ifndef HI_MAC_MESSAGE_CATALOG_H
#define HI_MAC_MESSAGE_CATALOG_H

#include "hi_mac_constants.h"
#include "hi_mac_pib_types.h"

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_mac_drv HIF MAC DRV
  * @{
  */

/** @defgroup MAC_Data_Structures MAC Data Structures
  * @brief MAC data structures module
  * @{
  */

/** @defgroup MAC_Frame MAC Frame
  * @brief    MAC Frame module
  * @details  Frame Format:
  * @n        The G3 standard refers to the message structure as a frame. The frames are the messages that are exchanged
  *           between the nodes. Three kinds of frame are used, the Beacon Frames, the Data Frames and the Command
  *           Frames. All the frame types derive from a General MAC Frame Format.
  *           Messages are described starting from right to left, as the bit 0 the lsb and bit 7 as msb. The first bit to be
  *           transmitted by the PHY Layer is the lsb. For fields that are longer that 1 byte, the first byte to be transmitted
  *           contains the lowest bits or, in other words, the lsb.
  * @{
  */

/** @defgroup MAC_Frame_TypesDefinitions Frame Types Definitions
  * @brief  Frame types definitions
  * @{
  */

/* Size Definitions */
/* Header */
#define MAC_SCH_SIZE                 ((uint16_t)0x03)  /**< @brief Segment Control Header size (in bytes) */
#define MAC_FCH_SIZE                 ((uint16_t)0x02)  /**< @brief Frame Control Header size (in bytes) */
#define MAC_AUX_SIZE                 ((uint16_t)0x06)  /**< @brief Auxiliary Security Header size (in bytes) */
#define MAC_SEQNUM_SIZE              ((uint16_t)0x01)  /**< @brief Sequence Number size (in bytes) */
#define MAC_FIX_MHR_SIZE             ((uint16_t)(MAC_SCH_SIZE + MAC_FCH_SIZE + MAC_SEQNUM_SIZE)) /**< @brief Size of Mac Header fields always present in the frame */
#define MAC_MAX_MHR_SIZE             ((uint16_t)(MAC_FIX_MHR_SIZE + MAC_PAN_ID_SIZE + MAC_ADDR64_SIZE + MAC_PAN_ID_SIZE + MAC_ADDR64_SIZE)) /**< @brief Maximum size of the Mac Header */
/* FCS */
#define CRC16_SIZE                   ((uint16_t)0x02)  /**< @brief Size of CRC 16 value */
/* CMD ID */
#define MAC_CMD_ID_SIZE              ((uint16_t)0x01)  /**< @brief Size of CMD ID: 8 bit*/
/* Overhead sizes */
#define MAC_FIX_OVERHEAD_SIZE        ((uint16_t)(MAC_FIX_MHR_SIZE + CRC16_SIZE)) /**< @brief Fixed overhead size: Header of fixed length plus crc16 */

/** @defgroup MAC_Frame_PayloadTypesDefinitions Frame Payload Type Definitions
  * @brief Payload Sizes including Command Header ID
  * @{
  */
#define MAC_BEACON_REQ_PAY_SIZE      (MAC_CMD_ID_SIZE + (uint16_t)sizeof(MAC_BeaconReqPay_t) - CRC16_SIZE)  /**< @brief Beacon Request payload size */
#define MAC_TONEMAPRESP_FCC_PAY_SIZE (MAC_CMD_ID_SIZE + (uint16_t)sizeof(MAC_ToneMapRespPay_FCC_t) - CRC16_SIZE)  /**< @brief Tone Map Response payload size */
#define MAC_TONEMAPRESP_CEN_PAY_SIZE (MAC_CMD_ID_SIZE + (uint16_t)sizeof(MAC_ToneMapRespPay_CEN_t) - CRC16_SIZE)  /**< @brief Tone Map Response payload size */

/**
  * @}
  */

#define MAC_MAX_FRAMEDATA_SIZE       ((uint16_t)(2*MAC_PAN_ID_SIZE + 2*MAC_ADDR64_SIZE + MAC_TOTALPAYLOAD_SIZE + CRC16_SIZE))  /**< @brief The maximum size of the Addresses + Payload + FCS */

/* FIELDS VALUE macro */
/* Reserved field values */
#define MAC_RESERVED_FIELD               ((uint8_t)0x00)  /**< @brief Value of the Reserved field in the Mac Frame */

/* Tone Map Request */
#define MAC_TONE_MAP_REQUEST_ON          ((uint8_t)0x01)  /**< @brief Tone Map Request bit is set */
#define MAC_TONE_MAP_REQUEST_OFF         ((uint8_t)0x00)  /**< @brief Tone Map Request bit is not set */

/* CC Values */
#define MAC_CONTENTION_CONTROL_ALLOWED   ((uint8_t)0x00)  /**< @brief Contention Control allowed */
#define MAC_CONTENTION_FREE_ACCESS       ((uint8_t)0x01)  /**< @brief Contention free access */

/* CAP Values */
#define MAC_CHANNEL_PRIORITY_NORMAL      ((uint8_t)0x00)  /**< @brief Normal priority on channel access */
#define MAC_CHANNEL_PRIORITY_HIGH        ((uint8_t)0x01)  /**< @brief High priority on channel access */

/* LSF Values */
#define MAC_LAST_SEGMENT_FLAG_NOT_LAST   ((uint8_t)0x00)  /**< @brief Not Last Segment indication */
#define MAC_LAST_SEGMENT_FLAG_LAST       ((uint8_t)0x01)  /**< @brief Last Segment indication */

/* Security Enable */
#define MAC_SECURITY_NOT_ENABLED         ((uint8_t)0x00)  /**< @brief Frame protected by the security sublayer */
#define MAC_SECURITY_ENABLED             ((uint8_t)0x01)  /**< @brief Frame not protected by a security sublayer */

#define MAC_SECURITY_KEYIDMODE           ((uint8_t)0x01)  /**< @brief Key Identifier mode should be 0x01: key identified by 1-octect key index */

/* ACK Request */
#define MAC_ACK_REQUEST_OFF              ((uint8_t)0x00)  /**< @brief The ACK is not required for the current frame */
#define MAC_ACK_REQUEST_ON               ((uint8_t)0x01)  /**< @brief The ACK is required for the current frame */

/* Frame Pending */
#define MAC_FRAME_PENDING                ((uint8_t)0x00)  /**< @brief Always set to 0 */

/* PAN ID Compression */
#define MAC_PAN_ID_COMPRESSION_OFF       ((uint8_t)0x00)  /**< @brief Send both destination and source Pan Id */
#define MAC_PAN_ID_COMPRESSION_ON        ((uint8_t)0x01)  /**< @brief Send only the destination Pan Id */

/* Frame Version */
#define MAC_FRAME_VERSION                ((uint8_t)0x00)  /**< @brief Fixed to 0 */

/* Address field types */
#define MAC_NO_ADDR_FIELD                ((uint8_t)0x00)  /**< @brief Address not present */
#define MAC_ADDR_MODE_16                 ((uint8_t)0x02)  /**< @brief 16-bit Address mode */
#define MAC_ADDR_MODE_64                 ((uint8_t)0x03)  /**< @brief 64-bit Address mode */

/* Frame Types */
#define MAC_BEACON_FRAME_TYPE            ((uint8_t)0x00)  /**< @brief Beacon Frame type    */
#define MAC_DATA_FRAME_TYPE              ((uint8_t)0x01)  /**< @brief Data Frame type      */
#define MAC_COMMAND_FRAME_TYPE           ((uint8_t)0x03)  /**< @brief Command Frame type   */
#define MAC_RES_FRAME_TYPE               ((uint8_t)0x02)  /**< @brief Reserved Frame type  */

/* Command Frame ids */
#define MAC_BEACON_REQ_ID                ((uint8_t)0x07)  /**< @brief Beacon Request Id  */
#define MAC_TONEMAP_RESP_ID              ((uint8_t)0x0A)  /**< @brief Tone Map Response Id  */

/* Specific Message Type */
// WARNING: Superframe specification is 16-bits wide, need to change endianness
#define MAC_BEACON_SUPERFRAME_PANCOORD  ((uint16_t)0xC000)  /**< @brief Beacon Frame payload - Superframe Specification part (for PAN Coordinator)  */
#define MAC_BEACON_SUPERFRAME_COORD     ((uint16_t)0x8000)  /**< @brief Beacon Frame payload - Superframe Specification part (for Coordinator)  */
#define MAC_BEACON_GTS_PA               ((uint16_t)0x0000)  /**< @brief Beacon Frame payload - GTS and Pending Address Specifications  */
#define MAC_BROADCAST_PAN_ID            ((uint16_t)0xFFFF)              /**< @brief Broadcast Pan Id value  */
#define MAC_BROADCAST_SHORT_ADDR        ((uint16_t)0xFFFF)              /**< @brief Broadcast Short Address value  */
#define MAC_BROADCAST_EXTENDED_ADDR     ((uint64_t)0xFFFFFFFFFFFFFFFF)  /**< @brief Broadcast Extended Address value  */


/**
  * @brief  Segment Control Header section of MAC Frame
  */
typedef struct
{
    // byte 1
    uint8_t   LSF  : 1;  /**< @brief Last Segment Flag, set to 1 if this is the last segment transmitted  */
    uint8_t   CAP  : 1;  /**< @brief Channel Access Priority, 0 for Normal 1 for High  */
    uint8_t   CC   : 1;  /**< @brief Contention Control, set to 0 if the contention is allowed in the next contention state, 1 for
                           *         Contention Free Access  */
    uint8_t   TMR  : 1;  /**< @brief Tone Map Request, set to 1 if a Tone Map Response is requested  */
    uint8_t   Res  : 4;  /**< @brief Reserved Field  */
    // byte 2
    uint8_t   SL98 : 2;  /**< @brief Segment Length of the MAC Frame (without padding), bit 8-9  */
    uint8_t   SC   : 6;  /**< @brief Segment Count, set to 0 in the first segment and incremented in every next segments  */
    // byte 3
    uint8_t   SL70;      /**< @brief Segment Length of the MAC Frame (without padding), bit 0-7  */
} SegmentControlHeader_t;

/**
  * @brief  Frame Control Header section of MAC Frame
  */
typedef struct
{
    // byte 1
    uint8_t   FrameType : 3;    /**< @brief 0b000 Beacon Frame, 0b001 Data Frame, 0b011 MAC Command Frame  */
    uint8_t   SE        : 1;    /**< @brief Security Enable, set to 1 if the frame is protected by the security sublayer  */
    uint8_t   FP        : 1;    /**< @brief Frame Pending (always set to 0)  */
    uint8_t   AckReq    : 1;    /**< @brief Acknowledgement Request, set to 1 if the ACK is required for the current frame  */
    uint8_t   PIC       : 1;    /**< @brief PAN ID Compression, set to 1 for sending only the Destination PAN Identifier if equal to the Source  */
    uint8_t   Rsv1      : 1;    /**< @brief Reserved field  */
    // byte 2
    uint8_t  Rsv            :2; /**< @brief Reserved field  */
    uint8_t  DestAddrMode   :2; /**< @brief Destination address mode: 0b00 PAN ID and Address field are not present, 0b10 16-bit Address field only, 0b11 64-bit Address field only  */
    uint8_t  FrameVersion   :2; /**< @brief Fixed to 0b00  */
    uint8_t  SourceAddrMode :2; /**< @brief Source address mode: 0b00 PAN ID and Address field are not present, 0b10 16-bit Address field only, 0b11 64-bit Address field only  */

} FrameControlHeader_t;

/**
  * @brief  Auxiliary Secuirty Header section of MAC Frame
  */
typedef struct
{
    // byte 1
    uint8_t  SecurityLevel     : 3; /**< @brief Security Level fieldL 0x00 No security, 0x05 ENC-MIC-32 with CCM*  */
    uint8_t  KeyIdentifierMode : 2; /**< @brief Key Identifier Mode field. Should be only 0x01, Key is identified by 1-octect Key Index  */
    uint8_t  Rsv               : 3; /**< @brief Reserved field  */
    uint32_t FrameCounter;          /**< @brief Frame counter field. Contain the value of the attribute macFrameCounter  */
    uint8_t  KeyIndex;              /**< @brief KeyIndex field  */

} AuxiliarySecurityHeader_t;

/**
  * @brief  Generic MAC Header structure for fixed part only. This does not include Addressing fields and Auxiliary security Header
  */
typedef struct
{
    SegmentControlHeader_t  SegmentControl; /**< @brief Segment Control Header  */
    FrameControlHeader_t    FrameControl;   /**< @brief Frame Control Header  */
    uint8_t                 SeqNum;         /**< @brief Frame Sequence Number (contains the macBSN for the Beacon frame and the macDSN for the other frame types)  */

} MAC_MHR_t;

/**
  * @brief  Beacon Frame Payload structure
  */
typedef struct
{
    uint16_t BeaconPayload;   /**< @brief Beacon Payload for G3-PLC (ITU G.9903) is only 16 bits Route Cost to coordinator (RC_COORD) */
} MAC_BeaconPayload_t;

/**
  * @brief  Generic Beacon Frame Payload structure
  */
typedef struct
{
  uint16_t SuperFrameSpec;   /**< @brief Superframe Specification section  */
  uint16_t GTSPendAddrSpec;  /**< @brief GTS and Pending Address Specifications section  */
  MAC_BeaconPayload_t BeaconPay;    /**< @brief Beacon Payload field depends on the protocol (see ::MAC_BeaconPayload_t) */
  uint16_t FCS;              /**< @brief Frame Check Sequence  */
} MAC_BeaconFramePay_t;

/**
  * @brief  Beacon Request Payload structure, CmdFrameId = 0x07
  */
typedef struct
{
  uint16_t FCS;        /**< @brief Frame Check Sequence  */
} MAC_BeaconReqPay_t;

/**
  * @brief  Tone Map Response Payload structure for FCC bandplan, CmdFrameId = 0x0A
  */
typedef struct
{
    uint8_t  Mod       : 3;             /**< @brief Requested Modulation Type (0: Robust mode, 1: DBPSK or BPSK, 2: DQPSK or QPSK, 3: D8PSK or 8-PSK, 4: 16-QAM, 5-7: Reserved by ITU-T)  */
    uint8_t  TxGain    : 4;             /**< @brief Desired transmitter gain specifying how many gain steps are requested */
    uint8_t  TxRes     : 1;             /**< @brief Tx Gain resolution corresponding to one gain step (0: 6 dB, 1: 3 dB) */
    uint8_t  ToneMap[PHY_TONEMAP_SIZE]; /**< @brief Tone map */
    uint8_t  LQI;                       /**< @brief Link quality indicator */
    PHY_FCCTXCoeff_t TXCoeff;           /**< @brief Specifies the number of gain steps requested for the tones represented by Tone Map */
    uint8_t  Res       : 7;             /**< @brief  Shall be set to zero at the transmitter and ignored by the receiver */
    uint8_t  PayModSch : 1;             /**< @brief Payload Modulation Scheme (0 Differential, 1 Coherent) */
    uint16_t FCS;                       /**< @brief Frame Check Sequence  */
} MAC_ToneMapRespPay_FCC_t;

/**
  * @brief  Tone Map Response Payload structure for CEN bandplan, CmdFrameId = 0x0A
  */
typedef struct
{
    uint8_t  PayModSch : 1;    /**< @brief Payload Modulation Scheme (0 Differential, 1 Coherent) */
    uint8_t  Mod       : 2;    /**< @brief Requested Modulation Type (0: ROBO, 1: DBSPK, 2: DQPSK)  */
    uint8_t  TxGain    : 4;    /**< @brief Desired Transmitted gain specifying how many gain steps are requested  */
    uint8_t  TxRes     : 1;    /**< @brief Tx Gain Resolution (0: 6 dB, 1: 3 dB)  */
    uint8_t  TM05      : 6;    /**< @brief Tone Map value (bit 0-5)  */
    uint8_t  Res       : 2;    /**< @brief Reserved field */
    uint8_t  LQI;              /**< @brief Link Quality Indicator  */
    PHY_CENTXCoeff_t TXCoeff;  /**< @brief Specifies the number of gain steps requested for the tones represented by Tone Map */
    uint8_t  Rsv;              /**< @brief Reserved Field  */
    uint16_t FCS;              /**< @brief Frame Check Sequence  */
} MAC_ToneMapRespPay_CEN_t;

/**
  * @brief  Generic Command Frame Payload structure
  */
typedef struct
{
    uint8_t CmdFrameId;      /**< @brief Command Frame Id  */
    union
    {
        MAC_BeaconReqPay_t        BeaconReqPayload;       /**< @brief Beacon Request payload */
        MAC_ToneMapRespPay_CEN_t  CEN_ToneMapRspPayload;  /**< @brief Tone Map Response payload for G3 CEN-A and CEN-B */
        MAC_ToneMapRespPay_FCC_t  FCC_ToneMapRspPayload;  /**< @brief Tone Map Response payload for G3 FCC */
    };
} MAC_CommandFramePayload_t;

/**
  * @brief  Generic FrameData structure (used to decode messages)
  */
typedef struct
{
  uint8_t                     RawData[MAC_MAX_FRAMEDATA_SIZE + MAC_AUX_SIZE]; /**< @brief Buffer used to decode messages  */
} MAC_FrameData_t;

/**
  * @brief   Generic MAC Frame structure
  * @details All the specific frame formats derive from a common structure named General MAC Frame Format.
  * @n@n     The Segment Control sub-header contains the information related to the current segment:
  * @li      TMR: Tone Map Request, set to 1 if a Tone Map Response is requested (see 2.2.3)
  * @li      CC: Contention Control, set to 0 if the contention is allowed in the next contention state, 1 for
  *          Contention Free Access.
  * @li      CAP: Channel Access Priority, 0 for Normal 1 for High (see 2.2.6)
  * @li      LSF: Last Segment Flag, set to 1 if this is the last segment transmitted (See 2.2.4)
  * @li      SC: Segment Count is set to 0 in the first segment and is incremented in every next segments
  * @li      SL: Segment Length of the MAC Frame (without padding).

  * @n       The Frame Control sub-header contains the information related to the type of the frame and the used
  *          addressing mode:
  * @li      Frame Type: 0b000 Beacon Frame, 0b001 Data Frame, 0b011 MAC Command Frame
  * @li      SE: Security Enable set to 1 if the frame is protected by the security sublayer. See 2.2.13.
  * @li      FP: Frame Pending is always 0
  * @li      AckReq: Acknowledgement Request is set to 1 if the ACK is required for the current frame
  * @li      PIC: PAN ID Compression is set to 1 for sending only the Destination PAN Identifier if equal to the Source
  * @li      Dest Addr Mode and Src Addr Mode: 0b00 PAN ID and Address field are not present 0b10 16-bit
  *          Address field only, 0b11 64-bit Address field only (see 2.2.11)
  * @li      Frame Version: fixed to 0b00.

  * @n       The Sequence Number contains the macBSN for the Beacon frame and the macDSN for the other frame types.
  * @n       The Destination PAN ID could be 0 or 2 bytes based on the Dst Addr Mode.
  * @n       The Destination Address could be 0, 2 or 8 bytes based on the Dst Addr Mode.
  * @n       The Source PAN ID could be 0 or 2 bytes based on the Frame Control PIC and Src Addr Mode.
  * @n       The Source Address could be 0, 2 or 8 bytes based on the Frame Control PIC and Src Addr Mode.
  * @n       The Auxiliary Security header could be 0 or 5 bytes, it is used when the MAC Security sublayer is adopted (if SE
  *          bit in the Frame Control sub-header is set to 1). See Table 7 for the complete format.
  * @n       The Frame Payload contains the information data exchanged within the message
  * @n       The Frame Check Sequence is a 16-bit CRC (see 2.2.9). It is sent as FCS[0..7] FCS[8..15].
  */
typedef struct
{
  MAC_MHR_t       MacHeader;  /**< @brief Mac Frame Header: SCH + FCH + SeqNum  */
  MAC_FrameData_t FrameData;  /**< @brief Mac Frame Data: Addresses + Payload + FCS  */
} MAC_Frame_t;


#define SECURITY_ADATA_MAX_LEN (MAC_MAX_MHR_SIZE + MAC_AUX_SIZE - sizeof(SegmentControlHeader_t) + 1) /**< @brief The size of aData used for Security operation  */

#endif //HI_MAC_MESSAGE_CATALOG_H

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

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
