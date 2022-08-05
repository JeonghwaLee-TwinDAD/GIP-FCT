/**
  ******************************************************************************
  * @file    hi_mac_pib_types.h
  * @author  AMG/IPC Application Team
  * @brief   Header file with the Types used in Attribute and MAC management
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
#ifndef HI_MAC_PIB_TYPES_H
#define HI_MAC_PIB_TYPES_H

#include "hi_mac_constants.h"
#include "hi_phy_support.h"

/** @addtogroup HI_DRIVER ST8500 HI DRIVER
  * @{
  */

/** @defgroup hif_mac_drv HIF MAC DRV
  * @{
  */

/** @defgroup MAC_PIB_Attributes_TypesDefinitions HIF MAC PIB Types Definitions
  * @{
  */

/**
  * @brief  MAC Device Addressing Information
  */
typedef struct
{
    uint8_t      Mode;                      /**< @brief The Address Mode - 0: Address not present, 1: Reserved, 2: 16-bit Address present, 3: 64-bit Address present  */
    uint16_t     PanID;                     /**< @brief The Pan Identifier  */
    union
    {
        uint16_t Short;                     /**< @brief 16-bit Address */
        uint8_t  Extended[MAC_ADDR64_SIZE]; /**< @brief 64-bit Address */
    }; /**< @brief The address of the device (16 or 64 bit) */
} MAC_DeviceAddress_t;


/**
  * @brief  A key for secured communications
  */
typedef struct
{
    uint8_t FreeElement;           /**< @brief indicates if the entry is valid or not */
    uint8_t KeyData[MAC_KEY_SIZE]; /**< @brief The buffer in which the Key is stored */
} MAC_Key_t;

/**
  * @brief  The number of key contained in the KeyTable
  */
#define MAC_KEY_TBL_LEN ((uint8_t)2)

/**
  * @brief  A table containing keys required for secured communications
  */
typedef struct
{
    MAC_Key_t CurrentKey;   /**< @brief The Current Key */
    MAC_Key_t PrecedingKey; /**< @brief The Previous Key */
} MAC_KeyTable_t;




/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* HI_MAC_PIB_TYPES_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
