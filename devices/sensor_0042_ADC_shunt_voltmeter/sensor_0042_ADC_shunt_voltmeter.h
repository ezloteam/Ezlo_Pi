#ifndef _SENSOR_0042_ADC_SHUNT_VOLTMETER_H_
#define _SENSOR_0042_ADC_SHUNT_VOLTMETER_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
/**
 * SHUNT VOLTAGE -> measures DC voltage [0V - 25V]
 *
 *  NOTE : Shunt Voltage - module gives (0V - 5V) as analog output .
 *  But ESP32- only allows upto 2.4V max input.
 *
 *  Remedy:  introduce a voltage divider of ratio [1:2 , i.e. 50%]  on the Sensor analog output , so that esp32 adc pin recieves half voltage only.
 *             ( Half of 5) -> ~2.5V
 *
 *            > 5V------,
 *                      |
 *                     {1KOhm}
 *                      |
 *                      +------------+ ~2.4V
 *                      |                ^
 *                     {1KOhm}           |  esp32 analog input
 *                      |                v
 *            > 0V------+------------+ 0V
 **/

//------------------------------------------

/**
 * Please don't forget to uncomment ,
 * -> If you added a voltage divider at sensor's analog output. [Make sure voltage does not exceed 2.5V]
 *      |
 *      |
 *      V
 */

//------------------------------------------

int sensor_0042_ADC_shunt_voltmeter(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif //_SENSOR_0042_ADC_SHUNT_VOLTMETER_H_
