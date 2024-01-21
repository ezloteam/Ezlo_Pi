#ifndef _SENSOR_0065_DIGITALIN_FLOAT_SWITCH_H_
#define _SENSOR_0065_DIGITALIN_FLOAT_SWITCH_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"

/**
 *   5v/3v+-------------------------------+------< (+) float s/w (-) >----+ 5v/3v
 *                                        |                               ^
 *                                        |                               |
 *                                     {10kOhm}                         esp32 digita input
 *                                        |                               |
 *                                        |                               V
 *   0V +---------------------------------+-------------------------------+ 0V
 */

typedef enum
{
    WATER_LEVEL_ALARM_WATER_LEVEL_OK,
    WATER_LEVEL_ALARM_WATER_LEVEL_BELOW_LOW_THRESHOLD,
    WATER_LEVEL_ALARM_WATER_LEVEL_ABOVE_HIGH_THRESHOLD,
    WATER_LEVEL_ALARM_UNKNOWN,
    WATER_LEVEL_ALARM_MAX
} e_water_level_alarm_states_t;
//-------------------------------------------------------------------

int sensor_0065_digitalIn_float_switch(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif