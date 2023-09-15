#include "trace.h"
#include "esp_err.h"
#include "math.h"
#include "stdbool.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_timer.h"
#include "cJSON.h"
#include "driver/gpio.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"

#include "sensor_0040_other_TCS230.h"
#include "TCS230.h"

/*************************************************************************************************/
/*                              DEFINES                                                     */
/*************************************************************************************************/

static bool calibration_complete = false;

//------------------------------------------------------------------------------
static s_ezlopi_device_properties_t *rgb_properties = NULL;
//------------------------------------------------------------------------------
#define ADD_PROPERTIES_DEVICE_LIST(_properties, device_id, category, subcategory, item_name, value_type, cjson_device, sensor_0040_other_TCS230_data) \
    {                                                                                                                                                 \
        _properties = sensor_other_tcs230_prepare_properties(device_id, category, subcategory, item_name,                                             \
                                                             value_type, cjson_device, sensor_0040_other_TCS230_data);                                \
        if (NULL != _properties)                                                                                                                      \
        {                                                                                                                                             \
            add_device_to_list(prep_arg, _properties, NULL);                                                                                          \
        }                                                                                                                                             \
    }
//------------------------------------------------------------------------------
static int sensor_other_tcs230_prepare(void *arg);
static s_ezlopi_device_properties_t *sensor_other_tcs230_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY,
                                                                            const char *ITEM_NAME, const char *VALUE_TYPE,
                                                                            cJSON *cjson_device, TCS230_data_t *sensor_0040_other_TCS230_data);
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_arg);

static void tcs230_setup_gpio(gpio_num_t s0_pin,
                              gpio_num_t s1_pin,
                              gpio_num_t s2_pin,
                              gpio_num_t s3_pin,
                              gpio_num_t gpio_output_en,
                              gpio_num_t gpio_pulse_output);

