#ifndef _0057_OTHER_KY026_FLAMEDETECTOR_H_
#define _0057_OTHER_KY026_FLAMEDETECTOR_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

/**
 *  NOTE : Flame Sensor [KY-026 module] gives (0V - 5V) analog output .
 *  But ESP32- only allows upto 2.4V max input.
 *
 *  Remedy:  introduce a voltage divider of ratio [1:2 , i.e. 50%]  on the Sensor analog output ,
 *           so that esp32 adc pin recieves half voltage only.
 *           (Half of 5V) -> 2.5V
 *
 *
 *
 *  >   +Vc[5V] ------------+
 *                          |
 *                       {1KOhm}
 *                          |
 *                          +--------------+ ~2.5V
 *                          |              ^
 *                       {1kOhm}           | esp32 analog input
 *                          |              v
 *  >     [0V] -------------+--------------+ 0V
 *
 *
 **/

//------------------------------------------
/**
 * Please don't forget to uncomment ,
 * -> If you added a voltage divider at sensor's analog output. [Make sure voltage does not exceed 2.5V]
 *      |
 *      |
 *  #define VOLTAGE_DIVIDER_ADDED 1
 */
#define VOLTAGE_DIVIDER_ADDED 1
//------------------------------------------
typedef enum
{
    KY206_HEAT_ALARM_HEAT_OK,
    KY206_HEAT_ALARM_OVERHEAT_DETECTED,
    KY206_HEAT_ALARM_UNDERHEAT_DETECTED,
    KY206_HEAT_ALARM_UNKNOWN,
    KY206_HEAT_ALARM_MAX
} e_ky206_heat_alarm_states_t;

//----------------------------------------------------
int sensor_0057_other_KY026(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *user_args);

#endif // _0057_OTHER_KY026_FLAMEDETECTOR_H_