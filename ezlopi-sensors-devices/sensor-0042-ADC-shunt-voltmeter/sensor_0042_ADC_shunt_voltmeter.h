#ifndef _SENSOR_0042_ADC_SHUNT_VOLTMETER_H_
#define _SENSOR_0042_ADC_SHUNT_VOLTMETER_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_errors.h"

/**
 * SHUNT VOLTAGE -> measures DC voltage [0V - 25V]
 *
 *  NOTE : Shunt Voltage - module gives (0V - 5V) as analog output .
 *  But ESP32- only allows upto 2.4V max input.
 *
 *  Remedy:  introduce a voltage divider of ratio [1:2 , i.e. 50%]  on the Sensor analog output , so that esp32 adc pin recieves half voltage only.
 *             ( Half of 5) -> ~2.5V
 *
 *       +------------>+
 *       |             |
 *       |             |
 *       |          {30Kohm}
 *       |             |
 *       |             |
 *   [1v-25v]          +----->{X}V<-----+
 *       |             |                |
 *       |             |               {2.7k or 3.3K}
 *       |             |                |
 *       |          {7.5Kohm}           +------------+ 100mV~2500mV
 *       |             |                |                ^
 *       |             |               {9.4K or 10k}     |  esp32 analog input
 *       |             |                |                v
 *       +------------>+----->{0}V<-----+------------+ 0V
 **/

//------------------------------------------

/**
 * Please don't forget to uncomment ,
 * -> If you added a voltage divider at sensor's analog output. [Make sure voltage does not exceed 2.5V]
 *      |
 *      |
 *      V
 */
// #define VOLTAGE_DIVIDER_EN 1

//------------------------------------------

ezlopi_error_t sensor_0042_ADC_shunt_voltmeter(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif //_SENSOR_0042_ADC_SHUNT_VOLTMETER_H_
