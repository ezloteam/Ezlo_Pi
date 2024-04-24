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

#include "sensor_0070_ADC_dummy_potentiometer.h"
typedef struct s_dummy_potentiometer
{
    float pot_val; // 0-100%
    char pot_status_str[30];
} s_dummy_potentiometer_t;

static int __0070_prepare(void* arg);
static int __0070_init(l_ezlopi_item_t* item);
static int __0070_set_value(l_ezlopi_item_t* item, void* arg);
static int __0070_get_cjson_value(l_ezlopi_item_t* item, void* arg);
static int __0070_notify(l_ezlopi_item_t* item);
//--------------------------------------------------------------------------------------------------------
int sensor_0070_ADC_dummy_potentiometer(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0070_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0070_init(item);
        break;
    }
    case EZLOPI_ACTION_SET_VALUE:
    {
        ret = __0070_set_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0070_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0070_notify(item);
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
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type = dev_type_sensor;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_data)
{
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_sound_volume;
    item->cloud_properties.value_type = value_type_string;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;

    item->is_user_arg_unique = true;
    item->user_arg = user_data;

    item->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT; // other
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_gpio_str, item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3;
}
//-------------------------------------------------------------------------------------------------------------------------
static int __0070_prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* device_prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        cJSON* cj_device = device_prep_arg->cjson_device;
        s_dummy_potentiometer_t* user_data = (s_dummy_potentiometer_t*)malloc(sizeof(s_dummy_potentiometer_t));
        if (NULL != user_data)
        {
            memset(user_data, 0, sizeof(s_dummy_potentiometer_t));
            user_data->pot_val = 1;
            snprintf(user_data->pot_status_str, sizeof(user_data->pot_status_str) - 1, "%s", "setup");
            l_ezlopi_device_t* dummy_potentiometer_device = ezlopi_device_add_device(cj_device, NULL);
            if (dummy_potentiometer_device)
            {
                ret = 1;
                __prepare_device_cloud_properties(dummy_potentiometer_device, cj_device);
                l_ezlopi_item_t* dummy_potentiometer_item = ezlopi_device_add_item_to_device(dummy_potentiometer_device, sensor_0070_ADC_dummy_potentiometer);
                if (dummy_potentiometer_item)
                {
                    __prepare_item_cloud_properties(dummy_potentiometer_item, cj_device, user_data);
                }
                else
                {
                    ezlopi_device_free_device(dummy_potentiometer_device);
                    free(user_data);
                    ret = -1;
                }
            }
            else
            {
                free(user_data);
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

static int __0070_init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        s_dummy_potentiometer_t* user_data = (s_dummy_potentiometer_t*)item->user_arg;
        if (user_data)
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
        else
        {
            ret = -1;
        }
    }
    return ret;
}


