#include "trace.h"
#include "esp_err.h"
#include "math.h"
#include "stdbool.h"
#include "ezlopi_i2c_master.h"
#include "sensor_0040_PWM_TCS230.h"
#include "ezlopi_timer.h"
#include "cJSON.h"
#include "driver/gpio.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"
/*************************************************************************************************/
/*                              DEFINES                                                     */
/*************************************************************************************************/
#define REG_COUNT_LEN 6 // magnetometer data is to be read in one go .

static QueueHandle_t tcs230_queue = NULL;
static TCS230_queue_enum_t QueueFlag = TCS230_QUEUE_RESET;
static bool calibration_complete = false;

//------------------------------------------------------------------------------
static s_ezlopi_device_properties_t *rgb_properties = NULL;
//------------------------------------------------------------------------------
#define ADD_PROPERTIES_DEVICE_LIST(_properties, device_id, category, subcategory, item_name, value_type, cjson_device, sensor_0040_PWM_TCS230_data) \
    {                                                                                                                                               \
        _properties = sensor_pwm_tcs230_prepare_properties(device_id, category, subcategory, item_name,                                             \
                                                           value_type, cjson_device, sensor_0040_PWM_TCS230_data);                                  \
        if (NULL != _properties)                                                                                                                    \
        {                                                                                                                                           \
            add_device_to_list(prep_arg, _properties, NULL);                                                                                        \
        }                                                                                                                                           \
    }

//------------------------------------------------------------------------------

static void IRAM_ATTR gpio_isr_handler(void *args) // argument => time_us
{
    int32_t instant_uSec = (int32_t)esp_timer_get_time();
    if (xQueueSendFromISR(tcs230_queue, &instant_uSec, NULL))
    {
        QueueFlag = TCS230_QUEUE_AVAILABLE;
    }
    else
    {
        QueueFlag = TCS230_QUEUE_FULL;
    }
}

//------------------------------------------------------------------------------

static int sensor_pwm_tcs230_prepare(void *arg);
static s_ezlopi_device_properties_t *sensor_pwm_tcs230_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY,
                                                                          const char *ITEM_NAME, const char *VALUE_TYPE,
                                                                          cJSON *cjson_device, TCS230_data_t *sensor_0040_PWM_TCS230_data);
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_arg);

static void tcs230_setup_gpio(gpio_num_t s0_pin,
                              gpio_num_t s1_pin,
                              gpio_num_t s2_pin,
                              gpio_num_t s3_pin,
                              gpio_num_t gpio_output_en,
                              gpio_num_t gpio_pulse_output);

static void Extract_TCS230_Pulse_Period_func(gpio_num_t gpio_pulse_output, int32_t *Time_period);

static void Calculate_max_min_color_values(gpio_num_t gpio_output_en, gpio_num_t gpio_pulse_output, int32_t *least_color_timeP, int32_t *most_color_timeP);
static bool TCS230_set_filter_color(s_ezlopi_device_properties_t *properties, TCS230_color_enum_t color);
static bool TCS230_set_frequency_scaling(s_ezlopi_device_properties_t *properties, TCS230_freq_scaling_enum_t scale);

static int sensor_pwm_tcs230_init(s_ezlopi_device_properties_t *properties);
static int sensor_pwm_tcs230_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);

static int MAP_color_value(int x, int fromLow, int fromHigh, int toLow, int toHigh);
static void Get_mapped_color_value(uint32_t *color_value, gpio_num_t gpio_pulse_output, int32_t *period, int32_t min_time_limit, int32_t max_time_limit);
static bool get_tcs230_sensor_value(s_ezlopi_device_properties_t *properties);
static int sensor_0040_PWM_TCS230_update_values(s_ezlopi_device_properties_t *properties);

