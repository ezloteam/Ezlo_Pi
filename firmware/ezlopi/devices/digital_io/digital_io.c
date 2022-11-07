#include <string.h>
#include "sdkconfig.h"

#include "sensor_bme280.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"
#include "frozen.h"
#include "trace.h"
#include "cJSON.h"
#include "ezlopi_devices.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"

static int digital_io_prepare(void *arg);
static int digital_io_init(s_ezlopi_device_properties_t *properties);
static int digital_io_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg);
static int digital_io_set_value(s_ezlopi_device_properties_t *properties, void *arg);

int digital_io(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        TRACE_I("EZLOPI_ACTION_PREPARE");
        ret = digital_io_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        TRACE_I("EZLOPI_ACTION_INITIALIZE event.");
        ret = digital_io_init(properties);
        break;
    }
    case EZLOPI_ACTION_SET_VALUE:
    {
        TRACE_I("EZLOPI_ACTION_SET_VALUE event.");
        ret = digital_io_set_value(properties, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = digital_io_get_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = ezlopi_device_value_updated_from_device(properties);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int digital_io_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)arg;
    if (cjson_propertise)
    {
        cJSON_AddBoolToObject(cjson_propertise, "value", properties->interface.gpio.gpio_out.value);
        ret = 1;
    }

    return ret;
}

// Must type cast the 'digital_io_device_properties' to 'int' and return
static int digital_io_prepare(void *arg)
{
    cJSON *cjson_device = (cJSON *)arg;
    s_ezlopi_device_properties_t *digital_io_device_properties = NULL;

    if ((NULL == digital_io_device_properties) && (NULL != cjson_device))
    {
        digital_io_device_properties = malloc(sizeof(s_ezlopi_device_properties_t));

        if (digital_io_device_properties)
        {
            int tmp_var = 0;
            memset(digital_io_device_properties, 0, sizeof(s_ezlopi_device_properties_t));
            digital_io_device_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT;

            char *device_name = NULL;
            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
            ASSIGN_DEVICE_NAME(digital_io_device_properties, device_name);
            digital_io_device_properties->ezlopi_cloud.category = category_switch;
            digital_io_device_properties->ezlopi_cloud.subcategory = subcategory_in_wall;
            digital_io_device_properties->ezlopi_cloud.item_name = ezlopi_item_name_switch;
            digital_io_device_properties->ezlopi_cloud.device_type = dev_type_switch_inwall;
            digital_io_device_properties->ezlopi_cloud.value_type = value_type_bool;
            digital_io_device_properties->ezlopi_cloud.has_getter = true;
            digital_io_device_properties->ezlopi_cloud.has_setter = true;
            digital_io_device_properties->ezlopi_cloud.reachable = true;
            digital_io_device_properties->ezlopi_cloud.battery_powered = false;
            digital_io_device_properties->ezlopi_cloud.show = true;
            digital_io_device_properties->ezlopi_cloud.room_name[0] = '\0';
            digital_io_device_properties->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
            digital_io_device_properties->ezlopi_cloud.room_id = ezlopi_device_generate_room_id();
            digital_io_device_properties->ezlopi_cloud.item_id = ezlopi_device_generate_item_id();
            // CJSON_GET_VALUE_INT(cjson_device, "id_room", digital_io_device_properties->ezlopi_cloud.room_id);
            // CJSON_GET_VALUE_INT(cjson_device, "id_item", digital_io_device_properties->ezlopi_cloud.item_id);

            CJSON_GET_VALUE_INT(cjson_device, "is_ip", digital_io_device_properties->interface.gpio.gpio_in.enable);
            CJSON_GET_VALUE_INT(cjson_device, "gpio_in", digital_io_device_properties->interface.gpio.gpio_in.gpio_num);
            CJSON_GET_VALUE_INT(cjson_device, "ip_inv", digital_io_device_properties->interface.gpio.gpio_in.invert);
            CJSON_GET_VALUE_INT(cjson_device, "val_ip", digital_io_device_properties->interface.gpio.gpio_in.value);
            CJSON_GET_VALUE_INT(cjson_device, "pullup_ip", tmp_var);
            digital_io_device_properties->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
            digital_io_device_properties->interface.gpio.gpio_in.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;

            digital_io_device_properties->interface.gpio.gpio_out.enable = true;
            CJSON_GET_VALUE_INT(cjson_device, "gpio_out", digital_io_device_properties->interface.gpio.gpio_out.gpio_num);
            CJSON_GET_VALUE_INT(cjson_device, "op_inv", digital_io_device_properties->interface.gpio.gpio_out.invert);
            CJSON_GET_VALUE_INT(cjson_device, "val_op", digital_io_device_properties->interface.gpio.gpio_out.value);
            CJSON_GET_VALUE_INT(cjson_device, "pullup_op", tmp_var);
            digital_io_device_properties->interface.gpio.gpio_out.interrupt = GPIO_INTR_DISABLE;
            digital_io_device_properties->interface.gpio.gpio_out.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
        }
    }

    return ((int)digital_io_device_properties);
}

static int digital_io_set_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_params = (cJSON *)arg;

    if (NULL != cjson_params)
    {
        int value = 0;
        CJSON_GET_VALUE_INT(cjson_params, "value", value);

        TRACE_I("item_name: %s", properties->ezlopi_cloud.item_name);
        TRACE_I("gpio_num: %d", properties->interface.gpio.gpio_out.gpio_num);
        TRACE_I("item_id: %d", properties->ezlopi_cloud.item_id);
        TRACE_I("prev value: %d", properties->interface.gpio.gpio_out.value);
        TRACE_I("cur value: %d", value);

        if (GPIO_IS_VALID_OUTPUT_GPIO(properties->interface.gpio.gpio_out.gpio_num))
        {
            gpio_set_level(properties->interface.gpio.gpio_out.gpio_num, value);
            properties->interface.gpio.gpio_out.value = value;
        }
    }

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

static int digital_io_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (GPIO_IS_VALID_OUTPUT_GPIO(properties->interface.gpio.gpio_out.gpio_num))
    {
        const gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << properties->interface.gpio.gpio_out.gpio_num),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = ((properties->interface.gpio.gpio_out.pull == GPIO_PULLUP_ONLY) ||
                           (properties->interface.gpio.gpio_out.pull == GPIO_PULLUP_PULLDOWN))
                              ? GPIO_PULLUP_ENABLE
                              : GPIO_PULLUP_DISABLE,
            .pull_down_en = ((properties->interface.gpio.gpio_out.pull == GPIO_PULLDOWN_ONLY) ||
                             (properties->interface.gpio.gpio_out.pull == GPIO_PULLUP_PULLDOWN))
                                ? GPIO_PULLDOWN_ENABLE
                                : GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        
        gpio_config(&io_conf);
        esp_err_t error = gpio_set_level(properties->interface.gpio.gpio_out.gpio_num, properties->interface.gpio.gpio_out.value);
    }

    if (GPIO_IS_VALID_GPIO(properties->interface.gpio.gpio_in.gpio_num))
    {
        const gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << properties->interface.gpio.gpio_in.gpio_num),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = ((properties->interface.gpio.gpio_in.pull == GPIO_PULLUP_ONLY) ||
                           (properties->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                              ? GPIO_PULLUP_ENABLE
                              : GPIO_PULLUP_DISABLE,
            .pull_down_en = ((properties->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ||
                             (properties->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                                ? GPIO_PULLDOWN_ENABLE
                                : GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };

        gpio_config(&io_conf);
        properties->interface.gpio.gpio_in.value = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
    }
    

    return ret;
}
