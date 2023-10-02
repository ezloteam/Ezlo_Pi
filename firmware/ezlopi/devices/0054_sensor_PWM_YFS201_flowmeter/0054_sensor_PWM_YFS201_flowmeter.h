#ifndef _0054_SENSOR_PWM_YFS201_FLOWMETER_H_
#define _0054_SENSOR_PWM_YFS201_FLOWMETER_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
/**
 *  This sensor basically consists of a plastic valve body, a rotor and a hall effect sensor. The pinwheel rotor rotates when water / liquid flows through the valve and its speed will be directly proportional to the flow rate. The hall effect sensor will provide an electrical pulse with every revolution of the pinwheel rotor. This water flow sensor module can be easily interfaced with Microcontrollers, Arduino Boards and Raspberry Pi. Connect the PWM output of this module to interrupt pin of microcontroller unit and count the number of pulses / interrupt per unit time.
 *  The rate of water flow will be directly proportional to the number of pulses counted.
 *
 *      +-----------------------+
 *      | Flow[L/H] | Freq(Hz)  |
 *      +-----------------------+
 *      |     120   |   16      |
 *      |     240   |   32.5    |
 *      |     360   |   49.3    |
 *      |     480   |   65.5    |
 *      |     600   |   82      |
 *      |     720   |   90.2    |
 *      +-----------------------+
 */

#define YFS201_QUEUE_SIZE 5

typedef enum YFS201_queue_enum
{
    YFS201_QUEUE_RESET = 0,
    YFS201_QUEUE_AVAILABLE,
    YFS201_QUEUE_FULL,
} YFS201_queue_enum_t;

//-----------------------------------------------------------------------------------------------
int sensor_0054_pwm_flowmeter_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif