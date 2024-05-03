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

#include "sensor_0042_ADC_shunt_voltmeter.h"
#include "EZLOPI_USER_CONFIG.h"

typedef struct s_voltmeter
{
    float volt;
} s_voltmeter_t;

//------------------------------------------------------------------------------------------------------------------------------
static int __0042_prepare(void* arg);
static int __0042_init(l_ezlopi_item_t* item);
static int __0042_get_cjson_value(l_ezlopi_item_t* item, void* arg);
static int __0042_notify(l_ezlopi_item_t* item);
//------------------------------------------------------------------------------------------------------------------------------
int sensor_0042_ADC_shunt_voltmeter(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0042_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0042_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0042_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0042_notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

//------------------------------------------------------------------------------------------------------
static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_electricity;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type = dev_type_sensor;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_data)
{
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_voltage;
    item->cloud_properties.show = true;
    item->cloud_properties.value_type = value_type_electric_potential;
    item->cloud_properties.scale = scales_volt;

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_gpio_str, item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit

    // passing the custom data_structure
    item->is_user_arg_unique = true;
    item->user_arg = user_data;
}

//----------------------------------------------------
static int __0042_prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* device_prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        s_voltmeter_t* user_data = (s_voltmeter_t*)malloc(__FUNCTION__, sizeof(s_voltmeter_t));
        if (NULL != user_data)
        {
            memset(user_data, 0, sizeof(s_voltmeter_t));

            l_ezlopi_device_t* voltmeter_device = ezlopi_device_add_device(device_prep_arg->cjson_device, NULL);
            if (voltmeter_device)
            {
                ret = 1;
                __prepare_device_cloud_properties(voltmeter_device, device_prep_arg->cjson_device);
                l_ezlopi_item_t* voltmeter_item = ezlopi_device_add_item_to_device(voltmeter_device, sensor_0042_ADC_shunt_voltmeter);
                if (voltmeter_item)
                {
                    __prepare_item_cloud_properties(voltmeter_item, device_prep_arg->cjson_device, user_data);
                }
                else
                {
                    ret = -1;
                    ezlopi_device_free_device(voltmeter_device);
                    free(__FUNCTION__, user_data);
                }
            }
            else
            {
                ret = -1;
                free(__FUNCTION__, user_data);
            }
        }
        else
        {
            ret = -1;
        }
    }
    return ret;
}

static int __0042_init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        s_voltmeter_t* user_data = (s_voltmeter_t*)item->user_arg;
        if (user_data)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
            {
                // initialize analog_pin
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
        else
        {
            ret = -1;
        }
    }
    return ret;
}

static int __0042_get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cj_result = (cJSON*)arg;
        if (cj_result)
        {
            s_voltmeter_t* user_data = (s_voltmeter_t*)item->user_arg;
            if (user_data)
            {
#if VOLTAGE_DIVIDER_EN
                ezlopi_valueformatter_float_to_cjson(item, cj_result, (user_data->volt) * 9.52f);
#else
                ezlopi_valueformatter_float_to_cjson(item, cj_result, (user_data->volt) * 4.2f);
#endif
                ret = 1;
            }
        }
    }
    return ret;
}

static int __0042_notify(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        s_voltmeter_t* user_data = (s_voltmeter_t*)item->user_arg;
        if (user_data)
        {
            s_ezlopi_analog_data_t ezlopi_analog_data = { .value = 0, .voltage = 0 };
            ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
            float Vout = (ezlopi_analog_data.voltage) / 1000.0f; // millivolt -> voltage

            if (fabs(Vout - (user_data->volt)) > 0.5)
            {
                user_data->volt = Vout;
                ezlopi_device_value_updated_from_device_broadcast(item);
            }
            ret = 1;
        }
    }
    return ret;
}