#include "trace.h"
#include "cJSON.h"
#include "math.h"
#include "stdbool.h"

#include "ezlopi_devices_list.h"
#include "ezlopi_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"

#include "sensor_0047_other_HX711_loadcell.h"
/********************************************************************************/
/*                    global defines                                            */
/********************************************************************************/
#define PORT_ENTER_CRITICAL() portENTER_CRITICAL(&mux)
#define PORT_EXIT_CRITICAL() portEXIT_CRITICAL(&mux)
static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

typedef enum
{
    HX711_GAIN_128 = 2,
    HX711_GAIN_32 = 3,
    HX711_GAIN_64 = 4
} hx711_gain_t;

typedef struct s_hx711_data
{
    gpio_num_t HX711_DT_pin;
    gpio_num_t HX711_SCK_pin;
    bool HX711_initialized;
    float HX711_tare_wt;
    float weight;
} s_hx711_data_t;

static int __0047_prepare(void *arg);
static int __0047_init(l_ezlopi_item_t *item);
static int __0047_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __0047_notify(l_ezlopi_item_t *item);

static void __hx711_power_reset(l_ezlopi_item_t *item);
static float __hx711_rawdata(l_ezlopi_item_t *item, hx711_gain_t _gain);
static float __hx711_avg_reading(l_ezlopi_item_t *item, uint8_t sample_iteration);

static void __Calculate_hx711_tare_wt(void *params);
//-------------------------------------------------------------------------------------------------------------------
int sensor_0047_other_HX711_loadcell(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0047_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0047_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0047_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        if (item)
        {
            s_hx711_data_t *user_data = (s_hx711_data_t *)item->user_arg;
            if (user_data->HX711_initialized)
            {
                __0047_notify(item);
            }
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
//------------------------------------------------------------------------------------------------------
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type = dev_type_sensor;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, s_hx711_data_t *user_data)
{
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_weight;
    item->cloud_properties.value_type = value_type_mass;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_kilo_gram;
    item->user_arg = user_data;

    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type); // _max = 10
    CJSON_GET_VALUE_INT(cj_device, "gpio1", user_data->HX711_SCK_pin);
    TRACE_I("hx711_SCL_PIN: %d ", user_data->HX711_SCK_pin);
    CJSON_GET_VALUE_INT(cj_device, "gpio2", user_data->HX711_DT_pin);
    TRACE_I("hx711_DT_PIN: %d ", user_data->HX711_DT_pin);
}

static int __0047_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        s_hx711_data_t *hx711_data = (s_hx711_data_t *)malloc(sizeof(s_hx711_data_t));
        if (hx711_data)
        {
            memset(hx711_data, 0, sizeof(s_hx711_data_t));
            //---------------------------  DIGI - DEVICE 1 --------------------------------------------
            l_ezlopi_device_t *hx711_device = ezlopi_device_add_device();
            if (hx711_device)
            {
                __prepare_device_cloud_properties(hx711_device, device_prep_arg->cjson_device);
                l_ezlopi_item_t *hx711_item = ezlopi_device_add_item_to_device(hx711_device, sensor_0047_other_HX711_loadcell);
                if (hx711_item)
                {
                    __prepare_item_cloud_properties(hx711_item, device_prep_arg->cjson_device, hx711_data);
                }
                else
                {
                    ezlopi_device_free_device(hx711_device);
                    free(hx711_data);
                }
            }
            else
            {
                ezlopi_device_free_device(hx711_device);
                free(hx711_data);
            }
        }
    }
    return ret;
}

static int __0047_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        s_hx711_data_t *user_data = (s_hx711_data_t *)item->user_arg;
        // Configure 'CLOCK_PIN' -> GPIO output pins for HX711.
        gpio_config_t output_conf;
        output_conf.pin_bit_mask = (1ULL << (user_data->HX711_SCK_pin));
        output_conf.intr_type = GPIO_INTR_DISABLE;
        output_conf.mode = GPIO_MODE_OUTPUT;
        output_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
        output_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        ret |= gpio_config(&output_conf);

        // Configure 'DATA_PIN' ->  GPIO input pins for HX711.
        gpio_config_t input_conf;
        input_conf.pin_bit_mask = (1ULL << (user_data->HX711_DT_pin));
        input_conf.intr_type = GPIO_INTR_DISABLE;
        input_conf.mode = GPIO_MODE_INPUT;
        input_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
        input_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        ret |= gpio_config(&input_conf);

        // then initiate calibration task
        if (false == (user_data->HX711_initialized))
        {
            __hx711_power_reset(item);
            xTaskCreate(__Calculate_hx711_tare_wt, "Calculate the Tare weight", 2 * 2048, item, 1, NULL);
        }
        ret = 1;
    }
    return ret;
}

static int __0047_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            s_hx711_data_t *user_data = (s_hx711_data_t *)item->user_arg;
            char *valueFormatted = ezlopi_valueformatter_float(user_data->weight);
            cJSON_AddStringToObject(cj_result, "ValueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cj_result, "value", user_data->weight);
            free(valueFormatted);
        }
        ret = 1;
    }
    return ret;
}

