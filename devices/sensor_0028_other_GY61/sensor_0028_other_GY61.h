#ifndef _0028_SENS_GY61_OTHER_H_
#define _0028_SENS_GY61_OTHER_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

// 1g = 9.80665 m/s^2
#define GY61_STANDARD_G_TO_ACCEL_CONVERSION_VALUE 9.80665f

int sensor_0028_other_GY61(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);

#endif //_0028_SENS_GY61_OTHER_H_