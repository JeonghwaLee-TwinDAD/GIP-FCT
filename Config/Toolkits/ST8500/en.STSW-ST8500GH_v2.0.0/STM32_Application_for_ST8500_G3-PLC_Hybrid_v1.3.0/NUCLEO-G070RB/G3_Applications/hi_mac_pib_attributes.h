/**
  ******************************************************************************
  * @file    hi_mac_pib_attributes.h
  * @author  AMG/IPC Application Team
  * @brief   Header file with the definitions related to the MAC PIB Attributes
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

/* Define to prevent recursive inclusion */
#ifndef HI_MAC_PIB_ATTRIBUTES_H
#define HI_MAC_PIB_ATTRIBUTES_H

#include "hi_mac_pib_types.h"

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_mac_drv HIF MAC DRV
  * @{
  */

/** @defgroup MAC_PIB_Attributes MAC PIB Attributes
  * @brief    MAC PIB Attributes module
  * @details  This module contains the definition/handling of the MAC PIB Attributes, which are defined by the G3 and IEEE 802.15.4 standards. The module contains also implementation defined attributes
  * @{
  */

/** @defgroup MAC_PIB_Attributes_TypesDefinitions HIF MAC PIB Types Definitions
  * @brief    MAC PIB Attributes types definitions
  * @{
  */
/* Standard IEEE 802.15.4 PIB attribute ID definitions */
#define MAC_ACKWAITDURATION_ID          ((uint32_t)0x0040) /**< @brief macAckWaitDuration Attribute ID */
#define MAC_MAXBE_ID                    ((uint32_t)0x0047) /**< @brief macMaxBE Attribute ID */
#define MAC_BSN_ID                      ((uint32_t)0x0049) /**< @brief macBSN Attribute ID */
#define MAC_DSN_ID                      ((uint32_t)0x004C) /**< @brief macDSN Attribute ID */
#define MAC_MAXCSMABACKOFFS_ID          ((uint32_t)0x004E) /**< @brief macMaxCSMABackoffs Attribute ID */
#define MAC_MINBE_ID                    ((uint32_t)0x004F) /**< @brief macMinBE Attribute ID */
#define MAC_PANID_ID                    ((uint32_t)0x0050) /**< @brief macPanId Attribute ID */
#define MAC_PROMISCUOUSMODE_ID          ((uint32_t)0x0051) /**< @brief macPromiscuousMode Attribute ID */
#define MAC_SHORTADDRESS_ID             ((uint32_t)0x0053) /**< @brief macShortAddress Attribute ID */
#define MAC_MAXFRAMERETRIES_ID          ((uint32_t)0x0059) /**< @brief macMaxFrameRetries Attribute ID */
#define MAC_TIMESTAMPSUPP_ID            ((uint32_t)0x005C) /**< @brief macTimeStampSupport Attribute ID */
#define MAC_SECURITYENABLED_ID          ((uint32_t)0x005D) /**< @brief macSecurityEnabled Attribute ID */
#define MAC_KEYTABLE_ID                 ((uint32_t)0x0071) /**< @brief macKeyTable Attribute ID */
#define MAC_FRAMECOUNTER_ID             ((uint32_t)0x0077) /**< @brief macFrameCounter Attribute ID */
/* Standard G.9903 PIB attribute ID definitions */
#define MAC_HIGHPRIORITYWS_ID           ((uint32_t)0x0100) /**< @brief macHighPriorityWindowSize Attribute ID */
#define MAC_TXDATAPACKETCOUNT_ID        ((uint32_t)0x0101) /**< @brief macTxDataPacketCount Attribute ID */
#define MAC_RXDATAPACKETCOUNT_ID        ((uint32_t)0x0102) /**< @brief macRxDataPacketCount Attribute ID */
#define MAC_TXCMDPACKETCOUNT_ID         ((uint32_t)0x0103) /**< @brief macTxCmdPacketCount Attribute ID */
#define MAC_RXCMDPACKETCOUNT_ID         ((uint32_t)0x0104) /**< @brief macRxCmdPacketCount Attribute ID */
#define MAC_CSMAFAILCOUNT_ID            ((uint32_t)0x0105) /**< @brief macCSMAFailCount Attribute ID */
#define MAC_CSMANOACKCOUNT_ID           ((uint32_t)0x0106) /**< @brief macCSMAnoACKCount Attribute ID */
#define MAC_RXDATABROADCASTCOUNT_ID     ((uint32_t)0x0107) /**< @brief macRxDataBroadcastCount Attribute ID */
#define MAC_TXDATABROADCASTCOUNT_ID     ((uint32_t)0x0108) /**< @brief macTxDataBroadcastCount Attribute ID */
#define MAC_BADCRCCOUNT_ID              ((uint32_t)0x0109) /**< @brief macBadCRCCount Attribute ID */
#define MAC_NEIGHBORTABLE_ID            ((uint32_t)0x010A) /**< @brief macNeighborTable Attribute ID */
#define MAC_RESERVED1_ID                ((uint32_t)0x010B) /**< @brief not used */
#define MAC_CSMAFAIRNESSLIMIT_ID        ((uint32_t)0x010C) /**< @brief macCSMAFairnessLimit Attribute ID */
#define MAC_TMRTTL_ID                   ((uint32_t)0x010D) /**< @brief macTMRTTL Attribute ID */
#define MAC_POSTABLEENTRYTTL_ID         ((uint32_t)0x010E) /**< @brief macPOSTableEntryTTL Attribute ID */
#define MAC_RCCOORD_ID                  ((uint32_t)0x010F) /**< @brief macRCCoord Attribute ID */
#define MAC_TONEMASK_ID                 ((uint32_t)0x0110) /**< @brief macToneMask Attribute ID */
#define MAC_BCNRANDWINLEN_ID            ((uint32_t)0x0111) /**< @brief macBeaconRandomizationWindowLengh Attribute ID */
#define MAC_A_ID                        ((uint32_t)0x0112) /**< @brief macA Attribute ID */
#define MAC_K_ID                        ((uint32_t)0x0113) /**< @brief macK Attribute ID */
#define MAC_MINCWATTEMPTS_ID            ((uint32_t)0x0114) /**< @brief macMinCWAttempts Attribute ID */
#define MAC_CENELECLEGACYMODE_ID        ((uint32_t)0x0115) /**< @brief macCENELECLegacyMode Attribute ID */
#define MAC_FCCLEGACYMODE_ID            ((uint32_t)0x0116) /**< @brief macFCCLegacyMode Attribute ID */
#define MAC_BROADCASTMAXCWENABLE_ID     ((uint32_t)0x011E) /**< @brief macBroadcastMaxCWEnable If enabled (set to 1) MAC uses maximum contention window for transmitting normal priority broadcast packets */
#define MAC_TRANSMITATTEN_ID            ((uint16_t)0x011F) /**< @brief macTransmitAtten Attribute ID */
#define MAC_POSTABLE_ID                 ((uint32_t)0x0120) /**< @brief macPOSTable Attribute ID */
/* Implementation defined PIB attribute ID definitions */
#define MAC_EXTENDEDADDRESS_ID          ((uint32_t)0x3000) /**< @brief aExtendedAddress Attribute ID: EUI-64 address to be used */
#define MAC_RESERVED3_ID                ((uint32_t)0x3001) /**< @brief not used */
#define MAC_TRXSTATENOTCORRECT_ID       ((uint32_t)0x3002) /**< @brief macTrxStateNotCorrect Attribute ID: Counter of discarded frame for RTE state not correct */
#define MAC_PSDUTOLONG_ID               ((uint32_t)0x3003) /**< @brief macPsduToLong Attribute ID: Counter of discarded frame for PDU too long */
#define MAC_INVALIDFRAMETYPE_ID         ((uint32_t)0x3004) /**< @brief macInvalidFrameType Attribute ID: Counter of discarded frame for Invalid Frame Type field  */
#define MAC_INVALIDFRAMEVERSION_ID      ((uint32_t)0x3005) /**< @brief macInvalidFrameVersion Attribute ID: Counter of discarded frame for invalid Frame Version field  */
#define MAC_INVALIDDAM_ID               ((uint32_t)0x3006) /**< @brief macInvalidDAM Attribute ID: Counter of discarded frame for invalid Destination Address Mode  */
#define MAC_INVALIDSAM_ID               ((uint32_t)0x3007) /**< @brief macInvalidSAM Attribute ID: Counter of discarded frame for invalid Source Address Mode  */
#define MAC_INVALIDPANID_ID             ((uint32_t)0x3008) /**< @brief macInvalidPanID Attribute ID: Counter of discarded frame for invalid PanID  */
#define MAC_INVALIDSHORTADDR_ID         ((uint32_t)0x3009) /**< @brief macInvalidShortAddr Attribute ID: Counter of discarded frame for invalid Short Address  */
#define MAC_INVALIDEXTADDR_ID           ((uint32_t)0x300A) /**< @brief macInvalidExtAddr Attribute ID: Counter of discarded frame for invalid Extended Address  */
#define MAC_INVALIDBEACON_ID            ((uint32_t)0x300B) /**< @brief macInvalidBeacon Attribute ID: Counter of discarded frame for invalid Beacon  */
#define MAC_INVALIDDATACMD_ID           ((uint32_t)0x300C) /**< @brief macInvalidDataCmd Attribute ID: Counter of discarded frame for invalid Command  */
#define MAC_INTERNALSTATENOTCORRECT_ID  ((uint32_t)0x300D) /**< @brief macInternalStateNotCorrect Attribute ID: Counter of discarded frame for invalid state of MAC at data indication */
#define MAC_FRAMEDISCARDED_ID           ((uint32_t)0x300E) /**< @brief macFrameDiscarded Attribute ID: Counter of discarded frame for any of the above reason */
#define MAC_ACKTRXSTATENOTCORRECT_ID    ((uint32_t)0x300F) /**< @brief macAckTrxStateNotCorrect attribute ID: Counter of discarded ACK frames for PHY State not correct  */
#define MAC_ACKNOTCORRECT_ID            ((uint32_t)0x3010) /**< @brief macAckNotCorrect attribute ID: Counter of discarded ACK frames for wrong FCH received */
#define MAC_ACKINTSTATENOTCORRECT_ID    ((uint32_t)0x3011) /**< @brief macAckIntStateNotCorrect attribute ID: Counter of discarded ACK frames for invalid state of the MAC */
#define MAC_ACKCRCNOTEXPECTED_ID        ((uint32_t)0x3012) /**< @brief macAckCrcNotExpected attribute ID: Counter of discarded ACK frames for invalid CRC expected */
#define MAC_ACKDISCARDED_ID             ((uint32_t)0x3013) /**< @brief macAckDiscarded attribute ID: Counter of total discarded ACK frames */
#define MAC_CSMASEED_ID                 ((uint32_t)0x3014) /**< @brief macCSMASeed attribute ID: Seed value in function that generates random numbers */
#define MAC_INVALIDFRAMEPENDING_ID      ((uint32_t)0x3015) /**< @brief macInvalidFramePending attribute ID: Counter of discarded frame for invalid Frame Pending field detected */
#define MAC_INVALIDPIC_ID               ((uint32_t)0x3016) /**< @brief macInvalidPIC attribute ID: Counter of discarded frame for invalid PIC field detected */
#define MAC_PSDUTOOSHORT_ID             ((uint32_t)0x3017) /**< @brief macPsduTooShort Attribute ID: Counter of discarded frame for PDU too short */
#define MAC_CNTFIXDT_ID                 ((uint32_t)0x3018) /**< @brief macCntFixDT Attribute ID: This attribute (b31 b30 ... b2 b1 b0) contain two fields:
                                                                      b28-0:  represent a counter indicating the number of time the DT indicated in bits b31-29 must be inserted in the FCH composed by rtMAC and passed to phy
                                                                      b31-29: represent a DT value that must be used by rtMAC to compose an FCH passed to PHY */
#define MAC_DISABLEPLC_ID               ((uint32_t)0x3019) /**< @brief macDisablePLC Attribute ID: This attribute indicates if modem is enabled (0) or disabled(1) when modem is disabled all counter managed by MAC are frozen, counters managed by phy are not frozen */
#define MAC_STARTLOWERINGRETRIES_ID     ((uint32_t)0x301A) /**< @brief macStartLoweringRetries Attribute ID: If this attribute has value different from 0 indicate in which retransmission start lowering modulation mechanism, value 0 indicate that lowering modulation mechanism is disabled. */
#define MAC_TIMERWABEND_ID              ((uint32_t)0x301B) /**< @brief macTimerWABEnd Attribute ID: Counter of number of times after channel sensed busy was received nothing from phy */
#define MAC_DONTSENDNACK_ID             ((uint32_t)0x301C) /**< @brief macDontSendNACK Attribute ID: Never send NACK frame */
#define MAC_SNRDBROBO_ID                ((uint32_t)0x3280) /**< @brief macSNRdBROBO Attribute ID: tone map algorithm thresold for ROBO */
#define MAC_SNRDBDBPSK_ID               ((uint32_t)0x3281) /**< @brief macSNRdBDBPSK Attribute ID: tone map algorithm thresold for xBPSK */
#define MAC_SNRDBDQPSK_ID               ((uint32_t)0x3282) /**< @brief macSNRdBDQPSK Attribute ID: tone map algorithm thresold for xQPSK */
#define MAC_SNRDBD8PSK_ID               ((uint32_t)0x3283) /**< @brief macSNRdBD8PSK Attribute ID: tone map algorithm thresold for x8PSK */
#define MAC_RXBCNLASTSCAN_ID            ((uint32_t)0x3284) /**< @brief macRXBeaconReceivedFromLastScan: Number of received beacon from the start of the scan */
#define MAC_MODULATIONCONTROL_ID        ((uint32_t)0x3285) /**< @brief macModulationControl: Field mask to control the modulation used by the MAC Layer where
                                                                      Bit [0..3]: defines the modulation to be used for the data transmission (0: ROBO, 1: DBPSK or BPSK, 2: DQPSK or QPSK, 3: D8PSK or 8PSK, F use neighbor table)
                                                                      Bit [4..7]: defines the modulation scheme to be asked in the tone map response (0: ROBO, 1: DBPSK or BPSK, 2: DQPSK or QPSK, 3: D8PSK or 8PSK, F use the internal algorithm)
                                                                      Bit 8: if 1 force a scheme selection in the tone map response. Scheme is selected through bit 9
                                                                      Bit 9: if bit 8 indicate to foce the scheme this bit indicates 0 for differential 1 for coherent
                                                                      Bit [10..11]: defines the modulation to be used for the broadcast data transmission (0: ROBO, 1: DBPSK or BPSK, 2: DQPSK or QPSK, 3: D8PSK or 8PSK)
                                                                      Bit 12: defines the modulation scheme to be used for broadcast transmission (0: differential, 1: coherent)
                                                                      Bit 13: if set to 1 force the scheme specified in Bit 14
                                                                      Bit 14: if set to 0 force differential, if set to 1 force coherent for unicast data transmission
                                                                      Bit [15..31]: not used */
#define MAC_OPTIONALFEATURES_ID         ((uint32_t)0x3286) /**< @brief macOptionalFeatures: Field mask to control optional features provided by the MAC Layer where
                                                                      Bit 0: if 1 it enables the TMR bit in the tone map response
                                                                      Bit 1: send the ToneMap Response message cyphered
                                                                      Bit 2: set the TMR bit also if the source address is 64bit
                                                                      Bit 3: set the ToneMap Response priority (0: Normal priority, 1: High priority)
                                                                      Bit 4: export the MAC frames with both 64 bit src and dst addresses through the MCPS-DATA-EXTADDR.ind
                                                                      Bit 5: ignore beacon request frames and do not reply to them
                                                                      Bit 6: do not send ToneMapResponse message
                                                                      Bit 7: do not invoke MLME-COMM-STATUS.Indication for PAN_ID_CONFILCT reason
                                                                      Bit 8: do not invoke MLME-COMM-STATUS.Indication for security reasons (COUNTER_ERROR, SECURITY_ERROR, UNAVAILABLE_KEY, UNSUPPORTED_SECURITY, IMPROPER_KEY_TYPE)
                                                                      Bit 9: do not invoke MLME-COMM-STATUS.Indication for other reasons */
#define MAC_TMRMMODULATIONCONTROL_ID    ((uint32_t)0x3287) /**< @brief macTMRModulationControl: Field mask to control the modulation used by the MAC Layer during data transmission when TMR bit is set
                                                                      Bit [0..2]: defines the modulation to be used to the first data tranmission (0: ROBO, 1: DBPSK or BPSK, 2: DQPSK or QPSK, 3: D8PSK or 8PSK)
                                                                      Bit 3: defines the modulation scheme to be used to the first data tranmission (when TMR bit is set) (0: differential, 1: coherent)
                                                                      Bit [4..6]: defines the number of negative step with respect to maximum tx power (only negative values of TxGain)
                                                                      Bit 7: defines the TxRes(0: 3 dB, 1: 6 dB)
                                                                      Bit [8..31]: defines the tone map field */
#define MAC_TMRMINNUMCARRIER_ID         ((uint32_t)0x3288) /**< @brief macTMRMinNumCarrier: minimum number of carrier that the must be used when selecting a modulation in the tone map response algorithm */
#define MAC_SNRDBSUPERROBO_ID           ((uint32_t)0x3289) /**< @brief macSNRdBSuperROBO Attribute ID: tone map algorithm thresold for SuperROBO */
#define MAC_MAXTXPOWER_ID               ((uint32_t)0x328A) /**< @brief macMaxTxPower Attribute ID: default value for the maxium power */
#define MAC_DATATXTIMEOUT_ID            ((uint32_t)0x328B) /**< @brief macDataTxTimeout Attribute ID: timeout for a MCPS-DATA.confirm generation (seconds) */
#define MAC_DEVICETABLE_ID              ((uint32_t)0x328C) /**< @brief macDeviceTable Attribute ID: this is used to get the MAC device table entry by entry */
#define MAC_NEIGHBORTABLE_BYSHORT_ID    ((uint32_t)0x328D) /**< @brief macNeighborTable Attribute ID, index is used as short address */
#define MAC_NEIGHBORTABLE_VALIDENTRY_ID ((uint32_t)0x328E) /**< @brief macNeighborTable Attribute ID but the entry returned is the next one valid */
#define MAC_SNRDB_USEDIFF8PSK_ID        ((uint32_t)0x328F) /**< @brief macSNRdBUseDiff8PSK Attribute ID: indicates the number of dB with respect to the macSNRdBD8PSK to force the switch to differential */
#define MAC_POSTABLE_VALIDENTRY_ID      ((uint32_t)0x3290) /**< @brief macPOSTable Attribute ID, but the entry returned is the next one valid */
#define MAC_NEIGHBORTABLE2015_ID        ((uint32_t)0x3291) /**< @brief macNeighborTable Attribute ID returned in 2015 format */
#define MAC_NEIGHBORTABLE2015_BYSHORT_ID    ((uint32_t)0x3292) /**< @brief macNeighborTable Attribute ID returned in 2015 format, index is used as short address */
#define MAC_NEIGHBORTABLE2015_VALIDENTRY_ID ((uint32_t)0x3293) /**< @brief macNeighborTable Attribute ID returned in 2015 format but the entry returned is the next one valid */
#define MAC_IMPULSIVENOISELOWERING_ID   ((uint32_t)0x3294) /**< @brief macImpulsiveNoiseLowering Attribute ID that specify how many modulation step we have to lower when impuslive noise is present */
#define MAC_DELAYTXAFTERTMR_ID          ((uint32_t)0x3295) /**< @brief macDelayTxAfterTMR Attribute ID that specifies how the maxiumum time (in ms) the node has to wait before sending a next MAC frame after the TMR bit was set in a previous transmission */

/* Bitmask definitions */
#define MAC_MODULATIONCONTROL_TXMOD                 ((uint32_t)0x0000000F) /**< @brief mask for macModulationControl parameter */
#define MAC_MODULATIONCONTROL_TMMOD                 ((uint32_t)0x000000F0) /**< @brief mask for macModulationControl parameter */
#define MAC_MODULATIONCONTROL_FORCETMSCHEME         ((uint32_t)0x00000100) /**< @brief mask for macModulationControl parameter */
#define MAC_MODULATIONCONTROL_TMSCHEME              ((uint32_t)0x00000200) /**< @brief mask for macModulationControl parameter */
#define MAC_MODULATIONCONTROL_BRMOD                 ((uint32_t)0x00000C00) /**< @brief mask for macModulationControl parameter */
#define MAC_MODULATIONCONTROL_BRSCHEME              ((uint32_t)0x00001000) /**< @brief mask for macModulationControl parameter */
#define MAC_MODULATIONCONTROL_FORCEUNISCHEME        ((uint32_t)0x00002000) /**< @brief mask for macModulationControl parameter */
#define MAC_MODULATIONCONTROL_UNISCHEME_COHE        ((uint32_t)0x00004000) /**< @brief mask for macModulationControl parameter */
#define MAC_MODULATIONCONTROL_MOD_DISABLE           ((uint32_t)0x0000000F) /**< @brief mask for macModulationControl parameter */

#define MAC_OPTIONALFEATURES_SETTMR                 ((uint32_t)0x00000001) /**< @brief mask for macOptionalFeatures parameter */
#define MAC_OPTIONALFEATURES_TMRESPENCR             ((uint32_t)0x00000002) /**< @brief mask for macOptionalFeatures parameter */
#define MAC_OPTIONALFEATURES_TMREXTADDR             ((uint32_t)0x00000004) /**< @brief mask for macOptionalFeatures parameter */
#define MAC_OPTIONALFEATURES_TMRESPPRIO             ((uint32_t)0x00000008) /**< @brief mask for macOptionalFeatures parameter */
#define MAC_OPTIONALFEATURES_EXPEXTADDRS            ((uint32_t)0x00000010) /**< @brief mask for macOptionalFeatures parameter */
#define MAC_OPTIONALFEATURES_IGNOREBCNREQ           ((uint32_t)0x00000020) /**< @brief mask for macOptionalFeatures parameter */
#define MAC_OPTIONALFEATURES_STOPTMR                ((uint32_t)0x00000040) /**< @brief mask for macOptionalFeatures parameter */
#define MAC_OPTIONALFEATURES_NOCOMMIND_PANCONFLICT  ((uint32_t)0x00000080) /**< @brief mask for macOptionalFeatures parameter */
#define MAC_OPTIONALFEATURES_NOCOMMIND_SECURITYERR  ((uint32_t)0x00000100) /**< @brief mask for macOptionalFeatures parameter */
#define MAC_OPTIONALFEATURES_NOCOMMIND_OTHER        ((uint32_t)0x00000200) /**< @brief mask for macOptionalFeatures parameter */

#define MAC_TMRMODULATIONCONTROL_MOD                ((uint32_t)0x00000007) /**< @brief mask for macTMRModulationControl parameter */
#define MAC_TMRMODULATIONCONTROL_SCHEME             ((uint32_t)0x00000008) /**< @brief mask for macTMRModulationControl parameter */
#define MAC_TMRMODULATIONCONTROL_TXGAIN             ((uint32_t)0x00000070) /**< @brief mask for macTMRModulationControl parameter */
#define MAC_TMRMODULATIONCONTROL_TXRESP             ((uint32_t)0x00000080) /**< @brief mask for macTMRModulationControl parameter */

#define MAC_TONEMASK_LEN                (PHY_TONEMASK_SIZE) /**< @brief The size of the macToneMask parameter in Bytes */

#define MAC_RCCOORD_COORD_DEF       ((uint16_t)0x0000)  /**< @brief The PAN coordinator shall set its RC_COORD to 0x0000  */
#define MAC_RCCOORD_DEV_DEF         ((uint16_t)0xFFFF)  /**< @brief The device shall set its RC_COORD to 0xFFFF by default  */
#define MAC_RCCOORD_DEV_NOROUTE     ((uint16_t)0x7FFF)  /**< @brief A device shall initialize RC_COORD to 0x7FFF on association or it is at adpMaxHops  */
#define MAC_RCCOORD_DEV_COMM_FAILED ((uint16_t)0xFFFF)  /**< @brief If a device has failed to communicate with the PAN coordinator it shall set RC_COORD to its maximum value of 0xFFFF  */

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

#endif /*HI_MAC_PIB_ATTRIBUTES_H*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
