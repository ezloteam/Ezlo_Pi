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

#include "sensor_0070_ADC_dummy_potentiometer.h"
typedef struct s_dummy_potentiometer
{
    float pot_val; // 0-100%
} s_dummy_potentiometer_t;

static ezlopi_error_t __0070_prepare(void *arg);
static ezlopi_error_t __0070_init(l_ezlopi_item_t *item);
static ezlopi_error_t __0070_set_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0070_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0070_notify(l_ezlopi_item_t *item);
//--------------------------------------------------------------------------------------------------------
ezlopi_error_t sensor_0070_ADC_dummy_potentiometer(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
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
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type = dev_type_sensor;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data)
{
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
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
static ezlopi_error_t __0070_prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        cJSON *cj_device = device_prep_arg->cjson_device;
        s_dummy_potentiometer_t *user_data = (s_dummy_potentiometer_t *)malloc(sizeof(s_dummy_potentiometer_t));
        if (NULL != user_data)
        {
            memset(user_data, 0, sizeof(s_dummy_potentiometer_t));
            l_ezlopi_device_t *dummy_potentiometer_device = EZPI_core_device_add_device(cj_device, NULL);
            if (dummy_potentiometer_device)
            {
                __prepare_device_cloud_properties(dummy_potentiometer_device, cj_device);
                l_ezlopi_item_t *dummy_potentiometer_item = EZPI_core_device_add_item_to_device(dummy_potentiometer_device, sensor_0070_ADC_dummy_potentiometer);
                if (dummy_potentiometer_item)
                {
                    __prepare_item_cloud_properties(dummy_potentiometer_item, cj_device, user_data);
                    ret = EZPI_SUCCESS;
                }
                else
                {
                    EZPI_core_device_free_device(dummy_potentiometer_device);
                    free(user_data);
                }
            }
            else
            {
                free(user_data);
            }
        }
    }
    return ret;
}

static ezlopi_error_t __0070_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {
        s_dummy_potentiometer_t *user_data = (s_dummy_potentiometer_t *)item->user_arg;
        if (user_data)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
            {
                if (EZPI_SUCCESS == ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
                {
                    ret = EZPI_SUCCESS;
                }
            }
        }
    }
    return ret;
}


static ezlopi_error_t __0070_set_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cjson_params = (cJSON *)arg;
        if (cjson_params)
        {
            s_dummy_potentiometer_t *user_data = (s_dummy_potentiometer_t *)item->user_arg;
            if (user_data)
            {
                CJSON_TRACE("cjson_params  [dummy_potentiometer]:", cjson_params);

                float value_double = 0;
                bool update_flag = false;
                cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cjson_params, ezlopi_value_str);
                if (cj_value)
                {
                    switch (cj_value->type)
                    {
                    case cJSON_Number:
                    {
                        value_double = (float)cj_value->valuedouble;

                        if (user_data->pot_val != value_double)
                        {
                            update_flag = true;
                        }

                        break;
                    }
                    default:
                        break;
                    }

                    if (update_flag)
                    {
                        TRACE_S("item_name: %s", item->cloud_properties.item_name);
                        TRACE_S("item_id: 0x%08x", item->cloud_properties.item_id);
                        TRACE_S("prev_pot_state: [%.2f]", user_data->pot_val);
                        TRACE_S("curr_pot_state: [%.2f]", value_double);

                        user_data->pot_val = value_double;
                        EZPI_core_device_value_updated_from_device_broadcast(item);
                        ret = EZPI_SUCCESS;
                    }
                }
            }
        }
    }
    return ret;
}

static ezlopi_error_t __0070_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            s_dummy_potentiometer_t *user_data = (s_dummy_potentiometer_t *)item->user_arg;
            if (user_data)
            {
                if (user_data->pot_val > 70)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, "high_volume");
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, "high_volume");
                }
                else if (user_data->pot_val > 30)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, "mid_volume");
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, "mid_volume");
                }
                else
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, "low_volume");
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, "low_volume");
                }
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static ezlopi_error_t __0070_notify(l_ezlopi_item_t *item)
{
    static uint8_t timing = 10;// 5sec
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && (0 == timing--))
    {
        timing = 10;
        s_dummy_potentiometer_t *user_data = (s_dummy_potentiometer_t *)item->user_arg;
        if (user_data)
        {
            s_ezlopi_analog_data_t adc_data = { .value = 0, .voltage = 0 };
            ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &adc_data);
            float new_pot = (((float)(4095.0f - (adc_data.value)) / 4095.0f) * 100);

            if (fabs((user_data->pot_val) - new_pot) > 0.05)
            {
                user_data->pot_val = new_pot;
                EZPI_core_device_value_updated_from_device_broadcast(item);
            }
            ret = EZPI_FAILED;
        }
    }
    return ret;
}

//-------------------------------------------------------------------------------------------------------------------------