/**
  ******************************************************************************
  * @file    hi_phy_support.h
  * @author  AMG/IPC Application Team
  * @brief   Header file for PHY support code
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

#ifndef HI_PHY_SUPPORT_H
#define HI_PHY_SUPPORT_H

#include <stdint.h>

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_phy_drv HIF PHY DRV
  * @{
  */

/** @defgroup PHY_Support_TypesDefinitions PHY Support Types Definitions
  * @brief    PHY Support types definitions
  * @{
  */

#define MAC_TX_RES_6DB                     ((uint8_t)0x00)     /**< @brief Transmission power increase step of 6db  */
#define MAC_TX_RES_3DB                     ((uint8_t)0x01)     /**< @brief Transmission power increase step of 3db  */

#define MAC_NEIGHBOR_TABLE_DEFAULT_TXCOEFF ((int8_t)0x00)  /**< @brief Default TxCoeff value for a sub-band used for transmission */
#define MAC_NEIGHBOR_TABLE_DEFAULT_LQI     ((uint8_t)0x00) /**< @brief Default Link Quality Indicator value for a neighbor table entry */

#define PHY_ROBO_MODULATION              ((uint8_t)0x00) /**< @brief ROBO Modulation Id */
#define PHY_DBPSK_MODULATION             ((uint8_t)0x01) /**< @brief DBPSK Modulation Id */
#define PHY_DQPSK_MODULATION             ((uint8_t)0x02) /**< @brief DQPSK Modulation Id */
#define PHY_D8PSK_MODULATION             ((uint8_t)0x03) /**< @brief D8PSK Modulation Id */

#define PHY_MODULATION_TYPES             ((uint8_t)0x04)   /**< @brief Number of different modulation types*/

#define PHY_CC_REPETITION                ((uint8_t)2)    /**< @brief Convolutional Encoder with rate 1/2. The repetition factor is 2 */
#define PHY_CC_FLUSH                     ((uint8_t)6)    /**< @brief 6 bits are needed to flush the Convolutional Encoder */

#define PHY_RS_REDUNDANCY_ROBO           ((uint8_t)64)   /**< @brief Reed Solomon Encoder adds 64 bits of redundacy for ROBO mode */
#define PHY_RS_REDUNDANCY_DxPSK          ((uint8_t)128)  /**< @brief Reed Solomon Encoder adds 128 bits of redundacy for DBPSK, DQPSK and D8PSK modes */

#define PHY_REPETITION_FACTOR_ROBO       ((uint8_t)4)   /**< @brief Repetition Code by 4 in ROBO mode */
#define PHY_REPETITION_FACTOR_DxPSK      ((uint8_t)1)   /**< @brief No Repetition Code in DBPSK, DQPSK and D8PSK mode (repetition by 1) */
#define PHY_REPETITION_FACTOR_SUPERROBO  ((uint8_t)6)   /**< @brief Repetition Code by 6 in SUPER-ROBO mode */

#define PHY_MAX_RS_INPUT_ROBO            ((uint16_t)247)    /**< @brief Maximum number of bytes for a frame transmitted with ROBO modulation  */
#define PHY_MAX_RS_INPUT_DXPSK           ((uint16_t)239)    /**< @brief Maximum number of bytes for a frame transmitted with DXPSK modulation  */

#define PHY_MAX_OFDM_SYMBOLS_CENA        ((uint16_t)252)    /**< @brief Maximum number of symbols in a PHY frame  */
#define PHY_MAX_OFDM_SYMBOLS_FCC         ((uint16_t)511)    /**< @brief Maximum number of symbols in a PHY frame  */

#define PHY_MAX_FRAME_LENGTH             ((uint16_t)63)     /**< @brief Maximum number of OFDM symbols in one PHY frame */

#define PHY_BITS_PER_SYM_ROBO            ((uint16_t)1) /**< @brief Bits for modulated symbols in ROBO */
#define PHY_BITS_PER_SYM_DBPSK           ((uint16_t)1) /**< @brief Bits for modulated symbols in DBPSK */
#define PHY_BITS_PER_SYM_DQPSK           ((uint16_t)2) /**< @brief Bits for modulated symbols in DQPSK */
#define PHY_BITS_PER_SYM_D8PSK           ((uint16_t)3) /**< @brief Bits for modulated symbols in D8PSK */

#define PHY_CC_ZERO_TAIL                 ((uint16_t)6) /**< @brief Numbero of bits for Zero Tail in convolutional encoder */
#define PHY_CC_RATE_INV                  ((uint16_t)2) /**< @brief Inverse of the rate of convolutional encoder */
#define PHY_CC_RATE                      ((double)1.0 / PHY_CC_RATE_INV) /**< @brief Rate of convolutional encoder */

