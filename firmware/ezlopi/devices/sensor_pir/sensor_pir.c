

#include "sensor_pir.h"
#include "gpio_isr_service.h"


static int sensor_pir_prepare(void* arg);
static int sensor_pir_init(s_ezlopi_device_properties_t* properties);
static int sensor_pir_get_value_cjson(s_ezlopi_device_properties_t* properties, void* args);
static int sensor_pir_get_value(s_ezlopi_device_properties_t* properties);
static void sensor_pir_value_updated_from_device(s_ezlopi_device_properties_t* properties);




int ezlopi_pir_begin(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg)
{
     int ret = 0;

    switch (action)
    {
        case EZLOPI_ACTION_PREPARE:
        {
            TRACE_I("EZLOPI_ACTION_PREPARE");
            ret = sensor_pir_prepare(arg);
            break;
        }
        case EZLOPI_ACTION_INITIALIZE:
        {
            TRACE_I("EZLOPI_ACTION_INITIALIZE event.");
            ret = sensor_pir_init(properties);
            break;
        }
        case EZLOPI_ACTION_GET_EZLOPI_VALUE:
        {
            ret = sensor_pir_get_value_cjson(properties, arg);
            break;
        }
        // case EZLOPI_ACTION_NOTIFY_1000_MS:
        // {
        //     // TRACE_I("EZLOPI_ACTION_NOTIFY_100_MS");
        //     // sensor_pir_get_value(properties);
        //     ezlopi_device_value_updated_from_device(properties);
        //     ret = 0;
        //     break;
        // }
        default:
        {
            TRACE_I("Default action encountered.(action : %s)", ezlopi_actions_to_string(action));
            break;
        }
    }

    return ret;
}

static int sensor_pir_get_value(s_ezlopi_device_properties_t* properties)
{
    int detection = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
    TRACE_E("Sensor value is: %d", detection);
    return 0;
}


static int sensor_pir_prepare(void* arg)
{
    cJSON* cjson_device = (cJSON*)arg;
    s_ezlopi_device_properties_t* sensor_pir_properties = NULL;
    if((NULL == sensor_pir_properties) && (NULL != cjson_device))
    {
        sensor_pir_properties = malloc(sizeof(s_ezlopi_device_properties_t));
        if (sensor_pir_properties)
        {
            int tmp_var = 0;
            memset(sensor_pir_properties, 0, sizeof(s_ezlopi_device_properties_t));
            sensor_pir_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;
            char *device_name = NULL;
            char * cjson_device_str = cJSON_Print(cjson_device);
            TRACE_B("cjson str: %s", cjson_device_str);
            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
            TRACE_E("cjson_device: %p, device_name: %p", cjson_device, device_name);
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
            sensor_pir_properties->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
            sensor_pir_properties->ezlopi_cloud.room_id = ezlopi_device_generate_room_id();
            sensor_pir_properties->ezlopi_cloud.item_id = ezlopi_device_generate_item_id();
            // CJSON_GET_VALUE_INT(cjson_device, "id_room", sensor_pir_properties->ezlopi_cloud.room_id);
            // CJSON_GET_VALUE_INT(cjson_device, "id_item", sensor_pir_properties->ezlopi_cloud.item_id);

            CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_pir_properties->interface.gpio.gpio_in.gpio_num);
            CJSON_GET_VALUE_INT(cjson_device, "ip_inv", sensor_pir_properties->interface.gpio.gpio_in.invert);
            CJSON_GET_VALUE_INT(cjson_device, "val_ip", sensor_pir_properties->interface.gpio.gpio_in.value);

            sensor_pir_properties->interface.gpio.gpio_in.enable = true;
            sensor_pir_properties->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
            sensor_pir_properties->interface.gpio.gpio_in.pull = GPIO_PULLUP_ONLY;            
        }
    }

    return ((int)sensor_pir_properties); 
    
}


static int sensor_pir_init(s_ezlopi_device_properties_t* properties)
{
    int ret = 0;
    if (GPIO_IS_VALID_GPIO(properties->interface.gpio.gpio_in.gpio_num))
        {
            const gpio_config_t io_conf = {
                .pin_bit_mask = (1ULL << properties->interface.gpio.gpio_in.gpio_num),
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = (properties->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_DISABLE,
            };

            ret = gpio_config(&io_conf);
            if(ret)
            {
                TRACE_E("Error initializing PIR sensor");
            }
            else
            {
                TRACE_I("PIR sensor initialize successfully.");
                properties->interface.gpio.gpio_in.value = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
            }

            gpio_isr_service_register(properties, ezlopi_device_value_updated_from_device);
        }
        
        return ret;
}

static void sensor_pir_value_updated_from_device(s_ezlopi_device_properties_t* properties)
{
    ezlopi_device_value_updated_from_device(properties);
}

static int sensor_pir_get_value_cjson(s_ezlopi_device_properties_t* properties, void* args)
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