static int sensor_other_tcs230_init(s_ezlopi_device_properties_t *properties);
static int sensor_other_tcs230_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);
static int sensor_0040_other_TCS230_update_values(s_ezlopi_device_properties_t *properties);
static void Gather_tcs230_Calibration_data(void *params);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int sensor_0040_other_TCS230(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        sensor_other_tcs230_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        sensor_other_tcs230_init(properties);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        sensor_other_tcs230_get_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        static uint8_t count = 0;
        if (calibration_complete)
        {
            // TRACE_B("..................Calibration Complete.................");
            sensor_0040_other_TCS230_update_values(properties);
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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static void Gather_tcs230_Calibration_data(void *params) // calibration task
{
    vTaskDelay(4000 / portTICK_PERIOD_MS); // 4sec
    s_ezlopi_device_properties_t *properties = (s_ezlopi_device_properties_t *)params;

    // extracting the 'user_args' from "properties"
    TCS230_data_t *sensor_0040_other_TCS230_data = (TCS230_data_t *)properties->user_arg;

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
			    TCS230_set_filter_color(properties, COLOR_SENSOR_COLOR_RED);
			    Calculate_max_min_color_values(sensor_0040_other_TCS230_data->TCS230_pin.gpio_output_en,
						           sensor_0040_other_TCS230_data->TCS230_pin.gpio_pulse_output,
						           &sensor_0040_other_TCS230_data->calib_data.least_red_timeP,
						           &sensor_0040_other_TCS230_data->calib_data.most_red_timeP);

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
			    Calculate_max_min_color_values(sensor_0040_other_TCS230_data->TCS230_pin.gpio_output_en,
						           sensor_0040_other_TCS230_data->TCS230_pin.gpio_pulse_output,
						           &sensor_0040_other_TCS230_data->calib_data.least_green_timeP,
						           &sensor_0040_other_TCS230_data->calib_data.most_green_timeP);

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
			    Calculate_max_min_color_values(sensor_0040_other_TCS230_data->TCS230_pin.gpio_output_en,
						           sensor_0040_other_TCS230_data->TCS230_pin.gpio_pulse_output,
						           &sensor_0040_other_TCS230_data->calib_data.least_blue_timeP,
						           &sensor_0040_other_TCS230_data->calib_data.most_blue_timeP);

			    //--------------------------------------------------
			    // show (LOW,HIGH) -> (max,min)
#endif

    sensor_0040_other_TCS230_data->calib_data.least_red_timeP = 120; /*Defaults*/
    sensor_0040_other_TCS230_data->calib_data.most_red_timeP = 48;
    sensor_0040_other_TCS230_data->calib_data.least_green_timeP = 109;
    sensor_0040_other_TCS230_data->calib_data.most_green_timeP = 86;
    sensor_0040_other_TCS230_data->calib_data.least_blue_timeP = 120;
    sensor_0040_other_TCS230_data->calib_data.most_blue_timeP = 78;

    TRACE_B("red(Least,Most) => red(%d,%d)", sensor_0040_other_TCS230_data->calib_data.least_red_timeP, sensor_0040_other_TCS230_data->calib_data.most_red_timeP);
    TRACE_B("green(Least,Most) => green(%d,%d)", sensor_0040_other_TCS230_data->calib_data.least_green_timeP, sensor_0040_other_TCS230_data->calib_data.most_green_timeP);
    TRACE_B("blue(Least,Most) => blue(%d,%d)", sensor_0040_other_TCS230_data->calib_data.least_blue_timeP, sensor_0040_other_TCS230_data->calib_data.most_blue_timeP);
    //--------------------------------------------------
    // set the calib flag
    calibration_complete = true;
    // delete this task
    vTaskDelete(NULL);
}
//------------------------------------------------------------------------------

static s_ezlopi_device_properties_t *sensor_other_tcs230_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, const char *VALUE_TYPE, cJSON *cjson_device, TCS230_data_t *sensor_0040_other_TCS230_data)
{
    s_ezlopi_device_properties_t *sensor_other_TCS230_properties = NULL;

    if ((NULL != cjson_device))
    {
        sensor_other_TCS230_properties = (s_ezlopi_device_properties_t *)malloc(sizeof(s_ezlopi_device_properties_t));
        if (sensor_other_TCS230_properties)
        {
            memset(sensor_other_TCS230_properties, 0, sizeof(s_ezlopi_device_properties_t));
            sensor_other_TCS230_properties->interface_type = EZLOPI_DEVICE_INTERFACE_MAX;

            char *device_name = NULL;
            // "dev_name" : factory_info_h
            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);

            ASSIGN_DEVICE_NAME(sensor_other_TCS230_properties, device_name);
            sensor_other_TCS230_properties->ezlopi_cloud.category = CATEGORY;
            sensor_other_TCS230_properties->ezlopi_cloud.subcategory = SUB_CATEGORY;
            sensor_other_TCS230_properties->ezlopi_cloud.item_name = ITEM_NAME;
            sensor_other_TCS230_properties->ezlopi_cloud.device_type = dev_type_sensor;
            sensor_other_TCS230_properties->ezlopi_cloud.value_type = VALUE_TYPE;
            sensor_other_TCS230_properties->ezlopi_cloud.has_getter = true;
            sensor_other_TCS230_properties->ezlopi_cloud.has_setter = false;
            sensor_other_TCS230_properties->ezlopi_cloud.reachable = true;
            sensor_other_TCS230_properties->ezlopi_cloud.battery_powered = false;
            sensor_other_TCS230_properties->ezlopi_cloud.show = true;
            sensor_other_TCS230_properties->ezlopi_cloud.room_name[0] = '\0';
            sensor_other_TCS230_properties->ezlopi_cloud.device_id = DEVICE_ID;
            sensor_other_TCS230_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            sensor_other_TCS230_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

            CJSON_GET_VALUE_INT(cjson_device, "gpio1", sensor_0040_other_TCS230_data->TCS230_pin.gpio_s0);           // gpio_s0
            CJSON_GET_VALUE_INT(cjson_device, "gpio2", sensor_0040_other_TCS230_data->TCS230_pin.gpio_s1);           // gpio_s1
            CJSON_GET_VALUE_INT(cjson_device, "gpio3", sensor_0040_other_TCS230_data->TCS230_pin.gpio_s2);           // gpio_s2
            CJSON_GET_VALUE_INT(cjson_device, "gpio4", sensor_0040_other_TCS230_data->TCS230_pin.gpio_s3);           // gpio_s3
            CJSON_GET_VALUE_INT(cjson_device, "gpio5", sensor_0040_other_TCS230_data->TCS230_pin.gpio_output_en);    // gpio_output_en
            CJSON_GET_VALUE_INT(cjson_device, "gpio6", sensor_0040_other_TCS230_data->TCS230_pin.gpio_pulse_output); // gpio_pulse_output

            sensor_other_TCS230_properties->user_arg = sensor_0040_other_TCS230_data; // structure containing calib_factors & data_val
        }
    }
    return sensor_other_TCS230_properties;
}

