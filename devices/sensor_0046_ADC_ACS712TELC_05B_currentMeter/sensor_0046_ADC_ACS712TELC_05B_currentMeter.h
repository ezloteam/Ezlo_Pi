#ifndef _0046_ADC_ACS712TELC_05B_CURRENTMETER_H_
#define _0046_ADC_ACS712TELC_05B_CURRENTMETER_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

/**
 * Chip model : ACS712ELCTR-05B-T
 *
 * Optimized Measurement Range (Ip) = +-5A
 *
 * Sensitivity, Sens(Typ) (mV/A) = 185mv/A
 * */

/**
 *  NOTE : ACS712ELCTR-05B-T - module gives (0V - 5V) as analog output .
 *  But ESP32- only allows upto 2.4V max input.
 *
 *  Remedy:  introduce a voltage divider of ratio [1:2 , i.e. 50%]  on the Sensor analog output , so that esp32 adc pin recieves half voltage only.
 *             ( Half of 4.2) -> 2.1V
 *
 *            > 5V------,
 *                      |
 *                     {1KOhm}
 *                      |
 *                      +------------+ 2.4V
 *                      |                ^
 *                     {1KOhm}           |  esp32 analog input
 *                      |                v
 *            > 0V------+------------+ 0V
 *
 *
 **/

//------------------------------------------

/**
 * Please don't forget to uncomment ,
 * -> If you added a voltage divider at sensor's analog output. [Make sure voltage does not exceed 2.5V]
 *      |
 *      |
 *      V
 */
// #define voltage_divider_added 1

//------------------------------------------
#define ASC712TELC_05B_zero_point_mV 2350 // adc_value at the output ; when no current in input side
#define DEFAULT_AC_FREQUENCY 50
//------------------------------------------

int sensor_0046_ADC_ACS712TELC_05B_currentMeter(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *user_args);

#endif // _0046_ADC_ACS712TELC_05B_CURRENTMETER_H_