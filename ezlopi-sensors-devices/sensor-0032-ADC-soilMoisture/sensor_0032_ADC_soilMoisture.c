
#include <math.h>
#include "ezlopi_util_trace.h"

// #include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0032_ADC_soilMoisture.h"
#include "EZLOPI_USER_CONFIG.h"

static ezlopi_error_t __prepare(void* arg);
static ezlopi_error_t __init(l_ezlopi_item_t* item);
static ezlopi_error_t __notify(l_ezlopi_item_t* item);
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t* item, void* arg);

ezlopi_error_t sensor_0032_ADC_soilMoisture(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
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
        __get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t* item)
{
    ezlopi_error_t ret = EZPI_FAILED;

    double* soil_moisture_data = (double*)item->user_arg;
    if (soil_moisture_data)
    {
        s_ezlopi_analog_data_t tmp_data = { .value = 0, .voltage = 0 };
        ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &tmp_data);
        double percentage = ((4095 - tmp_data.value) / 4096.0) * 100;
        if (fabs(percentage - *soil_moisture_data) > 1.0)
        {
            *soil_moisture_data = percentage;
            ezlopi_device_value_updated_from_device_broadcast(item);
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON* cj_result = (cJSON*)arg;

        double* soil_moisture_data = (double*)item->user_arg;
        if (soil_moisture_data)
        {
            ezlopi_valueformatter_double_to_cjson(cj_result, *soil_moisture_data, scales_percent);
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t* item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
        {
            if (EZPI_SUCCESS == ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
            {
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    // cJSON *cj_info = cJSON_CreateObject(__FUNCTION__);
    // cJSON_AddStringToObject(__FUNCTION__, cj_info, ezlopi_manufacturer_str, "EzloPi");
    // cJSON_AddStringToObject(__FUNCTION__, cj_info, ezlopi_model_str, "EzloPi Generic");
    // cJSON_AddStringToObject(__FUNCTION__, cj_info, "protocol", "WiFi");
    // cJSON_AddStringToObject(__FUNCTION__, cj_info, "firmware.stack", "3.0.4");
    // cJSON_AddStringToObject(__FUNCTION__, cj_info, "hardware", "ESP32");
    device->cloud_properties.category = category_generic_sensor;
    device->cloud_properties.subcategory = subcategory_moisture;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_soil_humidity;
    item->cloud_properties.value_type = value_type_humidity;
    item->cloud_properties.scale = scales_percent;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;
    item->interface.adc.resln_bit = 3;
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.adc.gpio_num);

    item->is_user_arg_unique = true;
    item->user_arg = user_arg;
}

static ezlopi_error_t __prepare(void* arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;

    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t* device = ezlopi_device_add_device(prep_arg->cjson_device, NULL);
        if (device)
        {
            __prepare_device_cloud_properties(device, prep_arg->cjson_device);
            l_ezlopi_item_t* item_temperature = ezlopi_device_add_item_to_device(device, sensor_0032_ADC_soilMoisture);
            if (item_temperature)
            {
                double* soil_moisture_data = (double*)ezlopi_malloc(__FUNCTION__, sizeof(double));
                if (soil_moisture_data)
                {
                    memset(soil_moisture_data, 0, sizeof(double));
                    __prepare_item_properties(item_temperature, prep_arg->cjson_device, (void*)soil_moisture_data);
                    ret = EZPI_SUCCESS;
                }
            }
            else
            {
                ezlopi_device_free_device(device);
            }
        }
    }
    return ret;
}
