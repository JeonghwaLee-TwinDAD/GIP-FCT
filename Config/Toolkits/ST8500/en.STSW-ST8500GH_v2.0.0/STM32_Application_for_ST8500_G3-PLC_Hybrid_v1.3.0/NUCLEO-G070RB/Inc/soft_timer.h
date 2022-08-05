/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : soft_timer.h
* Author             : IMS- Industrial BU
* Version            : V1.0.0
* Date               : 03/21/2011
* Description        : Include file for Timer.c code.
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef SOFT_TIMER_H_
#define SOFT_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/* Definitions.                                                               */
/******************************************************************************/

#define TICKS_IN_A_SEC           1000UL
#define TICKS_IN_A_MIN           (TICKS_IN_A_SEC * 60UL)

#define SOFT_TIMER_INACTIVE     ((uint8_t)0x00)
#define SOFT_TIMER_ACTIVE       ((uint8_t)0x01)

#define SOFT_TIMER_SUCCESS      ((uint8_t)0x00)
#define SOFT_TIMER_ERR_BADTMRID ((uint8_t)0x01)

/******************************************************************************/
/* Typedef definition.                                                        */
/******************************************************************************/

typedef enum {SOFT_TIMER_ID_G3APP_KA_PAN = 0, SOFT_TIMER_CNT} timer_index_t;

/******************************************************************************/
/* Function prototypes.                                                       */
/******************************************************************************/

void st_timer_init(void);
uint64_t st_getsystime(void);
uint8_t st_set_timer(timer_index_t timer_id, uint32_t value);
uint8_t  st_reset_timer(timer_index_t timer_id);
bool st_is_timer_active(timer_index_t timer_id);
bool st_is_timer_expired(timer_index_t timer_id);
void st_isr(void);
void st_delay(uint32_t delay);
bool st_is_timeout_expired(uint64_t timeout);

#endif

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
