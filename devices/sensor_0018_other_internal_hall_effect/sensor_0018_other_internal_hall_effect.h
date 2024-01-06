#ifndef _SENSOR_0018_OTHER_INTERNAL_HALL_EFFECT_H_
#define _SENSOR_0018_OTHER_INTERNAL_HALL_EFFECT_H_

#include "driver/adc.h"
#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

typedef enum
{
    HALL_DOOR_WINDOW_DW_IS_OPENED,
    HALL_DOOR_WINDOW_DW_IS_CLOSED,
    HALL_DOOR_WINDOW_UNKNOWN,
    HALL_DOOR_WINDOW_MAX,
} e_hall_door_window_states_t;

int sensor_0018_other_internal_hall_effect(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // _SENSOR_0018_OTHER_INTERNAL_HALL_EFFECT_H_
