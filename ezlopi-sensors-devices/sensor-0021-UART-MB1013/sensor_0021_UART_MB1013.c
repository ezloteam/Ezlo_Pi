
#include "ezlopi_util_trace.h"

// #include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_hal_uart.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0021_UART_MB1013.h"
#include "EZLOPI_USER_CONFIG.h"

typedef struct s_mb1013_args
{
    float current_value;
    float previous_value;
} s_mb1013_args_t;

static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __notify(l_ezlopi_item_t *item);
static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg);

ezlopi_error_t sensor_0021_UART_MB1013(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;

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

static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        s_mb1013_args_t *mb1013_args = item->user_arg;
        if (mb1013_args)
        {
            cJSON *cj_result = (cJSON *)arg;
            ezlopi_valueformatter_float_to_cjson(cj_result, mb1013_args->current_value, scales_meter);
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static void __uart_data_upcall(uint8_t *buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle)
{
    if (buffer && output_len && uart_object_handle)
    {
        l_ezlopi_item_t *item = (l_ezlopi_item_t *)uart_object_handle->arg;
        if (item)
        {
            s_mb1013_args_t *s_mb1013_args = (s_mb1013_args_t *)item->user_arg;
            if (s_mb1013_args)
            {
                int idx = 0;
                TRACE_D("BUFFER-DATA-LEN: %d", output_len);
                while (idx < output_len)
                {
                    // dump("rx-buffer", buffer, idx, 6);
                    if ('R' == buffer[idx] && '\r' == buffer[idx + 5])
                    {
                        s_mb1013_args->current_value = atoi((const char *)&buffer[idx + 1]) / 10.0;
                        TRACE_D("range: %f", s_mb1013_args->current_value);
                        break;
                    }
                    idx++;
                }
            }
        }
    }
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {
        s_mb1013_args_t *mb1013_args = (s_mb1013_args_t *)item->user_arg;
        if (mb1013_args)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.uart.tx) && GPIO_IS_VALID_GPIO(item->interface.uart.rx))
            {
                s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle = ezlopi_uart_init(item->interface.uart.baudrate, item->interface.uart.tx, item->interface.uart.rx, __uart_data_upcall, item);
                item->interface.uart.channel = ezlopi_uart_get_channel(ezlopi_uart_object_handle);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static void __setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __setup_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_distance;
    item->cloud_properties.value_type = value_type_length;
    item->cloud_properties.scale = scales_centi_meter;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
}

static void __setup_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_baud_str, item->interface.uart.baudrate);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_tx_str, item->interface.uart.tx);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_rx_str, item->interface.uart.rx);
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (arg)
    {
        cJSON *cjson_device = prep_arg->cjson_device;
        if (cjson_device)
        {
            l_ezlopi_device_t *device = ezlopi_device_add_device(prep_arg->cjson_device, NULL);
            if (device)
            {
                __setup_device_cloud_properties(device, cjson_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, sensor_0021_UART_MB1013);
                if (item)
                {
                    __setup_item_cloud_properties(item, cjson_device);
                    __setup_item_interface_properties(item, cjson_device);

                    s_mb1013_args_t *mb1030_args = ezlopi_malloc(__FUNCTION__, sizeof(s_mb1013_args_t));
                    if (mb1030_args)
                    {
                        mb1030_args->current_value = 0.0;
                        mb1030_args->previous_value = 0.0;
                        item->is_user_arg_unique = true;
                        item->user_arg = mb1030_args;
                        ret = EZPI_SUCCESS;
                    }
                }
                else
                {
                    ezlopi_device_free_device(device);
                }
            }
        }
    }

    return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && item->user_arg)
    {
        s_mb1013_args_t *mb1013_args = (s_mb1013_args_t *)item->user_arg;
        if (mb1013_args)
        {
            if (abs(mb1013_args->current_value - mb1013_args->previous_value) > 0.2) // accuracy of 0.5cm (i.e. 5mm)
            {
                ezlopi_device_value_updated_from_device_broadcast(item);
                mb1013_args->previous_value = mb1013_args->current_value;
                ret = EZPI_SUCCESS;
            }
        }
    }

    return ret;
}
