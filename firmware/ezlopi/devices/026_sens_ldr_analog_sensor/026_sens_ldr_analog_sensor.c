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

#include "026_sens_ldr_analog_sensor.h"

static char *present_light_status = "no_light";
static char *previous_light_status = "no_light";

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_value_cjson(l_ezlopi_item_t *item, void *arg);
static void __setup_device_cloud_params(l_ezlopi_device_t *device, cJSON *cj_device);

int sensor_ldr_analog_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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

static int __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)arg;
    if (cjson_propertise)
    {
        cJSON_AddStringToObject(cjson_propertise, "value", present_light_status);
        ret = 1;
    }

    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, ezlopi_analog_data);
    TRACE_B("Value is: %d, voltage is: %d", ezlopi_analog_data->value, ezlopi_analog_data->voltage);
    if (150 >= ezlopi_analog_data->voltage)
    {
        present_light_status = "no_light";
    }
    else
    {
        present_light_status = "light_detected";
    }
    if (present_light_status != previous_light_status)
    {
        ezlopi_device_value_updated_from_device_v3(item);
        previous_light_status = present_light_status;
    }
    free(ezlopi_analog_data);
    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
    {
        ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit);
        ret = 1;
    }
    return ret;
}

static int __prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *parep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (parep_arg)
    {
        cJSON *cj_device = parep_arg->cjson_device;
        if (cj_device)
        {
            l_ezlopi_device_t *device = ezlopi_device_add_device();
            if (device)
            {
                __setup_device_cloud_params(device, cj_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, sensor_ldr_analog_v3);
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
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}
