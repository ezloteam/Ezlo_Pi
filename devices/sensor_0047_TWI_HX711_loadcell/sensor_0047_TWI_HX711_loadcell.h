#ifndef _0047_TWI_HX711_LOADCELL_H_
#define _0047_TWI_HX711_LOADCELL_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

typedef enum
{
    HX711_GAIN_128 = 2,
    HX711_GAIN_32 = 3,
    HX711_GAIN_64 = 4
} hx711_gain_t;

//-----------------------------------
int sensor_0047_twi_hx711(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *user_args);

#endif