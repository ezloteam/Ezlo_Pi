

#include "sensor_0031_other_JSNSR04T.h"
#include "cJSON.h"
#include "trace.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "stdlib.h"

static int ezlopi_sensor_0031_other_JSNSR04T_prepare_and_add(void *args);
static s_ezlopi_device_properties_t *ezlopi_sensor_0031_other_JSNSR04T_prepare(cJSON *cjson_device);
static int ezlopi_sensor_0031_other_JSNSR04T_init(s_ezlopi_device_properties_t *properties);
static int ezlopi_sensor_0031_other_JSNSR04T_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);
static bool ezlopi_sensor_0031_other_JSNSR04T_get_from_sensor(s_ezlopi_device_properties_t *properties);

static esp_err_t ultrasonic_init(const ultrasonic_JSNSR04T_sensor_t *dev);
static esp_err_t ultrasonic_measure_raw(const ultrasonic_JSNSR04T_sensor_t *dev, uint32_t max_time_us, uint32_t *time_us);
static esp_err_t ultrasonic_measure(const ultrasonic_JSNSR04T_sensor_t *dev, uint32_t max_distance, uint32_t *distance);

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

int sensor_0031_other_JSNSR04T(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = ezlopi_sensor_0031_other_JSNSR04T_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = ezlopi_sensor_0031_other_JSNSR04T_init(properties);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ezlopi_sensor_0031_other_JSNSR04T_get_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        static int count = 2;
        if (2 == count)
        {
            ret = ezlopi_sensor_0031_other_JSNSR04T_get_from_sensor(properties);
            ezlopi_device_value_updated_from_device(properties);
            count = 0;
        }
        count++;
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int ezlopi_sensor_0031_other_JSNSR04T_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *ezlopi_sensor_0031_other_JSNSR04T_properties = ezlopi_sensor_0031_other_JSNSR04T_prepare(device_prep_arg->cjson_device);
        if (ezlopi_sensor_0031_other_JSNSR04T_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, ezlopi_sensor_0031_other_JSNSR04T_properties, NULL))
            {
                free(ezlopi_sensor_0031_other_JSNSR04T_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static s_ezlopi_device_properties_t *ezlopi_sensor_0031_other_JSNSR04T_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *ezlopi_sensor_0031_other_JSNSR04T_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    ultrasonic_JSNSR04T_sensor_t *ultrasonic_JSNSR04T_sensor = (ultrasonic_JSNSR04T_sensor_t *)malloc(sizeof(ultrasonic_JSNSR04T_sensor_t));

    if (ezlopi_sensor_0031_other_JSNSR04T_properties && ultrasonic_JSNSR04T_sensor)
    {
        memset(ezlopi_sensor_0031_other_JSNSR04T_properties, 0, sizeof(s_ezlopi_device_properties_t));
        memset(ultrasonic_JSNSR04T_sensor, 0, sizeof(ultrasonic_JSNSR04T_sensor_t));
        ezlopi_sensor_0031_other_JSNSR04T_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(ezlopi_sensor_0031_other_JSNSR04T_properties, device_name);
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.category = category_generic_sensor;
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.item_name = ezlopi_item_name_distance;
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.device_type = dev_type_sensor;
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.value_type = value_type_length;
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.has_getter = true;
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.has_setter = false;
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.reachable = true;
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.battery_powered = false;
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.show = true;
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.room_name[0] = '\0';
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        ezlopi_sensor_0031_other_JSNSR04T_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio1", ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_out.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "gpio2", ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_in.gpio_num);

        ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_out.enable = true;
        ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_out.interrupt = GPIO_INTR_DISABLE;
        ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_out.invert = EZLOPI_GPIO_LOGIC_NONINVERTED;
        ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_out.mode = GPIO_MODE_OUTPUT;
        ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_out.pull = GPIO_PULLDOWN_ONLY;
        ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_out.value = 0;

        ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_in.enable = true;
        ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
        ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_in.invert = EZLOPI_GPIO_LOGIC_NONINVERTED;
        ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
        ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
        ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_in.value = 0;

        ultrasonic_JSNSR04T_sensor->echo_pin = ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_in.gpio_num;
        ultrasonic_JSNSR04T_sensor->trigger_pin = ezlopi_sensor_0031_other_JSNSR04T_properties->interface.gpio.gpio_out.gpio_num;
        ultrasonic_JSNSR04T_sensor->distance = 0;

        ezlopi_sensor_0031_other_JSNSR04T_properties->user_arg = ultrasonic_JSNSR04T_sensor;
    }
    return ezlopi_sensor_0031_other_JSNSR04T_properties;
}

static int ezlopi_sensor_0031_other_JSNSR04T_init(s_ezlopi_device_properties_t *properties)
{
    int ret = -1;

    ultrasonic_JSNSR04T_sensor_t *ultrasonic_JSNSR04T_sensor = (ultrasonic_JSNSR04T_sensor_t *)properties->user_arg;
    if (ultrasonic_JSNSR04T_sensor)
    {
        ultrasonic_init(ultrasonic_JSNSR04T_sensor);
    }

    return ret;
}

static bool ezlopi_sensor_0031_other_JSNSR04T_get_from_sensor(s_ezlopi_device_properties_t *properties)
{
    uint32_t distance;

    ultrasonic_JSNSR04T_sensor_t *ultrasonic_JSNSR04T_sensor = (ultrasonic_JSNSR04T_sensor_t *)properties->user_arg;
    if (ultrasonic_JSNSR04T_sensor)
    {
        esp_err_t res = ultrasonic_measure(ultrasonic_JSNSR04T_sensor, MAX_DISTANCE_CM, &ultrasonic_JSNSR04T_sensor->distance);

        if (res != ESP_OK)
        {
            TRACE_B("Error 0x%X: ", res);
            switch (res)
            {
            case ESP_ERR_ULTRASONIC_PING:
                TRACE_B("Cannot ping (device is in invalid state)\n");
                break;
            case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                TRACE_B("Ping timeout (no device found)\n");
                break;
            case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                TRACE_B("Echo timeout (i.e. distance too big)\n");
                break;
            default:
                TRACE_B("%s\n", esp_err_to_name(res));
            }
        }
        else
        {
            // TRACE_B("Distance: %d cm\n", ultrasonic_JSNSR04T_sensor->distance);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
    return true;
}

static int ezlopi_sensor_0031_other_JSNSR04T_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    bool current_val = true;
    cJSON *cjson_propertise = (cJSON *)args;

    ultrasonic_JSNSR04T_sensor_t *ultrasonic_JSNSR04T_sensor = (ultrasonic_JSNSR04T_sensor_t *)properties->user_arg;

    if (cjson_propertise && ultrasonic_JSNSR04T_sensor)
    {
        cJSON_AddNumberToObject(cjson_propertise, "value", (ultrasonic_JSNSR04T_sensor->distance / 100.0));
        cJSON_AddStringToObject(cjson_propertise, "scale", "meter");
        ret = 1;
    }
    return ret;
}

static esp_err_t ultrasonic_init(const ultrasonic_JSNSR04T_sensor_t *dev)
{
    CHECK_ARG(dev);

    gpio_config_t trigger_config = {
        .pin_bit_mask = (1ULL << dev->trigger_pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config_t echo_config = {
        .pin_bit_mask = (1ULL << dev->echo_pin),
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&trigger_config));
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&echo_config));

    return gpio_set_level(dev->trigger_pin, 0);
}

static esp_err_t ultrasonic_measure_raw(const ultrasonic_JSNSR04T_sensor_t *dev, uint32_t max_time_us, uint32_t *time_us)
{
    CHECK_ARG(dev && time_us);

    CHECK(gpio_set_level(dev->trigger_pin, 0));
    vTaskDelay(1 / portTICK_PERIOD_MS);
    CHECK(gpio_set_level(dev->trigger_pin, 1));
    vTaskDelay(1 / portTICK_PERIOD_MS);
    CHECK(gpio_set_level(dev->trigger_pin, 0));

    PORT_ENTER_CRITICAL;

    // Previous ping isn't ended
    if (gpio_get_level(dev->echo_pin))
    {
        RETURN_CRITICAL(ESP_ERR_ULTRASONIC_PING);
    }

    // Wait for echo
    int64_t start = esp_timer_get_time();
    while (!gpio_get_level(dev->echo_pin))
    {
        if (timeout_expired(start, PING_TIMEOUT))
            RETURN_CRITICAL(ESP_ERR_ULTRASONIC_PING_TIMEOUT);
    }

    // got echo, measuring
    int64_t echo_start = esp_timer_get_time();
    int64_t time = echo_start;
    while (gpio_get_level(dev->echo_pin))
    {
        time = esp_timer_get_time();
        if (timeout_expired(echo_start, max_time_us))
            RETURN_CRITICAL(ESP_ERR_ULTRASONIC_ECHO_TIMEOUT);
    }
    PORT_EXIT_CRITICAL;

    *time_us = time - echo_start;

    return ESP_OK;
}

static esp_err_t ultrasonic_measure(const ultrasonic_JSNSR04T_sensor_t *dev, uint32_t max_distance, uint32_t *distance)
{
    CHECK_ARG(dev && distance);

    uint32_t time_us;
    CHECK(ultrasonic_measure_raw(dev, max_distance * ROUNDTRIP_CM, &time_us));
    *distance = time_us / ROUNDTRIP_CM;

    return ESP_OK;
}
