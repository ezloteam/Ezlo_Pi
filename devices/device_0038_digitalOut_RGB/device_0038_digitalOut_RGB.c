#include "cJSON.h"
#include "trace.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_pwm.h"
#include "device_0038_digitalOut_RGB.h"

#define ADD_PROPERTIES_DEVICE_LIST(_properties, device_id, category, sub_category, item_name, value_type, cjson_device, RGB_struct)                      \
    {                                                                                                                                                    \
        _properties = device_0038_digitalOut_RGB_prepare_properties(device_id, category, sub_category, item_name, value_type, cjson_device, RGB_struct); \
        if (NULL != _properties)                                                                                                                         \
        {                                                                                                                                                \
            add_device_to_list(device_prep_arg, _properties, NULL);                                                                                      \
        }                                                                                                                                                \
    }

static bool RGB_LED_initialized = false;

static s_ezlopi_device_properties_t *RGB_LED_properties = NULL;
static s_ezlopi_device_properties_t *RGB_LED_onoff_switch_properties = NULL;
static s_ezlopi_device_properties_t *RGB_LED_dimmer_properties = NULL;

static int device_0038_digitalOut_RGB_prepare(void *args);
static s_ezlopi_device_properties_t *device_0038_digitalOut_RGB_prepare_properties(uint32_t device_id, const char *category,
                                                                                   const char *sub_category, const char *item_name,
                                                                                   const char *value_type, cJSON *cjson_device, device_0038_digitalOut_RGB_struct_t *RGB_struct);
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_args);
static int device_0038_digitalOut_RGB_initialize(s_ezlopi_device_properties_t *properties);
static int device_0038_digitalOut_RGB_set_value(s_ezlopi_device_properties_t *properties, void *arg);
static int device_0038_digitalOut_RGB_get_value(s_ezlopi_device_properties_t *properties, void *arg);

static int device_0038_digitalOut_RGB_change_color_value(device_0038_digitalOut_RGB_struct_t *RGB_struct);

int device_0038_digitalOut_RGB(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        device_0038_digitalOut_RGB_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        device_0038_digitalOut_RGB_initialize(properties);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        device_0038_digitalOut_RGB_get_value(properties, arg);
        break;
    }
    case EZLOPI_ACTION_SET_VALUE:
    {
        device_0038_digitalOut_RGB_set_value(properties, arg);
        break;
    }
    default:
        break;
    }

    return ret;
}