static void Gather_tcs230_Calibration_data(void *params);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int sensor_0040_PWM_TCS230(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        sensor_pwm_tcs230_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        sensor_pwm_tcs230_init(properties);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        sensor_pwm_tcs230_get_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        static uint8_t count = 0;
        if (calibration_complete)
        {
            // TRACE_B("..................Calibration Complete.................");
            sensor_0040_PWM_TCS230_update_values(properties);
            if (count > 5)
            {
                count = 0;
                // if greater than 3sec post the values to cloud
                ezlopi_device_value_updated_from_device(rgb_properties);
            }
            count++;
        }
        else
        {
            TRACE_B("..................Calibrating.................\n");
        }

        break;
    }
    default:
    {
        break;
    }
    }
    return 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_arg)
{
    int ret = 0;
    if (properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, properties, user_arg))
        {
            free(properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------
/*!
    @brief Configures Control GPIO pins (Ouput & Input) for TCS230.
*/
static void tcs230_setup_gpio(gpio_num_t s0_pin,
                              gpio_num_t s1_pin,
                              gpio_num_t s2_pin,
                              gpio_num_t s3_pin,
                              gpio_num_t gpio_output_en,
                              gpio_num_t gpio_pulse_output)
{
    esp_err_t ret = ESP_FAIL;
    // Configure GPIO ouput pins (S0, S1, S2, S3 & Freq_scale) for TCS230.
    gpio_config_t output_conf;
    output_conf.pin_bit_mask = (1ULL << s0_pin) | (1ULL << s1_pin) | (1ULL << s2_pin) | (1ULL << s3_pin) | (1ULL << gpio_output_en);
    output_conf.intr_type = GPIO_INTR_DISABLE;
    output_conf.mode = GPIO_MODE_OUTPUT;
    output_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    output_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ret |= gpio_config(&output_conf);

    // Configures GPIO input pins (Freq_Out_pin) for TCS230.
    gpio_config_t input_conf;
    input_conf.pin_bit_mask = (1ULL << gpio_pulse_output);
    input_conf.intr_type = GPIO_INTR_POSEDGE;
    input_conf.mode = GPIO_MODE_INPUT;
    input_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    input_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ret |= gpio_config(&input_conf);
    if (ESP_OK == ret)
    {
        TRACE_B("GPIO setup..... complete");
    }
    else
    {
        TRACE_B("GPIO setup..... failed");
    }
}

static bool TCS230_set_filter_color(s_ezlopi_device_properties_t *properties, TCS230_color_enum_t color_code)
{
    TCS230_data_t *sensor_0040_PWM_TCS230_data = (TCS230_data_t *)properties->user_arg;
    bool ret = false;
    switch (color_code)
    {
    case COLOR_SENSOR_COLOR_RED:
        // TRACE_E("Configuring gpio for red.");
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s2, 0));
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s3, 0));
        ret = true;
        break;
    case COLOR_SENSOR_COLOR_BLUE:
        // TRACE_E("Configuring gpio for blue.");
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s2, 0));
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s3, 1));
        ret = true;
        break;
    case COLOR_SENSOR_COLOR_GREEN:
        // TRACE_E("Configuring gpio for green.");
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s2, 1));
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s3, 1));
        ret = true;
        break;
    case COLOR_SENSOR_COLOR_CLEAR:
        // TRACE_E("Configuring gpio for no filter.");
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s2, 1));
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s3, 0));
        ret = true;
        break;
    default:
        ret = false;
        break;
    }
    return ret;
}

