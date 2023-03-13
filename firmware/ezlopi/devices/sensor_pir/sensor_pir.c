

#include "ezlopi_cloud.h"
#include "sensor_pir.h"
#include "gpio_isr_service.h"

static int sensor_pir_prepare_and_add(void *args);
static int sensor_pir_init(s_ezlopi_device_properties_t *properties);
static s_ezlopi_device_properties_t *sensor_pir_prepare(cJSON *cjson_device);
static void sensor_pir_value_updated_from_device(s_ezlopi_device_properties_t *properties);
static int sensor_pir_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);

int sensor_pir(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *args, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_pir_prepare_and_add(args);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_pir_init(properties);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_pir_get_value_cjson(properties, args);
        break;
    }

    default:
    {
        break;
    }
    }

    return ret;
}

static int sensor_pir_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_pir_properties = sensor_pir_prepare(device_prep_arg->cjson_device);
        if (sensor_pir_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, sensor_pir_properties, NULL))
            {
                free(sensor_pir_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static s_ezlopi_device_properties_t *sensor_pir_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_pir_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (sensor_pir_properties)
    {
        memset(sensor_pir_properties, 0, sizeof(s_ezlopi_device_properties_t));
        sensor_pir_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_pir_properties, device_name);
        sensor_pir_properties->ezlopi_cloud.category = category_security_sensor;
        sensor_pir_properties->ezlopi_cloud.subcategory = subcategory_motion;
        sensor_pir_properties->ezlopi_cloud.item_name = ezlopi_item_name_motion;
        sensor_pir_properties->ezlopi_cloud.device_type = dev_type_sensor_motion;
        sensor_pir_properties->ezlopi_cloud.value_type = value_type_bool;
        sensor_pir_properties->ezlopi_cloud.has_getter = true;
        sensor_pir_properties->ezlopi_cloud.has_setter = false;
        sensor_pir_properties->ezlopi_cloud.reachable = true;
        sensor_pir_properties->ezlopi_cloud.battery_powered = false;
        sensor_pir_properties->ezlopi_cloud.show = true;
        sensor_pir_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_pir_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        sensor_pir_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_pir_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_pir_properties->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "ip_inv", sensor_pir_properties->interface.gpio.gpio_in.invert);
        CJSON_GET_VALUE_INT(cjson_device, "val_ip", sensor_pir_properties->interface.gpio.gpio_in.value);

        sensor_pir_properties->interface.gpio.gpio_in.enable = true;
        sensor_pir_properties->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
        sensor_pir_properties->interface.gpio.gpio_in.pull = GPIO_PULLUP_ONLY;
    }

    return sensor_pir_properties;
}

static int sensor_pir_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (GPIO_IS_VALID_GPIO(properties->interface.gpio.gpio_in.gpio_num))
    {
        const gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << properties->interface.gpio.gpio_in.gpio_num),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = (properties->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
            .intr_type = properties->interface.gpio.gpio_in.interrupt,
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

        gpio_isr_service_register(properties, sensor_pir_value_updated_from_device, 200);
    }

    return ret;
}

static void sensor_pir_value_updated_from_device(s_ezlopi_device_properties_t *properties)
{
    ezlopi_device_value_updated_from_device(properties);
}

static int sensor_pir_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)args;
    if (cjson_propertise)
    {
        properties->interface.gpio.gpio_out.value = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
        cJSON_AddBoolToObject(cjson_propertise, "value", properties->interface.gpio.gpio_out.value);
        ret = 1;
    }

    return ret;
}