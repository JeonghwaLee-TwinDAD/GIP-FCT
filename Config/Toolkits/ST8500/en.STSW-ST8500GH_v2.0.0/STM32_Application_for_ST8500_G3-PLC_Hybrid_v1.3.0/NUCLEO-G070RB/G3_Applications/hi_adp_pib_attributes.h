/**
  ******************************************************************************
  * @file    hi_adp_pib_attributes.h
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
#ifndef HI_ADP_PIB_ATTRIBUTES_H
#define HI_ADP_PIB_ATTRIBUTES_H

#include <stdint.h>

#include "hi_adp_pib_types.h"
#include "hi_adp_lbp_message_catalog.h"

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_adp_drv HIF ADP DRV
  * @{
  */

/** @defgroup ADP_PIB_Attributes ADP PIB Attributes
  * @brief ADP PIB Attributes module
  * @details  This module contains the definition/handling of ADPMAC PIB Attributes, which are defined by the G3 and 6LoWPAN standards.
  *           @n There's also an additional set of ADP PIB Attributes, which are implementation specific.
  *           @n Below there's the description of each implemented PIB Attribute.
  *           @n @c adpPv6Address Counter of discarded ACK frame for any of the above reason.
  * @{
  */

/** @defgroup ADP_PIB_Attributes_TypesDefinitions ADP PIB Attributes Types Definitions
  * @brief    ADP PIB Attributes types definitions
  * @{
  */