static int sensor_other_tcs230_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg; // create a ' TCS230_data_t ' type dummy pointer
    TCS230_data_t *sensor_0040_other_TCS230_data = (TCS230_data_t *)malloc(sizeof(TCS230_data_t));

    if ((NULL != prep_arg) && (NULL != prep_arg->cjson_device) && (NULL != sensor_0040_other_TCS230_data))
    {
        memset(sensor_0040_other_TCS230_data, 0, sizeof(TCS230_data_t));
        uint32_t device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(rgb_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_rgbcolor, value_type_rgb, prep_arg->cjson_device, sensor_0040_other_TCS230_data);
    }
    return ret;
}

static int sensor_other_tcs230_init(s_ezlopi_device_properties_t *properties)
{
    static bool guard = false;
    if (!guard)
    {
        guard = true;
        TCS230_data_t *sensor_0040_other_TCS230_data = (TCS230_data_t *)properties->user_arg;
        if (NULL != sensor_0040_other_TCS230_data)
        {
            tcs230_setup_gpio(sensor_0040_other_TCS230_data->TCS230_pin.gpio_s0,
                              sensor_0040_other_TCS230_data->TCS230_pin.gpio_s1,
                              sensor_0040_other_TCS230_data->TCS230_pin.gpio_s2,
                              sensor_0040_other_TCS230_data->TCS230_pin.gpio_s3,
                              sensor_0040_other_TCS230_data->TCS230_pin.gpio_output_en,
                              sensor_0040_other_TCS230_data->TCS230_pin.gpio_pulse_output);
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

static int sensor_other_tcs230_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)args;
    TCS230_data_t *sensor_0040_other_TCS230_data = (TCS230_data_t *)properties->user_arg;

    if (cjson_properties && sensor_0040_other_TCS230_data)
    {
        if (ezlopi_item_name_rgbcolor == properties->ezlopi_cloud.item_name)
        {
            cJSON *Color_Values = cJSON_AddObjectToObject(cjson_properties, "value");
            cJSON_AddNumberToObject(Color_Values, "red", sensor_0040_other_TCS230_data->red_mapped);
            cJSON_AddNumberToObject(Color_Values, "green", sensor_0040_other_TCS230_data->green_mapped);
            cJSON_AddNumberToObject(Color_Values, "blue", sensor_0040_other_TCS230_data->blue_mapped);
        }
        ret = 1;
    }
    return ret;
}

//------------------------------------------------------------------------------

static int sensor_0040_other_TCS230_update_values(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    // 'void_type' addrress -> 'TCS230_data_t' address
    TCS230_data_t *sensor_0040_other_TCS230_data = (TCS230_data_t *)properties->user_arg;

    if (NULL != properties)
    {
        bool valid_status = get_tcs230_sensor_value(properties); // Informs and updates if valid data
        if (valid_status)
        {
            // now using the data_stored within "USER_ARG"
#if 0
            TRACE_I("---------------------------------------");
            TRACE_I("Red : %d", sensor_0040_other_TCS230_data->red_mapped);
            TRACE_I("Green :%d", sensor_0040_other_TCS230_data->green_mapped);
            TRACE_I("Blue : %d", sensor_0040_other_TCS230_data->blue_mapped);
            TRACE_I("---------------------------------------");
#endif
        }
    }
    return ret;
}