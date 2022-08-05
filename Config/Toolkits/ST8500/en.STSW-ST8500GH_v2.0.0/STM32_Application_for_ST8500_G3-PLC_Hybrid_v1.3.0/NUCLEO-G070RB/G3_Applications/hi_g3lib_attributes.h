/**
  ******************************************************************************
  * @file    hi_g3lib_attributes.h
  * @author  AMG/IPC Application Team
  * @brief   Header file for Device PIB Attributes definitions
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
#ifndef HI_G3LIB_ATTRIBUTES_H
#define HI_G3LIB_ATTRIBUTES_H

#include <hi_mac_pib_attributes.h>
#include <hi_mac_constants.h>
#include <hi_mac_message_catalog.h>
#include <hi_adp_pib_types.h>
#include <hi_adp_pib_attributes.h>
#include <hi_adp_lbp_message_catalog.h>
#include <hi_phy_support.h>
#include <hi_g3lib_trace_modules.h>

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_g3lib_drv HIF G3LIB DRV
  * @{
  */

/** @defgroup G3_LIB_Attributes G3_LIB PIB Attributes
  * @brief    G3_LIB PIB Attributes module
  * @details  The Protocol Engine maintains this set of attribute used for its configuration and management.
  * @{
  */

/** @defgroup G3_LIB_Attributes_TypesDefinitions  G3 library PIB Attributes Definitions
  * @brief G3_LIB PIB Attributes types definitions
  * @{
  */

/* Attribute ID definition */

/* G3-LIB Protocol Engine Attributes */
#define G3_LIB_PEFWVERSION_ID           ((uint32_t)0x3200)  /**< @brief G3_LIB Attribute peFWVersion: build release*/
#define G3_LIB_PEPROTOVERSION_ID        ((uint32_t)0x3201)  /**< @brief G3_LIB Attribute peProtocolVersion: store the value of the bandplan used. It can be #G3_LIB_G3PLC_CENA or #G3_LIB_G3PLC_FCC */
#define G3_LIB_RESERVED1_ID             ((uint32_t)0x3202)  /**< @brief G3_LIB Attribute not used */
#define G3_LIB_PEMODE_ID                ((uint32_t)0x3203)  /**< @brief G3_LIB Attribute peMode: define the working mode of the library (#G3_LIB_PEMODE_PHY or #G3_LIB_PEMODE_MAC or G3_LIB_PEMODE_ADP or G3_LIB_PEMODE_BOOT */
#define G3_LIB_PEOPTIONS_ID             ((uint32_t)0x3204)  /**< @brief G3_LIB Attribute peOptions: bitmask to enable or disable options
                                                                        @li Bit 0: enable RTE debuginfo
                                                                        @li Bit 1..31: not used
                                                            */
#define G3_LIB_PECURRENTTIME_ID         ((uint32_t)0x3205)  /**< @brief G3_LIB Attribute peCurrentTime: 64-bits counter since the reset */
#define G3_LIB_PEMEMPOOLMAXALLOCS_ID    ((uint32_t)0x3206)  /**< @brief G3_LIB Attribute peMemPoolMaxAllocs: internal pool allocation */
#define G3_LIB_PEMEMPOOLCURRALLOCS_ID   ((uint32_t)0x3207)  /**< @brief G3_LIB Attribute peMemPoolCurrAllocs: internal pool allocation */
#define G3_LIB_PETOTALSIGNALPOWER_ID    ((uint32_t)0x3208)  /**< @brief G3_LIB Attribute peTotalSignalPower: total signal power at the output of the device characterized using MAC_MAXTXPOWER_ID equal to maximum (0x20) */
#define G3_LIB_PEEVENTINDICATION_ID     ((uint32_t)0x3209)  /**< @brief G3_LIB Attribute peEventIndication: bitmask to enable (set to 1) the notification of a single event. Each bit corresponds to one event as below:
                                                                        @li Bit 0: enable error notification
                                                                        @li Bit 1: enable GMK update notification
                                                                        @li Bit 2: enable the notification of a new context assignement
                                                                        @li Bit 3: enable the notification of a new group id configuration
                                                                        @li Bit 4: enable the notification of the active key change
                                                                        @li Bit 5: enable the notification of the short address change
                                                                        @li Bit 6: enable the notification of the PAN ID change
                                                                        @li Bit 7: enable the notification of the thermal event
                                                                        @li Bit 8: enable the notification of the transmission of a MAC Tone Map Response frame
                                                                        @li Bit 9: enable the notification of the reception of a MAC Tone Map Response frame
                                                                        @li Bit 10: enable the notification of the installation/update of a route in the routing table
                                                                        @li Bit 11: enable the notification of the reception of a PREQ message
                                                                        @li Bit 12: enable the notification of surge event
                                                                        @li Bits [13..63]: not used
                                                            */
