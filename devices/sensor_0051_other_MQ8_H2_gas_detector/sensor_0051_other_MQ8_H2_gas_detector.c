
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
#include "math.h"

#include "sensor_0051_other_MQ8_H2_gas_detector.h"

const char *mq8_sensor_gas_alarm_token[] =
    {
        "no_gas",
        "combustible_gas_detected",
        "toxic_gas_detected",
        "unknown"};
//------------------------------------------------------------------------------
#define ADD_PROPERTIES_DEVICE_LIST(device_id, category, subcategory, item_name, value_type, cjson_device)                     \
    {                                                                                                                         \
        s_ezlopi_device_properties_t *_properties = sensor_other_mq8_prepare_properties(device_id, category, subcategory,     \
                                                                                        item_name, value_type, cjson_device); \
        if (NULL != _properties)                                                                                              \
        {                                                                                                                     \
            add_device_to_list(device_prep_args, _properties, NULL);                                                          \
        }                                                                                                                     \
    }

//------------------------------------------------------------------------------

//*************************************************************************
//                          Declaration
//*************************************************************************
static uint8_t mq8_digital_pin = 0;
static uint8_t mq8_adc_pin = 0;

static float MQ8_R0_constant = 0;         // Define variable for MQ8_R0_constant [always constant]
static bool Calibration_complete = false; // flag to activate calibration phase

static int add_device_to_list(s_ezlopi_prep_arg_t *device_prep_args, s_ezlopi_device_properties_t *sensor_other_mq8_properties, void *user_arg);