/* G3-PLC PIB attribute ID definitions */
#define ADP_SECURITYLEVEL_ID             ((uint32_t)0x00)  /**< @brief adpSecurityLevel Attribute */
#define ADP_PREFIXTABLE_ID               ((uint32_t)0x01)  /**< @brief adpPrefixTable Attribute */
#define ADP_BROADCASTLOGTABLEENTRYTTL_ID ((uint32_t)0x02)  /**< @brief adpBroadcastLogTableEntryTTL  Attribute */
#define ADP_METRICTYPE_ID                ((uint32_t)0x03)  /**< @brief adpMetricType Attribute */
#define ADP_LOWLQIVALUE_ID               ((uint32_t)0x04)  /**< @brief adpLowLQIValue Attribute */
#define ADP_HIGHLQIVALUE_ID              ((uint32_t)0x05)  /**< @brief adpHighLQIValue Attribute */
#define ADP_RREPWAIT_ID                  ((uint32_t)0x06)  /**< @brief AdpRREPWait Attribute */
#define ADP_CONTEXTINFORMATIONTABLE_ID   ((uint32_t)0x07)  /**< @brief adpContextInformationTable Attribute */
#define ADP_COORDSHORTADDRESS_ID         ((uint32_t)0x08)  /**< @brief adpCoordShortAddress Attribute */
#define ADP_RLCENABLED_ID                ((uint32_t)0x09)  /**< @brief adpRLCEnabled Attribute */
#define ADP_ADDREVLINKCOST               ((uint32_t)0x0A)  /**< @brief adpAddRevLinkCost Attribute */
#define ADP_BROADCASTLOGTABLE_ID         ((uint32_t)0x0B)  /**< @brief adpBroadcastLogTable Attribute */
#define ADP_ROUTINGTABLE_ID              ((uint32_t)0x0C)  /**< @brief adpRoutingTable Attribute */
#define ADP_UNICASTRREQGENENABLE_ID      ((uint32_t)0x0D)  /**< @brief adpUnicastRREQGenEnable Attribute */
#define ADP_GROUPTABLE_ID                ((uint32_t)0x0E)  /**< @brief adpGroupTable Attribute */
#define ADP_MAXHOPS_ID                   ((uint32_t)0x0F)  /**< @brief adpMaxHops Attribute */
#define ADP_DEVICETYPE_ID                ((uint32_t)0x10)  /**< @brief adpDeviceType Attribute */
#define ADP_NETTRAVERSALTIME_ID          ((uint32_t)0x11)  /**< @brief adpNetTraversalTime Attribute */
#define ADP_ROUTINGTABLEENTRYTTL_ID      ((uint32_t)0x12)  /**< @brief adpRoutingTableEntryTTL Attribute */
#define ADP_KR_ID                        ((uint32_t)0x13)  /**< @brief adpKr Attribute */
#define ADP_KM_ID                        ((uint32_t)0x14)  /**< @brief adpKm Attribute */
#define ADP_KC_ID                        ((uint32_t)0x15)  /**< @brief adpKc Attribute */
#define ADP_KQ_ID                        ((uint32_t)0x16)  /**< @brief adpKq Attribute */
#define ADP_KH_ID                        ((uint32_t)0x17)  /**< @brief adpKh Attribute */
#define ADP_RREQRETRIES_ID               ((uint32_t)0x18)  /**< @brief adpRREQRetries Attribute */
#define ADP_RREQRERRWAIT_ID              ((uint32_t)0x19)  /**< @brief adpRREQRERRWait Attribute */
#define ADP_WEAKLQIVALUE_ID              ((uint32_t)0x1A)  /**< @brief adpWeakLQIValue Attribute */
#define ADP_KRT_ID                       ((uint32_t)0x1B)  /**< @brief adpKrt Attribute */
#define ADP_SOFTVERSION_ID               ((uint32_t)0x1C)  /**< @brief adpSoftVersion Attribute: certification firmware version */
#define ADP_SNIFFERMODE_ID               ((uint32_t)0x1D)  /**< @brief adpSnifferMode Attribute */
#define ADP_BLACKLISTTABLE_ID            ((uint32_t)0x1E)  /**< @brief adpBlacklistTable Attribute */
#define ADP_BLACKLISTTABLEENTRYTTL_ID    ((uint32_t)0x1F)  /**< @brief adpBlacklistTableEntryTTL Attribute */
#define ADP_MAXJOINWAITTIME_ID           ((uint32_t)0x20)  /**< @brief adpMaxJoinWaitTime Attribute */
#define ADP_PATHDISCOVERYTIME_ID         ((uint32_t)0x21)  /**< @brief adpPathDiscoveryTime Attribute */
#define ADP_ACTIVEKEYINDEX_ID            ((uint32_t)0x22)  /**< @brief adpActiveKeyIndex Attribute */
#define ADP_DESTINATIONADDRESSSET_ID     ((uint32_t)0x23)  /**< @brief adpDestinationAddressSet Attribute */
#define ADP_DEFAULTCOORDROUTEENABLED     ((uint32_t)0x24)  /**< @brief adpDefaultCoordRouteEnabled Attribute */
#define ADP_DISABLEDEFAULTROUTING_ID     ((uint32_t)0xF0)  /**< @brief adpDisableDefaultRouting Attribute */
/* Implementation Defined PIB attribute ID definitions */
#define ADP_DATAGRAMTAG_ID               ((uint32_t)0x90)  /**< @brief adpDatagramTag Attribute: Define the DatagramTag for fragmentation dispatch. Incerased by one for each transmission */
#define ADP_BRDSEQNUM_ID                 ((uint32_t)0x91)  /**< @brief adpBroadcastSeqNumber Attribute: Define the SequenceNumber for broadcast dispatch. Increased by one for each broadcast transmission */
#define ADP_ENABLEHDRCOMPR_ID            ((uint32_t)0x92)  /**< @brief adpEnableHeaderCompression: set to 1 to enable the IPv6 header compression (default), set to 0 to disable the IPv6 header compression */
#define ADP_RREQFWDMAXTIME_ID            ((uint32_t)0x93)  /**< @brief adpRREQFfwdMaxTime Attribute: the maximum time in ms a RREQ takes to be forwarded */
#define ADP_EAPPSKKEY_ID                 ((uint32_t)0x94)  /**< @brief adpEAPPSKKey Attribute: it contains the EAP-PSK attribute used for bootstrap */
#define ADP_EAPPSKIDP_ID                 ((uint32_t)0x95)  /**< @brief adpEAPPSKIdP Attribute: it contains the IdP (device NAI) attribute used for bootstrap */
#define ADP_EAPPSKIDPLEN_ID              ((uint32_t)0x96)  /**< @brief adpEAPPSKIdPLen Attribute: it contains the IdP length used for bootstrap */
#define ADP_MAXREKEYINGWAITTIME_ID       ((uint32_t)0x97)  /**< @brief adpMaxRekeyingWaitTime Attribute: rekeying timeout in seconds for LBD */
#define ADP_LOADngSEQNUM_ID              ((uint32_t)0x98)  /**< @brief adpLOADngSeqNum: The sequence number used by LOADng routing algorithm. It shall be included in each generated RREQ or RREP message */
#define ADP_OPTIONALFEATURES_ID          ((uint32_t)0x99)  /**< @brief adpOptionalFeatures: Field mask to control optional features provided by the ADP Layer where
                                                                      Bit 0 - adpAllowInvalidHopsLeft: if 6LoWPAN frame with HopsLeft > adpMaxHops is received, simply decrement HopsLeft without setting it to adpMaxHops
                                                                      Bit 1 - adpMulticastFWDisabled: if 1 the forwarding of LOADng multicast frames is disabled
                                                                      Bit 2 - adpNoAck: if 1 the ADP does not require MAC Acknowledgement
                                                                      Bit 3 - adpRREQFWDisabled: if 1 the forwarding of LOADng RREQ messages in broadcast is disabled
                                                                      Bit 4 - adpRERRifHopsLeftExp: if 1 sends a RERR when receiving  packet and Hops Left is == 0
                                                                      Bit 5 - adpDiscoveryRouteCost: configure how to compute the RouteCost of the PAN Descriptors during discovery.
                                                                      Bit 6 - If 1 the RouteCost is considered only from LBA to Coord, if 0 the RouteCost is considered from LBD to Coord (adding the LinkCost from LBD to LBA computed on Beacon Frame)
                                                                      Bit 7 - If 1 blacklist the node also in case of failure of data transmission
                                                                      */