#define G3_LIB_PEPOLLRTEINTERVAL_ID     ((uint32_t)0x320A)  /**< @brief G3_LIB Attribute pePePollRTEInterval: interval time in milliseconds to pool RTE */
#define G3_LIB_PEPOLLRTETIMEOUT_ID      ((uint32_t)0x320B)  /**< @brief G3_LIB Attribute pePePollRTETimeout: time in milliseconds to declare a failure in the pool of RTE */
#define G3_LIB_PEPOLLRTEEXPNUM_ID       ((uint32_t)0x320C)  /**< @brief G3_LIB Attribute pePePollRTEExprNum: the number of consecutive time the RTE expiration should occur before doing the reset  */
/* G3-LIB Real-Time Engine Attributes */
#define G3_LIB_RTEFWVERSION_ID          ((uint32_t)0x3100)  /**< @brief The Real-Time Engine Firmware version  */
#define G3_LIB_RTEPROTOVERSION_ID       ((uint32_t)0x3101)  /**< @brief The Real-Time Engine protocol version  */
#define G3_LIB_RTEDRQUEUESIZE_ID        ((uint32_t)0x3102)  /**< @brief The number of Rx Packets that could be handled by the PD-DATA.Request primitive in PHY Only Mode or by the RTMACDATA.Request primitive in MAC Mode  */
#define G3_LIB_RTETXPACKETCOUNT_ID      ((uint32_t)0x3103)  /**< @brief Statistical counter of transmitted Packet by the Real Time Engine. This could be the number of transmitted frames by the PHY Layer or the number of transmitted segment by the Real Time part of the MAC  */
#define G3_LIB_RTERXPACKETCOUNT_ID      ((uint32_t)0x3104)  /**< @brief Statistical counter of correct received Packet by the Real Time Engine. This could be the number of received frames at PHY Layer or the number of received segment by the Real Time part of the MAC  */
#define G3_LIB_RTEMODE_ID               ((uint32_t)0x3105)  /**< @brief G3_LIB Working Mode of RTE (#G3_LIB_RTEMODE_MAC or #G3_LIB_RTEMODE_PHY)  */
#define G3_LIB_RTECAPABILITIES_ID       ((uint32_t)0x3106)  /**< @brief Bit mask (b31b30�b2b1b0) indicating which features are implemented in released sw (internally used) */
#define G3_LIB_RTEVALIDATIONTEST_ID     ((uint32_t)0x3107)  /**< @brief Indication if PHY validation test is in progress */
#define G3_LIB_RTEMAXNUMSEGFRAME_ID     ((uint32_t)0x3108)  /**< @brief Max length of segmented frame managed by rte that require acknowledge */
#define G3_LIB_RTEGPIO_ID               ((uint32_t)0x3109)  /**< @brief Field mask to control the GPIO exported events from the RTE, where
                                                                        Bit [0..2]:   GPIO_TX_SEL_GROUP (0: TX event output disabled, 1: GPIO5, 2: GPIO6, 3: GPIO7, 4: GPIO8, 5: GPIO9, 6: GPIO10, 7: Reserved)
                                                                        Bit [3..5]:   GPIO_TX_SEL_PIN (Selects the GPIO pin where to direct the output of TX event. Has no effect if GPIO_TX_SEL_GROUP is 0)
                                                                        Bit [6..7]:   Reserved
                                                                        Bit [8..10]:  GPIO_RX_SEL_GROUP (0: RX event output disabled, 1: GPIO5, 2: GPIO6, 3: GPIO7, 4: GPIO8, 5: GPIO9, 6: GPIO10, 7: Reserved)
                                                                        Bit [11..13]: GPIO_RX_SEL_PIN (Selects the GPIO pin where to direct the output of RX event. Has no effect if GPIO_RX_SEL_GROUP is 0)
                                                                        Bit [14..31]: Reserved */

