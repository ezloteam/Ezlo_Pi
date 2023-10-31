#include "esp_err.h"
#include "cJSON.h"
#include "driver/gpio.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"

#include "items.h"
#include "trace.h"
#include "gpio_isr_service.h"

#include "sensor_0023_digitalIn_touch_switch_TTP223B.h"

static bool sensor_bme280_initialized = false;

static int __prepare(void *arg);
static void __prepare_touch_switch_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_touch_switch_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static int __init(l_ezlopi_item_t *item);
static void touch_switch_callback(void *arg);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);

/**
 * @brief Public function to interface bme280. This is used to handles all the action on the bme280 sensor and is the entry point to interface the sensor.
 *
 * @param action e_ezlopi_actions_t
 * @param arg Other arguments if needed
 * @return int
 */
int sensor_0023_digitalIn_touch_switch_TTP223B(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __get_cjson_value(item, arg);
        break;
    }
    default:
    {
        break;
    }
    }
    return 0;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    cJSON *param = (cJSON *)arg;
    if (param)
    {
        cJSON_AddBoolToObject(param, "value", item->interface.gpio.gpio_in.value);
        char *valueFormatted = ezlopi_valueformatter_bool(item->interface.gpio.gpio_in.value ? true : false);
        cJSON_AddStringToObject(param, "valueFormatted", valueFormatted);
    }

    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;

    const gpio_config_t touch_switch_config = {
        .pin_bit_mask = (1ULL << item->interface.gpio.gpio_in.gpio_num),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = (item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };

    ESP_ERROR_CHECK(gpio_config(&touch_switch_config));
    gpio_isr_service_register_v3(item, touch_switch_callback, 200);

    return ret;
}

static void touch_switch_callback(void *arg)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)arg;
    item->interface.gpio.gpio_in.value = !item->interface.gpio.gpio_in.value;
    ezlopi_device_value_updated_from_device_v3(item);
}

static int __prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (cJSON *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *touch_device = ezlopi_device_add_device();
        if (touch_device)
        {
            __prepare_touch_switch_device_cloud_properties(touch_device, prep_arg->cjson_device);
            l_ezlopi_item_t *touch_switch_item = ezlopi_device_add_item_to_device(touch_device, sensor_0023_digitalIn_touch_switch_TTP223B);
            if (touch_switch_item)
            {
                __prepare_touch_switch_properties(touch_switch_item, prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(touch_device);
            }
        }
        else
        {
            ezlopi_device_free_device(touch_device);
        }
    }

    return ret;
}

static void __prepare_touch_switch_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    ASSIGN_DEVICE_NAME_V2(device, device_name);

    device->cloud_properties.category = category_switch;
    device->cloud_properties.subcategory = subcategory_in_wall;
    device->cloud_properties.device_type = dev_type_switch_outlet;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __prepare_touch_switch_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.scale = true;
    item->cloud_properties.item_name = ezlopi_item_name_switch;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.gpio.gpio_in.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "ip_inv", item->interface.gpio.gpio_in.invert);
    int val_ip = 0;
    CJSON_GET_VALUE_INT(cj_device, "val_ip", val_ip);

    item->interface.gpio.gpio_in.value = (true == item->interface.gpio.gpio_in.invert) ? !val_ip : val_ip;
    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_POSEDGE;
    item->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
}

#if 0
static int sensor_touch_tpp_223b_prepare_and_add(void *args);
static s_ezlopi_device_properties_t *sensor_touch_tpp_223b_prepare(cJSON *cjson_device);
static int sensor_touch_tpp_223b_init(s_ezlopi_device_properties_t *properties);
static void sensor_touch_tpp_223b_value_updated_from_device(s_ezlopi_device_properties_t *properties);
static int sensor_touch_tpp_223b_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);

