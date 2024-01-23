#include "ezlopi_util_trace.h"
#include "freertos/queue.h"
#include "sensor_0040_other_TCS230.h"

#define TCS230_QUEUE_SIZE 5

static QueueHandle_t tcs230_queue = NULL;
static e_TCS230_queue_t QueueFlag = TCS230_QUEUE_RESET;
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

static void Extract_TCS230_Pulse_Period_func(gpio_num_t gpio_pulse_output, int32_t *Time_period);
static int MAP_color_value(int x, int fromLow, int fromHigh, int toLow, int toHigh);
static void Get_mapped_color_value(uint32_t *color_value, gpio_num_t gpio_pulse_output, int32_t *period, int32_t min_time_limit, int32_t max_time_limit);
//------------------------------------------------------------------------------

// This function is used to get the time_period of incoming pulses in "freq_input pin". [So call 'gpio_install_isr_service()' before using this function]
static void Extract_TCS230_Pulse_Period_func(gpio_num_t gpio_pulse_output, int32_t *Time_period)
{
    // creating queue here
    tcs230_queue = xQueueCreate(TCS230_QUEUE_SIZE, sizeof(int32_t)); // takes max -> 1mSec
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
            int32_t diff[TCS230_QUEUE_SIZE] = {0}; // correct data starts from 1-9, not zero
            // extract the 10 queued values
            for (uint8_t i = 0; i < TCS230_QUEUE_SIZE; i++)
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
            uint8_t freq[TCS230_QUEUE_SIZE] = {0};
            for (uint8_t x = 1; x < TCS230_QUEUE_SIZE; x++)
            {
                for (uint8_t i = 1; i < TCS230_QUEUE_SIZE; i++)
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
            for (uint8_t i = 0; i < TCS230_QUEUE_SIZE; i++)
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

bool tcs230_set_filter_color(l_ezlopi_item_t *item, e_TCS230_color_t color_code)
{
    bool ret = false;
    if (item)
    {
        s_TCS230_data_t *_TCS230_user_data = (s_TCS230_data_t *)item->user_arg;
        switch (color_code)
        {
        case COLOR_SENSOR_COLOR_RED:
            // TRACE_E("Configuring gpio for red.");
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s2, 0));
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s3, 0));
            ret = true;
            break;
        case COLOR_SENSOR_COLOR_BLUE:
            // TRACE_E("Configuring gpio for blue.");
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s2, 0));
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s3, 1));
            ret = true;
            break;
        case COLOR_SENSOR_COLOR_GREEN:
            // TRACE_E("Configuring gpio for green.");
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s2, 1));
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s3, 1));
            ret = true;
            break;
        case COLOR_SENSOR_COLOR_CLEAR:
            // TRACE_E("Configuring gpio for no filter.");
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s2, 1));
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s3, 0));
            ret = true;
            break;
        default:
            ret = false;
            break;
        }
    }
    return ret;
}

bool tcs230_set_frequency_scaling(l_ezlopi_item_t *item, e_TCS230_freq_scaling_t scale)
{
    bool ret = false;
    if (item)
    {
        s_TCS230_data_t *_TCS230_user_data = (s_TCS230_data_t *)item->user_arg;
        switch (scale)
        {
        case COLOR_SENSOR_FREQ_SCALING_POWER_DOWN:
            // TRACE_E("Configuring frequency Scaling to [Power Down]");
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s0, 0));
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s1, 0));
            ret = true;
            break;
        case COLOR_SENSOR_FREQ_SCALING_2_PERCENT:
            // TRACE_E("Configuring frequency Scaling to [2 percent]");
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s0, 0));
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s1, 1));
            ret = true;
            break;
        case COLOR_SENSOR_FREQ_SCALING_20_PERCENT:
            // TRACE_E("Configuring frequency Scaling to [20 percent]");
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s0, 1));
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s1, 0));
            ret = true;
            break;
        case COLOR_SENSOR_FREQ_SCALING_100_PERCENT:
            // TRACE_E("Configuring frequency Scaling to [100 percent]");
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s0, 1));
            ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_s1, 1));
            ret = true;
            break;
        default:
            ret = false;
            break;
        }
    }
    return ret;
}
#if 0
// function to calibrate the data for 30 seconds
void calculate_max_min_color_values(gpio_num_t gpio_output_en, gpio_num_t gpio_pulse_output, int32_t *least_color_timeP, int32_t *most_color_timeP)
{
    int32_t Period = 0;
    *least_color_timeP = 0;
    *most_color_timeP = 1000;
    for (uint8_t x = 0; x <= 100; x++) // 50ms * 100 = 10sec
    {
        // if (x % 10 == 0)
        // {
        //     TRACE_W(".....................................................%d", x);
        // }
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
#endif

bool get_tcs230_sensor_value(l_ezlopi_item_t *item)
{
    if (item)
    { // 'void_type' addrress -> 's_TCS230_data_t' address
        s_TCS230_data_t *_TCS230_user_data = (s_TCS230_data_t *)item->user_arg;
        ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_output_en, 1));

        //--------------------------------------------------
        int32_t Red_period = 0;
        tcs230_set_filter_color(item, COLOR_SENSOR_COLOR_RED);
        Extract_TCS230_Pulse_Period_func(_TCS230_user_data->TCS230_pin.gpio_pulse_output,
                                         &Red_period);
        Get_mapped_color_value(&_TCS230_user_data->red_mapped,                  // dest_var
                               _TCS230_user_data->TCS230_pin.gpio_pulse_output, // use this pin to populate src_var
                               &Red_period,                                     // src_var
                               _TCS230_user_data->calib_data.most_red_timeP,    // calib paramter
                               _TCS230_user_data->calib_data.least_red_timeP);  // calib paramter
        // TRACE_E("RED => %d....", _TCS230_user_data->red_mapped);
        //--------------------------------------------------

        int32_t Green_period = 0;
        tcs230_set_filter_color(item, COLOR_SENSOR_COLOR_GREEN);
        Extract_TCS230_Pulse_Period_func(_TCS230_user_data->TCS230_pin.gpio_pulse_output,
                                         &Green_period);
        Get_mapped_color_value(&_TCS230_user_data->green_mapped, // dest_var
                               _TCS230_user_data->TCS230_pin.gpio_pulse_output,
                               &Green_period,
                               _TCS230_user_data->calib_data.most_green_timeP,
                               _TCS230_user_data->calib_data.least_green_timeP);
        // TRACE_I("GREEN => %d....", _TCS230_user_data->green_mapped);
        //--------------------------------------------------

        int32_t Blue_period = 0;
        tcs230_set_filter_color(item, COLOR_SENSOR_COLOR_BLUE);
        Extract_TCS230_Pulse_Period_func(_TCS230_user_data->TCS230_pin.gpio_pulse_output,
                                         &Blue_period);
        Get_mapped_color_value(&_TCS230_user_data->blue_mapped, // dest_var
                               _TCS230_user_data->TCS230_pin.gpio_pulse_output,
                               &Blue_period,
                               _TCS230_user_data->calib_data.most_blue_timeP,
                               _TCS230_user_data->calib_data.least_blue_timeP);
        // TRACE_B("\t\t BLUE => %d....", _TCS230_user_data->blue_mapped);
        //--------------------------------------------------

        ESP_ERROR_CHECK(gpio_set_level(_TCS230_user_data->TCS230_pin.gpio_output_en, 0));
        // TRACE_B("------------------------------------------------------");
    }
    return true;
}