#include "trace.h"
#include "cJSON.h"
#include "items.h"
#include "driver/gpio.h"

#include "ezlopi_pwm.h"
#include "ezlopi_timer.h"
#include "ezlopi_actions.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_scales_str.h"
#include "ezlopi_valueformatter.h"

#include "sensor_0054_PWM_YFS201_flowmeter.h"
//*************************************************************************
//                          Declaration
//*************************************************************************
static QueueHandle_t yfs201_queue = NULL;
//------------------------------------------------------------------------------
static void IRAM_ATTR gpio_isr_handler(void *arg) // argument => time_us
{
    *((uint32_t *)arg) = *((uint32_t *)arg) + 1;
}
//------------------------------------------------------------------------------
static int __0054_prepare(void *arg);
static int __0054_init(l_ezlopi_item_t *item);
static int __0054_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __0054_notify(l_ezlopi_item_t *item);
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data);
static void Extract_YFS201_Pulse_Count_func(l_ezlopi_item_t *item);
//------------------------------------------------------------------------------
int sensor_0054_PWM_YFS201_flowmeter(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0054_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0054_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0054_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        static uint8_t count;
        if (count++ > 1)
        {
            __0054_notify(item);
            count = 0;
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
    device->cloud_properties.category = category_flow_meter;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_water_flow;
    item->cloud_properties.value_type = value_type_volume_flow;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_liter_per_hour;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type); // _max = 10
    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.pwm.gpio_num);

    // passing the custom data_structure
    item->user_arg = user_data;
}
//------------------------------------------------------------------------------------------------------
static int __0054_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        //---------------------------  DIGI - DEVICE 1 --------------------------------------------

        yfs201_t *yfs201_data = (yfs201_t *)malloc(sizeof(yfs201_t));
        if (NULL != yfs201_data)
        {
            memset(yfs201_data, 0, sizeof(yfs201_t));
            l_ezlopi_device_t *flowmeter_device = ezlopi_device_add_device();
            if (flowmeter_device)
            {
                __prepare_device_cloud_properties(flowmeter_device, device_prep_arg->cjson_device);
                l_ezlopi_item_t *flowmeter_item = ezlopi_device_add_item_to_device(flowmeter_device, sensor_0054_PWM_YFS201_flowmeter);
                if (flowmeter_item)
                {
                    __prepare_item_cloud_properties(flowmeter_item, device_prep_arg->cjson_device, yfs201_data);
                }
                else
                {
                    ezlopi_device_free_device(flowmeter_device);
                    free(yfs201_data);
                }
            }
            else
            {
                ezlopi_device_free_device(flowmeter_device);
                free(yfs201_data);
            }

            ret = 1;
        }
    }
    return ret;
}

static int __0054_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (NULL != item)
    { // intialize digital_pin
        gpio_config_t input_conf = {
            .pin_bit_mask = (1ULL << item->interface.pwm.gpio_num),
            .intr_type = GPIO_INTR_POSEDGE,
            .mode = GPIO_MODE_INPUT,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
        };
        gpio_config(&input_conf);
        ret = 1;
    }
    return ret;
}

static int __0054_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            yfs201_t *yfs201_data = (yfs201_t *)item->user_arg;
            float freq = 0, Lt_per_hr = 0;
            // converting pulse_counta into frequency (uSec -> Hz)
            freq = yfs201_data->yfs201_dominant_pulse_count * YFS201_QUEUE_SIZE; // [counts_200ms -> counts_1sec]

            // liter per hr
            Lt_per_hr = freq * 7.3f;
            Lt_per_hr = (Lt_per_hr < 1) ? 0 : Lt_per_hr;
            Lt_per_hr = (Lt_per_hr > 720) ? 720 : Lt_per_hr;
            // TRACE_E(" Frequency : %.2f Hz --> FlowRate : %.2f [Lt_per_hr]", freq, Lt_per_hr);

            char *valueFormatted = ezlopi_valueformatter_float(Lt_per_hr);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cj_result, "value", Lt_per_hr);
            free(valueFormatted);
            ret = 1;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------------------------------
static int __0054_notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        yfs201_t *yfs201_data = (yfs201_t *)item->user_arg;
        // extract new pulse count
        uint32_t prev_yfs201_dominant_pulse_count = yfs201_data->yfs201_dominant_pulse_count;
        Extract_YFS201_Pulse_Count_func(item);
        if (prev_yfs201_dominant_pulse_count != yfs201_data->yfs201_dominant_pulse_count)
        {
            ezlopi_device_value_updated_from_device_v3(item);
        }
        ret = 1;
    }
    return ret;
}

//------------------------------------------------------------------------------
// This function is used to get the time_period of incoming pulses . [NOTE: call 'gpio_install_isr_service()' before using this function]
static void Extract_YFS201_Pulse_Count_func(l_ezlopi_item_t *item)
{
    if (NULL != item)
    {
        yfs201_t *yfs201_data = (yfs201_t *)item->user_arg;
        gpio_num_t pulse_pin = item->interface.pwm.gpio_num;
        // creating queue here
        yfs201_queue = xQueueCreate(YFS201_QUEUE_SIZE, sizeof(int32_t)); // takes max -> 1mSec
        if (yfs201_queue)
        {
            int32_t start_time = 0;
            // extract data for untill all queue is filled
            // TRACE_E("--------- Queue Empty --------");

            while ((yfs201_data->yfs201_QueueFlag) < YFS201_QUEUE_FULL)
            {
                (yfs201_data->_pulses_yfs201) = 0;                                                 // reset variable to store fresh counts within [200ms]
                gpio_isr_handler_add(pulse_pin, gpio_isr_handler, &(yfs201_data->_pulses_yfs201)); // add -> gpio_isr_handle(pin_num)
                start_time = (int32_t)esp_timer_get_time();
                while (((int32_t)esp_timer_get_time() - start_time) < (1000000 / YFS201_QUEUE_SIZE)) // 200ms -> 200000uS
                {
                    // polls for '(1000000 / YFS201_QUEUE_SIZE)' -> eg. 200ms
                }
                // check queue_full => 1

                if (xQueueSend(yfs201_queue, &(yfs201_data->_pulses_yfs201), NULL))
                {
                    (yfs201_data->yfs201_QueueFlag) = YFS201_QUEUE_AVAILABLE;
                    // TRACE_E("Pulse_count : %d", (yfs201_data->_pulses_yfs201));
                }
                else
                {
                    // TRACE_E("--------- Queue Full --------");
                    (yfs201_data->yfs201_QueueFlag) = YFS201_QUEUE_FULL;
                }
                // disable -> gpio_isr_handle_remove(pin_num)
                gpio_isr_handler_remove(pulse_pin);
            }
        }

        if ((yfs201_data->yfs201_QueueFlag) == YFS201_QUEUE_FULL)
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
            (yfs201_data->yfs201_QueueFlag) = YFS201_QUEUE_AVAILABLE;

            // write the dominant pulse count
            yfs201_data->yfs201_dominant_pulse_count = P_count[max_freq_index];
        }

        // Deleting queue after no use to avoid conflicts
        vQueueDelete(yfs201_queue);
    }
}

//------------------------------------------------------------------------------