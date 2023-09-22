#include "trace.h"
#include "cJSON.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_adc.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"

#include "0048_sensor_other_MQ4_CH4_detector.h"

//*************************************************************************
//                          Declaration
//*************************************************************************
static float MQ4_R0_constant = 0;         // Define variable for MQ4_R0_constant [always constant]
static bool Calibration_complete = false; // flag to activate calibration phase
const char *mq4_sensor_gas_alarm_token[] =
    {
        "no_gas",
        "combustible_gas_detected",
        "toxic_gas_detected",
        "unknown"};
//--------------------------------------------------------------------------------------------------------
static int __0048_prepare(void *arg);
static int __0048_init(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __notify(l_ezlopi_item_t *item);
//--------------------------------------------------------------------------------------------------------

int sensor_MQ4_CH4_detector_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0048_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0048_init(item);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __notify(item);
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
static int __0048_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        //---------------------------  DIGI - DEVICE 1 --------------------------------------------
        l_ezlopi_device_t *MQ4_device_digi = ezlopi_device_add_device();
        if (MQ4_device_digi)
        {
            __prepare_device_digi_cloud_properties(MQ4_device_digi, device_prep_arg->cjson_device);
            l_ezlopi_item_t *MQ4_item_digi = ezlopi_device_add_item_to_device(MQ4_device_digi, sensor_MQ4_CH4_detector_v3);
            if (MQ4_item_digi)
            {
                _prepare_item_digi_cloud_properties(MQ4_item_digi, device_prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(MQ4_device_digi);
            }
        }
        else
        {
            ezlopi_device_free_device(MQ4_device_digi);
        }

        //---------------------------- ADC - DEVICE 2 -------------------------------------------

        l_ezlopi_device_t *MQ4_device_adc = ezlopi_device_add_device();
        if (MQ4_device_adc)
        {
            __prepare_device_adc_cloud_properties(MQ4_device_adc, device_prep_arg->cjson_device);
            l_ezlopi_item_t *MQ4_item_adc = ezlopi_device_add_item_to_device(MQ4_device_adc, sensor_MQ4_CH4_detector_v3);
            if (MQ4_item_adc)
            {
                _prepare_item_adc_cloud_properties(MQ4_item_adc, device_prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(MQ4_device_adc);
            }
        }
        else
        {
            ezlopi_device_free_device(MQ4_device_adc);
        }
    }
}

static int __0048_init(l_ezlopi_item_t *item)
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
            // TRACE_W(".........................................CALIB_GAURD : %d", Calibration_complete);
            if (!Calibration_complete)
            {
                xTaskCreate(Calibrate_MQ4_R0_resistance, "Task_to_calculate_R0_air", 2048, NULL, 1, NULL);
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
    // CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    device_name = "MQ4-CH4-alert";
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
    CJSON_GET_VALUE_INT(cj_device, "en_gpio1", item->interface.gpio.gpio_in.enable); // we can enable/disable DIGI_GPIO
}
//------------------------------------------------------------------------------------------------------
static void __prepare_device_adc_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    device_name = "MQ4-CH4-level[ppm]";
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

    item->interface.adc.resln_bit = 3; // ADC 12_bit
}
//------------------------------------------------------------------------------------------------------
void Calibrate_MQ4_R0_resistance(void *params)
{
    float RS_calib = 0; // Define variable for sensor resistance
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
            ezlopi_adc_get_adc_data(mq4_adc_pin, ezlopi_analog_data);
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
    RS_calib = ((MQ4_VOLT_RESOLUTION_Vc * mq4_eqv_RL) / (_sensor_volt / 1000.0f)) - mq4_eqv_RL; // Calculate RS in fresh air
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