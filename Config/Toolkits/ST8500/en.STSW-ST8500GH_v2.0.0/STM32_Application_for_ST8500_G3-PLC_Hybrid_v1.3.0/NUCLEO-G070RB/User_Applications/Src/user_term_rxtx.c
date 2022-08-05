/**
  ******************************************************************************
  * @file    user_term_rxtx.c
  * @author  AMG/IPC Application Team
  * @brief   Handles the transmission of Rx and Tx message between User task
  *          and Terminal port. 
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
  
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "user_term_rxtx.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_ll_usart.h"
#include "usart.h"
#include "cmsis_os.h"
#include "task.h"


/** @addtogroup User_App
  * @{
  */

  /** @addtogroup User_Term
  * @{
  */
/* Exported variables -------------------------------------------------------*/
User_Term_Ctx_t User_Term;


 /** @addtogroup User_Term_Private_Code
  * @{
  */


/* Private constants --------------------------------------------------------*/
#define TERM_PRINTF_MAX_SIZE                                             (100U)    /*!< in nb of char elements */
#define TERM_RX_FIFO_SIZE                                                  (8U)    /*!< in nb of User_Term_CmdBody_t elements (beware: power of 2 value expected!) */
#define TERM_TX_FIFO_SIZE                                                (512U)    /*!< in nb of char elements (beware: power of 2 value expected! + value >> TERM_PRINTF_MAX_SIZE!) */
#define TERM_RX_FIFO_SIZ_MSK                           (TERM_RX_FIFO_SIZE - 1U)
#define TERM_TX_FIFO_SIZ_MSK                           (TERM_TX_FIFO_SIZE - 1U)
#define TERM_LF_ASCII_CODE                                               (0x0A)   /* Line Feed ('\n') */
#define TERM_CR_ASCII_CODE                                               (0x0D)   /* Carriage Return ('\r') */

#define UT_CMDTAB_SIZE                                                    (4U)   /* For test purpose */

/* Private macros -----------------------------------------------------------*/

/* Private types ------------------------------------------------------------*/
typedef enum
{
  USER_TERM_ERROR_NONE,
  USER_TERM_ERROR_DATAOVERFLOW
} User_Term_Error_t;

typedef struct
{
  uint32_t ReadIdx;
  uint32_t ReadIdx2;
  uint32_t WriteIdx;
  int32_t FreeSpace;
  User_Term_CmdBody_t Data[TERM_RX_FIFO_SIZE];
} User_Term_RxFifo_t;

typedef struct
{
  uint32_t ReadIdx;
  uint32_t WriteIdx;
  uint8_t Data[TERM_TX_FIFO_SIZE];
} User_Term_TxFifo_t;

/* Private variables ---------------------------------------------------------*/
static User_Term_RxFifo_t TermRxFifo;    /*!< Terminal Rx data circ. FIFO  */
static User_Term_TxFifo_t TermTxFifo;    /*!< Terminal Tx data circ. FIFO  */


#ifdef TERM_UT
/* Code for Term test (see User_Term_Test()) only! */
static User_Term_CmdBody_t UT_CmdTab[UT_CMDTAB_SIZE];
static volatile uint32_t UT_CmdTabReadIdx = 0U;
static volatile uint32_t UT_CmdTabWriteIdx = 0U;
#endif

/* Private functions --------------------------------------------------------*/
static inline void User_Term_UartRxHandler(uint8_t RxByte);
static inline void User_Term_UartTxHandler(uint8_t * TxByte);
static void User_Term_UartTxStart(void);
static inline User_Term_Error_t User_Term_BuildCommand(uint8_t RxByte);
static void User_Term_ResetCommand(void);
static void User_Term_ParseCommand(User_Term_CmdBody_t * TermCmd);

/**
* @}
*/

/** @addtogroup WBC_User_Term_Exported_Code
* @{
*/

/* Exported (public) function implementations --------------------------------*/

/**
  * @brief This function handles the User Terminal initialization. 
  * @note To be called once at the User init
  * @retval None
  */
