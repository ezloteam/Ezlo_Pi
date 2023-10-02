
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

#include "sensor_0042_ADC_shunt_voltmeter.h"
//*************************************************************************
//                          Declaration
//*************************************************************************
static s_ezlopi_device_properties_t *sensor_0042_ADC_shunt_voltmeter_prepare(cJSON *cjson_device);
static int sensor_0042_adc_shunt_voltmeter_prep_and_add(void *arg);
static int sensor_0042_adc_shunt_voltmeter_init(s_ezlopi_device_properties_t *properties);
static int sensor_0042_adc_shunt_voltmeter_get_value(s_ezlopi_device_properties_t *properties, void *arg);
//*************************************************************************
//                          Defination
//*************************************************************************

//----------------------------------------------------------------------------------------------------------
int sensor_0042_ADC_shunt_voltmeter(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *user_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_0042_adc_shunt_voltmeter_prep_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_0042_adc_shunt_voltmeter_init(ezlopi_device);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_0042_adc_shunt_voltmeter_get_value(ezlopi_device, arg);
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
//----------------------------------------------------------------------------------------------------------
static int sensor_0042_adc_shunt_voltmeter_prep_and_add(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if ((device_prep_arg) && (device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_0042_voltmeter_properties = sensor_0042_ADC_shunt_voltmeter_prepare(device_prep_arg->cjson_device);
        if (sensor_0042_voltmeter_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, sensor_0042_voltmeter_properties, NULL))
            {
                // if doesnot exsist
                free(sensor_0042_voltmeter_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *sensor_0042_ADC_shunt_voltmeter_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_0042_adc_shunt_voltmeter_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if (sensor_0042_adc_shunt_voltmeter_properties)
    {
        memset(sensor_0042_adc_shunt_voltmeter_properties, 0, sizeof(s_ezlopi_device_properties_t));
        sensor_0042_adc_shunt_voltmeter_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_0042_adc_shunt_voltmeter_properties, device_name);
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.category = category_level_sensor;
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.subcategory = subcategory_electricity;
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.item_name = ezlopi_item_name_voltage;
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.device_type = dev_type_sensor;
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.value_type = value_type_electric_potential;
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.has_getter = true;
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.has_setter = false;
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.reachable = true;
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.battery_powered = false;
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.show = true;
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();
        sensor_0042_adc_shunt_voltmeter_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_0042_adc_shunt_voltmeter_properties->interface.adc.gpio_num);
        sensor_0042_adc_shunt_voltmeter_properties->interface.adc.resln_bit = 3;
    }
    return sensor_0042_adc_shunt_voltmeter_properties; // returns void if its not created
}

static int sensor_0042_adc_shunt_voltmeter_init(s_ezlopi_device_properties_t *properties)
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

static int sensor_0042_adc_shunt_voltmeter_get_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)arg;
    s_ezlopi_analog_data_t *sensor_0042_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(sensor_0042_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    char valueFormatted[20];
    if (cjson_propertise && sensor_0042_analog_data)
    {
        // extracting the analog value
        ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, sensor_0042_analog_data);
#ifdef VOLTAGE_DIVIDER_ADDED
        int voltage_data = ((int)(sensor_0042_analog_data->voltage) * 2) * 5; // first we double the incoming voltage
#else
        int voltage_data = (int)(sensor_0042_analog_data->voltage) * 5; // if you havenot added a voltage divider at sensor's analog output for esp32

#endif
        TRACE_B("Voltage : %d mV", voltage_data);
        snprintf(valueFormatted, 20, "%d", voltage_data);
        cJSON_AddNumberToObject(cjson_propertise, "value", voltage_data);
        cJSON_AddStringToObject(cjson_propertise, "valueFormatted", valueFormatted);
        cJSON_AddStringToObject(cjson_propertise, "scale", "millivolt");
        ret = 1;
    }
    free(sensor_0042_analog_data);
    return ret;
}

//--------------------------------------------------------------------------------------------------------------------------------------
