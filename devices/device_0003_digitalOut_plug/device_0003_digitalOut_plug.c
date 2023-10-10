#include <string.h>
#include "sdkconfig.h"

#include "cJSON.h"
#include "trace.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "gpio_isr_service.h"
#include "ezlopi_gpio.h"
#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"

static int device_0003_digitalOut_plug_prepare(void *arg);
static int device_0003_digitalOut_plug_init(s_ezlopi_device_properties_t *properties);
static int device_0003_digitalOut_plug_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg);
static int device_0003_digitalOut_plug_get_item(s_ezlopi_device_properties_t *properties, void *arg);
static int device_0003_digitalOut_plug_set_value(s_ezlopi_device_properties_t *properties, void *arg);
static s_ezlopi_device_properties_t *device_0003_digitalOut_plug_prepare_item(cJSON *cjson_device);
static void device_0003_digitalOut_plug_write_gpio_value(s_ezlopi_device_properties_t *properties);
static uint32_t device_0003_digitalOut_plug_read_gpio_value(s_ezlopi_device_properties_t *properties);
static void device_0003_digitalOut_plug_gpio_interrupt_upcall(s_ezlopi_device_properties_t *properties);
static void device_0003_digitalOut_plug_toggle_gpio(s_ezlopi_device_properties_t *properties);

int device_0003_digitalOut_plug(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = device_0003_digitalOut_plug_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = device_0003_digitalOut_plug_init(properties);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        ret = device_0003_digitalOut_plug_get_item(properties, arg);
        break;
    }
    case EZLOPI_ACTION_SET_VALUE:
    {
        ret = device_0003_digitalOut_plug_set_value(properties, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = device_0003_digitalOut_plug_get_value_cjson(properties, arg);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int device_0003_digitalOut_plug_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg)
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

static int device_0003_digitalOut_plug_get_item(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)arg;
    if (cjson_propertise)
    {
        cJSON_AddBoolToObject(cjson_propertise, "value", properties->interface.gpio.gpio_out.value);
        cJSON_AddBoolToObject(cjson_propertise, "valueFormatted", properties->interface.gpio.gpio_out.value);
    }
    return ret;
}
static void ___set_gpio_value(s_ezlopi_device_properties_t *properties, int value)
{
    int temp_value = (0 == properties->interface.gpio.gpio_out.invert) ? value : !(value);
    gpio_set_level(properties->interface.gpio.gpio_out.gpio_num, value);
    properties->interface.gpio.gpio_out.value = value;
}

static int device_0003_digitalOut_plug_set_value(s_ezlopi_device_properties_t *properties, void *arg)
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

                if ((EZLOPI_DEVICE_0001_DIGITAL_OUT_LED == configured_devices->device->id) && (255 != configured_devices->properties->interface.gpio.gpio_out.gpio_num))
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

static int device_0003_digitalOut_plug_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    cJSON *cjson_device = prep_arg->cjson_device;

    s_ezlopi_device_properties_t *device_0003_digitalOut_plug_device_properties = NULL;

    if ((NULL == device_0003_digitalOut_plug_device_properties) && (NULL != cjson_device))
    {
        device_0003_digitalOut_plug_device_properties = device_0003_digitalOut_plug_prepare_item(cjson_device);
        if (device_0003_digitalOut_plug_device_properties)
        {
            if (0 == ezlopi_devices_list_add(prep_arg->device, device_0003_digitalOut_plug_device_properties, NULL))
            {
                free(device_0003_digitalOut_plug_device_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static int device_0003_digitalOut_plug_init(s_ezlopi_device_properties_t *properties)
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
            device_0003_digitalOut_plug_write_gpio_value(properties);
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
        gpio_isr_service_register(properties, device_0003_digitalOut_plug_gpio_interrupt_upcall, 1000);
    }

    return ret;
}

static void device_0003_digitalOut_plug_gpio_interrupt_upcall(s_ezlopi_device_properties_t *properties)
{
    device_0003_digitalOut_plug_toggle_gpio(properties);
    ezlopi_device_value_updated_from_device(properties);
}

static void device_0003_digitalOut_plug_toggle_gpio(s_ezlopi_device_properties_t *properties)
{
    uint32_t write_value = !(properties->interface.gpio.gpio_out.value);
    esp_err_t error = gpio_set_level(properties->interface.gpio.gpio_out.gpio_num, write_value);
    properties->interface.gpio.gpio_out.value = write_value;
}

static void device_0003_digitalOut_plug_write_gpio_value(s_ezlopi_device_properties_t *properties)
{
    uint32_t write_value = (0 == properties->interface.gpio.gpio_out.invert) ? properties->interface.gpio.gpio_out.value : (properties->interface.gpio.gpio_out.value ? 0 : 1);
    esp_err_t error = gpio_set_level(properties->interface.gpio.gpio_out.gpio_num, write_value);
}

static uint32_t device_0003_digitalOut_plug_read_gpio_value(s_ezlopi_device_properties_t *properties)
{
    uint32_t read_value = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
    read_value = (0 == properties->interface.gpio.gpio_in.invert) ? read_value : (read_value ? 0 : 1);
    return read_value;
}

static s_ezlopi_device_properties_t *device_0003_digitalOut_plug_prepare_item(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *device_0003_digitalOut_plug_device_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (device_0003_digitalOut_plug_device_properties)
    {
        int tmp_var = 0;
        memset(device_0003_digitalOut_plug_device_properties, 0, sizeof(s_ezlopi_device_properties_t));
        device_0003_digitalOut_plug_device_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(device_0003_digitalOut_plug_device_properties, device_name);
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.category = category_switch;
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.subcategory = subcategory_relay;
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.item_name = ezlopi_item_name_switch;
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.device_type = dev_type_switch_inwall;
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.value_type = value_type_bool;
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.has_getter = true;
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.has_setter = true;
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.reachable = true;
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.battery_powered = false;
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.show = true;
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.room_name[0] = '\0';
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        device_0003_digitalOut_plug_device_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "is_ip", device_0003_digitalOut_plug_device_properties->interface.gpio.gpio_in.enable);
        CJSON_GET_VALUE_INT(cjson_device, "gpio_in", device_0003_digitalOut_plug_device_properties->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "ip_inv", device_0003_digitalOut_plug_device_properties->interface.gpio.gpio_in.invert);
        CJSON_GET_VALUE_INT(cjson_device, "val_ip", device_0003_digitalOut_plug_device_properties->interface.gpio.gpio_in.value);
        CJSON_GET_VALUE_INT(cjson_device, "pullup_ip", tmp_var);
        device_0003_digitalOut_plug_device_properties->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
        device_0003_digitalOut_plug_device_properties->interface.gpio.gpio_in.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;

        device_0003_digitalOut_plug_device_properties->interface.gpio.gpio_out.enable = true;
        CJSON_GET_VALUE_INT(cjson_device, "gpio_out", device_0003_digitalOut_plug_device_properties->interface.gpio.gpio_out.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "op_inv", device_0003_digitalOut_plug_device_properties->interface.gpio.gpio_out.invert);
        CJSON_GET_VALUE_INT(cjson_device, "val_op", device_0003_digitalOut_plug_device_properties->interface.gpio.gpio_out.value);
        CJSON_GET_VALUE_INT(cjson_device, "pullup_op", tmp_var);
        device_0003_digitalOut_plug_device_properties->interface.gpio.gpio_out.interrupt = GPIO_INTR_DISABLE;
        device_0003_digitalOut_plug_device_properties->interface.gpio.gpio_out.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
    }

    return device_0003_digitalOut_plug_device_properties;
}