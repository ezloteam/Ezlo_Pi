#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0055_ADC_FlexResistor.h"

//--------------------------------------------------------------------------------------------------------
static int __0055_prepare(void *arg);
static int __0055_init(l_ezlopi_item_t *item);
static int __0055_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __0055_notify(l_ezlopi_item_t *item);
static void __prepare_device_adc_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_adc_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data);
//--------------------------------------------------------------------------------------------------------

int sensor_0055_ADC_FlexResistor(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0055_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0055_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0055_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0055_notify(item);
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
static void __prepare_device_adc_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
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
    item->cloud_properties.item_name = ezlopi_item_name_electrical_resistivity;
    item->cloud_properties.value_type = value_type_electrical_resistance;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_ohm_meter;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_gpio_str, item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit

    // passing the custom data_structure
    item->is_user_arg_unique = true;
    item->user_arg = user_data;
}

//------------------------------------------------------------------------------------------------------

static int __0055_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        flex_t *flex_res_value = (flex_t *)malloc(sizeof(flex_t));
        if (flex_res_value)
        {
            memset(flex_res_value, 0, sizeof(flex_t));
            l_ezlopi_device_t *device_adc = ezlopi_device_add_device(device_prep_arg->cjson_device, NULL);
            if (device_adc)
            {
                ret = 1;
                __prepare_device_adc_cloud_properties(device_adc, device_prep_arg->cjson_device);
                l_ezlopi_item_t *item_adc = ezlopi_device_add_item_to_device(device_adc, sensor_0055_ADC_FlexResistor);
                if (item_adc)
                {
                    __prepare_item_adc_cloud_properties(item_adc, device_prep_arg->cjson_device, flex_res_value);
                }
                else
                {
                    ret = -1;
                    ezlopi_device_free_device(device_adc);
                    free(flex_res_value);
                }
            }
            else
            {
                ret = -1;
                free(flex_res_value);
            }
        }
    }
    return ret;
}

static int __0055_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (NULL != item)
    {
        flex_t *flex_res_value = (flex_t *)malloc(sizeof(flex_t));
        if (flex_res_value)
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
static int __0055_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            flex_t *flex_res_value = (flex_t *)item->user_arg;
            if (flex_res_value)
            {
                char *valueFormatted = ezlopi_valueformatter_int(flex_res_value->rs_0055);
                if (valueFormatted)
                {
                    cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                    cJSON_AddNumberToObject(cj_result, ezlopi_value_str, flex_res_value->rs_0055);
                    free(valueFormatted);
                }
                ret = 1;
            }
        }
    }
    return ret;
}
static int __0055_notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        flex_t *flex_res_value = (flex_t *)item->user_arg;
        if (flex_res_value)
        {
            s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0,
                                                         .voltage = 0};
            // extract the sensor_output_values
            ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
            float Vout = (ezlopi_analog_data.voltage) / 1000.0f; // millivolt -> voltage

            // calculate the 'rs_0055' resistance value using [voltage divider rule]
            int new_rs_0055 = (int)(((flex_Vin / Vout) - 1) * flex_Rout);
            if (new_rs_0055 != flex_res_value->rs_0055)
            {
                ezlopi_device_value_updated_from_device_v3(item);
                flex_res_value->rs_0055 = new_rs_0055;
            }
            ret = 1;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------------------------------
