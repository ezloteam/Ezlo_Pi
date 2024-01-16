#ifndef _SENSOR_0043_ADC_UV_INTENSITY_H_
#define _SENSOR_0043_ADC_UV_INTENSITY_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
/**
 *  -> GYML_8511 - UV sensor operates in 3.3V
 *
 *  -> Better to connect EN->3.3v
 *
 *  Output analog voltage range : [0.99v - 2.7V ] which is acceptable for esp32 input range with only small error.
 */
// action function
int sensor_0043_ADC_GYML8511_UV_intensity(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif //_SENSOR_0043_ADC_UV_INTENSITY_H_