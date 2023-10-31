

#ifndef SENSOR_0026_ADC_LDR_H
#define SENSOR_0026_ADC_LDR_H

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

typedef enum
{
    LIGHT_ALARM_NO_LIGHT,
    LIGHT_ALARM_LIGHT_DETECTED,
    LIGHT_ALARM_UNKNOWN,
    LIGHT_ALARM_MAX
} e_light_alarm_states_t;

int sensor_0026_ADC_LDR(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // SENSOR_0026_ADC_LDR_H
