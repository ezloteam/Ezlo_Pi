#ifndef SENSOR_0029_I2C_GXHTC3_H
#define SENSOR_0029_I2C_GXHTC3_H

#include "gxhtc3.h"

#define FLOAT_EPSILON 0.5

typedef struct s_gxhtc3_value
{
    float temperature;
    float humidity;
    s_gxhtc3_sensor_handler_t *gxhtc3;
} s_gxhtc3_value_t;

int sensor_0029_I2C_GXHTC3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // SENSOR_0029_I2C_GXHTC3_H
