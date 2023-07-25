#ifndef _0043_ADC_UV_INTENSITY_H_
#define _0043_ADC_UV_INTENSITY_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
/**
 *  -> GYML_8511 - UV sensor operates in 3.3V
 *
 *  Output analog voltage range : [0.99v - 2.7V ] which is acceptable for esp32 input range with only small error.
 */
// action function
int sensor_0043_adc_gyml8511_uv_intensity(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *usr_args);

#endif //_0043_ADC_UV_INTENSITY_H_