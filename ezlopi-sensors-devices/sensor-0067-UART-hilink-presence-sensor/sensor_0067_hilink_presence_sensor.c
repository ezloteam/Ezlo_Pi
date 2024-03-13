#include <string.h>
#include "sdkconfig.h"
#include "ezlopi_util_trace.h"

#include "ld2410.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0067_hilink_presence_sensor.h"
#include "hilink_presence_sensor_setting.h"

static const char* hilink_presence_sensor_motion_direction_enum[] = {
    "unknown",
    "no_motion",
    "incoming",
    "outgoing",
};

static l_ezlopi_item_t* motion_item;
static l_ezlopi_item_t* motion_direction_item;
static l_ezlopi_item_t* distance_item;

static int __prepare(void* arg, void* user_arg);
static int __init(l_ezlopi_item_t* item);
static int __get_hub_item_value(l_ezlopi_item_t* item, void* args);
static int __get_cjson_value(l_ezlopi_item_t* item, void* args);
static int __notify(l_ezlopi_item_t* item);

int sensor_0067_hilink_presence_sensor_v3(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __prepare(arg, user_arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        __get_hub_item_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static void __notify_if_needed(ld2410_outputs_t* hilink_data, ld2410_outputs_t* present_hilink_data)
{
    // Update specific item to the cloud only when the target's value is changed, else continue.
    if (hilink_data->direction != present_hilink_data->direction)
    {
        hilink_data->direction = present_hilink_data->direction;
        ezlopi_device_value_updated_from_device_v3(motion_direction_item);
    }
    if (hilink_data->presence != present_hilink_data->presence)
    {
        hilink_data->presence = present_hilink_data->presence;
        ezlopi_device_value_updated_from_device_v3(motion_item);
    }
    if (hilink_data->moving_target_distance != present_hilink_data->moving_target_distance)
    {
        hilink_data->moving_target_distance = present_hilink_data->moving_target_distance;
        ezlopi_device_value_updated_from_device_v3(distance_item);
    }
}

static int __notify(l_ezlopi_item_t* item)
{
    int ret = 0;
    static int sec_count = 0;
    ld2410_outputs_t* hilink_data = (ld2410_outputs_t*)item->user_arg;
    if (hilink_data)
    {
        ld2410_outputs_t present_hilink_data;
        memcpy(&present_hilink_data, hilink_data, sizeof(ld2410_outputs_t));
        ESP_ERROR_CHECK(ld2410_get_data(&present_hilink_data));
        // If direction is stationay, no target is moving so, set moving_target_distance to prevent continuous update to the cloud in abasence of moving target.
        if (STATIONARY == present_hilink_data.direction)
        {
            present_hilink_data.moving_target_distance = 0;
        }
        // Check if the moving target is outside the minumum detectable range, if so, update to the cloud immediately.
        if (hilink_presence_sensor_target_in_detectable_range(present_hilink_data.moving_target_distance))
        {
            __notify_if_needed(hilink_data, &present_hilink_data);
            sec_count = 0;
        }
        // If not, update every 30 seconds.
        else
        {
            // Since there are 3 items in this device, all of them calls this function and increae the sec_count causing values to be updated at every 10 sec. To mitigate, sec_count should be compared
            // to 90.
            if (90 == sec_count)
            {
                memcpy(hilink_data, &present_hilink_data, sizeof(ld2410_outputs_t));
                ezlopi_device_value_updated_from_device_v3(motion_direction_item);
                ezlopi_device_value_updated_from_device_v3(motion_item);
                ezlopi_device_value_updated_from_device_v3(distance_item);
                sec_count = 0;
            }
            sec_count++;
        }
    }
    else
    {
        ret = 1;
    }
    return ret;
}

static int __add_array_to_object(cJSON* cj_params, const char* const* arr, const int count)
{
    int ret = 0;
    if (cj_params && arr)
    {
        cJSON* enum_array = cJSON_CreateStringArray(arr, count);
        cJSON_AddItemToObject(cj_params, "enum", enum_array);
        TRACE_I("%s", cJSON_Print(cj_params));
    }
    else
    {
        ret = 1;
    }
    return ret;
}

static int __add_value_to_cjson(l_ezlopi_item_t* item, cJSON* cj_params, bool is_hub_request)
{
    int ret = 0;
    ld2410_outputs_t* hilink_data = (ld2410_outputs_t*)item->user_arg;

    if (item && cj_params && hilink_data)
    {
        if (ezlopi_item_name_motion == item->cloud_properties.item_name)
        {
            bool motion = false;
            if ((APPROACHING == hilink_data->direction) || (MOVING_AWAY == hilink_data->direction))
            {
                motion = true;
            }
            cJSON_AddBoolToObject(cj_params, ezlopi_valueFormatted_str, motion);
            cJSON_AddBoolToObject(cj_params, ezlopi_value_str, motion);
        }
        if (ezlopi_item_name_motion_direction == item->cloud_properties.item_name)
        {
            if (is_hub_request)
            {
                ESP_ERROR_CHECK(__add_array_to_object(cj_params, hilink_presence_sensor_motion_direction_enum, 4));
            }
            cJSON_AddStringToObject(cj_params, ezlopi_valueFormatted_str, hilink_presence_sensor_motion_direction_enum[hilink_data->direction + 1]);
            cJSON_AddStringToObject(cj_params, ezlopi_value_str, hilink_presence_sensor_motion_direction_enum[hilink_data->direction + 1]);
        }
        if (ezlopi_item_name_distance == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(cj_params, ezlopi_valueFormatted_str, hilink_data->moving_target_distance);
            cJSON_AddNumberToObject(cj_params, ezlopi_value_str, hilink_data->moving_target_distance);
        }
    }

    return ret;
}

static int __get_hub_item_value(l_ezlopi_item_t* item, void* args)
{
    int ret = 0;

    cJSON* cj_params = (cJSON*)args;
    if (cj_params)
    {
        ESP_ERROR_CHECK(__add_value_to_cjson(item, cj_params, true));
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t* item, void* args)
{
    int ret = 0;

    cJSON* cj_params = (cJSON*)args;

    if (cj_params)
    {
        ESP_ERROR_CHECK(__add_value_to_cjson(item, cj_params, false));
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int __init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if ((item))
    {
        ld2410_outputs_t* hilink_data = (ld2410_outputs_t*)item->user_arg;
        if (hilink_data)
        {
            if (item->interface.uart.enable)
            {
                s_ezlopi_uart_t uart_settings = {
                    .baudrate = LD2410_BAUDRATE,
                    .tx = item->interface.uart.tx,
                    .rx = item->interface.uart.rx,
                };
                if (ESP_OK == ld2410_setup(uart_settings))
                {
                    ESP_ERROR_CHECK(hilink_presence_sensor_apply_settings());
                    ESP_ERROR_CHECK(ld2410_get_data(hilink_data));
                    ret = 1;
                }
                else
                {
                    ret = -1;
                    free(item->user_arg); // this will free ; memory address linked to all items
                    item->user_arg = NULL;
                    ezlopi_device_free_device_by_item(item);
                }
            }
        }
        else
        {
            ret = -1;
            ezlopi_device_free_device_by_item(item);
        }
    }
    return ret;
}

static void __perare_hilink_device_cloud_properties(l_ezlopi_device_t* device)
{
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_motion;
    device->cloud_properties.device_type = dev_type_sensor_motion;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    device->cloud_properties.parent_device_id = device->cloud_properties.device_id;
}

static void __prepare_hilink_motion_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_properties, void* user_args)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_motion;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.show = true;
    item->cloud_properties.value_type = value_type_bool;

    item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
    item->interface.uart.enable = true;
    item->interface.uart.channel = 0;

    CJSON_GET_VALUE_DOUBLE(cj_properties, ezlopi_baud_str, item->interface.uart.baudrate);
    CJSON_GET_VALUE_DOUBLE(cj_properties, ezlopi_gpio_tx_str, item->interface.uart.tx);
    CJSON_GET_VALUE_DOUBLE(cj_properties, ezlopi_gpio_rx_str, item->interface.uart.rx);

    item->user_arg = user_args;
}

static void __prepare_hilink_motion_direction_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_properties, void* user_arg)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_motion_direction;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.show = true;
    item->cloud_properties.value_type = value_type_token;

    item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
    item->interface.uart.enable = false;
    item->interface.uart.channel = 0;

    CJSON_GET_VALUE_DOUBLE(cj_properties, ezlopi_baud_str, item->interface.uart.baudrate);
    CJSON_GET_VALUE_DOUBLE(cj_properties, ezlopi_gpio_tx_str, item->interface.uart.tx);
    CJSON_GET_VALUE_DOUBLE(cj_properties, ezlopi_gpio_rx_str, item->interface.uart.rx);

    item->user_arg = user_arg;
}