#define PHY_PARITY_LENGTH_ROBO           ((uint16_t)8)     /**< @brief Number of bytes added by RS encoder with ROBO mode */
#define PHY_PARITY_LENGTH_DxPSK          ((uint16_t)16)    /**< @brief Number of bytes added by RS encoder without ROBO mode */

#define PHY_TRXSTATE_TXONRXOFF           ((uint8_t)0x09)   /**< @brief PHY in Tx state */
#define PHY_TRXSTATE_TXOFFRXON           ((uint8_t)0x0A)   /**< @brief PHY in Rx state */

#define PHY_MAXTXPOWER                   ((uint8_t)0x20)   /**< @brief Maximum value of transmission power */

#define PHY_DEFAULTTONEMAP_CENA          ((uint8_t)0x3F)   /**< @brief Default Value of the Tone Map (all subbands active) CEN-A */
#define PHY_CARRIERS_PER_SUBBAND_CENA    ((uint8_t)0x06)   /**< @brief Number of carriers for each subband  CEN-A */
#define PHY_NUM_OF_SUBBANDS_CENA         ((uint8_t)0x06)   /**< @brief Number of subbands used for transmission CEN-A */

#define PHY_DEFAULTTONEMAP_CENB          ((uint8_t)0x0F)   /**< @brief Default Value of the Tone Map (all subbands active) CEN-B */
#define PHY_CARRIERS_PER_SUBBAND_CENB    ((uint8_t)0x04)   /**< @brief Number of carriers for each subband  CEN-B */
#define PHY_NUM_OF_SUBBANDS_CENB         ((uint8_t)0x04)   /**< @brief Number of subbands used for transmission CEN-B */

#define PHY_TXCOEFF_SIZE_CEN             ((uint8_t)3)      /**< @brief Sixe in byte of the TxCoeff array  CEN-Aand CEN-B*/

#define PHY_DEFAULTTONEMAP_FCC           ((uint8_t)0xFF)   /**< @brief Default Value of the Tone Map (all subbands active) FCC */
#define PHY_CARRIERS_PER_SUBBAND_FCC     ((uint8_t)3)      /**< @brief Number of carriers for each subband  FCC */
#define PHY_SNR_PER_SUBBAND_FCC          PHY_CARRIERS_PER_SUBBAND_FCC /**< @brief Number of SNR for each subband  FCC */
#define PHY_NUM_OF_SUBBANDS_FCC          ((uint8_t)24)     /**< @brief Number of subbands used for transmission  FCC */
#define PHY_TXCOEFF_SIZE_FCC             ((uint8_t)6)      /**< @brief Sixe in byte of the TxCoeff array  FCC */

#define PHY_NUM_OF_CARRIERS              ((uint16_t)72)    /**< @brief Maximum number of carriers (72 carriers defined by FCC bandplan) */

#define PHY_NFCH                         ((uint8_t)0x0D)   /**< @brief Number of FCH symbols */
#define PHY_NPRE                         ((double)  9.5)   /**< @brief Number of preamble symbols */

#define PHY_DT_NOACK                     ((uint8_t)0x00)   /**< @brief Delimiter Type: No Response Required */
#define PHY_DT_ACK                       ((uint8_t)0x01)   /**< @brief Delimiter Type: Response Required */

#define PHY_MOD_SCHEME_DIFF              ((uint8_t)0)      /**< @brief Mod Scheme: Differential Modulation Scheme */
#define PHY_MOD_SCHEME_COHE              ((uint8_t)1)      /**< @brief Mod Scheme: Coherent Modulation Scheme */
#define PHY_MOD_SCHEME_NUM               ((uint8_t)2)      /**< @brief Number of Mod Schemes: 2 beacuse we support Differential and Coherent Schemes */

#define PHY_PILOT_FREQSPACING            ((double)12.0)    /**< @brief Pilot Spacing in frequency: Pilot Spacing as defined by G3 (12 for CENELEC-A Bandplan) */

#define PHY_REED_SOLOMON_BPS             ((uint8_t)8)       /**< @brief Number of bits in one RS symbol (8 bits, 1 symbol is equal to 1 byte) */

#define PHY_2RSBLOCKS_ONE                ((uint8_t)0x01)    /**< @brief Internal code to identify if one RS block is used */
#define PHY_2RSBLOCKS_TWO                ((uint8_t)0x00)    /**< @brief Internal code to identify if two RS blocks are used */

#define PHY_MAX_PAYLOAD_LEN_FCC          ((uint16_t)494)    /**< @brief Maximum payload handled by PHY in term of bytes for FCC bandplan */
#define PHY_MAX_PAYLOAD_LEN_CEN          ((uint16_t)239)    /**< @brief Maximum payload handled by PHY in term of bytes for CEN-A and CEN-B bandplans*/

