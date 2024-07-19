#ifndef _SENSOR_0017_ADC_POTENTIOMETER_H_
#define _SENSOR_0017_ADC_POTENTIOMETER_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_errors.h"

/**
 *  NOTE : FC28 - module gives (0V - 4.2V) as analog output .
 *  But ESP32- only allows upto 2.4V max input.
 *
 *  Remedy:  introduce a voltage divider of ratio [1:2 , i.e. 50%]  on the Sensor analog output , so that esp32 adc pin recieves half voltage only.
 *             ( Half of 4.2) -> 2.1V
 *
 *            > 3.3V----+
 *                      |
 *              {potentiometer_half1}
 *                      |
 *                      +------------+ ~2.4V
 *                      |                ^
 *              {potentiometer_half2}    |  esp32 analog input
 *                      |                v
 *            > 0V------+------------+ 0V
 *
 *
 **/

ezlopi_error_t sensor_0017_ADC_potentiometer(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif //_SENSOR_0017_ADC_POTENTIOMETER_H_