static int device_0038_digitalOut_RGB_prepare(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        device_0038_digitalOut_RGB_struct_t *RGB_struct = (device_0038_digitalOut_RGB_struct_t *)malloc(sizeof(device_0038_digitalOut_RGB_struct_t));

        if (RGB_struct)
        {
            memset(RGB_struct, 0, sizeof(device_0038_digitalOut_RGB_struct_t));
            uint32_t device_id = ezlopi_cloud_generate_device_id();

            ADD_PROPERTIES_DEVICE_LIST(RGB_LED_properties, device_id, category_dimmable_light,
                                       subcategory_dimmable_colored, ezlopi_item_name_rgbcolor,
                                       value_type_rgb, device_prep_arg->cjson_device, RGB_struct);

            ADD_PROPERTIES_DEVICE_LIST(RGB_LED_dimmer_properties, device_id, category_dimmable_light,
                                       subcategory_dimmable_colored, ezlopi_item_name_dimmer,
                                       value_type_int, device_prep_arg->cjson_device, RGB_struct);

            ADD_PROPERTIES_DEVICE_LIST(RGB_LED_onoff_switch_properties, device_id, category_dimmable_light,
                                       subcategory_dimmable_colored, ezlopi_item_name_switch,
                                       value_type_bool, device_prep_arg->cjson_device, RGB_struct);
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *device_0038_digitalOut_RGB_prepare_properties(uint32_t device_id, const char *category,
                                                                                   const char *sub_category, const char *item_name,
                                                                                   const char *value_type, cJSON *cjson_device, device_0038_digitalOut_RGB_struct_t *RGB_struct)
{
    s_ezlopi_device_properties_t *device_0038_digitalOut_RGB_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (device_0038_digitalOut_RGB_properties)
    {
        memset(device_0038_digitalOut_RGB_properties, 0, sizeof(s_ezlopi_device_properties_t));

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(device_0038_digitalOut_RGB_properties, device_name);
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.category = category;
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.subcategory = sub_category;
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.item_name = item_name;
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.device_type = dev_type_dimmer_outlet;
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.value_type = value_type;
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.has_getter = false;
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.has_setter = true;
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.reachable = true;
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.battery_powered = false;
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.show = true;
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.room_name[0] = '\0';
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.device_id = device_id;
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        device_0038_digitalOut_RGB_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        if (ezlopi_item_name_rgbcolor == item_name)
        {
            device_0038_digitalOut_RGB_properties->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
            device_0038_digitalOut_RGB_properties->interface.pwm.gpio_num = 0;
            device_0038_digitalOut_RGB_properties->interface.pwm.channel = 0;
            device_0038_digitalOut_RGB_properties->interface.pwm.value = 0;
            device_0038_digitalOut_RGB_properties->interface.pwm.duty_cycle = 0;
            device_0038_digitalOut_RGB_properties->interface.pwm.freq_hz = 5000;
            device_0038_digitalOut_RGB_properties->interface.pwm.pwm_resln = 8;

            CJSON_GET_VALUE_INT(cjson_device, "gpio1", RGB_struct->red_struct.gpio_num);
            CJSON_GET_VALUE_INT(cjson_device, "gpio2", RGB_struct->green_struct.gpio_num);
            CJSON_GET_VALUE_INT(cjson_device, "gpio3", RGB_struct->blue_struct.gpio_num);

            RGB_struct->red_struct.duty_cycle = 0;
            RGB_struct->green_struct.duty_cycle = 0;
            RGB_struct->blue_struct.duty_cycle = 0;

            RGB_struct->red_struct.freq_hz = 5000;
            RGB_struct->green_struct.freq_hz = 5000;
            RGB_struct->blue_struct.freq_hz = 5000;

            RGB_struct->red_struct.pwm_resln = 8;
            RGB_struct->green_struct.pwm_resln = 8;
            RGB_struct->blue_struct.pwm_resln = 8;

            RGB_struct->red_struct.value = 255;
            RGB_struct->green_struct.value = 255;
            RGB_struct->blue_struct.value = 255;
            RGB_struct->brightness = 0.0;
        }
        if (ezlopi_item_name_switch == item_name)
        {
            device_0038_digitalOut_RGB_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;
            device_0038_digitalOut_RGB_properties->interface.gpio.gpio_in.enable = true;
            device_0038_digitalOut_RGB_properties->interface.gpio.gpio_in.gpio_num = 0;
            device_0038_digitalOut_RGB_properties->interface.gpio.gpio_in.interrupt = false;
            device_0038_digitalOut_RGB_properties->interface.gpio.gpio_in.invert = false;
            device_0038_digitalOut_RGB_properties->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
            device_0038_digitalOut_RGB_properties->interface.gpio.gpio_in.pull = false;
            device_0038_digitalOut_RGB_properties->interface.gpio.gpio_in.value = false;
            device_0038_digitalOut_RGB_properties->interface.gpio.gpio_out.enable = false;
        }
        if(ezlopi_item_name_dimmer == item_name)
        {
            device_0038_digitalOut_RGB_properties->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
            device_0038_digitalOut_RGB_properties->interface.pwm.gpio_num = 0;
            device_0038_digitalOut_RGB_properties->interface.pwm.channel = 0;
            device_0038_digitalOut_RGB_properties->interface.pwm.value = 0;
            device_0038_digitalOut_RGB_properties->interface.pwm.duty_cycle = 0;
            device_0038_digitalOut_RGB_properties->interface.pwm.freq_hz = 5000;
            device_0038_digitalOut_RGB_properties->interface.pwm.pwm_resln = 8;
        }

        device_0038_digitalOut_RGB_properties->user_arg = RGB_struct;
    }

    return device_0038_digitalOut_RGB_properties;
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

static int device_0038_digitalOut_RGB_initialize(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;

    device_0038_digitalOut_RGB_struct_t *RGB_struct = (device_0038_digitalOut_RGB_struct_t *)properties->user_arg;

    if ((NULL != RGB_struct) && (!RGB_LED_initialized))
    {
        s_ezlopi_channel_speed_t *RGB_LED_red_channel_speed = ezlopi_pwm_init(RGB_struct->red_struct.gpio_num, RGB_struct->red_struct.pwm_resln, RGB_struct->red_struct.freq_hz, RGB_struct->red_struct.duty_cycle);
        s_ezlopi_channel_speed_t *RGB_LED_green_channel_speed = ezlopi_pwm_init(RGB_struct->green_struct.gpio_num, RGB_struct->green_struct.pwm_resln, RGB_struct->green_struct.freq_hz, RGB_struct->green_struct.duty_cycle);
        s_ezlopi_channel_speed_t *RGB_LED_blue_channel_speed = ezlopi_pwm_init(RGB_struct->blue_struct.gpio_num, RGB_struct->blue_struct.pwm_resln, RGB_struct->blue_struct.freq_hz, RGB_struct->blue_struct.duty_cycle);

        RGB_struct->red_struct.channel = RGB_LED_red_channel_speed->channel;
        RGB_struct->red_struct.speed_mode = RGB_LED_red_channel_speed->speed_mode;
        TRACE_B("red channel is %d", RGB_struct->red_struct.channel);

        RGB_struct->green_struct.channel = RGB_LED_green_channel_speed->channel;
        RGB_struct->green_struct.speed_mode = RGB_LED_green_channel_speed->speed_mode;
        TRACE_B("green channel is %d", RGB_struct->green_struct.channel);

        RGB_struct->blue_struct.channel = RGB_LED_blue_channel_speed->channel;
        RGB_struct->blue_struct.speed_mode = RGB_LED_blue_channel_speed->speed_mode;
        TRACE_B("blue channel is %d", RGB_struct->blue_struct.channel);

        free(RGB_LED_red_channel_speed);
        free(RGB_LED_green_channel_speed);
        free(RGB_LED_blue_channel_speed);
        
        device_0038_digitalOut_RGB_change_color_value(RGB_struct);

        RGB_LED_initialized = true;
    }

    return ret;
}

static int device_0038_digitalOut_RGB_set_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_params = (cJSON *)arg;
    static float previous_dim_factor = 1.0;

    device_0038_digitalOut_RGB_struct_t *RGB_struct = (device_0038_digitalOut_RGB_struct_t *)properties->user_arg;

    if ((NULL != cjson_params) && (NULL != RGB_struct))
    {
        if (ezlopi_item_name_rgbcolor == properties->ezlopi_cloud.item_name)
        {
            cJSON *cjson_params_rgb_values = cJSON_GetObjectItem(cjson_params, "value");
            TRACE_B("%s", cJSON_Print(cjson_params_rgb_values));

            CJSON_GET_VALUE_INT(cjson_params_rgb_values, "red", RGB_struct->red_struct.value);
            CJSON_GET_VALUE_INT(cjson_params_rgb_values, "green", RGB_struct->green_struct.value);
            CJSON_GET_VALUE_INT(cjson_params_rgb_values, "blue", RGB_struct->blue_struct.value);

            device_0038_digitalOut_RGB_change_color_value(RGB_struct);
        }
        if (ezlopi_item_name_switch == properties->ezlopi_cloud.item_name)
        {
            int led_state = 0;
            CJSON_GET_VALUE_INT(cjson_params, "value", led_state);
            previous_dim_factor = ((0 == led_state) ? RGB_struct->brightness : previous_dim_factor);
            RGB_struct->brightness = ((0 == led_state) ? 0.0 : ((0 == previous_dim_factor) ? 1.0 : previous_dim_factor));
            TRACE_B("Brightness value is %d, %d, %d", (uint8_t)(RGB_struct->red_struct.value * RGB_struct->brightness), (uint8_t)(RGB_struct->green_struct.value * RGB_struct->brightness),
                    (uint8_t)(RGB_struct->blue_struct.value * RGB_struct->brightness));
            device_0038_digitalOut_RGB_change_color_value(RGB_struct);
            ezlopi_device_value_updated_from_device(RGB_LED_dimmer_properties);
        }
        if(ezlopi_item_name_dimmer == properties->ezlopi_cloud.item_name)
        {
            int dim_percent = 0;
            CJSON_GET_VALUE_INT(cjson_params, "value", dim_percent);
            float dim_brightness_factor = dim_percent / 100.0;
            TRACE_B("dim_percent %d, dim_brightness_factor is %f", dim_percent, dim_brightness_factor);
            RGB_struct->brightness = dim_brightness_factor;
            device_0038_digitalOut_RGB_change_color_value(RGB_struct);
            ezlopi_device_value_updated_from_device(RGB_LED_onoff_switch_properties);
        }
    }
    return ret;
}

static int device_0038_digitalOut_RGB_get_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;

    cJSON *cjson_params = (cJSON *)arg;
    device_0038_digitalOut_RGB_struct_t *RGB_struct = (device_0038_digitalOut_RGB_struct_t *)properties->user_arg;

    if ((NULL != cjson_params) && (NULL != RGB_struct))
    {
        if (ezlopi_item_name_rgbcolor == properties->ezlopi_cloud.item_name)
        {
            cJSON *color_values = cJSON_AddObjectToObject(cjson_params, "value");
            if (color_values)
            {
                cJSON_AddNumberToObject(color_values, "red", RGB_struct->red_struct.value);
                cJSON_AddNumberToObject(color_values, "green", RGB_struct->green_struct.value);
                cJSON_AddNumberToObject(color_values, "blue", RGB_struct->blue_struct.value);
            }
        }
        if (ezlopi_item_name_switch == properties->ezlopi_cloud.item_name)
        {
            int state = ((0 == RGB_struct->brightness) ? 0 : 1);
            cJSON_AddBoolToObject(cjson_params, "value", state);
        }
        if(ezlopi_item_name_dimmer == properties->ezlopi_cloud.item_name)
        {
            int dim_percentage = (int)(RGB_struct->brightness * 100);
            cJSON_AddNumberToObject(cjson_params, "value", dim_percentage);
        }
    }
    return ret;
}

static int device_0038_digitalOut_RGB_change_color_value(device_0038_digitalOut_RGB_struct_t *RGB_struct)
{
    int ret = 0;

    TRACE_B("Brightness value is %d, %d, %d", (uint8_t)(RGB_struct->red_struct.value * RGB_struct->brightness), (uint8_t)(RGB_struct->green_struct.value * RGB_struct->brightness),
                (uint8_t)(RGB_struct->blue_struct.value * RGB_struct->brightness));

    ezlopi_pwm_change_duty(RGB_struct->red_struct.channel, RGB_struct->red_struct.speed_mode, (uint8_t)(RGB_struct->red_struct.value * RGB_struct->brightness));
    ezlopi_pwm_change_duty(RGB_struct->green_struct.channel, RGB_struct->green_struct.speed_mode, (uint8_t)(RGB_struct->green_struct.value * RGB_struct->brightness));
    ezlopi_pwm_change_duty(RGB_struct->blue_struct.channel, RGB_struct->blue_struct.speed_mode, (uint8_t)(RGB_struct->blue_struct.value * RGB_struct->brightness));

    return ret;
}