static bool TCS230_set_frequency_scaling(s_ezlopi_device_properties_t *properties, TCS230_freq_scaling_enum_t scale)
{
    TCS230_data_t *sensor_0040_PWM_TCS230_data = (TCS230_data_t *)properties->user_arg;
    bool ret = false;
    switch (scale)
    {
    case COLOR_SENSOR_FREQ_SCALING_POWER_DOWN:
        // TRACE_E("Configuring frequency Scaling to [Power Down]");
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s0, 0));
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s1, 0));
        ret = true;
        break;
    case COLOR_SENSOR_FREQ_SCALING_2_PERCENT:
        // TRACE_E("Configuring frequency Scaling to [2 percent]");
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s0, 0));
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s1, 1));
        ret = true;
        break;
    case COLOR_SENSOR_FREQ_SCALING_20_PERCENT:
        // TRACE_E("Configuring frequency Scaling to [20 percent]");
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s0, 1));
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s1, 0));
        ret = true;
        break;
    case COLOR_SENSOR_FREQ_SCALING_100_PERCENT:
        // TRACE_E("Configuring frequency Scaling to [100 percent]");
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s0, 1));
        ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s1, 1));
        ret = true;
        break;
    default:
        ret = false;
        break;
    }
    return ret;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// This function is used to get the time_period of incoming pulses in "freq_input pin". [So call 'gpio_install_isr_service()' before using this function]
