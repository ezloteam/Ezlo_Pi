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
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"

static int digital_io_prepare(void *arg);
static int digital_io_init(s_ezlopi_device_properties_t *properties);
static int digital_io_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg);
static int digital_io_set_value(s_ezlopi_device_properties_t *properties, void *arg);
static s_ezlopi_device_properties_t *digital_io_prepare_item(cJSON *cjson_device);
static void digital_io_write_gpio_value(s_ezlopi_device_properties_t *properties);
static uint32_t digital_io_read_gpio_value(s_ezlopi_device_properties_t *properties);
extern void digital_io_isr_service_init(s_ezlopi_device_properties_t *properties);

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
            value = (0 == properties->interface.gpio.gpio_out.invert) ? value : ((EZLOPI_GPIO_LOW == value) ? EZLOPI_GPIO_HIGH : EZLOPI_GPIO_LOW);
            gpio_set_level(properties->interface.gpio.gpio_out.gpio_num, value);
            properties->interface.gpio.gpio_out.value = value;
        }
    }

    return ret;
}

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
            if (0 == ezlopi_devices_list_add(prep_arg->device, digital_io_device_properties))
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

    if (GPIO_IS_VALID_GPIO(properties->interface.gpio.gpio_in.gpio_num))
    {
        // TRACE_W("Setting up gpio_in");
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
        extern void gpio_isr_service_register(s_ezlopi_device_properties_t * properties, void (*__upcall)(s_ezlopi_device_properties_t * properties));
        gpio_isr_service_register(properties, NULL);
        // digital_io_isr_service_init(properties);
    }
    

    return ret;
}

static void digital_io_write_gpio_value(s_ezlopi_device_properties_t *properties)
{
    uint32_t write_value = (0 == properties->interface.gpio.gpio_out.invert) ? properties->interface.gpio.gpio_out.value : (properties->interface.gpio.gpio_out.value ? 0 : 1);
    gpio_set_level(properties->interface.gpio.gpio_out.gpio_num, write_value);
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
        digital_io_device_properties->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
        digital_io_device_properties->ezlopi_cloud.room_id = ezlopi_device_generate_room_id();
        digital_io_device_properties->ezlopi_cloud.item_id = ezlopi_device_generate_item_id();

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