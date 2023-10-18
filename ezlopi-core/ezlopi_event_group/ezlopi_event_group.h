#ifndef __EZLOPI_EVENT_GROUP_H__
#define __EZLOPI_EVENT_GROUP_H__

#include "ctype.h"
#include "string.h"
#include "esp_bit_defs.h"

#define EZLOPI_EVENT_BASE 0x10

typedef enum e_ezlopi_event
{
    EZLOPI_EVENT_OTA = BIT0,
    EZLOPI_EVENT_WIFI_CONNECTED = BIT1,
    EZLOPI_EVENT_WIFI_FAIL = BIT2,

    // can not be more than BIT31
} e_ezlopi_event_t;

void ezlopi_event_group_create(void);
void ezlopi_event_group_set_event(e_ezlopi_event_t event);
int ezlopi_event_group_wait_for_event(e_ezlopi_event_t event, uint32_t wait_time_ms, uint32_t clear_on_exit);

#endif // __EZLOPI_EVENT_GROUP_H__
