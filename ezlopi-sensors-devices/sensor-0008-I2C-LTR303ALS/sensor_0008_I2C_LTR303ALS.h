

#ifndef __SENSOR_0008_I2C_LTR303ALS_H__
#define __SENSOR_0008_I2C_LTR303ALS_H__

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"

#define LTR303_I2C_CHANNEL I2C_NUM_0

int sensor_0008_I2C_LTR303ALS(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // __SENSOR_0008_I2C_LTR303ALS_H__
