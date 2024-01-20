
#include <stdlib.h>
#include "cJSON.h"

#include "trace.h"
#include "ezlopi_cloud_items.h"

#include "ezlopi_timer.h"
#include "ezlopi_cloud.h"
#include "ezlopi_actions.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_cjson_macros.h"
#include "ezlopi_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"

#include "sensor_0021_UART_MB1013.h"

typedef struct s_mb1013_args
{
    float current_value;
    float previous_value;
} s_mb1013_args_t;

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_value_cjson(l_ezlopi_item_t *item, void *arg);

int sensor_0021_UART_MB1013(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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

static int __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    char valueFormatted[20];
    if (item && arg && item->user_arg)
    {
        s_mb1013_args_t *mb1013_args = item->user_arg;

        cJSON *cj_result = (cJSON *)arg;
        cJSON_AddNumberToObject(cj_result, ezlopi_value_str, mb1013_args->current_value);
        snprintf(valueFormatted, 20, "%.2f", mb1013_args->current_value);
        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
        ret = 1;
    }
    return ret;
}

static void __uart_data_upcall(uint8_t *buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle)
{
    if (buffer && output_len)
    {
        if (uart_object_handle)
        {
            l_ezlopi_item_t *item = (l_ezlopi_item_t *)uart_object_handle->arg;
            if (item && item->user_arg)
            {
                s_mb1013_args_t *s_mb1013_args = (s_mb1013_args_t *)item->user_arg;
                int idx = 0;
                TRACE_D("BUFFER-DATA-LEN: %d", output_len);
                while (idx < output_len)
                {
                    dump("rx-buffer", buffer, idx, 6);
                    if ('R' == buffer[idx] && '\r' == buffer[idx + 5])
                    {
                        s_mb1013_args->current_value = atoi((const char *)&buffer[idx + 1]) / 10.0;
                        TRACE_B("range: %f", s_mb1013_args->current_value);
                        break;
                    }
                    idx++;
                }

#warning "use ring buffer"
            }
        }
    }
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.uart.tx) && GPIO_IS_VALID_GPIO(item->interface.uart.rx))
        {
            s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle = ezlopi_uart_init(item->interface.uart.baudrate, item->interface.uart.tx, item->interface.uart.rx, __uart_data_upcall, item);
            item->interface.uart.channel = ezlopi_uart_get_channel(ezlopi_uart_object_handle);
            ret = 1;
        }
    }

    if (0 == ret)
    {
        ret = -1;
        if (item->user_arg)
        {
            free(item->user_arg);
            item->user_arg = NULL;
        }
    }

    return ret;
}

static void __setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    // char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
    // ASSIGN_DEVICE_NAME_V2(device, device_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

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
    CJSON_GET_VALUE_INT(cj_device, ezlopi_baud_rate_str, item->interface.uart.baudrate);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_tx_str, item->interface.uart.tx);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_rx_str, item->interface.uart.rx);
}

static int __prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (arg)
    {
        cJSON *cjson_device = prep_arg->cjson_device;
        if (cjson_device)
        {
            l_ezlopi_device_t *device = ezlopi_device_add_device(prep_arg->cjson_device);
            if (device)
            {
                __setup_device_cloud_properties(device, cjson_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, sensor_0021_UART_MB1013);
                if (item)
                {
                    item->cloud_properties.device_id = device->cloud_properties.device_id;
                    __setup_item_cloud_properties(item, cjson_device);
                    __setup_item_interface_properties(item, cjson_device);
                    ret = 1;

                    s_mb1013_args_t *mb1030_args = malloc(sizeof(s_mb1013_args_t));
                    if (mb1030_args)
                    {
                        mb1030_args->current_value = 0.0;
                        mb1030_args->previous_value = 0.0;
                        item->user_arg = mb1030_args;
                    }
                }
                else
                {
                    ret = -1;
                }
            }
        }
    }

    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item && item->user_arg)
    {
        s_mb1013_args_t *mb1013_args = (s_mb1013_args_t *)item->user_arg;
        if (abs(mb1013_args->current_value - mb1013_args->previous_value) > 0.2) // accuracy of 0.5cm (i.e. 5mm)
        {
            ezlopi_device_value_updated_from_device_v3(item);
            mb1013_args->previous_value = mb1013_args->current_value;
        }
    }

    return ret;
}
