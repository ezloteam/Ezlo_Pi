#include <string.h>
#include <math.h>
#include "ezlopi_util_trace.h"

#include "freertos/task.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0040_other_TCS230.h"

static int __0040_prepare(void* arg);
static int __0040_init(l_ezlopi_item_t* item);
static int __0040_get_cjson_value(l_ezlopi_item_t* item, void* arg);
static int __0040_notify(l_ezlopi_item_t* item);
static void __tcs230_setup_gpio(gpio_num_t s0_pin, gpio_num_t s1_pin, gpio_num_t s2_pin, gpio_num_t s3_pin, gpio_num_t gpio_output_en, gpio_num_t gpio_pulse_output);

static void __tcs230_calibration_task(void* params);

//------------------------------------------------------------------------------------------------------
int sensor_0040_other_TCS230(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
        __0040_prepare(arg);
        break;
    case EZLOPI_ACTION_INITIALIZE:
        __0040_init(item);
        break;
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
        __0040_get_cjson_value(item, arg);
        break;
    case EZLOPI_ACTION_NOTIFY_1000_MS:
        __0040_notify(item);
        break;

    default:
        break;
    }
    return ret;
}

//------------------------------------------------------------------------------------------------------
static void __tcs230_setup_gpio(gpio_num_t s0_pin,
    gpio_num_t s1_pin,
    gpio_num_t s2_pin,
    gpio_num_t s3_pin,
    gpio_num_t gpio_output_en,
    gpio_num_t gpio_pulse_output)
{
    esp_err_t ret = ESP_FAIL;
    // Configure GPIO ouput pins (S0, S1, S2, S3 & Freq_scale) for TCS230.
    gpio_config_t output_conf;
    output_conf.pin_bit_mask = ((1ULL << s0_pin) | (1ULL << s1_pin) | (1ULL << s2_pin) | (1ULL << s3_pin) | (1ULL << gpio_output_en));
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
        TRACE_I("GPIO setup..... complete");
    }
    else
    {
        TRACE_I("GPIO setup..... failed");
    }
}

//------------------------------------------------------------------------------------------------------
static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    char* device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    device->cloud_properties.category = category_generic_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type = dev_type_sensor;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, void* user_data)
{
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_rgbcolor;
    item->cloud_properties.value_type = value_type_rgb;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->user_arg = user_data;
}

static void __prepare_item_interface_properties(l_ezlopi_item_t* item, cJSON* cj_device)
{
    if (item && cj_device)
    {
        s_TCS230_data_t* user_data = (s_TCS230_data_t*)item->user_arg;
        item->interface_type = EZLOPI_DEVICE_INTERFACE_MAX;
        CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio1_str, user_data->TCS230_pin.gpio_s0);           // gpio_s0
        CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio2_str, user_data->TCS230_pin.gpio_s1);           // gpio_s1
        CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio3_str, user_data->TCS230_pin.gpio_s2);           // gpio_s2
        CJSON_GET_VALUE_INT(cj_device, "gpio4", user_data->TCS230_pin.gpio_s3);           // gpio_s3
        CJSON_GET_VALUE_INT(cj_device, "gpio5", user_data->TCS230_pin.gpio_output_en);    // gpio_output_en
        CJSON_GET_VALUE_INT(cj_device, "gpio6", user_data->TCS230_pin.gpio_pulse_output); // gpio_pulse_output
    }
}
//------------------------------------------------------------------------------------------------------
static int __0040_prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* device_prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        cJSON* cj_device = device_prep_arg->cjson_device;
        s_TCS230_data_t* user_data = (s_TCS230_data_t*)malloc(sizeof(s_TCS230_data_t));
        if (user_data)
        {
            l_ezlopi_device_t* tcs230_device = ezlopi_device_add_device(cj_device);
            if (tcs230_device)
            {
                __prepare_device_cloud_properties(tcs230_device, cj_device);
                l_ezlopi_item_t* tcs230_item = ezlopi_device_add_item_to_device(tcs230_device, sensor_0040_other_TCS230);
                if (tcs230_item)
                {
                    __prepare_item_cloud_properties(tcs230_item, user_data);
                    __prepare_item_interface_properties(tcs230_item, cj_device);
                    ret = 1;
                }
                else
                {
                    ret = -1;
                    ezlopi_device_free_device(tcs230_device);
                    free(user_data);
                }
            }
            else
            {
                ret = -1;
                free(user_data);
            }
        }
    }
    return ret;
}

