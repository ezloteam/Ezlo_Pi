

#ifndef __0010_SENS_BME680_SENSOR_H_
#define __0010_SENS_BME680_SENSOR_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

#if 0 // v2.x
int sensor_0010_I2C_BME680(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);
#endif

int sensor_0010_I2C_BME680_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // __0010_SENS_BME680_SENSOR_H_
