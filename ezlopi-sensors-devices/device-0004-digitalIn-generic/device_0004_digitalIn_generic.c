#include "../../build/config/sdkconfig.h"
#include "ezlopi_util_trace.h"

// #include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_hal_gpio.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_gpioisr.h"

#include "device_0004_digitalIn_generic.h"
static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static void __interrupt_upcall(void *arg);
static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg);

ezlopi_error_t device_0004_digitalIn_generic(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    case EZLOPI_ACTION_SET_VALUE:
    {

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

static void __setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cjson_device)
{
    device->cloud_properties.category = category_switch;
    device->cloud_properties.subcategory = subcategory_in_wall;
    device->cloud_properties.device_type = dev_type_switch_inwall;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __setup_item_properties(l_ezlopi_item_t *item, cJSON *cjson_device)
{
    int tmp_var = 0;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_switch;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface.gpio.gpio_in.enable = true;
    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_dev_type_str, item->interface_type);
    CJSON_GET_VALUE_GPIO(cjson_device, ezlopi_gpio_str, item->interface.gpio.gpio_in.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_logic_inv_str, item->interface.gpio.gpio_in.invert);
    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_pull_up_str, tmp_var);
    item->interface.gpio.gpio_in.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
}

static ezlopi_error_t __prepare(void *arg)
{
    int ret = EZPI_SUCCESS;
    if (arg)
    {
        s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
        cJSON *cjson_device = prep_arg->cjson_device;
        if (cjson_device)
        {
            l_ezlopi_device_t *device = ezlopi_device_add_device(cjson_device, NULL, 0);
            if (device)
            {
                ret = 1;
                __setup_device_cloud_properties(device, cjson_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, device_0004_digitalIn_generic);
                if (item)
                {
                    __setup_item_properties(item, cjson_device);
                }
                else
                {
                    ezlopi_device_free_device(device);
                    ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                }
            }
        }
    }

    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num) &&
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
                ezlopi_service_gpioisr_register_v3(item, __interrupt_upcall, 1000);
            }
            else
            {
                ret = EZPI_ERR_INIT_DEVICE_FAILED;
            }
        }
        else
        {
            ret = EZPI_ERR_INIT_DEVICE_FAILED;
        }
    }

    return ret;
}

static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_propertise = (cJSON *)arg;
        if (cj_propertise)
        {
            ezlopi_valueformatter_bool_to_cjson(cj_propertise, item->interface.gpio.gpio_out.value, NULL);
            ret = EZPI_SUCCESS;
        }
    }

    return ret;
}

static void __interrupt_upcall(void *arg)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)arg;
    if (item)
    {
        item->interface.gpio.gpio_in.value = !item->interface.gpio.gpio_in.value;
        ezlopi_device_value_updated_from_device_broadcast(item);
    }
}
