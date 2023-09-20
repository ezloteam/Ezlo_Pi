#include "cJSON.h"
#include "trace.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "device_0022_PWM_dimmable_lamp.h"
#include "math.h"

#define ADD_PROPERTIES_DEVICE_LIST(_properties, device_id, category, sub_category, item_name, value_type, cjson_device, dimmable_bulb_state)                \
    {                                                                                                                                                       \
        _properties = ezlopi_dimmable_bulb_prepare_properties(device_id, category, sub_category, item_name, value_type, cjson_device, dimmable_bulb_state); \
        if (NULL != _properties)                                                                                                                            \
        {                                                                                                                                                   \
            add_device_to_list(device_prep_arg, _properties, NULL);                                                                                         \
        }                                                                                                                                                   \
    }

static int ezlopi_dimmable_bulb_prepare(void *args);
static s_ezlopi_device_properties_t *ezlopi_dimmable_bulb_prepare_properties(uint32_t device_id, const char *category,
                                                                             const char *sub_category, const char *item_name,
                                                                             const char *value_type, cJSON *cjson_device, ezlopi_dimmable_bulb_state_struct_t* dimmable_bulb);
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_args);
static int ezlopi_dimmable_bulb_init(s_ezlopi_device_properties_t *properties);
static int ezlopi_dimmable_bulb_set_value(s_ezlopi_device_properties_t *properties, void *arg);
static int ezlopi_dimmable_bulb_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);

static bool dimmable_bulb_initialized = false;

static s_ezlopi_device_properties_t *switch_properties = NULL;
static s_ezlopi_device_properties_t *dimmer_properties = NULL;
static s_ezlopi_device_properties_t *dimmer_up_properties = NULL;
static s_ezlopi_device_properties_t *dimmer_down_properties = NULL;
static s_ezlopi_device_properties_t *dimmer_stop_properties = NULL;

int device_0022_PWM_dimmable_lamp(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = ezlopi_dimmable_bulb_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = ezlopi_dimmable_bulb_init(properties);
        break;
    }
    case EZLOPI_ACTION_SET_VALUE:
    {
        ret = ezlopi_dimmable_bulb_set_value(properties, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ezlopi_dimmable_bulb_get_value_cjson(properties, arg);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int ezlopi_dimmable_bulb_prepare(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        ezlopi_dimmable_bulb_state_struct_t *dimmable_bulb_state = (ezlopi_dimmable_bulb_state_struct_t *)malloc(sizeof(ezlopi_dimmable_bulb_state_struct_t));
        if (dimmable_bulb_state)
        {
            uint32_t device_id = ezlopi_cloud_generate_device_id();

            ADD_PROPERTIES_DEVICE_LIST(dimmer_properties, device_id, category_dimmable_light, subcategory_dimmable_bulb, ezlopi_item_name_dimmer, value_type_int, device_prep_arg->cjson_device, dimmable_bulb_state);
            ADD_PROPERTIES_DEVICE_LIST(dimmer_up_properties, device_id, category_dimmable_light, subcategory_dimmable_bulb, ezlopi_item_name_dimmer_up, value_type_int, device_prep_arg->cjson_device, dimmable_bulb_state);
            ADD_PROPERTIES_DEVICE_LIST(dimmer_down_properties, device_id, category_dimmable_light, subcategory_dimmable_bulb, ezlopi_item_name_dimmer_down, value_type_int, device_prep_arg->cjson_device, dimmable_bulb_state);
            ADD_PROPERTIES_DEVICE_LIST(dimmer_stop_properties, device_id, category_dimmable_light, subcategory_dimmable_bulb, ezlopi_item_name_dimmer_stop, value_type_int, device_prep_arg->cjson_device, dimmable_bulb_state);
            ADD_PROPERTIES_DEVICE_LIST(switch_properties, device_id, category_dimmable_light, subcategory_dimmable_bulb, ezlopi_item_name_switch, value_type_bool, device_prep_arg->cjson_device, dimmable_bulb_state);
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *ezlopi_dimmable_bulb_prepare_properties(uint32_t device_id, const char *category,
                                                                             const char *sub_category, const char *item_name,
                                                                             const char *value_type, cJSON *cjson_device, ezlopi_dimmable_bulb_state_struct_t *dimmable_bulb_state)
{
    s_ezlopi_device_properties_t *ezlopi_dimmable_bulb_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (ezlopi_dimmable_bulb_properties)
    {
        memset(ezlopi_dimmable_bulb_properties, 0, sizeof(s_ezlopi_device_properties_t));

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(ezlopi_dimmable_bulb_properties, device_name);
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.category = category;
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.subcategory = sub_category;
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.item_name = item_name;
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.device_type = dev_type_dimmer_outlet;
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.value_type = value_type;
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.has_getter = true;
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.has_setter = true;
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.reachable = true;
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.battery_powered = false;
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.show = true;
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.room_name[0] = '\0';
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.device_id = device_id;
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        ezlopi_dimmable_bulb_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();
        ezlopi_dimmable_bulb_properties->user_arg = dimmable_bulb_state;

        if (ezlopi_item_name_switch == item_name)
        {
            ezlopi_dimmable_bulb_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;
            ezlopi_dimmable_bulb_properties->interface.gpio.gpio_in.enable = true;
            CJSON_GET_VALUE_INT(cjson_device, "gpio", ezlopi_dimmable_bulb_properties->interface.gpio.gpio_in.gpio_num);
            ezlopi_dimmable_bulb_properties->interface.gpio.gpio_in.interrupt = false;
            ezlopi_dimmable_bulb_properties->interface.gpio.gpio_in.invert = false;
            ezlopi_dimmable_bulb_properties->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
            ezlopi_dimmable_bulb_properties->interface.gpio.gpio_in.pull = false;
            ezlopi_dimmable_bulb_properties->interface.gpio.gpio_in.value = true;
            ezlopi_dimmable_bulb_properties->interface.gpio.gpio_out.enable = false;
        }
        else
        {
            ezlopi_dimmable_bulb_properties->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
            CJSON_GET_VALUE_INT(cjson_device, "gpio", ezlopi_dimmable_bulb_properties->interface.pwm.gpio_num);
            CJSON_GET_VALUE_INT(cjson_device, "duty_cycle", ezlopi_dimmable_bulb_properties->interface.pwm.duty_cycle);
            CJSON_GET_VALUE_INT(cjson_device, "freq_hz", ezlopi_dimmable_bulb_properties->interface.pwm.freq_hz);
            ezlopi_dimmable_bulb_properties->interface.pwm.pwm_resln = 12;
        }
    }

    return ezlopi_dimmable_bulb_properties;
}

static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_args)
{
    int ret = 0;

    if (properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, properties, user_args))
        {
            free(properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

static int ezlopi_dimmable_bulb_init(s_ezlopi_device_properties_t *properties)
{
    int ret = -1;

    ezlopi_dimmable_bulb_state_struct_t* dimmable_bulb_state = (ezlopi_dimmable_bulb_state_struct_t*)properties->user_arg;
    if ((!dimmable_bulb_initialized) && (NULL != dimmable_bulb_state))
    {

        static s_ezlopi_channel_speed_t *ezlopi_dimmable_channel_speed = NULL;
        if (GPIO_IS_VALID_GPIO(properties->interface.pwm.gpio_num))
        {
            ezlopi_dimmable_channel_speed = ezlopi_pwm_init(properties->interface.pwm.gpio_num, properties->interface.pwm.pwm_resln,
                                                            properties->interface.pwm.freq_hz, properties->interface.pwm.duty_cycle);
            properties->interface.pwm.channel = ezlopi_dimmable_channel_speed->channel;
            properties->interface.pwm.speed_mode = ezlopi_dimmable_channel_speed->speed_mode;
            dimmable_bulb_state->current_brightness_value = properties->interface.pwm.duty_cycle;
            dimmable_bulb_state->previous_brightness_value = properties->interface.pwm.duty_cycle;
            ret = 0;
        }
        dimmable_bulb_initialized = true;
    }
    return ret;
}

static int ezlopi_dimmable_bulb_set_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_params = (cJSON *)arg;
    ezlopi_dimmable_bulb_state_struct_t* dimmable_bulb_state = (ezlopi_dimmable_bulb_state_struct_t*)properties->user_arg;

    if ((NULL != cjson_params) && (NULL != dimmable_bulb_state))
    {
        
        if (ezlopi_item_name_dimmer == properties->ezlopi_cloud.item_name)
        {
            
            int value = 0;
            CJSON_GET_VALUE_INT(cjson_params, "value", value);
            // TRACE_I("item_name: %s", properties->ezlopi_cloud.item_name);
            // TRACE_I("gpio_num: %d", properties->interface.pwm.gpio_num);
            // TRACE_I("item_id: %d", properties->ezlopi_cloud.item_id);
            TRACE_I("cur value: %d", value);

            if (GPIO_IS_VALID_OUTPUT_GPIO(properties->interface.pwm.gpio_num))
            {
                int target_value = (int)ceil(((value * 4095.0) / 100.0));
                // TRACE_B("target_value is %d", target_value);
                ezlopi_pwm_change_duty(properties->interface.pwm.channel, properties->interface.pwm.speed_mode, target_value);
                dimmable_bulb_state->previous_brightness_value = dimmable_bulb_state->current_brightness_value;
                dimmable_bulb_state->current_brightness_value = target_value;
                // TRACE_B("target_value is %d", dimmable_bulb_state->current_brightness_value);
                ezlopi_device_value_updated_from_device(switch_properties);
            }
        }
        else if (ezlopi_item_name_switch == properties->ezlopi_cloud.item_name)
        {
            int switch_state = 0;
            CJSON_GET_VALUE_INT(cjson_params, "value", switch_state);
            // TRACE_I("cur value: %d", switch_state);

            if (GPIO_IS_VALID_OUTPUT_GPIO(properties->interface.gpio.gpio_in.gpio_num))
            {
                dimmable_bulb_state->previous_brightness_value = (false == switch_state) ? dimmable_bulb_state->current_brightness_value : dimmable_bulb_state->previous_brightness_value;
                dimmable_bulb_state->current_brightness_value = (false == switch_state) ? 0 : dimmable_bulb_state->previous_brightness_value;
                // TRACE_B("target_value is %d", dimmable_bulb_state->current_brightness_value);
                ezlopi_pwm_change_duty(dimmer_properties->interface.pwm.channel, dimmer_properties->interface.pwm.speed_mode, dimmable_bulb_state->current_brightness_value);
                ezlopi_device_value_updated_from_device(dimmer_properties);
            }
        }
    }
    return ret;
}

static int ezlopi_dimmable_bulb_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)args;
    ezlopi_dimmable_bulb_state_struct_t* dimmable_bulb_state = (ezlopi_dimmable_bulb_state_struct_t*)properties->user_arg;
    if ((cjson_propertise) && (NULL != dimmable_bulb_state))
    {
        if (ezlopi_item_name_dimmer == properties->ezlopi_cloud.item_name)
        {
            int dimmable_value_percentage = (int)floor(((dimmable_bulb_state->current_brightness_value * 100.0) / 4095.0));
            TRACE_B("target_value is %d", dimmable_value_percentage);
            cJSON_AddNumberToObject(cjson_propertise, "value", dimmable_value_percentage);
        }
        else if (ezlopi_item_name_switch == properties->ezlopi_cloud.item_name)
        {
            bool switch_state = (0 == dimmable_bulb_state->current_brightness_value) ? false : true;
            TRACE_B("Switch state is %d", switch_state);
            cJSON_AddBoolToObject(cjson_propertise, "value", switch_state);
        }
        ret = 1;
    }
    return ret;
}