static void Extract_TCS230_Pulse_Period_func(gpio_num_t gpio_pulse_output, int32_t *Time_period)
{
    // creating queue here
    tcs230_queue = xQueueCreate(QUEUE_SIZE, sizeof(int32_t)); // takes max -> 1mSec
    if (tcs230_queue)
    {
        // add -> gpio_isr_handle(pin_num)
        // TRACE_B("Queue_Available..... Adding Gpio_interrupt");
        gpio_isr_handler_add(gpio_pulse_output, gpio_isr_handler, NULL);

        // check queue_full => 1
        while (QueueFlag < TCS230_QUEUE_FULL)
        {
        }
        // disable -> gpio_isr_handle_remove(pin_num)
        // TRACE_B("Queue_full.....Removing Gpio_interrupt");
        gpio_isr_handler_remove(gpio_pulse_output);

        if (QueueFlag == TCS230_QUEUE_FULL)
        {
            // loop through all the queue[0-5] values -> active low instants
            int32_t prev_us_time = 0;
            int32_t us_time = 0;
            int32_t diff[QUEUE_SIZE] = {0}; // correct data starts from 1-9, not zero
            // extract the 10 queued values
            for (uint8_t i = 0; i < QUEUE_SIZE; i++)
            {
                if (xQueueReceive(tcs230_queue, &us_time, portMAX_DELAY))
                {
                    if ((us_time - prev_us_time) >= 0)
                    {
                        diff[i] = us_time - prev_us_time; // 0-9
                        prev_us_time = us_time;
                    }
                }
            }

            // generate frequency of occurance for Time-period from "diff[]" array
            uint8_t freq[QUEUE_SIZE] = {0};
            for (uint8_t x = 1; x < QUEUE_SIZE; x++)
            {
                for (uint8_t i = 1; i < QUEUE_SIZE; i++)
                {
                    float error = diff[x] - diff[i];
                    error = ((error >= 0) ? error : error * -1); // finding difference between two readings
                    if (error <= (diff[x] * 0.002))              // for Freq_scaling = 20% -> [error within => +-0.2%]
                    {
                        freq[x] += 1; // increment count
                    }
                }
            }
            // find the dominant period
            uint8_t max_freq_index = 0;
            int32_t dominant_val = 0;
            for (uint8_t i = 0; i < QUEUE_SIZE; i++)
            {
                if (freq[i] > dominant_val)
                {
                    dominant_val = freq[i];
                    max_freq_index = i;
                }
            }
            // TRACE_W("......................Dominant {%duS} => freq : %d", diff[max_freq_index], freq[max_freq_index]);

            // reset Queue_flag
            QueueFlag = TCS230_QUEUE_AVAILABLE;
            *Time_period = diff[max_freq_index];
        }
        // Deleting queue after no use to avoid conflicts
        vQueueDelete(tcs230_queue);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms delay
}

// function to calibrate the data for 30 seconds
static void Calculate_max_min_color_values(gpio_num_t gpio_output_en, gpio_num_t gpio_pulse_output, int32_t *least_color_timeP, int32_t *most_color_timeP)
{
    int32_t Period = 0;
    *least_color_timeP = 0;
    *most_color_timeP = 1000;
    for (uint8_t x = 0; x <= 100; x++) // 50ms * 100 = 10sec
    {
        if (x % 10 == 0)
        {
            TRACE_W(".....................................................%d", x);
        }
        //--------------------------------------------------
        ESP_ERROR_CHECK(gpio_set_level(gpio_output_en, 1));
        Extract_TCS230_Pulse_Period_func(gpio_pulse_output, &Period); // stalls for 10 queue to be filled // 10ms delay
        if (Period > (*least_color_timeP))
        {
            (*least_color_timeP) = Period; // less_red ->  larger_period [i.e. lower freq]
        }
        if (Period < (*most_color_timeP))
        {
            (*most_color_timeP) = Period; // more_red ->  smaller_period [i.e. higher freq]
        }
        ESP_ERROR_CHECK(gpio_set_level(gpio_output_en, 0));
        //--------------------------------------------------
        vTaskDelay(50 / portTICK_PERIOD_MS); // 50ms delay
    }
    if (((*least_color_timeP) != 0) || ((*most_color_timeP) != 1000))
    {
        TRACE_W("Calibration............. completed");
    }
    else
    {
        TRACE_E("Calibration............. failed");
    }
}

static void Gather_tcs230_Calibration_data(void *params) // calibration task
{
    vTaskDelay(4000 / portTICK_PERIOD_MS); // 4sec
    s_ezlopi_device_properties_t *properties = (s_ezlopi_device_properties_t *)params;

    // extracting the 'user_args' from "properties"
    TCS230_data_t *sensor_0040_PWM_TCS230_data = (TCS230_data_t *)properties->user_arg;

    //--------------------------------------------------
    // calculate red min-max periods for each colour
    TRACE_E("Please, place the red paper in front of colour sensor..... Starting Calibration for RED in ....");
    for (uint8_t j = 5; j > 0; j--)
    {
        TRACE_E("....................................................... {%d} ", j);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // 4sec
    }
    // choose  RED filter
    TCS230_set_filter_color(properties, COLOR_SENSOR_COLOR_RED);
    Calculate_max_min_color_values(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_output_en,
                                   sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_pulse_output,
                                   &sensor_0040_PWM_TCS230_data->calib_data.least_red_timeP,
                                   &sensor_0040_PWM_TCS230_data->calib_data.most_red_timeP);

    //--------------------------------------------------
    // calculate green min-max periods for each colour
    TRACE_I("Please, place the green paper in front of colour sensor..... Starting Calibration for GREEN in ....");
    for (uint8_t j = 5; j > 0; j--)
    {
        TRACE_I("....................................................... {%d} ", j);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // 4sec
    }
    // choose GREEN filter
    TCS230_set_filter_color(properties, COLOR_SENSOR_COLOR_GREEN);
    Calculate_max_min_color_values(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_output_en,
                                   sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_pulse_output,
                                   &sensor_0040_PWM_TCS230_data->calib_data.least_green_timeP,
                                   &sensor_0040_PWM_TCS230_data->calib_data.most_green_timeP);

    //--------------------------------------------------
    // calculate blue min-max periods for each colour
    TRACE_B("Please, place the blue paper in front of colour sensor..... Starting Calibration for BLUE in ....");
    for (uint8_t j = 5; j > 0; j--)
    {
        TRACE_B("....................................................... {%d} ", j);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // 4sec
    }
    // choose BLUE filter
    TCS230_set_filter_color(properties, COLOR_SENSOR_COLOR_BLUE);
    Calculate_max_min_color_values(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_output_en,
                                   sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_pulse_output,
                                   &sensor_0040_PWM_TCS230_data->calib_data.least_blue_timeP,
                                   &sensor_0040_PWM_TCS230_data->calib_data.most_blue_timeP);

    //--------------------------------------------------
    // show (LOW,HIGH) -> (max,min)
    TRACE_B("red(Least,Most) => red(%d,%d)", sensor_0040_PWM_TCS230_data->calib_data.least_red_timeP, sensor_0040_PWM_TCS230_data->calib_data.most_red_timeP);
    TRACE_B("green(Least,Most) => green(%d,%d)", sensor_0040_PWM_TCS230_data->calib_data.least_green_timeP, sensor_0040_PWM_TCS230_data->calib_data.most_green_timeP);
    TRACE_B("blue(Least,Most) => blue(%d,%d)", sensor_0040_PWM_TCS230_data->calib_data.least_blue_timeP, sensor_0040_PWM_TCS230_data->calib_data.most_blue_timeP);
    //--------------------------------------------------
    // set the calib flag
    calibration_complete = true;
    // delete this task
    vTaskDelete(NULL);
}

//------------------------------------------------------------------------------

static s_ezlopi_device_properties_t *sensor_pwm_tcs230_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, const char *VALUE_TYPE, cJSON *cjson_device, TCS230_data_t *sensor_0040_PWM_TCS230_data)
{
    s_ezlopi_device_properties_t *sensor_PWM_TCS230_properties = NULL;

    if ((NULL != cjson_device))
    {
        sensor_PWM_TCS230_properties = (s_ezlopi_device_properties_t *)malloc(sizeof(s_ezlopi_device_properties_t));
        if (sensor_PWM_TCS230_properties)
        {
            memset(sensor_PWM_TCS230_properties, 0, sizeof(s_ezlopi_device_properties_t));
            sensor_PWM_TCS230_properties->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;

            char *device_name = NULL;
            // "dev_name" : factory_info_h
            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);

            ASSIGN_DEVICE_NAME(sensor_PWM_TCS230_properties, device_name);
            sensor_PWM_TCS230_properties->ezlopi_cloud.category = CATEGORY;
            sensor_PWM_TCS230_properties->ezlopi_cloud.subcategory = SUB_CATEGORY;
            sensor_PWM_TCS230_properties->ezlopi_cloud.item_name = ITEM_NAME;
            sensor_PWM_TCS230_properties->ezlopi_cloud.device_type = dev_type_sensor;
            sensor_PWM_TCS230_properties->ezlopi_cloud.value_type = VALUE_TYPE;
            sensor_PWM_TCS230_properties->ezlopi_cloud.has_getter = true;
            sensor_PWM_TCS230_properties->ezlopi_cloud.has_setter = false;
            sensor_PWM_TCS230_properties->ezlopi_cloud.reachable = true;
            sensor_PWM_TCS230_properties->ezlopi_cloud.battery_powered = false;
            sensor_PWM_TCS230_properties->ezlopi_cloud.show = true;
            sensor_PWM_TCS230_properties->ezlopi_cloud.room_name[0] = '\0';
            sensor_PWM_TCS230_properties->ezlopi_cloud.device_id = DEVICE_ID;
            sensor_PWM_TCS230_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            sensor_PWM_TCS230_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

            CJSON_GET_VALUE_INT(cjson_device, "gpio_s0", sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s0);
            CJSON_GET_VALUE_INT(cjson_device, "gpio_s1", sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s1);
            CJSON_GET_VALUE_INT(cjson_device, "gpio_s2", sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s2);
            CJSON_GET_VALUE_INT(cjson_device, "gpio_s3", sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s3);
            CJSON_GET_VALUE_INT(cjson_device, "gpio_output_en", sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_output_en);
            CJSON_GET_VALUE_INT(cjson_device, "gpio_pulse_output", sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_pulse_output);

            sensor_PWM_TCS230_properties->user_arg = sensor_0040_PWM_TCS230_data; // structure containing calib_factors & data_val
        }
    }
    return sensor_PWM_TCS230_properties;
}

