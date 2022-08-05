/**
  ******************************************************************************
  * @file    g3_app_config.h
  * @author  AMG/IPC Application Team
  * @brief   This file contains declarations for g3 config functionalities
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

#ifndef G3_APP_CONFIG_H_
#define G3_APP_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include <task_comm.h>


#define DEV_TYPE_DEVICE     0U
#define DEV_TYPE_COORD      1U

#ifdef DEVICE_TYPE_DEVICE
static const uint8_t    dev_type = DEV_TYPE_DEVICE;
#elif defined(DEVICE_TYPE_COORD)
static const uint8_t    dev_type = DEV_TYPE_COORD;
#else
#error "either DEVICE_TYPE_DEVICE or DEVICE_TYPE_COORD must be defined"
#endif

typedef enum
{
  G3_SUCCESS = (uint8_t)0x00,        /**< @brief Operation completed successfully  */
  G3_WRONG_CRC,                      /**< @brief Wrong CRC */
  G3_WRONG_MSG_LEN,                  /**< @brief Length field in the message header has a wrong value */
  G3_WRONG_CMD_ID,                   /**< @brief Command ID not supported */
  G3_WRONG_MODE,                     /**< @brief Mode not supported */
  G3_NOT_EXPECTED,                   /**< @brief The received command is not expected */
  G3_WRONG_STATE,                    /**< @brief The received command is not correct in the current state */
  G3_WRONG_TRX_STATE,                /**< @brief Wrong transceiver state */
  G3_NOT_ACCEPTABLE,                 /**< @brief The requested operation cannot be performed in the current operation mode */
  G3_WRONG_TX_POWER,                 /**< @brief Wrong TX Power value */
  G3_WRONG_MOD_SCHEME,               /**< @brief Wrong Modulation Scheme value */
  G3_WRONG_MOD_TYPE,                 /**< @brief Wrong Modulation Type value */
  G3_WRONG_PREEMPHASIS,              /**< @brief Wrong Pre-emphasis value */
  G3_WRONG_TONEMAP,                  /**< @brief Wrong Tonemap value */
  G3_WRONG_TONEMASK,                 /**< @brief Wrong Tonemask value */
  G3_WRONG_FCH,                      /**< @brief Wrong FCH */
  G3_WRONG_VALUE_LEN,                /**< @brief The length of value for the specified attribute is not in the supported range */
  G3_WRONG_TEST_MODE,                /**< @brief The test mode given is not supported */
  G3_QUEUE_FULL,                     /**< @brief Error on queue management */
  G3_DATA_LIMIT_REACHED,             /**< @brief The data passed as input to RTE exceeds the supported limit */
  G3_RESERVERD_CMD_ID,               /**< @brief The Command ID given is reserved */
  G3_RTE_INTERFACE_TIMEOUT,          /**< @brief Operation aborted due to a RTE timeout */
  G3_TEST_FAILED,                    /**< @brief Activation or deactivation of the test is not possible */
  G3_TEST_NOT_ACTIVE,                /**< @brief Deactivation not possible because a test is not active */
  G3_TEST_ALREADY_ACTIVE,            /**< @brief Activation not possible because a test is already active */
  G3_TEST_NOT_IMPLEMENTED,           /**< @brief Activation not possible because the test is not implemented yet */
  G3_TEST_NOT_IDLE,                  /**< @brief Activation not possible because the system is not idle */
  G3_TEST_NOT_ACCEPTABLE,            /**< @brief Received command cannot be managed when test is active */
  G3_FAILED,                         /**< @brief The requested operation is failed */
  G3_TX_BUSY,                        /**< @brief The transmitted is busy */
  G3_RX_BUSY,                        /**< @brief The receiver is busy */
  G3_CS_IDLE,                        /**< @brief Carrier sense is idle */
  G3_CS_BUSY,                        /**< @brief Carrier sense is busy */
  G3_PREVIOUS_REQUEST_FAILED,        /**< @brief The current operation cannot be completed as the previous request failed */
  G3_NO_INDICATION,                  /**< @brief An expected Indication is missing */
  G3_PLME_CS_CONF_TMR_EXP,           /**< @brief Transmission failed because the RTE did not receive the PLME_CS.Confirm() */
  G3_PLME_SET_TRX1_KO,               /**< @brief Transmission failed because the RTE received the first PLME_SET_TRX_STATE.Confirm() with un unexpected result */
  G3_PLME_SET_TRX1_TMR_EXP,          /**< @brief Transmission failed because the RTE did not receive the first PLME_SET_TRX_STATE.Confirm() */
  G3_PLME_SET_TRX2_KO,               /**< @brief Transmission failed because the RTE received the second PLME_SET_TRX_STATE.Confirm() with un unexpected result */
  G3_PLME_SET_TRX2_TMR_EXP,          /**< @brief Transmission failed because the RTE did not receive the second PLME_SET_TRX_STATE.Confirm()*/
  G3_PLME_SET_TRX3_KO,               /**< @brief Transmission failed because the RTE received the third PLME_SET_TRX_STATE.Confirm() with un unexpected result */
  G3_PLME_SET_TRX3_TMR_EXP,          /**< @brief Transmission failed because the RTE did not receive the third PLME_SET_TRX_STATE.Confirm() */
  G3_PLME_SET_KO,                    /**< @brief Transmission failed because the RTE received the PLME_SET.Confirm() with un unexpected result */
  G3_PLME_SET_TMR_EXP,               /**< @brief Transmission failed because the RTE did not receive the PLME_SET.Confirm() */
  G3_PD_DATA_CONF_FAILED,            /**< @brief Transmission failed because the RTE received the PLME_SET.Confirm() with un unexpected result */
  G3_PD_DATA_CONF_TMR_EXP,           /**< @brief Transmission failed because the RTE did not receive the PD_DATA.Confirm() */
  G3_SEGMENT_DISCARDED,              /**< @brief The RTE discarded the last segment due to some errors on the transmission of the previous one. */
  G3_WRONG_VALUE,                    /**< @brief The value for the specified attribute is not in the supported range */
  G3_WRONG_ATTR_NUM,                 /**< @brief The number of attributes given is out of the supported range */
  G3_WRONG_DT,                       /**< @brief Wrong DT type */
  G3_ACTION_NOT_IMPLEMENTED,         /**< @brief The required recovery action is not implemented */
  G3_ACTION_UNKNOWN,                 /**< @brief The required recovery action is unknow */
  G3_DT_ACKREQ_MISMATCH,             /**< @brief The indicated value for filed DT does not match the bit AckReq in PSD */
  G3_PLME_GET_CONF_TMR_EXP,          /**< @brief Reception failed on RTE*/
  G3_PD_ACK_CONF_FAILED,             /**< @brief Reception failed on RTE */
  G3_PD_ACK_REQ_TMR_EXP,             /**< @brief Reception failed on RTE */
  G3_RX_NACK,                        /**< @brief Received NACK after macMaxFrameRetries attempt of transmission */
  G3_ERR_SEG_COUNT,                  /**< @brief Error in SC field of MAC header */
  G3_ERR_SEG_ID,                     /**< @brief Error in SC field of MAC header */
  G3_ERR_SEG_CC,                     /**< @brief Error in SC field of MAC header */
  G3_ERR_SEG_AR,                     /**< @brief Error in SC field of MAC header */
  G3_TX_QUEUE_TO_SHORT,              /**< @brief The RTE didn’t accept the segment because the data queue is to short to manage the transmission of the frame */
  G3_NO_EARLIER_SEG,                 /**< @brief The RTE didn't accept the segment because it didn't receive the previous one */
  G3_ERR_SEQ_NUM,                    /**< @brief Error in SN field of MAC header */
  G3_INTERNAL_ERROR,                 /**< @brief RTE internal error */
  G3_SEGMENT_TOO_LATE,               /**< @brief The RTE didn’t accept the segment because it is hasn't been received in time for the TX */
  G3_WRONG_TWORSBLOCKS,              /**< @brief Wrong TwoRSBlocks value */
  G3_NOT_IDLE,                       /**< @brief Required operation cannot be performed because RTE is not IDLE */
  G3_WRONG_BAND,                     /**< @brief Wrong band */
  G3_NOT_SUPPORTED_BAND,             /**< @brief Not supported band */
  G3_NOT_ACCEPTABLE_LEN,             /**< @brief The length of the frame is not acceptable */
  G3_PLC_DISABLED,                   /**< @brief The RTE didn’t accept the frame to because PLC is disablede */
  G3_PHY_NOT_READY,                  /**< @brief The requested action can’t be accepted by the PHY */
  G3_WRONG_LEN,                      /**< @brief Wrong length */
  G3_BANDSETOK_ATTRSETKO,            /**< @brief Bandplan set ended with success, but the default PHY setting hasn't been restored */

  G3_BUSY = (uint8_t)0x60,           /**< @brief The requested operation was not successfully completed beacuse the stack is in busy state */
  G3_INVALID_IPv6_FRAME,             /**< @brief A wrong IPv6 frame has been provided  */
  G3_INVALID_QOS,                    /**< @brief Invalid QoS */
  G3_BAD_CCM_OUTPUT,                 /**< @brief The security processing has been attempted on an outgoing frame and it failed because the security engine produced erroneous output */
  G3_ROUTE_ERROR,                    /**< @brief The requested operation has failed due to a routing failure on the sending device  */
  G3_BT_TABLE_FULL,                  /**< @brief An attempt to send a broadcast frame or member mode multicast has failed due to the fact that there is no room in the BTT  */
  G3_FRAME_NOT_BUFFERED,             /**< @brief An frame transmission has failed due to insufficient buffering available  */
  G3_STARTUP_FAILURE,                /**< @brief An ADPM-NETWORK-START.request has failed to start a network  */
  G3_NOT_PERMITTED,                  /**< @brief An ADPM-NETWORK-JOIN.request has been disallowed  */
  G3_UNKNOWN_DEVICE,                 /**< @brief The device addressed in the parameter list is not accepted */
  G3_SENDING_DATA_ERROR,             /**< @brief The requested operation has failed because the lower layers were not able to successfully send the frame  */
  G3_PARAMETER_ERROR,                /**< @brief Error while setting or retrieving a parameter  */
  G3_WRONG_DEVICE_TYPE,              /**< @brief The requested operation has failed because of a wrong adpDeviceType for the specified operation */
  G3_ADD_EVENT_ERROR,                /**< @brief An attempt to add an event to internal FSM faild */
  G3_GET_EVENT_ERROR,                /**< @brief An attempt to get an event from internal FSM failed  */
  G3_MALLOC_FAIL,                    /**< @brief Memory buffer management error  */
  G3_RTEIPC_ERROR,                   /**< @brief Error on nRT - RTE interface */
  G3_WRONG_MEDIA_TYPE,               /**< @brief Wrong Media Type value */
  G3_UNAVAILABLE_MEDIA_TYPE,         /**< @brief The media for the transmission are blacklisted */
  G3_DUTY_CYCLE_ERR,                 /**< @brief The data frame cannot be transmitted over the RF interface due to Duty Cycle limitation */
  G3_DISABLED_MEDIA_TYPE,            /**< @brief The media type selected for the transmission is disabled */

  G3_COUNTER_ERROR = (uint8_t)0xdb, /**< @brief The frame counter of the received frame is invalid  */
  G3_IMPROPER_KEY_TYPE,             /**< @brief The key applied on the received frame is not allowed to be used with that frame type  */
  G3_IMPROPER_SECURITY_LEVEL,       /**< @brief The security level does not meet the minimum required security level  */
  G3_UNSUPPORTED_LEGACY,            /**< @brief The received frame was purportedly secured using security based on IEEE Std 802154-2003, and such security is not supported by this standard  */
  G3_UNSUPPORTED_SECURITY,          /**< @brief The security purportedly applied by the originator of the received frame is not supported  */
  G3_BEACON_LOSS,                   /**< @brief The beacon was lost following a synchronization request  */
  G3_CHANNEL_ACCESS_FAILURE,        /**< @brief A transmission could not take place due to activity on the channel  */
  G3_DENIED,                        /**< @brief The GTS request has been denied by the PAN coordinator  */
  G3_DISABLE_TRX_FAILURE,           /**< @brief The attempt to disable the transceiver has failed  */
  G3_SECURITY_ERROR,                /**< @brief Cryptographic processing of the received secured frame failed  */
  G3_FRAME_TOO_LONG,                /**< @brief Either a frame resulting from processing has a length that is greater than aMaxPHYPacketSize or a requested transaction is too large to fit in the CAP or GTS  */
  G3_INVALID_GTS,                   /**< @brief The requested GTS transmission failed because the specified GTS either did not have a transmit GTS direction or was not defined  */
  G3_INVALID_HANDLE,                /**< @brief A request to purge an MSDU from the transaction queue was made using an MSDU handle that was not found in the transaction table  */
  G3_INVALID_PARAMETER,             /**< @brief A parameter in the primitive is either not supported or is out of the valid range  */
  G3_NO_ACK,                        /**< @brief No acknowledgment was received after macMaxFrameRetries  */
  G3_NO_BEACON,                     /**< @brief A scan operation failed to find any network beacons  */
  G3_NO_DATA,                       /**< @brief No response data were available following a request  */
  G3_NO_SHORT_ADDRESS,              /**< @brief The operation failed because a 16-bit short address was not allocated  */
  G3_OUT_OF_CAP,                    /**< @brief A receiver enable request was unsuccessful because it could not be  completed within the CAP  */
  G3_PAN_ID_CONFLICT,               /**< @brief A PAN identifier conflict has been detected and communicated to the PAN coordinator  */
  G3_REALIGNMENT,                   /**< @brief A coordinator realignment command has been received  */
  G3_TRANSACTION_EXPIRED,           /**< @brief The transaction has expired and its information was discarded  */
  G3_TRANSACTION_OVERFLOW,          /**< @brief There is no capacity to store the transaction  */
  G3_TX_ACTIVE,                     /**< @brief The transceiver was in the transmitter enabled state when the receiver was requested to be enabled  */
  G3_UNAVAILABLE_KEY,               /**< @brief The key purportedly used by the originator of the received frame is not available or, if available, the originating device is not known or is blacklisted with that particular key  */
  G3_UNSUPPORTED_ATTRIBUTE,         /**< @brief A SET/GET request was issued with the identifier of a PIB attribute that is not supported  */
  G3_INVALID_ADDRESS,               /**< @brief A request to send data was unsuccessful because neither the source address parameters nor the destination address parameters were present  */
  G3_ON_TIME_TOO_LONG,              /**< @brief A receiver enable request was unsuccessful because it specified a number of symbols that was longer than the beacon interval  */
  G3_PAST_TIME,                     /**< @brief A receiver enable request was unsuccessful because it could not be completed within the current superframe and was not permitted to be deferred until the next superframe  */
  G3_TRACKING_OFF,                  /**< @brief The device was instructed to start sending beacons based on the timing of the beacon transmissions of its coordinator, but the device is not currently tracking the beacon of its coordinator  */
  G3_INVALID_INDEX,                 /**< @brief An attempt to write to a PIB attribute that is in a table failed because the specified table index was out of range  */
  G3_LIMIT_REACHED,                 /**< @brief A scan operation terminated prematurely because the number of PAN descriptors stored reached an implementationspecified maximum  */
  G3_READ_ONLY,                     /**< @brief A SET/GET request was issued with the identifier of an attribute that is read only  */
  G3_SCAN_IN_PROGRESS,              /**< @brief A request to perform a scan operation failed because the MLME was in the process of performing a previously initiated scan operation  */
  G3_SUPERFRAME_OVERLAP,            /**< @brief The device was instructed to start sending beacons based on the timing of the beacon transmissions of its coordinator, but the instructed start time overlapped the transmission time ofthe beacon of its coordinator  */
} g3_result_t;

void g3_app_conf_init(task_comm_info_t *g3_task_comm_info_);
void g3_app_conf_start(void);
void g3_app_conf(void);
bool g3_app_conf_msg_needed(const msg_t *msg);
void g3_app_conf_msg_handler(uint8_t cmd_id, const void *msg);

#endif
