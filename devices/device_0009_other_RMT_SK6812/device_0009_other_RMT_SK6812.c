
#include <string.h>
#include "sdkconfig.h"

#include "cJSON.h"
#include "trace.h"
#include "ezlopi_timer.h"
#include "items.h"
#include "math.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_devices.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_spi_master.h"
#include "device_0009_other_RMT_SK6812.h"
#include "led_strip.h"
#include "driver/gpio.h"
#include "color_codes.h"

#define ADD_PROPERTIES_DEVICE_LIST(_properties, device_id, category, sub_category, item_name, value_type, cjson_device, sk6812_strip)         \
    {                                                                                                                                         \
        _properties = ezlopi_sk6812_prepare_properties(device_id, category, sub_category, item_name, value_type, cjson_device, sk6812_strip); \
        if (NULL != _properties)                                                                                                              \
        {                                                                                                                                     \
            add_device_to_list(prep_arg, _properties, NULL);                                                                                  \
        }                                                                                                                                     \
    }

static bool sk6812_led_strip_initialized = false;

static s_ezlopi_device_properties_t *switch_properties = NULL;
static s_ezlopi_device_properties_t *dimmer_properties = NULL;
static s_ezlopi_device_properties_t *dimmer_up_properties = NULL;
static s_ezlopi_device_properties_t *dimmer_down_properties = NULL;
static s_ezlopi_device_properties_t *dimmer_stop_properties = NULL;
static s_ezlopi_device_properties_t *rgb_color_properties = NULL;

static int ezlopi_sk6812_prepare(void *arg);
static s_ezlopi_device_properties_t *ezlopi_sk6812_prepare_properties(uint32_t device_id, const char *category,
                                                                      const char *sub_category, const char *item_name,
                                                                      const char *value_type, cJSON *cjson_device, led_strip_t *sk6812_strip);
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_args);
static int ezlopi_sk6812_init(s_ezlopi_device_properties_t *properties);
static int ezlopi_sk6812_set_value_cjson(s_ezlopi_device_properties_t *properties, void *args);
static int ezlopi_sk6812_get_value(s_ezlopi_device_properties_t *properties, void *arg);