void User_Term_Init(void)
{
  /* Init Terminal Rx part: */
  User_Term_UartRxFifoInit();

  /* Init Terminal Tx part: */
  TermTxFifo.ReadIdx = 0U;
  TermTxFifo.WriteIdx = 0U;
  memset(&TermTxFifo.Data, 0x0U, TERM_TX_FIFO_SIZE); /* reset TermTxFifo payload */
  User_Term.TxState = USER_TERM_DATA_TO_TX_NONE;
}


/**
  * @brief This function handles the UART Rx FIFO User Terminal initialization. 
  * @retval None
  */
void User_Term_UartRxFifoInit(void)
{
  /* Init Terminal Rx part: */
  TermRxFifo.ReadIdx = 0U;
  TermRxFifo.ReadIdx2 = 0U;
  TermRxFifo.WriteIdx = 0U;
  TermRxFifo.FreeSpace = (int32_t)TERM_RX_FIFO_SIZE;
  memset(&TermRxFifo.Data, 0x0U, TERM_RX_FIFO_SIZE*sizeof(User_Term_CmdBody_t)); /* reset TermRxFifo payload */
  User_Term_ResetCommand();
}


/**
  * @brief  This function handles Terminal Usart ISR
  * @retval None
  */
void User_Term_UsartIsr(void)
{

  if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE))
  {
    User_Term_UartRxHandler(LL_USART_ReceiveData8(USART2));
    __HAL_UART_CLEAR_OREFLAG(&huart2); /* Clear likely Overrun situation where uart data reception can be stuck */
  }

  if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC))
  {
    uint8_t tx_data;

    User_Term_UartTxHandler(&tx_data);
    if (User_Term.TxState == USER_TERM_DATA_TO_TX_HANDLED)
    {
      LL_USART_TransmitData8(USART2, tx_data);
    } 
    else
    {
      __HAL_UART_CLEAR_FLAG(&huart2, UART_CLEAR_TCF);
    }
  }
}


/**
  * @brief This function handles User terminal data fifo unstacking
  * @note Can be used in 2 modes: either reading of all unread data at once or a single data read.
  * @param HandlerReadMode: can be either FIFO_SINGLEREAD or FIFO_MULTIREAD (see User_Term_HandlerMode_t)
  * @param TermCurrentCmd: output handle pointing on read data (only useful in FIFO_SINGLEREAD mode)
  * @retval Pointer on current command received from terminal (NULL if read mode is FIFO_MULTIREAD)
  */
User_Term_CmdBody_t * User_Term_UserTermHandler(User_Term_HandlerMode_t HandlerReadMode)
{
  User_Term_CmdBody_t * term_cmd = NULL;

  if (TermRxFifo.FreeSpace < 0)
  {
    User_Term_Printf("\n\rTerminal RX fifo overflow detected!\n\r");
    TermRxFifo.FreeSpace = 0;
  }

  if (HandlerReadMode == USER_TERM_HANDLER_FIFO_SINGLEREAD)
  {
    if (TermRxFifo.ReadIdx != TermRxFifo.WriteIdx)
    {
      term_cmd = &TermRxFifo.Data[TermRxFifo.ReadIdx];
      TermRxFifo.ReadIdx = ((TermRxFifo.ReadIdx+1U) & TERM_RX_FIFO_SIZ_MSK);
      TermRxFifo.FreeSpace++;
    }
    else
    {
      TermRxFifo.FreeSpace = (int32_t)TERM_RX_FIFO_SIZE; /* all Rx data have been read: full space is available */
    }
  }
  else /* FIFO_MULTIREAD */
  {
    /* UART Rx fifo handling */
    while (TermRxFifo.ReadIdx != TermRxFifo.WriteIdx)
    {
      User_Term_ParseCommand(&(TermRxFifo.Data[TermRxFifo.ReadIdx])); /* read & parse each command */
      TermRxFifo.ReadIdx = ((TermRxFifo.ReadIdx+1U) & TERM_RX_FIFO_SIZ_MSK);
      TermRxFifo.FreeSpace++;
    }
    TermRxFifo.FreeSpace = (int32_t)TERM_RX_FIFO_SIZE; /* all Rx data have been read: full space is available */
  }
  return term_cmd;
}


