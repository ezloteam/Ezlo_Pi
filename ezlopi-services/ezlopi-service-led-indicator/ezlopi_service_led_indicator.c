

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_ENABLE_LED_INDICATOR

#include "ezlopi_service_loop.h"
#include "ezlopi_service_led_indicator.h"

#include "driver/rmt.h"
#include "led_strip.h"
#include "esp_err.h"
#include "color_codes.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_event_group.h"
#include "ezlopi_core_ping.h"
#include "ezlopi_core_processes.h"

static void __indicator_LED_loop(void *arg);

static e_indicator_led_priority_t __indicator_priority = PRIORITY_CLOUD;

#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)

#define COLOR_GET_RED(x) ( 0xFF & ( x >> 16 ))
#define COLOR_GET_GREEN(x) ( 0xFF & ( x >> 8 ) )
#define COLOR_GET_BLUE(x) ( 0xFF & x )

#define DEVICE_POWERED_ON_LED_COLOR 0xFFFFFF

#define INDICATOR_LED_FADE_TIME_MS 20

#ifdef CONFIG_IDF_TARGET_ESP32S3
#define INDICATOR_LED_PIN 48
#elif CONFIG_IDF_TARGET_ESP32C3
#define INDICATOR_LED_PIN 8
#endif
#define INDICATOR_RGB_RMT_TX_CHANNEL RMT_CHANNEL_1

static led_strip_t indicator_led;

#elif defined(CONFIG_IDF_TARGET_ESP32) // CONFIG_IDF_TARGET_ESP32S3 OR OR CONFIG_IDF_TARGET_ESP32C3 OR CONFIG_IDF_TARGET_ESP32

#define INDICATOR_LED_PIN 2

#define INDICATTOR_LED_POWER_STATUS_BLINK_MS 20
#define INDICATTOR_LED_WIFI_STATUS_BLINK_MS 50
#define INDICATTOR_LED_INTERNET_STATUS_BLINK_MS 100
#define INDICATTOR_LED_CLOUD_STATUS_BLINK_MS 200

#endif // CONFIG_IDF_TARGET_ESP32S3 OR OR CONFIG_IDF_TARGET_ESP32C3 OR CONFIG_IDF_TARGET_ESP32

#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)

static int indicator_RGB_led_fade_out(uint16_t fade_time_ms)
{
    static int _brightness = 255;
    static uint32_t _time_stamp = 0;

    if ((xTaskGetTickCount() - _time_stamp) >= (fade_time_ms / portTICK_RATE_MS))
    {
        led_strip_set_brightness(&indicator_led, _brightness);
        led_strip_flush(&indicator_led);

        _brightness -= 5;
        _brightness = (_brightness < 0) ? 255 : _brightness;

        _time_stamp = xTaskGetTickCount();
    }

    return _brightness;
}

static int indicator_RGB_led_fade_up(uint16_t fade_time_ms)
{
    static int _brightness = 0;
    static uint32_t _time_stamp = 0;

    if ((xTaskGetTickCount() - _time_stamp) >= (fade_time_ms / portTICK_RATE_MS))
    {
        led_strip_set_brightness(&indicator_led, _brightness);
        led_strip_flush(&indicator_led);

        _brightness += 5;
        _brightness = (_brightness > 255) ? 0 : _brightness;
    }

    return _brightness;
}

// fade HIGH-LOW-HIGH
static int __LED_fade(uint16_t fade_time_ms, rgb_t _color)
{
    int ret = 0;
    static uint32_t _state = 0;

    led_strip_fill(&indicator_led, 0, indicator_led.length, _color);

    if (0 == _state)
    {
        if (255 == indicator_RGB_led_fade_out(fade_time_ms)) // we switch state if fade-out is completed once. On completion _brightness reset to 255, which indicates 'cycle complete'
        {
            _state = 1;
        }
    }
    else
    {
        if (0 == indicator_RGB_led_fade_up(fade_time_ms)) // we switch state if fade-up is completed once. On completion _brightness reset to 0, which indicates 'cycle complete'
        {
            ret = 1;
            _state = 0;
        }
    }

    return ret;
}

