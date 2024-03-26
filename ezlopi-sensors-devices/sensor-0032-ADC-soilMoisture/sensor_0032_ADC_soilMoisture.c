
#include <math.h>
#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0032_ADC_soilMoisture.h"

static int __prepare(void* arg);
static int __init(l_ezlopi_item_t* item);
static int __notify(l_ezlopi_item_t* item);
static int __get_cjson_value(l_ezlopi_item_t* item, void* arg);

int sensor_0032_ADC_soilMoisture(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
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

static int __notify(l_ezlopi_item_t* item)
{
    int ret = 0;

    double* soil_moisture_data = (double*)item->user_arg;
    if (soil_moisture_data)
    {
        s_ezlopi_analog_data_t tmp_data = { .value = 0, .voltage = 0 };
        ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &tmp_data);
        double percentage = ((4095 - tmp_data.value) / 4096.0) * 100;
        if (fabs(percentage - *soil_moisture_data) > 1.0)
        {
            *soil_moisture_data = percentage;
            ezlopi_device_value_updated_from_device_v3(item);
        }
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cj_result = (cJSON*)arg;
        
        double* soil_moisture_data = (double*)item->user_arg;
        if (soil_moisture_data)
        {
            cJSON_AddNumberToObject(cj_result, ezlopi_value_str, *soil_moisture_data);
            char* valueFormatted = ezlopi_valueformatter_double(*soil_moisture_data);
            if (valueFormatted)
            {
                cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
            ret = 1;
        }
    }
    return ret;
}

static int __init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
        {
            if (0 == ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
            {
                ret = 1;
            }
            else
            {
                ret = -1;
            }
        }
        else
        {
            ret = -1;
        }
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    // cJSON *cj_info = cJSON_CreateObject();
    // cJSON_AddStringToObject(cj_info, ezlopi_manufacturer_str, "EzloPi");
    // cJSON_AddStringToObject(cj_info, ezlopi_model_str, "EzloPi Generic");
    // cJSON_AddStringToObject(cj_info, "protocol", "WiFi");
    // cJSON_AddStringToObject(cj_info, "firmware.stack", "3.0.4");
    // cJSON_AddStringToObject(cj_info, "hardware", "ESP32");
    device->cloud_properties.category = category_generic_sensor;
    device->cloud_properties.subcategory = subcategory_moisture;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_soil_humidity;
    item->cloud_properties.value_type = value_type_humidity;
    item->cloud_properties.scale = scales_percent;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;
    item->interface.adc.resln_bit = 3;
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_str, item->interface.adc.gpio_num);
    item->user_arg = user_arg;
}

static int __prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;

    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t* device = ezlopi_device_add_device(prep_arg->cjson_device);
        if (device)
        {
            __prepare_device_cloud_properties(device, prep_arg->cjson_device);
            l_ezlopi_item_t* item_temperature = ezlopi_device_add_item_to_device(device, sensor_0032_ADC_soilMoisture);
            if (item_temperature)
            {
                double* soil_moisture_data = (double*)malloc(sizeof(double));
                if (soil_moisture_data)
                {
                    memset(soil_moisture_data, 0, sizeof(double));
                    __prepare_item_properties(item_temperature, prep_arg->cjson_device, (void*)soil_moisture_data);
                }
                ret = 1;
            }
            else
            {
                ezlopi_device_free_device(device);
                ret = -1;
            }
        }
    }
    return ret;
}
