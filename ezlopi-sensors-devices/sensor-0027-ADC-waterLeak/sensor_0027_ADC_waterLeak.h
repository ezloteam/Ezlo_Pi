
#ifndef _SENSOR_0027_ADC_WATERLEAK_H_
#define _SENSOR_0027_ADC_WATERLEAK_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_errors.h"

typedef enum
{
    WATERLEAK_NO_WATER_LEAK_OK,
    WATERLEAK_WATER_LEAK_DETECTED,
    WATERLEAK_UNKNOWN,
    WATERLEAK_MAX
} e_water_leak_alarm_states_t;

ezlopi_error_t sensor_0027_ADC_waterLeak(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // _SENSOR_0027_ADC_WATERLEAK_H_
