#ifndef _EZLOPI_CORE_EVENT_GROUP_H_
#define _EZLOPI_CORE_EVENT_GROUP_H_

#include "ezlopi_core_errors.h"

#define EZLOPI_EVENT_BASE 0x10

typedef enum e_ezlopi_event
{
    EZLOPI_EVENT_OTA = BIT0,
    EZLOPI_EVENT_WIFI_CONNECTED = BIT1,
    EZLOPI_EVENT_WIFI_FAIL = BIT2,
    EZLOPI_EVENT_NMA_REG = BIT3,
    EZLOPI_EVENT_PING = BIT4,
    // can not be more than BIT31
} e_ezlopi_event_t;

void ezlopi_event_group_create(void);
ezlopi_error_t ezlopi_event_group_set_event(e_ezlopi_event_t event);
ezlopi_error_t ezlopi_event_group_clear_event(e_ezlopi_event_t event);
ezlopi_error_t ezlopi_event_group_wait_for_event(e_ezlopi_event_t event, uint32_t wait_time_ms, uint32_t clear_on_exit);
e_ezlopi_event_t ezlopi_get_event_bit_status();

#endif // _EZLOPI_CORE_EVENT_GROUP_H_
