#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_adc.h"

#include "esp_err.h"
#include "items.h"
#include "trace.h"
#include "cJSON.h"

#include "027_sens_water_leak_sensor.h"

const static char *_no_water_leak = "no_water_leak";
const static char *_water_leak_detected = "water_leak_detected";

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __get_ezlopi_value(l_ezlopi_item_t *item, void *arg);
static int __notify(l_ezlopi_item_t *item);

int water_leak_sensor_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __get_ezlopi_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_200_MS:
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

static void prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_leak;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_water_leak_alarm;
    item->cloud_properties.value_type = value_type_token;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
}

static void prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;
    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3;
}

static int __prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg)
    {
        cJSON *cj_device = prep_arg->cjson_device;
        if (cj_device)
        {
            l_ezlopi_device_t *device = ezlopi_device_add_device();
            if (device)
            {
                prepare_device_cloud_properties(device, cj_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, water_leak_sensor_v3);
                if (item)
                {
                    prepare_item_cloud_properties(item, cj_device);
                    prepare_item_interface_properties(item, cj_device);
                }
            }
        }
    }

    return ret;
}

static int __get_ezlopi_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            cJSON_AddStringToObject(cj_result, "value", (char *)item->user_arg ? item->user_arg : _no_water_leak);
            ret = 1;
        }
    }
    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;

    if (item)
    {
        char *curret_value = NULL;
        s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};

        ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
        TRACE_B("Value is: %d, voltage is: %d", ezlopi_analog_data.value, ezlopi_analog_data.value);

        if (1000 <= ezlopi_analog_data.voltage)
        {
            curret_value = _water_leak_detected;
        }
        else
        {
            curret_value = _no_water_leak;
        }

        if (curret_value != (char *)item->user_arg)
        {
            item->user_arg = (void *)curret_value;
            ezlopi_device_value_updated_from_device_v3(item);
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        int ret = 0;
        if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
        {
            ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit);
            ret = 1;
        }
        return ret;
    }
    return ret;
}