/* Protocol Engine Operation Mode */
#define G3_LIB_PEMODE_PHY            ((uint8_t)0x00)        /**< @brief PE PHY working mode  */
#define G3_LIB_PEMODE_MAC            ((uint8_t)0x01)        /**< @brief PE MAC working mode  */
#define G3_LIB_PEMODE_6LoWPAN        ((uint8_t)0x02)        /**< @brief PE 6LoWPAN working mode  */
#define G3_LIB_PEMODE_BOOT           ((uint8_t)0x03)        /**< @brief PE Bootstrap Handler Only working mode  */
#define G3_LIB_PEMODE_DEFAULT        G3_LIB_PEMODE_BOOT     /**< @brief Default PE mode */

/**< @brief Real-Time Engine Operation Mode */
#define G3_LIB_RTEMODE_PHY           ((uint8_t)0x01)  /**< @brief RTE PHY working mode (internally used) */
#define G3_LIB_RTEMODE_MAC           ((uint8_t)0x00)  /**< @brief RTE MAC working mode (internally used) */

/**< @brief Protocol Versions, CENELEC-A, CENELEC-B (CEN) or ARIB, FCC (FCC) */
#define G3_LIB_G3PLC_CENA            ((uint8_t)0x00)  /**< @brief Library configuration value for ITU-T G.9903 - Cenelec A bandplan */
#define G3_LIB_G3PLC_CENB            ((uint8_t)0x01)  /**< @brief Library configuration value for ITU-T G.9903 - Cenelec B bandplan */
#define G3_LIB_G3PLC_ARIB            ((uint8_t)0x02)  /**< @brief Library configuration value for ITU-T G.9903 - ARIB bandplan */
#define G3_LIB_G3PLC_FCC             ((uint8_t)0x03)  /**< @brief Library configuration value for ITU-T G.9903 - FCC bandplan */
#define G3_LIB_IEEE_1901_2_FCC       ((uint8_t)0x04)  /**< @brief Library configuration value for IEEE Std 1901.2 - FCC above CENELEC bandplan */

/**< @brief Event Notification Indication bitmask */
#define G3_LIB_PEEVENTINDICATION_ERROR         ((uint64_t)0x0000000000000001)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_GMK           ((uint64_t)0x0000000000000002)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_CONTEXT       ((uint64_t)0x0000000000000004)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_GROUP         ((uint64_t)0x0000000000000008)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_ACTKEY        ((uint64_t)0x0000000000000010)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_PANID         ((uint64_t)0x0000000000000020)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_SHORTADDR     ((uint64_t)0x0000000000000040)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_THERMEV       ((uint64_t)0x0000000000000080)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_TMRTX         ((uint64_t)0x0000000000000100)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_TMRRX         ((uint64_t)0x0000000000000200)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_ROUTE         ((uint64_t)0x0000000000000400)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_PREQ          ((uint16_t)0x0000000000000800)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_SURGEEV       ((uint64_t)0x0000000000001000)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_RTEWARN       ((uint64_t)0x0000000000002000)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_RTEPOLL       ((uint64_t)0x0000000000004000)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_BOOTMAXRETIES ((uint64_t)0x0000000000008000)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_BANDMISMATCH  ((uint64_t)0x0000000000010000)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_PHYQUALITY_RX ((uint64_t)0x0000000000020000)  /**< @brief Bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_PHYQUALITY_TX ((uint64_t)0x0000000000040000)  /**< @brief Bitmask definition for the event notification */

/** @brief Deafault value for the bitmask definition for the event notification */
#define G3_LIB_PEEVENTINDICATION_DEFAULT   (G3_LIB_PEEVENTINDICATION_ERROR         | \
                                            G3_LIB_PEEVENTINDICATION_ACTKEY        | \
                                            G3_LIB_PEEVENTINDICATION_PANID         | \
                                            G3_LIB_PEEVENTINDICATION_SHORTADDR     | \
                                            G3_LIB_PEEVENTINDICATION_GMK           | \
                                            G3_LIB_PEEVENTINDICATION_PREQ          | \
                                            G3_LIB_PEEVENTINDICATION_SURGEEV       | \
                                            G3_LIB_PEEVENTINDICATION_RTEWARN       | \
                                            G3_LIB_PEEVENTINDICATION_RTEPOLL       | \
                                            G3_LIB_PEEVENTINDICATION_BOOTMAXRETIES | \
                                            G3_LIB_PEEVENTINDICATION_BANDMISMATCH)