static int sensor_pwm_tcs230_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg; // create a ' TCS230_data_t ' type dummy pointer
    TCS230_data_t *sensor_0040_PWM_TCS230_data = (TCS230_data_t *)malloc(sizeof(TCS230_data_t));

    if ((NULL != prep_arg) && (NULL != prep_arg->cjson_device) && (NULL != sensor_0040_PWM_TCS230_data))
    {
        memset(sensor_0040_PWM_TCS230_data, 0, sizeof(TCS230_data_t));
        uint32_t device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(rgb_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_rgbcolor, value_type_rgb, prep_arg->cjson_device, sensor_0040_PWM_TCS230_data);
    }
    return ret;
}

static int sensor_pwm_tcs230_init(s_ezlopi_device_properties_t *properties)
{
    static bool guard = false;
    if (!guard)
    {
        guard = true;
        TCS230_data_t *sensor_0040_PWM_TCS230_data = (TCS230_data_t *)properties->user_arg;
        if (NULL != sensor_0040_PWM_TCS230_data)
        {
            tcs230_setup_gpio(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s0,
                              sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s1,
                              sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s2,
                              sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_s3,
                              sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_output_en,
                              sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_pulse_output);
            TRACE_W("Entering Calibration Phase for 30 seconds.....");

            // configure Freq_scale at 20%
            TCS230_set_frequency_scaling(properties, COLOR_SENSOR_FREQ_SCALING_20_PERCENT);

            // activate a task to calibrate data
            xTaskCreate(Gather_tcs230_Calibration_data, "TCS230_Calibration_Task", 2 * 2048, properties, 1, NULL);
        }
    }
    return (int)guard;
}

