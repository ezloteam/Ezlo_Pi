

#ifndef _JOYSTICK_2_AXIX_H_
#define _JOYSTICK_2_AXIX_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

typedef enum
{
    SENSOR_0020_ADC_2AXIS_JOYSTICK_X,
    SENSOR_0020_ADC_2AXIS_JOYSTICK_Y,
    SENSOR_0020_ADC_2AXIS_JOYSTICK_SWITCH,
    SENSOR_0020_ADC_2AXIS_JOYSTICK_MAX
} e_sensor_0020_ADC_2axis_joystick_item_ids_t;

int sensor_0020_ADC_2axis_joystick(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_args);

#endif //_JOYSTICK_2_AXIX_H_