#define ADP_REASSEMBLYTIMEOUT_ID         ((uint32_t)0x9A)  /**< @brief adpReassemblyTimeout: when this time expires, if the entire packet has not been reassembled, the existing fragments are discarded (seconds) */
#define ADP_RETXTIMEFORCHBUSY_ID         ((uint32_t)0x9B)  /**< @brief adpRetransmissionDelayforChannelBusy: specify the daly in milliseconds for the ADP retransmission of one packet when the MAC report Channel Access Busy error */
#define ADP_HOPSFORDELAYFRAGN_ID         ((uint32_t)0x9C)  /**< @brief adpHopsForDelayFragN: specify the number of hops for which the FragN fragments are delayed */
#define ADP_DELAYFRAGN_ID                ((uint32_t)0x9D)  /**< @brief adpDelayFRAGN: specify the delay in milliseconds (up to 0xFFFF) for which the ADP waits before sending the FragN if the endpoint is adpHopsForDelayFragN far from the node */
#define ADP_DELAYFRAGNBROAD_ID           ((uint32_t)0x9E)  /**< @brief adpDelayFRAGNBroad: specify the delay in milliseconds (up to 0xFFFF) for which the ADP waits before sending the FragN if the trasnmission is multicast/broadcast */
#define ADP_ROUTINGTABLE_BYSHORTADDR_ID  ((uint32_t)0x9F)  /**< @brief adpRoutingTable Attribute, index is used as short address */
#define ADP_ROUTINGTABLE_VALIDENTRY_ID   ((uint32_t)0xA0)  /**< @brief adpRoutingTable Attribute but the entry returned is the next one valid */
#define ADP_UNICASTRXFRAGCNT_ID          ((uint32_t)0xA1)  /**< @brief counter of received unicast fragments */
#define ADP_UNICASTTXFRAGCNT_ID          ((uint32_t)0xA2)  /**< @brief counter of transmitted unicast framgents */
#define ADP_UNICASTFWFRAGCNT_ID          ((uint32_t)0xA3)  /**< @brief counter of forwarded unicast framgents */
#define ADP_UNICASTINDCNT_ID             ((uint32_t)0xA4)  /**< @brief counter of unicast ADP indication */
#define ADP_MCASTRXFRAGCNT_ID            ((uint32_t)0xA5)  /**< @brief counter of received multicast fragments */
#define ADP_MCASTTXFRAGCNT_ID            ((uint32_t)0xA6)  /**< @brief counter of transmitted multicast fragments */
#define ADP_MCASTFWFRAGCNT_ID            ((uint32_t)0xA7)  /**< @brief counter of forwarded multicast framgents*/
#define ADP_MCASTINDFRAGCNT_ID           ((uint32_t)0xA8)  /**< @brief counter of multicast fragments used for indication */
#define ADP_MCASTFWTHRESHOLDS1_ID        ((uint32_t)0xA9)  /**< @brief thresholds for multicast forward */
#define ADP_MCASTFWTHRESHOLDS2_ID        ((uint32_t)0xAA)  /**< @brief thresholds for multicast forward */
#define ADP_RETRYFORCHBUSY_ID            ((uint32_t)0xAB)  /**< @brief Number of retransmission attempts before declaring a failure in case of channel busy) */