static void __prepare_hilink_distance_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_properties, void* user_arg)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_distance;
    item->cloud_properties.scale = scales_centi_meter;
    item->cloud_properties.show = true;
    item->cloud_properties.value_type = value_type_length;

    item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
    item->interface.uart.enable = false;
    item->interface.uart.channel = 0;

    CJSON_GET_VALUE_DOUBLE(cj_properties, ezlopi_baud_str, item->interface.uart.baudrate);
    CJSON_GET_VALUE_DOUBLE(cj_properties, ezlopi_gpio_tx_str, item->interface.uart.tx);
    CJSON_GET_VALUE_DOUBLE(cj_properties, ezlopi_gpio_rx_str, item->interface.uart.rx);

    item->user_arg = user_arg;
}

static int __prepare(void* arg, void* user_arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (prep_arg)
    {
        ld2410_outputs_t* hilink_data = (ld2410_outputs_t*)malloc(sizeof(ld2410_outputs_t));
        if (hilink_data)
        {
            l_ezlopi_device_t* hilink_device = ezlopi_device_add_device(prep_arg->cjson_device);
            if (hilink_device)
            {
                memset(hilink_data, 0, sizeof(ld2410_outputs_t));
                __perare_hilink_device_cloud_properties(hilink_device);

                motion_item = ezlopi_device_add_item_to_device(hilink_device, sensor_0067_hilink_presence_sensor_v3);
                if (motion_item)
                {
                    __prepare_hilink_motion_item_cloud_properties(motion_item, prep_arg->cjson_device, (void*)hilink_data);
                }

                motion_direction_item = ezlopi_device_add_item_to_device(hilink_device, sensor_0067_hilink_presence_sensor_v3);
                if (motion_direction_item)
                {
                    __prepare_hilink_motion_direction_item_cloud_properties(motion_direction_item, prep_arg->cjson_device, (void*)hilink_data);
                }
                distance_item = ezlopi_device_add_item_to_device(hilink_device, sensor_0067_hilink_presence_sensor_v3);
                if (distance_item)
                {
                    __prepare_hilink_distance_item_cloud_properties(distance_item, prep_arg->cjson_device, (void*)hilink_data);
                }
                if (!motion_item && !motion_direction_item && !distance_item)
                {
                    ezlopi_device_free_device(hilink_device);
                    free(hilink_data);
                    ret = -1;
                }
                else
                {
                    ret = hilink_presence_sensor_initialize_settings(hilink_device);
                }
            }
            else
            {
                ezlopi_device_free_device(hilink_device);
                free(hilink_data);
                ret = -1;
            }
        }
    }
    return ret;
}
