#ifndef _SENSOR_0028_OTHER_GY61_H_
#define _SENSOR_0028_OTHER_GY61_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"

// 1g = 9.80665 m/s^2
#define GY61_STANDARD_G_TO_ACCEL_CONVERSION_VALUE 9.80665f

int sensor_0028_other_GY61(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif //_SENSOR_0028_OTHER_GY61_H_