#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "jsn_sr04t.h"
#include "sensor_0031_other_JSNSR04T.h"
#include "EZLOPI_USER_CONFIG.h"

static int __prepare(void* arg);
static int __init(l_ezlopi_item_t* item);
static int __notify(l_ezlopi_item_t* item);
static int __get_cjson_value(l_ezlopi_item_t* item, void* arg);

int sensor_0031_other_JSNSR04T(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
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

static int __notify(l_ezlopi_item_t* item)
{
    return ezlopi_device_value_updated_from_device_broadcast(item);
}

static int __get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cj_result = (cJSON*)arg;
        jsn_sr04t_config_t* tmp_config = (jsn_sr04t_config_t*)item->user_arg;
        if (tmp_config)
        {
            jsn_sr04t_data_t jsn_sr04t_data;
            ret = measurement(tmp_config, &jsn_sr04t_data);
            if (ret)
            {
                float distance = (jsn_sr04t_data.distance_cm / 100.0f);
                ezlopi_valueformatter_float_to_cjson(item, cj_result, distance);
                ret = 1;
            }
            else
            {
                ESP_LOGE(TAG1, "ERROR in getting measurement: ret=%d", ret);
            }
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        jsn_sr04t_config_t* jsn_sr04t_config = malloc(sizeof(jsn_sr04t_config_t));
        if (jsn_sr04t_config)
        {
            jsn_sr04t_config_t tmp_config = (jsn_sr04t_config_t)JSN_SR04T_CONFIG_DEFAULT();
            tmp_config.trigger_gpio_num = item->interface.gpio.gpio_out.gpio_num;
            tmp_config.echo_gpio_num = item->interface.gpio.gpio_in.gpio_num;
            tmp_config.rmt_channel = 4;

            memcpy(jsn_sr04t_config, &tmp_config, sizeof(jsn_sr04t_config_t));
            item->user_arg = (void*)jsn_sr04t_config;

            if (ESP_OK == init_JSN_SR04T(jsn_sr04t_config))
            {
                TRACE_S("JSN_SR04T initialized");
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
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_distance;
    item->cloud_properties.value_type = value_type_length;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
}

static void __prepare_item_interface_properties(l_ezlopi_item_t* item, cJSON* cj_device)
{
    item->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT;

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_out_str, item->interface.gpio.gpio_out.gpio_num);
    item->interface.gpio.gpio_out.enable = true;
    item->interface.gpio.gpio_out.interrupt = GPIO_INTR_DISABLE;
    item->interface.gpio.gpio_out.invert = EZLOPI_GPIO_LOGIC_NONINVERTED;
    item->interface.gpio.gpio_out.mode = GPIO_MODE_OUTPUT;
    item->interface.gpio.gpio_out.pull = GPIO_PULLDOWN_ONLY;
    item->interface.gpio.gpio_out.value = 0;

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_in_str, item->interface.gpio.gpio_in.gpio_num);
    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
    item->interface.gpio.gpio_in.invert = EZLOPI_GPIO_LOGIC_NONINVERTED;
    item->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
    item->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_DISABLE;
    item->interface.gpio.gpio_in.value = 0;
}

static int __prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;

    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t* device = ezlopi_device_add_device(prep_arg->cjson_device, NULL);
        if (device)
        {
            ret = 1;
            __prepare_device_cloud_properties(device, prep_arg->cjson_device);
            l_ezlopi_item_t* item_temperature = ezlopi_device_add_item_to_device(device, sensor_0031_other_JSNSR04T);
            if (item_temperature)
            {
                __prepare_item_cloud_properties(item_temperature, prep_arg->cjson_device);
                __prepare_item_interface_properties(item_temperature, prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(device);
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