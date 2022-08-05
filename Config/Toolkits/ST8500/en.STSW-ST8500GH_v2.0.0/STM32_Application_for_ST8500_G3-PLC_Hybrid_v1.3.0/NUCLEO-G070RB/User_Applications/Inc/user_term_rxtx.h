/**
  ******************************************************************************
  * @file    user_term_rxtx.h
  * @author  AMG/IPC Application Team
  * @brief   Header file for Rx/Tx message management between User task and 
  *          Terminal port.
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
#ifndef USER_TERM_RXTX_H
#define USER_TERM_RXTX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/** @defgroup User_Term Terminal - User task communication management
  * @{
  */
  
/**
  * @}
  */  
  
/** @defgroup User_Term_Exported_Code Exported Code
  * @{
  */
  
/**
  * @}
  */  

/** @defgroup User_Term_Private_Code Private Code
  * @{
  */
  
/**
  * @}
  */    

/** @addtogroup User_App
  * @{
  */  
  
/** @addtogroup User_Term
  * @{  
  */
  
/** @addtogroup User_Term_Exported_Code
  * @{
  */
  
/* Exported constants -------------------------------------------------------*/
#define TERM_CMD_PAYLD_MAX_SIZE                                          (100U)    /*!< in bytes  */
//#define TERM_UT
#define TERM_TIMESTAMP  /* To display the timestamp on the serial terminal */

/* Exported macros ----------------------------------------------------------*/
#define USER_TERM_GET_COMMAND             \
                User_Term_UserTermHandler(USER_TERM_HANDLER_FIFO_SINGLEREAD)

/* Exported types -----------------------------------------------------------*/
typedef enum
{
  USER_TERM_CMD_RECEPT_NONE,
  USER_TERM_CMD_RECEPT_ONGOING,
  USER_TERM_CMD_STS_READY
} User_Term_CmdStatus_t;

typedef enum
{
  USER_TERM_DATA_TO_TX_NONE,
  USER_TERM_DATA_TO_TX_STARTED,
  USER_TERM_DATA_TO_TX_HANDLED
} User_Term_TxDataState_t;

typedef enum
{
  USER_TERM_HANDLER_FIFO_SINGLEREAD,
  USER_TERM_HANDLER_FIFO_MULTIREAD
} User_Term_HandlerMode_t;

typedef struct
{
  uint32_t Type;   /* RFU */
  uint32_t Length;
  uint8_t Payload[TERM_CMD_PAYLD_MAX_SIZE];
} User_Term_CmdBody_t;

typedef struct
{
  /* Term Rx side ctx */
  User_Term_CmdStatus_t CmdStatus;
  uint32_t CmdByteCnt;
  User_Term_CmdBody_t CmdBody;
  /* Term Tx side ctx */
  User_Term_TxDataState_t TxState;
} User_Term_Ctx_t;


/* Exported variables -------------------------------------------------------*/

/* Exported functions ------------------------------------------------ ------*/
void User_Term_Init(void);
void User_Term_UartRxFifoInit(void);
void User_Term_UsartIsr(void); 
User_Term_CmdBody_t * User_Term_UserTermHandler(User_Term_HandlerMode_t HandlerReadMode);
uint32_t User_Term_Printf(const char * format,...);
uint32_t User_Term_Printb(uint8_t * ByteBuffer, uint32_t Length);
bool User_Term_LookForTermByteCmd(uint8_t ByteChar);

#ifdef TERM_UT
void User_Term_Test(void);
#endif


/**
  * @}
  */

/**
  * @}
  */

  /**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* USER_TERM_RXTX_H */

/*********************** (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
