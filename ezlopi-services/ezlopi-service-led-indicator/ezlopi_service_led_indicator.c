

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_ENABLE_LED_INDICATOR

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

static void __indicator_LED_blinker(void* params);

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

static void indicator_RGB_led_fade_out(uint16_t fade_time_ms)
{
    for (int i = 255; i >= 0; i -= 5)
    {
        led_strip_set_brightness(&indicator_led, i);
        led_strip_flush(&indicator_led);
        vTaskDelay(fade_time_ms / portTICK_PERIOD_MS);
    }
}

static void indicator_RGB_led_fade_up(uint16_t fade_time_ms)
{
    for (int i = 0; i < 255; i += 5)
    {
        led_strip_set_brightness(&indicator_led, i);
        led_strip_flush(&indicator_led);
        vTaskDelay(fade_time_ms / portTICK_PERIOD_MS);
    }
}

static void indicator_LED_fade_red(uint16_t fade_time_ms)
{
    rgb_t color = {
            .red = COLOR_GET_RED(DEVICE_POWERED_ON_LED_COLOR),
            .green = 0,
            .blue = 0,
    };
    led_strip_fill(&indicator_led, 0, indicator_led.length, color);
    indicator_RGB_led_fade_out(fade_time_ms);
    indicator_RGB_led_fade_up(fade_time_ms);
}

static void indicator_LED_fade_green(uint16_t fade_time_ms)
{
    rgb_t color = {
            .red = 0,
            .green = COLOR_GET_GREEN(DEVICE_POWERED_ON_LED_COLOR),
            .blue = 0,
    };
    led_strip_fill(&indicator_led, 0, indicator_led.length, color);
    indicator_RGB_led_fade_out(fade_time_ms);
    indicator_RGB_led_fade_up(fade_time_ms);
}

static void indicator_LED_fade_blue(uint16_t fade_time_ms)
{
    rgb_t color = {
            .red = 0,
            .green = 0,
            .blue = COLOR_GET_BLUE(DEVICE_POWERED_ON_LED_COLOR),
    };
    led_strip_fill(&indicator_led, 0, indicator_led.length, color);
    indicator_RGB_led_fade_out(fade_time_ms);
    indicator_RGB_led_fade_up(fade_time_ms);
}

static void __indicator_LED_power_on_effect()
{
    indicator_LED_fade_red(INDICATOR_LED_FADE_TIME_MS);
    indicator_LED_fade_green(INDICATOR_LED_FADE_TIME_MS);
    indicator_LED_fade_blue(INDICATOR_LED_FADE_TIME_MS);
}

static void __indicator_LED_wifi_connected_effect()
{
    indicator_LED_fade_red(INDICATOR_LED_FADE_TIME_MS);
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
    indicator_led.type = LED_STRIP_WS2812;
    indicator_led.length = 1;
    indicator_led.gpio = INDICATOR_LED_PIN;
    indicator_led.buf = NULL;
    indicator_led.brightness = 255;
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
                TaskHandle_t ezlopi_service_led_indicator_task_handle = NULL;
                xTaskCreate(__indicator_LED_blinker, "indicator_task", EZLOPI_SERVICE_LED_INDICATOR_TASK_DEPTH, NULL, tskIDLE_PRIORITY, &ezlopi_service_led_indicator_task_handle);
                ezlopi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_LED_INDICATOR_TASK, &ezlopi_service_led_indicator_task_handle, EZLOPI_SERVICE_LED_INDICATOR_TASK_DEPTH);
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
    for (; i < 4095; i += 117)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i));
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
        vTaskDelay(fade_ms / portTICK_PERIOD_MS);
    }
    for (; i > 0; i -= 117)
    {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(fade_ms / portTICK_PERIOD_MS);
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
            TaskHandle_t indicator_task_handle = NULL;
            xTaskCreate(__indicator_LED_blinker, "indicator_task", 2048 * 2, NULL, 1, &indicator_task_handle);
            ezlopi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_LED_INDICATOR_TASK, &indicator_task_handle, EZLOPI_SERVICE_LED_INDICATOR_TASK_DEPTH);
            ret = 1;
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

static void __indicator_LED_blinker(void* params)
{
    while (1)
    {
        __process_event();

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

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    ezlopi_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_LED_INDICATOR_TASK);
    vTaskDelete(NULL);
}

int ezlopi_service_led_indicator_init()
{
    return __indicator_led_init();
}


#endif // CONFIG_EZPI_ENABLE_LED_INDICATOR