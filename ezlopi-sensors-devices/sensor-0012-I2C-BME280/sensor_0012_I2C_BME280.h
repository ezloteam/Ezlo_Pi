#ifndef SENSOR_0012_I2C_BME280_H
#define SENSOR_0012_I2C_BME280_H

// #include "driver/i2c.h"
#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "sensor_bme280.h"

#define CHIP_ID 0x76
#define ACK_CHECK_EN 0x1

int sensor_0012_I2C_BME280(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // SENSOR_0012_I2C_BME280_H
