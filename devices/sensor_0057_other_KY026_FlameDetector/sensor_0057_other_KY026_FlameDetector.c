
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

#include "sensor_0057_other_KY026_FlameDetector.h"

//------------------------------------------------------------------------------
#define ADD_PROPERTIES_DEVICE_LIST(device_id, category, subcategory, item_name, value_type, cjson_device)                       \
    {                                                                                                                           \
        s_ezlopi_device_properties_t *_properties = sensor_other_KY026_prepare_properties(device_id, category, subcategory,     \
                                                                                          item_name, value_type, cjson_device); \
        if (NULL != _properties)                                                                                                \
        {                                                                                                                       \
            add_device_to_list(device_prep_args, _properties, NULL);                                                            \
        }                                                                                                                       \
    }

//------------------------------------------------------------------------------

//*************************************************************************
//                          Declaration
//*************************************************************************
static uint8_t KY026_digital_pin = 0;
static uint8_t KY026_adc_pin = 0;

static int add_device_to_list(s_ezlopi_prep_arg_t *device_prep_args, s_ezlopi_device_properties_t *sensor_other_KY026_properties, void *user_arg);

static s_ezlopi_device_properties_t *sensor_other_KY026_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, const char *VALUE_TYPE, cJSON *cjson_device); // you can directly add the prepare args here
static int sensor_other_KY026_prepare_and_add(void *arg);
static int sensor_other_KY026_init(s_ezlopi_device_properties_t *properties);
static int sensor_other_KY026_get_value(s_ezlopi_device_properties_t *properties, void *arg);
static void Extract_KY026_sensor_value(float *analog_sensor_volt, s_ezlopi_device_properties_t *properties);
//--------------------------------------------------------------------------------------------------------------------------------------
int sensor_0057_other_KY026(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *user_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_other_KY026_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_other_KY026_init(ezlopi_device);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_other_KY026_get_value(ezlopi_device, arg);
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
static s_ezlopi_device_properties_t *sensor_other_KY026_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, const char *VALUE_TYPE, cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_0057_other_KY026_properties = NULL;
    if (NULL != cjson_device)
    {
        sensor_0057_other_KY026_properties = (s_ezlopi_device_properties_t *)malloc(sizeof(s_ezlopi_device_properties_t));
        if (sensor_0057_other_KY026_properties)
        {
            memset(sensor_0057_other_KY026_properties, 0, sizeof(s_ezlopi_device_properties_t));
            sensor_0057_other_KY026_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

            // set the device name according to device_id
            char *device_name = NULL;
            if (ezlopi_item_name_heat_alarm == ITEM_NAME)
            {
                device_name = "KY026-Fire-alert";
            }
            if (ezlopi_item_name_temperature_changes == ITEM_NAME)
            {
                device_name = "KY026-Fire-level";
            }
            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);

            ASSIGN_DEVICE_NAME(sensor_0057_other_KY026_properties, device_name);
            sensor_0057_other_KY026_properties->ezlopi_cloud.category = CATEGORY;
            sensor_0057_other_KY026_properties->ezlopi_cloud.subcategory = SUB_CATEGORY;
            sensor_0057_other_KY026_properties->ezlopi_cloud.item_name = ITEM_NAME;
            sensor_0057_other_KY026_properties->ezlopi_cloud.device_type = dev_type_sensor;
            sensor_0057_other_KY026_properties->ezlopi_cloud.value_type = VALUE_TYPE;
            sensor_0057_other_KY026_properties->ezlopi_cloud.has_getter = true;
            sensor_0057_other_KY026_properties->ezlopi_cloud.has_setter = false;
            sensor_0057_other_KY026_properties->ezlopi_cloud.reachable = true;
            sensor_0057_other_KY026_properties->ezlopi_cloud.battery_powered = false;
            sensor_0057_other_KY026_properties->ezlopi_cloud.show = true;
            sensor_0057_other_KY026_properties->ezlopi_cloud.room_name[0] = '\0';
            sensor_0057_other_KY026_properties->ezlopi_cloud.device_id = DEVICE_ID;
            sensor_0057_other_KY026_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            sensor_0057_other_KY026_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();
            if (ezlopi_item_name_heat_alarm == ITEM_NAME)
            {
                CJSON_GET_VALUE_INT(cjson_device, "gpio_digi", KY026_digital_pin);
                TRACE_I("KY026-> DIGITAL_PIN: %d ", KY026_digital_pin);
            }
            if (ezlopi_item_name_temperature_changes == ITEM_NAME)
            {
                CJSON_GET_VALUE_INT(cjson_device, "gpio_adc", KY026_adc_pin);
                TRACE_I("KY026-> ADC_PIN: %d ", KY026_adc_pin);
            }
            sensor_0057_other_KY026_properties->interface.adc.resln_bit = 3; // ADC 12-bit
        }
    }
    return sensor_0057_other_KY026_properties;
}

