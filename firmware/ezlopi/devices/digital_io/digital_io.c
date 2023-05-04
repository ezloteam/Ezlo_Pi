#include <string.h>
#include "sdkconfig.h"

#include "cJSON.h"
#include "trace.h"
#include "frozen.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "gpio_isr_service.h"
#include "ezlopi_gpio.h"
#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"

static int digital_io_set_value(s_ezlopi_device_properties_t *properties, void *arg);
static int digital_io_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg);

static int digital_io_prepare(void *arg);
static int digital_io_init(s_ezlopi_device_properties_t *properties);

#if 0
static s_ezlopi_device_properties_t *digital_io_prepare_item(cJSON *cjson_device);
static void digital_io_write_gpio_value(s_ezlopi_device_properties_t *properties);
static uint32_t digital_io_read_gpio_value(s_ezlopi_device_properties_t *properties);
static void digital_io_gpio_interrupt_upcall(s_ezlopi_device_properties_t *properties);
static void digital_io_toggle_gpio(s_ezlopi_device_properties_t *properties);
#endif

int digital_io(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = digital_io_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = digital_io_init(properties);
        break;
    }
    case EZLOPI_ACTION_SET_VALUE:
    {
        ret = digital_io_set_value(properties, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = digital_io_get_value_cjson(properties, arg);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static void digital_io_setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cjson_device)
{
    if (device)
    {
        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        
        ASSIGN_DEVICE_NAME(digital_io_device_properties, device_name);
        device->cloud_properties.category = category_switch;
        device->cloud_properties.subcategory = subcategory_in_wall;
        device->cloud_properties.device_type = dev_type_switch_inwall;
        device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    }
}

static int digital_io_prepare(void *arg)
{
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (arg)
    {
        cJSON *cjson_device = prep_arg->cjson_device;
        if (cjson_device)
        {
            l_ezlopi_device_t *device = ezlopi_device_add_device();
            if (device)
            {
                digital_io_setup_device_cloud_properties(device, cjson_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device);
                if (item)
                {
                }
                else
                {
                    ezlopi_device_free_device(device);
                }
            }
        }

#if 0
        s_ezlopi_device_properties_t *digital_io_device_properties = malloc(sizeof(s_ezlopi_device_properties_t));

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
            digital_io_device_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
            digital_io_device_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            digital_io_device_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

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
#endif
    }
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

static void ___set_gpio_value(s_ezlopi_device_properties_t *properties, int value)
{
    int temp_value = (0 == properties->interface.gpio.gpio_out.invert) ? value : !(value);
    gpio_set_level(properties->interface.gpio.gpio_out.gpio_num, value);
    properties->interface.gpio.gpio_out.value = value;
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

        if (255 != properties->interface.gpio.gpio_out.gpio_num)
        {
            if (GPIO_IS_VALID_OUTPUT_GPIO(properties->interface.gpio.gpio_out.gpio_num))
            {
                ___set_gpio_value(properties, value);
            }
        }
        else
        {
            l_ezlopi_configured_devices_t *configured_devices = ezlopi_devices_list_get_configured_items();

            while (configured_devices)
            {

                if ((EZLOPI_SENSOR_0001_LED == configured_devices->device->id) && (255 != configured_devices->properties->interface.gpio.gpio_out.gpio_num))
                {
                    TRACE_D("device-id: %d", configured_devices->device->id);
                    TRACE_D("GPIO-pin: %d", configured_devices->properties->interface.gpio.gpio_out.gpio_num);
                    TRACE_D("value: %d", value);
                    ___set_gpio_value(configured_devices->properties, value);
                    ezlopi_device_value_updated_from_device(configured_devices->properties);
                }

                configured_devices = configured_devices->next;
            }

            properties->interface.gpio.gpio_out.value = value;
            ezlopi_device_value_updated_from_device(properties);
        }
    }

    return ret;
}

#if 0
static int digital_io_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    cJSON *cjson_device = prep_arg->cjson_device;

    s_ezlopi_device_properties_t *digital_io_device_properties = NULL;

    if ((NULL == digital_io_device_properties) && (NULL != cjson_device))
    {
        digital_io_device_properties = digital_io_prepare_item(cjson_device);
        if (digital_io_device_properties)
        {
            if (0 == ezlopi_devices_list_add(prep_arg->device, digital_io_device_properties, NULL))
            {
                free(digital_io_device_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static int digital_io_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (255 != properties->interface.gpio.gpio_out.gpio_num)
    {
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
            digital_io_write_gpio_value(properties);
        }
    }

    if (GPIO_IS_VALID_GPIO(properties->interface.gpio.gpio_in.gpio_num) &&
        (-1 != properties->interface.gpio.gpio_in.gpio_num) &&
        (255 != properties->interface.gpio.gpio_in.gpio_num))
    {
        const gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << properties->interface.gpio.gpio_in.gpio_num),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = ((properties->interface.gpio.gpio_in.pull == GPIO_PULLUP_ONLY) ||
                           (properties->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                              ? GPIO_PULLUP_ENABLE
                              : GPIO_PULLUP_DISABLE,
            .pull_down_en = ((properties->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ||
                             (properties->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                                ? GPIO_PULLDOWN_ENABLE
                                : GPIO_PULLDOWN_DISABLE,
            .intr_type = (GPIO_PULLUP_ONLY == properties->interface.gpio.gpio_in.pull)
                             ? GPIO_INTR_POSEDGE
                             : GPIO_INTR_NEGEDGE,
        };

        gpio_config(&io_conf);
        gpio_isr_service_register(properties, digital_io_gpio_interrupt_upcall, 1000);
    }

    return ret;
}

static void digital_io_gpio_interrupt_upcall(s_ezlopi_device_properties_t *properties)
{
    digital_io_toggle_gpio(properties);
    ezlopi_device_value_updated_from_device(properties);
}

static void digital_io_toggle_gpio(s_ezlopi_device_properties_t *properties)
{
    uint32_t write_value = !(properties->interface.gpio.gpio_out.value);
    esp_err_t error = gpio_set_level(properties->interface.gpio.gpio_out.gpio_num, write_value);
    properties->interface.gpio.gpio_out.value = write_value;
}

static void digital_io_write_gpio_value(s_ezlopi_device_properties_t *properties)
{
    uint32_t write_value = (0 == properties->interface.gpio.gpio_out.invert) ? properties->interface.gpio.gpio_out.value : (properties->interface.gpio.gpio_out.value ? 0 : 1);
    esp_err_t error = gpio_set_level(properties->interface.gpio.gpio_out.gpio_num, write_value);
}

static uint32_t digital_io_read_gpio_value(s_ezlopi_device_properties_t *properties)
{
    uint32_t read_value = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
    read_value = (0 == properties->interface.gpio.gpio_in.invert) ? read_value : (read_value ? 0 : 1);
    return read_value;
}

static s_ezlopi_device_properties_t *digital_io_prepare_item(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *digital_io_device_properties = malloc(sizeof(s_ezlopi_device_properties_t));

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
        digital_io_device_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        digital_io_device_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        digital_io_device_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

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

    return digital_io_device_properties;
}
#endif
