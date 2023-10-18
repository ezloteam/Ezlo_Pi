#ifndef __EZLOPI_EVENT_QUEUE_H__
#define __EZLOPI_EVENT_QUEUE_H__

#include "ezlopi_actions.h"

typedef struct s_ezlo_event
{
    e_ezlopi_actions_t action;
    void *arg;
} s_ezlo_event_t;

void ezlopi_event_queue_init(void);
int ezlopi_event_queue_send(s_ezlo_event_t *event_data, int from_isr);
int ezlopi_event_queue_receive(s_ezlo_event_t **event_data, int time_out_ms);

#endif // __EZLOPI_EVENT_QUEUE_H__