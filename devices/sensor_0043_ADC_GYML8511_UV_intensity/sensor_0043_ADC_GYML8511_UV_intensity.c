#include "trace.h"
#include "cJSON.h"
#include "ezlopi_adc.h"
#include "ezlopi_cloud.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_valueformatter.h"

#include "sensor_0043_ADC_GYML8511_UV_intensity.h"

//********************************************************************
//                          Declarations
//********************************************************************
static int sensor_0043_adc_uv_prep_and_add(void *arg);
static s_ezlopi_device_properties_t *sensor_0043_adc_uv_prepare(cJSON *cjson_device);
static int sensor_0043_adc_uv_init(s_ezlopi_device_properties_t *properties);
static int sensor_0043_adc_uv_get_value(s_ezlopi_device_properties_t *properties, void *arg);
static float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);
//---------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------
int sensor_0043_adc_gyml8511_uv_intensity(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *usr_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_0043_adc_uv_prep_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_0043_adc_uv_init(ezlopi_device);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_0043_adc_uv_get_value(ezlopi_device, arg);
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
    {
        break;
    }
    }
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------

static int sensor_0043_adc_uv_prep_and_add(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        // extract properpties from cjson
        s_ezlopi_device_properties_t *sensor_0043_adc_uv_properties = sensor_0043_adc_uv_prepare(device_prep_arg->cjson_device);
        if (sensor_0043_adc_uv_properties)
        { // add the deivce properties
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, sensor_0043_adc_uv_properties, NULL))
            {
                // if doesn't exists free the properties pointer
                free(sensor_0043_adc_uv_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *sensor_0043_adc_uv_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_0043_adc_uv_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if (sensor_0043_adc_uv_properties)
    {
        memset(sensor_0043_adc_uv_properties, 0, sizeof(s_ezlopi_device_t));
        sensor_0043_adc_uv_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);

        ASSIGN_DEVICE_NAME(sensor_0043_adc_uv_properties, device_name)
        // ezlopi_cloud...
        sensor_0043_adc_uv_properties->ezlopi_cloud.category = category_level_sensor;
        sensor_0043_adc_uv_properties->ezlopi_cloud.category = category_level_sensor;
        sensor_0043_adc_uv_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
        sensor_0043_adc_uv_properties->ezlopi_cloud.item_name = ezlopi_item_name_ultraviolet;
        sensor_0043_adc_uv_properties->ezlopi_cloud.device_type = dev_type_sensor;
        sensor_0043_adc_uv_properties->ezlopi_cloud.value_type = ezlopi_item_name_ultraviolet;
        sensor_0043_adc_uv_properties->ezlopi_cloud.battery_powered = false;
        sensor_0043_adc_uv_properties->ezlopi_cloud.has_getter = true;
        sensor_0043_adc_uv_properties->ezlopi_cloud.has_setter = false;
        sensor_0043_adc_uv_properties->ezlopi_cloud.reachable = true;
        sensor_0043_adc_uv_properties->ezlopi_cloud.show = true;
        sensor_0043_adc_uv_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_0043_adc_uv_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        sensor_0043_adc_uv_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_0043_adc_uv_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_0043_adc_uv_properties->interface.adc.gpio_num);
        sensor_0043_adc_uv_properties->interface.adc.resln_bit = 3;
    }
    return sensor_0043_adc_uv_properties;
}

static int sensor_0043_adc_uv_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (properties)
    {
        if (GPIO_IS_VALID_GPIO(properties->interface.adc.gpio_num))
        {
            ezlopi_adc_init(properties->interface.adc.gpio_num, properties->interface.adc.resln_bit);
        }
        ret = 1;
    }
    return ret;
}
static float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * ((out_max - out_min) / (in_max - in_min)) + out_min;
}

static int sensor_0043_adc_uv_get_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)arg;
    s_ezlopi_analog_data_t *sensor_0043_adc_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(sensor_0043_adc_data, 0, sizeof(s_ezlopi_analog_data_t));
    if ((NULL != cjson_properties) && (NULL != sensor_0043_adc_data))
    {
        ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, sensor_0043_adc_data);
        // generating UV mW/cm2
        float uvIntensity = mapfloat(((float)(sensor_0043_adc_data->voltage) / 1000.0f), 0.97, 2.7, 0.0, 15.0);
        TRACE_B("[200-380nm]UV  : %.2f mW/cm^2 ", uvIntensity);
        cJSON_AddNumberToObject(cjson_properties, "value", uvIntensity);
        char *valueFormatted = ezlopi_valueformatter_float(uvIntensity);
        cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
        free(valueFormatted);
        cJSON_AddStringToObject(cjson_properties, "scale", "lux");

        // corresponding voltage
        int analog_data = (sensor_0043_adc_data->voltage);
        TRACE_B("Analog Output voltage : %d mV", analog_data);
        ret = 1;
    }
    free(sensor_0043_adc_data);
    return ret;
}