#define PHY_PREEMPHASIS_SIZE             ((uint16_t)0x0018) /**< @brief Size (in bytes) of the PreEmphasis parameter */
#define PHY_TONEMAP_SIZE                 ((uint16_t)0x0003) /**< @brief Size (in bytes) of the Tone Map parameter */
#define PHY_TONEMASK_SIZE                ((uint16_t)0x0009) /**< @brief Size (in bytes) of the Tone Mask parameter */

#define PHY_PDC_IMPULSIVE_NOISE_PRESENCE ((uint8_t)0x80)   /**< @brief Flag for impulsive noise presence */
#define PHY_PDC_IMPULSIVE_NOISE_MASK     ((uint8_t)0x7F)   /**< @brief Mask for impulsive noise presence */

/**
  * @brief PHY estimation of noise and signal power
  */
typedef struct
{
    uint8_t NoisePow[PHY_NUM_OF_CARRIERS];  /**< @brief Estimation of noise power of subcarrier 0, ..., 71 in dB�V */
    uint8_t SignalPow[PHY_NUM_OF_CARRIERS]; /**< @brief Estimation of signal power of subcarrier 0, ..., 71 in dB�V */
} PHY_SNPowerEst_t;

/**
  * @brief Specify transmit gain for each sub-band represented by tone map (FCC bandplan).
  */
typedef struct
{
    uint8_t Coeff[PHY_TXCOEFF_SIZE_FCC];  /**< @brief Number of gain steps requested for the tones represented byeach bit in the TM */
} PHY_FCCTXCoeff_t;

/**
  * @brief Specify transmit gain for each sub-band represented by tone map (CEN bandplan).
  */
typedef struct
{
    uint8_t  TxCoeff1  : 4;  /**< @brief Number of gain steps requested for the tones represented by TM[1]  */
    uint8_t  TxCoeff0  : 4;  /**< @brief Number of gain steps requested for the tones represented by TM[0]  */
    uint8_t  TxCoeff3  : 4;  /**< @brief Number of gain steps requested for the tones represented by TM[3]  */
    uint8_t  TxCoeff2  : 4;  /**< @brief Number of gain steps requested for the tones represented by TM[2]  */
    uint8_t  TxCoeff5  : 4;  /**< @brief Number of gain steps requested for the tones represented by TM[5]  */
    uint8_t  TxCoeff4  : 4;  /**< @brief Number of gain steps requested for the tones represented by TM[4]  */
} PHY_CENTXCoeff_t;

/**
  * @brief  PHY Parameters Structure used in PHY data transmission request
  */
typedef struct
{
    uint8_t TxPower;                            /**< @brief The Transmission power */
    uint8_t PayloadModType;                     /**< @brief The modulation type (0: ROBO, 1: DBPSK/BPSK, 2: DQPSK/QPSK, 3: D8PSK/8PSK) */
    uint8_t PayloadModScheme;                   /**< @brief The payload modulation scheme (0: Differential, 1: Coherent) */
    uint8_t PreEmphasis[PHY_PREEMPHASIS_SIZE];  /**< @brief The PreEmphasis parameter */
    uint8_t ToneMap[PHY_TONEMAP_SIZE];          /**< @brief The Tone map parameter */
    uint8_t DT;                                 /**< @brief Delimiter type (000: Start of frame with no response expected;
                                                 *         001: Start of frame with response expected; 010: Positive acknowledgement (ACK)
                                                 *         011: Negative acknowledgement (NACK); 100-111: Reserved) */
    uint8_t TwoRSBlocks;                        /**< @brief Indication on how many RS blocks have to be used (0: Two RS blocks, 1: One RS block) */
} PHY_ReqParams_t;

/**
  * @brief  PHY Parameters Structure used in PHY data transmission confirm
  */
typedef struct
{
    uint8_t CSThresholdEvents; /**< @brief Number of Current Sense Threshold events fired during this transmissions */
    uint8_t TxPowerBegin;      /**< @brief The transmission power configured at the beginning of transmission */
    uint8_t TxPowerEnd;        /**< @brief The transmission power at the end of transmission */
    uint8_t ThermalWarnings;   /**< @brief Indicates if thermal warnings occurred during the transmission */
} PHY_ConfParams_t;

/**
  * @brief  PHY Parameters Structure used in PHY data reception indication
  */
