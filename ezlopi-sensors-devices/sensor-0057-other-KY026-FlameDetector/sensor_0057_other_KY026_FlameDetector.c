#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0057_other_KY026_FlameDetector.h"

//------------------------------------------------------------------------------
const char *ky206_sensor_heat_alarm_token[] = {
    "heat_ok",
    "overheat_detected",
    "under_heat_detected",
    "unknown",
};
//------------------------------------------------------------------------------
static int __0057_prepare(void *arg);
static int __0057_init(l_ezlopi_item_t *item);
static int __0057_get_item(l_ezlopi_item_t *item, void *arg);
static int __0057_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __0057_notify(l_ezlopi_item_t *item);

static void __prepare_device_digi_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_digi_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __prepare_device_adc_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_adc_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data);
static void __extract_KY026_sensor_value(uint32_t flame_adc_pin, float *analog_sensor_volt, float *max_reading);
//----------------------------------------------------------------------------------------------------------------

int sensor_0057_other_KY026_FlameDetector(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0057_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0057_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        __0057_get_item(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0057_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __0057_notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

//----------------------------------------------------
static int __0057_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        //---------------------------  DIGI - DEVICE 1 --------------------------------------------
        l_ezlopi_device_t *flame_device_digi = ezlopi_device_add_device(device_prep_arg->cjson_device);
        if (flame_device_digi)
        {
            __prepare_device_digi_cloud_properties(flame_device_digi, device_prep_arg->cjson_device);
            l_ezlopi_item_t *flame_item_digi = ezlopi_device_add_item_to_device(flame_device_digi, sensor_0057_other_KY026_FlameDetector);
            if (flame_item_digi)
            {
                flame_item_digi->cloud_properties.device_id = flame_device_digi->cloud_properties.device_id;
                __prepare_item_digi_cloud_properties(flame_item_digi, device_prep_arg->cjson_device);
                ret = 1;
            }
            else
            {
                ret = -1;
                ezlopi_device_free_device(flame_device_digi);
            }
        }

        //---------------------------- ADC - DEVICE 2 -------------------------------------------
        flame_t *flame_struct = (flame_t *)malloc(sizeof(flame_t));
        if (NULL != flame_struct)
        {
            memset(flame_struct, 0, sizeof(flame_t));
            l_ezlopi_device_t *flame_device_adc = ezlopi_device_add_device(device_prep_arg->cjson_device);
            if (flame_device_adc)
            {
                __prepare_device_adc_cloud_properties(flame_device_adc, device_prep_arg->cjson_device);
                l_ezlopi_item_t *flame_item_adc = ezlopi_device_add_item_to_device(flame_device_adc, sensor_0057_other_KY026_FlameDetector);
                if (flame_item_adc)
                {
                    flame_item_adc->cloud_properties.device_id = flame_device_adc->cloud_properties.device_id;
                    __prepare_item_adc_cloud_properties(flame_item_adc, device_prep_arg->cjson_device, flame_struct);
                    ret = 1;
                }
                else
                {
                    ret = -1;
                    ezlopi_device_free_device(flame_device_adc);
                    free(flame_struct);
                }
            }
            else
            {
                ret = -1;
                free(flame_struct);
            }
        }
    }
    return ret;
}

static int __0057_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (NULL != item)
    {
        if (ezlopi_item_name_heat_alarm == item->cloud_properties.item_name)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
            { // intialize digital_pin
                gpio_config_t input_conf = {};
                input_conf.pin_bit_mask = (1ULL << (item->interface.gpio.gpio_in.gpio_num));
                input_conf.intr_type = GPIO_INTR_DISABLE;
                input_conf.mode = GPIO_MODE_INPUT;
                input_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
                input_conf.pull_up_en = GPIO_PULLUP_ENABLE;
                ret = (0 == gpio_config(&input_conf)) ? 1 : -1;
            }
            else
            {
                ret = -1;
                // ezlopi_device_free_device_by_item(item);
            }
        }
        else if (ezlopi_item_name_temperature_changes == item->cloud_properties.item_name)
        {
            flame_t *flame_struct = (flame_t *)item->user_arg;
            if (flame_struct)
            {
                if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
                { // initialize analog_pin
                    if (0 == ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
                    {
                        ret = 1;
                    }
                    else
                    {
                        ret = -1;
                        free(item->user_arg); // this will free ; memory address linked to all items
                        item->user_arg = NULL;
                        // ezlopi_device_free_device_by_item(item);
                    }
                }
                else
                {
                    ret = -1;
                    free(item->user_arg); // this will free ; memory address linked to all items
                    item->user_arg = NULL;
                    // ezlopi_device_free_device_by_item(item);
                }
            }
            // else
            // {
            //     ret = -1;
            //     // ezlopi_device_free_device_by_item(item);
            // }
        }
    }
    return ret;
}

//------------------------------------------------------------------------------------------------------
static void __prepare_device_digi_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    // char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
    // ASSIGN_DEVICE_NAME_V2(device, device_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_heat;
    device->cloud_properties.device_type = dev_type_sensor;
}
static void __prepare_item_digi_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_heat_alarm;
    item->cloud_properties.value_type = value_type_token;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.gpio.gpio_in.gpio_num);
    TRACE_S("flame_> DIGITAL_PIN: %d ", item->interface.gpio.gpio_in.gpio_num);
}
//------------------------------------------------------------------------------------------------------
static void __prepare_device_adc_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    // char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
    // ASSIGN_DEVICE_NAME_V2(device, device_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}
