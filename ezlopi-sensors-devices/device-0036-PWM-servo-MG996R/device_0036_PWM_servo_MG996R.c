#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_pwm.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "device_0036_PWM_servo_MG996R.h"

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __set_cjson_value(l_ezlopi_item_t *item, void *arg);

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);

int device_0036_PWM_servo_MG996R(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_cjson_value(item, arg);
        break;
    }
    default:
        break;
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_dimmable_light;
    device->cloud_properties.subcategory = subcategory_dimmable_bulb;
    device->cloud_properties.device_type = dev_type_dimmer_outlet;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.item_name = ezlopi_item_name_dimmer;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_gpio_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_duty_cycle_str, item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_freq_hz_str, item->interface.pwm.freq_hz);
#if CONFIG_IDF_TARGET_ESP32C3
    item->interface.pwm.pwm_resln = 9;
#else
    item->interface.pwm.pwm_resln = 8; // 2^8 = 255
#endif
}

static int __prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *dev_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (dev_prep_arg->cjson_device)
    {
        cJSON *cj_device = dev_prep_arg->cjson_device;

        l_ezlopi_device_t *servo_device = ezlopi_device_add_device(dev_prep_arg->cjson_device, NULL);
        if (servo_device)
        {
            ret = 1;
            __prepare_device_cloud_properties(servo_device, cj_device);
            l_ezlopi_item_t *servo_item = ezlopi_device_add_item_to_device(servo_device, device_0036_PWM_servo_MG996R);
            if (servo_item)
            {
                __prepare_item_cloud_properties(servo_item, cj_device);
            }
            else
            {
                ezlopi_device_free_device(servo_device);
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

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.pwm.gpio_num))
        {
            static s_ezlopi_channel_speed_t *servo_item = NULL;
            servo_item = ezlopi_pwm_init(item->interface.pwm.gpio_num, item->interface.pwm.pwm_resln,
                                         item->interface.pwm.freq_hz, item->interface.pwm.duty_cycle);
            if (servo_item)
            {
                item->interface.pwm.channel = servo_item->channel;
                item->interface.pwm.speed_mode = servo_item->speed_mode;
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

static int __set_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result && item)
        {
            int value = 0;
            CJSON_GET_VALUE_DOUBLE(cj_result, ezlopi_value_str, value);

            TRACE_I("gpio_num: %d", item->interface.pwm.gpio_num);
            TRACE_I("item_id: %d", item->cloud_properties.item_id);
            TRACE_I("cur value: %d", value);

            if (GPIO_IS_VALID_OUTPUT_GPIO(item->interface.pwm.gpio_num))
            {
                int target_value = (int)(((value * 17) / 100) + 13);
                TRACE_I("target value: %d", target_value);
                ezlopi_pwm_change_duty(item->interface.pwm.channel, item->interface.pwm.speed_mode, target_value);
            }
        }
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            uint32_t duty = ezlopi_pwm_get_duty(item->interface.pwm.channel, item->interface.pwm.speed_mode);
            TRACE_I("raw duty value: %d", duty);

            if (duty < 13)
            {
                duty = 13;
                TRACE_W("new _ raw duty value: %d", duty);
            }
            int target_duty = (int)(((duty - 13) * 100) / 17);
            TRACE_I("target duty value: %d", target_duty);

            if (target_duty > 100)
            {
                target_duty = 100;
                TRACE_W("new _ target duty value: %d", duty);
            }

            cJSON_AddNumberToObject(cj_result, ezlopi_value_str, target_duty);
            char *formatted_val = ezlopi_valueformatter_int(target_duty);
            if (formatted_val)
            {
                cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, formatted_val);
                free(formatted_val);
            }
            ret = 1;
        }
    }

    return ret;
}

#if 0 // v2.x
#include "cJSON.h"

#include "ezlopi_util_trace.h"
#include "ezlopi_cloud_items.h"

#include "ezlopi_core_cloud.h"
#include "ezlopi_core_timer.h"
#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_core_device_value_updated.h"

#include "device_0036_PWM_servo_MG996R.h"

static int ezlopi_servo_motor_MG_996R_prepare_and_add(void* args);
static s_ezlopi_device_properties_t* ezlopi_servo_motor_MG_996R_prepare(cJSON* cjson_device);
static int ezlopi_servo_motor_MG_996R_init(s_ezlopi_device_properties_t* properties);
static int ezlopi_servo_motor_MG_996R_get_value_cjson(s_ezlopi_device_properties_t* properties, void* args);
static int ezlopi_servo_motor_MG_996R_set_value(s_ezlopi_device_properties_t* properties, void* arg);

