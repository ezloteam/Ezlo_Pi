#include <string.h>

#include "sensor_bme280.h"
#include "ezlopi_actions.h"
// #include "ezlopi_sensors.h"
#include "ezlopi_timer.h"
#include "items.h"
#include "frozen.h"
#include "trace.h"
#include "cJSON.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"



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

static int sensor_ble280_prepare(void* arg);
static int8_t sensor_bme280_get_value(char *sensor_data);
static int sensor_bme280_set_value(void *arg);
static int sensor_bme280_ezlopi_update_data(void);
static int sensor_bme280_notify_30_seconds(void);
static int sensor_bme280_init();


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
        case EZLOPI_ACTION_PREPARE:
        {
            TRACE_I("%s", ezlopi_actions_to_string(action));

            break;
        }
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
            TRACE_E("Action not implemented found!");
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

static int sensor_ble280_prepare(void* arg)
{
     cJSON* cjson_device = (cJSON*)arg;
    s_ezlopi_device_properties_t* sensor_ble280_properties = NULL;
    if((NULL == sensor_ble280_properties) && (NULL != cjson_device))
    {
        sensor_ble280_properties = malloc(sizeof(s_ezlopi_device_properties_t));
        if (sensor_ble280_properties)
        {
            int tmp_var = 0;
            memset(sensor_ble280_properties, 0, sizeof(s_ezlopi_device_properties_t));
            sensor_ble280_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;

            char *device_name = NULL;
            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
            ASSIGN_DEVICE_NAME(sensor_ble280_properties, device_name);
            sensor_ble280_properties->ezlopi_cloud.category = category_not_defined;
            sensor_ble280_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
            sensor_ble280_properties->ezlopi_cloud.item_name = ezlopi_item_name_pressure;
            sensor_ble280_properties->ezlopi_cloud.device_type = "";
            sensor_ble280_properties->ezlopi_cloud.value_type = value_type_bool;
            sensor_ble280_properties->ezlopi_cloud.has_getter = true;
            sensor_ble280_properties->ezlopi_cloud.has_setter = false;
            sensor_ble280_properties->ezlopi_cloud.reachable = true;
            sensor_ble280_properties->ezlopi_cloud.battery_powered = false;
            sensor_ble280_properties->ezlopi_cloud.show = true;
            sensor_ble280_properties->ezlopi_cloud.room_name[0] = '\0';
            sensor_ble280_properties->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
            sensor_ble280_properties->ezlopi_cloud.room_id = ezlopi_device_generate_room_id();
            sensor_ble280_properties->ezlopi_cloud.item_id = ezlopi_device_generate_item_id();
            // CJSON_GET_VALUE_INT(cjson_device, "id_room", sensor_ble280_properties->ezlopi_cloud.room_id);
            // CJSON_GET_VALUE_INT(cjson_device, "id_item", sensor_ble280_properties->ezlopi_cloud.item_id);

            CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_ble280_properties->interface.gpio.gpio_in.gpio_num);
            CJSON_GET_VALUE_INT(cjson_device, "ip_inv", sensor_ble280_properties->interface.gpio.gpio_in.invert);
            CJSON_GET_VALUE_INT(cjson_device, "val_ip", sensor_ble280_properties->interface.gpio.gpio_in.value);

            sensor_ble280_properties->interface.gpio.gpio_in.enable = true;
            sensor_ble280_properties->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
            sensor_ble280_properties->interface.gpio.gpio_in.pull = GPIO_PULLUP_ONLY;            
        }
    }

    return ((int)sensor_ble280_properties); 
    
}

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
    // send_buf = items_update_from_sensor(0, data);
    // TRACE_I("The send_buf is: %s, the size is: %d", send_buf, strlen(send_buf));
    free(data);
    // free(send_buf);
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