static int indicator_LED_fade_orange(uint16_t fade_time_ms)
{
    rgb_t _color = {
        .red = COLOR_GET_RED(COLOR_YELLOW),
        .green = COLOR_GET_GREEN(COLOR_YELLOW),
        .blue = COLOR_GET_BLUE(COLOR_YELLOW),
    };

    return __LED_fade(fade_time_ms, _color);
}

static int indicator_LED_fade_red(uint16_t fade_time_ms)
{
    rgb_t _color = {
        .red = COLOR_GET_RED(DEVICE_POWERED_ON_LED_COLOR),
        .green = 0,
        .blue = 0,
    };

    return __LED_fade(fade_time_ms, _color);
}

static int indicator_LED_fade_green(uint16_t fade_time_ms)
{
    rgb_t _color = {
        .red = 0,
        .green = COLOR_GET_GREEN(DEVICE_POWERED_ON_LED_COLOR),
        .blue = 0,
    };

    return __LED_fade(fade_time_ms, _color);
}

static int indicator_LED_fade_blue(uint16_t fade_time_ms)
{
    rgb_t _color = {
        .red = 0,
        .green = 0,
        .blue = COLOR_GET_BLUE(DEVICE_POWERED_ON_LED_COLOR),
    };

    return __LED_fade(fade_time_ms, _color);
}

static void __indicator_LED_power_on_effect()
{
    static uint32_t _state = 0;
    // TRACE_D("state: %d", _state);

    switch (_state)
    {
    case 0:
    {
        if (1 == indicator_LED_fade_red(INDICATOR_LED_FADE_TIME_MS))
        {
            _state = 1;
        }
        break;
    }
    case 1:
    {
        if (1 == indicator_LED_fade_green(INDICATOR_LED_FADE_TIME_MS))
        {
            _state = 2;
        }
        break;
    }
    case 2:
    {
        if (1 == indicator_LED_fade_blue(INDICATOR_LED_FADE_TIME_MS))
        {
            _state = 0;
        }
        break;
    }
    }

}

static void __indicator_LED_wifi_connected_effect()
{
    indicator_LED_fade_orange(INDICATOR_LED_FADE_TIME_MS);
}

static void __indicator_LED_internet_connected_effect()
{
    indicator_LED_fade_blue(INDICATOR_LED_FADE_TIME_MS);
}

static void __indicator_LED_cloud_connected_effect()
{
    indicator_LED_fade_green(INDICATOR_LED_FADE_TIME_MS);
}

static int __indicator_led_init(void)
{
    int ret = 0;

    indicator_led.length = 1;
    indicator_led.buf = NULL;
    indicator_led.brightness = 255;
    indicator_led.type = LED_STRIP_WS2812;
    indicator_led.gpio = INDICATOR_LED_PIN;
    indicator_led.channel = INDICATOR_RGB_RMT_TX_CHANNEL;

    led_strip_install();

    if (ESP_OK == led_strip_init(&indicator_led))
    {
        rgb_t color = {
            .red = COLOR_GET_RED(DEVICE_POWERED_ON_LED_COLOR),
            .green = COLOR_GET_GREEN(DEVICE_POWERED_ON_LED_COLOR),
            .blue = COLOR_GET_BLUE(DEVICE_POWERED_ON_LED_COLOR),
        };

        esp_err_t err = led_strip_fill(&indicator_led, 0, indicator_led.length, color);

        if (ESP_OK == (err = led_strip_set_brightness(&indicator_led, 255)))
        {
            if (ESP_OK == (err = led_strip_flush(&indicator_led)))
            {
                ezlopi_service_loop_add("indicator-loop", __indicator_LED_loop, 10, NULL);
                ret = 1;
            }
        }
    }

    return ret;
}


#elif defined(CONFIG_IDF_TARGET_ESP32) // // CONFIG_IDF_TARGET_ESP32S3 OR OR CONFIG_IDF_TARGET_ESP32C3

static const ledc_timer_config_t indicator_pwm_timer_cfg = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_12_BIT,
    .timer_num = LEDC_TIMER_3,
    .freq_hz = 5000,
    .clk_cfg = LEDC_AUTO_CLK,
};

