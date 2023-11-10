

#include "ezlopi_cloud.h"
#include "gpio_isr_service.h"
#include "ezlopi_valueformatter.h"

#include "sensor_0019_digitalIn_PIR.h"

static int sensor_pir_prepare_v3(void *arg);
static int sensor_pir_init_v3(l_ezlopi_item_t *item);
static void sensor_pir_setup_device_cloud_properties_v3(l_ezlopi_device_t *device, cJSON *cj_device);
static void sensor_pir_setup_item_properties_v3(l_ezlopi_item_t *item, cJSON *cj_device);
static int sensor_pir_get_value_cjson_v3(l_ezlopi_item_t *item, void *arg);

int sensor_0019_digitalIn_PIR(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *args, void *user_arg)
{
    int ret = 0;

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

static int sensor_pir_get_value_cjson_v3(l_ezlopi_item_t *item, void *args)
{
    int ret = 0;
    cJSON *cj_result = (cJSON *)args;
    if (cj_result)
    {
        item->interface.gpio.gpio_out.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
        cJSON_AddBoolToObject(cj_result, "value", item->interface.gpio.gpio_out.value);
        char *valueFormatted = ezlopi_valueformatter_bool(item->interface.gpio.gpio_out.value ? true : false);
        cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
        ret = 1;
    }

    return ret;
}

static void sensor_pir_value_updated_from_device_v3(l_ezlopi_item_t *item)
{
    ezlopi_device_value_updated_from_device_v3(item);
}

static int sensor_pir_init_v3(l_ezlopi_item_t *item)
{
    int ret = 0;
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

            ret = gpio_config(&io_conf);
            if (ESP_OK == ret)
            {
                TRACE_I("PIR sensor initialize successfully.");
                item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
                gpio_isr_service_register_v3(item, sensor_pir_value_updated_from_device_v3, 200);
            }
            else
            {
                TRACE_E("Error initializing PIR sensor, error: %s", esp_err_to_name(ret));
            }
        }
    }
    return ret;
}

static int sensor_pir_prepare_v3(void *arg)
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
                sensor_pir_setup_device_cloud_properties_v3(device, cj_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, NULL);
                if (item)
                {

                    item->func = sensor_0019_digitalIn_PIR;
                    item->cloud_properties.device_id = device->cloud_properties.device_id;
                    sensor_pir_setup_item_properties_v3(item, cj_device);
                    ret = 1;
                }
                else
                {
                    ezlopi_device_free_device(device);
                }
            }
        }
    }

    return ret;
}

static void sensor_pir_setup_device_cloud_properties_v3(l_ezlopi_device_t *device, cJSON *cj_device)
{
    if (device && cj_device)
    {
        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME_V2(device, device_name);

        device->cloud_properties.category = category_security_sensor;
        device->cloud_properties.subcategory = subcategory_motion;
        device->cloud_properties.device_type = dev_type_sensor_motion;
        device->cloud_properties.info = NULL;
        device->cloud_properties.device_type_id = NULL;
        device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
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

    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);

    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.gpio.gpio_in.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "logic_inv", item->interface.gpio.gpio_in.invert);
    CJSON_GET_VALUE_INT(cj_device, "pull_up", tmp_var);
    item->interface.gpio.gpio_in.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
}
