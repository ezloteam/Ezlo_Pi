#ifndef __EZLOPI_EVENT_QUEUE_H__
#define __EZLOPI_EVENT_QUEUE_H__

#include "ezlopi_actions.h"

typedef struct s_ezlo_event
{
    e_ezlopi_actions_t action;
    void *arg;
} s_ezlo_event_t;

int ezlopi_event_queue_send();
int ezlopi_event_queue_receive(s_ezlo_event_t **event_data, int time_out_ms);

#endif