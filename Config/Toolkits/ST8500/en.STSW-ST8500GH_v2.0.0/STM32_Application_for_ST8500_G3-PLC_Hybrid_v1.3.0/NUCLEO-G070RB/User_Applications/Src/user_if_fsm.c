/**
  ******************************************************************************
  * @file    user_if_fsm.c
  * @author  AMG/IPC Application Team
  * @brief   Handles the User interface FSM. 
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
#include "cmsis_os.h"
#include "main.h"
#include "user_if_fsm.h"
#include "user_term_rxtx.h"
#include "user_g3_common.h"
#include "g3_app_config.h"
#include "stm32g0xx_hal.h"
#include "hi_msgs_impl.h"
#include "hi_g3lib_attributes.h"
    

//#define ONE_TIME_TEST


/** @addtogroup User_App
  * @{
  */

  /** @addtogroup User_If_Fsm
  * @{
  */
/* Exported variables -------------------------------------------------------*/

 /** @addtogroup User_If_Fsm_Private_Code
  * @{
  */


/* Private constants --------------------------------------------------------*/
#define ESC_ASCII_CODE                                                   (0x1B)
#define ASCII_OFFSET                                                     (0x30)
#define TIMEOUT_DELAY                                                  (10000U)

/* Private macros -----------------------------------------------------------*/
#define PARSE_CMD_CHAR(cHAR)                                                 \
    ( (rcv_command != NULL) && (rcv_command->Payload[0U] == cHAR) &&         \
     ((rcv_command->Payload[1U] == '\r') || (rcv_command->Payload[1U] == '\n')) )

#define PARSE_CMD_ANY_CHAR           ((rcv_command != NULL) ? true : false)

#define USEREVT_JUST_RISEN(eVENT)                                            \
                              ((User_Fsm.localUserEvents & eVENT) == eVENT)
#define USEREVT_RISEN(eVENT)                                                 \
                              ((User_Fsm.UserEventsKeep & eVENT) == eVENT)
#define USEREVT_FLUSH(eVENT)  (User_Fsm.UserEventsKeep &= ~(eVENT))

