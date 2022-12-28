#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_adc.h"

#include "esp_err.h"
#include "items.h"
#include "trace.h"
#include "cJSON.h"

#include "027_sens_water_sensor.h"

static char *ezlopi_water_previous_leak_state = NULL;
static char *ezlopi_water_present_leak_state = NULL;

static int sensor_water_sensor_prepare_and_add(void *args);
static s_ezlopi_device_properties_t *sensor_water_sensor_prepare(cJSON *cjson_device);
static int sensor_water_sensor_init(s_ezlopi_device_properties_t *properties);
static int get_sensor_water_sensor_value_to_cloud(s_ezlopi_device_properties_t *properties, void *args);
static int sensor_water_sensor_get_value(s_ezlopi_device_properties_t *properties);

int sensor_water_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        TRACE_I("%s", ezlopi_actions_to_string(action));
        ret = sensor_water_sensor_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        TRACE_I("%s", ezlopi_actions_to_string(action));
        ret = sensor_water_sensor_init(ezlo_device);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        TRACE_I("%s", ezlopi_actions_to_string(action));
        get_sensor_water_sensor_value_to_cloud(ezlo_device, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_200_MS:
    {
        TRACE_I("%s", ezlopi_actions_to_string(action));
        sensor_water_sensor_get_value(ezlo_device);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static int sensor_water_sensor_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_water_sensor_properties = sensor_water_sensor_prepare(device_prep_arg->cjson_device);
        if (sensor_water_sensor_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, sensor_water_sensor_properties, NULL))
            {
                free(sensor_water_sensor_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static s_ezlopi_device_properties_t *sensor_water_sensor_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_water_sensor_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (sensor_water_sensor_properties)
    {
        memset(sensor_water_sensor_properties, 0, sizeof(s_ezlopi_device_properties_t));
        sensor_water_sensor_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_water_sensor_properties, device_name);
        sensor_water_sensor_properties->ezlopi_cloud.category = category_security_sensor;
        sensor_water_sensor_properties->ezlopi_cloud.subcategory = subcategory_leak;
        sensor_water_sensor_properties->ezlopi_cloud.item_name = ezlopi_item_name_water_leak_alarm;
        sensor_water_sensor_properties->ezlopi_cloud.device_type = dev_type_sensor;
        sensor_water_sensor_properties->ezlopi_cloud.value_type = value_type_token;
        sensor_water_sensor_properties->ezlopi_cloud.has_getter = true;
        sensor_water_sensor_properties->ezlopi_cloud.has_setter = false;
        sensor_water_sensor_properties->ezlopi_cloud.reachable = true;
        sensor_water_sensor_properties->ezlopi_cloud.battery_powered = false;
        sensor_water_sensor_properties->ezlopi_cloud.show = true;
        sensor_water_sensor_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_water_sensor_properties->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
        sensor_water_sensor_properties->ezlopi_cloud.room_id = ezlopi_device_generate_room_id();
        sensor_water_sensor_properties->ezlopi_cloud.item_id = ezlopi_device_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_water_sensor_properties->interface.adc.gpio_num);
        // CJSON_GET_VALUE_INT(cjson_device, "resln_bit", sensor_water_sensor_properties->interface.adc.resln_bit);
        sensor_water_sensor_properties->interface.adc.resln_bit = 3;
    }

    return sensor_water_sensor_properties;
}

static int sensor_water_sensor_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (GPIO_IS_VALID_GPIO(properties->interface.adc.gpio_num))
    {
        ezlopi_adc_init(properties->interface.adc.gpio_num, properties->interface.adc.resln_bit);
        ret = 1;
    }
    return ret;
}

static int get_sensor_water_sensor_value_to_cloud(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)arg;
    if (cjson_propertise)
    {
        cJSON_AddStringToObject(cjson_propertise, "value", ezlopi_water_present_leak_state);
        ret = 1;
    }
    return ret;
}

static int sensor_water_sensor_get_value(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data);
    TRACE_B("Value is: %d, voltage is: %d", ezlopi_analog_data->value, ezlopi_analog_data->voltage);
    if (1000 <= ezlopi_analog_data->voltage)
    {
        ezlopi_water_present_leak_state = "water_leak_detected";
    }
    else
    {
        ezlopi_water_present_leak_state = "no_water_leak";
    }

    if (ezlopi_water_previous_leak_state != ezlopi_water_present_leak_state)
    {
        ezlopi_device_value_updated_from_device(properties);
        ezlopi_water_previous_leak_state = ezlopi_water_present_leak_state;
    }
    
    free(ezlopi_analog_data);
    return ret;
}
