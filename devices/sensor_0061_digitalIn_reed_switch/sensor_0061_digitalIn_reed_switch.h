#ifndef _SENSOR_0061_DIGITALIN_REED_SWITCH_H_
#define _SENSOR_0061_DIGITALIN_REED_SWITCH_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
/**
 *                          REED switch setup
 * [3.3V] +Vin ------->-----+
 *                          |
 *                          |     Reed Switch
 *                          |          /
 *                   _______+-------->/  |<-------+ [gpio]
 *                   ^      |                     ^
 *                   |      |  [constant]         |
 *                   |      |                     |
 *              Resistor {~10kOhm}           esp32 analog input
 *                   |      |                     |
 *                   |      |                     |
 *                   V      |                     v
 *      [0V] ---------------+--> 0V --------------+ 0V
 */
//----------------------------------------------------------------------
int sensor_0061_digitalIn_reed_switch(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);

#endif //_SENSOR_0061_DIGITALIN_REED_SWITCH_H_