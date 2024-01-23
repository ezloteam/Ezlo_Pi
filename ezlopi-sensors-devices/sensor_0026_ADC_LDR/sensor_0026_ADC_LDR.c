#include "trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0026_ADC_LDR.h"

const char *light_alarm_states[] =
    {
        "no_light",
        "light_detected",
        "unknown",
};

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_value_cjson(l_ezlopi_item_t *item, void *arg);
static void __setup_device_cloud_params(l_ezlopi_device_t *device, cJSON *cj_device);
static void __setup_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static int _get_item_list(l_ezlopi_item_t *item, void *arg);

int sensor_0026_ADC_LDR(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    {
        _get_item_list(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __get_value_cjson(item, arg);
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

static void __setup_device_cloud_params(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_light_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_device;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __setup_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    if (item)
    {
        item->cloud_properties.show = true;
        item->cloud_properties.has_getter = true;
        item->cloud_properties.has_setter = false;
        item->cloud_properties.item_name = ezlopi_item_name_light_alarm;
        item->cloud_properties.value_type = value_type_token;
        item->cloud_properties.scale = NULL;
        item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

        item->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;
        item->interface.adc.resln_bit = 3;
        CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.adc.gpio_num);
    }
}

static int __prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        cJSON *cj_device = prep_arg->cjson_device;
        if (cj_device)
        {
            l_ezlopi_device_t *device = ezlopi_device_add_device(cj_device);
            if (device)
            {
                __setup_device_cloud_params(device, cj_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, sensor_0026_ADC_LDR);
                if (item)
                {
                    __setup_item_cloud_properties(item, cj_device);
                }
                else
                {
                    ezlopi_device_free_device(device);
                }
            }
        }
    }
    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
        {
            ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit);
            ret = 1;
        }
    }
    return ret;
}

static int _get_item_list(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    cJSON *cj_result = (cJSON *)arg;
    if (cj_result && item)
    {
        cJSON *json_array_enum = cJSON_CreateArray();
        if (NULL != json_array_enum)
        {
            for (uint8_t i = 0; i < LIGHT_ALARM_MAX; i++)
            {
                cJSON *json_value = cJSON_CreateString(light_alarm_states[i]);
                if (NULL != json_value)
                {
                    cJSON_AddItemToArray(json_array_enum, json_value);
                }
            }
            cJSON_AddItemToObject(cj_result, ezlopi_enum_str, json_array_enum);
        }
        cJSON_AddStringToObject(cj_result, ezlopi_value_str, ((char *)item->user_arg) ? item->user_arg : light_alarm_states[0]);
        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, ((char *)item->user_arg) ? item->user_arg : light_alarm_states[0]);
        ret = 1;
    }

    return ret;
}

static int __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    cJSON *cj_result = (cJSON *)arg;
    if (cj_result && item)
    {
        cJSON_AddStringToObject(cj_result, ezlopi_value_str, ((char *)item->user_arg) ? item->user_arg : light_alarm_states[0]);
        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, ((char *)item->user_arg) ? item->user_arg : light_alarm_states[0]);
        ret = 1;
    }
    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};
        ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
        TRACE_B("Value is: %d, voltage is: %d", ezlopi_analog_data.value, ezlopi_analog_data.voltage);
        char *curr_ldr_state = NULL;
        if (((float)(4096 - ezlopi_analog_data.value) / 4096.0f) > 0.5f) // pot_val : [100% - 30%]
        {
            curr_ldr_state = light_alarm_states[0]; // no light
        }
        else
        {
            curr_ldr_state = light_alarm_states[1]; // light detected
        }
        if (curr_ldr_state != (char *)item->user_arg)
        {
            item->user_arg = (void *)curr_ldr_state;
            ezlopi_device_value_updated_from_device_v3(item);
        }
    }
    return ret;
}