static int __0040_init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        s_TCS230_data_t* user_data = (s_TCS230_data_t*)item->user_arg;
        if (user_data)
        {
            if (GPIO_IS_VALID_GPIO(user_data->TCS230_pin.gpio_s0) &&
                GPIO_IS_VALID_GPIO(user_data->TCS230_pin.gpio_s1) &&
                GPIO_IS_VALID_GPIO(user_data->TCS230_pin.gpio_s2) &&
                GPIO_IS_VALID_GPIO(user_data->TCS230_pin.gpio_s3) &&
                GPIO_IS_VALID_GPIO(user_data->TCS230_pin.gpio_output_en) &&
                GPIO_IS_VALID_GPIO(user_data->TCS230_pin.gpio_pulse_output))
            {
                __tcs230_setup_gpio(user_data->TCS230_pin.gpio_s0,
                    user_data->TCS230_pin.gpio_s1,
                    user_data->TCS230_pin.gpio_s2,
                    user_data->TCS230_pin.gpio_s3,
                    user_data->TCS230_pin.gpio_output_en,
                    user_data->TCS230_pin.gpio_pulse_output);
                TRACE_W("Entering Calibration Phase for 30 seconds.....");

                // configure Freq_scale at 20%
                tcs230_set_frequency_scaling(item, COLOR_SENSOR_FREQ_SCALING_20_PERCENT);

                // activate a task to calibrate data
                xTaskCreate(__tcs230_calibration_task, "TCS230_Calibration_Task", 2 * 2048, item, 1, NULL);
                ret = 1;
            }
            // else
            // {
            //     // ret = -1;
            //     // free(item->user_arg); // this will free ; memory address linked to all items
            //     // item->user_arg = NULL;
            //     // ezlopi_device_free_device_by_item(item);
            // }
        }
        // else
        // {
        //     ret = -1;
        //     ezlopi_device_free_device_by_item(item);
        // }
    }
    return ret;
}

static int __0040_get_cjson_value(l_ezlopi_item_t* item, void* args)
{
    int ret = 0;
    cJSON* cj_result = (cJSON*)args;
    if (cj_result && item)
    {
        s_TCS230_data_t* user_data = (s_TCS230_data_t*)item->user_arg;
        if (user_data)
        {
            if (ezlopi_item_name_rgbcolor == item->cloud_properties.item_name)
            {
                cJSON* color_values = cJSON_AddObjectToObject(cj_result, ezlopi_value_str);
                cJSON_AddNumberToObject(color_values, "red", user_data->red_mapped);
                cJSON_AddNumberToObject(color_values, "green", user_data->green_mapped);
                cJSON_AddNumberToObject(color_values, "blue", user_data->blue_mapped);
                char* valueFormatted = ezlopi_valueformatter_rgb(user_data->red_mapped, user_data->green_mapped, user_data->blue_mapped);
                if (valueFormatted)
                {
                    cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
                    free(valueFormatted);
                }
            }
            ret = 1;
        }
    }
    return ret;
}

static int __0040_notify(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        s_TCS230_data_t* user_data = (s_TCS230_data_t*)item->user_arg;
        if (user_data)
        {
            uint32_t red = user_data->red_mapped;
            uint32_t green = user_data->green_mapped;
            uint32_t blue = user_data->blue_mapped;
            bool valid_status = get_tcs230_sensor_value(item); // Informs and updates if valid data
            if (valid_status)
            {
                if (fabs(red - user_data->red_mapped) > 10 ||
                    fabs(green - user_data->green_mapped) > 10 ||
                    fabs(blue - user_data->blue_mapped) > 10)
                {
                    TRACE_S("---------------------------------------");
                    TRACE_S("Red : %d", user_data->red_mapped);
                    TRACE_S("Green :%d", user_data->green_mapped);
                    TRACE_S("Blue : %d", user_data->blue_mapped);
                    TRACE_S("---------------------------------------");
                    ezlopi_device_value_updated_from_device_v3(item);
                }
            }
        }
    }
    return ret;
}

