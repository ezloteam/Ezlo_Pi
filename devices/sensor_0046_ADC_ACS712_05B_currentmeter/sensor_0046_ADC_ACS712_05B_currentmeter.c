#include "trace.h"
#include "cJSON.h"
#include "math.h"
#include "ezlopi_adc.h"
#include "ezlopi_timer.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"

#include "sensor_0046_ADC_ACS712_05B_currentmeter.h"

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#define PORT_ENTER_CRITICAL() portENTER_CRITICAL(&mux)
#define PORT_EXIT_CRITICAL() portEXIT_CRITICAL(&mux)

typedef struct s_currentmeter
{
    float amp_value;
} s_currentmeter_t;

static int __0046_prepare(void *arg);
static int __0046_init(l_ezlopi_item_t *item);
static int __0046_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __0046_notify(l_ezlopi_item_t *item);
static void __calculate_current_value(l_ezlopi_item_t *item);

int sensor_0046_ADC_ACS712_05B_currentmeter(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0046_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0046_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0046_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __0046_notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_electricity;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type = dev_type_sensor;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data)
{
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_current;
    item->cloud_properties.show = true;
    item->cloud_properties.value_type = value_type_electric_current;
    item->cloud_properties.scale = scales_ampere;

    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type); // _max = 10
    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit

    // passing the custom data_structure
    item->user_arg = user_data;
}

//----------------------------------------------------
static int __0046_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        s_currentmeter_t *user_data = (s_currentmeter_t *)malloc(sizeof(s_currentmeter_t));
        if (NULL != user_data)
        {
            memset(user_data, 0, sizeof(s_currentmeter_t));
            l_ezlopi_device_t *currentmeter_device = ezlopi_device_add_device();
            if (currentmeter_device)
            {
                __prepare_device_cloud_properties(currentmeter_device, device_prep_arg->cjson_device);
                l_ezlopi_item_t *currentmeter_item = ezlopi_device_add_item_to_device(currentmeter_device, sensor_0046_ADC_ACS712_05B_currentmeter);
                if (currentmeter_item)
                {
                    __prepare_item_cloud_properties(currentmeter_item, device_prep_arg->cjson_device, user_data);
                }
                else
                {
                    ezlopi_device_free_device(currentmeter_device);
                    free(user_data);
                }
            }
            else
            {
                ezlopi_device_free_device(currentmeter_device);
                free(user_data);
            }
            ret = 1;
        }
    }
    return ret;
}

static int __0046_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
        {
            ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit);
            ret = 1;
        }
    }
    return ret;
}

static int __0046_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)arg;
    if (cjson_properties && item)
    {
        s_currentmeter_t *user_data = (s_currentmeter_t *)item->user_arg;
        char *valueFormatted = ezlopi_valueformatter_float(user_data->amp_value);
        cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
        cJSON_AddNumberToObject(cjson_properties, "value", user_data->amp_value); // Irms [A]
        free(valueFormatted);
        ret = 1;
    }
    return ret;
}

static int __0046_notify(l_ezlopi_item_t *item)
{
    // During this calculation the system is polled for 20mS
    int ret = 0;
    if (item)
    {
        s_currentmeter_t *user_data = (s_currentmeter_t *)item->user_arg;
        float prev_amp = user_data->amp_value;
        __calculate_current_value(item); // update amp
        if (fabs(user_data->amp_value - prev_amp) > 0.5)
        {
            ezlopi_device_value_updated_from_device_v3(item);
        }
    }
    return ret;
}
//--------------------------------------------------------------------------------------------------------------------------------------
static void __calculate_current_value(l_ezlopi_item_t *item)
{
    if (NULL != item)
    {
        s_currentmeter_t *user_data = (s_currentmeter_t *)item->user_arg;
        s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};

        uint32_t period_dur = (1000000 / DEFAULT_AC_FREQUENCY); // 20000uS
        int Vnow = 0;
        uint32_t Vsum = 0;
        uint32_t measurements_count = 0;

        // starting 't' instant
        uint32_t t_start = (uint32_t)esp_timer_get_time();
        uint32_t Volt = 0;
        int diff = 0;
        PORT_ENTER_CRITICAL();
        while (((uint32_t)esp_timer_get_time() - t_start) < period_dur) // loops within 1-complete cycle
        {
            ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
            Volt = 2 * (ezlopi_analog_data.voltage); // since the input is half the original value after voltage division
            diff = ((ASC712TELC_05B_zero_point_mV - Volt) > 0 ? (ASC712TELC_05B_zero_point_mV - Volt) : (Volt - ASC712TELC_05B_zero_point_mV));
            // getting the voltage value at this instant
            if (diff > 150 && diff < 1500) // the reading voltage less than 00mV is noise
            {
                Vnow = Volt - ASC712TELC_05B_zero_point_mV; // ()at zero offset => full-scale/2
            }
            else
            {
                Vnow = 0;
            }
            Vsum += (Vnow * Vnow); // sumof(I^2 + I^2 + .....)
            measurements_count++;
        }
        PORT_EXIT_CRITICAL();

        // If applied for DC;  'AC_Irms' calculation give same value as 'DC-value'
        if (0 == measurements_count)
        {
            measurements_count = 1; // <-- avoid dividing by zero
        }

        user_data->amp_value = ((float)sqrt(Vsum / measurements_count)) / 185.0f; //  -> I[rms] Ampere
        if ((user_data->amp_value) < 0.4)
        {
            user_data->amp_value = 0;
        }
    }
}