#include <math.h>
#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_processes.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0059_other_MQ6_LPG_detector.h"
#include "EZLOPI_USER_CONFIG.h"

//*************************************************************************
//                          Declaration
//*************************************************************************
typedef struct s_mq6_value
{
    float _LPG_ppm;
    float MQ6_R0_constant;
    bool Calibration_complete_LPG;
} s_mq6_value_t;

const char* mq6_sensor_gas_alarm_token[] = {
    "no_gas",
    "combustible_gas_detected",
    "toxic_gas_detected",
    "unknown",
};
//--------------------------------------------------------------------------------------------------------
static int __0059_prepare(void* arg);
static int __0059_init(l_ezlopi_item_t* item);
static int __0059_get_item(l_ezlopi_item_t* item, void* arg);
static int __0059_get_cjson_value(l_ezlopi_item_t* item, void* arg);
static int __0059_notify(l_ezlopi_item_t* item);

static float __extract_MQ6_sensor_ppm(l_ezlopi_item_t* item);
static void __calibrate_MQ6_R0_resistance(void* params);
static void __prepare_device_digi_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device);
static void __prepare_item_digi_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device);
static void __prepare_device_adc_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device);
static void __prepare_item_adc_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_data);
//--------------------------------------------------------------------------------------------------------

int sensor_0059_other_MQ6_LPG_detector(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0059_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0059_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        ret = __0059_get_item(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0059_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0059_notify(item);
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
static int __0059_prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* device_prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        //---------------------------  DIGI - DEVICE 1 --------------------------------------------
        l_ezlopi_device_t* MQ6_device_parent_digi = ezlopi_device_add_device(device_prep_arg->cjson_device, "digi");
        if (MQ6_device_parent_digi)
        {
            ret = 1;
            TRACE_I("Parent_MQ6_device_digi-[0x%x] ", MQ6_device_parent_digi->cloud_properties.device_id);
            __prepare_device_digi_cloud_properties(MQ6_device_parent_digi, device_prep_arg->cjson_device);

            l_ezlopi_item_t* MQ6_item_digi = ezlopi_device_add_item_to_device(MQ6_device_parent_digi, sensor_0059_other_MQ6_LPG_detector);
            if (MQ6_item_digi)
            {
                __prepare_item_digi_cloud_properties(MQ6_item_digi, device_prep_arg->cjson_device);
            }
            else
            {
                ret = -1;
            }

            //---------------------------- ADC - DEVICE 2 -------------------------------------------
            s_mq6_value_t* MQ6_value = (s_mq6_value_t*)ezlopi_malloc(__FUNCTION__, sizeof(s_mq6_value_t));
            if (NULL != MQ6_value)
            {
                memset(MQ6_value, 0, sizeof(s_mq6_value_t));
                l_ezlopi_device_t* MQ6_device_child_adc = ezlopi_device_add_device(device_prep_arg->cjson_device, "adc");
                if (MQ6_device_child_adc)
                {
                    TRACE_I("Child_MQ135_device_adc-[0x%x] ", MQ6_device_child_adc->cloud_properties.device_id);
                    __prepare_device_adc_cloud_properties(MQ6_device_child_adc, device_prep_arg->cjson_device);

                    MQ6_device_child_adc->cloud_properties.parent_device_id = MQ6_device_parent_digi->cloud_properties.device_id;
                    l_ezlopi_item_t* MQ6_item_adc = ezlopi_device_add_item_to_device(MQ6_device_child_adc, sensor_0059_other_MQ6_LPG_detector);
                    if (MQ6_item_adc)
                    {
                        __prepare_item_adc_cloud_properties(MQ6_item_adc, device_prep_arg->cjson_device, MQ6_value);
                    }
                    else
                    {
                        ret = -1;
                        ezlopi_device_free_device(MQ6_device_child_adc);
                        ezlopi_free(__FUNCTION__, MQ6_value);
                    }
                }
                else
                {
                    ret = -1;
                    ezlopi_free(__FUNCTION__, MQ6_value);
                }
            }
            else
            {
                ret = -1;
            }
        }
        else
        {
            ret = -1;
        }
    }
    return ret;
}

