
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
#include "math.h"

#include "sensor_0048_ADC_MQ4_methane_gas_detector.h"
//*************************************************************************
//                          Declaration
//*************************************************************************
static float MQ4_R0_constant = 0;         // Define variable for MQ4_R0_constant [always constant]
static bool Calibration_complete = false; // flag to activate calibration phase

static s_ezlopi_device_properties_t *sensor_adc_mq4_prepare(cJSON *cjson_device); // you can directly add the prepare args here
static int sensor_adc_mq4_prepare_and_add(void *arg);

static int sensor_adc_MQ4_init(s_ezlopi_device_properties_t *properties);
static int sensor_adc_MQ4_get_value(s_ezlopi_device_properties_t *properties, void *arg);
//--------------------------------------------------------------------------------------------------------------------------------------
int sensor_0048_ADC_MQ4_methane(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *user_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_adc_mq4_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_adc_MQ4_init(ezlopi_device);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_adc_MQ4_get_value(ezlopi_device, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        if (Calibration_complete)
        {
            ret = ezlopi_device_value_updated_from_device(ezlopi_device);
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
static s_ezlopi_device_properties_t *sensor_adc_mq4_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_0048_adc_MQ4_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if (sensor_0048_adc_MQ4_properties)
    {
        memset(sensor_0048_adc_MQ4_properties, 0, sizeof(s_ezlopi_device_properties_t));
        // setting the interface of the device
        sensor_0048_adc_MQ4_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

        // set the device name according to device_id
        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_0048_adc_MQ4_properties, device_name);

        sensor_0048_adc_MQ4_properties->ezlopi_cloud.category = category_security_sensor;
        sensor_0048_adc_MQ4_properties->ezlopi_cloud.subcategory = subcategory_gas;
        sensor_0048_adc_MQ4_properties->ezlopi_cloud.item_name = ezlopi_item_name_gas_alarm;
        sensor_0048_adc_MQ4_properties->ezlopi_cloud.device_type = dev_type_sensor;
        sensor_0048_adc_MQ4_properties->ezlopi_cloud.value_type = value_type_token; // Alarm Sensor Events( Gas ) -->  'no_gas' , 'combustible_gas_detected' & 'toxic_gas_detected'
        sensor_0048_adc_MQ4_properties->ezlopi_cloud.has_getter = true;
        sensor_0048_adc_MQ4_properties->ezlopi_cloud.has_setter = false;
        sensor_0048_adc_MQ4_properties->ezlopi_cloud.reachable = true;
        sensor_0048_adc_MQ4_properties->ezlopi_cloud.battery_powered = false;
        sensor_0048_adc_MQ4_properties->ezlopi_cloud.show = true;
        sensor_0048_adc_MQ4_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_0048_adc_MQ4_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        sensor_0048_adc_MQ4_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_0048_adc_MQ4_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        sensor_0048_adc_MQ4_properties->interface.adc.resln_bit = 3; // ADC 12-bit
        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_0048_adc_MQ4_properties->interface.adc.gpio_num);
    }
    return sensor_0048_adc_MQ4_properties;
}