int sensor_touch_ttp_223b(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_touch_tpp_223b_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_touch_tpp_223b_init(ezlo_device);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_touch_tpp_223b_get_value_cjson(ezlo_device, arg);
        break;
    }

    default:
    {
        break;
    }
    }

    return ret;
}

static int sensor_touch_tpp_223b_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_touch_tpp_223b_properties = sensor_touch_tpp_223b_prepare(device_prep_arg->cjson_device);
        if (sensor_touch_tpp_223b_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, sensor_touch_tpp_223b_properties, NULL))
            {
                free(sensor_touch_tpp_223b_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static s_ezlopi_device_properties_t *sensor_touch_tpp_223b_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_touch_tpp_223b_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (sensor_touch_tpp_223b_properties)
    {
        memset(sensor_touch_tpp_223b_properties, 0, sizeof(s_ezlopi_device_properties_t));
        sensor_touch_tpp_223b_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_touch_tpp_223b_properties, device_name);
        sensor_touch_tpp_223b_properties->ezlopi_cloud.category = category_switch;
        sensor_touch_tpp_223b_properties->ezlopi_cloud.subcategory = subcategory_in_wall;
        sensor_touch_tpp_223b_properties->ezlopi_cloud.item_name = ezlopi_item_name_switch;
        sensor_touch_tpp_223b_properties->ezlopi_cloud.device_type = dev_type_switch_outlet;
        sensor_touch_tpp_223b_properties->ezlopi_cloud.value_type = value_type_bool;
        sensor_touch_tpp_223b_properties->ezlopi_cloud.has_getter = true;
        sensor_touch_tpp_223b_properties->ezlopi_cloud.has_setter = false;
        sensor_touch_tpp_223b_properties->ezlopi_cloud.reachable = true;
        sensor_touch_tpp_223b_properties->ezlopi_cloud.battery_powered = false;
        sensor_touch_tpp_223b_properties->ezlopi_cloud.show = true;
        sensor_touch_tpp_223b_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_touch_tpp_223b_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        sensor_touch_tpp_223b_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_touch_tpp_223b_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_touch_tpp_223b_properties->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "ip_inv", sensor_touch_tpp_223b_properties->interface.gpio.gpio_in.invert);
        CJSON_GET_VALUE_INT(cjson_device, "val_ip", sensor_touch_tpp_223b_properties->interface.gpio.gpio_in.value);

        sensor_touch_tpp_223b_properties->interface.gpio.gpio_in.enable = true;
        sensor_touch_tpp_223b_properties->interface.gpio.gpio_in.interrupt = GPIO_INTR_POSEDGE;
        sensor_touch_tpp_223b_properties->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
    }

    return sensor_touch_tpp_223b_properties;
}

static int sensor_touch_tpp_223b_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (GPIO_IS_VALID_GPIO(properties->interface.gpio.gpio_in.gpio_num))
    {
        const gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << properties->interface.gpio.gpio_in.gpio_num),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = (properties->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_POSEDGE,
        };

        ret = gpio_config(&io_conf);
        if (ret)
        {
            TRACE_E("Error initializing PIR sensor");
        }
        else
        {
            TRACE_I("PIR sensor initialize successfully.");
            properties->interface.gpio.gpio_in.value = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
        }

        gpio_isr_service_register(properties, sensor_touch_tpp_223b_value_updated_from_device, 200);
    }

    return ret;
}

static void sensor_touch_tpp_223b_value_updated_from_device(s_ezlopi_device_properties_t *properties)
{
    ezlopi_device_value_updated_from_device(properties);
}

static int sensor_touch_tpp_223b_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)args;
    if (cjson_propertise)
    {
        int gpio_level = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
        properties->interface.gpio.gpio_in.value = 0 == properties->interface.gpio.gpio_in.invert ? gpio_level : !gpio_level;
        cJSON_AddBoolToObject(cjson_propertise, "value", properties->interface.gpio.gpio_in.value);
        ret = 1;
    }

    return ret;
}
#endif // if 0
