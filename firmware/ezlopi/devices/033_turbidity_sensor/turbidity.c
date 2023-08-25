#include "trace.h"
#include "ezlopi_device_value_updated.h"

#include "ezlopi_adc.h"
#include "turbidity.h"
#include "ezlopi_cloud_constants.h"

#include "cJSON.h"
#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"

#include "trace.h"
#include "ezlopi_adc.h"
#include "032_sens_soil_moisture.h"

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);

int turbidity_sensor_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __init(item);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        s_ezlopi_analog_data_t tmp_data = {.value = 0, .voltage = 0};
        ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &tmp_data);
        double percent_data = ((4095 - tmp_data.value) / 4095.0) * 100;
        cJSON_AddNumberToObject(cj_result, "value", percent_data);
        cJSON_AddStringToObject(cj_result, "scale", scales_percent);
        ret = 1;
    }
    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
    {
        ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit);
        ret = 1;
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_moisture;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __prepare_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_soil_humidity;
    item->cloud_properties.value_type = value_type_humidity;
    item->cloud_properties.scale = scales_percent;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;
    item->interface.adc.resln_bit = 3;
    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.adc.gpio_num);
}

static int __prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *device = ezlopi_device_add_device();
        if (device)
        {
            __prepare_device_cloud_properties(device, prep_arg->cjson_device);
            l_ezlopi_item_t *item_temperature = ezlopi_device_add_item_to_device(device, turbidity_sensor_v3);
            if (item_temperature)
            {
                __prepare_item_properties(item_temperature, prep_arg->cjson_device);
            }
        }
    }
    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    return ezlopi_device_value_updated_from_device_v3(item);
}

#if 1
static const char *ezlopi_water_present_turbidity_state = NULL;
int count = 0;
int turbidity_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = ezlopi_turbidity_sensor_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = ezlopi_turbidity_sensor_init(properties);
        break;
    }
    // case EZLOPI_ACTION_NOTIFY_200_MS:
    // case EZLOPI_ACTION_SET_VALUE:
    // {
    //     // TRACE_B("HEre");
    //     ret = ezlopi_turbidity_sensor_update_value(properties, arg);
    //     break;
    // }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = ezlopi_turbidity_sensor_get_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        count++;
        if (5 == count)
        {
            ret = ezlopi_turbidity_sensor_update_value(properties, arg);
            count = 0;
        }
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int ezlopi_turbidity_sensor_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *ezlopi_turbidity_sensor_properties = ezlopi_turbidity_sensor_prepare(device_prep_arg->cjson_device);
        if (ezlopi_turbidity_sensor_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, ezlopi_turbidity_sensor_properties, NULL))
            {
                free(ezlopi_turbidity_sensor_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static s_ezlopi_device_properties_t *ezlopi_turbidity_sensor_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *ezlopi_turbidity_sensor_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (ezlopi_turbidity_sensor_properties)
    {
        memset(ezlopi_turbidity_sensor_properties, 0, sizeof(s_ezlopi_device_properties_t));
        ezlopi_turbidity_sensor_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(ezlopi_turbidity_sensor_properties, device_name);
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.category = category_level_sensor;
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.subcategory = subcategory_water;
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.item_name = ezlopi_item_name_water_filter_replacement_alarm;
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.device_type = dev_type_sensor;
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.value_type = value_type_token;
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.has_getter = true;
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.has_setter = false;
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.reachable = true;
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.battery_powered = false;
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.show = true;
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.room_name[0] = '\0';
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        ezlopi_turbidity_sensor_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", ezlopi_turbidity_sensor_properties->interface.adc.gpio_num);
        ezlopi_turbidity_sensor_properties->interface.adc.resln_bit = 3;
    }

    return ezlopi_turbidity_sensor_properties;
}

static int ezlopi_turbidity_sensor_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (GPIO_IS_VALID_GPIO(properties->interface.adc.gpio_num))
    {
        ezlopi_adc_init(properties->interface.adc.gpio_num, properties->interface.adc.resln_bit);
        ret = 1;
    }
    return ret;
}

static int ezlopi_turbidity_sensor_update_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    static char *ezlopi_water_previous_turbidity_state;
    const static char *_no_ezlopi_turbidity = "water_filter_ok";
    const static char *_ezlopi_turbidity_detected = "replace_water_filter";
    s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};

    ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, &ezlopi_analog_data);
    TRACE_B("Value is: %d, voltage is: %d", ezlopi_analog_data.value, ezlopi_analog_data.voltage);

    if (1000 > ezlopi_analog_data.voltage)
    {
        ezlopi_water_present_turbidity_state = _ezlopi_turbidity_detected;
    }
    else
    {
        ezlopi_water_present_turbidity_state = _no_ezlopi_turbidity;
    }

    // if (ezlopi_water_previous_turbidity_state != ezlopi_water_present_turbidity_state)
    // {
    ezlopi_device_value_updated_from_device(properties);
    //     ezlopi_water_previous_turbidity_state = ezlopi_water_present_turbidity_state;
    // }

    return ret;
}

static int ezlopi_turbidity_sensor_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)arg;
    if (cjson_propertise)
    {
        cJSON_AddStringToObject(cjson_propertise, "value", ezlopi_water_present_turbidity_state);
        ret = 1;
    }
    return ret;
}
#endif
