#include "trace.h"
#include "cJSON.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"
#include "math.h"
#include "stdbool.h"
#include "string.h"

#include "ezlopi_adc.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_scales_str.h"
#include "0062_sensor_other_MQ7_CO_detector.h"

//*************************************************************************
//                          Declaration
//*************************************************************************

static float MQ7_R0_constant = 0;            // Define variable for MQ7_R0_constant [always constant]
static bool Calibration_complete_CO = false; // flag to activate calibration phase
const char *mq7_sensor_gas_alarm_token[] =
    {
        "no_gas",
        "combustible_gas_detected",
        "toxic_gas_detected",
        "unknown"};
//--------------------------------------------------------------------------------------------------------
static int __0062_prepare(void *arg);
static int __0062_init(l_ezlopi_item_t *item);
static int __0062_get_item(l_ezlopi_item_t *item, void *arg);
static int __0062_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __0062_notify(l_ezlopi_item_t *item);
static float Extract_MQ7_sensor_ppm(uint32_t mq7_adc_pin);
void Calibrate_MQ7_R0_resistance(void *params);
static void __prepare_device_digi_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_digi_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __prepare_device_adc_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_adc_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
//--------------------------------------------------------------------------------------------------------

int sensor_MQ7_CO_detector_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0062_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0062_init(item);
        break;
    }
    // case EZLOPI_ACTION_HUB_GET_ITEM:
    // {
    //     __0062_get_item(item, arg);
    //     break;
    // }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0062_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        if (Calibration_complete_CO)
        {
            __0062_notify(item);
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

//----------------------------------------------------
static int __0062_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        //---------------------------  DIGI - DEVICE 1 --------------------------------------------
        l_ezlopi_device_t *MQ7_device_digi = ezlopi_device_add_device();
        if (MQ7_device_digi)
        {
            __prepare_device_digi_cloud_properties(MQ7_device_digi, device_prep_arg->cjson_device);
            l_ezlopi_item_t *MQ7_item_digi = ezlopi_device_add_item_to_device(MQ7_device_digi, sensor_MQ7_CO_detector_v3);
            if (MQ7_item_digi)
            {
                __prepare_item_digi_cloud_properties(MQ7_item_digi, device_prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(MQ7_device_digi);
            }
        }
        else
        {
            ezlopi_device_free_device(MQ7_device_digi);
        }

        //---------------------------- ADC - DEVICE 2 -------------------------------------------

        l_ezlopi_device_t *MQ7_device_adc = ezlopi_device_add_device();
        if (MQ7_device_adc)
        {
            __prepare_device_adc_cloud_properties(MQ7_device_adc, device_prep_arg->cjson_device);
            l_ezlopi_item_t *MQ7_item_adc = ezlopi_device_add_item_to_device(MQ7_device_adc, sensor_MQ7_CO_detector_v3);
            if (MQ7_item_adc)
            {
                __prepare_item_adc_cloud_properties(MQ7_item_adc, device_prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(MQ7_device_adc);
            }
        }
        else
        {
            ezlopi_device_free_device(MQ7_device_adc);
        }
        ret = 1;
    }
    return ret;
}

static int __0062_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (NULL != item)
    {
        if ((ezlopi_item_name_gas_alarm == item->cloud_properties.item_name) && GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
        {
            // intialize digital_pin
            gpio_config_t input_conf = {};
            input_conf.pin_bit_mask = (1ULL << (item->interface.gpio.gpio_in.gpio_num));
            input_conf.intr_type = GPIO_INTR_DISABLE;
            input_conf.mode = GPIO_MODE_INPUT;
            input_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
            input_conf.pull_up_en = GPIO_PULLUP_ENABLE;
            gpio_config(&input_conf);
            ret = 1;
        }
        if ((ezlopi_item_name_smoke_density == item->cloud_properties.item_name) && GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
        {
            // initialize analog_pin
            ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit);
            // calibrate if not done
            if (!Calibration_complete_CO)
            {
                xTaskCreate(Calibrate_MQ7_R0_resistance, "Task_to_calculate_R0_air", 2048, &(item->interface.adc.gpio_num), 1, NULL);
            }
            ret = 2;
        }
    }
    return ret;
}

//------------------------------------------------------------------------------------------------------
static void __prepare_device_digi_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    char *_addition = " CO-alert";
    device_name = strncat(device_name, _addition, strlen(_addition)+1);
    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_gas;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}