/**
  * @brief This function prints out in terminal stings and values from its variable func parameters
  * @param format: classical "printf" input parameters (string(s) and value(s))
  * @retval Length of final string sent in UART Tx buffer
  */
uint32_t User_Term_Printf(const char * format,...)
{
  uint8_t buffer[TERM_PRINTF_MAX_SIZE];
  va_list args;
  uint32_t i, size;
  volatile int32_t delta_wr;

  va_start(args, format);
  size = vsprintf((char *)buffer, (const char*)format, args);
  va_end(args);

  if (size > 0U)
  {
    if (buffer[size-1U] == '\n')
    {
      buffer[size-1U] = '\r';
      buffer[size] = '\n';
      size++;
    }
    
    /* A "wait for TermTxFifo buffer space" management is implemented here to avoid TermTxFifo overflow (i.e. WriteIdx overtaking ReadIdx situation) */
    delta_wr = (int32_t)TermTxFifo.ReadIdx - ((int32_t)TermTxFifo.WriteIdx + 1); /* Note: do a +1 on writeIdx so that delta_wr is never 0  when ReadIdx=WriteIdx but rather = whole fifo space */
    if (delta_wr < 0)
    {
      delta_wr += (int32_t)TERM_TX_FIFO_SIZE;
    }
    while ((int32_t)size > delta_wr)
    {
      osDelay(1); /* Wait until enough space is made available in TermTxFifo */
      delta_wr = (int32_t)TermTxFifo.ReadIdx - ((int32_t)TermTxFifo.WriteIdx + 1);
      if (delta_wr < 0)
      {
        delta_wr += (int32_t)TERM_TX_FIFO_SIZE;
      }
    }
    
    /* Copy buffer into User terminal Tx fifo */
    /* Enter in a critical section here if needed (if, e.g., Printf is used outside User task: risks to get mixed Tx msg) */
    for (i=0U; i<size; i++)
    {
      TermTxFifo.Data[TermTxFifo.WriteIdx] = buffer[i];
      TermTxFifo.WriteIdx = ((TermTxFifo.WriteIdx+1U) & TERM_TX_FIFO_SIZ_MSK);
    }
    /* Leave critical section here if needed */
    
    /* If needed (i.e. no transfer on-going yet), initiate an UART Tx */
    User_Term_UartTxStart();
  }

  return(size);
}


/**
  * @brief This function prints out in terminal a number of data bytes indicated in its parameter
  * @param pByteBuffer: pointer on data payload to send
  * @param Length: data payload size 
  * @retval Length of final string sent in UART Tx buffer
  */
uint32_t User_Term_Printb(uint8_t * pByteBuffer, uint32_t Length)
{
  uint32_t i;
  volatile int32_t delta_wr;

  if (Length > 0U)
  {

    /* A "wait for TermTxFifo buffer space" management is implemented here to avoid TermTxFifo overflow (i.e. WriteIdx overtakes ReadIdx) */
    delta_wr = (int32_t)TermTxFifo.ReadIdx - ((int32_t)TermTxFifo.WriteIdx + 1); /* Note: do a +1 on writeIdx so that delta_wr is never 0  when ReadIdx=WriteIdx but rather = whole fifo space */
    if (delta_wr < 0)
    {
      delta_wr += (int32_t)TERM_TX_FIFO_SIZE;
    }
    while ((int32_t)Length > delta_wr)
    {
      osDelay(1); /* Wait until enough space is made available in TermTxFifo */
      delta_wr = (int32_t)TermTxFifo.ReadIdx - ((int32_t)TermTxFifo.WriteIdx + 1);
      if (delta_wr < 0)
      {
        delta_wr += (int32_t)TERM_TX_FIFO_SIZE;
      }
    }
    
    /* Copy buffer into User terminal Tx fifo */
    /* Enter in a critical section here if needed (if, e.g., Printf is used outside User task: risks to get mixed Tx msg) */
    for (i=0U; i<Length; i++)
    {
      TermTxFifo.Data[TermTxFifo.WriteIdx] = *(pByteBuffer+i);
      TermTxFifo.WriteIdx = ((TermTxFifo.WriteIdx+1U) & TERM_TX_FIFO_SIZ_MSK);
    }
    /* Leave critical section here if needed */
    
    /* If needed (i.e. no transfer on-going yet), initiate an UART Tx */
    User_Term_UartTxStart();
  }

  return(Length);
}