static s_ezlopi_device_properties_t *sensor_other_mq8_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, const char *VALUE_TYPE, cJSON *cjson_device); // you can directly add the prepare args here
static int sensor_other_MQ8_prepare_and_add(void *arg);
static int sensor_other_MQ8_init(s_ezlopi_device_properties_t *properties);
static void sensor_other_MQ8_get_item(s_ezlopi_device_properties_t *properties, void *arg);
static int sensor_other_MQ8_get_value(s_ezlopi_device_properties_t *properties, void *arg);
static void Extract_MQ8_sensor_ppm(float *analog_sensor_volt, float *_ppm, s_ezlopi_device_properties_t *properties);
//--------------------------------------------------------------------------------------------------------------------------------------
int sensor_0051_MQ8_H2(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *user_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_other_MQ8_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_other_MQ8_init(ezlopi_device);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        sensor_other_MQ8_get_item(ezlopi_device, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_other_MQ8_get_value(ezlopi_device, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        if (Calibration_complete)
        {
            ret = ezlopi_device_value_updated_from_device(ezlopi_device);
        }
        else
        {
            TRACE_E("----------------------- Calibrating ---------------------");
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
static s_ezlopi_device_properties_t *sensor_other_mq8_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, const char *VALUE_TYPE, cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_0051_other_MQ8_properties = NULL;
    if (NULL != cjson_device)
    {
        sensor_0051_other_MQ8_properties = (s_ezlopi_device_properties_t *)malloc(sizeof(s_ezlopi_device_properties_t));
        if (sensor_0051_other_MQ8_properties)
        {
            memset(sensor_0051_other_MQ8_properties, 0, sizeof(s_ezlopi_device_properties_t));
            sensor_0051_other_MQ8_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

            // set the device name according to device_id
            char *device_name = NULL;
            if (ezlopi_item_name_gas_alarm == ITEM_NAME)
            {
                device_name = "MQ8-Hydrogen-alert";
            }
            if (ezlopi_item_name_smoke_density == ITEM_NAME)
            {
                device_name = "MQ8-Hydrogen-level[ppm]";
            }
            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);

            ASSIGN_DEVICE_NAME(sensor_0051_other_MQ8_properties, device_name);
            sensor_0051_other_MQ8_properties->ezlopi_cloud.category = CATEGORY;
            sensor_0051_other_MQ8_properties->ezlopi_cloud.subcategory = SUB_CATEGORY;
            sensor_0051_other_MQ8_properties->ezlopi_cloud.item_name = ITEM_NAME;
            sensor_0051_other_MQ8_properties->ezlopi_cloud.device_type = dev_type_sensor;
            sensor_0051_other_MQ8_properties->ezlopi_cloud.value_type = VALUE_TYPE;
            sensor_0051_other_MQ8_properties->ezlopi_cloud.has_getter = true;
            sensor_0051_other_MQ8_properties->ezlopi_cloud.has_setter = false;
            sensor_0051_other_MQ8_properties->ezlopi_cloud.reachable = true;
            sensor_0051_other_MQ8_properties->ezlopi_cloud.battery_powered = false;
            sensor_0051_other_MQ8_properties->ezlopi_cloud.show = true;
            sensor_0051_other_MQ8_properties->ezlopi_cloud.room_name[0] = '\0';
            sensor_0051_other_MQ8_properties->ezlopi_cloud.device_id = DEVICE_ID;
            sensor_0051_other_MQ8_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            sensor_0051_other_MQ8_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();
            if (ezlopi_item_name_gas_alarm == ITEM_NAME)
            {
                CJSON_GET_VALUE_INT(cjson_device, "gpio1", mq8_digital_pin);
                TRACE_I("MQ8-> DIGITAL_PIN: %d ", mq8_digital_pin);
            }
            if (ezlopi_item_name_smoke_density == ITEM_NAME)
            {
                CJSON_GET_VALUE_INT(cjson_device, "gpio2", mq8_adc_pin);
                TRACE_I("MQ8-> ADC_PIN: %d ", mq8_adc_pin);
            }
            sensor_0051_other_MQ8_properties->interface.adc.resln_bit = 3; // ADC 12-bit
        }
    }
    return sensor_0051_other_MQ8_properties;
}

static int sensor_other_MQ8_prepare_and_add(void *arg) // carries cJSON
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_args = (s_ezlopi_prep_arg_t *)arg;

    if ((NULL != device_prep_args) && (NULL != device_prep_args->cjson_device))
    {
        uint32_t device_id = 0;
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_security_sensor, subcategory_gas, ezlopi_item_name_gas_alarm, value_type_token, device_prep_args->cjson_device);
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_level_sensor, subcategory_not_defined, ezlopi_item_name_smoke_density, value_type_substance_amount, device_prep_args->cjson_device);
    }
    return ret;
}
static int add_device_to_list(s_ezlopi_prep_arg_t *device_prep_args, s_ezlopi_device_properties_t *sensor_other_mq8_properties, void *user_arg)
{
    int ret = 0;
    if (sensor_other_mq8_properties)
    {
        if (0 == ezlopi_devices_list_add(device_prep_args->device, sensor_other_mq8_properties, NULL))
        {
            free(sensor_other_mq8_properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}
void Calibrate_MQ8_R0_resistance(void *params)
{
    float RS_calib = 0; // Define variable for sensor resistance
    s_ezlopi_device_properties_t *properties = (s_ezlopi_device_properties_t *)params;

    // if (properties == NULL)
    // {
    //     TRACE_E("CALIB_TASK -> properties arg NULL...................");
    // }

    //-------------------------------------------------
    // let the sensor to heat for 20seconds
    for (uint8_t j = 20; j > 0; j--)
    {
        TRACE_E("Heating sensor.........time left: %d sec", j);
        vTaskDelay(100); // 1sec delay before calibration
    }
    //-------------------------------------------------
    // extract the mean_sensor_analog_output_voltage
    float _sensor_volt = 0;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    if (ezlopi_analog_data)
    {
        memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
        for (uint8_t i = 200; i > 0; i--)
        {
            if (i % 20 == 0)
            {
                TRACE_W("Please Wait..Collecting Ambient Air data ........... [Avoid Smokes/gases]");
            }
            // extract ADC values
            ezlopi_adc_get_adc_data(mq8_adc_pin, ezlopi_analog_data);
#ifdef VOLTAGE_DIVIDER_ADDED
            _sensor_volt += (float)((ezlopi_analog_data->voltage) * 2); // [0-2.4V] X2
#else
            _sensor_volt += (float)(ezlopi_analog_data->voltage);
#endif
        }
        _sensor_volt = _sensor_volt / 200.0f;

        // free data pointer
        free(ezlopi_analog_data);
    }
    //-------------------------------------------------
    // Calculate the 'Rs' of heater during clean air [calibration phase]
    // Range -> [2Kohm - 20Kohm]
    RS_calib = ((MQ8_VOLT_RESOLUTION_Vc * mq8_eqv_RL) / (_sensor_volt / 1000.0f)) - mq8_eqv_RL; // Calculate RS in fresh air
    TRACE_E("CALIB_TASK -> 'RS_calib' = %.2f", RS_calib);
    if (RS_calib < 0)
    {
        RS_calib = 0; // No negative values accepted.
    }

    // Calculate the R0_air which is constant through-out
    MQ8_R0_constant = (RS_calib / RatioMQ8CleanAir); // Calculate MQ8_R0_constant
    TRACE_E("CALIB_TASK -> 'MQ8_R0_constant' = %.2f", MQ8_R0_constant);
    if (MQ8_R0_constant < 0)
    {
        MQ8_R0_constant = 0; // No negative values accepted.
    }

    // Set calibration_complete flag
    Calibration_complete = true;
    vTaskDelete(NULL);
}

static int sensor_other_MQ8_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    static bool guard_digi = false;
    static bool guard_adc = false;

    if ((!guard_digi) && (0 != mq8_digital_pin) && GPIO_IS_VALID_GPIO(mq8_digital_pin))
    { // intialize digital_pin
        gpio_config_t input_conf = {};
        input_conf.pin_bit_mask = (1ULL << (mq8_digital_pin));
        input_conf.intr_type = GPIO_INTR_DISABLE;
        input_conf.mode = GPIO_MODE_INPUT;
        input_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        input_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_config(&input_conf);

        guard_digi = true;
    }

    if ((!guard_adc) && (0 != mq8_adc_pin) && GPIO_IS_VALID_GPIO(mq8_adc_pin))
    {
        // initialize analog_pin
        ezlopi_adc_init(mq8_adc_pin, properties->interface.adc.resln_bit);
        guard_adc = true;

        // calibrate if not done
        TRACE_W(".........................................CALIB_GAURD : %d", Calibration_complete);
        if (!Calibration_complete)
        {
            xTaskCreate(Calibrate_MQ8_R0_resistance, "Task_to_calculate_R0_air", 2048, (void *)properties, 1, NULL);
        }
        ret = 1;
    }

    return ret;
}

static void Extract_MQ8_sensor_ppm(float *analog_sensor_volt, float *_ppm, s_ezlopi_device_properties_t *properties)
{
    // calculation process
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    //-------------------------------------------------
    // extract the mean_sensor_analog_output_voltage
    for (uint8_t x = 10; x > 0; x--)
    {
        ezlopi_adc_get_adc_data(mq8_adc_pin, ezlopi_analog_data);
#ifdef VOLTAGE_DIVIDER_ADDED
        *analog_sensor_volt += ((float)(ezlopi_analog_data->voltage) * 2.0f);
#else
        *analog_sensor_volt += (float)(ezlopi_analog_data->voltage);
#endif
    }
    *analog_sensor_volt = *analog_sensor_volt / 10.0f;

    //-----------------------------------------------------------------------------------
    // Stage_2 : [from 'sensor_0051_ADC_MQ8_H2_gas_detector.h']

    // 1. Calculate 'Rs_gas' for the gas detected
    float Rs_gas = (((MQ8_VOLT_RESOLUTION_Vc * mq8_eqv_RL) / (*analog_sensor_volt / 1000.0f)) - mq8_eqv_RL);

    // 1.1 Calculate @ 'ratio' during H2 presence
    double _ratio = (Rs_gas / ((MQ8_R0_constant <= 0) ? (1.0f) : (MQ8_R0_constant))); // avoid dividing by zero??
    if (_ratio <= 0)
    {
        _ratio = 0;
    }
    //-------------------------------------------------

    // 1.2 Calculate _ppm
    *_ppm = (float)pow(10, (((float)log10(_ratio)) - b_coeff_mq8) / m_slope_mq8); // ---> _ppm = 10 ^ [ ( log(ratio) - b ) / m ]
    if (*_ppm < 0)
    {
        *_ppm = 0; // No negative values accepted or upper datasheet recomendation.
    }
    else
    {
        TRACE_E("_ppm [H2] : %.2f -> ratio[RS/R0] : %.2f -> Volts : %0.2fmv", *_ppm, (float)_ratio, *analog_sensor_volt);
    }
    //-------------------------------------------------

    free(ezlopi_analog_data);
}

static void sensor_other_MQ8_get_item(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    float analog_sensor_volt = 0;
    float _ppm = 0;
    cJSON *cjson_properties = (cJSON *)arg;

    if (cjson_properties)
    {
        //-------------------------------------------------

        if (ezlopi_item_name_gas_alarm == properties->ezlopi_cloud.item_name)
        {
            cJSON *json_array_enum = cJSON_CreateArray();
            if (NULL != json_array_enum)
            {
                for (uint8_t i = 0; i < MQ8_GAS_ALARM_MAX; i++)
                {
                    cJSON *json_value = cJSON_CreateString(mq8_sensor_gas_alarm_token[i]);
                    if (NULL != json_value)
                    {
                        cJSON_AddItemToArray(json_array_enum, json_value);
                    }
                }
                cJSON_AddItemToObject(cjson_properties, "enum", json_array_enum);
            }

            if (0 == gpio_get_level(mq8_digital_pin)) // when D0 -> 0V,
            {
                cJSON_AddStringToObject(cjson_properties, "valueFormatted", "combustible_gas_detected");
                cJSON_AddStringToObject(cjson_properties, "value", "combustible_gas_detected");
            }
            else
            {
                cJSON_AddStringToObject(cjson_properties, "valueFormatted", "no_gas");
                cJSON_AddStringToObject(cjson_properties, "value", "no_gas");
            }
        }
        if (ezlopi_item_name_smoke_density == properties->ezlopi_cloud.item_name)
        {
            // extract the sensor_output_values
            Extract_MQ8_sensor_ppm(&analog_sensor_volt, &_ppm, properties);
            cJSON_AddNumberToObject(cjson_properties, "value", _ppm);
            char *valueFormatted = ezlopi_valueformatter_float(_ppm);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cjson_properties, "scale", "parts_per_million");
        }
        //-----------------------------------------------------------------------------------------
        ret = 1;
    }

    return ret;
}

static int sensor_other_MQ8_get_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    float analog_sensor_volt = 0;
    float _ppm = 0;
    cJSON *cjson_properties = (cJSON *)arg;

    if (cjson_properties)
    {
        //-------------------------------------------------

        if (ezlopi_item_name_gas_alarm == properties->ezlopi_cloud.item_name)
        {
            if (0 == gpio_get_level(mq8_digital_pin)) // when D0 -> 0V,
            {
                cJSON_AddStringToObject(cjson_properties, "valueFormatted", "combustible_gas_detected");
                cJSON_AddStringToObject(cjson_properties, "value", "combustible_gas_detected");
            }
            else
            {
                cJSON_AddStringToObject(cjson_properties, "valueFormatted", "no_gas");
                cJSON_AddStringToObject(cjson_properties, "value", "no_gas");
            }
        }
        if (ezlopi_item_name_smoke_density == properties->ezlopi_cloud.item_name)
        {
            // extract the sensor_output_values
            Extract_MQ8_sensor_ppm(&analog_sensor_volt, &_ppm, properties);
            cJSON_AddNumberToObject(cjson_properties, "value", _ppm);
            char *valueFormatted = ezlopi_valueformatter_float(_ppm);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cjson_properties, "scale", "parts_per_million");
        }
        //-----------------------------------------------------------------------------------------
        ret = 1;
    }

    return ret;
}

//--------------------------------------------------------------------------------------------------------------------------------------
