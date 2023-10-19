
#include <stdlib.h>

#include "cJSON.h"
#include "trace.h"

#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_valueformatter.h"

#include "sensor_0021_UART_MB1013.h"

static float mb1013_value = 0;

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
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        cJSON_AddNumberToObject(cj_result, "value", mb1013_value);
        snprintf(valueFormatted, 20, "%.2f", mb1013_value);
        cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
        ret = 1;
    }
    return ret;
}

static void __uart_data_upcall(uint8_t *buffer, s_ezlopi_uart_object_handle_t uart_object_handle)
{
    if (uart_object_handle->arg)
    {
        l_ezlopi_item_t *item = (l_ezlopi_item_t *)uart_object_handle->arg;
        // TRACE_E("Buffer is %s", buffer);
        char *tmp_buffer = (char *)malloc(256);
        if (tmp_buffer)
        {
            memcpy(tmp_buffer, buffer + 1, 4);

#warning "use ring buffer"
            mb1013_value = atoi(tmp_buffer) / 10.0;
            free(tmp_buffer);
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
    return ret;
}

static void __setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
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
    CJSON_GET_VALUE_INT(cj_device, "baud_rate", item->interface.uart.baudrate);
    CJSON_GET_VALUE_INT(cj_device, "gpio_tx", item->interface.uart.tx);
    CJSON_GET_VALUE_INT(cj_device, "gpio_rx", item->interface.uart.rx);
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
            l_ezlopi_device_t *device = ezlopi_device_add_device();
            if (device)
            {
                __setup_device_cloud_properties(device, cjson_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, sensor_0021_UART_MB1013);
                if (item)
                {
                    __setup_item_cloud_properties(item, cjson_device);
                    __setup_item_interface_properties(item, cjson_device);
                    ret = 1;
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

static int __notify(l_ezlopi_item_t *item)
{
    return ezlopi_device_value_updated_from_device_v3(item);
}
