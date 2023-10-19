#ifndef SENSOR_0012_I2C_BME280_H
#define SENSOR_0012_I2C_BME280_H

#include "driver/i2c.h"
#include "sensor_bme280.h"
#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

#define CHIP_ID 0x76
#define ACK_CHECK_EN 0x1

typedef struct s_ezlopi_bmp280{
    bmp280_params_t bmp280_params;
    bmp280_t bmp280_dev;
    float temperature;
    float pressure;
    float humidity;
}s_ezlopi_bmp280_t;

int sensor_0012_I2C_BME280(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // SENSOR_0012_I2C_BME280_H