static int __0047_notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        s_hx711_data_t *user_data = (s_hx711_data_t *)item->user_arg;
        float Mass = __hx711_avg_reading(item, 10); /// 1000.0f; // to avoid spikes
        float weight_in_kg = ((Mass - (user_data->HX711_tare_wt)) / 100.0f) / 1000.0f;

        if (fabs(weight_in_kg - (user_data->weight)) > 0.05)
        {
            if (weight_in_kg < 0)
            {
                weight_in_kg = 0;
            }
            user_data->weight = weight_in_kg;
            // TRACE_I("Mass : %0.2f unit , _Offset : %0.2f unit , Actual_Mass : %0.2f kg ,", Mass, (user_data->HX711_tare_wt), weight_in_kg);
            ezlopi_device_value_updated_from_device_v3(item);
        }
        ret = 1;
    }
    return ret;
}

static void __Calculate_hx711_tare_wt(void *params)
{
    float RAW_tare = 0;
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)params;
    if (item)
    {
        s_hx711_data_t *user_data = (s_hx711_data_t *)item->user_arg;
        // For Output settling time ; [10SPS] is 400ms
        // So, wait for 400ms after reset [as per datasheet]
        vTaskDelay(400 / portTICK_PERIOD_MS);

        // ignore first few weight readings [150th ~ 180th]
        for (uint8_t i = 180; i > 0; i--)
        {
            RAW_tare = __hx711_rawdata(item, HX711_GAIN_64); // 100ms each read
            if (i > 150)
            {
                (user_data->HX711_tare_wt) = (RAW_tare);
            }
            else
            {
                (user_data->HX711_tare_wt) = 0.2f * (user_data->HX711_tare_wt) + 0.8f * (RAW_tare);
            }
            TRACE_I("Calibration_No : %d , Raw_data : %.2f", i, RAW_tare);
        }
        TRACE_I("Calibration Stage ----------> Tare_Offset : %0.2f ", (user_data->HX711_tare_wt));
        if ((user_data->HX711_tare_wt) > 3000)
        {
            user_data->HX711_initialized = true;
        }
    }
    vTaskDelete(NULL);
}

static float __hx711_rawdata(l_ezlopi_item_t *item, hx711_gain_t _gain)
{
    float raw_data = 0;
    unsigned long data = 0;
    if (item)
    {
        s_hx711_data_t *user_data = (s_hx711_data_t *)item->user_arg;
        vTaskDelay(8); // 80ms
        // first check if the data is ready [i.e. 'data_pin' = 1 ; when 'clk_pin' = 0]

        if (gpio_get_level(user_data->HX711_DT_pin))
        {
            // TRACE_E("Data_pin not ready..................");
            do
            {
                vTaskDelay(2); // 20ms
            } while (gpio_get_level(user_data->HX711_DT_pin));
        }

        PORT_ENTER_CRITICAL();
        /* STEP 1*/
        // perform 24bits read
        for (uint8_t i = 0; i < 24; i++)
        {
            gpio_set_level(user_data->HX711_SCK_pin, 1);
            esp_rom_delay_us(1); //  >= 0.2 us
            gpio_set_level(user_data->HX711_SCK_pin, 0);
            data = (data << 1);                          // shift the 'result' by 1 bit
            if (gpio_get_level(user_data->HX711_DT_pin)) // if data_pin is high
            {
                data |= (1 << 0); //[ 23~0 ]step
            }
            esp_rom_delay_us(1); //   keep duty cycle ~50%
        }

        /* STEP 2*/
        // Apply the gain pulses to complete serial communication
        for (uint8_t n = ((uint8_t)_gain); n > 0; n--)
        {
            gpio_set_level(user_data->HX711_SCK_pin, 1); // 'n' steps
            esp_rom_delay_us(1);
            gpio_set_level(user_data->HX711_SCK_pin, 0);
            esp_rom_delay_us(1);
        }
        PORT_EXIT_CRITICAL();

        //----ending the conversion---
        // Note : 'Data_pin' is pulled high starting during  step: [24~27]
        if (!gpio_get_level(user_data->HX711_DT_pin))
        {
            TRACE_E("DATA_PIN is low........................... SYSTEM BUSY");
        }

        // SIGN extend
        // 24th bit is the sign bit //
        data ^= 0x800000;

        raw_data = (float)data;
    }
    // TRACE_E("Raw_data -> %.2f ", raw_data);
    return raw_data;
}

static float __hx711_avg_reading(l_ezlopi_item_t *item, uint8_t sample_iteration)
{
    float sum = 0;
    if (sample_iteration < 1)
    {
        sample_iteration = 1;
    }
    if (sample_iteration > 10)
    {
        sample_iteration = 10;
    }

    for (int i = 0; i < sample_iteration; i++)
    {
        sum += __hx711_rawdata(item, HX711_GAIN_64);
    }
    sum /= (float)sample_iteration; // avoid dividing by zero
    return sum;
}

static void __hx711_power_reset(l_ezlopi_item_t *item)
{
    if (item)
    {
        s_hx711_data_t *user_data = (s_hx711_data_t *)item->user_arg;
        PORT_ENTER_CRITICAL();
        // Pull the clock pin low to make sure this reset condition occurs
        gpio_set_level(user_data->HX711_SCK_pin, 0);

        // Pull the clock pin high
        gpio_set_level(user_data->HX711_SCK_pin, 1);
        esp_rom_delay_us(70);

        // Pull the clock pin low for NORMAL operation
        gpio_set_level(user_data->HX711_SCK_pin, 0);
        esp_rom_delay_us(10);
        PORT_EXIT_CRITICAL();
    }
}