static int sensor_adc_mq4_prepare_and_add(void *arg) // carries cJSON
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_args = (s_ezlopi_prep_arg_t *)arg;
    if ((NULL != device_prep_args) && (NULL != device_prep_args->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_adc_mq4_properties = sensor_adc_mq4_prepare(device_prep_args->cjson_device);
        if (sensor_adc_mq4_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_args->device, sensor_adc_mq4_properties, NULL))
            {
                free(sensor_adc_mq4_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}

void Calibrate_MQ4_R0_resistance(void *params)
{
    float RS_calib = 0; // Define variable for sensor resistance
    s_ezlopi_device_properties_t *properties = (s_ezlopi_device_properties_t *)params;

    if (properties == NULL)
    {
        TRACE_E("CALIB_TASK -> properties arg NULL...................");
    }

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
                TRACE_W("Collecting Ambient Air data ........... [Avoid Smokes/gases]");
            }
            // extract ADC values
            ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data);
#ifdef voltage_divider_added
            _sensor_volt += (float)((ezlopi_analog_data->voltage) * 2); // [0-2.4V] X2
#else if
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
    RS_calib = ((MQ4_VOLT_RESOLUTION_Vc * eqv_RL) / (_sensor_volt / 1000.0f)) - eqv_RL; // Calculate RS in fresh air
    TRACE_E("CALIB_TASK -> 'RS_calib' = %.2f", RS_calib);
    if (RS_calib < 0)
    {
        RS_calib = 0; // No negative values accepted.
    }

    // Calculate the R0_air which is constant through-out
    MQ4_R0_constant = (RS_calib / RatioMQ4CleanAir); // Calculate MQ4_R0_constant
    TRACE_E("CALIB_TASK -> 'MQ4_R0_constant' = %.2f", MQ4_R0_constant);
    if (MQ4_R0_constant < 0)
    {
        MQ4_R0_constant = 0; // No negative values accepted.
    }

    // Set calibration_complete flag
    Calibration_complete = true;

    vTaskDelete(NULL);
}

static int sensor_adc_MQ4_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    // calibrate if not done
    if (!Calibration_complete)
    {
        if (GPIO_IS_VALID_GPIO(properties->interface.adc.gpio_num))
        {
            ezlopi_adc_init(properties->interface.adc.gpio_num, properties->interface.adc.resln_bit);

            // call a calibration task to determin 'Ro_air' value
            xTaskCreate(Calibrate_MQ4_R0_resistance, "Task_to_calculate_R0_air", 2048, (void *)properties, 1, NULL);

            ret = 1;
        }
    }
    return ret;
}

static int sensor_adc_MQ4_get_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)arg;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    if (cjson_properties)
    {
        //-------------------------------------------------
        // extract the mean_sensor_analog_output_voltage
        float analog_sensor_volt = 0;
        for (uint8_t x = 10; x > 0; x--)
        {
            ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data);
#ifdef voltage_divider_added
            analog_sensor_volt += ((float)(ezlopi_analog_data->voltage) * 2.0f);
#else if
            analog_sensor_volt += (float)(ezlopi_analog_data->voltage);
#endif
        }
        analog_sensor_volt = analog_sensor_volt / 10.0f;

        //-----------------------------------------------------------------------------------
        // Stage_2 : [from 'sensor_0048_ADC_MQ4_methane_gas_detector.h']

        // 1. Calculate 'Rs_gas' for the gas detected
        float Rs_gas = (((MQ4_VOLT_RESOLUTION_Vc * eqv_RL) / (analog_sensor_volt / 1000.0f)) - eqv_RL);

        // 1.1 Calculate @ 'ratio' during CH4 presence
        double _ratio = (Rs_gas / ((MQ4_R0_constant <= 0) ? (1.0f) : (MQ4_R0_constant))); // avoid dividing by zero??
        if (_ratio <= 0)
        {
            _ratio = 0;
        }
        //-------------------------------------------------

        // 1.2 Calculate _ppm
        float _ppm = (float)pow(10, (((float)log10(_ratio)) - b_coeff_mq4) / m_slope_mq4); // ---> _ppm = 10 ^ [ ( log(ratio) - b ) / m ]
        if (_ppm < 0)
        {
            _ppm = 0; // No negative values accepted or upper datasheet recomendation.
        }
        else
        {
            TRACE_E("_ppm [CH4] : %.2f -> Volts : %0.2fmv", _ppm, analog_sensor_volt);
        }
        //-------------------------------------------------
        // 2. Check optimal condition and generate properties
        if ((analog_sensor_volt > 800.0f) && _ppm > 2000.0f)
        {
            cJSON_AddStringToObject(cjson_properties, "value", "combustible_gas_detected");
        }
        else
        {
            cJSON_AddStringToObject(cjson_properties, "value", "no_gas");
        }
        //-----------------------------------------------------------------------------------------
        ret = 1;
    }
    free(ezlopi_analog_data);
    return ret;
}

//--------------------------------------------------------------------------------------------------------------------------------------
