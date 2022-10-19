#include <string.h>

#include "sensor_bme280.h"
#include "ezlopi_actions.h"
#include "ezlopi_sensors.h"
#include "ezlopi_timer.h"
#include "items.h"
#include "frozen.h"
#include "trace.h"

bme280_identifier_t identifier = {
    .dev_addr = 0x76,
};

static sensor_bme280_dev_t device = {
    .intf = BME280_I2C_INTF,
    .read = user_i2c_read,
    .write = user_i2c_write,
    .delay_us = user_delay_us,
    .settings = {
        .osr_h = BME280_OVERSAMPLING_1X,
        .osr_p = BME280_OVERSAMPLING_1X,
        .osr_t = BME280_OVERSAMPLING_1X,
        .filter = BME280_FILTER_COEFF_16,
    },
    .intf_ptr = &identifier,
};

/**
 * @brief static function to read data from the sensor.
 *
 * @return return `0` if everything is successfuly done.
 */
static int8_t sensor_bme280_get_value(char *sensor_data)
{
    int ret = bme280_set_sensor_mode(BME280_FORCED_MODE, &device);
    int data_len = 100;
    if (ret != BME280_OK)
    {
        TRACE_E("Failed to set sensor mode (code %+d).", ret);
    }
    else
    {
        TRACE_I("Sensor mode set successfully!!");
    }

    sensor_bme280_data_t data;
    ret = bme280_get_sensor_data(BME280_ALL, &data, &device);
    if (ret != BME280_OK)
    {
        TRACE_E("Failed to get sensor data (code %+d).", ret);
    }
    else
    {
        TRACE_I("Sensor mode obtained successfully!!");
    }

    float temp = data.temperature;
    float press = 0.01f * data.pressure;
    float hum = data.humidity;

    snprintf(sensor_data, data_len, "\"Temperature: %0.2lf deg C, Pressure: %0.2lf hPa, Humidity: %0.2lf%%\"", temp, press, hum);

    // TRACE_I("Data len=> %d, Temperature: %0.2lf deg C, Pressure: %0.2lf hPa, Humidity: %0.2lf%%\n", sizeof(data), temp, press, hum);

    return ret;
}

static int sensor_bme280_set_value(void *arg)
{
    int ret = 0;

    return ret;
}

static int sensor_bme280_ezlopi_update_data(void)
{
    char *data = (char *)malloc(65);
    char *send_buf = malloc(1024);
    sensor_bme280_get_value(data);
    send_buf = items_update_from_sensor(0, data);
    TRACE_I("The send_buf is: %s, the size is: %d", send_buf, strlen(send_buf));
    free(data);
    free(send_buf);
    return 0;
}

static int sensor_bme280_notify_30_seconds(void)
{
    int ret = 0;

    static int seconds_counter;
    seconds_counter = (seconds_counter % 30) ? seconds_counter : 0;

    if (0 == seconds_counter)
    {
        seconds_counter = 0;
        /* Send the value to cloud using web-socket */
        char *data = sensor_bme280_ezlopi_update_data();
        if (data)
        {
            /* Send to ezlo cloud */
        }
    }

    seconds_counter++;
    return ret;
}

/**
 * @brief Static function to initialize the bme280 sensor.
 *
 * @return returns 0 for successful initialization.
 */
static int sensor_bme280_init()
{
    int ret = 0;

    uint8_t sampling_settting = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    ret = bme280_init(&device);
    if (ret != BME280_OK)
    {
        TRACE_E("Failed to initialize bme280 (code %+d).", ret);
    }
    else
    {
        TRACE_I("Sensor bme280 was successfully initialized.");
    }

    ret = bme280_set_sensor_settings(sampling_settting, &device);
    if (ret != BME280_OK)
    {
        TRACE_E("Failed to set sensor settings (code %+d).", ret);
    }
    else
    {
        TRACE_I("Sensor setting was successfully set.");
    }
    return ret;
}

/**
 * @brief Public function to interface bme280. This is used to handles all the action on the bme280 sensor and is the entry point to interface the sensor.
 *
 * @param action e_ezlopi_actions_t
 * @param arg Other arguments if needed
 * @return int
 */
int sensor_bme280(e_ezlopi_actions_t action, void *arg)
{
    switch (action)
    {
    case EZLOPI_ACTION_INITIALIZE:
    {
        TRACE_I("EZLOPI_ACTION_INITIALIZE event.");
        sensor_bme280_init();
        break;
    }
    case EZLOPI_ACTION_GET_VALUE:
    {
        TRACE_I("EZLOPI_ACTION_GET_VALUE event.");
        char *data = (char *)malloc(100);
        sensor_bme280_get_value(data);
        TRACE_I("The string is: %s", data);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_500_MS:
    {
        TRACE_I("EZLOPI_ACTION_NOTIFY_500_MS");
        sensor_bme280_notify_30_seconds();
        break;
    }
    default:
    {
        TRACE_E("Unknown defaule bm280 action found!");
        break;
    }
    }
    return 0;
}

int8_t user_i2c_read(uint8_t register_addr, uint8_t *data, uint32_t len, void *intf_ptr)
{
    i2c_master_write_read_device(I2C_NUM_0, CHIP_ID, &register_addr, 1, data, len, 1000 / portTICK_RATE_MS);
    return 0;
}

void user_delay_us(uint32_t period, void *intf_ptr)
{
    vTaskDelay(period / portTICK_PERIOD_MS);
    return;
}

int8_t user_i2c_write(uint8_t register_addr, const uint8_t *data, uint32_t len, void *intf_ptr)
{
    i2c_master_write_read_device(I2C_NUM_0, CHIP_ID, &register_addr, 1, data, len, 1000 / portTICK_RATE_MS);
    return 0;
}
