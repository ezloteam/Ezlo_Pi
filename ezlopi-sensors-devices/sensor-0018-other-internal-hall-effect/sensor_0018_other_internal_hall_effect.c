

#include "sdkconfig.h"

#ifdef CONFIG_IDF_TARGET_ESP32

#include <math.h>
#include "ezlopi_util_trace.h"
// #include "esp_err.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0018_other_internal_hall_effect.h"

const char* hall_door_window_states[] = {
    "dw_is_opened",
    "dw_is_closed",
    "unknown",
};
typedef struct s_hall_data
{
    bool calibration_complete;
    char* hall_state;
    int Custom_stable_val;
} s_hall_data_t;

static int __prepare(void* arg);
static int __init(l_ezlopi_item_t* item);
static int __get_item_cjson(l_ezlopi_item_t* item, void* arg);
static int __get_value_cjson(l_ezlopi_item_t* item, void* arg);
static int __notify(l_ezlopi_item_t* item);
static void __setup_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device);
static void __setup_item_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_data);
static void __hall_calibration_task(void* params);

int sensor_0018_other_internal_hall_effect(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        ret = __get_item_cjson(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_value_cjson(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __notify(item);
        break;
    }

    default:
    {
        break;
    }
    }

    return ret;
}

static void __setup_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    char* device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_door;
    device->cloud_properties.device_type = dev_type_doorlock;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __setup_item_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_data)
{
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_dw_state;
    item->cloud_properties.value_type = value_type_token;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;

    item->interface.gpio.gpio_in.gpio_num = GPIO_NUM_36;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;

    item->user_arg = user_data;
}

static int __prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (arg && prep_arg->cjson_device)
    {
        cJSON* cj_device = prep_arg->cjson_device;
        s_hall_data_t* user_data = (s_hall_data_t*)malloc(sizeof(s_hall_data_t));
        if (user_data)
        {
            memset(user_data, 0, sizeof(s_hall_data_t));
            l_ezlopi_device_t* hall_device = ezlopi_device_add_device(cj_device);
            if (hall_device)
            {
                __setup_device_cloud_properties(hall_device, cj_device);
                l_ezlopi_item_t* hall_item = ezlopi_device_add_item_to_device(hall_device, sensor_0018_other_internal_hall_effect);
                if (hall_item)
                {
                    __setup_item_properties(hall_item, cj_device, user_data);
                }
                else
                {
                    ezlopi_device_free_device(hall_device);
                    free(user_data);
                    ret = -1;
                }
            }
            else
            {
                free(user_data);
                ret = -1;
            }
        }

        ret = 1;
    }
    return ret;
}

static int __init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
#ifdef CONFIG_IDF_TARGET_ESP32
        esp_err_t error = adc1_config_width(ADC_WIDTH_BIT_12);
#else
        esp_err_t error = ESP_ERR_NOT_FOUND;
#endif
        s_hall_data_t* user_data = (s_hall_data_t*)item->user_arg;
        if (user_data)
        {
            if (ESP_OK == error)
            {
                TRACE_I("Width configuration was successfully done!");
                TRACE_W("Calibrating.....");
                user_data->hall_state = "dw_is_closed";
                xTaskCreate(__hall_calibration_task, "Hall_Calibration_Task", 2048, item, 1, NULL);
                ret = 1;
            }
            else
            {
                TRACE_E("Error 'sensor_door_init'. error: %s)", esp_err_to_name(error));
                ret = -1;
            }
        }
        else
        {
            ret = -1;
        }
    }
    return ret;
}

static int __get_item_cjson(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        s_hall_data_t* user_data = (s_hall_data_t*)item->user_arg;
        if (user_data)
        {
            cJSON* cj_result = (cJSON*)arg;
            if (cj_result)
            {
                //-------------------  POSSIBLE JSON ENUM LPGNTENTS ----------------------------------
                cJSON* json_array_enum = cJSON_CreateArray();
                if (NULL != json_array_enum)
                {
                    for (uint8_t i = 0; i < HALL_DOOR_WINDOW_MAX; i++)
                    {
                        cJSON* json_value = cJSON_CreateString(hall_door_window_states[i]);
                        if (NULL != json_value)
                        {
                            cJSON_AddItemToArray(json_array_enum, json_value);
                        }
                    }
                    cJSON_AddItemToObject(cj_result, ezlopi_enum_str, json_array_enum);
                }
                //--------------------------------------------------------------------------------------
                cJSON_AddStringToObject(cj_result, ezlopi_value_str, user_data->hall_state);
                cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, user_data->hall_state);
            }
            ret = 1;
        }
    }
    return ret;
}

static int __get_value_cjson(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        s_hall_data_t* user_data = (s_hall_data_t*)item->user_arg;
        if (user_data)
        {
            cJSON* cj_result = (cJSON*)arg;
            if (cj_result)
            {
                cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, user_data->hall_state);
                cJSON_AddStringToObject(cj_result, ezlopi_value_str, user_data->hall_state);
            }
            ret = 1;
        }
    }
    return ret;
}

static int __notify(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        s_hall_data_t* user_data = (s_hall_data_t*)item->user_arg;
        if (user_data)
        {
            if (user_data->calibration_complete)
            {
                char* curret_value = NULL;
#ifdef CONFIG_IDF_TARGET_ESP32
                int sensor_data = hall_sensor_read();
#else
                int sensor_data = 0;
#endif
                TRACE_D(" Hall door value ; %d", sensor_data);

                curret_value = ((fabs(user_data->Custom_stable_val - sensor_data) > 35) ? "dw_is_closed" : "dw_is_opened");

                if (curret_value != user_data->hall_state) // calls update only if there is change in state
                {
                    user_data->hall_state = curret_value;
                    ezlopi_device_value_updated_from_device_v3(item);
                }
                ret = 1;
            }
        }
    }
    return ret;
}

static void __hall_calibration_task(void* params) // calibrate task
{
    l_ezlopi_item_t* item = (l_ezlopi_item_t*)params;
    if (item)
    {
        s_hall_data_t* user_data = (s_hall_data_t*)item->user_arg;
        if (user_data)
        {

            float sensor_data = (float)hall_sensor_read();
            for (uint8_t i = 0; i < 10; i++)
            {
                sensor_data = (sensor_data * .8f) + (.2f * (float)hall_sensor_read());
                vTaskDelay(500);
            }
            user_data->Custom_stable_val = (int)sensor_data;
            TRACE_W("Calibration Complete...... Stable_hall is : [%d]", user_data->Custom_stable_val);
            user_data->calibration_complete = true;
        }
    }
    vTaskDelete(NULL);
}

#endif // CONFIG_IDF_TARGET_ESP32