static int __0059_init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (NULL != item)
    {
        if (ezlopi_item_name_gas_alarm == item->cloud_properties.item_name)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
            { // intialize digital_pin
                gpio_config_t input_conf = {};
                input_conf.pin_bit_mask = (1ULL << (item->interface.gpio.gpio_in.gpio_num));
                input_conf.intr_type = GPIO_INTR_DISABLE;
                input_conf.mode = GPIO_MODE_INPUT;
                input_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
                input_conf.pull_up_en = GPIO_PULLUP_ENABLE;
                ret = (0 == gpio_config(&input_conf)) ? 1 : -1;
            }
            else
            {
                ret = -1;
            }
        }
        else if ((ezlopi_item_name_smoke_density == item->cloud_properties.item_name))
        {
            s_mq6_value_t* MQ6_value = (s_mq6_value_t*)item->user_arg;
            if (MQ6_value)
            {
                if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
                {
                    // initialize analog_pin
                    if (0 == ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
                    {
                        // calibrate if not done
                        if (false == MQ6_value->Calibration_complete_LPG)
                        {
                            TaskHandle_t ezlopi_sensor_mq6_task_handle = NULL;
                            xTaskCreate(__calibrate_MQ6_R0_resistance, "Task_to_calculate_R0_air", EZLOPI_SENSOR_MQ6_TASK_DEPTH, item, 1, &ezlopi_sensor_mq6_task_handle);
                            ezpi_core_process_set_process_info(ENUM_EZLOPI_SENSOR_MQ6_TASK, &ezlopi_sensor_mq6_task_handle, EZLOPI_SENSOR_MQ6_TASK_DEPTH);
                        }

                        ret = 1;
                    }
                    else
                    {
                        ret = -1;
                    }
                }
                else
                {
                    ret = -1;
                }
            }
            else
            {
                ret = -1;
            }
        }
    }
    return ret;
}

//------------------------------------------------------------------------------------------------------
static void __prepare_device_digi_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_gas;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}
static void __prepare_item_digi_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_gas_alarm;
    item->cloud_properties.value_type = value_type_token;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.gpio.gpio_in.gpio_num);
    TRACE_S("MQ6-> DIGITAL_PIN: %d ", item->interface.gpio.gpio_in.gpio_num);
}
//------------------------------------------------------------------------------------------------------
static void __prepare_device_adc_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}
static void __prepare_item_adc_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_data)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_smoke_density;
    item->cloud_properties.value_type = value_type_substance_amount;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_parts_per_million;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_gpio2_str, item->interface.adc.gpio_num);
    TRACE_S("MQ6-> ADC_PIN: %d ", item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit

    // passing the custom data_structure
    item->is_user_arg_unique = true;
    item->user_arg = user_data;
}

//------------------------------------------------------------------------------------------------------
static int __0059_get_item(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cj_result = (cJSON*)arg;
        if (cj_result)
        {
            if (ezlopi_item_name_gas_alarm == item->cloud_properties.item_name)
            {
                //-------------------  POSSIBLE JSON ENUM CONTENTS ----------------------------------
                cJSON* json_array_enum = cJSON_CreateArray(__FUNCTION__);
                if (NULL != json_array_enum)
                {
                    for (uint8_t i = 0; i < MQ6_GAS_ALARM_MAX; i++)
                    {
                        cJSON* json_value = cJSON_CreateString(__FUNCTION__, mq6_sensor_gas_alarm_token[i]);
                        if (NULL != json_value)
                        {
                            cJSON_AddItemToArray(json_array_enum, json_value);
                        }
                    }
                    cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_enum_str, json_array_enum);
                }
                //--------------------------------------------------------------------------------------
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, (char*)item->user_arg ? item->user_arg : "no_gas");
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, (char*)item->user_arg ? item->user_arg : "no_gas");
            }
            else if (ezlopi_item_name_smoke_density == item->cloud_properties.item_name)
            {
                s_mq6_value_t* MQ6_value = ((s_mq6_value_t*)item->user_arg);
                if (MQ6_value)
                {
                    ezlopi_valueformatter_float_to_cjson(item, cj_result, MQ6_value->_LPG_ppm);
                }
            }
            ret = 1;
        }
    }
    return ret;
}

