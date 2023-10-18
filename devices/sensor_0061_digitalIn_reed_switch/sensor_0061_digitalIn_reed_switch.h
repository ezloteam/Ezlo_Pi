#ifndef _SENSOR_0061_DIGITALIN_REED_SWITCH_H_
#define _SENSOR_0061_DIGITALIN_REED_SWITCH_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
/**
 *                          REED switch setup
 * [3.3V] +Vin ------->-----+
 *                          |
 *                          |     Reed Switch
 *                          |          /
 *                   _______+-------->/  |<-------+ [gpio]
 *                   ^      |                     ^
 *                   |      |  [constant]         |
 *                   |      |                     |
 *              Resistor {~10kOhm}           esp32 analog input
 *                   |      |                     |
 *                   |      |                     |
 *                   V      |                     v
 *      [0V] ---------------+--> 0V --------------+ 0V
 */

typedef enum
{
    REED_DOOR_WINDOW_DW_IS_OPENED,
    REED_DOOR_WINDOW_DW_IS_CLOSED,
    REED_DOOR_WINDOW_UNKNOWN,
    REED_DOOR_WINDOW_MAX
} e_reed_door_window_states_t;
//-------------------------------------------------------------------
int sensor_0061_digitalIn_reed_switch(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif