/**
  ******************************************************************************
  * @file    user_task.c
  * @author  AMG/IPC Application Team
  * @brief   This file contains code that implements the User task
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
#include <stdbool.h>
#include <task_comm.h>
#include <cmsis_os2.h>
#include <assert.h>
#include <user_task.h>
#include <hi_g3lib_sap_interface.h>
#include <g3_app_attrib_tbl.h>
#include <hi_ip_sap_interface.h>
#include <hi_msgs_impl.h>
#include <g3_app_config.h>
#include <hif_g3_common.h>
#include <g3_hif.h>
#include "user_term_rxtx.h"
#include "user_if_fsm.h"
#include <utils.h>


/* Private constants and macros-----------------------------------------------*/
#define WAIT_10 10U

/* Local types ---------------------------------------------------------------*/

/* Local variables -----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported variables --------------------------------------------------------*/ 

/* Exported functions --------------------------------------------------------*/

/**
  * @brief This is the initialization of the User task.
  * @retval None
  */
void USER_TaskInit(task_comm_info_t *g3_task_comm_info_)
{
  UserG3_InitVars(g3_task_comm_info_);
}


/**
  * @brief This is the main function of the User task.
  * @retval None
  */
void USER_AppTask(void)
{
  g3_user_msg_t msg;

#ifndef TU_WO_SERIAL_TERMINAL
  User_Term_Init();
  UserIf_Fsm_Init();
#endif /* TU_WO_SERIAL_TERMINAL */

  while (1)
  {
    /* Check if a message coming from the G3 task has been received in the queue */
    if (UserG3_ReceiveMessage(g3_to_user_queueHandle,&msg, WAIT_10)) 
    {
      UserG3_MsgHandler(&msg);

      UserG3_AppConf();
    }

#ifndef TU_WO_SERIAL_TERMINAL
    UserIf_Fsm_FsmBody();
#endif /* TU_WO_SERIAL_TERMINAL */
    
  }
}


