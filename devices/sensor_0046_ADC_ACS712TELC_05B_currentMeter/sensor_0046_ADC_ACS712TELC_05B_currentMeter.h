#ifndef _0046_ADC_ACS712TELC_05B_CURRENTMETER_H_
#define _0046_ADC_ACS712TELC_05B_CURRENTMETER_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

// Set mannually
#define ASC712TELC_05B_zero_point_mV 2350 // adc_value at the output ; when no current in input side
#define DEFAULT_AC_FREQUENCY 50

/**
 * Chip model : ACS712ELCTR-05B-T
 *
 * Optimized Measurement Range (Ip) = +-5A
 *
 * Sensitivity, Sens(Typ) (mV/A) = 185mv/A
 * */

int sensor_0046_ADC_ACS712TELC_05B_currentMeter(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *user_args);

#endif // _0046_ADC_ACS712TELC_05B_CURRENTMETER_H_