static void __prepare_item_adc_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_temperature_changes;
    item->cloud_properties.value_type = value_type_general_purpose;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_percent;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio2_str, item->interface.adc.gpio_num);
    TRACE_S("flame_> ADC_PIN: %d ", item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit

    // passing the custom data_structure
    item->user_arg = user_data;
}

//------------------------------------------------------------------------------------------------------
static int __0057_get_item(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            if (ezlopi_item_name_heat_alarm == item->cloud_properties.item_name)
            {
                //-------------------  POSSIBLE JSON ENUM CONTENTS ----------------------------------
                cJSON *json_array_enum = cJSON_CreateArray();
                if (NULL != json_array_enum)
                {
                    for (uint8_t i = 0; i < KY206_HEAT_ALARM_MAX; i++)
                    {
                        cJSON *json_value = cJSON_CreateString(ky206_sensor_heat_alarm_token[i]);
                        if (NULL != json_value)
                        {
                            cJSON_AddItemToArray(json_array_enum, json_value);
                        }
                    }
                    cJSON_AddItemToObject(cj_result, ezlopi_enum_str, json_array_enum);
                }
                //--------------------------------------------------------------------------------------
                cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, (char *)item->user_arg ? item->user_arg : "heat_ok");
                cJSON_AddStringToObject(cj_result, ezlopi_value_str, (char *)item->user_arg ? item->user_arg : "heat_ok");
            }
            else if (ezlopi_item_name_temperature_changes == item->cloud_properties.item_name)
            {
                flame_t *flame_struct = (flame_t *)item->user_arg;
                if (flame_struct)
                {
                    cJSON_AddNumberToObject(cj_result, ezlopi_value_str, flame_struct->absorbed_percent);
                    char *valueFormatted = ezlopi_valueformatter_float(flame_struct->absorbed_percent);
                    if (valueFormatted)
                    {
                        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                        free(valueFormatted);
                    }
                }
                ret = 1;
            }
        }
    }
    return ret;
}

static int __0057_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            if (ezlopi_item_name_heat_alarm == item->cloud_properties.item_name)
            {
                cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, (char *)item->user_arg ? item->user_arg : "heat_ok");
                cJSON_AddStringToObject(cj_result, ezlopi_value_str, (char *)item->user_arg ? item->user_arg : "heat_ok");
            }
            else if (ezlopi_item_name_temperature_changes == item->cloud_properties.item_name)
            {
                flame_t *flame_struct = (flame_t *)item->user_arg;
                if (flame_struct)
                {
                    cJSON_AddNumberToObject(cj_result, ezlopi_value_str, flame_struct->absorbed_percent);
                    char *valueFormatted = ezlopi_valueformatter_float(flame_struct->absorbed_percent);
                    if (valueFormatted)
                    {
                        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                        free(valueFormatted);
                    }
                }
            }
            ret = 1;
        }
    }
    return ret;
}

static int __0057_notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        if (ezlopi_item_name_heat_alarm == item->cloud_properties.item_name)
        {
            const char *curret_value = NULL;
            if (0 == gpio_get_level(item->interface.gpio.gpio_in.gpio_num)) // when D0 -> 0V,
            {
                curret_value = "heat_ok";
            }
            else
            {
                curret_value = "overheat_detected";
            }
            if (curret_value != (char *)item->user_arg) // calls update only if there is change in state
            {
                item->user_arg = (void *)curret_value;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        else if (ezlopi_item_name_temperature_changes == item->cloud_properties.item_name)
        {
            flame_t *flame_struct = (flame_t *)item->user_arg;
            if (flame_struct)
            {
                float analog_sensor_volt = 0, max_volt_reading = 0;
                // extract the sensor_output_values
                __extract_KY026_sensor_value(item->interface.adc.gpio_num, &analog_sensor_volt, &max_volt_reading);
                float new_percent = ((1 - (analog_sensor_volt / max_volt_reading)) * 100.0f);
                // TRACE_E("Heat-detected: %.2f percent", absorbed_percent);
                if (new_percent != flame_struct->absorbed_percent)
                {
                    ezlopi_device_value_updated_from_device_v3(item);
                    flame_struct->absorbed_percent = new_percent;
                }
            }
        }
        ret = 1;
    }
    return ret;
}
//--------------------------------------------------------------------------------------------------------------------------------------
static void __extract_KY026_sensor_value(uint32_t flame_adc_pin, float *analog_sensor_volt, float *max_reading)
{
    static float max = 0;
    // calculation process
    s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};
    //-------------------------------------------------
    // extract the mean_sensor_analog_output_voltage
    for (uint8_t x = 10; x > 0; x--)
    {
        ezlopi_adc_get_adc_data(flame_adc_pin, &ezlopi_analog_data);
#ifdef VOLTAGE_DIVIDER_ADDED
        *analog_sensor_volt += ((float)((ezlopi_analog_data.voltage) / 1000.0f) * 2.0f); // V
#else
        *analog_sensor_volt += (float)(ezlopi_analog_data.voltage);
#endif
    }
    // find mean
    *analog_sensor_volt = *analog_sensor_volt / 10.0f;

    if (max < *analog_sensor_volt)
    {
        max = *analog_sensor_volt;
    }
    // Set max_value of the readings done
    *max_reading = max;
}

//--------------------------------------------------------------------------------------------------------------------------------------