static int __0059_get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cj_result = (cJSON*)arg;
        if (cj_result)
        {
            if (ezlopi_item_name_gas_alarm == item->cloud_properties.item_name)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, (char*)item->user_arg ? item->user_arg : "no_gas");
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, (char*)item->user_arg ? item->user_arg : "no_gas");
            }
            else if (ezlopi_item_name_smoke_density == item->cloud_properties.item_name)
            {
                s_mq6_value_t* MQ6_value = ((s_mq6_value_t*)item->user_arg);
                if (MQ6_value)
                {
                    ezlopi_valueformatter_float_to_cjson(item, cj_result, MQ6_value->_LPG_ppm);
                }
            }
            ret = 1;
        }
    }
    return ret;
}

static int __0059_notify(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        if (ezlopi_item_name_gas_alarm == item->cloud_properties.item_name)
        {
            const char* curret_value = NULL;
            if (0 == gpio_get_level(item->interface.gpio.gpio_in.gpio_num)) // when D0 -> 0V,
            {
                curret_value = mq6_sensor_gas_alarm_token[1];
            }
            else
            {
                curret_value = mq6_sensor_gas_alarm_token[0];
            }
            if (curret_value != (char*)item->user_arg) // calls update only if there is change in state
            {
                item->user_arg = (void*)curret_value;
                ezlopi_device_value_updated_from_device_broadcast(item);
            }
        }
        else if (ezlopi_item_name_smoke_density == item->cloud_properties.item_name)
        {
            // extract the sensor_output_values
            s_mq6_value_t* MQ6_value = (s_mq6_value_t*)item->user_arg;
            if ((MQ6_value) && (true == MQ6_value->Calibration_complete_LPG))
            {
                double new_value = (double)__extract_MQ6_sensor_ppm(item);
                if (fabs((double)(MQ6_value->_LPG_ppm) - new_value) > 0.0001)
                {
                    MQ6_value->_LPG_ppm = (float)new_value;
                    ezlopi_device_value_updated_from_device_broadcast(item);
                }
            }
        }
        ret = 1;
    }
    return ret;
}
//------------------------------------------------------------------------------------------------------
static float __extract_MQ6_sensor_ppm(l_ezlopi_item_t* item)
{
    s_mq6_value_t* MQ6_value = (s_mq6_value_t*)item->user_arg;
    if (MQ6_value)
    { // calculation process
      //-------------------------------------------------
        uint32_t mq6_adc_pin = item->interface.adc.gpio_num;
        s_ezlopi_analog_data_t ezlopi_analog_data = { .value = 0, .voltage = 0 };
        // extract the mean_sensor_analog_output_voltage
        float analog_sensor_volt = 0;
        for (uint8_t x = 10; x > 0; x--)
        {
            ezlopi_adc_get_adc_data(mq6_adc_pin, &ezlopi_analog_data);
#ifdef VOLTAGE_DIVIDER_ADDED
            analog_sensor_volt += ((float)(ezlopi_analog_data.voltage) * 2.0f);
#else
            analog_sensor_volt += (float)(ezlopi_analog_data.voltage);
#endif
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        analog_sensor_volt = analog_sensor_volt / 10.0f;

        //-----------------------------------------------------------------------------------
        // Stage_2 : [from 'sensor_0059_ADC_MQ6_methane_gas_detector.h']

        // 1. Calculate 'Rs_gas' for the gas detected
        float Rs_gas = (((MQ6_VOLT_RESOLUTION_Vc * mq6_eqv_RL) / (analog_sensor_volt / 1000.0f)) - mq6_eqv_RL);

        // 1.1 Calculate @ 'ratio' during LPG presence
        double _ratio = (Rs_gas / ((MQ6_value->MQ6_R0_constant <= 0) ? (1.0f) : (MQ6_value->MQ6_R0_constant))); // avoid dividing by zero??
        if (_ratio <= 0)
        {
            _ratio = 0;
        }
        //-------------------------------------------------

        // 1.2 Calculate _LPG_ppm
        float _LPG_ppm = (float)pow(10, (((float)log10(_ratio)) - b_coeff_mq6) / m_slope_mq6); // ---> _LPG_ppm = 10 ^ [ ( log(ratio) - b ) / m ]
        if (_LPG_ppm < 0)
        {
            _LPG_ppm = 0; // No negative values accepted or upper datasheet recomendation.
        }
        TRACE_E("_LPG_ppm [LPG] : %.2f -> ratio[RS/R0] : %.2f -> Volts : %0.2fmv", _LPG_ppm, (float)_ratio, analog_sensor_volt);

        //-------------------------------------------------
        return _LPG_ppm;
    }
    return 0;
}

static void __calibrate_MQ6_R0_resistance(void* params)
{
    l_ezlopi_item_t* item = (l_ezlopi_item_t*)params;
    if (NULL != item)
    {
        s_mq6_value_t* MQ6_value = (s_mq6_value_t*)item->user_arg;
        if (MQ6_value)
        {
            uint32_t mq6_adc_pin = item->interface.adc.gpio_num;
            //-------------------------------------------------
            // let the sensor to heat for 20seconds
            for (uint8_t j = 20; j > 0; j--)
            {
                TRACE_E("Heating sensor.........time left: %d sec", j);
                vTaskDelay(1000 / portTICK_PERIOD_MS); // 1sec delay before calibration
            }
            //-------------------------------------------------
            // extract the mean_sensor_analog_output_voltage
            float _sensor_volt = 0;
            s_ezlopi_analog_data_t ezlopi_analog_data = { .value = 0, .voltage = 0 };
            for (uint8_t i = 100; i > 0; i--)
            {
                if (i % 20 == 0)
                {
                    TRACE_W("Please Wait..Collecting Ambient Air data ........... [Avoid Smokes/gases]");
                }
                // extract ADC values
                ezlopi_adc_get_adc_data(mq6_adc_pin, &ezlopi_analog_data);
#ifdef VOLTAGE_DIVIDER_ADDED
                _sensor_volt += (float)((ezlopi_analog_data.voltage) * 2.0f); // [0-2.4V] X2
#else
                _sensor_volt += (float)(ezlopi_analog_data->voltage);
#endif
                vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
            }
            _sensor_volt = _sensor_volt / 100.0f;

            //-------------------------------------------------
            // Calculate the 'Rs' of heater during clean air [calibration phase]
            // Range -> [2Kohm - 20Kohm]
            float RS_calib = ((MQ6_VOLT_RESOLUTION_Vc * mq6_eqv_RL) / (_sensor_volt / 1000.0f)) - mq6_eqv_RL; // Calculate RS in fresh air
            TRACE_E("CALIB_TASK -> 'RS_calib' = %.2f", RS_calib);
            if (RS_calib < 0)
            {
                RS_calib = 0; // No negative values accepted.
            }
            // Calculate the R0_air which is constant through-out
            MQ6_value->MQ6_R0_constant = (RS_calib / RatioMQ6CleanAir); // Calculate MQ6_R0_constant
            TRACE_E("CALIB_TASK -> 'MQ6_R0_constant' = %.2f", MQ6_value->MQ6_R0_constant);
            if (MQ6_value->MQ6_R0_constant < 0)
            {
                MQ6_value->MQ6_R0_constant = 0; // No negative values accepted.
            }
            // Set calibration_complete_LPG flag
            MQ6_value->Calibration_complete_LPG = true;
        }
    }
    ezpi_core_process_set_is_deleted(ENUM_EZLOPI_SENSOR_MQ6_TASK);
    vTaskDelete(NULL);
}