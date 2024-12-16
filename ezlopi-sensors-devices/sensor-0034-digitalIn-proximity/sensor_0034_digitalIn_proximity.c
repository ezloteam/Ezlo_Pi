#include "ezlopi_util_trace.h"

// #include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_gpioisr.h"

#include "sensor_0034_digitalIn_proximity.h"

static ezlopi_error_t proximity_sensor_prepare(void *args);
static ezlopi_error_t proximity_sensor_init(l_ezlopi_item_t *item);
static void proximity_sensor_value_updated_from_device(void *arg);
static ezlopi_error_t proximity_sensor_get_value_cjson(l_ezlopi_item_t *item, void *args);

ezlopi_error_t sensor_0034_digitalIn_proximity(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *args, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;

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
        item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

        CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);

        item->interface.gpio.gpio_in.enable = true;
        item->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
        CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_logic_inv_str, item->interface.gpio.gpio_in.invert);
        // CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_pull_up_str, tmp_var);
        item->interface.gpio.gpio_in.pull = GPIO_PULLUP_ONLY; // tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
        item->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
    }
}

static ezlopi_error_t proximity_sensor_prepare(void *args)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        l_ezlopi_device_t *device = EZPI_core_device_add_device(device_prep_arg->cjson_device, NULL);
        if (device)
        {
            proximity_sensor_setup_device_cloud_properties(device, device_prep_arg->cjson_device);
            l_ezlopi_item_t *item = EZPI_core_device_add_item_to_device(device, sensor_0034_digitalIn_proximity);
            if (item)
            {
                proximity_sensor_setup_item_properties(item, device_prep_arg->cjson_device);
                ret = EZPI_SUCCESS;
            }
            else
            {
                EZPI_core_device_free_device(device);
            }
        }
    }
    return ret;
}

static ezlopi_error_t proximity_sensor_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
        {
            const gpio_config_t io_conf = {
                .pin_bit_mask = (1ULL << item->interface.gpio.gpio_in.gpio_num),
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = (item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
                .intr_type = item->interface.gpio.gpio_in.interrupt,
            };

            if (ESP_OK == gpio_config(&io_conf))
            {
                EZPI_service_gpioisr_register_v3(item, proximity_sensor_value_updated_from_device, 200);
                item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
                TRACE_I("Proximity sensor initialize successfully.");
                ret = EZPI_SUCCESS;
            }
            else
            {
                TRACE_E("Error initializing Proximity sensor");
            }
        }
    }

    return ret;
}

static void proximity_sensor_value_updated_from_device(void *arg)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)arg;
    if (item)
    {
        EZPI_core_device_value_updated_from_device_broadcast(item);
    }
}

static ezlopi_error_t proximity_sensor_get_value_cjson(l_ezlopi_item_t *item, void *args)
{
    ezlopi_error_t ret = EZPI_FAILED;
    cJSON *cj_result = (cJSON *)args;
    if (cj_result)
    {
        item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
        if (item->interface.gpio.gpio_in.invert)
        {
            item->interface.gpio.gpio_in.value = item->interface.gpio.gpio_in.value ? false : true;
        }

        ezlopi_valueformatter_bool_to_cjson(cj_result, item->interface.gpio.gpio_in.value, NULL);
        ret = EZPI_SUCCESS;
        // TRACE_D("value: %d", item->interface.gpio.gpio_in.value);
    }

    return ret;
}
