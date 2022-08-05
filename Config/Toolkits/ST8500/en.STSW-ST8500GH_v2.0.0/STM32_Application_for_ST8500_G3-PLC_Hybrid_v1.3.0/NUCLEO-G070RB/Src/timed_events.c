/**
*   @file      timed_events.c
*   @author    IPC - Industrial BU
*   @date      23 october 2019
*   @brief     This source code includes functions to handle timed events
*   @note      (C) COPYRIGHT 2019 STMicroelectronics
*
* @attention
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
*/

#include <timed_events.h>
#include <soft_timer.h>

typedef struct {
    uint64_t abs_time_to_fire;
    uint32_t reset_delay;
    timed_event_func_t func;
    void *func_arg;
} timed_event_t;

typedef struct {
    bool used;
    timed_event_t event;
} timed_event_slot_t;

#define TIMED_EVENTS_ABS_TIME_NOW   st_getsystime()

static timed_event_slot_t timed_events[MAX_TIMED_EVENTS_NUM];

void timed_events_init(void)
{
    for (int i = 0; i < MAX_TIMED_EVENTS_NUM; i++) {
        timed_events[i].used = false;
    }
}

bool timed_events_add(timed_event_func_t func, void *func_arg,
                      uint64_t abs_time_to_fire, uint32_t reset_delay,
                      uint8_t *index_used)
{
    int index;

    for (index = 0; index < MAX_TIMED_EVENTS_NUM; index++) {
        if (!timed_events[index].used)
            break;
    }

    if (index >= MAX_TIMED_EVENTS_NUM)
    {
        if (index_used) {
            *index_used = MAX_TIMED_EVENTS_NUM;
        }
        return false;
    }

    timed_events[index].used = true;
    timed_events[index].event.func = func;
    timed_events[index].event.func_arg = func_arg;
    timed_events[index].event.abs_time_to_fire = abs_time_to_fire;
    timed_events[index].event.reset_delay = reset_delay;

    if (index_used) {
        *index_used = (uint8_t)index;
    }

    return true;
}

void timed_events_remove(uint8_t index)
{
    if (index >= MAX_TIMED_EVENTS_NUM)
        return;

    timed_events[index].used = false;
}

void timed_events_exec(void)
{
    for (int i = 0; i < MAX_TIMED_EVENTS_NUM; i++) {
        if (!timed_events[i].used)
            continue;

        if (timed_events[i].event.abs_time_to_fire <= st_getsystime())
        {
            timed_events[i].event.func(timed_events[i].event.func_arg);

            if (timed_events[i].event.reset_delay > 0) {
                timed_events[i].event.abs_time_to_fire = TIMED_EVENTS_ABS_TIME(timed_events[i].event.reset_delay);
            } else {
                timed_events[i].used = false;
            }
        }
    }
}

bool timed_events_is_used(uint8_t index)
{
    if (index >= MAX_TIMED_EVENTS_NUM)
        return false;

    return timed_events[index].used;
}

/**
  * @brief    Checks if there is at least one timed event that expires in less than 1 second
  * @return   G3_FALSE if no, G3_TRUE if yes.
  */
bool timed_events_critical(void)
{
    for (int i = 0; i < MAX_TIMED_EVENTS_NUM; i++)
    {
        if (timed_events_is_used(i) && ((timed_events[i].event.abs_time_to_fire - st_getsystime()) < TICKS_IN_A_SEC))
            return true;
    }

    return false;
}