static int __0070_set_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cjson_params = (cJSON*)arg;
        if (cjson_params)
        {
            s_dummy_potentiometer_t* user_data = (s_dummy_potentiometer_t*)item->user_arg;
            if (user_data)
            {
                CJSON_TRACE("cjson_params  [dummy_potentiometer]:", cjson_params);

                float value_double = 0;
                // char* value_str = NULL;
                bool update_flag = false;
                cJSON* cj_value = cJSON_GetObjectItem(cjson_params, ezlopi_value_str);
                if (cj_value)
                {
                    switch (cj_value->type)
                    {
                    case cJSON_Number:
                    {
                        value_double = cj_value->valuedouble;

                        if (user_data->pot_val != value_double)
                        {
                            update_flag = true;

                        }

                        break;
                    }
                    // case cJSON_String:
                    // {
                    //     value_str = cj_value->valuestring;
                    //     if (value_str)
                    //     {
                    //         size_t len_value_str = strlen(value_str);
                    //         size_t len_pot_status_str = (NULL != user_data->pot_status_str) ? strlen(user_data->pot_status_str) : 0;
                    //         size_t max_len = (len_value_str > len_pot_status_str) ? len_value_str : len_pot_status_str;
                    //         if (0 != strncmp(user_data->pot_status_str, value_str, max_len + 1))
                    //         {
                    //             update_flag = true;
                    //         }
                    //     }
                    //     break;
                    // }
                    default:
                        break;
                    }

                    if (update_flag)
                    {
                        TRACE_S("item_name: %s", item->cloud_properties.item_name);
                        TRACE_S("item_id: 0x%08x", item->cloud_properties.item_id);
                        TRACE_S("prev_pot_state: '%s' [%.2f]", user_data->pot_status_str, user_data->pot_val);

                        // if (value_str)
                        // {
                        //     snprintf(user_data->pot_status_str, sizeof(user_data->pot_status_str)-1, "%s", value_str);
                        // }

                        if (fabs((user_data->pot_val) - value_double) > 0.05)
                        {
                            user_data->pot_val = value_double;
                            if (user_data->pot_val > 70)
                            {
                                snprintf(user_data->pot_status_str, sizeof(user_data->pot_status_str) - 1, "%s", "high_volume");
                            }
                            else if (user_data->pot_val > 30)
                            {
                                snprintf(user_data->pot_status_str, sizeof(user_data->pot_status_str) - 1, "%s", "mid_volume");
                            }
                            else
                            {
                                snprintf(user_data->pot_status_str, sizeof(user_data->pot_status_str) - 1, "%s", "low_volume");
                            }
                            TRACE_S("curr_pot_state: '%s' [%.2f]", user_data->pot_status_str, user_data->pot_val);
                        }

                        ezlopi_device_value_updated_from_device_broadcast(item);
                        ret = 1;
                    }
                }
            }
            else
            {
                TRACE_E("Here !!");
            }
        }
    }
    return ret;
}

static int __0070_get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cj_result = (cJSON*)arg;
        if (cj_result)
        {
            s_dummy_potentiometer_t* user_data = (s_dummy_potentiometer_t*)item->user_arg;
            if (user_data)
            {
                cJSON_AddStringToObject(cj_result, ezlopi_value_str, (NULL != user_data->pot_status_str) ? user_data->pot_status_str : "low_volume");
                cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, (NULL != user_data->pot_status_str) ? user_data->pot_status_str : "low_volume");
                ret = 1;
            }
            else
            {
                TRACE_E("Here !!");
            }
        }
    }
    return ret;
}

static int __0070_notify(l_ezlopi_item_t* item)
{
    static uint8_t timing = 10;// 5sec
    int ret = 0;
    if (item && (0 == timing--))
    {
        timing = 10;
        s_dummy_potentiometer_t* user_data = (s_dummy_potentiometer_t*)item->user_arg;
        if (user_data)
        {
            s_ezlopi_analog_data_t adc_data = { .value = 0, .voltage = 0 };
            ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &adc_data);
            float new_pot = (((float)(4095.0f - (adc_data.value)) / 4095.0f) * 100);

            if (fabs((user_data->pot_val) - new_pot) > 0.05)
            {
                user_data->pot_val = new_pot;
                if (user_data->pot_val > 70)
                {
                    snprintf(user_data->pot_status_str, sizeof(user_data->pot_status_str) - 1, "%s", "high_volume");
                }
                else if (user_data->pot_val > 30)
                {
                    snprintf(user_data->pot_status_str, sizeof(user_data->pot_status_str) - 1, "%s", "mid_volume");
                }
                else
                {
                    snprintf(user_data->pot_status_str, sizeof(user_data->pot_status_str) - 1, "%s", "low_volume");
                }
                ezlopi_device_value_updated_from_device_broadcast(item);
            }
            ret = 1;
        }
        else
        {
            TRACE_E("Here!!");
        }
    }
    return ret;
}

//-------------------------------------------------------------------------------------------------------------------------