#include <string.h>

#include "sensor_bme280.h"
#include "ezlopi_actions.h"
#include "ezlopi_sensors.h"
#include "ezlopi_timer.h"
#include "items.h"
#include "frozen.h"
#include "trace.h"
#include "cJSON.h"
#include "ezlopi_devices.h"
#include "ezlopi_cloud_category_str.h"

static int digital_io_init();
static int8_t digital_io_get_value(char *sensor_data);
static int digital_io_notify_30_seconds(void);
static int digital_io_prepare(void *arg);

/**
 * @brief Public function to interface bme280. This is used to handles all the action on the bme280 sensor and is the entry point to interface the sensor.
 *
 * @param action e_ezlopi_actions_t
 * @param arg Other arguments if needed
 * @return int
 */
int digital_io(e_ezlopi_actions_t action, void *arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_INITIALIZE:
    {
        TRACE_I("EZLOPI_ACTION_INITIALIZE event.");
        ret = digital_io_init();
        break;
    }
    case EZLOPI_ACTION_GET_VALUE:
    {
        TRACE_I("EZLOPI_ACTION_GET_VALUE event.");
        char *data = (char *)malloc(100);
        ret = digital_io_get_value(data);
        TRACE_I("The string is: %s", data);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_500_MS:
    {
        TRACE_I("EZLOPI_ACTION_NOTIFY_500_MS");
        ret = digital_io_notify_30_seconds();
        break;
    }
    case EZLOPI_ACTION_PREPARE:
    {
        TRACE_I("EZLOPI_ACTION_PREPARE");
        ret = digital_io_prepare(arg);
        break;
    }
    default:
    {
        TRACE_E("Unknown defaule bm280 action found!");
        break;
    }
    }

    return ret;
}

static int digital_io_prepare(void *arg)
{
    int ret = 0;
    cJSON *cjson_device = (cJSON *)arg;

    s_ezlopi_devices_t *device = malloc(sizeof(s_ezlopi_devices_t));

    if (device)
    {
        int tmp_var = 0;
        device->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT;

        ASSIGN_DEVICE_NAME();
        device->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
        device->ezlopi_cloud.category = 
            CJSON_GET_VALUE_INT(cjson_device, "id_room", device->ezlopi_cloud.room_id);
        CJSON_GET_VALUE_INT(cjson_device, "id_item", device->ezlopi_cloud.item_id);
        device->ezlopi_cloud.has_getter = true;
        device->ezlopi_cloud.has_setter = true;

        CJSON_GET_VALUE_INT(cjson_device, "is_ip", device->interface.gpio.gpio_in.enable);
        CJSON_GET_VALUE_INT(cjson_device, "gpio_in", device->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "ip_inv", device->interface.gpio.gpio_in.invert);
        CJSON_GET_VALUE_INT(cjson_device, "val_ip", device->interface.gpio.gpio_in.value);
        CJSON_GET_VALUE_INT(cjson_device, "pullup_ip", tmp_var);
        device->interface.gpio.gpio_in.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;

        device->interface.gpio.gpio_out.enable = true;
        CJSON_GET_VALUE_INT(cjson_device, "gpio_out", device->interface.gpio.gpio_out.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "op_inv", device->interface.gpio.gpio_out.invert);
        CJSON_GET_VALUE_INT(cjson_device, "val_op", device->interface.gpio.gpio_out.value);
        CJSON_GET_VALUE_INT(cjson_device, "pullup_op", tmp_var);
        device->interface.gpio.gpio_out.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
    }

    return ret;
}

static int8_t digital_io_get_value(char *sensor_data)
{
    int ret = 0;
    return ret;
}

static int digital_io_set_value(void *arg)
{
    int ret = 0;

    return ret;
}

static int digital_io_ezlopi_update_data(void)
{
    int ret = 0;
    // char *data = (char *)malloc(65);
    // char *send_buf = malloc(1024);
    // digital_io_get_value(data);
    // send_buf = items_update_from_sensor(0, data);
    // TRACE_I("The send_buf is: %s, the size is: %d", send_buf, strlen(send_buf));
    // free(data);
    // free(send_buf);
    return 0;
}

static int digital_io_notify_30_seconds(void)
{
    int ret = 0;

    static int seconds_counter;
    seconds_counter = (seconds_counter % 30) ? seconds_counter : 0;

    if (0 == seconds_counter)
    {
        seconds_counter = 0;
        /* Send the value to cloud using web-socket */
        char *data = digital_io_ezlopi_update_data();
        if (data)
        {
            /* Send to ezlo cloud */
        }
    }

    seconds_counter++;
    return ret;
}

static int digital_io_init(void)
{
    int ret = 0;
    return ret;
}
