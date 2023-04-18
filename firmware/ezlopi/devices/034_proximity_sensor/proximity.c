#include "ezlopi_cloud.h"
#include "proximity.h"
#include "gpio_isr_service.h"
#include "ezlopi_cloud_value_type_str.h"


int proximity_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *args, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
        case EZLOPI_ACTION_PREPARE:
        {
            ret = proximity_sensor_prepare_and_add(args);
            break;
        }
        case EZLOPI_ACTION_INITIALIZE:
        {
            ret = proximity_sensor_init(properties);
            break;
        }
        case EZLOPI_ACTION_GET_EZLOPI_VALUE:
        {
            ret = proximity_sensor_get_value_cjson(properties, args);
            break;
        }

        default:
        {
            break;
        }
    }

    return ret;
}

static int proximity_sensor_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *proximity_sensor_properties = proximity_sensor_prepare(device_prep_arg->cjson_device);
        if (proximity_sensor_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, proximity_sensor_properties, NULL))
            {
                free(proximity_sensor_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static s_ezlopi_device_properties_t *proximity_sensor_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *proximity_sensor_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (proximity_sensor_properties)
    {
        memset(proximity_sensor_properties, 0, sizeof(s_ezlopi_device_properties_t));
        proximity_sensor_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(proximity_sensor_properties, device_name);
        proximity_sensor_properties->ezlopi_cloud.category = category_security_sensor;
        proximity_sensor_properties->ezlopi_cloud.subcategory = subcategory_motion;
        proximity_sensor_properties->ezlopi_cloud.item_name = ezlopi_item_name_motion;
        proximity_sensor_properties->ezlopi_cloud.device_type = dev_type_sensor_motion;
        proximity_sensor_properties->ezlopi_cloud.value_type = value_type_bool;
        proximity_sensor_properties->ezlopi_cloud.has_getter = true;
        proximity_sensor_properties->ezlopi_cloud.has_setter = false;
        proximity_sensor_properties->ezlopi_cloud.reachable = true;
        proximity_sensor_properties->ezlopi_cloud.battery_powered = false;
        proximity_sensor_properties->ezlopi_cloud.show = true;
        proximity_sensor_properties->ezlopi_cloud.room_name[0] = '\0';
        proximity_sensor_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        proximity_sensor_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        proximity_sensor_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", proximity_sensor_properties->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "logic_inv", proximity_sensor_properties->interface.gpio.gpio_in.invert);
        // CJSON_GET_VALUE_INT(cjson_device, "val_ip", proximity_sensor_properties->interface.gpio.gpio_in.value);

        proximity_sensor_properties->interface.gpio.gpio_in.enable = true;
        proximity_sensor_properties->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
        proximity_sensor_properties->interface.gpio.gpio_in.pull = GPIO_PULLUP_ONLY;
    }

    return proximity_sensor_properties;
}

static int proximity_sensor_init(s_ezlopi_device_properties_t *properties)
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
            TRACE_E("Error initializing Proximity sensor");
        }
        else
        {
            TRACE_I("Proximity sensor initialize successfully.");
            properties->interface.gpio.gpio_in.value = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
        }

        gpio_isr_service_register(properties, proximity_sensor_value_updated_from_device, 200);
    }

    return ret;
}

static void proximity_sensor_value_updated_from_device(s_ezlopi_device_properties_t *properties)
{
    ezlopi_device_value_updated_from_device(properties);
}

static int proximity_sensor_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
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
