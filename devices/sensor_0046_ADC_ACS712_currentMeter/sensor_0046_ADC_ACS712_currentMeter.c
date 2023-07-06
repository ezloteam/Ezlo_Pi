
#include "cJSON.h"
#include "ezlopi_cloud.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "trace.h"
#include "ezlopi_adc.h"

#include "sensor_0046_ADC_ACS712_currentMeter.h"
//*************************************************************************
//                          Declaration
//*************************************************************************

static s_ezlopi_device_properties_t *sensor_adc_ACS712_prepare(cJSON *cjson_device); // you can directly add the prepare args here
static int sensor_adc_ACS712_prepare_and_add(void *arg);

static int sensor_adc_ACS712_init(s_ezlopi_device_properties_t *properties);
static int sensor_adc_ACS712_get_value(s_ezlopi_device_properties_t *properties, void *arg);
//--------------------------------------------------------------------------------------------------------------------------------------
int sensor_0046_ADC_ACS712_currentMeter(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *user_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_adc_ACS712_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_adc_ACS712_init(ezlopi_device);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_adc_ACS712_get_value(ezlopi_device, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = ezlopi_device_value_updated_from_device(ezlopi_device);
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
static s_ezlopi_device_properties_t *sensor_adc_ACS712_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_0046_ADC_ACS712_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if (sensor_0046_ADC_ACS712_properties)
    {
        memset(sensor_0046_ADC_ACS712_properties, 0, sizeof(s_ezlopi_device_properties_t));
        // setting the interface of the device
        sensor_0046_ADC_ACS712_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

        // set the device name according to device_id
        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_0046_ADC_ACS712_properties, device_name);

        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.category = category_generic_sensor;
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.item_name = ezlopi_item_name_current;
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.device_type = dev_type_sensor;
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.value_type = value_type_electric_current;
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.has_getter = true;
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.has_setter = false;
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.reachable = true;
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.battery_powered = false;
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.show = true;
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id;
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id;

        sensor_0046_ADC_ACS712_properties->interface.adc.resln_bit = 3; // ADC 12-bit
        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_0046_ADC_ACS712_properties->interface.adc.gpio_num);
    }
    return sensor_0046_ADC_ACS712_properties;
}

static int sensor_adc_ACS712_prepare_and_add(void *arg) // carries cJSON
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_args = (s_ezlopi_prep_arg_t *)arg;
    if ((NULL != device_prep_args) && (NULL != device_prep_args->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_ADC_ACS712_properties = sensor_adc_ACS712_prepare(device_prep_args->cjson_device);
        if (sensor_ADC_ACS712_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_args->device, sensor_ADC_ACS712_properties, NULL))
            {
                free(sensor_ADC_ACS712_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}

static int sensor_adc_ACS712_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (GPIO_IS_VALID_GPIO(properties->interface.adc.gpio_num))
    {
        ezlopi_adc_init(properties->interface.adc.gpio_num, properties->interface.adc.resln_bit);
        ret = 1;
    }
    return ret;
}

static int sensor_adc_ACS712_get_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)arg;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    if (cjson_properties)
    {
        ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data);
        float Amp_data = (((float)(ezlopi_analog_data->voltage) - 2350.0f) / 100.0f); // ( current = mv / [100mV/A] )
        TRACE_B("Current : %.2f A", Amp_data);
        cJSON_AddNumberToObject(cjson_properties, "value", (int)Amp_data);
        cJSON_AddStringToObject(cjson_properties, "scale", "Ampere");
        ret = 1;
    }
    free(ezlopi_analog_data);
    return ret;
}

//--------------------------------------------------------------------------------------------------------------------------------------
