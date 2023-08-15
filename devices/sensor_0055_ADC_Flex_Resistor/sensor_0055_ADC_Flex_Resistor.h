#ifndef _0054_ADC_FLEX_RESISTOR_H_
#define _0054_ADC_FLEX_RESISTOR_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

/**
 *  NOTE : MQ8_hydrogen_gas - module gives (0V - 5V) as analog output .
 *  But ESP32- only allows upto 2.4V max input.
 *
 *  Remedy:  introduce a voltage divider of ratio [1:2 , i.e. 50%]  on the Sensor analog output , so that esp32 adc pin recieves half voltage only.
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
 *                 Rout = {10kOhm}        esp32 analog input [Vo]
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

#define flex_Rout 10000 // minimum Rout = 10KOhm
#define flex_Vin 5      // Vin = 5V  (input)

//-----------------------------------------------------------------------------------------------------------------------------
int sensor_0055_flex_resistor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_args);

#endif