#ifdef TERM_TIMESTAMP
#define PRINT(format, args...)                                               \
                               UserIf_Fsm_PrintTimestamp();                  \
                               User_Term_Printf(format, ## args);
#else
#define PRINT(format, args...)                                               \
                               User_Term_Printf(format, ## args);
#endif


/* Private types ------------------------------------------------------------*/
typedef enum
{ 
  USERIF_FSM_TERMENTRY_STATE_DISPMENU,                      /*!< User i/f FSM state for terminal entry menu display */
  USERIF_FSM_TERMENTRY_STATE_PROCSEL,                       /*!< User i/f FSM state for terminal entry processing */
  USERIF_FSM_PLATFORMINFO_STATE_DISPMENU,                   /*!< User i/f FSM state for getting board info menu display */
  USERIF_FSM_PLATFORMINFO_STATE_PROCSEL,                    /*!< User i/f FSM state for getting board info processing */
  USERIF_FSM_BOARDCFG_STATE_DISPMENU,                       /*!< User i/f FSM state for board configuration menu display */
  USERIF_FSM_BOARDCFG_STATE_PROCSEL,                        /*!< User i/f FSM state for processing of board configuration (from user's inputs from menu) */
  USERIF_FSM_TESTENTRY_STATE_DISPMENU,                      /*!< User i/f FSM state for test main menu display */
  USERIF_FSM_TESTENTRY_STATE_PROCSEL,                       /*!< User i/f FSM state for processing of user choice from test main menu  */
  USERIF_FSM_UDPTESTS_STATE_DISPMENU,                       /*!< User i/f FSM state for UDP tests menu display */
  USERIF_FSM_UDPTESTS_STATE_PROCSEL,                        /*!< User i/f FSM state for processing of user choice from UDP tests menu  */
  USERIF_FSM_UDPTEST_SINGLE_DATAREQ_STATE_DISPMENU,         /*!< User i/f FSM state for UDP test 1 menu display */
  USERIF_FSM_UDPTEST_SINGLE_DATAREQ_STATE_PROCSEL,          /*!< User i/f FSM state for processing of UDP test 1  */
  USERIF_FSM_UDPTEST_MULTIPLE_BASIC_DATAREQ_STATE_DISPMENU, /*!< User i/f FSM state for UDP test 2 menu display */
  USERIF_FSM_UDPTEST_MULTIPLE_BASIC_DATAREQ_STATE_PROCSEL,  /*!< User i/f FSM state for processing of UDP test 2  */
  USERIF_FSM_UDPTEST_LOOPBK_DATAREQ_STATE_DISPMENU,         /*!< User i/f FSM state for UDP test 3 menu display */
  USERIF_FSM_UDPTEST_LOOPBK_DATAREQ_STATE_PROCSEL,          /*!< User i/f FSM state for processing of UDP test 3  */
} UserIf_Fsm_State_t;

typedef enum
{
  USERIF_FSM_TESTSTEP_0,
  USERIF_FSM_TESTSTEP_1,
  USERIF_FSM_TESTSTEP_2,
  USERIF_FSM_TESTSTEP_3,
  USERIF_FSM_TESTSTEP_4,
  USERIF_FSM_TESTSTEP_5,
} UserIf_Fsm_TestStep_t;

typedef struct
{
  UserIf_Fsm_State_t CurState;
  UserIf_Fsm_TestStep_t TestStep;
  UserIf_Fsm_TestStep_t TestSubStep;
  uint32_t localUserEvents;
  uint32_t UserEventsKeep;
  uint32_t TimestampT0;  /* in ms */
  uint32_t UdpTransferTimeoutT0;  /* in ms */
} UserIf_Fsm_Ctx_t;

typedef enum
{
  USERIF_FSM_DISPMENU,
  USERIF_FSM_PROCSEL
} UserIf_Fsm_StateAction_t;

typedef struct
{
  char * pString;
  uint32_t isToBeDisplayed;
} UserIf_Fsm_DisplayedEvent_t;

typedef struct
{
  uint32_t Required_Nb_Tests_Orig_Side;
  uint32_t Required_Nb_Tests_Rcv_Side;
  uint32_t Test_Counter_Orig_Side;
  uint32_t Test_Counter_Rcv_Side;
  uint32_t Rcv_Command_Length;
  uint8_t  Rcv_Command_Payload[TERM_CMD_PAYLD_MAX_SIZE]; 
} UserIf_Fsm_MultipleBasicTestsInfo_t;


/* Private variables ---------------------------------------------------------*/
static const char * pString_NoSuitableAnswerFound = "Invalid entry! Please retry...\n\r"; 
static const char * pString_CR = "\n\r";  /* Carriage Return string */
#ifdef ONE_TIME_TEST
static const char * pString_TestIsFinished = "Test is finished!\n\r";
#else
static const char * pString_PlayTestOnceMore = "Play test once more or press ESC. key then ENTER to go back to main menu\n\r";
#endif
static const UserIf_Fsm_DisplayedEvent_t UserIf_Fsm_EventsToDisplay[USEREVT_LAST_POSITION+1U] =
{
/*  pString:                  isToBeDisplayed:    Bit position: */    
  {"G3BOOT-DEV-PANSORT.Ind",                1U},           /* 0 */
  {"G3BOOT-SRV-JOIN.Ind",                   1U},           /* 1 */
  {"G3BOOT-DEV-START.Cnf",                  1U},           /* 2 */
  {"G3ICMP-ECHO.Ind",                       0U},           /* 3 */
  {"G3UDP-CONN-SET.Cnf",                    0U},           /* 4 */
  {"G3UDP-DATA.Cnf",                        1U},           /* 5 */
  {"G3UDP-DATA.Ind",                        1U},           /* 6 */
  {"G3BOOT-DEV-LEAVE.Cnf",                  1U},           /* 7 */
  {"G3BOOT-SRV-STOP.Cnf",                   1U},           /* 8 */
  {"HOSTIF-DBGTOOL.Cnf",                    0U},           /* 9 */
};

static UserIf_Fsm_Ctx_t User_Fsm;
UserIf_Fsm_MultipleBasicTestsInfo_t UserIf_Fsm_MultipleBasicTestsInfo;

/* Private functions --------------------------------------------------------*/
static void UserIf_Fsm_ParseUserEvents(void);
static void UserIf_Fsm_ParseTerminalSpecificEvents(void);
static void UserIf_Fsm_TermEntryState(UserIf_Fsm_StateAction_t FuncAction);
static void UserIf_Fsm_GetPlatformInfoState(UserIf_Fsm_StateAction_t FuncAction);
static void UserIf_Fsm_BoardCfgState(UserIf_Fsm_StateAction_t FuncAction);
static void UserIf_Fsm_TestEntryState(UserIf_Fsm_StateAction_t FuncAction);
static void UserIf_Fsm_UdpTestsState(UserIf_Fsm_StateAction_t FuncAction);
static void UserIf_Fsm_UdpTestSingleDataReqState(UserIf_Fsm_StateAction_t FuncAction);
static inline void UserIf_Fsm_UdpTestSingleDataReqOriginatorExec(void);
static inline void UserIf_Fsm_UdpTestSingleDataReqRecipientExec(void);
static void UserIf_Fsm_UdpTestLoopbackDataReqState(UserIf_Fsm_StateAction_t FuncAction);
static inline void UserIf_Fsm_UdpTestLoopbackDataReqOriginatorExec(void);
static inline void UserIf_Fsm_UdpTestLoopbackDataReqLooperExec(void);
static void UserIf_Fsm_ResetToState(UserIf_Fsm_State_t StateToResetTo);
static void UserIf_Fsm_PrintCR(uint32_t CrNb);
static void UserIf_Fsm_PrintWaitingForMsg(uint32_t MessageIdx);
static void UserIf_Fsm_UdpTestMultipleBasicDataReqState(UserIf_Fsm_StateAction_t FuncAction);
static inline void UserIf_Fsm_UdpTestMultipleBasicDataReqOriginatorExec(void);
static inline void UserIf_Fsm_UdpTestMultipleBasicDataReqRecipientExec(void);
static uint32_t UserIf_Fsm_ReadChar(User_Term_CmdBody_t * RcvCommand);
#ifdef TERM_TIMESTAMP
static void UserIf_Fsm_PrintTimestamp(void);
#endif
static void UserIf_Fsm_PrintPlatformInfo(void);
static uint32_t UserIf_Fsm_EstimateDelay(uint32_t T0Time);

/**
* @}
*/

/** @addtogroup WBC_User_If_Fsm_Exported_Code
* @{
*/

/* Exported (public) function implementations --------------------------------*/

/**
  * @brief This function handles User interface FSM initialization
  * @note To be called once at the User init.
  * @retval None
  */
void UserIf_Fsm_Init(void)
{
  UserIf_Fsm_ResetToState(USERIF_FSM_TERMENTRY_STATE_DISPMENU);
}


/**
  * @brief This function implements User interface FSM
  * @note Entry function called from User task infinite execution loop.
  * @retval None
  */
void UserIf_Fsm_FsmBody(void)
{
  UserIf_Fsm_ParseUserEvents();

  UserIf_Fsm_ParseTerminalSpecificEvents();

  switch (User_Fsm.CurState)
  {
    case USERIF_FSM_TERMENTRY_STATE_DISPMENU:
      UserIf_Fsm_TermEntryState(USERIF_FSM_DISPMENU);
      break;
    case USERIF_FSM_TERMENTRY_STATE_PROCSEL:
      UserIf_Fsm_TermEntryState(USERIF_FSM_PROCSEL);
      break;

    case USERIF_FSM_PLATFORMINFO_STATE_DISPMENU:
      UserIf_Fsm_GetPlatformInfoState(USERIF_FSM_DISPMENU);
      break;
    case USERIF_FSM_PLATFORMINFO_STATE_PROCSEL:
      UserIf_Fsm_GetPlatformInfoState(USERIF_FSM_PROCSEL);
      break;

    case USERIF_FSM_BOARDCFG_STATE_DISPMENU:
      UserIf_Fsm_BoardCfgState(USERIF_FSM_DISPMENU);
      break;
    case USERIF_FSM_BOARDCFG_STATE_PROCSEL:
      UserIf_Fsm_BoardCfgState(USERIF_FSM_PROCSEL);
      break;

    case USERIF_FSM_TESTENTRY_STATE_DISPMENU:
      UserIf_Fsm_TestEntryState(USERIF_FSM_DISPMENU);
      break;
    case USERIF_FSM_TESTENTRY_STATE_PROCSEL:
      UserIf_Fsm_TestEntryState(USERIF_FSM_PROCSEL);
      break;

    case USERIF_FSM_UDPTESTS_STATE_DISPMENU:
      UserIf_Fsm_UdpTestsState(USERIF_FSM_DISPMENU);
      break;
    case USERIF_FSM_UDPTESTS_STATE_PROCSEL:
      UserIf_Fsm_UdpTestsState(USERIF_FSM_PROCSEL);
      break;

    case USERIF_FSM_UDPTEST_SINGLE_DATAREQ_STATE_DISPMENU:
      UserIf_Fsm_UdpTestSingleDataReqState(USERIF_FSM_DISPMENU);
      break;
    case USERIF_FSM_UDPTEST_SINGLE_DATAREQ_STATE_PROCSEL:
      UserIf_Fsm_UdpTestSingleDataReqState(USERIF_FSM_PROCSEL);
      break;

    case USERIF_FSM_UDPTEST_MULTIPLE_BASIC_DATAREQ_STATE_DISPMENU:
      UserIf_Fsm_UdpTestMultipleBasicDataReqState(USERIF_FSM_DISPMENU);
      break;

    case USERIF_FSM_UDPTEST_MULTIPLE_BASIC_DATAREQ_STATE_PROCSEL:
      UserIf_Fsm_UdpTestMultipleBasicDataReqState(USERIF_FSM_PROCSEL);
      break;      
      
    case USERIF_FSM_UDPTEST_LOOPBK_DATAREQ_STATE_DISPMENU:
      UserIf_Fsm_UdpTestLoopbackDataReqState(USERIF_FSM_DISPMENU);
      break;
      
    case USERIF_FSM_UDPTEST_LOOPBK_DATAREQ_STATE_PROCSEL:
      UserIf_Fsm_UdpTestLoopbackDataReqState(USERIF_FSM_PROCSEL);
      break;

    default:
      break;
  }

}





/**
* @}
*/

/** @addtogroup User_If_Fsm_Private_Code
* @{
*/


/**
  * @brief This function parses the events upcoming from user G3 common interface
  * @retval None
  */
static void UserIf_Fsm_ParseUserEvents(void)
{
  uint32_t i;

  User_Fsm.localUserEvents = UserG3_CommonEvt.UserEvents; /* Locally store instant event bitmap so that it can be used meanwhile UserTask can update it again */
  User_Fsm.UserEventsKeep |= UserG3_CommonEvt.UserEvents; /* Store user events for likely asynchronous (delayed) use of risen events in some tests */
  UserG3_CommonEvt.UserEvents = 0U; /* Read-clear user events */

  /* Manage display of User Task events */
  for (i=0U; i<(USEREVT_LAST_POSITION+1U); i++)
  {
    if ( ((User_Fsm.localUserEvents & (0x1UL << i)) == (0x1UL << i)) && (UserIf_Fsm_EventsToDisplay[i].isToBeDisplayed != 0U))
    {
      PRINT("%s received!\n\r", UserIf_Fsm_EventsToDisplay[i].pString);
    }
  }
}


/**
  * @brief This function parses specific events coming from user terminal interface
  * @retval None
  */
static void UserIf_Fsm_ParseTerminalSpecificEvents(void)
{
  if(User_Term_LookForTermByteCmd(ESC_ASCII_CODE))
  {
    UserIf_Fsm_ResetToState(USERIF_FSM_TESTENTRY_STATE_DISPMENU);
  }
}


/**
  * @brief User i/f FSM implementation for the terminal entry menu and related actions state
  * @param FuncAction: type of action from UserIf_Fsm_StateAction_t to run for function.
  * @retval None
  */
static void UserIf_Fsm_TermEntryState(UserIf_Fsm_StateAction_t FuncAction)
{
  if (FuncAction == USERIF_FSM_DISPMENU)
  {
    UserIf_Fsm_PrintCR(3U);
#ifdef DEVICE_TYPE_COORD
    PRINT("[COORD.]\n\r");
#else /* DEVICE_TYPE_DEVICE */
    PRINT("[DEVICE]\n\r");
#endif
    
#ifdef RF_IS_868
    PRINT("[RF TYPE: 868MHz]\n\r");
#else /* RF_IS_915 */
    PRINT("[RF TYPE: 915MHz]\n\r");
#endif
    
    PRINT(pString_CR);
    PRINT("> EVLKST8500GH STM32 User Terminal <\n\r");
    UserIf_Fsm_PrintCR(2U);
#ifdef DEVICE_TYPE_COORD
    UserIf_Fsm_PrintWaitingForMsg(USEREVT_G3_BOOTSERVERJOININD_RCVED_Pos);  /* Waiting for G3BOOT-SRV-JOIN.Ind message... */
#else /* DEVICE_TYPE_DEVICE */
    UserIf_Fsm_PrintWaitingForMsg(USEREVT_G3_BOOTDEVICESTARTCNF_RCVED_Pos);  /* Waiting for G3BOOT-DEV-START.Cnf message... */
#endif
    User_Fsm.CurState = USERIF_FSM_TERMENTRY_STATE_PROCSEL;
  }
  else if (FuncAction == USERIF_FSM_PROCSEL)
  {
    if (USEREVT_JUST_RISEN(USEREVT_G3_BOOTDEVICESTARTCNF_RCVED) || USEREVT_JUST_RISEN(USEREVT_G3_BOOTSERVERJOININD_RCVED))
    {
      User_Fsm.CurState = USERIF_FSM_PLATFORMINFO_STATE_DISPMENU;
    }
  }
  else {}
}


/**
  * @brief User i/f FSM implementation getting STM32 + ST8500 platform information
  * @param FuncAction: type of action from UserIf_Fsm_StateAction_t to run for function.
  * @retval None
  */
static void UserIf_Fsm_GetPlatformInfoState(UserIf_Fsm_StateAction_t FuncAction)
{
  if (FuncAction == USERIF_FSM_DISPMENU)
  {
    User_Term_Printf(pString_CR);
    UserG3_SystemInfoRequest(); /* Ask for platform information */
    PRINT("Waiting for Platform Info message...\n\r");
    User_Fsm.CurState = USERIF_FSM_PLATFORMINFO_STATE_PROCSEL;
  }
  else if (FuncAction == USERIF_FSM_PROCSEL)
  {
    if (USEREVT_RISEN(USEREVT_G3_SYSTEMINFO_RCVED))
    {
      UserIf_Fsm_PrintPlatformInfo();
      User_Fsm.CurState = USERIF_FSM_TESTENTRY_STATE_DISPMENU;
    }
  }
  else {}
}


/**
  * @brief User i/f FSM implementation for the board configuration step
  * @note FSM step function to be called if necessary.
  * @param FuncAction: type of action from UserIf_Fsm_StateAction_t to run for function.
  * @retval None
  */
/* Note: BOARD_CFG_STATE bypassed for the moment (could be activated when the User 
         will need to (re)config the board via terminal (Device/Coord., RF/PLC,...) */
static void UserIf_Fsm_BoardCfgState(UserIf_Fsm_StateAction_t FuncAction)
{
  User_Term_CmdBody_t * rcv_command = NULL;

  if (FuncAction == USERIF_FSM_DISPMENU)
  {
    PRINT(pString_CR);
    PRINT("> EVLKST8500GH board config: <\n\r");
    PRINT("  To select board config Aaa, press 0 then ENTER\n\r");
    PRINT("  To select board config Bbb, press 1 then ENTER\n\r");
    User_Fsm.CurState = USERIF_FSM_BOARDCFG_STATE_PROCSEL;
  }

  else if (FuncAction == USERIF_FSM_PROCSEL)
  {
    rcv_command = USER_TERM_GET_COMMAND;

    /* Parse received command: */
    if (PARSE_CMD_CHAR('0'))
    {
      /* Process likely board config Aaa here */
      User_Fsm.CurState = USERIF_FSM_TESTENTRY_STATE_DISPMENU;
    }
    else if (PARSE_CMD_CHAR('1'))
    {
      /* Process likely board config Aaa here */
      User_Fsm.CurState = USERIF_FSM_TESTENTRY_STATE_DISPMENU;
    }
    else if (PARSE_CMD_ANY_CHAR)
    {
      PRINT(pString_NoSuitableAnswerFound);
    }
    else {}
  }
  else {}
}


/**
  * @brief User i/f FSM implementation for the terminal entry menu step
  * @param FuncAction: type of action from UserIf_Fsm_StateAction_t to run for function.
  * @retval None
  */
static void UserIf_Fsm_TestEntryState(UserIf_Fsm_StateAction_t FuncAction)
{
  User_Term_CmdBody_t * rcv_command = NULL;

  if (FuncAction == USERIF_FSM_DISPMENU)
  {
    PRINT(pString_CR);
    PRINT("> EVLKST8500GH main test menu: <\n\r");
    PRINT("  (Press ESC. key then ENTER at any time to go back to this menu)\n\r");
    PRINT("  To select UDP tests, press 0 then ENTER\n\r");
    //PRINT("  To select Xxxx tests, press 1 then ENTER\n\r");
    /* Append other test to EVLKST8500GH test menu here... */
    User_Fsm.CurState = USERIF_FSM_TESTENTRY_STATE_PROCSEL;
  }

  else if (FuncAction == USERIF_FSM_PROCSEL)
  {
    rcv_command = USER_TERM_GET_COMMAND;

    /* Parse received command: */
    if (PARSE_CMD_CHAR('0'))
    {
      User_Fsm.CurState = USERIF_FSM_UDPTESTS_STATE_DISPMENU;
    }
    //else if (PARSE_CMD_CHAR('1'))
    //{
    //}
    else if (PARSE_CMD_ANY_CHAR)
    {
      PRINT(pString_NoSuitableAnswerFound);
    }
    else {}
  }
  else {}
}


/**
  * @brief User i/f FSM implementation for the terminal UDP tests menu step
  * @param FuncAction: type of action from UserIf_Fsm_StateAction_t to run for function.
  * @retval None
  */
static void UserIf_Fsm_UdpTestsState(UserIf_Fsm_StateAction_t FuncAction)
{
  User_Term_CmdBody_t * rcv_command = NULL;

  if (FuncAction == USERIF_FSM_DISPMENU)
  {
    PRINT(pString_CR);
    PRINT(">> UDP test menu: <<\n\r");
    PRINT("   To execute 'basic' UDP Data REQ test once, press 0 then ENTER\n\r");
    PRINT("   To execute several 'basic' UDP Data REQ tests in a raw , press 1 then ENTER\n\r");
    PRINT("   To execute 'loopback' UDP Data REQ test, press 2 then ENTER\n\r");
    /* Append other test to EVLKST8500GH test menu here... */

    USEREVT_FLUSH(USEREVT_G3_UDPDATAIND_RCVED);   /* Reset former UDP related kept events */

    UserG3_LaunchUdpConnSet();   /* Prepare for UDP data transfer by launching an UDP Connection set */

    User_Fsm.CurState = USERIF_FSM_UDPTESTS_STATE_PROCSEL;
  }

  else if (FuncAction == USERIF_FSM_PROCSEL)
  {
    rcv_command = USER_TERM_GET_COMMAND;

    /* Parse received command: */
    if (PARSE_CMD_CHAR('0'))
    {
      User_Fsm.CurState = USERIF_FSM_UDPTEST_SINGLE_DATAREQ_STATE_DISPMENU;
    }
    else if (PARSE_CMD_CHAR('1'))
    {
      User_Fsm.CurState = USERIF_FSM_UDPTEST_MULTIPLE_BASIC_DATAREQ_STATE_DISPMENU;
    }
    else if (PARSE_CMD_CHAR('2'))
    {
      User_Fsm.CurState = USERIF_FSM_UDPTEST_LOOPBK_DATAREQ_STATE_DISPMENU;
    }
    else if (PARSE_CMD_ANY_CHAR)
    {
      PRINT(pString_NoSuitableAnswerFound);
    }
    else {}
  }
  else {}
}


/**
  * @brief User i/f FSM implementation for the terminal single data req test step
  * @param FuncAction: type of action from UserIf_Fsm_StateAction_t to run for function.
  * @retval None
  */
static void UserIf_Fsm_UdpTestSingleDataReqState(UserIf_Fsm_StateAction_t FuncAction)
{
  User_Term_CmdBody_t * rcv_command = NULL;

  if (FuncAction == USERIF_FSM_DISPMENU)
  {
    PRINT(pString_CR);
    PRINT(">>> Basic UDP DataReq test: <<<\n\r");
    PRINT("    If board is UDP DataReq originator, press 0 then ENTER\n\r");
    PRINT("    If board is UDP DataReq recipient, press 1 then ENTER\n\r");
    PRINT(pString_CR);
    PRINT("    If not already done, launch corresponding test on the OTHER BOARD now!...\n\r");

    User_Fsm.CurState = USERIF_FSM_UDPTEST_SINGLE_DATAREQ_STATE_PROCSEL;
  }

  else if (FuncAction == USERIF_FSM_PROCSEL)
  {
    switch (User_Fsm.TestStep)
    {   
      case USERIF_FSM_TESTSTEP_0:
        rcv_command = USER_TERM_GET_COMMAND;
        
        /* Parse received command: */
        if (PARSE_CMD_CHAR('0'))
        {
          User_Fsm.TestStep = USERIF_FSM_TESTSTEP_1;
        }
        else if (PARSE_CMD_CHAR('1'))
        {
          User_Fsm.TestStep = USERIF_FSM_TESTSTEP_2;
        }
        else if (PARSE_CMD_ANY_CHAR)
        {
          PRINT(pString_NoSuitableAnswerFound);
        }
        else {}
        break;

      case USERIF_FSM_TESTSTEP_1:
          UserIf_Fsm_UdpTestSingleDataReqOriginatorExec();
        break;

      case USERIF_FSM_TESTSTEP_2:
          UserIf_Fsm_UdpTestSingleDataReqRecipientExec();
        break;
   
      default:
        break;
    }

  }
  else {}
}


/**
  * @brief This function implements the single UDP data req test execution from UDP DATA Req originator side
  * @retval None
  */
static inline void UserIf_Fsm_UdpTestSingleDataReqOriginatorExec(void)
{
  User_Term_CmdBody_t * rcv_command = NULL;

  switch (User_Fsm.TestSubStep)
  {
    case USERIF_FSM_TESTSTEP_0:
      PRINT(pString_CR);
      PRINT("    Type (or send via file) the string to send over UDP link (max size = %d), then ENTER\n\r", (TERM_CMD_PAYLD_MAX_SIZE-1U));
      User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_1;
      break;

    case USERIF_FSM_TESTSTEP_1:
      rcv_command = USER_TERM_GET_COMMAND;
      if (PARSE_CMD_ANY_CHAR)
      {
        UserG3_StartUdpDataTransfer((void *)rcv_command->Payload, rcv_command->Length);
        PRINT("    Typed string is: ");
        User_Term_Printb(rcv_command->Payload, rcv_command->Length);
        PRINT(pString_CR);

        PRINT(pString_CR);
        UserIf_Fsm_PrintWaitingForMsg(USEREVT_G3_UDPDATACNF_RCVED_Pos);  /* Now waiting for G3UDP-DATA.Cnf message... */
        User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_2;
      }
      break;

    case USERIF_FSM_TESTSTEP_2:
      if (USEREVT_JUST_RISEN(USEREVT_G3_UDPDATACNF_RCVED))
      {
#ifdef ONE_TIME_TEST
        /* Close connection  */
        //UserConf_Fsm_CloseUdpDataTransfer(); /* No need to close here (Close to be checked/used later) */

        /* Test ended! Go back to Test menu display */
        PRINT(pString_TestIsFinished);
        UserIf_Fsm_ResetToState(USERIF_FSM_TESTENTRY_STATE_DISPMENU);
#else
        PRINT(pString_PlayTestOnceMore);
        User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_0;
#endif
      }
      break;
    default:
      break;
  }
}


/**
  * @brief This function implements the single UDP data req test execution from UDP DATA Req recipient side
  * @retval None
  */
static inline void UserIf_Fsm_UdpTestSingleDataReqRecipientExec(void)
{

  switch (User_Fsm.TestSubStep)
  {
    case USERIF_FSM_TESTSTEP_0:
      PRINT(pString_CR);
      UserIf_Fsm_PrintWaitingForMsg(USEREVT_G3_UDPDATAIND_RCVED_Pos);  /* Now waiting for G3UDP-DATA.Ind message... */
      User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_1;
      break;

    case USERIF_FSM_TESTSTEP_1:
      if (USEREVT_RISEN(USEREVT_G3_UDPDATAIND_RCVED))
      {
        USEREVT_FLUSH(USEREVT_G3_UDPDATAIND_RCVED);
        PRINT("    G3UDP-DATA.Ind msg content is: ");
        User_Term_Printb(UserG3_CommonEvt.UdpData.pPayload, UserG3_CommonEvt.UdpData.Length);
        PRINT(pString_CR);

#ifdef ONE_TIME_TEST
        /* Close connection  */
        //UserConf_Fsm_CloseUdpDataTransfer(); /* No need to close here (Close to be checked/used later) */

        /* Test ended! Go back to Test menu display */
        PRINT(pString_TestIsFinished);
        UserIf_Fsm_ResetToState(USERIF_FSM_TESTENTRY_STATE_DISPMENU);
#else
        PRINT(pString_PlayTestOnceMore);
        User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_0;
#endif
      }
      break;
    default:
      break;
  }
}



/**
  * @brief User i/f FSM implementation for the terminal multiple basic data req test step
  * @param FuncAction: type of action from UserIf_Fsm_StateAction_t to run for function.
  * @retval None
  */
static void UserIf_Fsm_UdpTestMultipleBasicDataReqState(UserIf_Fsm_StateAction_t FuncAction)
{
  User_Term_CmdBody_t * rcv_command = NULL;

  if (FuncAction == USERIF_FSM_DISPMENU)
  {
    PRINT(pString_CR);
    PRINT(">>> Basic UDP DataReq tests in a raw: <<<\n\r");
    PRINT("    If board is UDP DataReq originator, press 0 then ENTER\n\r");
    PRINT("    If board is UDP DataReq recipient, press 1 then ENTER\n\r");
    PRINT(pString_CR);
    PRINT("    If not already done, launch corresponding test on the OTHER BOARD now!...\n\r");

    User_Fsm.CurState = USERIF_FSM_UDPTEST_MULTIPLE_BASIC_DATAREQ_STATE_PROCSEL;
  }
  else if (FuncAction == USERIF_FSM_PROCSEL)
  {
    switch (User_Fsm.TestStep)
    {   
      case USERIF_FSM_TESTSTEP_0:
        rcv_command = USER_TERM_GET_COMMAND;
        
        /* Parse received command: */
        if (PARSE_CMD_CHAR('0'))
        {
          User_Fsm.TestStep = USERIF_FSM_TESTSTEP_1;
        }
        else if (PARSE_CMD_CHAR('1'))
        {
          User_Fsm.TestStep = USERIF_FSM_TESTSTEP_2;
        }
        else if (PARSE_CMD_ANY_CHAR)
        {
          User_Term_Printf(pString_NoSuitableAnswerFound);
        }
        else {}
        break;

      case USERIF_FSM_TESTSTEP_1:
          UserIf_Fsm_UdpTestMultipleBasicDataReqOriginatorExec();
        break;

      case USERIF_FSM_TESTSTEP_2:
          UserIf_Fsm_UdpTestMultipleBasicDataReqRecipientExec();
        break;
   
      default:
        break;
    }

  }
  else {}
}


/**
  * @brief This function implements the multiple basic UDP data req test executed in a raw from UDP DATA Req originator side
  * @retval None
  */
static inline void UserIf_Fsm_UdpTestMultipleBasicDataReqOriginatorExec(void)
{
  User_Term_CmdBody_t * rcv_command = NULL;
  uint8_t i;
   
  switch (User_Fsm.TestSubStep)
  {
    case USERIF_FSM_TESTSTEP_0:
      PRINT(pString_CR);
      PRINT("   Enter the nb of tests to launch (btw 1 and 99) then ENTER\n\r");
      UserIf_Fsm_MultipleBasicTestsInfo.Test_Counter_Orig_Side = 0U;
      UserIf_Fsm_MultipleBasicTestsInfo.Required_Nb_Tests_Orig_Side = 0U;
      User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_1;
      break;

    case USERIF_FSM_TESTSTEP_1:
      rcv_command = USER_TERM_GET_COMMAND;
  
      UserIf_Fsm_MultipleBasicTestsInfo.Required_Nb_Tests_Orig_Side = UserIf_Fsm_ReadChar(rcv_command);  
      if (UserIf_Fsm_MultipleBasicTestsInfo.Required_Nb_Tests_Orig_Side != 0U)
      {
        User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_2;
      }
      break;
      
    case USERIF_FSM_TESTSTEP_2:
      PRINT(pString_CR);
      PRINT("    Type (or send via file) the string to send over UDP link (max size = %d), then ENTER\n\r", (TERM_CMD_PAYLD_MAX_SIZE-1U));
      User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_3;
      break;

    case USERIF_FSM_TESTSTEP_3:
      rcv_command = USER_TERM_GET_COMMAND;
      if (PARSE_CMD_ANY_CHAR)
      {
        /* Store the command received from the user for the next UDP transfers */
        UserIf_Fsm_MultipleBasicTestsInfo.Rcv_Command_Length = rcv_command->Length;

        for (i=0; i<TERM_CMD_PAYLD_MAX_SIZE; i++)
        {
          UserIf_Fsm_MultipleBasicTestsInfo.Rcv_Command_Payload[i] = rcv_command->Payload[i];
        }
        
        PRINT("    Typed string is: ");
        User_Term_Printb(rcv_command->Payload, rcv_command->Length);
        PRINT(pString_CR);
        
        /* Send the 1st UDP with the nb of tests as data so that the receiver knows when to finish the test */
        rcv_command->Payload[0] = UserIf_Fsm_MultipleBasicTestsInfo.Required_Nb_Tests_Orig_Side;
        rcv_command->Length = 1U;
        UserG3_StartUdpDataTransfer((void *)rcv_command->Payload, rcv_command->Length);

        UserIf_Fsm_PrintWaitingForMsg(USEREVT_G3_UDPDATACNF_RCVED_Pos);  /* Now waiting for G3UDP-DATA.Cnf message... */
        User_Fsm.UdpTransferTimeoutT0 = HAL_GetTick(); /* Arm timeout timer */
        User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_4;
      }
      break;

    case USERIF_FSM_TESTSTEP_4:
      if (USEREVT_JUST_RISEN(USEREVT_G3_UDPDATACNF_RCVED))
      {
#ifdef ONE_TIME_TEST
        /* Close connection  */
        //UserConf_Fsm_CloseUdpDataTransfer(); /* No need to close here (Close to be checked/used later) */

        /* Test ended! Go back to Test menu display */
        PRINT(pString_TestIsFinished);
        UserIf_Fsm_ResetToState(USERIF_FSM_TESTENTRY_STATE_DISPMENU);
#else
        if (UserIf_Fsm_MultipleBasicTestsInfo.Test_Counter_Orig_Side < UserIf_Fsm_MultipleBasicTestsInfo.Required_Nb_Tests_Orig_Side)
        {
          UserIf_Fsm_MultipleBasicTestsInfo.Test_Counter_Orig_Side++;
          UserG3_StartUdpDataTransfer((void *)UserIf_Fsm_MultipleBasicTestsInfo.Rcv_Command_Payload, UserIf_Fsm_MultipleBasicTestsInfo.Rcv_Command_Length);  
        }
        else
        {
          PRINT(pString_CR);
          User_Term_Printf("    Test is finished! %d UDP transfers have been launched with your data.\n\r", UserIf_Fsm_MultipleBasicTestsInfo.Required_Nb_Tests_Orig_Side);
           
          PRINT(pString_PlayTestOnceMore);
          User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_0;        
        }
#endif
      }
      else if (UserIf_Fsm_EstimateDelay(User_Fsm.UdpTransferTimeoutT0) > TIMEOUT_DELAY)
      {
        PRINT("Timeout reached!\n\r");
        UserIf_Fsm_ResetToState(USERIF_FSM_TESTENTRY_STATE_DISPMENU);
      }
      else {}
      break;

    default:
      break;
  }
}


/**
  * @brief This function implements multiple UDP data req test from UDP DATA Req recipient side
  * @retval None
  */
static inline void UserIf_Fsm_UdpTestMultipleBasicDataReqRecipientExec(void)
{
  switch (User_Fsm.TestSubStep)
  {
    case USERIF_FSM_TESTSTEP_0:
      PRINT(pString_CR);
      UserIf_Fsm_PrintWaitingForMsg(USEREVT_G3_UDPDATAIND_RCVED_Pos);  /* Now waiting for G3UDP-DATA.Ind message... */
      User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_1;
      UserIf_Fsm_MultipleBasicTestsInfo.Test_Counter_Rcv_Side = 0U;
      UserIf_Fsm_MultipleBasicTestsInfo.Required_Nb_Tests_Rcv_Side = 0U;
      break;

    case USERIF_FSM_TESTSTEP_1:
      if (USEREVT_RISEN(USEREVT_G3_UDPDATAIND_RCVED))
      {
        USEREVT_FLUSH(USEREVT_G3_UDPDATAIND_RCVED);        
        if (UserIf_Fsm_MultipleBasicTestsInfo.Test_Counter_Rcv_Side == 0U)
        {
          /* The 1st data received is the expected iteration number */
          UserIf_Fsm_MultipleBasicTestsInfo.Required_Nb_Tests_Rcv_Side = UserG3_CommonEvt.UdpData.pPayload[0];
        }
        else if (UserIf_Fsm_MultipleBasicTestsInfo.Test_Counter_Rcv_Side >= UserIf_Fsm_MultipleBasicTestsInfo.Required_Nb_Tests_Rcv_Side)
        {
          PRINT("    Last G3UDP-DATA.Ind msg content is: ");
          User_Term_Printb(UserG3_CommonEvt.UdpData.pPayload, UserG3_CommonEvt.UdpData.Length);
          PRINT(pString_CR);
          PRINT("     Test is finished ! G3UDP-DATA.Ind received %d times\n\r", UserIf_Fsm_MultipleBasicTestsInfo.Required_Nb_Tests_Rcv_Side);

          PRINT(pString_PlayTestOnceMore);
          User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_0; 
        }
        UserIf_Fsm_MultipleBasicTestsInfo.Test_Counter_Rcv_Side++;
        User_Fsm.UdpTransferTimeoutT0 = HAL_GetTick(); /* Arm timeout timer */
      }
      else if ( (UserIf_Fsm_EstimateDelay(User_Fsm.UdpTransferTimeoutT0) > TIMEOUT_DELAY) && (UserIf_Fsm_MultipleBasicTestsInfo.Test_Counter_Rcv_Side > 1U) ) /* If timeout is detected AND a 1st data + the data for Required_Nb_Tests_Rcv_Side has already being received THEN quit */
      {
        PRINT("Timeout reached!\n\r");
        UserIf_Fsm_ResetToState(USERIF_FSM_TESTENTRY_STATE_DISPMENU);
      }
      else {}
      break;
      
    default:
      break;
  }
}


/**
  * @brief User i/f FSM implementation for the terminal loopback data req test step
  * @param FuncAction: type of action from UserIf_Fsm_StateAction_t to run for function.
  * @retval None
  */
static void UserIf_Fsm_UdpTestLoopbackDataReqState(UserIf_Fsm_StateAction_t FuncAction)
{
  User_Term_CmdBody_t * rcv_command = NULL;

  if (FuncAction == USERIF_FSM_DISPMENU)
  {
    PRINT(pString_CR);
    PRINT(">>> Loopback UDP DataReq test: <<<\n\r");
    PRINT("    If board is UDP DataReq originator, press 0 then ENTER\n\r");
    PRINT("    If board ensures the UDP DataReq mirroring, press 1 then ENTER\n\r");
    PRINT(pString_CR);
    PRINT("    If not already done, launch corresponding test on the OTHER BOARD now!...\n\r");

    User_Fsm.CurState = USERIF_FSM_UDPTEST_LOOPBK_DATAREQ_STATE_PROCSEL;
  }

  else if (FuncAction == USERIF_FSM_PROCSEL)
  {
    switch (User_Fsm.TestStep)
    {   
      case USERIF_FSM_TESTSTEP_0:
        rcv_command = USER_TERM_GET_COMMAND;
        
        /* Parse received command: */
        if (PARSE_CMD_CHAR('0'))
        {
          User_Fsm.TestStep = USERIF_FSM_TESTSTEP_1;
        }
        else if (PARSE_CMD_CHAR('1'))
        {
          User_Fsm.TestStep = USERIF_FSM_TESTSTEP_2;
        }
        else if (PARSE_CMD_ANY_CHAR)
        {
          PRINT(pString_NoSuitableAnswerFound);
        }
        else {}
        break;

      case USERIF_FSM_TESTSTEP_1:
          UserIf_Fsm_UdpTestLoopbackDataReqOriginatorExec();
        break;

      case USERIF_FSM_TESTSTEP_2:
          UserIf_Fsm_UdpTestLoopbackDataReqLooperExec();
        break;
   
      default:
        break;
    }

  }
  else {}
}


/**
  * @brief This function implements the loopback UDP data req test execution from UDP DATA Req originator side
  * @retval None
  */
static inline void UserIf_Fsm_UdpTestLoopbackDataReqOriginatorExec(void)
{
  User_Term_CmdBody_t * rcv_command = NULL;

  switch (User_Fsm.TestSubStep)
  {
    case USERIF_FSM_TESTSTEP_0:
      PRINT(pString_CR);
      PRINT("    Type (or send via file) the string to send over UDP link (max size = %d), then ENTER\n\r", (TERM_CMD_PAYLD_MAX_SIZE-1U));
      User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_1;
      break;

    case USERIF_FSM_TESTSTEP_1:
      rcv_command = USER_TERM_GET_COMMAND;
      if (PARSE_CMD_ANY_CHAR)
      {
        USEREVT_FLUSH(USEREVT_G3_UDPDATAIND_RCVED);   /* Reset any likely UDP-DATA.Ind events that could have been received prior to the test (in order not UDP loopback originator to be falsely triggered on it) */
        UserG3_StartUdpDataTransfer((void *)rcv_command->Payload, rcv_command->Length);
        UserG3_StartTimeCounter();  /* Start timer in order to evaluate data transfer loopback duration */
        PRINT("    Typed string is: ");
        User_Term_Printb(rcv_command->Payload, rcv_command->Length);
        PRINT(pString_CR);

        PRINT(pString_CR);
        UserIf_Fsm_PrintWaitingForMsg(USEREVT_G3_UDPDATAIND_RCVED_Pos);  /* Now waiting for G3UDP-DATA.Ind message... */
        User_Fsm.UdpTransferTimeoutT0 = HAL_GetTick();
        User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_2;
      }
      break;

    case USERIF_FSM_TESTSTEP_2:
      if (USEREVT_RISEN(USEREVT_G3_UDPDATAIND_RCVED))
      {
        UserG3_StopTimecounter();
        USEREVT_FLUSH(USEREVT_G3_UDPDATAIND_RCVED);
        PRINT("    Loopback'd msg content is: ");
        User_Term_Printb(UserG3_CommonEvt.UdpData.pPayload, UserG3_CommonEvt.UdpData.Length);
        PRINT(pString_CR);
        PRINT("%d characters loopbacked in %d ms \n\r", (UserG3_CommonEvt.UdpData.Length - 1U), UserG3_CommonEvt.LoopbackEndTime - UserG3_CommonEvt.LoopbackStartTime);
#ifdef ONE_TIME_TEST
        /* Close connection  */
        //UserConf_Fsm_CloseUdpDataTransfer(); /* No need to close here (Close to be checked/used later) */

        /* Test ended! Go back to Test menu display */
        PRINT(pString_TestIsFinished);
        UserIf_Fsm_ResetToState(USERIF_FSM_TESTENTRY_STATE_DISPMENU);
#else
        PRINT(pString_PlayTestOnceMore);
        User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_0;
#endif
      }
      else if (UserIf_Fsm_EstimateDelay(User_Fsm.UdpTransferTimeoutT0) > TIMEOUT_DELAY)
      {
        PRINT("Timeout reached!\n\r");
        UserIf_Fsm_ResetToState(USERIF_FSM_TESTENTRY_STATE_DISPMENU);
      }
      else {}
      break;

    default:
      break;
  }
}


/**
  * @brief This function implements the loopback UDP data req test execution from UDP DATA Req mirror node side
  * @retval None
  */
static inline void UserIf_Fsm_UdpTestLoopbackDataReqLooperExec(void)
{

  switch (User_Fsm.TestSubStep)
  {
    case USERIF_FSM_TESTSTEP_0:
      PRINT(pString_CR);
      UserIf_Fsm_PrintWaitingForMsg(USEREVT_G3_UDPDATAIND_RCVED_Pos);  /* Now waiting for G3UDP-DATA.Ind message... */
      User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_1;
      break;

    case USERIF_FSM_TESTSTEP_1:
      if (USEREVT_RISEN(USEREVT_G3_UDPDATAIND_RCVED))
      {
        USEREVT_FLUSH(USEREVT_G3_UDPDATAIND_RCVED);
        PRINT("    G3UDP-DATA.Ind msg content is: ");
        User_Term_Printb(UserG3_CommonEvt.UdpData.pPayload, UserG3_CommonEvt.UdpData.Length);
        PRINT(pString_CR);

        UserG3_StartUdpDataTransfer((void *)UserG3_CommonEvt.UdpData.pPayload, UserG3_CommonEvt.UdpData.Length);

#ifdef ONE_TIME_TEST
        /* Close connection  */
        //UserConf_Fsm_CloseUdpDataTransfer(); /* No need to close here (Close to be checked/used later) */

        /* Test ended! Go back to Test menu display */
        PRINT(pString_TestIsFinished);
        UserIf_Fsm_ResetToState(USERIF_FSM_TESTENTRY_STATE_DISPMENU);
#else
        PRINT(pString_PlayTestOnceMore);
        User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_0;
#endif
      }
      break;

    default:
      break;

  }
}


/**
  * @brief User i/f FSM implementation resetting FSM to a specific state 
  * @param StateToResetTo: state on which to reset to
  * @retval None
  */
static void UserIf_Fsm_ResetToState(UserIf_Fsm_State_t StateToResetTo)
{
  User_Term_UartRxFifoInit();  /* Flush UART Rx FIFO */
  User_Fsm.CurState = StateToResetTo;
  User_Fsm.TestStep = USERIF_FSM_TESTSTEP_0;
  User_Fsm.TestSubStep = USERIF_FSM_TESTSTEP_0;
  User_Fsm.localUserEvents = 0U;
}


/**
  * @brief Printf utility function centralizing carriage return printing 
  * @param CrNb: number of CR to print
  * @retval None
  */
static void UserIf_Fsm_PrintCR(uint32_t CrNb)
{
  uint32_t i;
  for (i=0U; i<CrNb; i++) 
  {
    PRINT(pString_CR);
  }
}


/**
  * @brief Printf utility function centralizing "waiting for message" printing 
  * @param MessageIdx: index of awaited message  
  * @retval None
  */
static void UserIf_Fsm_PrintWaitingForMsg(uint32_t MessageIdx)
{
  PRINT("Waiting for %s message...\n\r", UserIf_Fsm_EventsToDisplay[MessageIdx].pString);
}

/**
  * @brief This functions gets the number entered by the user on the terminal. It should be btw 1 and 99. 
  * @param RcvCommand: command typed by the user  
  * @retval Number typed by the user, in decimal format.
  */
static uint32_t UserIf_Fsm_ReadChar(User_Term_CmdBody_t * RcvCommand)
{ 
  uint32_t output = 0U;
  /* Check if the number is in the correct range. First make the check if it is on 1 digit */
  if ((RcvCommand != NULL) && (RcvCommand->Payload[1U] == '\r'))
  {
    /* The number got is in ASCII format. check it is well btw 1 and 9 */
    if ((RcvCommand->Payload[0U] <= ASCII_OFFSET) || (RcvCommand->Payload[0U] > (ASCII_OFFSET + 0x9)))
    {
      PRINT(pString_NoSuitableAnswerFound);
    }
    else
    {
      /* This means the number is between 0 and 9 */
      /* RcvCommand->Payload[i] comes in ACII format. If we substarct ASCII_OFFSET (=0x30) it is in decimal */
      output = (RcvCommand->Payload[0U] - ASCII_OFFSET);
    }
  }
  else if ((RcvCommand != NULL) && (RcvCommand->Payload[2U] == '\r'))
  {
    /* The user ahs entered 2 digits. Check they are both in the correct range */
    if (((RcvCommand->Payload[0U] < ASCII_OFFSET) || (RcvCommand->Payload[0U] > (ASCII_OFFSET + 0x9)))
        || ((RcvCommand->Payload[1U] < ASCII_OFFSET) || (RcvCommand->Payload[1U] > (ASCII_OFFSET + 0x9))))
    {
      PRINT(pString_NoSuitableAnswerFound);
    }
    else
    {
      /* This means the number is between 1 and 99 */
      output = 10U * (RcvCommand->Payload[0U] - ASCII_OFFSET) + (RcvCommand->Payload[1U] - ASCII_OFFSET);
    }
  }
  else if ((RcvCommand != NULL) && ((RcvCommand->Payload[1U] != '\r') || (RcvCommand->Payload[2U] != '\r')))
  {
    PRINT(pString_NoSuitableAnswerFound);
  }
  
  return (output);
}      

#ifdef TERM_TIMESTAMP
/**
  * @brief Printf utility function displaying the time in the format [d:hh:mm:ss] 
  * @retval None
  */
static void UserIf_Fsm_PrintTimestamp(void)
{
  uint32_t cur_timestamp;
  uint32_t seconds, minutes, hours, days;

  cur_timestamp = UserIf_Fsm_EstimateDelay(User_Fsm.TimestampT0);

  seconds = cur_timestamp / 1000U;
  minutes = seconds / 60U;
  seconds %= 60U;
  
  hours = minutes / 60U;
  minutes %= 60U;
  
  days = hours / 24U;
  hours %= 24;

  User_Term_Printf("[%.1d:%.2d:%.2d:%.2d] ", days,hours,minutes,seconds); 
  
}
#endif


/**
  * @brief Printf utility function displaying platform information
  * @retval None
  */
static void UserIf_Fsm_PrintPlatformInfo(void)
{
  User_Term_Printf(pString_CR);
  User_Term_Printf("Platform info:\n\r");



  User_Term_Printf(" STM32 FW version: %d%d%d\n\r", FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_SUB);
  User_Term_Printf(" PE FW version: %X\n\r", (UserG3_CommonEvt.PlatformInfo.g3_info & PE_VERSION_MASK));
  User_Term_Printf(" G3Lib FW version: %X\n\r", UserG3_CommonEvt.PlatformInfo.libVersion);
  User_Term_Printf(" G3RTE FW version: %X\n\r", UserG3_CommonEvt.PlatformInfo.rteVersion);
  User_Term_Printf(" Device Type: ");
  if (UserG3_CommonEvt.PlatformInfo.g3_info & PE_VERSION_PROFILE_MASK)
  {
    User_Term_Printf("COORD.");
  }
  else
  {
    User_Term_Printf("DEVICE");
  }
  User_Term_Printf(pString_CR);
  User_Term_Printf(" Bandplan: ");
  switch ((UserG3_CommonEvt.PlatformInfo.g3_info & PE_VERSION_BAND_MASK) >> PE_VERSION_BAND_SHIFT)
  {
    case G3_LIB_G3PLC_CENA:
      User_Term_Printf("CENELEC A");
      break;

    case G3_LIB_G3PLC_CENB:
      User_Term_Printf("CENELEC B");
      break;

    case G3_LIB_G3PLC_ARIB:
      User_Term_Printf("ARIB");
      break;

    case G3_LIB_G3PLC_FCC:
      User_Term_Printf("FCC");
      break;

    case G3_LIB_IEEE_1901_2_FCC:
      User_Term_Printf("FCC above CENELEC");
      break;

    default:
      break;
  }
  User_Term_Printf(pString_CR);
  User_Term_Printf(" MAC addr.: %X", UserG3_CommonEvt.PlatformInfo.eui64[0U]);
  User_Term_Printf("%X\n\r", UserG3_CommonEvt.PlatformInfo.eui64[1U]);
  User_Term_Printf(" sFlashID: %X\n\r", UserG3_CommonEvt.PlatformInfo.sflashid);
  User_Term_Printf(" Usart num: %X\n\r", UserG3_CommonEvt.PlatformInfo.usartnum);

  User_Term_Printf(" LED Conf: %X\n\r", UserG3_CommonEvt.PlatformInfo.ledconf);
  User_Term_Printf(" Platform: ");
  switch (UserG3_CommonEvt.PlatformInfo.platform)
  {
    case PLAT_TYPE_INVALID:
      User_Term_Printf("Invalid");
      break;

    case PLAT_TYPE_STCOMET_STCOM:
      User_Term_Printf("STCOMET/STCOM");
      break;

    case PLAT_TYPE_ST8500:
      User_Term_Printf("ST8500");
      break;

    case PLAT_TYPE_UNSPEC:
    default:
      User_Term_Printf("Unspecified");
      break;
  }
  User_Term_Printf(pString_CR);
  User_Term_Printf(" RF conn.: %X\n\r", UserG3_CommonEvt.PlatformInfo.rfconf);
  User_Term_Printf(" Plat. Mode: ");
  switch (UserG3_CommonEvt.PlatformInfo.plcmode)
  {
    case HI_CFG_PHY_MODE:
      User_Term_Printf("PHY");
      break;

    case HI_CFG_MAC_MODE:
      User_Term_Printf("MAC");
      break;

    case HI_CFG_ADP_MODE:
      User_Term_Printf("ADP");
      break;

    case HI_CFG_ADP_BOOT_MODE:
      User_Term_Printf("ADP BOOT");
      break;

    case HI_CFG_IPV6_BOOT_MODE:
      User_Term_Printf("IPV6 BOOT");
      break;

    default:
      break;
  }
  User_Term_Printf(" MODE\n\r");
  User_Term_Printf(" PanId: %X\n\r", UserG3_CommonEvt.PlatformInfo.panid);
  User_Term_Printf(" ShortAddr: %X\n\r", UserG3_CommonEvt.PlatformInfo.shortaddr);
  User_Term_Printf(pString_CR);
}


/**
  * @brief Delay estimation using HAL_GetTick API 
  * @param T0Time: Reference time for the delay estimation  
  * @retval Estimated delay.
  */
static uint32_t UserIf_Fsm_EstimateDelay(uint32_t T0Time)
{

  uint32_t time_delay = HAL_GetTick();

  if (time_delay < T0Time) 
  {
    time_delay = 0xFFFFFFFFU - (T0Time - time_delay); /* Manage HAL_GetTick() u32 output value roll-over */
  }
  else
  {
    time_delay = (time_delay - T0Time);
  }

  return time_delay;
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