/**
  * @brief This function looks for a specific char at start data byte in UART Rx fifo payloads
  * @param ByteChar: byte data char to look-up
  * @retval True: char found
  */
bool User_Term_LookForTermByteCmd(uint8_t ByteChar)
{
  bool ret_val = false;

  while (TermRxFifo.ReadIdx2 != TermRxFifo.WriteIdx)
  {
    if (TermRxFifo.Data[TermRxFifo.ReadIdx2].Payload[0U] == ByteChar)
    {
      ret_val = true;
    }    
    TermRxFifo.ReadIdx2 = ((TermRxFifo.ReadIdx2+1U) & TERM_RX_FIFO_SIZ_MSK);

    if (ret_val)
    {
      break;
    } 
  }

  return ret_val;
}


#ifdef TERM_UT
void User_Term_Test(void)
{
  static uint32_t dbg_step = 0U;
  uint32_t i;

  /* Handle Terminal message exchanges */
  User_Term_UserTermHandler(USER_TERM_HANDLER_FIFO_MULTIREAD, NULL);

  switch (dbg_step)
  {
  case 0U:
    User_Term_Printf("\n\r");
    User_Term_Printf(">>> STM32 User Terminal test <<<\n\r");
    User_Term_Printf("To select Term full TxFifo test, press 0 then ENTER\n\r");
    User_Term_Printf("To select Term string mirror test, press 1 then ENTER\n\r");
    User_Term_Printf("To end Term test, press q then ENTER\n\r");
    dbg_step = 1U;
    break;

  case 1U:
    while (UT_CmdTabReadIdx != UT_CmdTabWriteIdx)
    {
      /* Parse UT_CmdTab: */
      if ((UT_CmdTab[UT_CmdTabReadIdx].Payload[0U] == '0') && ((UT_CmdTab[UT_CmdTabReadIdx].Payload[1U] == '\r')||(UT_CmdTab[UT_CmdTabReadIdx].Payload[1U] == '\n')))
      {
        dbg_step = 20U;
      }
      else if ((UT_CmdTab[UT_CmdTabReadIdx].Payload[0U] == '1') && ((UT_CmdTab[UT_CmdTabReadIdx].Payload[1U] == '\r')||(UT_CmdTab[UT_CmdTabReadIdx].Payload[1U] == '\n')))
      {
        dbg_step = 30U;
      }
      else if ((UT_CmdTab[UT_CmdTabReadIdx].Payload[0U] == 'q') && ((UT_CmdTab[UT_CmdTabReadIdx].Payload[1U] == '\r')||(UT_CmdTab[UT_CmdTabReadIdx].Payload[1U] == '\n')))
      {
        dbg_step = 254U;
      }
      else
      {}

      UT_CmdTabReadIdx = (UT_CmdTabReadIdx + 1U) % UT_CMDTAB_SIZE;

      if (dbg_step != 1U)
      {
        break;
      }

      if (UT_CmdTabReadIdx == UT_CmdTabWriteIdx) /* all msgs have been parsed... */
      {
        User_Term_Printf("No suitable answer found! Type again (choice then ENTER)...\n\r");
      }
    }
    break;

  case 20U:
    /* Exec Test 0: */
    for(i=10U; i<30U; i++)
    {
      User_Term_Printf("%d EVLKST8500GH Term test 0 $#\n\r", i);
    }
    dbg_step = 0U;
    break;

  case 30U:
    User_Term_Printf("\n\r");
    User_Term_Printf("Reminder: Term 'string+ENTER' accepted max length is %d\n\r", TERM_CMD_PAYLD_MAX_SIZE);
    User_Term_Printf("To quit this test, press q then ENTER\n\r");
    dbg_step = 31U;
    break;

  case 31U:
    while (UT_CmdTabReadIdx != UT_CmdTabWriteIdx)
    {
      if ((UT_CmdTab[UT_CmdTabReadIdx].Payload[0U] == 'q') && ((UT_CmdTab[UT_CmdTabReadIdx].Payload[1U] == '\r')||(UT_CmdTab[UT_CmdTabReadIdx].Payload[1U] == '\n')))
      {
        dbg_step = 0U;
      }
      else
      {
        User_Term_Printb(UT_CmdTab[UT_CmdTabReadIdx].Payload, UT_CmdTab[UT_CmdTabReadIdx].Length);
        User_Term_Printf("\n\r");
      }
      UT_CmdTabReadIdx = (UT_CmdTabReadIdx + 1U) % UT_CMDTAB_SIZE;
      if (dbg_step != 31U)
      {
        break;
      }
    }
    break;

  case 254U:
    User_Term_Printf("Bye! To enter Term test again, press t then ENTER...\n\r");
    dbg_step = 255U;

  case 255U:
    while (UT_CmdTabReadIdx != UT_CmdTabWriteIdx)
    {
      /* Parse UT_CmdTab: */
      if ((UT_CmdTab[UT_CmdTabReadIdx].Payload[0U] == 't') && ((UT_CmdTab[UT_CmdTabReadIdx].Payload[1U] == '\r')||(UT_CmdTab[UT_CmdTabReadIdx].Payload[1U] == '\n')))
      {
        dbg_step = 0U;
      }

      UT_CmdTabReadIdx = (UT_CmdTabReadIdx + 1U) % UT_CMDTAB_SIZE;

      if (dbg_step != 255U)
      {
        break;
      }

      if (UT_CmdTabReadIdx == UT_CmdTabWriteIdx) /* all msgs have been parsed... */
      {
        User_Term_Printf("No suitable answer found! Type again (choice then ENTER)...\n\r");
      }
    }
    break;

    break;
  default:
    break;
  }
}
#endif /* TERM_UT */