static void __prepare_item_digi_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_gas_alarm;
    item->cloud_properties.value_type = value_type_token;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type); // _max = 10
    CJSON_GET_VALUE_INT(cj_device, "gpio1", item->interface.gpio.gpio_in.gpio_num);
    TRACE_I("MQ7-> DIGITAL_PIN: %d ", item->interface.gpio.gpio_in.gpio_num);
    // CJSON_GET_VALUE_INT(cj_device, "en_gpio1", item->interface.gpio.gpio_in.enable); // we can enable/disable DIGI_GPIO
}
//------------------------------------------------------------------------------------------------------
static void __prepare_device_adc_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    char *_addition = " CO-level [PPM]";
    device_name = strncat(device_name, _addition, strlen(_addition)+1);
    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}
static void __prepare_item_adc_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{

    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_smoke_density;
    item->cloud_properties.value_type = value_type_substance_amount;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_parts_per_million;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type); // _max = 10
    CJSON_GET_VALUE_INT(cj_device, "gpio2", item->interface.adc.gpio_num);
    TRACE_I("MQ7-> ADC_PIN: %d ", item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit
}

//------------------------------------------------------------------------------------------------------
static int __0062_get_item(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            if (ezlopi_item_name_gas_alarm == item->cloud_properties.item_name)
            {
                //-------------------  POSSIBLE JSON ENUM CONTENTS ----------------------------------
                cJSON *json_array_enum = cJSON_CreateArray();
                if (NULL != json_array_enum)
                {
                    for (uint8_t i = 0; i < MQ7_GAS_ALARM_MAX; i++)
                    {
                        cJSON *json_value = cJSON_CreateString(mq7_sensor_gas_alarm_token[i]);
                        if (NULL != json_value)
                        {
                            cJSON_AddItemToArray(json_array_enum, json_value);
                        }
                    }
                    cJSON_AddItemToObject(cj_result, "enum", json_array_enum);
                }
                //--------------------------------------------------------------------------------------
                cJSON_AddStringToObject(cj_result, "value", (char *)item->user_arg ? item->user_arg : "no_gas");
            }
            if (ezlopi_item_name_smoke_density == item->cloud_properties.item_name)
            {
                char valueFormatted[20];
                snprintf(valueFormatted, 20, "%.2f", *((float *)item->user_arg));
                cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
                cJSON_AddNumberToObject(cj_result, "value", *((float *)item->user_arg));
            }
            ret = 1;
        }
    }
    return ret;
}

static int __0062_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            if (ezlopi_item_name_gas_alarm == item->cloud_properties.item_name)
            {
                cJSON_AddStringToObject(cj_result, "value", (char *)item->user_arg ? item->user_arg : "no_gas");
            }
            if (ezlopi_item_name_smoke_density == item->cloud_properties.item_name)
            {
                char valueFormatted[20];
                snprintf(valueFormatted, 20, "%.2f", *((float *)item->user_arg));
                cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
                cJSON_AddNumberToObject(cj_result, "value", *((float *)item->user_arg));
            }
            ret = 1;
        }
    }
    return ret;
}

