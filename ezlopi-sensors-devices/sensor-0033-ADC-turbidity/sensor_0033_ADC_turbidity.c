#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0033_ADC_turbidity.h"

static int __prepare(void* arg);
static int __init(l_ezlopi_item_t* item);
static int __notify(l_ezlopi_item_t* item);
static int __get_cjson_value(l_ezlopi_item_t* item, void* arg);
static int __get_item_list(l_ezlopi_item_t* item, void* arg);

static const char* water_filter_replacement_alarm_states[] = {
    "water_filter_ok",
    "replace_water_filter",
    "unknown",
};

int sensor_0033_ADC_turbidity(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
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
        ret = __get_item_list(item, arg);
        break;
    }
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

static int __get_item_list(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    cJSON* cjson_propertise = (cJSON*)arg;
    if (cjson_propertise)
    {
        cJSON* json_array_enum = cJSON_CreateArray();
        if (NULL != json_array_enum)
        {
            for (uint8_t i = 0; i < TURBIDITY__MAX; i++)
            {
                cJSON* json_value = cJSON_CreateString(water_filter_replacement_alarm_states[i]);
                if (NULL != json_value)
                {
                    cJSON_AddItemToArray(json_array_enum, json_value);
                }
            }
            cJSON_AddItemToObject(cjson_propertise, ezlopi_enum_str, json_array_enum);
        }

        s_ezlopi_analog_data_t ezlopi_analog_data = { .value = 0, .voltage = 0 };

        ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
        TRACE_I("Value is: %d, voltage is: %d", ezlopi_analog_data.value, ezlopi_analog_data.voltage);
        const char* ezlopi_water_present_turbidity_state = NULL;
        if (1000 > ezlopi_analog_data.voltage)
        {
            ezlopi_water_present_turbidity_state = "replace_water_filter";
        }
        else
        {
            ezlopi_water_present_turbidity_state = "water_filter_ok";
        }

        if (ezlopi_water_present_turbidity_state)
        {
            cJSON_AddStringToObject(cjson_propertise, ezlopi_value_str, ezlopi_water_present_turbidity_state);
            cJSON_AddStringToObject(cjson_propertise, ezlopi_valueFormatted_str, ezlopi_water_present_turbidity_state);
        }
        else
        {
            cJSON_AddStringToObject(cjson_propertise, ezlopi_value_str, water_filter_replacement_alarm_states[TURBIDITY__UNKNOWN]);
            cJSON_AddStringToObject(cjson_propertise, ezlopi_valueFormatted_str, water_filter_replacement_alarm_states[TURBIDITY__UNKNOWN]);
        }
        ret = 1;
    }
    return ret;
}

static int __notify(l_ezlopi_item_t* item)
{
    int ret = 0;
    const char* tmp_sensor_state = NULL;
    char* turbidity_sensor_state = (char*)item->user_arg;

    if (turbidity_sensor_state)
    {
        s_ezlopi_analog_data_t tmp_analog_data = { .value = 0, .voltage = 0 };
        ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &tmp_analog_data);
        if (1000 > tmp_analog_data.voltage)
        {
            tmp_sensor_state = "replace_water_filter";
        }
        else
        {
            tmp_sensor_state = "water_filter_ok";
        }
        if (strcmp(turbidity_sensor_state, tmp_sensor_state) != 0)
        {
            memcpy(turbidity_sensor_state, tmp_sensor_state, (40 * sizeof(char)));
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
        char* turbidity_sensor_state = (char*)item->user_arg;
        if (turbidity_sensor_state)
        {
            cJSON_AddStringToObject(cj_result, ezlopi_value_str, turbidity_sensor_state);
            cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, turbidity_sensor_state);
        }
        ret = 1;
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
    // char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
    // ASSIGN_DEVICE_NAME_V2(device, device_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_water;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_water_filter_replacement_alarm;
    item->cloud_properties.value_type = value_type_token;
    item->cloud_properties.scale = NULL;
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
            l_ezlopi_item_t* item_turbidity = ezlopi_device_add_item_to_device(device, sensor_0033_ADC_turbidity);
            if (item_turbidity)
            {
                item_turbidity->cloud_properties.device_id = device->cloud_properties.device_id;
                char* turbidity_sensor_states = (char*)malloc(40 * sizeof(char));
                if (turbidity_sensor_states)
                {

                    memset(turbidity_sensor_states, 0, sizeof(s_ezlopi_analog_data_t));
                    __prepare_item_properties(item_turbidity, prep_arg->cjson_device, (void*)turbidity_sensor_states);
                    ret = 1;
                }
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
