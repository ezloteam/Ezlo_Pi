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

#include "sensor_0019_digitalIn_PIR.h"

static ezlopi_error_t sensor_pir_prepare_v3(void *arg);
static ezlopi_error_t sensor_pir_init_v3(l_ezlopi_item_t *item);
static void sensor_pir_value_updated_from_device_v3(void *arg);
static ezlopi_error_t sensor_pir_get_value_cjson_v3(l_ezlopi_item_t *item, void *arg);
static void sensor_pir_setup_item_properties_v3(l_ezlopi_item_t *item, cJSON *cj_device);
static void sensor_pir_setup_device_cloud_properties_v3(l_ezlopi_device_t *device, cJSON *cj_device);

ezlopi_error_t sensor_0019_digitalIn_PIR(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *args, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_pir_prepare_v3(args);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_pir_init_v3(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_pir_get_value_cjson_v3(item, args);
        break;
    }

    default:
    {
        break;
    }
    }

    return ret;
}

static ezlopi_error_t sensor_pir_get_value_cjson_v3(l_ezlopi_item_t *item, void *args)
{
    ezlopi_error_t ret = EZPI_FAILED;
    cJSON *cj_result = (cJSON *)args;
    if (cj_result)
    {
        item->interface.gpio.gpio_out.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
        ezlopi_valueformatter_bool_to_cjson(cj_result, item->interface.gpio.gpio_out.value, NULL);
        ret = EZPI_SUCCESS;
    }

    return ret;
}

static void sensor_pir_value_updated_from_device_v3(void *arg)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)arg;
    if (item)
    {
        ezlopi_device_value_updated_from_device_broadcast(item);
    }
}

static ezlopi_error_t sensor_pir_init_v3(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
        {
            const gpio_config_t io_conf = {
                .pin_bit_mask = (1ULL << item->interface.gpio.gpio_in.gpio_num),
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE, // (item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
                .intr_type = item->interface.gpio.gpio_in.interrupt,
            };

            if (ESP_OK == gpio_config(&io_conf))
            {
                TRACE_I("PIR sensor initialize successfully.");
                item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
                EZPI_service_gpioisr_register_v3(item, sensor_pir_value_updated_from_device_v3, 200);
            }
            else
            {
                TRACE_E("Error initializing PIR sensor, error: %s", esp_err_to_name(ret));
                ret = EZPI_ERR_INIT_DEVICE_FAILED;
            }
        }
        else
        {
            ret = EZPI_ERR_INIT_DEVICE_FAILED;
        }
    }
    else
    {
        ret = EZPI_ERR_INIT_DEVICE_FAILED;
    }
    return ret;
}

static ezlopi_error_t sensor_pir_prepare_v3(void *arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg)
    {
        cJSON *cj_device = prep_arg->cjson_device;
        if (cj_device)
        {
            l_ezlopi_device_t *device = ezlopi_device_add_device(prep_arg->cjson_device, NULL);
            if (device)
            {
                sensor_pir_setup_device_cloud_properties_v3(device, cj_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, sensor_0019_digitalIn_PIR);
                if (item)
                {
                    sensor_pir_setup_item_properties_v3(item, cj_device);
                }
                else
                {
                    ezlopi_device_free_device(device);
                    ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                }
            }
            else
            {
                ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
            }
        }
        else
        {
            ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
        }
    }
    else
    {
        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    }

    return ret;
}

static void sensor_pir_setup_device_cloud_properties_v3(l_ezlopi_device_t *device, cJSON *cj_device)
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

static void sensor_pir_setup_item_properties_v3(l_ezlopi_item_t *item, cJSON *cj_device)
{
    int tmp_var = 0;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_motion;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);

    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.gpio.gpio_in.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_logic_inv_str, item->interface.gpio.gpio_in.invert);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_pull_up_str, tmp_var);
    item->interface.gpio.gpio_in.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
}
