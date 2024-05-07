
#include "../../build/config/sdkconfig.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_gpio.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_gpioisr.h"

#include "device_0003_digitalOut_plug.h"

static int __prepare(void* arg);
static int __init(l_ezlopi_item_t* item);
static int __set_value(l_ezlopi_item_t* item, void* arg);
static int __get_value_cjson(l_ezlopi_item_t* item, void* arg);

static void __interrupt_upcall(void* arg);
static void __toggle_gpio(l_ezlopi_item_t* item);
static void __write_gpio_value(l_ezlopi_item_t* item);
static void __set_gpio_value(l_ezlopi_item_t* item, int value);

int device_0003_digitalOut_plug(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
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
        ret = __set_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_value_cjson(item, arg);
        break;
    }

    default:
    {
        break;
    }
    }

    return ret;
}

static void __setup_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cjson_device)
{
    device->cloud_properties.category = category_switch;
    device->cloud_properties.subcategory = subcategory_relay;
    device->cloud_properties.device_type = dev_type_switch_inwall;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __setup_item_properties(l_ezlopi_item_t* item, cJSON* cjson_device)
{
    int tmp_var = 0;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_switch;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_dev_type_str, item->interface_type);

    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_is_ip_str, item->interface.gpio.gpio_in.enable);
    CJSON_GET_VALUE_GPIO(cjson_device, ezlopi_gpio_in_str, item->interface.gpio.gpio_in.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_ip_inv_str, item->interface.gpio.gpio_in.invert);
    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_val_ip_str, item->interface.gpio.gpio_in.value);
    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_pullup_ip_str, tmp_var);
    item->interface.gpio.gpio_in.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;

    item->interface.gpio.gpio_out.enable = true;
    CJSON_GET_VALUE_GPIO(cjson_device, ezlopi_gpio_out_str, item->interface.gpio.gpio_out.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_op_inv_str, item->interface.gpio.gpio_out.invert);
    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_val_op_str, item->interface.gpio.gpio_out.value);
    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_pullup_op_str, tmp_var);
    item->interface.gpio.gpio_out.interrupt = GPIO_INTR_DISABLE;
    item->interface.gpio.gpio_out.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
}

static int __prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (arg)
    {
        cJSON* cjson_device = prep_arg->cjson_device;
        if (cjson_device)
        {
            l_ezlopi_device_t* device = ezlopi_device_add_device(cjson_device, NULL);
            if (device)
            {
                __setup_device_cloud_properties(device, cjson_device);
                l_ezlopi_item_t* item = ezlopi_device_add_item_to_device(device, device_0003_digitalOut_plug);
                if (item)
                {
                    __setup_item_properties(item, cjson_device);
                    ret = 1;
                }
                else
                {
                    ezlopi_device_free_device(device);
                    ret = -1;
                }
            }
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {

        if (GPIO_IS_VALID_OUTPUT_GPIO(item->interface.gpio.gpio_out.gpio_num) &&
            (255 != item->interface.gpio.gpio_out.gpio_num))
        {
            const gpio_config_t io_conf = {
                .pin_bit_mask = (1ULL << item->interface.gpio.gpio_out.gpio_num),
                .mode = GPIO_MODE_OUTPUT,
                .pull_up_en = ((item->interface.gpio.gpio_out.pull == GPIO_PULLUP_ONLY) ||
                               (item->interface.gpio.gpio_out.pull == GPIO_PULLUP_PULLDOWN))
                                  ? GPIO_PULLUP_ENABLE
                                  : GPIO_PULLUP_DISABLE,
                .pull_down_en = ((item->interface.gpio.gpio_out.pull == GPIO_PULLDOWN_ONLY) ||
                                 (item->interface.gpio.gpio_out.pull == GPIO_PULLUP_PULLDOWN))
                                    ? GPIO_PULLDOWN_ENABLE
                                    : GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_DISABLE,
            };

            if (0 == gpio_config(&io_conf))
            {
                // digital_io_write_gpio_value(item);
                __write_gpio_value(item);
                ret = 1;
            }
            else
            {
                ret = -1;
            }
        }
        else if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num) &&
            (-1 != item->interface.gpio.gpio_in.gpio_num) &&
            (255 != item->interface.gpio.gpio_in.gpio_num))
        {
            const gpio_config_t io_conf = {
                .pin_bit_mask = (1ULL << item->interface.gpio.gpio_in.gpio_num),
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = ((item->interface.gpio.gpio_in.pull == GPIO_PULLUP_ONLY) ||
                               (item->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                                  ? GPIO_PULLUP_ENABLE
                                  : GPIO_PULLUP_DISABLE,
                .pull_down_en = ((item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ||
                                 (item->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                                    ? GPIO_PULLDOWN_ENABLE
                                    : GPIO_PULLDOWN_DISABLE,
                .intr_type = (GPIO_PULLUP_ONLY == item->interface.gpio.gpio_in.pull)
                                 ? GPIO_INTR_POSEDGE
                                 : GPIO_INTR_NEGEDGE,
            };

            if (0 == gpio_config(&io_conf))
            {
                gpio_isr_service_register_v3(item, __interrupt_upcall, 1000);
                ret = 1;
            }
            else
            {
                ret = -1;
            }
        }
        else
        {
            ret = -1;
        }
    }

    return ret;
}

static int __get_value_cjson(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cj_propertise = (cJSON*)arg;
        if (cj_propertise)
        {
            ezlopi_valueformatter_bool_to_cjson(item, cj_propertise, item->interface.gpio.gpio_out.value);
            ret = 1;
        }
    }
    return ret;
}

static void __set_gpio_value(l_ezlopi_item_t* item, int value)
{
    gpio_set_level(item->interface.gpio.gpio_out.gpio_num, value);
    item->interface.gpio.gpio_out.value = value;
}

static int __set_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cjson_params = (cJSON*)arg;

        if (NULL != cjson_params)
        {
            CJSON_TRACE("cjson_params", cjson_params);

            int value = 0;
            cJSON* cj_value = cJSON_GetObjectItem(__FUNCTION__, cjson_params, ezlopi_value_str);
            if (cj_value)
            {
                switch (cj_value->type)
                {
                case cJSON_False:
                    value = 0;
                    break;
                case cJSON_True:
                    value = 1;
                    break;
                case cJSON_Number:
                    value = cj_value->valuedouble;
                    break;

                default:
                    break;
                }
            }

            TRACE_S("item_name: %s", item->cloud_properties.item_name);
            TRACE_S("gpio_num: %d", item->interface.gpio.gpio_out.gpio_num);
            TRACE_S("item_id: 0x%08x", item->cloud_properties.item_id);
            TRACE_S("prev value: %d", item->interface.gpio.gpio_out.value);
            TRACE_S("cur value: %d", value);

            if (255 != item->interface.gpio.gpio_out.gpio_num)
            {
                if (GPIO_IS_VALID_OUTPUT_GPIO(item->interface.gpio.gpio_out.gpio_num))
                {
                    __set_gpio_value(item, value);
                    ezlopi_device_value_updated_from_device_broadcast(item);
                    ret = 1;
                }
            }
            else
            {
                l_ezlopi_device_t* curr_device = ezlopi_device_get_head();
                while (curr_device)
                {
                    l_ezlopi_item_t* curr_item = curr_device->items;
                    while (curr_item)
                    {
                        if ((EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT == curr_item->interface_type) && (255 != curr_item->interface.gpio.gpio_out.gpio_num))
                        {
                            TRACE_D("GPIO-pin: %d", curr_item->interface.gpio.gpio_out.gpio_num);
                            TRACE_D("value: %d", value);
                            __set_gpio_value(curr_item, value);
                            ezlopi_device_value_updated_from_device_broadcast(curr_item);
                            ret = 1;
                        }
                        curr_item = curr_item->next;
                    }
                    curr_device = curr_device->next;
                }

                item->interface.gpio.gpio_out.value = value;
                ezlopi_device_value_updated_from_device_broadcast(item);
            }
        }
    }
    return ret;
}

static void __write_gpio_value(l_ezlopi_item_t* item)
{
    uint32_t write_value = (0 == item->interface.gpio.gpio_out.invert) ? item->interface.gpio.gpio_out.value : (item->interface.gpio.gpio_out.value ? 0 : 1);
    gpio_set_level(item->interface.gpio.gpio_out.gpio_num, write_value);
}

static void __interrupt_upcall(void* arg)
{
    l_ezlopi_item_t* item = (l_ezlopi_item_t*)arg;
    if (item)
    {
        __toggle_gpio(item);
        ezlopi_device_value_updated_from_device_broadcast(item);
    }
}

static void __toggle_gpio(l_ezlopi_item_t* item)
{
    uint32_t write_value = !(item->interface.gpio.gpio_out.value);
    gpio_set_level(item->interface.gpio.gpio_out.gpio_num, write_value);
    item->interface.gpio.gpio_out.value = write_value;
}
