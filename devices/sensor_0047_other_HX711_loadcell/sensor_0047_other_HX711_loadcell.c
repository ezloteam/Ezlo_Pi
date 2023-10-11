#include "trace.h"
#include "driver/gpio.h"
#include "ezlopi_cloud.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_item_name_str.h"

#include "sensor_0047_other_HX711_loadcell.h"
#include "sensor_0047_other_HX711_loadcell.h"

/********************************************************************************/
/*                    global defines                                            */
/********************************************************************************/
#define PORT_ENTER_CRITICAL() portENTER_CRITICAL(&mux)
#define PORT_EXIT_CRITICAL() portEXIT_CRITICAL(&mux)
static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

static gpio_num_t HX711_DT_pin, HX711_SCK_pin;
static bool HX711_initialized = false;

static float HX711_tare_wt = 0; // this value is weight of the device itself [treated as offset]
/********************************************************************************/
/*                    Declarations                                              */
/********************************************************************************/
static int sensor_0047_other_HX711_prepare_and_add(void *arg);
static s_ezlopi_device_properties_t *sensor_0047_hx711_prepare_properties(cJSON *cjson_device);
static int sensor_0047_other_HX711_init(s_ezlopi_device_properties_t *properties);
static int sensor_0047_other_HX711_get_value(s_ezlopi_device_properties_t *properties, void *args);
static float HX711_rawData(hx711_gain_t _gain);
static float HX711_avg_dataReading(uint8_t sample_iteration);
static void HX711_Power_Reset(void);
//----------------------------------------------------------------------------------------------------------
int sensor_0047_other_hx711(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlopi_device, void *arg, void *user_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_0047_other_HX711_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_0047_other_HX711_init(ezlopi_device);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_0047_other_HX711_get_value(ezlopi_device, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        static uint8_t count = 0;
        if (HX711_initialized)
        {
            if ((++count) > 2)
            {
                count = 0;
                ret = ezlopi_device_value_updated_from_device(ezlopi_device);
            }
        }

        break;
    }

    default:
        break;
    }
    return ret;
}
//----------------------------------------------------------------------------------------------------------