/**
* @}
*/

/** @addtogroup User_Term_Private_Code
* @{
*/


/**
  * @brief This function handles bytes received from UART Rx
  * @note Bytes are packetized to retrieve command messages from terminal.
  * @param RxByte: data byte received from UART
  * @retval None
  */
static inline void User_Term_UartRxHandler(uint8_t RxByte)
{
  User_Term_Error_t error;

  error = User_Term_BuildCommand(RxByte);

  if (User_Term.CmdStatus == USER_TERM_CMD_STS_READY)
  {
    /* Update circular RX buffer with newly received cmd */
    memcpy(&(TermRxFifo.Data[TermRxFifo.WriteIdx]), &(User_Term.CmdBody), sizeof(User_Term_CmdBody_t));
    TermRxFifo.WriteIdx = ((TermRxFifo.WriteIdx+1U) & TERM_RX_FIFO_SIZ_MSK);
    TermRxFifo.FreeSpace--;
    User_Term_ResetCommand();
    /* In case an overflow has been detected, replay BuildCommand() using byte that  
       produced the overflow so that it can be part of next msg (otherwise it will be lost) */ 
    if (error == USER_TERM_ERROR_DATAOVERFLOW)
    {
      User_Term_BuildCommand(RxByte);
    }
  }
}


/**
  * @brief This function handles bytes sending on UART Tx
  * @param pTxByte: data byte to be sent over UART
  * @retval None
  */
static inline void User_Term_UartTxHandler(uint8_t * pTxByte)
{

  if (TermTxFifo.ReadIdx == TermTxFifo.WriteIdx)
  {
    /* No more byte transmit */
    User_Term.TxState = USER_TERM_DATA_TO_TX_NONE;
  }
  else
  {
    /* Output byte to transmit */
    *pTxByte = TermTxFifo.Data[TermTxFifo.ReadIdx];
    TermTxFifo.ReadIdx = ((TermTxFifo.ReadIdx+1U) & TERM_TX_FIFO_SIZ_MSK);

    User_Term.TxState = USER_TERM_DATA_TO_TX_HANDLED;
  }
}