//------------------------------------------------------------------------------

static int sensor_pwm_tcs230_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)args;
    TCS230_data_t *sensor_0040_PWM_TCS230_data = (TCS230_data_t *)properties->user_arg;

    if (cjson_properties && sensor_0040_PWM_TCS230_data)
    {
        if (ezlopi_item_name_rgbcolor == properties->ezlopi_cloud.item_name)
        {
            // TRACE_I("Red : %d", (sensor_0040_PWM_TCS230_data->red_mapped));
            // TRACE_I("Blue : %d", (sensor_0040_PWM_TCS230_data->blue_mapped));
            // TRACE_I("Green : %d", (sensor_0040_PWM_TCS230_data->green_mapped));

            cJSON *Color_Values = cJSON_AddObjectToObject(cjson_properties, "value");
            cJSON_AddNumberToObject(Color_Values, "red", sensor_0040_PWM_TCS230_data->red_mapped);
            cJSON_AddNumberToObject(Color_Values, "green", sensor_0040_PWM_TCS230_data->green_mapped);
            cJSON_AddNumberToObject(Color_Values, "blue", sensor_0040_PWM_TCS230_data->blue_mapped);
        }
        ret = 1;
    }
    return ret;
}

//------------------------------------------------------------------------------

// this is Map Function
static int MAP_color_value(int x, int fromLow, int fromHigh, int toLow, int toHigh)
{

    return (int)(((float)(fromHigh - x) / (float)(fromHigh - fromLow)) * (float)toHigh);
}

// Generate the color value (0-255) from time_period_us
static void Get_mapped_color_value(uint32_t *color_value, gpio_num_t gpio_pulse_output, int32_t *period, int32_t min_time_limit, int32_t max_time_limit)
{
    // first populate the variable pointed by 'period' ptr
    Extract_TCS230_Pulse_Period_func(gpio_pulse_output, period); // stalls for 10 queue to be filled

    // setting limits to avoid error during calculations
    if (*period < min_time_limit) // if 'period' has smaller time_periods than min-limit [i.e. 'most_red_timeP']
    {
        *period = min_time_limit; // smaller time period
    }
    if (*period > max_time_limit) // if 'period' has larger time_periods than max-limit [i.e. 'least_red_timeP']
    {
        *period = max_time_limit; // larger time period
    }
    // mapping function (x ,smaller_time_period, larger_time_period, 0,255)
    // assign the mapped value to structure
    *(color_value) = MAP_color_value(*period, min_time_limit, max_time_limit, 0, 255);
}

