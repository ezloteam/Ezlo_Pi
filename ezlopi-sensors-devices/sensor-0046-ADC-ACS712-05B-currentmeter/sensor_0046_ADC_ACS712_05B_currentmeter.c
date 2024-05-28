#include <math.h>
#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0046_ADC_ACS712_05B_currentmeter.h"
#include "EZLOPI_USER_CONFIG.h"

typedef struct s_currentmeter
{
    float amp_value;
} s_currentmeter_t;

static int __0046_prepare(void* arg);
static int __0046_init(l_ezlopi_item_t* item);
static int __0046_get_cjson_value(l_ezlopi_item_t* item, void* arg);
static int __0046_notify(l_ezlopi_item_t* item);
static void __calculate_current_value(l_ezlopi_item_t* item);

int sensor_0046_ADC_ACS712_05B_currentmeter(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0046_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0046_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0046_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0046_notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

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
    item->cloud_properties.item_name = ezlopi_item_name_current;
    item->cloud_properties.show = true;
    item->cloud_properties.value_type = value_type_electric_current;
    item->cloud_properties.scale = scales_ampere;

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit

    // passing the custom data_structure
    item->is_user_arg_unique = true;
    item->user_arg = user_data;
}

//----------------------------------------------------
static int __0046_prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* device_prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        s_currentmeter_t* user_data = (s_currentmeter_t*)ezlopi_malloc(__FUNCTION__, sizeof(s_currentmeter_t));
        if (user_data)
        {
            ret = 1;
            memset(user_data, 0, sizeof(s_currentmeter_t));
            l_ezlopi_device_t* currentmeter_device = ezlopi_device_add_device(device_prep_arg->cjson_device, NULL);
            if (currentmeter_device)
            {
                __prepare_device_cloud_properties(currentmeter_device, device_prep_arg->cjson_device);
                l_ezlopi_item_t* currentmeter_item = ezlopi_device_add_item_to_device(currentmeter_device, sensor_0046_ADC_ACS712_05B_currentmeter);
                if (currentmeter_item)
                {
                    __prepare_item_cloud_properties(currentmeter_item, device_prep_arg->cjson_device, user_data);
                }
                else
                {
                    ret = -1;
                    ezlopi_device_free_device(currentmeter_device);
                    ezlopi_free(__FUNCTION__, user_data);
                }
            }
            else
            {
                ret = -1;
                ezlopi_free(__FUNCTION__, user_data);
            }
        }
    }
    return ret;
}

static int __0046_init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        s_currentmeter_t* user_data = (s_currentmeter_t*)item->user_arg;
        if (user_data)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
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
        else
        {
            ret = -1;
        }
    }
    return ret;
}

static int __0046_get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cj_result = (cJSON*)arg;
        s_currentmeter_t* user_data = (s_currentmeter_t*)item->user_arg;
        if (user_data)
        {
            ezlopi_valueformatter_float_to_cjson(item, cj_result, user_data->amp_value);
            ret = 1;
        }
    }

    return ret;
}

static int __0046_notify(l_ezlopi_item_t* item)
{
    // During this calculation the system is polled for 20mS
    int ret = 0;
    if (item)
    {
        s_currentmeter_t* user_data = (s_currentmeter_t*)item->user_arg;
        if (user_data)
        {
            float prev_amp = user_data->amp_value;
            __calculate_current_value(item); // update amp
            if (fabs(user_data->amp_value - prev_amp) > 0.5)
            {
                ezlopi_device_value_updated_from_device_broadcast(item);
            }
        }
    }
    return ret;
}
//--------------------------------------------------------------------------------------------------------------------------------------
static void __calculate_current_value(l_ezlopi_item_t* item)
{
    if (NULL != item)
    {
        s_currentmeter_t* user_data = (s_currentmeter_t*)item->user_arg;
        if (user_data)
        {
            s_ezlopi_analog_data_t ezlopi_analog_data = { .value = 0, .voltage = 0 };

            uint32_t period_dur = (1000000 / DEFAULT_AC_FREQUENCY); // 20000uS
            int Vnow = 0;
            uint32_t Vsum = 0;
            uint32_t measurements_count = 0;

            // starting 't' instant
            uint32_t t_start = (uint32_t)esp_timer_get_time();
            uint32_t Volt = 0;
            int diff = 0;

            while (((uint32_t)esp_timer_get_time() - t_start) < period_dur) // loops within 1-complete cycle
            {
                ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
                Volt = 2 * (ezlopi_analog_data.voltage); // since the input is half the original value after voltage division
                diff = ((ASC712TELC_05B_zero_point_mV - Volt) > 0 ? (ASC712TELC_05B_zero_point_mV - Volt) : (Volt - ASC712TELC_05B_zero_point_mV));
                // getting the voltage value at this instant
                if (diff > 150 && diff < 1500) // the reading voltage less than 00mV is noise
                {
                    Vnow = Volt - ASC712TELC_05B_zero_point_mV; // ()at zero offset => full-scale/2
                }
                else
                {
                    Vnow = 0;
                }
                Vsum += (Vnow * Vnow); // sumof(I^2 + I^2 + .....)
                measurements_count++;
            }

            // If applied for DC;  'AC_Irms' calculation give same value as 'DC-value'
            if (0 == measurements_count)
            {
                measurements_count = 1; // <-- avoid dividing by zero
            }

            user_data->amp_value = ((float)sqrt(Vsum / measurements_count)) / 185.0f; //  -> I[rms] Ampere
            if ((user_data->amp_value) < 0.4)
            {
                user_data->amp_value = 0;
            }
        }
    }
}