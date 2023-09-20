
#include "cJSON.h"
#include "trace.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "math.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

#include "device_0036_PWM_servo_MG996R.h"

static int ezlopi_servo_motor_MG_996R_prepare_and_add(void *args);
static s_ezlopi_device_properties_t *ezlopi_servo_motor_MG_996R_prepare(cJSON *cjson_device);
static int ezlopi_servo_motor_MG_996R_init(s_ezlopi_device_properties_t *properties);
static int ezlopi_servo_motor_MG_996R_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);
static int ezlopi_servo_motor_MG_996R_set_value(s_ezlopi_device_properties_t *properties, void *arg);

int device_0036_PWM_servo_MG996R(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg)
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

static int ezlopi_servo_motor_MG_996R_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *ezlopi_servo_motor_MG_996R_properties = ezlopi_servo_motor_MG_996R_prepare(device_prep_arg->cjson_device);
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

static s_ezlopi_device_properties_t *ezlopi_servo_motor_MG_996R_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *ezlopi_servo_motor_MG_996R_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (ezlopi_servo_motor_MG_996R_properties)
    {
        memset(ezlopi_servo_motor_MG_996R_properties, 0, sizeof(s_ezlopi_device_properties_t));
        ezlopi_servo_motor_MG_996R_properties->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(ezlopi_servo_motor_MG_996R_properties, device_name);
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
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        ezlopi_servo_motor_MG_996R_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", ezlopi_servo_motor_MG_996R_properties->interface.pwm.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "duty_cycle", ezlopi_servo_motor_MG_996R_properties->interface.pwm.duty_cycle);
        CJSON_GET_VALUE_INT(cjson_device, "freq_hz", ezlopi_servo_motor_MG_996R_properties->interface.pwm.freq_hz);
        CJSON_GET_VALUE_INT(cjson_device, "pwm_resln", ezlopi_servo_motor_MG_996R_properties->interface.pwm.pwm_resln);
        
        TRACE_B("HERE!!");
        TRACE_B("gpio = %d, duty_cycle = %d, freq_hz = %d, pwm_resln = %d", ezlopi_servo_motor_MG_996R_properties->interface.pwm.gpio_num, 
                                                                            ezlopi_servo_motor_MG_996R_properties->interface.pwm.duty_cycle,
                                                                            ezlopi_servo_motor_MG_996R_properties->interface.pwm.freq_hz,
                                                                            ezlopi_servo_motor_MG_996R_properties->interface.pwm.pwm_resln);


        // ezlopi_servo_motor_MG_996R_properties->interface.pwm.duty_cycle = 13;
        // ezlopi_servo_motor_MG_996R_properties->interface.pwm.freq_hz = 50;
// #if CONFIG_IDF_TARGET_ESP32C3
//         ezlopi_servo_motor_MG_996R_properties->interface.pwm.pwm_resln = 9;
// #else
//         ezlopi_servo_motor_MG_996R_properties->interface.pwm.pwm_resln = 8;
// #endif
    }
    return ezlopi_servo_motor_MG_996R_properties;
}

static int ezlopi_servo_motor_MG_996R_init(s_ezlopi_device_properties_t *properties)
{
    int ret = -1;
    // static s_ezlopi_channel_speed_t *ezlopi_servo_motor_MG_996R_speed = NULL;
    // if (GPIO_IS_VALID_GPIO(properties->interface.pwm.gpio_num))
    // {
    //     ezlopi_servo_motor_MG_996R_speed = ezlopi_pwm_init(properties->interface.pwm.gpio_num, properties->interface.pwm.pwm_resln,
    //                                                        properties->interface.pwm.freq_hz, properties->interface.pwm.duty_cycle);
    //     properties->interface.pwm.channel = ezlopi_servo_motor_MG_996R_speed->channel;
    //     properties->interface.pwm.speed_mode = ezlopi_servo_motor_MG_996R_speed->speed_mode;
    //     ret = 0;
    // }
    
    // Configure MCPWM
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, properties->interface.pwm.gpio_num));
    mcpwm_config_t pwm_config = {
        .frequency = MG996R_SERVO_FREQUENCY,
        .cmpr_a = 0,
        .cmpr_b = 0,
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0
    };
    ESP_ERROR_CHECK(mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config));
    
    ESP_ERROR_CHECK(mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0));

    return ret;
}

static int ezlopi_servo_motor_MG_996R_set_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_params = (cJSON *)arg;

    if (NULL != cjson_params)
    {
        int value = 0;
        CJSON_GET_VALUE_INT(cjson_params, "value", properties->interface.pwm.duty_cycle);
    
        TRACE_I("cur value: %d", properties->interface.pwm.duty_cycle);

        if (GPIO_IS_VALID_OUTPUT_GPIO(properties->interface.pwm.gpio_num))
        {
            // properties->interface.pwm.duty_cycle = value;
            uint32_t target_duty = (uint32_t)ceil(((properties->interface.pwm.duty_cycle * 17) + 500));
            TRACE_B("duty cycle is %d", target_duty);
            // ezlopi_pwm_change_duty(properties->interface.pwm.channel, properties->interface.pwm.speed_mode, target_value);
            ESP_ERROR_CHECK(mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, target_duty));
        }
    }
    return ret;
}

static int ezlopi_servo_motor_MG_996R_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)args;
    if (cjson_propertise)
    {
        // uint32_t duty = ezlopi_pwm_get_duty(properties->interface.pwm.channel, properties->interface.pwm.speed_mode);
        // int target_duty = (int)(((duty - 13) * 100) / 17);
        TRACE_B("targer duty is %d", properties->interface.pwm.duty_cycle);
        cJSON_AddNumberToObject(cjson_propertise, "value", properties->interface.pwm.duty_cycle);
        ret = 1;
    }
    return ret;
}