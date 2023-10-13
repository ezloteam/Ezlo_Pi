
#include "cJSON.h"
#include "ezlopi_cloud.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_valueformatter.h"
#include "trace.h"
#include "ezlopi_adc.h"

#include "sensor_0041_ADC_FC28_soilMoisture.h"
//*************************************************************************
//                          Declaration
//*************************************************************************

static s_ezlopi_device_properties_t *sensor_adc_FC28_prepare(cJSON *cjson_device); // you can directly add the prepare args here
static int sensor_adc_FC28_prepare_and_add(void *arg);
static int sensor_adc_FC28_init(s_ezlopi_device_properties_t *properties);
static int sensor_adc_FC28_get_value(s_ezlopi_device_properties_t *properties, void *arg);
//--------------------------------------------------------------------------------------------------------------------------------------
int sensor_0041_ADC_FC28_soilMoisture(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *user_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_adc_FC28_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_adc_FC28_init(ezlopi_device);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_adc_FC28_get_value(ezlopi_device, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        static uint8_t count = 0;
        if (count++ > 1)
        {
            ret = ezlopi_device_value_updated_from_device(ezlopi_device);
            count = 0;
        }
        break;
    }
    default:

        break;
    }
    return ret;
}

//--------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------

// funtion to generate the assosiated properties of the device_id
static s_ezlopi_device_properties_t *sensor_adc_FC28_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_0041_adc_FC28_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if (sensor_0041_adc_FC28_properties)
    {
        memset(sensor_0041_adc_FC28_properties, 0, sizeof(s_ezlopi_device_properties_t));
        // setting the interface of the device
        sensor_0041_adc_FC28_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

        // set the device name according to device_id
        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_0041_adc_FC28_properties, device_name);

        sensor_0041_adc_FC28_properties->ezlopi_cloud.category = category_humidity;
        sensor_0041_adc_FC28_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
        sensor_0041_adc_FC28_properties->ezlopi_cloud.item_name = ezlopi_item_name_moisture;
        sensor_0041_adc_FC28_properties->ezlopi_cloud.device_type = dev_type_sensor;
        sensor_0041_adc_FC28_properties->ezlopi_cloud.value_type = value_type_moisture;
        sensor_0041_adc_FC28_properties->ezlopi_cloud.has_getter = true;
        sensor_0041_adc_FC28_properties->ezlopi_cloud.has_setter = false;
        sensor_0041_adc_FC28_properties->ezlopi_cloud.reachable = true;
        sensor_0041_adc_FC28_properties->ezlopi_cloud.battery_powered = false;
        sensor_0041_adc_FC28_properties->ezlopi_cloud.show = true;
        sensor_0041_adc_FC28_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_0041_adc_FC28_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        sensor_0041_adc_FC28_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_0041_adc_FC28_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        sensor_0041_adc_FC28_properties->interface.adc.resln_bit = 3; // ADC 12-bit
        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_0041_adc_FC28_properties->interface.adc.gpio_num);
    }
    return sensor_0041_adc_FC28_properties;
}

static int sensor_adc_FC28_prepare_and_add(void *arg) // carries cJSON
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_args = (s_ezlopi_prep_arg_t *)arg;
    if ((NULL != device_prep_args) && (NULL != device_prep_args->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_adc_FC28_properties = sensor_adc_FC28_prepare(device_prep_args->cjson_device);
        if (sensor_adc_FC28_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_args->device, sensor_adc_FC28_properties, NULL))
            {
                free(sensor_adc_FC28_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}

static int sensor_adc_FC28_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (GPIO_IS_VALID_GPIO(properties->interface.adc.gpio_num))
    {
        ezlopi_adc_init(properties->interface.adc.gpio_num, properties->interface.adc.resln_bit);
        ret = 1;
    }
    return ret;
}

static int sensor_adc_FC28_get_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)arg;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    if (cjson_properties)
    {
        ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data);
        // NOTE : [ (0V-2.4V)  ==>  (0-4095) ]
        float adc_val = (ezlopi_analog_data->value); // The value maxes out the 2.4V

        float percent_data = (((float)(4095.0f - adc_val) / 4095.0f) * 100);
        TRACE_B("Percent moisture : %.2f", percent_data);

        // int volt_data = (int)(2400 - (ezlopi_analog_data->voltage)/2.0f); // max 2.4V
        // TRACE_B("voltage : %dmV", volt_data);
        cJSON_AddNumberToObject(cjson_properties, "value", percent_data);
        char *valueFormatted = ezlopi_valueformatter_float(percent_data);
        cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
        free(valueFormatted);
        cJSON_AddStringToObject(cjson_properties, "scale", "percent");
        ret = 1;
    }
    free(ezlopi_analog_data);
    return ret;
}

//--------------------------------------------------------------------------------------------------------------------------------------
