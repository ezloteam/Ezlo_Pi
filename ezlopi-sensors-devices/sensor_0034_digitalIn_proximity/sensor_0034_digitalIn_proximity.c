#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_gpioisr.h"
#include "sensor_0034_digitalIn_proximity.h"

static int proximity_sensor_prepare(void *args);
static int proximity_sensor_init(l_ezlopi_item_t *item);
static void proximity_sensor_value_updated_from_device(void *arg);
static int proximity_sensor_get_value_cjson(l_ezlopi_item_t *item, void *args);

int sensor_0034_digitalIn_proximity(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *args, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = proximity_sensor_prepare(args);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = proximity_sensor_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = proximity_sensor_get_value_cjson(item, args);
        break;
    }

    default:
    {
        break;
    }
    }

    return ret;
}

static void proximity_sensor_setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    if (device && cj_device)
    {
        // char *device_name = NULL;
        // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
        // ASSIGN_DEVICE_NAME_V2(device, device_name);
        // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

        device->cloud_properties.category = category_generic_sensor;
        device->cloud_properties.subcategory = subcategory_motion;
        device->cloud_properties.device_type = dev_type_sensor_motion;
        device->cloud_properties.info = NULL;
        device->cloud_properties.device_type_id = NULL;
    }
}

static void proximity_sensor_setup_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    if (item && cj_device)
    {
        item->cloud_properties.has_getter = true;
        item->cloud_properties.has_setter = false;
        item->cloud_properties.item_name = ezlopi_item_name_motion;
        item->cloud_properties.value_type = value_type_bool;
        item->cloud_properties.show = true;
        item->cloud_properties.scale = NULL;
        item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);

        item->interface.gpio.gpio_in.enable = true;
        item->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
        CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_name_str, item->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_INT(cj_device, "logic_inv", item->interface.gpio.gpio_in.invert);
        // CJSON_GET_VALUE_INT(cj_device, "pull_up", tmp_var);
        item->interface.gpio.gpio_in.pull = GPIO_PULLUP_ONLY; // tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
        item->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
    }
}

static int proximity_sensor_prepare(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        l_ezlopi_device_t *device = ezlopi_device_add_device(device_prep_arg->cjson_device);
        if (device)
        {
            proximity_sensor_setup_device_cloud_properties(device, device_prep_arg->cjson_device);
            l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, sensor_0034_digitalIn_proximity);
            if (item)
            {
                item->cloud_properties.device_id = device->cloud_properties.device_id;
                proximity_sensor_setup_item_properties(item, device_prep_arg->cjson_device);
                ret = 1;
            }
            else
            {
                ret = -1;
            }
        }
    }

    return ret;
}

static int proximity_sensor_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
    {
        const gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << item->interface.gpio.gpio_in.gpio_num),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = (item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
            .intr_type = item->interface.gpio.gpio_in.interrupt,
        };

        ret = gpio_config(&io_conf);
        if (ESP_OK == ret)
        {
            gpio_isr_service_register_v3(item, proximity_sensor_value_updated_from_device, 200);
            item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
            TRACE_I("Proximity sensor initialize successfully.");
        }
        else
        {
            TRACE_E("Error initializing Proximity sensor");
        }
    }

    return ret;
}

static void proximity_sensor_value_updated_from_device(void *arg)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)arg;
    if (item)
    {
        ezlopi_device_value_updated_from_device_v3(item);
    }
}

static int proximity_sensor_get_value_cjson(l_ezlopi_item_t *item, void *args)
{
    int ret = 0;
    cJSON *cj_result = (cJSON *)args;
    if (cj_result)
    {
        item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
        if (item->interface.gpio.gpio_in.invert)
        {
            item->interface.gpio.gpio_in.value = item->interface.gpio.gpio_in.value ? false : true;
        }
        cJSON_AddBoolToObject(cj_result, ezlopi_value_str, item->interface.gpio.gpio_in.value);
        char *valueFormatted = ezlopi_valueformatter_bool(item->interface.gpio.gpio_in.value ? false : true);
        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
        ret = 1;
        // TRACE_D("value: %d", item->interface.gpio.gpio_in.value);
    }

    return ret;
}