#define G3_LIB_PEOPTIONS_RTEDEBUG    ((uint32_t)0x00000001)  /**< @brief Bitmask definition for the PEOptions */
#define G3_LIB_PEOPTIONS_DEFAULT     ((uint32_t)0x00000000)  /**< @brief Deafult value for PEOptions */

/**< @brief Bitmask definitions for RTE GPIO TX/RX  */
#define G3_LIB_RTEGPIO_TX_SEL_GROUP  ((uint32_t)0x00000007) /**< @brief mask for GPIO_TX_SEL_GROUP bitfield */
#define G3_LIB_RTEGPIO_TX_SEL_PIN    ((uint32_t)0x00000038) /**< @brief mask for GPIO_TX_SEL_PIN bitfield */
#define G3_LIB_RTEGPIO_RX_SEL_GROUP  ((uint32_t)0x00000700) /**< @brief mask for GPIO_RX_SEL_GROUP bitfield */
#define G3_LIB_RTEGPIO_RX_SEL_PIN    ((uint32_t)0x00003800) /**< @brief mask for GPIO_RX_SEL_PIN bitfield */

#pragma pack(1)

/**
  * @brief  Exported Attribute structure that could be used directly by the application with a single function call
  *         These attributes need to be saved time by time and restored after a reset/power-on in order to comply with G3-PLC security mechanism
  */
typedef struct
{
    uint16_t adpLOADngSeqNum;             /**< @brief The adpLOADngSeqNum attribute  */
    uint32_t macFrameCounter;             /**< @brief The macFrameCounter attribute  */

} G3LIB_ExportedAttribute_t;

/**
 * @brief  MAC POS Table Structure
 */
typedef struct
{
    uint16_t ShortAddr;    /**< @brief Short Address of the device */
    uint8_t  LQI;             /**< @brief The forward Link Quality Indicator (LQI): the LQI of the last Rx packet */
    uint8_t  POSValidTime;    /**< @brief Remaining time in minutes until when this entry is considered valid. */
    uint8_t MediaType;     /**< @brief The channel to be used to transmit to the next hop */
} MAC_POSTableEntry_t;

/**
 * @brief  MAC Neighbor Table Structure
 */
typedef struct
{
    uint16_t         ShortAddr;                 /**< @brief Short Address of the device */
    uint8_t          Mod                : 3;    /**< @brief Requested Modulation Type (0: Robust mode, 1: DBPSK or BPSK, 2: DQPSK or QPSK, 3: D8PSK or 8-PSK, 4: 16-QAM, 5-7: Reserved by ITU-T)  */
    uint8_t          TxGain             : 4;    /**< @brief Desired transmitter gain specifying how many gain steps are requested */
    uint8_t          TxRes              : 1;    /**< @brief Tx Gain resolution corresponding to one gain step (0: 6 dB, 1: 3 dB) */
    uint8_t          ToneMap[PHY_TONEMAP_SIZE]; /**< @brief Tone map */
    uint8_t          ReverseLQI;                /**< @brief The reverse Link Quality Indicator (LQI) */
    PHY_FCCTXCoeff_t TXCoeff;                   /**< @brief Specifies the number of gain steps requested for the tones represented by Tone Map */
    uint8_t          PhaseDifferential  : 3;    /**< @brief The phase difference in multiples of 60 degrees of the last received frame */
    uint8_t          PayModSch          : 1;    /**< @brief Payload Modulation Scheme (0 Differential, 1 Coherent) */
    uint8_t          Rsv                : 4;    /**< @brief Reserved and not used */
    uint8_t          TMRValidTime;              /**< @brief TimeToLive of the TMR information validity in minutes (0 = information is no more valid) */
} MAC_NeighborTableEntry_t;


/**
 * @brief  MAC Neighbor Table Structure as in G3 Standard April 2015
 */
