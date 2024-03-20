#include "ezlopi_util_trace.h"
#include "cJSON.h"
#include <math.h>

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0041_ADC_FC28_soilMoisture.h"

typedef struct s_fc28_data
{
    uint32_t hum_val;
} s_fc28_data_t;

static int __0041_prepare(void* arg);
static int __0041_init(l_ezlopi_item_t* item);
static int __0041_get_cjson_value(l_ezlopi_item_t* item, void* arg);
static int __0041_notify(l_ezlopi_item_t* item);

//--------------------------------------------------------------------------------------------------------
int sensor_0041_ADC_FC28_soilMoisture(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0041_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0041_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0041_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __0041_notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}
//-------------------------------------------------------------------------------------------------------------------------
static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    char* dev_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, dev_name);
    ASSIGN_DEVICE_NAME_V2(device, dev_name);
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    device->cloud_properties.category = category_humidity;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type = dev_type_sensor;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, void* user_data)
{
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_moisture;
    item->cloud_properties.show = true;
    item->cloud_properties.value_type = value_type_moisture;
    item->cloud_properties.scale = scales_percent;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = user_data;
}
static void __prepare_item_interface_properties(l_ezlopi_item_t* item, cJSON* cj_device)
{
    if (item && cj_device)
    {
        item->interface_type = EZLOPI_DEVICE_INTERFACE_MAX; // other
        CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_gpio_str, item->interface.adc.gpio_num);
        item->interface.adc.resln_bit = 3;
    }
}
//-------------------------------------------------------------------------------------------------------------------------

static int __0041_prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* device_prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        cJSON* cj_device = device_prep_arg->cjson_device;

        s_fc28_data_t* user_data = (s_fc28_data_t*)malloc(sizeof(s_fc28_data_t));
        if (NULL != user_data)
        {
            memset(user_data, 0, sizeof(s_fc28_data_t));
            l_ezlopi_device_t* fc28_device = ezlopi_device_add_device(cj_device);
            if (fc28_device)
            {
                __prepare_device_cloud_properties(fc28_device, cj_device);

                l_ezlopi_item_t* fc28_item = ezlopi_device_add_item_to_device(fc28_device, sensor_0041_ADC_FC28_soilMoisture);
                if (fc28_item)
                {
                    __prepare_item_cloud_properties(fc28_item, user_data);
                    __prepare_item_interface_properties(fc28_item, cj_device);
                    ret = 1;
                }
                else
                {
                    ret = -1;
                    ezlopi_device_free_device(fc28_device);
                    free(user_data);
                }
            }
            else
            {
                ret = -1;
                free(user_data);
            }
        }
    }
    return ret;
}

static int __0041_init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (NULL != item)
    {
        s_fc28_data_t* user_data = (s_fc28_data_t*)item->user_arg;
        if (user_data)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
            {
                if (0 == ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
                {
                    ret = 1;
                }
                // else
                // {
                //     ret = -1;
                //     free(item->user_arg); // this will free ; memory address linked to all items
                //     item->user_arg = NULL;
                //     // ezlopi_device_free_device_by_item(item);
                // }
            }
            // else
            // {
            //     ret = -1;
            //     // ezlopi_device_free_device_by_item(item);
            // }
        }
        // else
        // {
        //     ret = -1;
        //     ezlopi_device_free_device_by_item(item);
        // }
    }
    return ret;
}

static int __0041_get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cj_result = (cJSON*)arg;
        if (cj_result)
        {
            s_fc28_data_t* user_data = (s_fc28_data_t*)item->user_arg;
            if (user_data)
            {
                cJSON_AddNumberToObject(cj_result, ezlopi_value_str, (user_data->hum_val));
                char* valueFormatted = ezlopi_valueformatter_uint32(user_data->hum_val);
                if (valueFormatted)
                {
                    cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                    // TRACE_S("soil moisture  : %d", user_data->hum_val);
                    free(valueFormatted);
                }
                ret = 1;
            }
        }
    }
    return ret;
}

static int __0041_notify(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        s_fc28_data_t* user_data = (s_fc28_data_t*)item->user_arg;
        if (user_data)
        {
            s_ezlopi_analog_data_t ezlopi_adc_data = { .value = 0, .voltage = 0 };
            ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_adc_data);
            uint32_t new_hum = ((4095.0f - (ezlopi_adc_data.value)) / 4095.0f) * 100;
            TRACE_S("[%dmv] soil moisture  : %d", ezlopi_adc_data.voltage, new_hum);

            if (fabs((user_data->hum_val) - new_hum) > 0.5) // percent
            {
                user_data->hum_val = new_hum;
                ezlopi_device_value_updated_from_device_v3(item);
            }
            ret = 1;
        }
    }
    return ret;
}
