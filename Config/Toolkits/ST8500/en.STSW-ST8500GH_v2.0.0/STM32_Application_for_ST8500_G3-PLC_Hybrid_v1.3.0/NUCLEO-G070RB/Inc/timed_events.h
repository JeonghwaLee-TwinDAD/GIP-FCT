#ifndef TIMED_EVENTS_H_
#define TIMED_EVENTS_H_

#include <stdbool.h>
#include <stdint.h>
#include <soft_timer.h>

#define MAX_TIMED_EVENTS_NUM             ((uint8_t)8)

#define TIMED_EVENTS_ONESHOT             ((uint32_t)0)

#define TIMED_EVENTS_ABS_TIME(delay)     ((uint64_t)(st_getsystime() + ((uint32_t)(delay))))

typedef void (*timed_event_func_t)(void *);

void timed_events_init(void);
bool timed_events_add(timed_event_func_t func, void *func_arg,
                            uint64_t abs_time_to_fire, uint32_t reset_delay,
                            uint8_t *index_used);
void timed_events_remove(uint8_t index);
void timed_events_exec(void);
bool timed_events_is_used(uint8_t index);

/**
  * @brief    Checks if there is at least one timed event that expires in less than 1 second
  * @return   G3_FALSE if no, G3_TRUE if yes.
  */
bool timed_events_critical(void);

#endif
