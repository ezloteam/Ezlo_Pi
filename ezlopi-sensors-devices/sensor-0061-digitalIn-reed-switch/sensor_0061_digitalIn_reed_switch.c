
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

#include "sensor_0061_digitalIn_reed_switch.h"
//-----------------------------------------------------------------------
const char *reed_door_window_states[] = {
    "dw_is_opened",
    "dw_is_closed",
    "unknown",
};

static ezlopi_error_t __0061_prepare(void *arg);
static ezlopi_error_t __0061_init(l_ezlopi_item_t *item);
static ezlopi_error_t __0061_get_item(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0061_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void _0061_update_from_device(void *arg);
//-----------------------------------------------------------------------

ezlopi_error_t sensor_0061_digitalIn_reed_switch(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0061_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0061_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        ret = __0061_get_item(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0061_get_cjson_value(item, arg);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

//----------------------------------------------------------------------------------------
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_door;
    device->cloud_properties.device_type = dev_type_doorlock;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_dw_state;
    item->cloud_properties.value_type = value_type_token;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.gpio.gpio_in.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_logic_inv_str, item->interface.gpio.gpio_in.invert);

    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
    item->interface.gpio.gpio_in.value = 0;
    item->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
}
static ezlopi_error_t __0061_prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    if (arg)
    {
        s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
        if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
        {
            l_ezlopi_device_t *reed_device = ezlopi_device_add_device(device_prep_arg->cjson_device, NULL, 0);
            if (reed_device)
            {
                __prepare_device_cloud_properties(reed_device, device_prep_arg->cjson_device);
                l_ezlopi_item_t *reed_item = ezlopi_device_add_item_to_device(reed_device, sensor_0061_digitalIn_reed_switch);
                if (reed_item)
                {
                    __prepare_item_cloud_properties(reed_item, device_prep_arg->cjson_device);
                    ret = EZPI_SUCCESS;
                }
                else
                {
                    ezlopi_device_free_device(reed_device);
                }
            }
        }
    }
    return ret;
}

static ezlopi_error_t __0061_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (NULL != item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
        {
            // intialize digital_pin
            gpio_config_t input_conf = {
                .pin_bit_mask = (1ULL << (item->interface.gpio.gpio_in.gpio_num)),
                .mode = item->interface.gpio.gpio_in.mode,
                .pull_down_en = ((item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ||
                                 (item->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                                    ? GPIO_PULLDOWN_ENABLE
                                    : GPIO_PULLDOWN_DISABLE,
                .pull_up_en = ((item->interface.gpio.gpio_in.pull == GPIO_PULLUP_ONLY) ||
                               (item->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                                  ? GPIO_PULLUP_ENABLE
                                  : GPIO_PULLUP_DISABLE,
                .intr_type = item->interface.gpio.gpio_in.interrupt,
            };
            if (ESP_OK == gpio_config(&input_conf))
            {
                item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
                ezlopi_service_gpioisr_register_v3(item, _0061_update_from_device, 200);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static ezlopi_error_t __0061_get_item(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            //-------------------  POSSIBLE JSON ENUM LPGNTENTS ----------------------------------
            cJSON *json_array_enum = cJSON_CreateArray(__FUNCTION__);
            if (NULL != json_array_enum)
            {
                for (uint8_t i = 0; i < REED_DOOR_WINDOW_MAX; i++)
                {
                    cJSON *json_value = cJSON_CreateString(__FUNCTION__, reed_door_window_states[i]);
                    if (NULL != json_value)
                    {
                        cJSON_AddItemToArray(json_array_enum, json_value);
                    }
                }
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_enum_str, json_array_enum);
            }
            //--------------------------------------------------------------------------------------

            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, (char *)item->user_arg ? item->user_arg : reed_door_window_states[1]);
            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, (char *)item->user_arg ? item->user_arg : reed_door_window_states[1]);
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}
static ezlopi_error_t __0061_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, (char *)item->user_arg ? item->user_arg : reed_door_window_states[1]);
            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, (char *)item->user_arg ? item->user_arg : reed_door_window_states[1]);
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

//------------------------------------------------------------------------------------------------------------
static void _0061_update_from_device(void *arg)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)arg;
    if (item)
    {
        const char *curret_value = NULL;
        item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);

        item->interface.gpio.gpio_in.value = (false == item->interface.gpio.gpio_in.invert)
            ? (item->interface.gpio.gpio_in.value)
            : (!item->interface.gpio.gpio_in.value);

        if (0 == (item->interface.gpio.gpio_in.value)) // when D0 -> 0V,
        {
            curret_value = reed_door_window_states[0];
        }
        else
        {
            curret_value = reed_door_window_states[1];
        }

        if (curret_value != (char *)item->user_arg) // calls update only if there is change in state
        {
            item->user_arg = (void *)curret_value;
            ezlopi_device_value_updated_from_device_broadcast(item);
        }
    }
}