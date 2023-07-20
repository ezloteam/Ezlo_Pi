
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
#include "esp_log.h"

#include "sensor_0046_ADC_ACS712TELC_05B_currentMeter.h"
//*************************************************************************
//                          Declaration
//*************************************************************************
static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
static float Ampere = 0;
#define PORT_ENTER_CRITICAL() portENTER_CRITICAL(&mux)
#define PORT_EXIT_CRITICAL() portEXIT_CRITICAL(&mux)

static s_ezlopi_device_properties_t *sensor_adc_ACS712_prepare(cJSON *cjson_device); // you can directly add the prepare args here
static int sensor_adc_ACS712_prepare_and_add(void *arg);

static int sensor_adc_ACS712_init(s_ezlopi_device_properties_t *properties);
static int sensor_adc_ACS712_get_value(s_ezlopi_device_properties_t *properties, void *arg);

static void Calculate_AC_DC_current_value(s_ezlopi_device_properties_t *properties);

//--------------------------------------------------------------------------------------------------------------------------------------
int sensor_0046_ADC_ACS712TELC_05B_currentMeter(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *user_args)
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
        static uint8_t count = 3;
        if (3 < count)
        {
            ret = ezlopi_device_value_updated_from_device(ezlopi_device);
            count = 0;
        }
        else
        {
            Calculate_AC_DC_current_value(ezlopi_device);
        }
        count++;
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
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_0046_ADC_ACS712_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

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

    if (cjson_properties)
    {
        cJSON_AddNumberToObject(cjson_properties, "value", Ampere); // Irms [A]
        cJSON_AddStringToObject(cjson_properties, "scale", "Ampere");
        ret = 1;
    }
    return ret;
}

static void Calculate_AC_DC_current_value(s_ezlopi_device_properties_t *properties)
{
    // During this calculation the system is polled for 20mS
    // first determine if the incomming current is DC or AC
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    if (NULL != ezlopi_analog_data)
    {
        memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));

        // 1wave->20ms->20000uS
        uint32_t period_dur = (1000000 / DEFAULT_AC_FREQUENCY); // 20000uS
        int Vnow = 0;
        uint32_t Vsum = 0;
        uint32_t measurements_count = 0;

        // starting 't' instant
        uint32_t t_start = (uint32_t)esp_timer_get_time();
        // ESP_LOGW("TIME_TAG", "start_time : %d", t_start);

        PORT_ENTER_CRITICAL();
        while (((uint32_t)esp_timer_get_time() - t_start) < period_dur) // loops within 1-complete cycle
        {
            ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data);
            // getting the voltage value at this instant
#ifdef voltage_divider_added
            Vnow = (ezlopi_analog_data->voltage) * 2 - ASC712TELC_05B_zero_point_mV; // ()at zero offset => full-scale/2
#else if
            Vnow = (ezlopi_analog_data->voltage) - ASC712TELC_05B_zero_point_mV; // ()at zero offset => full-scale/2
#endif
            Vsum += Vnow * Vnow; // sumof(I^2 + I^2 + .....)
            measurements_count++;
        }
        PORT_EXIT_CRITICAL();

        // If applied for DC;  'AC_Irms' calculation give same value as 'DC-value'
        if (0 == measurements_count)
        {
            measurements_count = 1; // <-- avoid dividing by zero
        }
        Ampere = ((float)sqrt(Vsum / measurements_count)) / 185.0f; //  -> I[rms] Ampere

        // TRACE_E("AC current = %0.2f A", Ampere);
        //----------------------------------------------------------

        /*this portion calculates an instantaneous current as soon as the AC mesurement process is done*/
        float Amp_data = 0;
        ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data);
#ifdef voltage_divider_added
        // since the incoming voltage is halfed after voltage divider ,
        // we will double the extracted voltage and then
        // apply standard calibration methods to get desired results
        Amp_data = ((((float)(ezlopi_analog_data->voltage) * 2.0f) - (float)ASC712TELC_05B_zero_point_mV) / 185.0f); // ( current = analog_output / sens [185mV/A] )
#else if
        // the value extracted for 0A is already at 2.5V ; which is the max 2.4V analog input of esp32
        // Wihtout
        Amp_data = (((float)(ezlopi_analog_data->voltage) - (float)ASC712TELC_05B_zero_point_mV) / 185.0f); // ( current = analog_output / sens [185mV/A] )
#endif
        if (((Amp_data > 0) ? (Amp_data) : (Amp_data * -1)) < 0.3)
        {
            Amp_data = 0;
        }
        // TRACE_E("DC current = %0.2f A", Amp_data);

        // clear the allocated memory
        free(ezlopi_analog_data);
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------
