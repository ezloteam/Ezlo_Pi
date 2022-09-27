#ifndef __SENSOR_BME280_H__
#define __SENSOR_BME280_H__

#include "ezlopi_actions.h"

#define SENSOR_0010_BME280

// int _sensor_bme280(e_ezlopi_actions_t action, void *arg);
int sensor_bme280(e_ezlopi_actions_t action, void *arg);

// int sensor_bme280(e_ezlopi_actions_t action, void *arg)
// {
//     return 0;
// }

#endif // __SENSOR_BME290_H__
