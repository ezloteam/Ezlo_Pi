

#include "ultrasonic_MB1013.h"
#include "cJSON.h"
#include "trace.h"
#include "frozen.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "stdlib.h"

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_value_cjson(l_ezlopi_item_t *item, void *arg);

int ultrasonic_mb1013_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    case EZLOPI_ACTION_SET_VALUE:
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
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        cJSON_AddBoolToObject(cj_result, "value", (bool)item->user_arg);
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
            int val = atoi(tmp_buffer);
            if (val <= 500)
            {
                item->user_arg = (void *)true;
            }
            else
            {
                item->user_arg = (void *)false;
            }
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
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_motion;
    device->cloud_properties.device_type = dev_type_sensor_motion;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __setup_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_motion;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.scale = NULL;
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
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, ultrasonic_mb1013_v3);
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

#if 0
static bool previous_motion = false;
static bool is_motion_detected = false;

static int ezlopi_ultrasonic_MB1013_prepare_and_add(void *args);
static s_ezlopi_device_properties_t *ezlopi_ultrasonic_MB1013_prepare(cJSON *cjson_device);
static int ezlopi_ultrasonic_MB1013_init(s_ezlopi_device_properties_t *properties);
static int ezlopi_ultrasonic_MB1013_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);
static void ezlopi_ultrasonic_MB1013_upcall(uint8_t *buffer, s_ezlopi_uart_object_handle_t uart_object_handle);
static int ezlopi_send_motion_detected_data(s_ezlopi_device_properties_t *properties);

int ultrasonic_MB1013(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = ezlopi_ultrasonic_MB1013_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = ezlopi_ultrasonic_MB1013_init(properties);
        break;
    }
    // case EZLOPI_ACTION_NOTIFY_200_MS:
    // {
    //     ret = ezlopi_send_motion_detected_data(properties);
    //     break;
    // }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ezlopi_ultrasonic_MB1013_get_value_cjson(properties, arg);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static void ezlopi_ultrasonic_MB1013_upcall(uint8_t *buffer, s_ezlopi_uart_object_handle_t uart_object_handle)
{
    // TRACE_E("Buffer is %s", buffer);
    char *another_buffer = (char *)malloc(256);
    memcpy(another_buffer, buffer + 1, 4);
#warning "use ring buffer"
    int val = atoi(another_buffer);
    if (val <= 500)
    {
        is_motion_detected = true;
    }
    else
    {
        is_motion_detected = false;
    }
    free(another_buffer);
}

static int ezlopi_ultrasonic_MB1013_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *ezlopi_ultrasonic_MB1013_properties = ezlopi_ultrasonic_MB1013_prepare(device_prep_arg->cjson_device);
        if (ezlopi_ultrasonic_MB1013_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, ezlopi_ultrasonic_MB1013_properties, NULL))
            {
                free(ezlopi_ultrasonic_MB1013_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static s_ezlopi_device_properties_t *ezlopi_ultrasonic_MB1013_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *ezlopi_ultrasonic_MB1013_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (ezlopi_ultrasonic_MB1013_properties)
    {
        memset(ezlopi_ultrasonic_MB1013_properties, 0, sizeof(s_ezlopi_device_properties_t));
        ezlopi_ultrasonic_MB1013_properties->interface_type = EZLOPI_DEVICE_INTERFACE_UART;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(ezlopi_ultrasonic_MB1013_properties, device_name);
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.category = category_security_sensor;
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.subcategory = subcategory_motion;
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.item_name = ezlopi_item_name_motion;
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.device_type = dev_type_sensor_motion;
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.value_type = value_type_bool;
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.has_getter = true;
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.has_setter = false;
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.reachable = true;
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.battery_powered = false;
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.show = true;
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.room_name[0] = '\0';
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        ezlopi_ultrasonic_MB1013_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "baud_rate", ezlopi_ultrasonic_MB1013_properties->interface.uart.baudrate);
        CJSON_GET_VALUE_INT(cjson_device, "gpio_tx", ezlopi_ultrasonic_MB1013_properties->interface.uart.tx);
        CJSON_GET_VALUE_INT(cjson_device, "gpio_rx", ezlopi_ultrasonic_MB1013_properties->interface.uart.rx);

        // ezlopi_ultrasonic_MB1013_properties->interface.uart.baudrate = 9600;
        // ezlopi_ultrasonic_MB1013_properties->interface.uart.tx = 0;
        // ezlopi_ultrasonic_MB1013_properties->interface.uart.rx = 18;
    }
    return ezlopi_ultrasonic_MB1013_properties;
}

static int ezlopi_ultrasonic_MB1013_init(s_ezlopi_device_properties_t *properties)
{
    int ret = -1;
    if (GPIO_IS_VALID_GPIO(properties->interface.uart.tx) && GPIO_IS_VALID_GPIO(properties->interface.uart.rx))
    {
        s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle = ezlopi_uart_init(properties->interface.uart.baudrate, properties->interface.uart.tx,
                                                                                   properties->interface.uart.rx, ezlopi_ultrasonic_MB1013_upcall);
        properties->interface.uart.channel = ezlopi_uart_get_channel(ezlopi_uart_object_handle);
        ret = 0;
    }
    return ret;
}

// static int ezlopi_send_motion_detected_data(s_ezlopi_device_properties_t *properties)
// {
//     int ret = 0;
//     if (is_motion_detected != previous_motion)
//     {
//         ret = ezlopi_device_value_updated_from_device(properties);
//         previous_motion = is_motion_detected;
//     }
//     return ret;
// }

static int ezlopi_ultrasonic_MB1013_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)args;
    if (cjson_propertise)
    {
        cJSON_AddBoolToObject(cjson_propertise, "value", is_motion_detected);
        ret = 1;
    }
    return ret;
}
#endif