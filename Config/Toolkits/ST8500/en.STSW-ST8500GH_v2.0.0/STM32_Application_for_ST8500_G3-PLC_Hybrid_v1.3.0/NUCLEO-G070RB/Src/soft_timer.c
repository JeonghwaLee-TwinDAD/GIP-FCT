/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : soft_timer.c
* Author             : IMS- Industrial BU
* Version            : V1.0.0
* Date               : 03/21/2011
* Description        : This file includes all the functions related to the
*                      system timer.
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/******************************************************************************/
/* Includes.                                                                  */
/******************************************************************************/
#include <soft_timer.h>
#include <limits.h>
#include <assert.h>
#include <FreeRTOSConfig.h>
#include <portmacro.h>

#define ST_GRANULARITY    1U

/******************************************************************************/
/* Global variables.                                                          */
/******************************************************************************/

static struct {
    volatile uint64_t st_current_time;
    volatile uint32_t st_timer[SOFT_TIMER_CNT];
    uint8_t           is_active[SOFT_TIMER_CNT];
} soft_timer_info;

/******************************************************************************/
/* External definitions.                                                      */
/******************************************************************************/

void st_isr(void)
{
    soft_timer_info.st_current_time += ST_GRANULARITY;
}

uint64_t st_getsystime(void)
{
    return soft_timer_info.st_current_time;
}

static uint32_t st_get_timer(timer_index_t timer_id)
{
    assert(timer_id < SOFT_TIMER_CNT);

    if (timer_id < SOFT_TIMER_CNT)
        return soft_timer_info.st_timer[timer_id];

    return 0;
}

/*******************************************************************************
* Function Name  : SetTimer
* Description    : This function is used to set the value of the selected Timer.
*                  The TimerId is used to event.
* Input          : - TimerId is the number of the timer under consideration.
*                    value is the timer starting time expressed in milli
*                    seconds.
* Output         : -
* Return         : - In case the TimerId is not in the right range the related
*                    error is returned.
*******************************************************************************/
uint8_t st_set_timer(timer_index_t timer_id, uint32_t value)
{
    assert(timer_id < SOFT_TIMER_CNT);

    if (timer_id < SOFT_TIMER_CNT) {
        soft_timer_info.st_timer[timer_id]  = value;
        soft_timer_info.is_active[timer_id] = SOFT_TIMER_ACTIVE;

        return SOFT_TIMER_SUCCESS;
    }

    return SOFT_TIMER_ERR_BADTMRID;
}

/*******************************************************************************
* Function Name  : ResetTimer
* Description    :
* Input          :
* Output         :
* Return         :
*******************************************************************************/
uint8_t  st_reset_timer(timer_index_t timer_id)
{
    assert(timer_id < SOFT_TIMER_CNT);

    if (timer_id < SOFT_TIMER_CNT) {
        soft_timer_info.st_timer[timer_id]  = 0;
        soft_timer_info.is_active[timer_id] = SOFT_TIMER_INACTIVE;

        return SOFT_TIMER_SUCCESS;
    }

    return SOFT_TIMER_ERR_BADTMRID;
}

/*******************************************************************************
* Function Name  : isTimerActive
* Description    :
* Input          :
* Output         :
* Return         :
*******************************************************************************/
bool st_is_timer_active(timer_index_t timer_id)
{
    assert(timer_id < SOFT_TIMER_CNT);

    return soft_timer_info.is_active[timer_id];
}

bool st_is_timer_expired(timer_index_t timer_id)
{
    assert(timer_id < SOFT_TIMER_CNT);

    return (st_is_timer_active(timer_id) && (st_get_timer(timer_id) == 0));
}

/*******************************************************************************
* Function Name  : Timer_Init
* Description    :
* Input          :
* Output         :
* Return         :
*******************************************************************************/
void st_timer_init(void)
{
    uint32_t i = 0;

    soft_timer_info.st_current_time = 0;

    for (i = 0; i < SOFT_TIMER_CNT; i++) {
        soft_timer_info.is_active[i] = SOFT_TIMER_INACTIVE;
        soft_timer_info.st_timer[i]  = 0;
    }
}

#pragma optimize=none
void st_delay(uint32_t delay)
{
    uint64_t curr_time;
    const uint64_t delay_end_time = soft_timer_info.st_current_time + delay;

    do
    {
        curr_time = soft_timer_info.st_current_time;
    } while (curr_time < delay_end_time);
}

bool st_is_timeout_expired(uint64_t timeout)
{
    if (timeout < soft_timer_info.st_current_time)
        return true;
    else
        return false;
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
