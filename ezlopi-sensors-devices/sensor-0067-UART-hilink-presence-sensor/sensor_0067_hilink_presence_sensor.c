#include <string.h>
#include "../../build/config/sdkconfig.h"
#include "ezlopi_util_trace.h"

#include "ld2410.h"

// #include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0067_hilink_presence_sensor.h"
#include "hilink_presence_sensor_setting.h"
#include "EZLOPI_USER_CONFIG.h"

static const char *hilink_presence_sensor_motion_direction_enum[] = {
    "unknown",
    "no_motion",
    "incoming",
    "outgoing",
};

static l_ezlopi_item_t *motion_item;
static l_ezlopi_item_t *motion_direction_item;
static l_ezlopi_item_t *distance_item;

static ezlopi_error_t __prepare(void *arg, void *user_arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __get_hub_item_value(l_ezlopi_item_t *item, void *args);
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *args);
static ezlopi_error_t __notify(l_ezlopi_item_t *item);

ezlopi_error_t sensor_0067_hilink_presence_sensor_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __prepare(arg, user_arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        ret = __get_hub_item_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_cjson_value(item, arg);
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

static void __notify_if_needed(ld2410_outputs_t *hilink_data, ld2410_outputs_t *present_hilink_data)
{
    // Update specific item to the cloud only when the target's value is changed, else continue.
    if (hilink_data->direction != present_hilink_data->direction)
    {
        hilink_data->direction = present_hilink_data->direction;
        ezlopi_device_value_updated_from_device_broadcast(motion_direction_item);
    }
    else if (hilink_data->presence != present_hilink_data->presence)
    {
        hilink_data->presence = present_hilink_data->presence;
        ezlopi_device_value_updated_from_device_broadcast(motion_item);
    }
    else if (hilink_data->moving_target_distance != present_hilink_data->moving_target_distance)
    {
        hilink_data->moving_target_distance = present_hilink_data->moving_target_distance;
        ezlopi_device_value_updated_from_device_broadcast(distance_item);
    }
}

static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    static int sec_count = 0;
    ld2410_outputs_t *hilink_data = (ld2410_outputs_t *)item->user_arg;
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
                ezlopi_device_value_updated_from_device_broadcast(motion_direction_item);
                ezlopi_device_value_updated_from_device_broadcast(motion_item);
                ezlopi_device_value_updated_from_device_broadcast(distance_item);
                sec_count = 0;
            }
            sec_count++;
        }
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static ezlopi_error_t __add_array_to_object(cJSON *cj_params, const char *const *arr, const int count)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (cj_params && arr)
    {
        cJSON *enum_array = cJSON_CreateStringArray(__FUNCTION__, arr, count);
        cJSON_AddItemToObject(__FUNCTION__, cj_params, "enum", enum_array);
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static ezlopi_error_t __add_value_to_cjson(l_ezlopi_item_t *item, cJSON *cj_params, bool is_hub_request)
{
    ezlopi_error_t ret = EZPI_FAILED;
    ld2410_outputs_t *hilink_data = (ld2410_outputs_t *)item->user_arg;

    if (item && cj_params && hilink_data)
    {
        if (ezlopi_item_name_motion == item->cloud_properties.item_name)
        {
            bool motion = false;
            if ((APPROACHING == hilink_data->direction) || (MOVING_AWAY == hilink_data->direction))
            {
                motion = true;
            }
            cJSON_AddBoolToObject(__FUNCTION__, cj_params, ezlopi_valueFormatted_str, motion);
            cJSON_AddBoolToObject(__FUNCTION__, cj_params, ezlopi_value_str, motion);
        }
        else if (ezlopi_item_name_motion_direction == item->cloud_properties.item_name)
        {
            if (is_hub_request)
            {
                ESP_ERROR_CHECK(__add_array_to_object(cj_params, hilink_presence_sensor_motion_direction_enum, 4));
            }
            cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_valueFormatted_str, hilink_presence_sensor_motion_direction_enum[hilink_data->direction + 1]);
            cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_value_str, hilink_presence_sensor_motion_direction_enum[hilink_data->direction + 1]);
        }
        else if (ezlopi_item_name_distance == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(__FUNCTION__, cj_params, ezlopi_valueFormatted_str, hilink_data->moving_target_distance);
            cJSON_AddNumberToObject(__FUNCTION__, cj_params, ezlopi_value_str, hilink_data->moving_target_distance);
        }
        ret = EZPI_SUCCESS;
    }

    return ret;
}

static ezlopi_error_t __get_hub_item_value(l_ezlopi_item_t *item, void *args)
{
    ezlopi_error_t ret = EZPI_FAILED;

    cJSON *cj_params = (cJSON *)args;
    if (cj_params)
    {
        ESP_ERROR_CHECK(__add_value_to_cjson(item, cj_params, true));
        ret = EZPI_SUCCESS;
    }

    return ret;
}

