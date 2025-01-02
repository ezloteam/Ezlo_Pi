#ifndef _EZLOPI_CORE_EVENT_QUEUE_H_
#define _EZLOPI_CORE_EVENT_QUEUE_H_

#if 0
#include "ezlopi_core_actions.h"
#include "ezlopi_core_errors.h"

typedef struct s_ezlo_event
{
    e_ezlopi_actions_t action;
    void *arg;
} s_ezlo_event_t;

void ezlopi_event_queue_init(void);
int ezlopi_event_queue_send(s_ezlo_event_t *event_data, int from_isr);
int ezlopi_event_queue_receive(s_ezlo_event_t **event_data, int time_out_ms);
#endif
#endif // _EZLOPI_CORE_EVENT_QUEUE_H_