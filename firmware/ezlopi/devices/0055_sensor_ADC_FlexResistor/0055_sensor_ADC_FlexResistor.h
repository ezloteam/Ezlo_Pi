#ifndef _0055_SENSOR_ADC_FLEXRESISTOR_H_
#define _0055_SENSOR_ADC_FLEXRESISTOR_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

/**
 *  NOTE : FlexResistor module gives (0V - 5V) as analog output .
 *  But ESP32- only allows upto 2.4V max input.
 *
 *  Remedy:  introduce 10kOhm [Rout] to achieve voltage divider of ratio [1:2 , i.e. 50%]  on the Sensor analog output ,
 *           so that esp32 adc pin recieves half voltage only.
 *             (Half of 5) -> 2.4V
 *
 *
 *              [flex resistor]
 *              [10kOhm - 110kOhm]
 *   +Vin[5V] ---{Rs?}------+
 *                          |
 *                          |
 *                          |
 * (Multimeter read) _______+--> 2.5V------+
 *                   ^      |              ^
 *                   |      |  [constant]  |
 *                   |      |              |
 *                Rout = {~10kOhm}        esp32 analog input [Vo]
 *                   |      |              |
 *                   |      |              |
 *                   V      |              v
 *      [0V] ---------------+--> 0V--------+------------+ 0V
 *
 *
 *
 *  // Here to find the 'Rs' value we can use voltage divider rule
 *          Vo = (Rout / Rout + Rs) * Vin ....................(1)
 *      or, Vo = (10K / 10K + Rs) * Vin
 *      or, Rs = (10K/Vo)*Vin - 10K
 *      or, Rs = [(Vin/Vo) - 1] * 10K ......................(2)
 *
 *      Now putting Vin = 5V, in eqn(2),
 *          Rs = [(5V / Vo) - 1] * 10K  ....................(3)
 *
 *      So using equation(3) and 'Vo' voltage value we get the required resistance value 'Rs'
 **/

// measure the resistor value using multimeter
#define flex_Rout 10000.0f // minimum Rout = 10KOhm
#define flex_Vin 5.0f      // Vin = 5V  (default) // if [3.3V] is used instead of [5.0V], Change [flex_Vin=> 5.0f to 3.3f]

//-----------------------------------------------------------------------------------------------------------------------------
int sensor_0055_flexResistor_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif