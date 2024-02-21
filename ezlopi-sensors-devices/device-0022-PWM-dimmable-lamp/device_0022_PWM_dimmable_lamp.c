#include <math.h>
#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_pwm.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "device_0022_PWM_dimmable_lamp.h"

typedef struct s_dimmable_bulb_properties
{
    bool dimmable_bulb_initialized;
    uint32_t previous_brightness_value;
    uint32_t current_brightness_value;
    l_ezlopi_item_t *item_dimmer;
    l_ezlopi_item_t *item_dimmer_up;
    l_ezlopi_item_t *item_dimmer_down;
    l_ezlopi_item_t *item_dimmer_stop;
    l_ezlopi_item_t *item_dimmer_switch;

} s_dimmable_bulb_properties_t;

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __list_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __set_cjson_value(l_ezlopi_item_t *item, void *arg);

int device_0022_PWM_dimmable_lamp(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
        ret = __set_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
        __list_cjson_value(item, arg);
        break;
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_cjson_value(item, arg);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int __set_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    cJSON *device_details = (cJSON *)arg;
    if (device_details)
    {
        s_dimmable_bulb_properties_t *dimmable_bulb_arg = (s_dimmable_bulb_properties_t *)item->user_arg;
        if (dimmable_bulb_arg)
        {
            if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
            {
                int value = 0;
                CJSON_GET_VALUE_INT(device_details, ezlopi_value_str, value);
                int target_value = (int)ceil(((value * 4095.0) / 100.0));
                ezlopi_pwm_change_duty(item->interface.pwm.channel, item->interface.pwm.speed_mode, target_value);
                dimmable_bulb_arg->previous_brightness_value = dimmable_bulb_arg->current_brightness_value;
                dimmable_bulb_arg->current_brightness_value = target_value;
                ezlopi_device_value_updated_from_device_v3(dimmable_bulb_arg->item_dimmer);
                ezlopi_device_value_updated_from_device_v3(dimmable_bulb_arg->item_dimmer_switch);
            }
            else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
            {
                bool switch_state = false;
                CJSON_GET_VALUE_INT(device_details, ezlopi_value_str, switch_state);
                dimmable_bulb_arg->previous_brightness_value = (false == switch_state) ? dimmable_bulb_arg->current_brightness_value : dimmable_bulb_arg->previous_brightness_value;
                dimmable_bulb_arg->current_brightness_value = (false == switch_state) ? 0 : (0 == dimmable_bulb_arg->previous_brightness_value ? 4095 : dimmable_bulb_arg->previous_brightness_value);
                ezlopi_pwm_change_duty(dimmable_bulb_arg->item_dimmer->interface.pwm.channel, dimmable_bulb_arg->item_dimmer->interface.pwm.speed_mode, dimmable_bulb_arg->current_brightness_value);
                ezlopi_device_value_updated_from_device_v3(dimmable_bulb_arg->item_dimmer);
                ezlopi_device_value_updated_from_device_v3(dimmable_bulb_arg->item_dimmer_switch);
            }
        }
    }

    return ret;
}

static int __list_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    cJSON *param = (cJSON *)arg;
    if (param && item && item->user_arg)
    {
        s_dimmable_bulb_properties_t *dimmable_bulb_arg = (s_dimmable_bulb_properties_t *)item->user_arg;
        if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
        {
            int dimmable_value_percentage = (int)floor(((dimmable_bulb_arg->current_brightness_value * 100.0) / 4095.0));
            cJSON_AddNumberToObject(param, ezlopi_value_str, dimmable_value_percentage);
            char *formatted_val = ezlopi_valueformatter_int(dimmable_value_percentage);
            if (formatted_val)
            {
                cJSON_AddStringToObject(param, ezlopi_valueFormatted_str, formatted_val);
                free(formatted_val);
            }
            cJSON_AddNumberToObject(param, "minValue", 0);
            cJSON_AddNumberToObject(param, "maxValue", 100);
        }
        else if (ezlopi_item_name_dimmer_up == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(param, ezlopi_value_str, 0);
            char *formatted_val = ezlopi_valueformatter_int(0);
            if (formatted_val)
            {
                cJSON_AddStringToObject(param, ezlopi_valueFormatted_str, formatted_val);
                free(formatted_val);
            }
        }
        else if (ezlopi_item_name_dimmer_down == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(param, ezlopi_value_str, 0);
            char *formatted_val = ezlopi_valueformatter_int(0);
            if (formatted_val)
            {
                cJSON_AddStringToObject(param, ezlopi_valueFormatted_str, formatted_val);
                free(formatted_val);
            }
        }
        else if (ezlopi_item_name_dimmer_stop == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(param, ezlopi_value_str, 0);
            char *formatted_val = ezlopi_valueformatter_int(0);
            if (formatted_val)
            {
                cJSON_AddStringToObject(param, ezlopi_valueFormatted_str, formatted_val);
                free(formatted_val);
            }
        }
        else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
        {
            bool switch_state = (0 == dimmable_bulb_arg->current_brightness_value) ? false : true;
            cJSON_AddBoolToObject(param, ezlopi_value_str, switch_state);
            cJSON_AddStringToObject(param, ezlopi_valueFormatted_str, ezlopi_valueformatter_bool(switch_state));
        }
    }

    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    cJSON *param = (cJSON *)arg;
    if (param && item && item->user_arg)
    {
        s_dimmable_bulb_properties_t *dimmable_bulb_arg = (s_dimmable_bulb_properties_t *)item->user_arg;
        if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
        {
            int dimmable_value_percentage = (int)floor(((dimmable_bulb_arg->current_brightness_value * 100.0) / 4095.0));
            cJSON_AddNumberToObject(param, ezlopi_value_str, dimmable_value_percentage);
            char *formatted_val = ezlopi_valueformatter_int(dimmable_value_percentage);
            cJSON_AddStringToObject(param, ezlopi_valueFormatted_str, formatted_val);
            free(formatted_val);
        }
        else if (ezlopi_item_name_dimmer_up == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(param, ezlopi_value_str, 0);
            char *formatted_val = ezlopi_valueformatter_int(0);
            if (formatted_val)
            {
                cJSON_AddStringToObject(param, ezlopi_valueFormatted_str, formatted_val);
                free(formatted_val);
            }
        }
        else if (ezlopi_item_name_dimmer_down == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(param, ezlopi_value_str, 0);
            char *formatted_val = ezlopi_valueformatter_int(0);
            if (formatted_val)
            {
                cJSON_AddStringToObject(param, ezlopi_valueFormatted_str, formatted_val);
                free(formatted_val);
            }
        }
        else if (ezlopi_item_name_dimmer_stop == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(param, ezlopi_value_str, 0);
            char *formatted_val = ezlopi_valueformatter_int(0);
            if (formatted_val)
            {
                cJSON_AddStringToObject(param, ezlopi_valueFormatted_str, formatted_val);
                free(formatted_val);
            }
        }
        else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
        {
            bool switch_state = (0 == dimmable_bulb_arg->current_brightness_value) ? false : true;
            cJSON_AddBoolToObject(param, ezlopi_value_str, switch_state);
            cJSON_AddStringToObject(param, ezlopi_valueFormatted_str, ezlopi_valueformatter_bool(switch_state));
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.pwm.gpio_num))
        {
            s_dimmable_bulb_properties_t *dimmable_bulb_arg = (s_dimmable_bulb_properties_t *)item->user_arg;
            if (dimmable_bulb_arg)
            {
                if (0 == dimmable_bulb_arg->dimmable_bulb_initialized)
                {
                    s_ezlopi_channel_speed_t *ezlopi_dimmable_channel_speed = ezlopi_pwm_init(item->interface.pwm.gpio_num, item->interface.pwm.pwm_resln,
                                                                                              item->interface.pwm.freq_hz, item->interface.pwm.duty_cycle);
                    if (ezlopi_dimmable_channel_speed)
                    {
                        item->interface.pwm.channel = ezlopi_dimmable_channel_speed->channel;
                        TRACE_I("Channel is %d", item->interface.pwm.channel);
                        item->interface.pwm.speed_mode = ezlopi_dimmable_channel_speed->speed_mode;
                        dimmable_bulb_arg->current_brightness_value = item->interface.pwm.duty_cycle;
                        dimmable_bulb_arg->previous_brightness_value = item->interface.pwm.duty_cycle;
                        dimmable_bulb_arg->dimmable_bulb_initialized = true;
                        ezlopi_pwm_change_duty(item->interface.pwm.channel, item->interface.pwm.speed_mode, item->interface.pwm.duty_cycle);
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
            else
            {
                ret = -1;
                ezlopi_device_free_device_by_item(item);
            }
        }
        else
        {
            ret = -1;
            ezlopi_device_free_device_by_item(item);
        }
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    // char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
    // ASSIGN_DEVICE_NAME_V2(device, device_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

    device->cloud_properties.category = category_dimmable_light;
    device->cloud_properties.subcategory = subcategory_dimmable_bulb;
    device->cloud_properties.device_type = dev_type_dimmer_outlet;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_dimmer_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_name_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "duty_cycle", item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_INT(cj_device, "freq_hz", item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_dimmer_up_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_up;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_name_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "duty_cycle", item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_INT(cj_device, "freq_hz", item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_dimmer_down_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_down;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_name_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "duty_cycle", item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_INT(cj_device, "freq_hz", item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_dimmer_stop_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_stop;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_name_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "duty_cycle", item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_INT(cj_device, "freq_hz", item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_dimmer_switch_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.item_name = ezlopi_item_name_switch;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_name_str, item->interface.gpio.gpio_out.gpio_num);
    item->interface.gpio.gpio_in.enable = false;
    item->interface.gpio.gpio_out.enable = true;
    item->interface.gpio.gpio_out.interrupt = false;
    item->interface.gpio.gpio_out.invert = false;
    item->interface.gpio.gpio_out.mode = GPIO_MODE_OUTPUT;
    item->interface.gpio.gpio_out.pull = false;
    item->interface.gpio.gpio_out.value = true;
}

static int __prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *device = ezlopi_device_add_device(prep_arg->cjson_device);
        if (device)
        {
            __prepare_device_cloud_properties(device, prep_arg->cjson_device);

            s_dimmable_bulb_properties_t *dimmable_bulb_arg = malloc(sizeof(s_dimmable_bulb_properties_t));
            if (dimmable_bulb_arg)
            {
                dimmable_bulb_arg->current_brightness_value = 0;
                dimmable_bulb_arg->previous_brightness_value = 4095;
                dimmable_bulb_arg->dimmable_bulb_initialized = false;

                dimmable_bulb_arg->item_dimmer = ezlopi_device_add_item_to_device(device, device_0022_PWM_dimmable_lamp);
                if (dimmable_bulb_arg->item_dimmer)
                {
                    dimmable_bulb_arg->item_dimmer->user_arg = dimmable_bulb_arg;
                    dimmable_bulb_arg->item_dimmer->cloud_properties.device_id = device->cloud_properties.device_id;
                    __prepare_dimmer_item_properties(dimmable_bulb_arg->item_dimmer, prep_arg->cjson_device);
                }

                dimmable_bulb_arg->item_dimmer_up = ezlopi_device_add_item_to_device(device, device_0022_PWM_dimmable_lamp);
                if (dimmable_bulb_arg->item_dimmer_up)
                {
                    dimmable_bulb_arg->item_dimmer_up->user_arg = dimmable_bulb_arg;
                    dimmable_bulb_arg->item_dimmer_up->cloud_properties.device_id = device->cloud_properties.device_id;
                    __prepare_dimmer_up_item_properties(dimmable_bulb_arg->item_dimmer_up, prep_arg->cjson_device);
                }

                dimmable_bulb_arg->item_dimmer_down = ezlopi_device_add_item_to_device(device, device_0022_PWM_dimmable_lamp);
                if (dimmable_bulb_arg->item_dimmer_down)
                {
                    dimmable_bulb_arg->item_dimmer_down->user_arg = dimmable_bulb_arg;
                    dimmable_bulb_arg->item_dimmer_down->cloud_properties.device_id = device->cloud_properties.device_id;
                    __prepare_dimmer_down_item_properties(dimmable_bulb_arg->item_dimmer_down, prep_arg->cjson_device);
                }

                dimmable_bulb_arg->item_dimmer_stop = ezlopi_device_add_item_to_device(device, device_0022_PWM_dimmable_lamp);
                if (dimmable_bulb_arg->item_dimmer_stop)
                {
                    dimmable_bulb_arg->item_dimmer_stop->user_arg = dimmable_bulb_arg;
                    dimmable_bulb_arg->item_dimmer_stop->cloud_properties.device_id = device->cloud_properties.device_id;
                    __prepare_dimmer_stop_item_properties(dimmable_bulb_arg->item_dimmer_stop, prep_arg->cjson_device);
                }

                dimmable_bulb_arg->item_dimmer_switch = ezlopi_device_add_item_to_device(device, device_0022_PWM_dimmable_lamp);
                if (dimmable_bulb_arg->item_dimmer_switch)
                {
                    dimmable_bulb_arg->item_dimmer_switch->user_arg = dimmable_bulb_arg;
                    dimmable_bulb_arg->item_dimmer_switch->cloud_properties.device_id = device->cloud_properties.device_id;
                    __prepare_dimmer_switch_item_properties(dimmable_bulb_arg->item_dimmer_switch, prep_arg->cjson_device);
                }

                ret = 1;
                if ((NULL == dimmable_bulb_arg->item_dimmer) || (NULL == dimmable_bulb_arg->item_dimmer_up) || (NULL == dimmable_bulb_arg->item_dimmer_down) || (NULL == dimmable_bulb_arg->item_dimmer_stop) || (NULL == dimmable_bulb_arg->item_dimmer_switch))
                {
                    ezlopi_device_free_device(device);
                    free(dimmable_bulb_arg);
                    ret = -1;
                }
            }
            else
            {
                ezlopi_device_free_device(device);
                ret = -1;
            }
        }
    }

    return ret;
}
