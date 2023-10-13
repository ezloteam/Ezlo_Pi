

#ifndef _SENSOR_DOOR_H_
#define _SENSOR_DOOR_H_

#include "driver/adc.h"
#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

typedef enum
{
    DOOR_WINDOW_DW_IS_OPENED,
    DOOR_WINDOW_DW_IS_CLOSED,
    DOOR_WINDOW_UNKNOWN,
    DOOR_WINDOW_MAX
} e_door_window_states_t;

int door_hall_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *args, void *user_arg);

#endif // _SENSOR_DOOR_H_
