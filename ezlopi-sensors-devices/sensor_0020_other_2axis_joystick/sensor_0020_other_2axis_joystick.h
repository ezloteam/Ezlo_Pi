

#ifndef _SENSOR_0020_OTHER_2AXIS_JOYSTICK_H_
#define _SENSOR_0020_OTHER_2AXIS_JOYSTICK_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
// #include <stdbool.h>

typedef enum e_joystick_item_id
{
    JOYSTICK_ITEM_ID_X,
    JOYSTICK_ITEM_ID_Y,
    JOYSTICK_ITEM_ID_SWITCH,
    JOYSTICK_ITEM_ID_MAX
} e_joystick_item_id_t;

int sensor_0020_other_2axis_joystick(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // _SENSOR_0020_OTHER_2AXIS_JOYSTICK_H_