int device_0009_other_RMT_SK6812(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ezlopi_sk6812_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ezlopi_sk6812_init(properties);
        break;
    }
    case EZLOPI_ACTION_SET_VALUE:
    {
        ezlopi_sk6812_set_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ezlopi_sk6812_get_value(properties, arg);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static int ezlopi_sk6812_prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if ((NULL != prep_arg) && (NULL != prep_arg->cjson_device))
    {
        led_strip_t *sk6812_strip = (led_strip_t *)malloc(sizeof(led_strip_t));
        if (sk6812_strip)
        {
            memset(sk6812_strip, 0, sizeof(led_strip_t));
            uint32_t device_id = ezlopi_cloud_generate_device_id();

            ADD_PROPERTIES_DEVICE_LIST(rgb_color_properties, device_id, category_dimmable_light, subcategory_dimmable_colored, ezlopi_item_name_rgbcolor, value_type_rgb, prep_arg->cjson_device, sk6812_strip);
            ADD_PROPERTIES_DEVICE_LIST(dimmer_properties, device_id, category_dimmable_light, subcategory_dimmable_colored, ezlopi_item_name_dimmer, value_type_int, prep_arg->cjson_device, sk6812_strip);
            ADD_PROPERTIES_DEVICE_LIST(dimmer_up_properties, device_id, category_dimmable_light, subcategory_dimmable_colored, ezlopi_item_name_dimmer_up, value_type_int, prep_arg->cjson_device, sk6812_strip);
            ADD_PROPERTIES_DEVICE_LIST(dimmer_down_properties, device_id, category_dimmable_light, subcategory_dimmable_colored, ezlopi_item_name_dimmer_down, value_type_int, prep_arg->cjson_device, sk6812_strip);
            ADD_PROPERTIES_DEVICE_LIST(dimmer_stop_properties, device_id, category_dimmable_light, subcategory_dimmable_colored, ezlopi_item_name_dimmer_stop, value_type_int, prep_arg->cjson_device, sk6812_strip);
            ADD_PROPERTIES_DEVICE_LIST(switch_properties, device_id, category_dimmable_light, subcategory_dimmable_colored, ezlopi_item_name_switch, value_type_bool, prep_arg->cjson_device, sk6812_strip);
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *ezlopi_sk6812_prepare_properties(uint32_t device_id, const char *category,
                                                                      const char *sub_category, const char *item_name,
                                                                      const char *value_type, cJSON *cjson_device, led_strip_t *sk6812_strip)
{
    s_ezlopi_device_properties_t *sk6812_properties = NULL;
    if (NULL != cjson_device)
    {
        sk6812_properties = (s_ezlopi_device_properties_t *)malloc(sizeof(s_ezlopi_device_properties_t));

        if (sk6812_properties)
        {
            memset(sk6812_properties, 0, sizeof(s_ezlopi_device_properties_t));

            char *device_name = NULL;
            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
            ASSIGN_DEVICE_NAME(sk6812_properties, device_name);
            sk6812_properties->ezlopi_cloud.category = category;
            sk6812_properties->ezlopi_cloud.subcategory = sub_category;
            sk6812_properties->ezlopi_cloud.item_name = item_name;
            sk6812_properties->ezlopi_cloud.value_type = value_type;
            sk6812_properties->ezlopi_cloud.device_type = dev_type_light_strip;
            sk6812_properties->ezlopi_cloud.device_id = device_id;
            sk6812_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();
            sk6812_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            sk6812_properties->ezlopi_cloud.reachable = true;
            sk6812_properties->ezlopi_cloud.battery_powered = false;
            sk6812_properties->ezlopi_cloud.show = true;
            sk6812_properties->ezlopi_cloud.has_getter = true;
            sk6812_properties->ezlopi_cloud.has_setter = true;
            sk6812_properties->ezlopi_cloud.room_name[0] = '\0';
            if (ezlopi_item_name_switch == item_name)
            {
                sk6812_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;
                sk6812_properties->interface.gpio.gpio_in.enable = true;
                CJSON_GET_VALUE_INT(cjson_device, "gpio", sk6812_properties->interface.gpio.gpio_in.gpio_num);
                sk6812_properties->interface.gpio.gpio_in.interrupt = false;
                sk6812_properties->interface.gpio.gpio_in.invert = false;
                sk6812_properties->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
                sk6812_properties->interface.gpio.gpio_in.pull = false;
                sk6812_properties->interface.gpio.gpio_in.value = false;
                sk6812_properties->interface.gpio.gpio_out.enable = false;
            }
            else
            {
                sk6812_properties->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
                CJSON_GET_VALUE_INT(cjson_device, "gpio", sk6812_properties->interface.pwm.gpio_num);
                CJSON_GET_VALUE_INT(cjson_device, "duty_cycle", sk6812_properties->interface.pwm.duty_cycle);
                CJSON_GET_VALUE_INT(cjson_device, "freq_hz", sk6812_properties->interface.pwm.freq_hz);
                sk6812_properties->interface.pwm.pwm_resln = 12;
            }
            sk6812_properties->user_arg = sk6812_strip;
        }
    }
    return sk6812_properties;
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

static int ezlopi_sk6812_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;

    led_strip_t *sk6812_strip = (led_strip_t *)properties->user_arg;
    if ((!sk6812_led_strip_initialized) && (NULL != sk6812_strip))
    {

        sk6812_strip->type = LED_STRIP_SK6812;
        sk6812_strip->length = 10;
        sk6812_strip->gpio = properties->interface.pwm.gpio_num;
        sk6812_strip->buf = NULL;
        sk6812_strip->brightness = 255;
        sk6812_strip->channel = RMT_CHANNEL_0;

        led_strip_install();
        led_strip_init(sk6812_strip);
        rgb_t color = {
            .red = 0,
            .green = 255,
            .blue = 0,
        };
        led_strip_fill(sk6812_strip, 0, sk6812_strip->length, color);
        led_strip_set_brightness(sk6812_strip, 255);
        led_strip_flush(sk6812_strip);
        sk6812_led_strip_initialized = true;
    }
    return ret;
}

static int ezlopi_sk6812_set_value_cjson(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    static uint8_t previous_brightness = 0;

    led_strip_t *sk6812_strip = (led_strip_t *)properties->user_arg;
    cJSON *cjson_params = (cJSON *)arg;

    if ((NULL != cjson_params) && (NULL != sk6812_strip))
    {

        if (ezlopi_item_name_rgbcolor == properties->ezlopi_cloud.item_name)
        {
            cJSON *cjson_params_color_values = cJSON_GetObjectItem(cjson_params, "value");

            rgb_t color = {
                .red = 0,
                .green = 0,
                .blue = 0,
            };

            CJSON_GET_VALUE_INT(cjson_params_color_values, "red", color.red);
            CJSON_GET_VALUE_INT(cjson_params_color_values, "green", color.green);
            CJSON_GET_VALUE_INT(cjson_params_color_values, "blue", color.blue);

            led_strip_fill(sk6812_strip, 0, sk6812_strip->length, color);
            led_strip_flush(sk6812_strip);
            ezlopi_device_value_updated_from_device(dimmer_properties);
        }
        else if (ezlopi_item_name_dimmer == properties->ezlopi_cloud.item_name)
        {
            int dimmable_value_percentage = 0;
            CJSON_GET_VALUE_INT(cjson_params, "value", dimmable_value_percentage);
            int dimmable_value = (int)((dimmable_value_percentage * 255) / 100);
            led_strip_set_brightness(sk6812_strip, dimmable_value);
            led_strip_flush(sk6812_strip);
            ezlopi_device_value_updated_from_device(switch_properties);
        }
        else if (ezlopi_item_name_switch == properties->ezlopi_cloud.item_name)
        {
            int led_state = 0;
            CJSON_GET_VALUE_INT(cjson_params, "value", led_state);
            previous_brightness = (false == led_state) ? sk6812_strip->brightness : previous_brightness;
            int brightness_value = (false == led_state) ? 0 : ((0 == previous_brightness) ? 255 : previous_brightness);
            led_strip_set_brightness(sk6812_strip, brightness_value);
            led_strip_flush(sk6812_strip);
            ezlopi_device_value_updated_from_device(dimmer_properties);
        }
        else
        {
            TRACE_B("properties->ezlopi_cloud.item_name => %s", properties->ezlopi_cloud.item_name);
        }
    }
    return ret;
}

static int ezlopi_sk6812_get_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;

    led_strip_t *sk6812_strip = (led_strip_t *)properties->user_arg;
    cJSON *cjson_properties = (cJSON *)arg;
    if ((NULL != cjson_properties) && (NULL != sk6812_strip))
    {
        if (ezlopi_item_name_rgbcolor == properties->ezlopi_cloud.item_name)
        {
            cJSON *color_json = cJSON_AddObjectToObject(cjson_properties, "value");
            if (color_json)
            {
                int green = sk6812_strip->buf[0];
                int red = sk6812_strip->buf[1];
                int blue = sk6812_strip->buf[2];
                // TRACE_B("green => %d, blue => %d, red => %d", green, blue, red);
                cJSON_AddNumberToObject(color_json, "red", red);
                cJSON_AddNumberToObject(color_json, "green", green);
                cJSON_AddNumberToObject(color_json, "blue", blue);
                cJSON_AddNumberToObject(color_json, "cwhite", ((red << 16) | (green << 8) | (blue)));
                // cJSON_AddNumberToObject(color_json, "wwhite", 0);
                // cJSON_AddNumberToObject(color_json, "amber", 0);
                // cJSON_AddNumberToObject(color_json, "cyan", 0);
                // cJSON_AddNumberToObject(color_json, "purple", 0);
                // cJSON_AddNumberToObject(color_json, "indexed", 0);
            }
        }
        else if (ezlopi_item_name_dimmer == properties->ezlopi_cloud.item_name)
        {
            properties->interface.pwm.duty_cycle = (int)ceil(((sk6812_strip->brightness * 100.0) / 255.0));
            cJSON_AddNumberToObject(cjson_properties, "value", properties->interface.pwm.duty_cycle);
        }
        else if (ezlopi_item_name_switch == properties->ezlopi_cloud.item_name)
        {
            properties->interface.gpio.gpio_in.value = (0 == sk6812_strip->brightness) ? 0 : 1;
            cJSON_AddBoolToObject(cjson_properties, "value", properties->interface.gpio.gpio_in.value);
        }
    }
    return ret;
}