#include <string.h>
#include "cJSON.h"

#include "trace.h"
#include "0012_bme280_sensor.h"
#include "ezlopi_cloud.h"
#include "ezlopi_timer.h"
#include "ezlopi_actions.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"

static int __prepare(void *arg);
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);

static void user_delay_us(uint32_t period, void *intf_ptr);
static int8_t user_i2c_read(uint8_t reg_addr, uint8_t *sensor_data, uint32_t len, void *intf_ptr);
static int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *sensor_data, uint32_t len, void *intf_ptr);

typedef struct s_bme280_hal_arg
{
    uint32_t addr;
    uint32_t channel;
} s_bme280_hal_arg_t;

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
 * @brief Public function to interface bme280. This is used to handles all the action on the bme280 sensor and is the entry point to interface the sensor.
 *
 * @param action e_ezlopi_actions_t
 * @param arg Other arguments if needed
 * @return int
 */
int sensor_bme280(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        break;
    }
    default:
    {
        break;
    }
    }
    return 0;
}


static void __prepare_sensor_config(sensor_bme280_dev_t *sensor_config, cJSON *cjson_device, s_bme280_hal_arg_t *bme280_hal_arg)
{
    sensor_config->read = user_i2c_read;
    sensor_config->write = user_i2c_write;
    sensor_config->delay_us = user_delay_us;
    sensor_config->intf = BME280_I2C_INTF;
    sensor_config->settings.osr_h = BME280_OVERSAMPLING_1X;
    sensor_config->settings.osr_p = BME280_OVERSAMPLING_1X;
    sensor_config->settings.osr_t = BME280_OVERSAMPLING_1X;
    sensor_config->settings.filter = BME280_FILTER_COEFF_16;
    sensor_config->intf_ptr = (void *)bme280_hal_arg;
    CJSON_GET_VALUE_INT(cjson_device, "slave_addr", sensor_config->chip_id);
}

static int __prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if(prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *device = ezlopi_device_add_device();
        if(device)
        {
            
        }
    }

    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_humidity;
    device->cloud_properties.subcategory = subcategory_not_defined;
    
}

/**
 * @brief Static function to initialize the bme280 sensor.
 *
 * @return returns 0 for successful initialization.
 */
// static int sensor_bme280_init(s_ezlopi_device_properties_t *properties, void *user_arg)
// {
//     int ret = 0;
//     sensor_bme280_dev_t *sensor_config = (sensor_bme280_dev_t *)user_arg;
//     ezlopi_i2c_master_init(&properties->interface.i2c_master);

//     TRACE_I("I2C master init successfully.");
//     uint8_t sampling_settting = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

//     ret = bme280_init(&device);
//     if (ret != BME280_OK)
//     {
//         TRACE_E("Failed to initialize bme280 (code %+d).", ret);
//     }
//     else
//     {
//         TRACE_I("Sensor bme280 was successfully initialized.");
//     }

//     ret = bme280_set_sensor_settings(sampling_settting, &device);
//     if (ret != BME280_OK)
//     {
//         TRACE_E("Failed to set sensor settings (code %+d).", ret);
//     }
//     else
//     {
//         TRACE_I("Sensor setting was successfully set.");
//     }
//     return ret;
// }



/**
 * @brief static function to read data from the sensor.
 *
 * @return return `0` if everything is successfuly done.
 */
static int8_t sensor_bme280_read_value_from_sensor(s_ezlopi_device_properties_t *properties, sensor_bme280_data_t *data_ptr)
{
    int8_t ret = bme280_set_sensor_mode(BME280_FORCED_MODE, &device);
    if (ret != BME280_OK)
    {
        TRACE_E("Failed to set sensor mode (code %+d).", ret);
    }
    else
    {
        TRACE_I("Sensor mode set successfully!!");
    }

    ret = bme280_get_sensor_data(BME280_ALL, data_ptr, &device);
    if (ret != BME280_OK)
    {
        TRACE_E("Failed to get sensor data (code %+d).", ret);
    }
    else
    {
        TRACE_I("Sensor mode obtained successfully!!");
    }

    return ret;
}

static int8_t user_i2c_read(uint8_t register_addr, uint8_t *data, uint32_t len, void *intf_ptr)
{
    i2c_master_write_read_device(I2C_NUM_0, CHIP_ID, &register_addr, 1, data, len, 1000 / portTICK_RATE_MS);
    return 0;
}

static void user_delay_us(uint32_t period, void *intf_ptr)
{
    vTaskDelay(period / portTICK_PERIOD_MS);
}

static int8_t user_i2c_write(uint8_t register_addr, const uint8_t *data, uint32_t len, void *intf_ptr)
{
    i2c_master_write_read_device(I2C_NUM_0, CHIP_ID, &register_addr, 1, data, len, 1000 / portTICK_RATE_MS);
    return 0;
}