static bool get_tcs230_sensor_value(s_ezlopi_device_properties_t *properties)
{
    // 'void_type' addrress -> 'TCS230_data_t' address
    TCS230_data_t *sensor_0040_PWM_TCS230_data = (TCS230_data_t *)properties->user_arg;
    ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_output_en, 1));

    //--------------------------------------------------
    int32_t Red_period = 0;
    TCS230_set_filter_color(properties, COLOR_SENSOR_COLOR_RED);
    Extract_TCS230_Pulse_Period_func(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_pulse_output,
                                     &Red_period);
    Get_mapped_color_value(&sensor_0040_PWM_TCS230_data->red_mapped,                  // dest_var
                           sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_pulse_output, // use this pin to populate src_var
                           &Red_period,                                               // src_var
                           sensor_0040_PWM_TCS230_data->calib_data.most_red_timeP,    // calib paramter
                           sensor_0040_PWM_TCS230_data->calib_data.least_red_timeP);  // calib paramter
    TRACE_E("RED => %d....", sensor_0040_PWM_TCS230_data->red_mapped);
    //--------------------------------------------------

    int32_t Green_period = 0;
    TCS230_set_filter_color(properties, COLOR_SENSOR_COLOR_GREEN);
    Extract_TCS230_Pulse_Period_func(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_pulse_output,
                                     &Green_period);
    Get_mapped_color_value(&sensor_0040_PWM_TCS230_data->green_mapped,
                           sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_pulse_output,
                           &Green_period,
                           sensor_0040_PWM_TCS230_data->calib_data.most_green_timeP,
                           sensor_0040_PWM_TCS230_data->calib_data.least_green_timeP);
    TRACE_I("GREEN => %d....", sensor_0040_PWM_TCS230_data->green_mapped);
    //--------------------------------------------------

    int32_t Blue_period = 0;
    TCS230_set_filter_color(properties, COLOR_SENSOR_COLOR_BLUE);
    Extract_TCS230_Pulse_Period_func(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_pulse_output,
                                     &Blue_period);
    Get_mapped_color_value(&sensor_0040_PWM_TCS230_data->blue_mapped,
                           sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_pulse_output,
                           &Blue_period,
                           sensor_0040_PWM_TCS230_data->calib_data.most_blue_timeP,
                           sensor_0040_PWM_TCS230_data->calib_data.least_blue_timeP);
    TRACE_B("\t\t BLUE => %d....", sensor_0040_PWM_TCS230_data->blue_mapped);
    //--------------------------------------------------

    ESP_ERROR_CHECK(gpio_set_level(sensor_0040_PWM_TCS230_data->TCS230_pin.gpio_output_en, 0));
    TRACE_B("------------------------------------------------------");
    return true;
}

static int sensor_0040_PWM_TCS230_update_values(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    // 'void_type' addrress -> 'TCS230_data_t' address
    TCS230_data_t *sensor_0040_PWM_TCS230_data = (TCS230_data_t *)properties->user_arg;

    if (NULL != properties)
    {
        bool valid_status = get_tcs230_sensor_value(properties); // Informs and updates if valid data
        if (valid_status)
        {
            // now using the data_stored within "USER_ARG"
#if 0
            TRACE_I("---------------------------------------");
            TRACE_I("Red : %d", sensor_0040_PWM_TCS230_data->red_mapped);
            TRACE_I("Green :%d", sensor_0040_PWM_TCS230_data->green_mapped);
            TRACE_I("Blue : %d", sensor_0040_PWM_TCS230_data->blue_mapped);
            TRACE_I("---------------------------------------");
#endif
        }
    }
    return ret;
}