/**
  * @brief This function initiates a data byte transfer over UART Tx
  * @note This function send first byte, then the rest is sent within UART Tx ISR.
  * @retval None
  */
static void User_Term_UartTxStart(void)
{
  if (User_Term.TxState == USER_TERM_DATA_TO_TX_NONE)
  {
    uint8_t first_byte;

    first_byte = TermTxFifo.Data[TermTxFifo.ReadIdx];
    TermTxFifo.ReadIdx = ((TermTxFifo.ReadIdx+1U) & TERM_TX_FIFO_SIZ_MSK);

    User_Term.TxState = USER_TERM_DATA_TO_TX_STARTED;

    /* Finish function by writing byte on UART. Ensures all previous actions in func implem  
       are well executed prior to sending over UART which is done under high prio ISR  */
    LL_USART_TransmitData8(USART2, first_byte);
  }
}


/**
  * @brief This function creates command messages from bytes received from UART Rx
  * @note A CR or a LF char reception marks the end of the message.
  * @param RxByte: data byte to add to command payload
  * @retval User_Term_Error_t: no error or data overflow detected
  */
static inline User_Term_Error_t User_Term_BuildCommand(uint8_t RxByte)
{
  User_Term_Error_t error = USER_TERM_ERROR_NONE;

  User_Term.CmdBody.Payload[User_Term.CmdByteCnt] = RxByte;
  User_Term.CmdByteCnt++;

  if (User_Term.CmdByteCnt > 0U)
  {
    User_Term.CmdStatus = USER_TERM_CMD_RECEPT_ONGOING;
  }

  /* Parse Cmd end conditions (i.e. detect the CR or LF char) */
  if ((RxByte == TERM_CR_ASCII_CODE) || (RxByte == TERM_LF_ASCII_CODE))
  {
    User_Term.CmdBody.Length = User_Term.CmdByteCnt;
    User_Term.CmdStatus = USER_TERM_CMD_STS_READY;
  }
  else if (User_Term.CmdByteCnt >= TERM_CMD_PAYLD_MAX_SIZE)
  {
    /* Terminal Cmd payload reached its max size: raise overflow situation, 
       auto terminate message (force CR insertion) and make it available   */
    error = USER_TERM_ERROR_DATAOVERFLOW;
    User_Term.CmdBody.Payload[TERM_CMD_PAYLD_MAX_SIZE-1U] = TERM_CR_ASCII_CODE;
    User_Term.CmdBody.Length = TERM_CMD_PAYLD_MAX_SIZE;
    User_Term.CmdStatus = USER_TERM_CMD_STS_READY;
  }
  else {}

  return error;
}


/**
  * @brief This function initializes Rx message structure
  * @retval None
  */
static void User_Term_ResetCommand(void)
{
  User_Term.CmdStatus = USER_TERM_CMD_RECEPT_NONE;
  User_Term.CmdByteCnt = 0U;
  User_Term.CmdBody.Type = 0xFFU;
  User_Term.CmdBody.Length = 0U;
  memset(&(User_Term.CmdBody.Payload), 0U, TERM_CMD_PAYLD_MAX_SIZE);
}


/**
  * @brief This function pre-parses buffered received messages from UART Rx
  * @note It is an optional function.
  * @param pTermCmd: pointer on message
  * @retval None
  */
static void User_Term_ParseCommand(User_Term_CmdBody_t * pTermCmd)
{
#ifdef TERM_UT
  /* Code for Term test (see User_Term_Test()) only! */
  memcpy(&(UT_CmdTab[UT_CmdTabWriteIdx]), pTermCmd, sizeof(User_Term_CmdBody_t));
  UT_CmdTabWriteIdx++;
  if (UT_CmdTabWriteIdx >= UT_CMDTAB_SIZE)
  {
    UT_CmdTabWriteIdx = 0U;
  }
#else 
  /* If needed, a parsing of each Terminal command can be implemented here */
#endif
}




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
