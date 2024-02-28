#include "soc/rtc.h"
#include "driver/gpio.h"
#include "driver/mcpwm.h"
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

#include "sensor_0024_other_HCSR04.h"

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

/**
 * Device descriptor
 */
typedef struct
{
    uint32_t trigger_pin; //!< GPIO output pin for trigger
    uint32_t echo_pin;    //!< GPIO input pin for echo
    uint32_t distance;    // distance in cm
} s_ultrasonic_sensor_t;

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_value_cjson(l_ezlopi_item_t *item, void *arg);
static bool ezlopi_sensor_0024_other_HCSR04_get_from_sensor(l_ezlopi_item_t *item);
static esp_err_t ultrasonic_measure(const s_ultrasonic_sensor_t *dev, uint32_t max_distance, uint32_t *distance);
static esp_err_t ultrasonic_measure_raw(const s_ultrasonic_sensor_t *dev, uint32_t max_time_us, uint32_t *time_us);

int sensor_0024_other_HCSR04_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_value_cjson(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __notify(item);
        break;
    }

    default:
    {
        break;
    }
    }

    return ret;
}

static int __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item)
    {
        cJSON *cj_param = (cJSON *)arg;
        s_ultrasonic_sensor_t *ultrasonic_sensor = (s_ultrasonic_sensor_t *)item->user_arg;
        if (cj_param && ultrasonic_sensor)
        {
            cJSON_AddNumberToObject(cj_param, ezlopi_value_str, ultrasonic_sensor->distance);
            char *valueFormatted = ezlopi_valueformatter_float(ultrasonic_sensor->distance);
            if (valueFormatted)
            {
                cJSON_AddStringToObject(cj_param, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
        }
    }
    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    static int count = 0;
    if (2 == ++count)
    {
        ezlopi_sensor_0024_other_HCSR04_get_from_sensor(item);
        ezlopi_device_value_updated_from_device_v3(item);
        count = 0;
    }
    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        s_ultrasonic_sensor_t *ultrasonic_HCSR04_sensor = (s_ultrasonic_sensor_t *)item->user_arg;
        if (ultrasonic_HCSR04_sensor)
        {
            if (GPIO_IS_VALID_OUTPUT_GPIO(item->interface.gpio.gpio_out.gpio_num))
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

                ret = (0 == gpio_config(&io_conf)) ? 1 : -1;
            }
            else if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
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

                ret = (0 == gpio_config(&io_conf)) ? 1 : -1;
            }
            if (1 != ret)
            {
                free(item->user_arg); // this will free ; memory address linked to all items
                item->user_arg = NULL;
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

static void __setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    // char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
    // ASSIGN_DEVICE_NAME_V2(device, device_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __setup_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_distance;
    item->cloud_properties.value_type = value_type_length;
    item->cloud_properties.scale = scales_centi_meter;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.gpio.gpio_out.gpio_num);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio2_str, item->interface.gpio.gpio_in.gpio_num);

    item->interface.gpio.gpio_out.enable = true;
    item->interface.gpio.gpio_out.interrupt = GPIO_INTR_DISABLE;
    item->interface.gpio.gpio_out.invert = EZLOPI_GPIO_LOGIC_NONINVERTED;
    item->interface.gpio.gpio_out.mode = GPIO_MODE_OUTPUT;
    item->interface.gpio.gpio_out.pull = GPIO_PULLDOWN_ONLY;
    item->interface.gpio.gpio_out.value = 0;

    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
    item->interface.gpio.gpio_in.invert = EZLOPI_GPIO_LOGIC_NONINVERTED;
    item->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
    item->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
    item->interface.gpio.gpio_in.value = 0;
}

static int __prepare(void *arg)
{
    int ret = 0;
    if (arg)
    {
        s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
        cJSON *cj_device = prep_arg->cjson_device;
        if (cj_device)
        {
            l_ezlopi_device_t *device = ezlopi_device_add_device(prep_arg->cjson_device);
            if (device)
            {
                __setup_device_cloud_properties(device, cj_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, sensor_0024_other_HCSR04_v3);
                if (item)
                {
                    item->cloud_properties.device_id = device->cloud_properties.device_id;
                    __setup_item_properties(item, cj_device);
                    s_ultrasonic_sensor_t *ultrasonic_sensor = (s_ultrasonic_sensor_t *)malloc(sizeof(s_ultrasonic_sensor_t));
                    if (ultrasonic_sensor)
                    {
                        memset(ultrasonic_sensor, 0, sizeof(s_ultrasonic_sensor_t));
                        ultrasonic_sensor->distance = 0;
                        ultrasonic_sensor->trigger_pin = item->interface.gpio.gpio_out.gpio_num;
                        ultrasonic_sensor->echo_pin = item->interface.gpio.gpio_in.gpio_num;
                        item->user_arg = (void *)ultrasonic_sensor;
                        ret = 1;
                    }
                }
                else
                {
                    ret = -1;
                }
            }
        }
    }
    return ret;
}

static bool ezlopi_sensor_0024_other_HCSR04_get_from_sensor(l_ezlopi_item_t *item)
{
    s_ultrasonic_sensor_t *ultrasonic_HCSR04_sensor = (s_ultrasonic_sensor_t *)item->user_arg;
    if (ultrasonic_HCSR04_sensor)
    {
        esp_err_t res = ultrasonic_measure(ultrasonic_HCSR04_sensor, MAX_DISTANCE_CM, &ultrasonic_HCSR04_sensor->distance);

        TRACE_I("Error 0x%X: ", res);
        if (res != ESP_OK)
        {
            switch (res)
            {
            case ESP_ERR_ULTRASONIC_PING:
                TRACE_I("Cannot ping (device is in invalid state)\n");
                break;
            case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                TRACE_I("Ping timeout (no device found)\n");
                break;
            case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                TRACE_I("Echo timeout (i.e. distance too big)\n");
                break;
            default:
                TRACE_I("%s\n", esp_err_to_name(res));
            }
            ultrasonic_HCSR04_sensor->distance = 0;
        }
        else
        {
            TRACE_I("Distance: %d cm\n", ultrasonic_HCSR04_sensor->distance);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }

    return true;
}

static esp_err_t ultrasonic_measure(const s_ultrasonic_sensor_t *dev, uint32_t max_distance, uint32_t *distance)
{
    CHECK_ARG(dev && distance);

    uint32_t time_us;
    CHECK(ultrasonic_measure_raw(dev, max_distance * ROUNDTRIP_CM, &time_us));
    *distance = time_us / ROUNDTRIP_CM;
    TRACE_I("time_us 2: %f cm\n", time_us / 58.0);

    return ESP_OK;
}

static esp_err_t ultrasonic_measure_raw(const s_ultrasonic_sensor_t *dev, uint32_t max_time_us, uint32_t *time_us)
{
    CHECK_ARG(dev && time_us);

    PORT_ENTER_CRITICAL;

    // Ping: Low for 2..4 us, then high 10 us
    CHECK(gpio_set_level(dev->trigger_pin, 0));
    ets_delay_us(TRIGGER_LOW_DELAY);
    CHECK(gpio_set_level(dev->trigger_pin, 1));
    ets_delay_us(TRIGGER_HIGH_DELAY);
    CHECK(gpio_set_level(dev->trigger_pin, 0));

    // Previous ping isn't ended
    if (gpio_get_level(dev->echo_pin))
        RETURN_CRITICAL(ESP_ERR_ULTRASONIC_PING);

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
