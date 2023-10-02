#include "cJSON.h"
#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "trace.h"
#include "ezlopi_adc.h"
#include "math.h"

#include "sensor_0056_ADC_Force_Sensitive_Resistor.h"
//-----------------------------------------------------------------------------------------------

static int sensor_0056_force_sensitive_resistor_prepare_and_add(void *args);
static s_ezlopi_device_properties_t *sensor_0056_prepare(cJSON *cjson_device);
static int sensor_0056_force_sensitive_resistor_init(s_ezlopi_device_properties_t *properties);
static void sensor_0056_get_item(s_ezlopi_device_properties_t *properties, void *arg);
static int sensor_0056_get_value(s_ezlopi_device_properties_t *properties, void *args);
static float Calculate_GramForce(float Vout);
//-----------------------------------------------------------------------------------------------------------------------------
int sensor_0056_force_sensitive_resistor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_0056_force_sensitive_resistor_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_0056_force_sensitive_resistor_init(properties);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        sensor_0056_get_item(properties, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_0056_get_value(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        static uint8_t count;
        if (count > 1)
        {
            ret = ezlopi_device_value_updated_from_device(properties);
            count = 0;
        }
        count++;
        break;
    }
    default:
        break;
    }
    return ret;
}
//-----------------------------------------------------------------------------------------------
static int sensor_0056_force_sensitive_resistor_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_0056_force_sensitive_resistor_properties = sensor_0056_prepare(device_prep_arg->cjson_device);
        if (sensor_0056_force_sensitive_resistor_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, sensor_0056_force_sensitive_resistor_properties, NULL))
            {
                free(sensor_0056_force_sensitive_resistor_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *sensor_0056_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_0056_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (sensor_0056_properties)
    {
        memset(sensor_0056_properties, 0, sizeof(s_ezlopi_device_properties_t));
        sensor_0056_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_0056_properties, device_name);
        sensor_0056_properties->ezlopi_cloud.category = category_level_sensor;
        sensor_0056_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
        sensor_0056_properties->ezlopi_cloud.item_name = ezlopi_item_name_applied_force_on_sensor;
        sensor_0056_properties->ezlopi_cloud.device_type = dev_type_sensor;
        sensor_0056_properties->ezlopi_cloud.value_type = value_type_force;
        sensor_0056_properties->ezlopi_cloud.has_getter = true;
        sensor_0056_properties->ezlopi_cloud.has_setter = false;
        sensor_0056_properties->ezlopi_cloud.reachable = true;
        sensor_0056_properties->ezlopi_cloud.battery_powered = false;
        sensor_0056_properties->ezlopi_cloud.show = true;
        sensor_0056_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_0056_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        sensor_0056_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_0056_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_0056_properties->interface.adc.gpio_num);
        sensor_0056_properties->interface.adc.resln_bit = 3;
    }

    return sensor_0056_properties;
}

static int sensor_0056_force_sensitive_resistor_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (GPIO_IS_VALID_GPIO(properties->interface.adc.gpio_num))
    {
        ezlopi_adc_init(properties->interface.adc.gpio_num, properties->interface.adc.resln_bit);
        ret = 1;
    }
    return ret;
}

static void sensor_0056_get_item(s_ezlopi_device_properties_t *properties, void *arg)
{
    float Vout = 0, gramForce = 0;
    char valueFormatted[20];
    cJSON *cjson_properties = (cJSON *)arg;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    if (cjson_properties)
    {
        if (ezlopi_item_name_applied_force_on_sensor == properties->ezlopi_cloud.item_name)
        {
            ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data);
            Vout = (ezlopi_analog_data->voltage) / 1000.0f; // millivolt -> voltage

            // Calculate_GramForce
            gramForce = Calculate_GramForce(Vout);
            // actual Force[N] is :
            float force = 0.0098 * gramForce;
            TRACE_E("GramForce[gN]: %.4f  => Force[N]: %.4f", gramForce, force);

            // prepare the json message
            snprintf(valueFormatted, 20, "%.2f", force);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cjson_properties, "value", force);
            cJSON_AddStringToObject(cjson_properties, "scale", "newton");
        }
    }
    free(ezlopi_analog_data);
}

static int sensor_0056_get_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    float Vout = 0, gramForce = 0;
    char valueFormatted[20];
    cJSON *cjson_properties = (cJSON *)arg;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    if (cjson_properties)
    {
        if (ezlopi_item_name_applied_force_on_sensor == properties->ezlopi_cloud.item_name)
        {
            ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data);
            Vout = (ezlopi_analog_data->voltage) / 1000.0f; // millivolt -> voltage

            // Calculate_GramForce
            gramForce = Calculate_GramForce(Vout);
            // actual Force[N] is :
            float force = 0.0098 * gramForce;
            TRACE_E("GramForce[gN]: %.4f  => Force[N]: %.4f", gramForce, force);

            // prepare the json message
            snprintf(valueFormatted, 20, "%.2f", force);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cjson_properties, "value", force);
            cJSON_AddStringToObject(cjson_properties, "scale", "newton");
        }
        ret = 1;
    }
    free(ezlopi_analog_data);
    return ret;
}
//-------------------------------------------------------------------------------------------
static float Calculate_GramForce(float Vout)
{
    float Rs = 0, gramForce = 0;
    // TRACE_E("Voltage [V]: %.4f", Vout);
    if ((Vout - 0.142f) > 0.1f)
    {
        // calculate the 'Rs[kOhm]' corresponding to 'Vout' using equation(A):
        Rs = (((FSR_Vin / Vout) - 1) * FSR_Rout) / (1000.0f); // Ohm -> KOhm
    }
    else
    {
        Rs = 750; // kOhm
    }
    TRACE_E("FSR value [Rs Kohm]: %.4f", Rs);

    // calculating the Force(g)
    if (Rs < 250)
    {
        // We choose Rs threshold below 250KOhm because:
        // The senor activation causes, imidiate resistance drop below 200KOhm
        // So, filter the 'Rs' range(250k to 2K)
        Rs = (Rs < 2) ? 2 : Rs;
        gramForce = (float)pow(10, (((float)log10(Rs)) - b_coeff_FSR) / m_slope_FSR);
    }
    else
    { //
        gramForce = 0;
    }
    // according to testing ; correct gramForce(G) is :
    gramForce = gramForce * FSR_correction_factor;

    return gramForce;
}