typedef struct
{
    uint8_t SNRCarriers[PHY_NUM_OF_CARRIERS];  /**< @brief The SNR measurements of each carrier */
    uint8_t PhaseDifferential;                 /**< @brief The phase difference in multiples of 60 degrees */
    uint8_t PayloadModType;                    /**< @brief The modulation type (0: ROBO, 1: DBPSK, 2: DQPSK, 3: D8PSK) */
    uint8_t PayloadModScheme;                  /**< @brief The payload modulation scheme (0: Differential, 1: Coherent) */
    uint8_t ToneMap[PHY_TONEMAP_SIZE];         /**< @brief The Tone map parameter */
    uint8_t DT;                                /**< @brief The Delimiter type */
    uint8_t TwoRSBlocks;                       /**< @brief Indication on how many RS blocks have to be used (0: Two RS blocks, 1: One RS block) */
    uint8_t Lqi;                               /**< @brief The Link Quality Indicator */
    PHY_SNPowerEst_t SNCarriers;               /**< @brief Estimation of signal and noise power for each subcarriers */
} PHY_IndParams_t;

/**
 * @brief  FCH bit fields for ACK/NACK frames for FCC bandplans.
 */
typedef struct
{
  uint8_t  FCS1;             /**< @brief MAC FCS[7:0] */
  uint8_t  Res1     : 1;     /**< @brief Reserved by ITU-T */
  uint8_t  DT       : 3;     /**< @brief Delimiter type */
  uint8_t  Res2     : 3;     /**< @brief Reserved by ITU-T */
  uint8_t  SSCA     : 1;     /**< @brief Subsequent segment collision avoidance */
  uint8_t  Res3;             /**< @brief Reserved by ITU-T */
  uint8_t  FCS2;             /**< @brief MAC FCS[15:8] */
  uint8_t  Res4;             /**< @brief Reserved by ITU-T */
  uint8_t  Res5;             /**< @brief Reserved by ITU-T */
  uint8_t  Res6;             /**< @brief Reserved by ITU-T */
  uint8_t  FccsH    : 6;     /**< @brief Frame Control Check Sequence[7:2] */
  uint8_t  Res7     : 2;     /**< @brief Reserved by ITU-T */
  uint8_t  ConvZeros: 6;     /**< @brief Zeros for convolutional encoder */
  uint8_t  FccsL    : 2;     /**< @brief Frame Control Check Sequence[1:0] */

} PHY_FCC_FCH_ACK_t;

/**
 * @brief  FCH bit fields for ACK/NACK frames for CENELEC bandplans.
 */
typedef struct
{
  uint8_t  FCS1;             /**< @brief MAC FCS[7:0] */
  uint8_t  Res1     : 7;     /**< @brief reserved bits  */
  uint8_t  SSCA     : 1;     /**< @brief Subsequent segment collision avoidance */
  uint8_t  FCS2;             /**< @brief MAC FCS[15:8]  */
  uint8_t  FccsL    : 4;     /**< @brief Frame Control Check Sequence, lowest bit (CRC5) */
  uint8_t  DT       : 3;     /**< @brief Delimiter type */
  uint8_t  Res2     : 1;     /**< @brief reserved bits  */
  uint8_t  FccsH    : 1;     /**< @brief Frame Control Check Sequence, highest bit (CRC5) */
  uint8_t           : 7;     /**< @brief Unused */

} PHY_CEN_FCH_ACK_t;

/**
 * @brief  ACK/NACK frame for CENELEC bandplans.
 */
typedef union
{
    PHY_FCC_FCH_ACK_t PHY_FCC_FCH_ACK; /**< @brief Definition of FCC bandplan FCH */
    PHY_CEN_FCH_ACK_t PHY_CEN_FCH_ACK; /**< @brief Definition of CENELEC bandplan FCH */
} PHY_FCH_ACK_t;

#define G3_PHY_PIBMAXVALUELEN       ((uint16_t)0x0120) /**< @brief Maximum size of attribute value for vectors (phyBkgnEstNorm1) */

/**
 * @brief  Attribute value structure (it can be 32-bit or array)
 */
typedef union
{
    uint32_t    Val;                           /**< @brief Attributes value for integer */
    uint8_t     ValVec[G3_PHY_PIBMAXVALUELEN]; /**< @brief Attributes value for vectors */
} PHY_AttrValueExt_t;

/**
 * @brief  Attribute info structure
 */
typedef struct
{
    uint32_t             ID;       /**< @brief Attributes ID */
    uint16_t             ValueLen; /**< @brief Length of the attributes value */
    PHY_AttrValueExt_t   Value;    /**< @brief Attributes value */
} PHY_AttributeInfo_t;

/**
  * @brief PHY Attribute value evaluation
  */
#define PHY_ATTRIBUTE_INFO_LEN(attribute_info) (sizeof(*(attribute_info)) - \
    sizeof((attribute_info)->Value) + (attribute_info)->ValueLen)

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif // HI_PHY_SUPPORT_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
