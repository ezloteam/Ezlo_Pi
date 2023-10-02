
#include "cJSON.h"
#include "trace.h"
#include "driver/gpio.h"
#include "ezlopi_pwm.h"
#include "ezlopi_cloud.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"

#include "sensor_0054_PWM_YFS201_flowmeter.h"

//*************************************************************************
//                          Declaration
//*************************************************************************

static uint32_t _pulses, dominant_pulse_count;
static QueueHandle_t yfs201_queue = NULL;
static YFS201_queue_enum_t yfs201_QueueFlag = YFS201_QUEUE_RESET;

static int sensor_pwm_yfs201_prepare_and_add(void *arg);
static int sensor_pwm_yfs201_init(s_ezlopi_device_properties_t *properties);
static void Extract_YFS201_Pulse_Count_func(gpio_num_t pulse_pin);
static void sensor_pwm_yfs201_get_item(s_ezlopi_device_properties_t *properties, void *arg);
static int sensor_pwm_yfs201_get_value(s_ezlopi_device_properties_t *properties, void *arg);
//------------------------------------------------------------------------------

static void IRAM_ATTR gpio_isr_handler(void *arg) // argument => time_us
{
    _pulses++;
}

//--------------------------------------------------------------------------------------------------------------------------------------
int sensor_0054_pwm_yfs201(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_pwm_yfs201_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_pwm_yfs201_init(properties);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        sensor_pwm_yfs201_get_item(properties, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_pwm_yfs201_get_value(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        static uint8_t x;
        if (x == 1)
        {
            Extract_YFS201_Pulse_Count_func(properties->interface.pwm.gpio_num);
        }
        else if (x == 3)
        {
            ret = ezlopi_device_value_updated_from_device(properties);
            x = 0;
        }
        x++;
        break;
    }
    default:

        break;
    }
    return ret;
}

//--------------------------------------------------------------------------------------------------------------------------------------
static s_ezlopi_device_properties_t *sensor_pwm_yfs201_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_0054_pwm_yfs201_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if (sensor_0054_pwm_yfs201_properties)
    {
        memset(sensor_0054_pwm_yfs201_properties, 0, sizeof(s_ezlopi_device_properties_t));
        // setting the interface of the device
        sensor_0054_pwm_yfs201_properties->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;

        // set the device name according to device_id
        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_0054_pwm_yfs201_properties, device_name);

        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.category = category_flow_meter;
        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.item_name = ezlopi_item_name_water_flow;
        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.device_type = dev_type_sensor;
        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.value_type = value_type_volume_flow;
        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.has_getter = true;
        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.has_setter = false;
        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.reachable = true;
        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.battery_powered = false;
        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.show = true;
        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_0054_pwm_yfs201_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_0054_pwm_yfs201_properties->interface.pwm.gpio_num);
    }
    return sensor_0054_pwm_yfs201_properties;
}