int ezlopi_servo_motor_MG_996R(e_ezlopi_actions_t action, s_ezlopi_device_properties_t* ezlo_device, void* arg, void* user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = ezlopi_servo_motor_MG_996R_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = ezlopi_servo_motor_MG_996R_init(ezlo_device);
        break;
    }
    case EZLOPI_ACTION_SET_VALUE:
    {
        ret = ezlopi_servo_motor_MG_996R_set_value(ezlo_device, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = ezlopi_servo_motor_MG_996R_get_value_cjson(ezlo_device, arg);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int ezlopi_servo_motor_MG_996R_prepare_and_add(void* args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* device_prep_arg = (s_ezlopi_prep_arg_t*)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t* ezlopi_servo_motor_MG_996R_properties = ezlopi_servo_motor_MG_996R_prepare(device_prep_arg->cjson_device);
        if (ezlopi_servo_motor_MG_996R_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, ezlopi_servo_motor_MG_996R_properties, NULL))
            {
                free(ezlopi_servo_motor_MG_996R_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static s_ezlopi_device_properties_t* ezlopi_servo_motor_MG_996R_prepare(cJSON* cjson_device)
{
    s_ezlopi_device_properties_t* ezlopi_servo_motor_MG_996R_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (ezlopi_servo_motor_MG_996R_properties)
    {
        memset(ezlopi_servo_motor_MG_996R_properties, 0, sizeof(s_ezlopi_device_properties_t));
        ezlopi_servo_motor_MG_996R_properties->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;

        // char *device_name = NULL;
        // CJSON_GET_VALUE_STRING(cjson_device, ezlopi_dev_name_str, device_name);
        // ASSIGN_DEVICE_NAME(ezlopi_servo_motor_MG_996R_properties, device_name);
        // ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();

        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.category = category_dimmable_light;
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.subcategory = subcategory_dimmable_bulb;
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.item_name = ezlopi_item_name_dimmer;
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.device_type = dev_type_dimmer_outlet;
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.value_type = value_type_int;
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.has_getter = true;
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.has_setter = true;
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.reachable = true;
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.battery_powered = false;
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.show = true;
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.room_name[0] = '\0';
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        // CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_gpio_str, ezlopi_servo_motor_MG_996R_properties->interface.pwm.gpio_num);
        // CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_duty_cycle_str, ezlopi_servo_motor_MG_996R_properties->interface.pwm.duty_cycle);
        // CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_freq_hz_str, ezlopi_servo_motor_MG_996R_properties->interface.pwm.freq_hz);
        // CJSON_GET_VALUE_DOUBLE(cjson_device, "pwm_resln", ezlopi_servo_motor_MG_996R_properties->interface.pwm.pwm_resln);

        ezlopi_servo_motor_MG_996R_properties->interface.pwm.gpio_num = 4;
        ezlopi_servo_motor_MG_996R_properties->interface.pwm.duty_cycle = 13;
        ezlopi_servo_motor_MG_996R_properties->interface.pwm.freq_hz = 50;
#if CONFIG_IDF_TARGET_ESP32C3
        ezlopi_servo_motor_MG_996R_properties->interface.pwm.pwm_resln = 9;
#else
        ezlopi_servo_motor_MG_996R_properties->interface.pwm.pwm_resln = 8;
#endif
    }
    return ezlopi_servo_motor_MG_996R_properties;
}

static int ezlopi_servo_motor_MG_996R_init(s_ezlopi_device_properties_t* properties)
{
    int ret = -1;
    static s_ezlopi_channel_speed_t* ezlopi_servo_motor_MG_996R_speed = NULL;
    if (GPIO_IS_VALID_GPIO(properties->interface.pwm.gpio_num))
    {
        ezlopi_servo_motor_MG_996R_speed = ezlopi_pwm_init(properties->interface.pwm.gpio_num, properties->interface.pwm.pwm_resln,
            properties->interface.pwm.freq_hz, properties->interface.pwm.duty_cycle);
        properties->interface.pwm.channel = ezlopi_servo_motor_MG_996R_speed->channel;
        properties->interface.pwm.speed_mode = ezlopi_servo_motor_MG_996R_speed->speed_mode;
        ret = 0;
    }
    return ret;
}

static int ezlopi_servo_motor_MG_996R_set_value(s_ezlopi_device_properties_t* properties, void* arg)
{
    int ret = 0;
    cJSON* cjson_params = (cJSON*)arg;

    if (NULL != cjson_params)
    {
        int value = 0;
        CJSON_GET_VALUE_DOUBLE(cjson_params, ezlopi_value_str, value);

        TRACE_I("item_name: %s", properties->ezlopi_cloud.item_name);
        TRACE_I("gpio_num: %d", properties->interface.pwm.gpio_num);
        TRACE_I("item_id: %d", properties->ezlopi_cloud.item_id);
        TRACE_I("cur value: %d", value);

        if (GPIO_IS_VALID_OUTPUT_GPIO(properties->interface.pwm.gpio_num))
        {
            int target_value = (int)(((value * 17) / 100) + 13);
            ezlopi_pwm_change_duty(properties->interface.pwm.channel, properties->interface.pwm.speed_mode, target_value);
        }
    }
    return ret;
}

static int ezlopi_servo_motor_MG_996R_get_value_cjson(s_ezlopi_device_properties_t* properties, void* args)
{
    int ret = 0;
    cJSON* cjson_propertise = (cJSON*)args;
    if (cjson_propertise)
    {
        uint32_t duty = ezlopi_pwm_get_duty(properties->interface.pwm.channel, properties->interface.pwm.speed_mode);
        int target_duty = (int)(((duty - 13) * 100) / 17);
        cJSON_AddNumberToObject(cjson_propertise, ezlopi_value_str, target_duty);
        ret = 1;
    }
    return ret;
}
#endif
