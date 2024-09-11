

#ifndef _SENSOR_0008_I2C_LTR303ALS_H_
#define _SENSOR_0008_I2C_LTR303ALS_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_errors.h"

#define LTR303_I2C_CHANNEL I2C_NUM_0

ezlopi_error_t sensor_0008_I2C_LTR303ALS(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // _SENSOR_0008_I2C_LTR303ALS_H_