static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *args)
{
    ezlopi_error_t ret = EZPI_FAILED;

    cJSON *cj_params = (cJSON *)args;

    if (cj_params)
    {
        ESP_ERROR_CHECK(__add_value_to_cjson(item, cj_params, false));
        ret = EZPI_SUCCESS;
    }

    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if ((item))
    {
        ld2410_outputs_t *hilink_data = (ld2410_outputs_t *)item->user_arg;
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
                    ret = EZPI_SUCCESS;
                }
            }
        }
    }
    return ret;
}

static void __perare_device_cloud_properties(l_ezlopi_device_t *device)
{
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_motion;
    device->cloud_properties.device_type = dev_type_sensor_motion;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_hilink_motion_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_properties, void *user_args)
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
    CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio_tx_str, item->interface.uart.tx);
    CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio_rx_str, item->interface.uart.rx);

    item->is_user_arg_unique = true;
    item->user_arg = user_args;
}

static void __prepare_hilink_motion_direction_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_properties, void *user_arg)
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
    CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio_tx_str, item->interface.uart.tx);
    CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio_rx_str, item->interface.uart.rx);

    item->user_arg = user_arg;
}

static void __prepare_hilink_distance_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_properties, void *user_arg)
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
    CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio_tx_str, item->interface.uart.tx);
    CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio_rx_str, item->interface.uart.rx);

    item->user_arg = user_arg;
}

static ezlopi_error_t __prepare(void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg)
    {
        ld2410_outputs_t *hilink_data = (ld2410_outputs_t *)ezlopi_malloc(__FUNCTION__, sizeof(ld2410_outputs_t));
        if (hilink_data)
        {
            memset(hilink_data, 0, sizeof(ld2410_outputs_t));
            l_ezlopi_device_t *parent_hilink_device_motion = ezlopi_device_add_device(prep_arg->cjson_device, "motion", 0);
            if (parent_hilink_device_motion)
            {
                ret = EZPI_SUCCESS;
                TRACE_I("Parent_hilink_motion-[0x%x] ", parent_hilink_device_motion->cloud_properties.device_id);
                __perare_device_cloud_properties(parent_hilink_device_motion);

                motion_item = ezlopi_device_add_item_to_device(parent_hilink_device_motion, sensor_0067_hilink_presence_sensor_v3);
                if (motion_item)
                {
                    __prepare_hilink_motion_item_cloud_properties(motion_item, prep_arg->cjson_device, (void *)hilink_data);
                }

                l_ezlopi_device_t *child_hilink_device_direction = ezlopi_device_add_device(prep_arg->cjson_device, "direction", parent_hilink_device_motion->cloud_properties.device_id);
                if (child_hilink_device_direction)
                {
                    TRACE_I("child_hilink_device_direction-[0x%x] ", child_hilink_device_direction->cloud_properties.device_id);
                    __perare_device_cloud_properties(child_hilink_device_direction);

                    motion_direction_item = ezlopi_device_add_item_to_device(child_hilink_device_direction, sensor_0067_hilink_presence_sensor_v3);
                    if (motion_direction_item)
                    {
                        __prepare_hilink_motion_direction_item_cloud_properties(motion_direction_item, prep_arg->cjson_device, (void *)hilink_data);
                    }
                    else
                    {
                        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                        ezlopi_device_free_device(child_hilink_device_direction);
                    }
                }

                l_ezlopi_device_t *child_hilink_device_distance = ezlopi_device_add_device(prep_arg->cjson_device, "distance", parent_hilink_device_motion->cloud_properties.device_id);
                if (child_hilink_device_distance)
                {
                    TRACE_I("child_hilink_device_distance-[0x%x] ", child_hilink_device_distance->cloud_properties.device_id);
                    __perare_device_cloud_properties(child_hilink_device_distance);

                    distance_item = ezlopi_device_add_item_to_device(child_hilink_device_distance, sensor_0067_hilink_presence_sensor_v3);
                    if (distance_item)
                    {
                        __prepare_hilink_distance_item_cloud_properties(distance_item, prep_arg->cjson_device, (void *)hilink_data);
                    }
                    else
                    {
                        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                        ezlopi_device_free_device(child_hilink_device_distance);
                    }
                }

                if ((NULL == motion_item) &&
                    (NULL == child_hilink_device_direction) &&
                    (NULL == child_hilink_device_distance))
                {
                    ezlopi_device_free_device(parent_hilink_device_motion);
                    ezlopi_free(__FUNCTION__, hilink_data);
                    ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                }
                else
                {
                    ret = hilink_presence_sensor_initialize_settings(parent_hilink_device_motion);
                }
            }
            else
            {
                ezlopi_device_free_device(parent_hilink_device_motion);
                ezlopi_free(__FUNCTION__, hilink_data);
                ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
            }
        }
        else
        {
            ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
        }
    }
    return ret;
}
