

#include "sensor_0023_digitalIn_touch_switch_TTP223B.h"
#include "gpio_isr_service.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"

#include "ezlopi_cloud.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "items.h"
#include "trace.h"
#include "cJSON.h"

static int sensor_touch_tpp_223b_switch_prepare_and_add(void *args);
static s_ezlopi_device_properties_t *sensor_touch_tpp_223b_switch_prepare(cJSON *cjson_device);
static int sensor_touch_tpp_223b_switch_init(s_ezlopi_device_properties_t *properties);
static void sensor_touch_tpp_223b_switch_value_updated_from_device(s_ezlopi_device_properties_t *properties);
static int sensor_touch_tpp_223b_switch_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);

int sensor_0023_digitalIn_touch_switch_TTP223B(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_touch_tpp_223b_switch_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_touch_tpp_223b_switch_init(ezlo_device);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_touch_tpp_223b_switch_get_value_cjson(ezlo_device, arg);
        break;
    }

    default:
    {
        break;
    }
    }

    return ret;
}

static int sensor_touch_tpp_223b_switch_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_touch_tpp_223b_switch_properties = sensor_touch_tpp_223b_switch_prepare(device_prep_arg->cjson_device);
        if (sensor_touch_tpp_223b_switch_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, sensor_touch_tpp_223b_switch_properties, NULL))
            {
                free(sensor_touch_tpp_223b_switch_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static s_ezlopi_device_properties_t *sensor_touch_tpp_223b_switch_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_touch_tpp_223b_switch_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (sensor_touch_tpp_223b_switch_properties)
    {
        memset(sensor_touch_tpp_223b_switch_properties, 0, sizeof(s_ezlopi_device_properties_t));
        sensor_touch_tpp_223b_switch_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_touch_tpp_223b_switch_properties, device_name);
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.category = category_switch;
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.subcategory = subcategory_in_wall;
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.item_name = ezlopi_item_name_switch;
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.device_type = dev_type_switch_outlet;
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.value_type = value_type_bool;
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.has_getter = true;
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.has_setter = false;
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.reachable = true;
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.battery_powered = false;
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.show = true;
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_touch_tpp_223b_switch_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_touch_tpp_223b_switch_properties->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "ip_inv", sensor_touch_tpp_223b_switch_properties->interface.gpio.gpio_in.invert);

        int val_ip = 0;
        CJSON_GET_VALUE_INT(cjson_device, "val_ip", val_ip);


        sensor_touch_tpp_223b_switch_properties->interface.gpio.gpio_in.value = (true == sensor_touch_tpp_223b_switch_properties->interface.gpio.gpio_in.invert) ? !val_ip : val_ip;
        sensor_touch_tpp_223b_switch_properties->interface.gpio.gpio_in.enable = true;
        sensor_touch_tpp_223b_switch_properties->interface.gpio.gpio_in.interrupt = GPIO_INTR_POSEDGE;
        sensor_touch_tpp_223b_switch_properties->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
    }

    return sensor_touch_tpp_223b_switch_properties;
}

static int sensor_touch_tpp_223b_switch_init(s_ezlopi_device_properties_t *properties)
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
            TRACE_E("Error initializing touch sensor");
        }
        gpio_isr_service_register(properties, sensor_touch_tpp_223b_switch_value_updated_from_device, 200);
    }

    return ret;
}

static void sensor_touch_tpp_223b_switch_value_updated_from_device(s_ezlopi_device_properties_t *properties)
{
    properties->interface.gpio.gpio_in.value = !properties->interface.gpio.gpio_in.value;
    ezlopi_device_value_updated_from_device(properties);
}

static int sensor_touch_tpp_223b_switch_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)args;
    char valueFormatted[20];
    if (cjson_propertise)
    {
        snprintf(valueFormatted, 20, "%s", ((0 == properties->interface.gpio.gpio_in.value) ? "false" : "true"));
        cJSON_AddStringToObject(cjson_propertise, "valueFormatted", valueFormatted);
        cJSON_AddBoolToObject(cjson_propertise, "value", properties->interface.gpio.gpio_in.value);
        ret = 1;
    }

    return ret;
}