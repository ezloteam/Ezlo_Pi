
#ifndef _027_SENS_WATER_SENSOR_H_
#define _027_SENS_WATER_SENSOR_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

typedef enum
{
    WATERLEAK_NO_WATER_LEAK_OK,
    WATERLEAK_WATER_LEAK_DETECTED,
    WATERLEAK_UNKNOWN,
    WATERLEAK_MAX
} e_water_leak_alarm_states_t;

int sensor_0027_ADC_waterLeak(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg);

#endif //_027_SENS_WATER_SENSOR_H_