static int __0062_notify(l_ezlopi_item_t *item)
{
    static float _CO_ppm = 0;
    int ret = 0;
    if (item)
    {
        if (ezlopi_item_name_gas_alarm == item->cloud_properties.item_name)
        {
            char *curret_value = NULL;
            if (0 == gpio_get_level(item->interface.gpio.gpio_in.gpio_num)) // when D0 -> 0V,
            {
                curret_value = "combustible_gas_detected";
            }
            else
            {
                curret_value = "no_gas";
            }
            if (curret_value != (char *)item->user_arg) // calls update only if there is change in state
            {
                item->user_arg = (void *)curret_value;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        if (ezlopi_item_name_smoke_density == item->cloud_properties.item_name)
        {
            // extract the sensor_output_values
            float prev_ppm = _CO_ppm;
            _CO_ppm = Extract_MQ7_sensor_ppm(item->interface.adc.gpio_num);
            if (prev_ppm != _CO_ppm)
            {
                item->user_arg = ((void *)(&_CO_ppm));
                ezlopi_device_value_updated_from_device_v3(item);
                item->user_arg = NULL;
            }
        }
        ret = 1;
    }
    return ret;
}
//------------------------------------------------------------------------------------------------------
static float Extract_MQ7_sensor_ppm(uint32_t mq7_adc_pin)
{
    // calculation process
    //-------------------------------------------------
    s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};
    // extract the mean_sensor_analog_output_voltage
    float analog_sensor_volt = 0;
    for (uint8_t x = 10; x > 0; x--)
    {
        ezlopi_adc_get_adc_data(mq7_adc_pin, &ezlopi_analog_data);
#ifdef VOLTAGE_DIVIDER_ADDED
        analog_sensor_volt += ((float)(ezlopi_analog_data.voltage) * 2.0f);
#else
        analog_sensor_volt += (float)(ezlopi_analog_data.voltage);
#endif
        vTaskDelay(1);
    }
    analog_sensor_volt = analog_sensor_volt / 10.0f;

    //-----------------------------------------------------------------------------------
    // Stage_2 : [from 'sensor_0062_ADC_MQ7_methane_gas_detector.h']

    // 1. Calculate 'Rs_gas' for the gas detected
    float Rs_gas = (((MQ7_VOLT_RESOLUTION_Vc * mq7_eqv_RL) / (analog_sensor_volt / 1000.0f)) - mq7_eqv_RL);

    // 1.1 Calculate @ 'ratio' during CO presence
    double _ratio = (Rs_gas / ((MQ7_R0_constant <= 0) ? (1.0f) : (MQ7_R0_constant))); // avoid dividing by zero??
    if (_ratio <= 0)
    {
        _ratio = 0;
    }
    //-------------------------------------------------

    // 1.2 Calculate _CO_ppm
    float _CO_ppm = (float)pow(10, (((float)log10(_ratio)) - b_coeff_mq7) / m_slope_mq7); // ---> _CO_ppm = 10 ^ [ ( log(ratio) - b ) / m ]
    if (_CO_ppm < 0)
    {
        _CO_ppm = 0; // No negative values accepted or upper datasheet recomendation.
    }
    TRACE_E("_CO_ppm [CO] : %.2f -> ratio[RS/R0] : %.2f -> Volts : %0.2fmv", _CO_ppm, (float)_ratio, analog_sensor_volt);

    //-------------------------------------------------
    return _CO_ppm;
}

void Calibrate_MQ7_R0_resistance(void *params)
{
    uint32_t mq7_adc_pin = *((uint32_t *)params);
    //-------------------------------------------------
    // let the sensor to heat for 20seconds
    for (uint8_t j = 20; j > 0; j--)
    {
        TRACE_E("Heating sensor.........time left: %d sec", j);
        vTaskDelay(100); // vTaskDelay(1000 / portTICK_PERIOD_MS); // 1sec delay before calibration
    }
    //-------------------------------------------------
    // extract the mean_sensor_analog_output_voltage
    float _sensor_volt = 0;
    s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};
    for (uint8_t i = 100; i > 0; i--)
    {
        if (i % 20 == 0)
        {
            TRACE_W("Please Wait..Collecting Ambient Air data ........... [Avoid Smokes/gases]");
        }
        // extract ADC values
        ezlopi_adc_get_adc_data(mq7_adc_pin, &ezlopi_analog_data);
#ifdef VOLTAGE_DIVIDER_ADDED
        _sensor_volt += (float)((ezlopi_analog_data.voltage) * 2.0f); // [0-2.4V] X2
#else
        _sensor_volt += (float)(ezlopi_analog_data.voltage);
#endif
        vTaskDelay(1); // 10ms
    }
    _sensor_volt = _sensor_volt / 100.0f;

    //-------------------------------------------------
    // Calculate the 'Rs' of heater during clean air [calibration phase]
    // Range -> [2Kohm - 20Kohm]
    float RS_calib = 0;                                                                         // Define variable for sensor resistance
    RS_calib = ((MQ7_VOLT_RESOLUTION_Vc * mq7_eqv_RL) / (_sensor_volt / 1000.0f)) - mq7_eqv_RL; // Calculate RS in fresh air
    TRACE_E("CALIB_TASK -> 'RS_calib' = %.2f", RS_calib);
    if (RS_calib < 0)
    {
        RS_calib = 0; // No negative values accepted.
    }
    // Calculate the R0_air which is constant through-out
    MQ7_R0_constant = (RS_calib / RatioMQ7CleanAir); // Calculate MQ7_R0_constant
    TRACE_E("CALIB_TASK -> 'MQ7_R0_constant' = %.2f", MQ7_R0_constant);
    if (MQ7_R0_constant < 0)
    {
        MQ7_R0_constant = 0; // No negative values accepted.
    }
    // Set calibration_complete_CO flag
    Calibration_complete_CO = true;
    vTaskDelete(NULL);
}