static int sensor_pwm_yfs201_prepare_and_add(void *arg) // carries cJSON
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_args = (s_ezlopi_prep_arg_t *)arg;
    if ((NULL != device_prep_args) && (NULL != device_prep_args->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_pwm_yfs201_properties = sensor_pwm_yfs201_prepare(device_prep_args->cjson_device);
        if (sensor_pwm_yfs201_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_args->device, sensor_pwm_yfs201_properties, NULL))
            {
                free(sensor_pwm_yfs201_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}

static int sensor_pwm_yfs201_init(s_ezlopi_device_properties_t *properties)
{
    static bool guard = false;
    int ret = 0;
    if (!guard)
    {
        if (GPIO_IS_VALID_GPIO(properties->interface.pwm.gpio_num))
        {
            // Configures GPIO input pins
            gpio_config_t input_conf;
            input_conf.pin_bit_mask = (1ULL << properties->interface.pwm.gpio_num);
            input_conf.intr_type = GPIO_INTR_POSEDGE;
            input_conf.mode = GPIO_MODE_INPUT;
            input_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
            input_conf.pull_up_en = GPIO_PULLUP_DISABLE;
            gpio_config(&input_conf);
        }
        guard = true;
    }
    return ret;
}

static void sensor_pwm_yfs201_get_item(s_ezlopi_device_properties_t *properties, void *arg)
{
    char valueFormatted[20];
    cJSON *cjson_properties = (cJSON *)arg;

    if (cjson_properties)
    {
        float freq = 0, Lt_per_hr = 0;

        // converting pulse_counta into frequency (uSec -> Hz)
        freq = dominant_pulse_count * YFS201_QUEUE_SIZE; // [counts_200ms -> counts_1sec]

        // liter per hr
        Lt_per_hr = freq * 7.3f;
        Lt_per_hr = (Lt_per_hr < 1) ? 0 : Lt_per_hr;
        Lt_per_hr = (Lt_per_hr > 720) ? 720 : Lt_per_hr;
        // TRACE_E(" Frequency : %.2f Hz --> FlowRate : %.2f [Lt_per_hr]", freq, Lt_per_hr);

        snprintf(valueFormatted, 20, "%.2f", Lt_per_hr);
        cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
        cJSON_AddNumberToObject(cjson_properties, "value", Lt_per_hr);
        cJSON_AddStringToObject(cjson_properties, "scale", "liter_per_hour");
    }
}

static int sensor_pwm_yfs201_get_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    char valueFormatted[20];
    cJSON *cjson_properties = (cJSON *)arg;

    if (cjson_properties)
    {
        float freq = 0, Lt_per_hr = 0;

        // converting pulse_counta into frequency (uSec -> Hz)
        freq = dominant_pulse_count * YFS201_QUEUE_SIZE; // [counts_200ms -> counts_1sec]

        // liter per hr
        Lt_per_hr = freq * 7.3f;
        Lt_per_hr = (Lt_per_hr < 1) ? 0 : Lt_per_hr;
        Lt_per_hr = (Lt_per_hr > 720) ? 720 : Lt_per_hr;
        // TRACE_E(" Frequency : %.2f Hz --> FlowRate : %.2f [Lt_per_hr]", freq, Lt_per_hr);

        snprintf(valueFormatted, 20, "%.2f", Lt_per_hr);
        cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
        cJSON_AddNumberToObject(cjson_properties, "value", Lt_per_hr);
        cJSON_AddStringToObject(cjson_properties, "scale", "liter_per_hour");

        ret = 1;
    }
    return ret;
}

//------------------------------------------------------------------------------
// This function is used to get the time_period of incoming pulses . [NOTE: call 'gpio_install_isr_service()' before using this function]
static void Extract_YFS201_Pulse_Count_func(gpio_num_t pulse_pin)
{
    // creating queue here
    yfs201_queue = xQueueCreate(YFS201_QUEUE_SIZE, sizeof(int32_t)); // takes max -> 1mSec
    if (yfs201_queue)
    {
        int32_t start_time = 0;
        // extract data for untill all queue is filled
        // TRACE_E("--------- Queue Empty --------");

        while (yfs201_QueueFlag < YFS201_QUEUE_FULL)
        {
            _pulses = 0;                                             // reset variable to store fresh counts within [200ms]
            gpio_isr_handler_add(pulse_pin, gpio_isr_handler, NULL); // add -> gpio_isr_handle(pin_num)
            start_time = (int32_t)esp_timer_get_time();
            while (((int32_t)esp_timer_get_time() - start_time) < (1000000 / YFS201_QUEUE_SIZE)) // 200ms -> 200000uS
            {
                // polls for '(1000000 / YFS201_QUEUE_SIZE)' -> eg. 200ms
            }
            // check queue_full => 1

            if (xQueueSend(yfs201_queue, &_pulses, NULL))
            {
                yfs201_QueueFlag = YFS201_QUEUE_AVAILABLE;
                // TRACE_E("Pulse_count : %d", _pulses);
            }
            else
            {
                // TRACE_E("--------- Queue Full --------");
                yfs201_QueueFlag = YFS201_QUEUE_FULL;
            }
            // disable -> gpio_isr_handle_remove(pin_num)
            gpio_isr_handler_remove(pulse_pin);
        }
    }

    if (yfs201_QueueFlag == YFS201_QUEUE_FULL)
    {
        // loop through all the queue[0-5] values -> pulse counts
        int32_t P_count[YFS201_QUEUE_SIZE] = {0};
        int val = 0;
        for (uint8_t i = 0; i < YFS201_QUEUE_SIZE; i++)
        {
            if (xQueueReceive(yfs201_queue, &val, portMAX_DELAY))
            {
                if (val)
                {
                    P_count[i] = val; // [0 - YFS201_QUEUE_SIZE]
                }
            }
        }

        // generate frequency of occurance table from "P_count[]" array values
        uint8_t freq[YFS201_QUEUE_SIZE] = {0};
        float error = 0;
        for (uint8_t x = 0; x < YFS201_QUEUE_SIZE; x++)
        {
            for (uint8_t i = 0; i < YFS201_QUEUE_SIZE; i++)
            {
                error = P_count[x] - P_count[i];
                error = ((error >= 0) ? error : error * -1); // finding difference between two readings
                if (error < P_count[x] * 0.1)                // [error less than +-10%]
                {
                    freq[x] += 1; // increment dominace count
                }
            }
        }
        // find the dominant period
        uint8_t max_freq_index = 0;
        int32_t dominant_val = 0;
        for (uint8_t i = 0; i < YFS201_QUEUE_SIZE; i++)
        {
            if (freq[i] > dominant_val)
            {
                dominant_val = freq[i];
                max_freq_index = i;
            }
        }
        // TRACE_I("......................Dominant count ......{%d} ", P_count[max_freq_index]);

        // reset Queue_flag
        yfs201_QueueFlag = YFS201_QUEUE_AVAILABLE;

        // write the dominant pulse count
        dominant_pulse_count = P_count[max_freq_index];
    }

    // Deleting queue after no use to avoid conflicts
    vQueueDelete(yfs201_queue);
}

//------------------------------------------------------------------------------
