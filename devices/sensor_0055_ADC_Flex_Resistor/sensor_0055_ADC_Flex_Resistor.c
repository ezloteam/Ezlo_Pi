#include "cJSON.h"
#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_valueformatter.h"
#include "trace.h"
#include "ezlopi_adc.h"

#include "sensor_0055_ADC_Flex_Resistor.h"
//-----------------------------------------------------------------------------------------------

static int sensor_0055_flex_resistor_prepare_and_add(void *args);
static s_ezlopi_device_properties_t *sensor_0055_prepare(cJSON *cjson_device);
static int sensor_0055_flex_resistor_init(s_ezlopi_device_properties_t *properties);
// static void sensor_0055_get_item(s_ezlopi_device_properties_t *properties, void *arg);
static int sensor_0055_get_value(s_ezlopi_device_properties_t *properties, void *args);
//-----------------------------------------------------------------------------------------------------------------------------
int sensor_0055_flex_resistor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_0055_flex_resistor_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_0055_flex_resistor_init(properties);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    // {
    //     sensor_0055_get_item(properties, arg);
    //     break;
    // }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_0055_get_value(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = ezlopi_device_value_updated_from_device(properties);
        break;
    }
    default:
        break;
    }
    return ret;
}
//-----------------------------------------------------------------------------------------------
static int sensor_0055_flex_resistor_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_0055_flex_resistor_properties = sensor_0055_prepare(device_prep_arg->cjson_device);
        if (sensor_0055_flex_resistor_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, sensor_0055_flex_resistor_properties, NULL))
            {
                free(sensor_0055_flex_resistor_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *sensor_0055_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_0055_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (sensor_0055_properties)
    {
        memset(sensor_0055_properties, 0, sizeof(s_ezlopi_device_properties_t));
        sensor_0055_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_0055_properties, device_name);
        sensor_0055_properties->ezlopi_cloud.category = category_level_sensor;
        sensor_0055_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
        sensor_0055_properties->ezlopi_cloud.item_name = ezlopi_item_name_electrical_resistivity;
        sensor_0055_properties->ezlopi_cloud.device_type = dev_type_sensor;
        sensor_0055_properties->ezlopi_cloud.value_type = value_type_electrical_resistance;
        sensor_0055_properties->ezlopi_cloud.has_getter = true;
        sensor_0055_properties->ezlopi_cloud.has_setter = false;
        sensor_0055_properties->ezlopi_cloud.reachable = true;
        sensor_0055_properties->ezlopi_cloud.battery_powered = false;
        sensor_0055_properties->ezlopi_cloud.show = true;
        sensor_0055_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_0055_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        sensor_0055_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_0055_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_0055_properties->interface.adc.gpio_num);
        sensor_0055_properties->interface.adc.resln_bit = 3;
    }

    return sensor_0055_properties;
}

static int sensor_0055_flex_resistor_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (GPIO_IS_VALID_GPIO(properties->interface.adc.gpio_num))
    {
        ezlopi_adc_init(properties->interface.adc.gpio_num, properties->interface.adc.resln_bit);
        ret = 1;
    }
    return ret;
}
#if 0
static void sensor_0055_get_item(s_ezlopi_device_properties_t *properties, void *arg)
{
    cJSON *cjson_propertise = (cJSON *)arg;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    if (cjson_propertise)
    {
        if (ezlopi_item_name_electrical_resistivity == properties->ezlopi_cloud.item_name)
        {
            ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data);
            float Vout = (ezlopi_analog_data->voltage) / 1000.0f; // millivolt -> voltage
            // TRACE_E("Voltage [mV]: %.4f", Vout);

            // calculate the 'Rs' resistance value using [voltage divider rule]
            int Rs = (int)(((flex_Vin / Vout) - 1) * flex_Rout);

            // prepare the json message
            cJSON_AddNumberToObject(cjson_propertise, "value", Rs);
            cJSON_AddStringToObject(cjson_propertise, "scale", "ohm_meter");
        }
    }
    free(ezlopi_analog_data);
}
#endif
static int sensor_0055_get_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)arg;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    if (cjson_propertise)
    {
        if (ezlopi_item_name_electrical_resistivity == properties->ezlopi_cloud.item_name)
        {
            ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data);
            float Vout = (ezlopi_analog_data->voltage) / 1000.0f; // millivolt -> voltage
            // TRACE_E("Voltage [mV]: %.4f", Vout);

            // calculate the 'Rs' resistance value using [voltage divider rule]
            int Rs = (int)(((flex_Vin / Vout) - 1) * flex_Rout);

            // prepare the json message
            cJSON_AddNumberToObject(cjson_propertise, "value", Rs);
            char *valueFormatted = ezlopi_valueformatter_int(Rs);
            cJSON_AddStringToObject(cjson_propertise, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cjson_propertise, "scale", "ohm_meter");
            ret = 1;
        }
    }
    free(ezlopi_analog_data);
    return ret;
}
