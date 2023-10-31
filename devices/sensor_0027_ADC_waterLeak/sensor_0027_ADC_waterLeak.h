
#ifndef SENSOR_0027_ADC_WATERLEAK_H
#define SENSOR_0027_ADC_WATERLEAK_H

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

typedef enum
{
    WATERLEAK_NO_WATER_LEAK_OK,
    WATERLEAK_WATER_LEAK_DETECTED,
    WATERLEAK_UNKNOWN,
    WATERLEAK_MAX
} e_water_leak_alarm_states_t;

int sensor_0027_ADC_waterLeak(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // SENSOR_0027_ADC_WATERLEAK_H