/* Bitmask definitions */
#define ADP_OPTFEAT_ALLOWINVALIDHOPSLEFT   ((uint32_t)0x00000001) /**< @brief mask for Allow Invalid Hops Left: if 6LoWPAN frame with HopsLeft > adpMaxHops is received, simply decrement HopsLeft without setting it to adpMaxHops */
#define ADP_OPTFEAT_MULTFWDISABLED         ((uint32_t)0x00000002) /**< @brief mask for Multicast FW Disabled: disable the forward of multicast packets */
#define ADP_OPTFEAT_NOACK                  ((uint32_t)0x00000004) /**< @brief mask for NoAck: send IPv6 data with NO_ACK option at MAC level */
#define ADP_OPTFEAT_RREQFWDISABLED         ((uint32_t)0x00000008) /**< @brief mask for RREQ FW Disabled: disable the forwarding of RREQ */
#define ADP_OPTFEAT_RERRHOPSLEFTZERO       ((uint32_t)0x00000010) /**< @brief mask for RERR if HopsLeft Expires parameter */
#define ADP_OPTFEAT_DISCOVERYROUTECOST     ((uint32_t)0x00000020) /**< @brief mask for Discovery Route Cost parameter: configure how to compute the RouteCost of the PAN Descriptors during discovery */
#define ADP_OPTFEAT_DISABLEPANIDCONFONBOOT ((uint32_t)0x00000040) /**< @brief mask for Disable the PAN_ID conflict notification during the device bootstrap phase and re-enable ti after the bootstrap phase */
#define ADP_OPTFEAT_BLACKLISTONDATAFAIL    ((uint32_t)0x00000080) /**< @brief mask for Set to 1 to blacklist the next hop also when a Data transmission fails (in addition to RREP tx failures) */
#define ADP_OPTFEAT_BLACKLISTONRREQFAIL    ((uint32_t)0x00000100) /**< @brief mask for Set to 1 to blacklist the next hop also when a Unicast RREQ transmission fails (in addition to RREP tx failures) */
#define ADP_OPTFEAT_MCASTFW_POLICYMASK     ((uint32_t)0xF0000000) /**< @brief mask for Multicast Fragment forward policy */
#define ADP_OPTFEAT_MCASTFW_PrFWD          ((uint32_t)0x10000000) /**< @brief mask for Multicast Fragment forward policy: PrFwd policy */

#define ADP_OPTFEAT_DEFAULTVALUE         (ADP_OPTFEAT_RERRHOPSLEFTZERO | ADP_OPTFEAT_DISABLEPANIDCONFONBOOT | ADP_OPTFEAT_BLACKLISTONDATAFAIL | ADP_OPTFEAT_BLACKLISTONRREQFAIL) /**< @brief optional features activated by default */


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

#endif /* HI_ADP_PIB_ATTRIBUTES_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
