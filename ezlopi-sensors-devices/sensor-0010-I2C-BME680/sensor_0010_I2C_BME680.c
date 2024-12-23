#include <math.h>
#include "ezlopi_util_trace.h"

// #include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_setting_commands.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_hal_i2c_master.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "bme680_bsec.h"
#include "sensor_0010_I2C_BME680.h"
#include "EZLOPI_USER_CONFIG.h"

static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __notify(l_ezlopi_item_t *item);
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg);

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);

ezlopi_error_t sensor_0010_I2C_BME680(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.show = true;
    item->user_arg = user_arg;

    if (ezlopi_item_name_temp == item->cloud_properties.item_name)
    {
        item->is_user_arg_unique = true;
        item->interface.i2c_master.enable = true;
        item->interface.i2c_master.channel = I2C_NUM_0;
        item->interface.i2c_master.clock_speed = 100000;
        CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
        CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
        CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_slave_addr_str, item->interface.i2c_master.address);
    }
    else
    {
        item->interface.i2c_master.enable = false;
    }
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        cJSON *cj_device = prep_arg->cjson_device;
        bme680_data_t *user_data = (bme680_data_t *)ezlopi_malloc(__FUNCTION__, sizeof(bme680_data_t));
        if (user_data)
        {
            memset(user_data, 0, sizeof(bme680_data_t));
            l_ezlopi_device_t *parent_temp_humid_device = ezlopi_device_add_device(cj_device, "temp_humid");
            if (parent_temp_humid_device)
            {
                TRACE_I("Parent_temp_humid_device-[0x%x] ", parent_temp_humid_device->cloud_properties.device_id);
                parent_temp_humid_device->cloud_properties.category = category_temperature;
                __prepare_device_cloud_properties(parent_temp_humid_device, cj_device);
                l_ezlopi_item_t *temperature_item = ezlopi_device_add_item_to_device(parent_temp_humid_device, sensor_0010_I2C_BME680);
                if (temperature_item)
                {
                    temperature_item->cloud_properties.item_name = ezlopi_item_name_temp;
                    temperature_item->cloud_properties.value_type = value_type_temperature;

                    temperature_item->cloud_properties.scale = ezlopi_core_setting_get_temperature_scale_str();
                    __prepare_cloud_properties(temperature_item, cj_device, user_data);
                }

                l_ezlopi_item_t *humidity_item = ezlopi_device_add_item_to_device(parent_temp_humid_device, sensor_0010_I2C_BME680);
                if (humidity_item)
                {
                    humidity_item->cloud_properties.item_name = ezlopi_item_name_humidity;
                    humidity_item->cloud_properties.value_type = value_type_humidity;
                    humidity_item->cloud_properties.scale = scales_percent;
                    __prepare_cloud_properties(humidity_item, cj_device, user_data);
                }

                l_ezlopi_device_t *child_pressure_device = ezlopi_device_add_device(cj_device, "pressure");
                if (child_pressure_device)
                {
                    TRACE_I("Child_pressure_device-[0x%x] ", child_pressure_device->cloud_properties.device_id);
                    child_pressure_device->cloud_properties.category = category_level_sensor;
                    __prepare_device_cloud_properties(child_pressure_device, cj_device);

                    l_ezlopi_item_t *pressure_item = ezlopi_device_add_item_to_device(child_pressure_device, sensor_0010_I2C_BME680);
                    if (pressure_item)
                    {
                        pressure_item->cloud_properties.item_name = ezlopi_item_name_atmospheric_pressure;
                        pressure_item->cloud_properties.value_type = value_type_pressure;
                        pressure_item->cloud_properties.scale = scales_kilo_pascal;
                        __prepare_cloud_properties(pressure_item, cj_device, user_data);
                    }
                    else
                    {
                        ezlopi_device_free_device(child_pressure_device);
                        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                    }
                }

                l_ezlopi_device_t *child_aqi_device = ezlopi_device_add_device(cj_device, "aqi");
                if (child_aqi_device)
                {
                    TRACE_I("Child_aqi_device-[0x%x] ", child_aqi_device->cloud_properties.device_id);
                    child_aqi_device->cloud_properties.category = category_level_sensor;
                    __prepare_device_cloud_properties(child_aqi_device, cj_device);

                    l_ezlopi_item_t *aqi_item = ezlopi_device_add_item_to_device(child_aqi_device, sensor_0010_I2C_BME680);
                    if (aqi_item)
                    {
                        aqi_item->cloud_properties.item_name = ezlopi_item_name_volatile_organic_compound_level;
                        aqi_item->cloud_properties.value_type = value_type_substance_amount;
                        aqi_item->cloud_properties.scale = scales_parts_per_million;
                        __prepare_cloud_properties(aqi_item, cj_device, user_data);
                    }
                    else
                    {
                        ezlopi_device_free_device(child_aqi_device);
                        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                    }
                }

                l_ezlopi_device_t *child_altitude_device = ezlopi_device_add_device(cj_device, "altitude");
                if (child_altitude_device)
                {
                    TRACE_I("Child_altitude_device-[0x%x] ", child_altitude_device->cloud_properties.device_id);
                    child_altitude_device->cloud_properties.category = category_level_sensor;
                    __prepare_device_cloud_properties(child_altitude_device, cj_device);

                    l_ezlopi_item_t *altitude_item = ezlopi_device_add_item_to_device(child_altitude_device, sensor_0010_I2C_BME680);
                    if (altitude_item)
                    {
                        altitude_item->cloud_properties.item_name = ezlopi_item_name_distance;
                        altitude_item->cloud_properties.value_type = value_type_length;
                        altitude_item->cloud_properties.scale = scales_meter;
                        __prepare_cloud_properties(altitude_item, cj_device, user_data);
                    }
                    else
                    {
                        ezlopi_device_free_device(child_altitude_device);
                        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                    }
                }

                l_ezlopi_device_t *child_co2_device = ezlopi_device_add_device(cj_device, "co2");
                if (child_co2_device)
                {
                    TRACE_I("Child_co2_device-[0x%x] ", child_co2_device->cloud_properties.device_id);
                    child_co2_device->cloud_properties.category = category_level_sensor;
                    __prepare_device_cloud_properties(child_co2_device, cj_device);

                    l_ezlopi_item_t *co2_item = ezlopi_device_add_item_to_device(child_co2_device, sensor_0010_I2C_BME680);
                    if (co2_item)
                    {
                        co2_item->cloud_properties.item_name = ezlopi_item_name_co2_level;
                        co2_item->cloud_properties.value_type = value_type_substance_amount;
                        co2_item->cloud_properties.scale = scales_parts_per_million;
                        __prepare_cloud_properties(co2_item, cj_device, user_data);
                    }
                    else
                    {
                        ezlopi_device_free_device(child_co2_device);
                        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                    }
                }

                if ((NULL == temperature_item) &&
                    (NULL == humidity_item) &&
                    (NULL == child_pressure_device) &&
                    (NULL == child_aqi_device) &&
                    (NULL == child_altitude_device) &&
                    (NULL == child_co2_device))
                {
                    ezlopi_device_free_device(parent_temp_humid_device);
                    ezlopi_free(__FUNCTION__, user_data);
                    ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                }
            }
            else
            {
                ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
            }
        }
        else
        {
            ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
        }
    }
    else
    {
        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    }
    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {
        bme680_data_t *user_data = (bme680_data_t *)item->user_arg;
        if (user_data)
        {
            if (item->interface.i2c_master.enable)
            {
                ezlopi_i2c_master_init(&item->interface.i2c_master);
                bme680_setup(item->interface.i2c_master.sda, item->interface.i2c_master.scl, true);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    if (item && arg)
    {
        cJSON *cj_device = (cJSON *)arg;
        if (cj_device)
        {
            bme680_data_t *user_data = (bme680_data_t *)item->user_arg;
            if (user_data)
            {
                if (ezlopi_item_name_temp == item->cloud_properties.item_name)
                {
                    ezlopi_valueformatter_float_to_cjson(cj_device, user_data->temperature, item->cloud_properties.scale);
                }
                else if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
                {
                    ezlopi_valueformatter_float_to_cjson(cj_device, user_data->humidity, item->cloud_properties.scale);
                }
                else if (ezlopi_item_name_atmospheric_pressure == item->cloud_properties.item_name)
                {
                    ezlopi_valueformatter_float_to_cjson(cj_device, user_data->pressure / 1000.0, item->cloud_properties.scale);
                }
                else if (ezlopi_item_name_volatile_organic_compound_level == item->cloud_properties.item_name)
                {
                    ezlopi_valueformatter_float_to_cjson(cj_device, user_data->iaq, item->cloud_properties.scale);
                }
                else if (ezlopi_item_name_distance == item->cloud_properties.item_name)
                {
                    ezlopi_valueformatter_float_to_cjson(cj_device, user_data->altitude, item->cloud_properties.scale);
                }
                else if (ezlopi_item_name_co2_level == item->cloud_properties.item_name)
                {
                    ezlopi_valueformatter_float_to_cjson(cj_device, user_data->co2_equivalent, item->cloud_properties.scale);
                }
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        bme680_data_t *user_data = (bme680_data_t *)item->user_arg;
        if (user_data)
        {
            float temperature = user_data->temperature;
            float humidity = user_data->humidity;
            float pressure = user_data->pressure;
            float iaq = user_data->iaq;
            float altitude = user_data->altitude;
            float co2_eqv = user_data->co2_equivalent;

            if (true == bme680_get_data(user_data))
            {
                if (ezlopi_item_name_temp == item->cloud_properties.item_name)
                {
                    item->cloud_properties.scale = ezlopi_core_setting_get_temperature_scale_str();
                    e_enum_temperature_scale_t scale_to_use = ezlopi_core_setting_get_temperature_scale();
                    if (TEMPERATURE_SCALE_FAHRENHEIT == scale_to_use)
                    {
                        user_data->temperature = (user_data->temperature * (9.0f / 5.0f)) + 32.0f;
                    }

                    if (fabs(user_data->temperature - temperature) > 0.05)
                    {
                        // this might be an issue, updating new temperature value with previous temperature value
                        user_data->temperature = temperature;
                        ezlopi_device_value_updated_from_device_broadcast(item);
                        ret = EZPI_SUCCESS;
                    }
                }
                if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
                {
                    if (fabs(user_data->humidity - humidity) > 0.05)
                    {
                        user_data->humidity = humidity;
                        ezlopi_device_value_updated_from_device_broadcast(item);
                        ret = EZPI_SUCCESS;
                    }
                }
                if (ezlopi_item_name_atmospheric_pressure == item->cloud_properties.item_name)
                {
                    if (fabs((user_data->pressure / 1000.0f) - (pressure / 1000.0f)) > 0.05)
                    {
                        user_data->pressure = pressure;
                        ezlopi_device_value_updated_from_device_broadcast(item);
                        ret = EZPI_SUCCESS;
                    }
                }
                if (ezlopi_item_name_volatile_organic_compound_level == item->cloud_properties.item_name)
                {
                    if (fabs(user_data->iaq - iaq) > 0.05)
                    {
                        user_data->iaq = iaq;
                        ezlopi_device_value_updated_from_device_broadcast(item);
                        ret = EZPI_SUCCESS;
                    }
                }
                if (ezlopi_item_name_distance == item->cloud_properties.item_name)
                {
                    if (fabs(user_data->altitude - altitude) > 0.05)
                    {
                        user_data->altitude = altitude;
                        ezlopi_device_value_updated_from_device_broadcast(item);
                        ret = EZPI_SUCCESS;
                    }
                }
                if (ezlopi_item_name_co2_level == item->cloud_properties.item_name)
                {
                    if (fabs(user_data->co2_equivalent - co2_eqv) > 0.05)
                    {
                        user_data->co2_equivalent = co2_eqv;
                        ezlopi_device_value_updated_from_device_broadcast(item);
                        ret = EZPI_SUCCESS;
                    }
                }
            }
        }
    }

    return ret;
}