static int sensor_other_KY026_prepare_and_add(void *arg) // carries cJSON
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_args = (s_ezlopi_prep_arg_t *)arg;

    if ((NULL != device_prep_args) && (NULL != device_prep_args->cjson_device))
    {
        uint32_t device_id = 0;
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_security_sensor, subcategory_heat, ezlopi_item_name_heat_alarm, value_type_token, device_prep_args->cjson_device);
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_level_sensor, subcategory_not_defined, ezlopi_item_name_temperature_changes, value_type_general_purpose, device_prep_args->cjson_device);
    }
    return ret;
}
static int add_device_to_list(s_ezlopi_prep_arg_t *device_prep_args, s_ezlopi_device_properties_t *sensor_other_KY026_properties, void *user_arg)
{
    int ret = 0;
    if (sensor_other_KY026_properties)
    {
        if (0 == ezlopi_devices_list_add(device_prep_args->device, sensor_other_KY026_properties, NULL))
        {
            free(sensor_other_KY026_properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

static int sensor_other_KY026_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    static bool guard_digi = false;
    static bool guard_adc = false;

    if ((!guard_digi) && (0 != KY026_digital_pin) && GPIO_IS_VALID_GPIO(KY026_digital_pin))
    { // intialize digital_pin
        gpio_config_t input_conf = {};
        input_conf.pin_bit_mask = (1ULL << (KY026_digital_pin));
        input_conf.intr_type = GPIO_INTR_DISABLE;
        input_conf.mode = GPIO_MODE_INPUT;
        input_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
        input_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_config(&input_conf);

        guard_digi = true;
    }

    if ((!guard_adc) && (0 != KY026_adc_pin) && GPIO_IS_VALID_GPIO(KY026_adc_pin))
    {
        // initialize analog_pin
        ezlopi_adc_init(KY026_adc_pin, properties->interface.adc.resln_bit);
        guard_adc = true;
        ret = 1;
    }

    return ret;
}

static int sensor_other_KY026_get_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    float analog_sensor_volt = 0;
    // char valueFormatted[20];
    cJSON *cjson_properties = (cJSON *)arg;

    if (cjson_properties)
    {
        //-------------------------------------------------

        if (ezlopi_item_name_heat_alarm == properties->ezlopi_cloud.item_name)
        {
            if (0 == gpio_get_level(KY026_digital_pin)) // when D0 -> 0V,
            {
                cJSON_AddStringToObject(cjson_properties, "value", "combustible_gas_detected");
            }
            else
            {
                cJSON_AddStringToObject(cjson_properties, "value", "no_gas");
            }
        }
        if (ezlopi_item_name_temperature_changes == properties->ezlopi_cloud.item_name)
        {
            // extract the sensor_output_values
            Extract_KY026_sensor_value(&analog_sensor_volt, properties); // extract the (mean analog voltage * 2) [0-5V]
            int fire_percent = (analog_sensor_volt / 5.0f) * 100;
            // snprintf(valueFormatted, 20, "%.2f", analog_sensor_volt);
            // cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cjson_properties, "value", fire_percent);
            cJSON_AddStringToObject(cjson_properties, "scale", "percent");
        }
        //-----------------------------------------------------------------------------------------
        ret = 1;
    }

    return ret;
}

//--------------------------------------------------------------------------------------------------------------------------------------

static void Extract_KY026_sensor_value(float *analog_sensor_volt, s_ezlopi_device_properties_t *properties)
{
    // calculation process
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    if (ezlopi_analog_data)
    {
        memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
        //-------------------------------------------------
        // extract the mean_sensor_analog_output_voltage
        for (uint8_t x = 10; x > 0; x--)
        {
            ezlopi_adc_get_adc_data(KY026_adc_pin, ezlopi_analog_data);
#ifdef voltage_divider_added
            *analog_sensor_volt += ((float)(ezlopi_analog_data->voltage) * 2.0f);
#else
            *analog_sensor_volt += (float)(ezlopi_analog_data->voltage);
#endif
        }
        *analog_sensor_volt = *analog_sensor_volt / 10.0f;
        free(ezlopi_analog_data);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------
