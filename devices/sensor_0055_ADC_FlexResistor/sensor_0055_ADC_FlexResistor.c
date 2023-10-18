#include "trace.h"
#include "cJSON.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"
#include "stdbool.h"

#include "ezlopi_adc.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_scales_str.h"

#include "sensor_0055_ADC_FlexResistor.h"

//--------------------------------------------------------------------------------------------------------
static int RS_0055 = 0;
static int __0055_prepare(void *arg);
static int __0055_init(l_ezlopi_item_t *item);
static int __0055_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __0055_notify(l_ezlopi_item_t *item);
static void __prepare_device_adc_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_adc_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
//--------------------------------------------------------------------------------------------------------

int sensor_0055_ADC_FlexResistor(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0055_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0055_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0055_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        static uint8_t count;
        if (count++ > 1)
        {
            __0055_notify(item);
            count = 0;
        }
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
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}
static void __prepare_item_adc_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{

    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_electrical_resistivity;
    item->cloud_properties.value_type = value_type_electrical_resistance;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_ohm_meter;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type); // _max = 10
    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit
}

//------------------------------------------------------------------------------------------------------

static int __0055_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        l_ezlopi_device_t *device_adc = ezlopi_device_add_device();
        if (device_adc)
        {
            __prepare_device_adc_cloud_properties(device_adc, device_prep_arg->cjson_device);
            l_ezlopi_item_t *item_adc = ezlopi_device_add_item_to_device(device_adc, sensor_0055_ADC_FlexResistor);
            if (item_adc)
            {
                __prepare_item_adc_cloud_properties(item_adc, device_prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(device_adc);
            }
        }
        else
        {
            ezlopi_device_free_device(device_adc);
        }
        ret = 1;
    }
    return ret;
}

static int __0055_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (NULL != item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
        {
            // initialize analog_pin
            ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit);
            ret = 1;
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
            char valueFormatted[20];
            snprintf(valueFormatted, 20, "%i", RS_0055);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cj_result, "value", RS_0055);
            ret = 1;
        }
    }
    return ret;
}
static int __0055_notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0,
                                                     .voltage = 0};
        // extract the sensor_output_values
        int prev_RS_0055 = RS_0055;
        ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
        float Vout = (ezlopi_analog_data.voltage) / 1000.0f; // millivolt -> voltage

        // calculate the 'RS_0055' resistance value using [voltage divider rule]
        RS_0055 = (int)(((flex_Vin / Vout) - 1) * flex_Rout);
        if (prev_RS_0055 != RS_0055)
        {
            ezlopi_device_value_updated_from_device_v3(item);
        }
        ret = 1;
    }
    return ret;
}
//------------------------------------------------------------------------------------------------------