static int sensor_0047_other_HX711_prepare_and_add(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if ((prep_arg) && (prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_0047_other_hx711_properties = sensor_0047_hx711_prepare_properties(prep_arg->cjson_device);
        if (sensor_0047_other_hx711_properties)
        {
            if (0 == ezlopi_devices_list_add(prep_arg->device, sensor_0047_other_hx711_properties, NULL))
            {
                free(sensor_0047_other_hx711_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *sensor_0047_hx711_prepare_properties(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_hx711_properties = NULL;

    if (NULL != cjson_device)
    {
        sensor_hx711_properties = (s_ezlopi_device_properties_t *)malloc(sizeof(s_ezlopi_device_properties_t));
        if (sensor_hx711_properties)
        {
            memset(sensor_hx711_properties, 0, sizeof(s_ezlopi_device_properties_t));
            sensor_hx711_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT;

            char *device_name = NULL;

            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
            ASSIGN_DEVICE_NAME(sensor_hx711_properties, device_name);

            sensor_hx711_properties->ezlopi_cloud.category = category_level_sensor;
            sensor_hx711_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
            sensor_hx711_properties->ezlopi_cloud.item_name = ezlopi_item_name_weight;
            sensor_hx711_properties->ezlopi_cloud.device_type = dev_type_sensor;
            sensor_hx711_properties->ezlopi_cloud.value_type = value_type_mass;
            sensor_hx711_properties->ezlopi_cloud.has_getter = true;
            sensor_hx711_properties->ezlopi_cloud.has_setter = false;
            sensor_hx711_properties->ezlopi_cloud.reachable = true;
            sensor_hx711_properties->ezlopi_cloud.battery_powered = false;
            sensor_hx711_properties->ezlopi_cloud.show = true;
            sensor_hx711_properties->ezlopi_cloud.room_name[0] = '\0';
            sensor_hx711_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
            sensor_hx711_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            sensor_hx711_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

            CJSON_GET_VALUE_INT(cjson_device, "gpio1", HX711_SCK_pin);
            CJSON_GET_VALUE_INT(cjson_device, "gpio2", HX711_DT_pin);
        }
    }
    return sensor_hx711_properties;
}

void Calculate_hx711_tare_wt(void *params)
{
    // For Output settling time ; [10SPS] is 400ms
    // So, wait for 400ms after reset [as per datasheet]
    vTaskDelay(400 / portTICK_PERIOD_MS);

    // ignore first few weight readings
    float RAW_tare = 0;
    for (uint8_t i = 180; i > 0; i--)
    {

        // extract the raw_data
        RAW_tare = HX711_rawData(HX711_GAIN_64); // 100ms each read

        if (i < 150)
        { // data below 100 iteration are considered
            HX711_tare_wt = 0.2f * HX711_tare_wt + 0.8f * (RAW_tare);
            // HX711_tare_wt += (RAW_tare);
        }
        else
        { // all the data above 150 iterations are replaced
            HX711_tare_wt = (RAW_tare);
        }

        // if data recieved is not timed-out [i.e. valid]
        TRACE_I("Calibration_No : %d , Raw_data : %.2f", i, RAW_tare);
    }

    // HX711_tare_wt /= 150.0f;

    // now set the offset_flag
    TRACE_I("Calibration Stage ----------> Tare_Offset : %0.2f ", HX711_tare_wt);
    if (HX711_tare_wt > 3000)
    {
        HX711_initialized = true;
    }

    // delete task
    vTaskDelete(NULL);
}

static void HX711_Power_Reset(void)
{
    PORT_ENTER_CRITICAL();
    // Pull the clock pin low to make sure this reset condition occurs
    gpio_set_level(HX711_SCK_pin, 0);

    // Pull the clock pin high
    gpio_set_level(HX711_SCK_pin, 1);
    esp_rom_delay_us(70);

    // Pull the clock pin low for NORMAL operation
    gpio_set_level(HX711_SCK_pin, 0);
    esp_rom_delay_us(10);
    PORT_EXIT_CRITICAL();
}

static float HX711_avg_dataReading(uint8_t sample_iteration)
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
        sum += HX711_rawData(HX711_GAIN_64);
    }
    sum /= (float)sample_iteration; // avoid dividing by zero
    return sum;
}

static float HX711_rawData(hx711_gain_t _gain)
{
    vTaskDelay(8); // 80ms
    float raw_data = 0;
    unsigned long data = 0;
    // first check if the data is ready [i.e. 'data_pin' = 1 ; when 'clk_pin' = 0]

    if (gpio_get_level(HX711_DT_pin))
    {
        // TRACE_E("Data_pin not ready..................");
        do
        {
            vTaskDelay(2); // 20ms
        } while (gpio_get_level(HX711_DT_pin));
    }

    PORT_ENTER_CRITICAL();
    /* STEP 1*/
    // perform 24bits read
    for (uint8_t i = 0; i < 24; i++)
    {
        gpio_set_level(HX711_SCK_pin, 1);
        esp_rom_delay_us(1); //  >= 0.2 us
        gpio_set_level(HX711_SCK_pin, 0);
        data = (data << 1);               // shift the 'result' by 1 bit
        if (gpio_get_level(HX711_DT_pin)) // if data_pin is high
        {
            data |= (1 << 0); //[ 23~0 ]step
        }
        esp_rom_delay_us(1); //   keep duty cycle ~50%
    }

    /* STEP 2*/
    // Apply the gain pulses to complete serial communication
    for (uint8_t n = ((uint8_t)_gain); n > 0; n--)
    {
        gpio_set_level(HX711_SCK_pin, 1); // 'n' steps
        esp_rom_delay_us(1);
        gpio_set_level(HX711_SCK_pin, 0);
        esp_rom_delay_us(1);
    }
    PORT_EXIT_CRITICAL();

    //----ending the conversion---
    // Note : 'Data_pin' is pulled high starting during  step: [24~27]
    if (!gpio_get_level(HX711_DT_pin))
    {
        TRACE_E("DATA_PIN is low........................... SYSTEM BUSY");
    }

    // SIGN extend
    // 24th bit is the sign bit //
    data ^= 0x800000;

    raw_data = (float)data;
    // TRACE_E("Raw_data -> %.2f ", raw_data);

    return raw_data;
}

static int sensor_0047_other_HX711_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    // first initialize the gpio_pins
    static bool gaurd = false;
    if (!gaurd)
    {
        // Configure 'CLOCK_PIN' -> GPIO output pins for HX711.
        gpio_config_t output_conf;
        output_conf.pin_bit_mask = (1ULL << HX711_SCK_pin);
        output_conf.intr_type = GPIO_INTR_DISABLE;
        output_conf.mode = GPIO_MODE_OUTPUT;
        output_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
        output_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        ret |= gpio_config(&output_conf);

        // Configure 'DATA_PIN' ->  GPIO input pins for HX711.
        gpio_config_t input_conf;
        input_conf.pin_bit_mask = (1ULL << HX711_DT_pin);
        input_conf.intr_type = GPIO_INTR_DISABLE;
        input_conf.mode = GPIO_MODE_INPUT;
        input_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
        input_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        ret |= gpio_config(&input_conf);
        gaurd = true;
    }

    // then initiate calibration task
    if (!HX711_initialized)
    {
        // Reset power
        HX711_Power_Reset();

        //  2. calibrate the load cell
        xTaskCreate(Calculate_hx711_tare_wt, "Calculate the Tare weight", 2*2048, NULL, 1, NULL);
    }
    return ret;
}

static int sensor_0047_other_HX711_get_value(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    static float Mass = 0;
    cJSON *cjson_properties = (cJSON *)args;
    char valueFormatted[20];
    if (cjson_properties)
    {
        if (ezlopi_item_name_weight == properties->ezlopi_cloud.item_name)
        {
            Mass = HX711_avg_dataReading(10); /// 1000.0f; // to avoid spikes
            float weight_in_gm = (Mass - HX711_tare_wt) / 100.0f;
            float weight_in_kg = weight_in_gm / 1000.0f;
            TRACE_I("Mass : %0.2f unit , _Offset : %0.2f unit , Actual_Mass : %0.2f kg ,", Mass, HX711_tare_wt, weight_in_kg);
            snprintf(valueFormatted, 20, "%.2f", weight_in_kg);
            cJSON_AddStringToObject(cjson_properties, "ValueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cjson_properties, "value", weight_in_kg);
            cJSON_AddStringToObject(cjson_properties, "scale", "kilo_gram");
        }
    }

    return ret;
}