static const ledc_channel_config_t indicator_pwm_channel_cfg = {
    .duty = 0,
    .channel = LEDC_CHANNEL_0,
    .timer_sel = LEDC_TIMER_3,
    .gpio_num = INDICATOR_LED_PIN,
    .intr_type = LEDC_INTR_DISABLE,
    .speed_mode = LEDC_LOW_SPEED_MODE,
};

static void indicator_LED_fade_effect(uint32_t fade_ms)
{
    static int i = 0;
    static uint32_t __state = 0;
    static uint32_t __fade_time_stamp = 0;

    if ((xTaskGetTickCount() - __fade_time_stamp) >= (fade_ms / portTICK_RATE_MS))
    {
        if (0 == __state)
        {
            if (i < 4095)
            {
                i += 117;
                ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i));
                ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
            }
            else
            {
                __state = 1;
            }
        }
        else
        {
            if (i > 0)
            {
                i -= 117;
                ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i);
                ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            }
            else
            {
                __state = 0;
            }
        }

        __fade_time_stamp = xTaskGetTickCount();
    }
}

static void __indicator_LED_power_on_effect()
{
    indicator_LED_fade_effect(50);
}

static void __indicator_LED_wifi_connected_effect()
{
    indicator_LED_fade_effect(10);
}

static void __indicator_LED_internet_connected_effect()
{
    indicator_LED_fade_effect(30);
}

static void __indicator_LED_cloud_connected_effect()
{
    indicator_LED_fade_effect(80);
}

static int __indicator_led_init(void)
{
    int ret = 0;
    if (ESP_OK == ledc_timer_config(&indicator_pwm_timer_cfg))
    {
        if (ESP_OK == ledc_channel_config(&indicator_pwm_channel_cfg))
        {
            ret = 1;
            ezlopi_service_loop_add("indicator-loop", __indicator_LED_loop, 1, NULL);
        }
    }

    return ret;
}

#endif // CONFIG_IDF_TARGET_ESP32S3 OR OR CONFIG_IDF_TARGET_ESP32C3


static void __process_event(void)
{
    e_ezlopi_event_t event = ezlopi_get_event_bit_status();

    __indicator_priority = PRIORITY_POWER;
    if ((event & EZLOPI_EVENT_WIFI_CONNECTED) == EZLOPI_EVENT_WIFI_CONNECTED)
    {
        __indicator_priority = PRIORITY_WIFI;
#ifdef CONFIG_EZPI_ENABLE_PING
        e_ping_status_t ping_status = ezlopi_ping_get_internet_status();
#else // CONFIG_EZPI_ENABLE_PING
        e_ping_status_t ping_status = EZLOPI_PING_STATUS_UNKNOWN;
#endif // CONFIG_EZPI_ENABLE_PING

        if (EZLOPI_PING_STATUS_LIVE == ping_status)
        {
            __indicator_priority = PRIORITY_INTERNET;
            if ((event & EZLOPI_EVENT_NMA_REG) == EZLOPI_EVENT_NMA_REG)
            {
                __indicator_priority = PRIORITY_CLOUD;
            }
        }
    }
}

static void __indicator_LED_loop(void *arg)
{
    __process_event();

    // TRACE_D("__indicator_priority: %d", __indicator_priority);

    switch (__indicator_priority)
    {
    case PRIORITY_POWER:
    {
        __indicator_LED_power_on_effect();
        break;
    }
    case PRIORITY_WIFI:
    {
        __indicator_LED_wifi_connected_effect();
        break;
    }
    case PRIORITY_INTERNET:
    {
        __indicator_LED_internet_connected_effect();
        break;
    }
    case PRIORITY_CLOUD:
    {
        __indicator_LED_cloud_connected_effect();
        break;
    }
    default:
    {
        break;
    }
    }

    // vTaskDelay(1 / portTICK_PERIOD_MS);
}

int ezlopi_service_led_indicator_init()
{
    return __indicator_led_init();
}


#endif // CONFIG_EZPI_ENABLE_LED_INDICATOR