typedef struct
{
    uint16_t         ShortAddr;                 /**< @brief Short Address of the device */
    uint8_t          Mod                : 3;    /**< @brief Requested Modulation Type (0: Robust mode, 1: DBPSK or BPSK, 2: DQPSK or QPSK, 3: D8PSK or 8-PSK, 4: 16-QAM, 5-7: Reserved by ITU-T)  */
    uint8_t          TxGain             : 4;    /**< @brief Desired transmitter gain specifying how many gain steps are requested */
    uint8_t          TxRes              : 1;    /**< @brief Tx Gain resolution corresponding to one gain step (0: 6 dB, 1: 3 dB) */
    uint8_t          ToneMap[PHY_TONEMAP_SIZE]; /**< @brief Tone map */
    uint8_t          ReverseLQI;                /**< @brief The reverse Link Quality Indicator (LQI) */
    PHY_FCCTXCoeff_t TXCoeff;                   /**< @brief Specifies the number of gain steps requested for the tones represented by Tone Map */
    uint8_t          PhaseDifferential  : 3;    /**< @brief The phase difference in multiples of 60 degrees of the last received frame */
    uint8_t          PayModSch          : 1;    /**< @brief Payload Modulation Scheme (0 Differential, 1 Coherent) */
    uint8_t          Rsv                : 4;    /**< @brief Reserved and not used */
    uint8_t          TMRValidTime;              /**< @brief TimeToLive of the TMR information validity in minutes (0 = information is no more valid) */
    uint8_t          EntryValidTime;    /**< @brief TimeToLive of the entry (0 = entry is no more valid) */
} MAC_NeighborTableEntry2015_t;

/**
 * @brief  MAC Device Table Structure
 */
typedef struct
{
    uint16_t PanID;              /**< @brief PAN ID of the device */
    uint16_t ShortAddress;       /**< @brief Short Address of the device */
    uint32_t LastRcvFrameCnt;    /**< @brief Last Received FrameCounter from the device */
} MAC_DeviceTableEntry_t;

/**
  * @brief  ADP Routing Table Entry
  */
typedef struct
{
    uint16_t DstAddr;        /**< @brief The Final Destination Address */
    uint16_t NextHop;        /**< @brief The Next Hop */
    uint16_t RouteCost;      /**< @brief The cumulative Route Cost towards the destination */
    uint8_t  HopCountWeakLink;  /**< @brief The Hop Count for first 4 bits  and The Weak Link to the destination for end 4 bits*/
    uint16_t ValidTime;      /**< @brief The remaining valid time in minutes for this entry (0 = entry is no more valid)  */
    uint8_t  MediaType;      /**< @brief The MediaType */
} ADP_RoutingTableEntry_t;

/**
  * @brief  Used only to get the max length of an attribute
  */
typedef union
{
    uint8_t                          ADP_EAP_Psk[ADP_EAP_PSK_KEY_LEN];
    uint8_t                          ADP_EAP_IdP[ADP_EAP_PSK_ID_MAX_LEN];
    ADP_ContextInformationEntry_t    ContextInfoEntry;
    ADP_RoutingTableEntry_t          RouteEntry;
    ADP_BroadcastTableEntry_t        BroadcastEntry;
    ADP_BlackListedNeighTableEntry_t BLNeighEntry;
    ADP_GroupTableEntry_t            ADP_GroupTable;
    ADP_PrefixTableEntry_t           ADP_PrefixTable;
    MAC_KeyTable_t                   MAC_KeyTable;
    MAC_POSTableEntry_t              MAC_POSTable;
    MAC_NeighborTableEntry_t         MAC_NeighTable;
    MAC_DeviceTableEntry_t           MAC_DeviceTable;
    uint8_t                          PHY_ToneMask[PHY_TONEMASK_SIZE];
    uint8_t                          PHY_StaticPreempahsisSize[PHY_NUM_OF_CARRIERS];
    uint8_t                          PHY_NoiseMaxSize[PHY_NUM_OF_CARRIERS*4];
} G3_LIB_PIBTypesStructsType;

/**< @brief Maximum size of the value for one ADP Attribute */
#define G3_LIB_MAX_ATTR_LEN  ((uint16_t)sizeof(G3_LIB_PIBTypesStructsType)) /**< @brief Maximum dimension of one value in the PIB  */

/**
  * @brief  Structure of the Attribute information
  */
typedef struct
{
    uint32_t ID;    /**< @brief The ID of the attribute                      */
    uint16_t Index; /**< @brief The entry index of the attribute (if table)  */
} G3_LIB_PIB_ID_t;

/**
  * @brief  Structure of the Attribute information with the value
  */
typedef struct
{
    G3_LIB_PIB_ID_t  Attribute;                  /**< @brief The ID+Indexof the attribute  */
    uint16_t         Len;                        /**< @brief The length of the value       */
    uint8_t          Value[G3_LIB_MAX_ATTR_LEN]; /**< @brief The value of the attibute     */
} G3_LIB_PIB_t;

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

#endif /*HI_G3LIB_ATTRIBUTES_H*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