//------------------------------------------------------------------------------
static void __tcs230_calibration_task(void* params) // calibration task
{
    vTaskDelay(4000 / portTICK_PERIOD_MS); // 4sec
    l_ezlopi_item_t* item = (l_ezlopi_item_t*)params;
    if (item)
    { // extracting the 'user_args' from "item"
        s_TCS230_data_t* user_data = (s_TCS230_data_t*)item->user_arg;
        if (user_data)
        {
#if 0
            //--------------------------------------------------
            // calculate red min-max periods for each colour
            TRACE_E("Please, place the red paper in front of colour sensor..... Starting Calibration for RED in ....");
            for (uint8_t j = 5; j > 0; j--)
            {
                TRACE_E("....................................................... {%d} ", j);
                vTaskDelay(1000 / portTICK_PERIOD_MS); // 4sec
            }
            // choose  RED filter
            tcs230_set_filter_color(item, COLOR_SENSOR_COLOR_RED);
            calculate_max_min_color_values(user_data->TCS230_pin.gpio_output_en,
                user_data->TCS230_pin.gpio_pulse_output,
                &user_data->calib_data.least_red_timeP,
                &user_data->calib_data.most_red_timeP);

            //--------------------------------------------------
            // calculate green min-max periods for each colour
            TRACE_S("Please, place the green paper in front of colour sensor..... Starting Calibration for GREEN in ....");
            for (uint8_t j = 5; j > 0; j--)
            {
                TRACE_S("....................................................... {%d} ", j);
                vTaskDelay(1000 / portTICK_PERIOD_MS); // 4sec
            }
            // choose GREEN filter
            tcs230_set_filter_color(item, COLOR_SENSOR_COLOR_GREEN);
            calculate_max_min_color_values(user_data->TCS230_pin.gpio_output_en,
                user_data->TCS230_pin.gpio_pulse_output,
                &user_data->calib_data.least_green_timeP,
                &user_data->calib_data.most_green_timeP);

            //--------------------------------------------------
            // calculate blue min-max periods for each colour
            TRACE_I("Please, place the blue paper in front of colour sensor..... Starting Calibration for BLUE in ....");
            for (uint8_t j = 5; j > 0; j--)
            {
                TRACE_I("....................................................... {%d} ", j);
                vTaskDelay(1000 / portTICK_PERIOD_MS); // 4sec
            }
            // choose BLUE filter
            tcs230_set_filter_color(item, COLOR_SENSOR_COLOR_BLUE);
            calculate_max_min_color_values(user_data->TCS230_pin.gpio_output_en,
                user_data->TCS230_pin.gpio_pulse_output,
                &user_data->calib_data.least_blue_timeP,
                &user_data->calib_data.most_blue_timeP);

            //--------------------------------------------------
            // show (LOW,HIGH) -> (max,min)
#endif

            user_data->calib_data.least_red_timeP = 120; /*Defaults*/
            user_data->calib_data.most_red_timeP = 48;
            user_data->calib_data.least_green_timeP = 109;
            user_data->calib_data.most_green_timeP = 86;
            user_data->calib_data.least_blue_timeP = 120;
            user_data->calib_data.most_blue_timeP = 78;

            TRACE_I("red(Least,Most) => red(%d,%d)", user_data->calib_data.least_red_timeP, user_data->calib_data.most_red_timeP);
            TRACE_I("green(Least,Most) => green(%d,%d)", user_data->calib_data.least_green_timeP, user_data->calib_data.most_green_timeP);
            TRACE_I("blue(Least,Most) => blue(%d,%d)", user_data->calib_data.least_blue_timeP, user_data->calib_data.most_blue_timeP);
            //--------------------------------------------------
            // set the calib flag
            user_data->calibration_complete = true;
        }
    }
    vTaskDelete(NULL);
}
//------------------------------------------------------------------------------