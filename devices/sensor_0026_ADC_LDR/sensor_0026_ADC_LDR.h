

#ifndef _026_SENS_LDR_ANALOG_SENSOR_H_
#define _026_SENS_LDR_ANALOG_SENSOR_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

typedef enum
{
    LIGHT_ALARM_NO_LIGHT,
    LIGHT_ALARM_LIGHT_DETECTED,
    LIGHT_ALARM_UNKNOWN,
    LIGHT_ALARM_MAX
} e_light_alarm_states_t;

int sensor_0026_ADC_LDR(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg);

#endif //_026_SENS_LDR_ANALOG_SENSOR_H_