

/**
 * @file    ezlopi_service_led_indicator.c
 * @brief
 * @authors Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version
 * @date
 */
/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_ENABLE_LED_INDICATOR

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/rmt.h"
#include "led_strip.h"
#include "esp_err.h"
#include "color_codes.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_event_group.h"
#include "ezlopi_core_ping.h"
#include "ezlopi_core_processes.h"

#include "ezlopi_service_loop.h"
#include "ezlopi_service_led_indicator.h"

#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)
/**
 * @brief Macro to extract red part of color on RGB format color
 *
 * @param[in] x 24-bit color code
 * Example
 * @code
 * int magenta = 0xFF00FF;
 * int red = COLOR_GET_RED(magenta)
 * @endcode
 *
 */
#define COLOR_GET_RED(x) (0xFF & (x >> 16))
/**
 * @brief Macro to extract green part of color on RGB format color
 *
 * @param[in] x 24-bit color code
 * Example
 * @code
 * int yellow = 0xFFFF00;
 * int green = COLOR_GET_GREEN(yellow)
 * @endcode
 *
 */
#define COLOR_GET_GREEN(x) (0xFF & (x >> 8))
/**
 * @brief Macro to extract blue part of color on RGB format color
 *
 * @param[in] x 24-bit color code
 * Example
 * @code
 * int magenta = 0xFF00FF;
 * int blue = COLOR_GET_BLUE(magenta)
 * @endcode
 *
 */
#define COLOR_GET_BLUE(x) (0xFF & x)

/**
 * @brief Color to be set to the indicator when device is just powered on
 *
 */
#define DEVICE_POWERED_ON_LED_COLOR 0xFFFFFF
/**
 * @brief Time indicator will take to fade in and fade out
 *
 */
#define INDICATOR_LED_FADE_TIME_MS 20

#ifdef CONFIG_IDF_TARGET_ESP32S3
/**
 * @brief Pin number the onboard LED strip is connected to on ESP32S3
 *
 */
#define INDICATOR_LED_PIN 48
#elif CONFIG_IDF_TARGET_ESP32C3
/**
 * @brief Pin number the onboard LED strip is connected to on ESP32C3
 *
 */
#define INDICATOR_LED_PIN 8
#endif
/**
 * @brief Default RMT channel to use for LED strip
 *
 */
#define INDICATOR_RGB_RMT_TX_CHANNEL RMT_CHANNEL_1

#elif defined(CONFIG_IDF_TARGET_ESP32) // CONFIG_IDF_TARGET_ESP32S3 OR OR CONFIG_IDF_TARGET_ESP32C3 OR CONFIG_IDF_TARGET_ESP32

/**
 * @brief Pin number the onboard LED is connected to on ESP32
 *
 */
#define INDICATOR_LED_PIN 2
/**
 * @brief Time indicator will take to fade in and fade out when powered on
 *
 */
#define INDICATTOR_LED_POWER_STATUS_BLINK_MS 20
/**
 * @brief Time indicator will take to fade in and fade out when WiFi is connected
 *
 */
#define INDICATTOR_LED_WIFI_STATUS_BLINK_MS 50
/**
 * @brief Time indicator will take to fade in and fade out when internet is connected
 *
 */
#define INDICATTOR_LED_INTERNET_STATUS_BLINK_MS 100
/**
 * @brief Time indicator will take to fade in and fade out when connected to cloud
 *
 */
#define INDICATTOR_LED_CLOUD_STATUS_BLINK_MS 200

#endif // CONFIG_IDF_TARGET_ESP32S3 OR OR CONFIG_IDF_TARGET_ESP32C3 OR CONFIG_IDF_TARGET_ESP32

/**
 * @brief Function to change the indicator LED indicator according to the priority
 *
 * @param[in] arg Loop argument
 */
static void ezpi_indicator_LED_loop(void *arg);
/**
 * @brief Function to set LED indicator on power on effect
 *
 */
static void ezpi_indicator_LED_power_on_effect();
/**
 * @brief Function to set LED indicator on WiFi connected effect
 *
 */
static void ezpi_indicator_LED_wifi_connected_effect();
/**
 * @brief Function to set LED indicator on internet connected effect
 *
 */
static void ezpi_indicator_LED_internet_connected_effect();
/**
 * @brief Function to set LED indicator on cloud connected effect
 *
 */
static void ezpi_indicator_LED_cloud_connected_effect();
/**
 * @brief Function to initialize indicator LED
 *
 */
static int ezpi_indicator_led_init(void);
/**
 * @brief Function to set LED indicator priority by checking several event group bits
 *
 */
static void ezpi_process_event(void);
#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)
/**
 * @brief Function controls LED indicator fade out
 *
 * @param fade_time_ms Time in ms to fadeout LED indicator
 * @return int
 * @retval Current brightness of the LED
 */
static int ezpi_indicator_RGB_led_fade_out(uint16_t fade_time_ms);
/**
 * @brief Function controls LED indicator fade in
 *
 * @param fade_time_ms Time in ms to fadein LED indicator
 * @return int
 * @retval Current brightness of the LED
 */
static int ezpi_indicator_RGB_led_fade_up(uint16_t fade_time_ms);
/**
 * @brief Function controls LED indicator fade in and fadeout
 *
 * @param fade_time_ms Time in ms to fade in/out LED indicator
 * @param _color Color to set to the LED
 * @return int
 * @retval 1 on success or 0 on error
 */
static int ezpi_LED_fade(uint16_t fade_time_ms, rgb_t _color);
/**
 * @brief Function to set LED to orange color while fading in and out
 *
 * @param fade_time_ms time in ms to fade the LED indicator
 * @return int
 * @retval 1 on success or 0 on error
 */
static int ezpi_indicator_LED_fade_orange(uint16_t fade_time_ms);
/**
 * @brief Function to set LED to red color while fading in and out
 *
 * @param fade_time_ms time in ms to fade the LED indicator
 * @return int
 * @retval 1 on success or 0 on error
 */
static int ezpi_indicator_LED_fade_red(uint16_t fade_time_ms);
/**
 * @brief Function to set LED to green color while fading in and out
 *
 * @param fade_time_ms time in ms to fade the LED indicator
 * @return int
 * @retval 1 on success or 0 on error
 */
static int ezpi_indicator_LED_fade_green(uint16_t fade_time_ms);
/**
 * @brief Function to set LED to blue color while fading in and out
 *
 * @param fade_time_ms time in ms to fade the LED indicator
 * @return int
 * @retval 1 on success or 0 on error
 */
static int ezpi_indicator_LED_fade_blue(uint16_t fade_time_ms);
#elif defined(CONFIG_IDF_TARGET_ESP32)
/**
 * @brief Fuction to set duty cycle for the LED indicator to fade in or out
 *
 * @param fade_ms time in ms to fade the LED indicator
 */
static void ezpi_indicator_LED_fade_effect(uint32_t fade_ms);
#endif

static e_indicator_led_priority_t __indicator_priority = PRIORITY_CLOUD;
#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)
static led_strip_t indicator_led;
#elif defined(CONFIG_IDF_TARGET_ESP32)
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
#endif

int EZPI_service_led_indicator_init()
{
    return ezpi_indicator_led_init();
}

static void ezpi_process_event(void)
{
    e_ezlopi_event_t event = EZPI_core_event_group_get_eventbit_status();

    __indicator_priority = PRIORITY_POWER;
    if ((event & EZLOPI_EVENT_WIFI_CONNECTED) == EZLOPI_EVENT_WIFI_CONNECTED)
    {
        __indicator_priority = PRIORITY_WIFI;
#ifdef CONFIG_EZPI_ENABLE_PING
        e_ping_status_t ping_status = EZPI_core_ping_get_internet_status();
#else  // CONFIG_EZPI_ENABLE_PING
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

static void ezpi_indicator_LED_loop(void *arg)
{
    ezpi_process_event();

    switch (__indicator_priority)
    {
    case PRIORITY_POWER:
    {
        ezpi_indicator_LED_power_on_effect();
        break;
    }
    case PRIORITY_WIFI:
    {
        ezpi_indicator_LED_wifi_connected_effect();
        break;
    }
    case PRIORITY_INTERNET:
    {
        ezpi_indicator_LED_internet_connected_effect();
        break;
    }
    case PRIORITY_CLOUD:
    {
        ezpi_indicator_LED_cloud_connected_effect();
        break;
    }
    default:
    {
        break;
    }
    }
}

#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)

static int ezpi_indicator_RGB_led_fade_out(uint16_t fade_time_ms)
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

static int ezpi_indicator_RGB_led_fade_up(uint16_t fade_time_ms)
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
static int ezpi_LED_fade(uint16_t fade_time_ms, rgb_t _color)
{
    int ret = 0;
    static uint32_t _state = 0;

    led_strip_fill(&indicator_led, 0, indicator_led.length, _color);

    if (0 == _state)
    {
        if (255 == ezpi_indicator_RGB_led_fade_out(fade_time_ms)) // we switch state if fade-out is completed once. On completion _brightness reset to 255, which indicates 'cycle complete'
        {
            _state = 1;
        }
    }
    else
    {
        if (0 == ezpi_indicator_RGB_led_fade_up(fade_time_ms)) // we switch state if fade-up is completed once. On completion _brightness reset to 0, which indicates 'cycle complete'
        {
            ret = 1;
            _state = 0;
        }
    }

    return ret;
}

static int ezpi_indicator_LED_fade_orange(uint16_t fade_time_ms)
{
    rgb_t _color = {
        .red = COLOR_GET_RED(COLOR_YELLOW),
        .green = COLOR_GET_GREEN(COLOR_YELLOW),
        .blue = COLOR_GET_BLUE(COLOR_YELLOW),
    };

    return ezpi_LED_fade(fade_time_ms, _color);
}

static int ezpi_indicator_LED_fade_red(uint16_t fade_time_ms)
{
    rgb_t _color = {
        .red = COLOR_GET_RED(DEVICE_POWERED_ON_LED_COLOR),
        .green = 0,
        .blue = 0,
    };

    return ezpi_LED_fade(fade_time_ms, _color);
}

static int ezpi_indicator_LED_fade_green(uint16_t fade_time_ms)
{
    rgb_t _color = {
        .red = 0,
        .green = COLOR_GET_GREEN(DEVICE_POWERED_ON_LED_COLOR),
        .blue = 0,
    };

    return ezpi_LED_fade(fade_time_ms, _color);
}

static int ezpi_indicator_LED_fade_blue(uint16_t fade_time_ms)
{
    rgb_t _color = {
        .red = 0,
        .green = 0,
        .blue = COLOR_GET_BLUE(DEVICE_POWERED_ON_LED_COLOR),
    };

    return ezpi_LED_fade(fade_time_ms, _color);
}

static void ezpi_indicator_LED_power_on_effect()
{
    static uint32_t _state = 0;

    switch (_state)
    {
    case 0:
    {
        if (1 == ezpi_indicator_LED_fade_red(INDICATOR_LED_FADE_TIME_MS))
        {
            _state = 1;
        }
        break;
    }
    case 1:
    {
        if (1 == ezpi_indicator_LED_fade_green(INDICATOR_LED_FADE_TIME_MS))
        {
            _state = 2;
        }
        break;
    }
    case 2:
    {
        if (1 == ezpi_indicator_LED_fade_blue(INDICATOR_LED_FADE_TIME_MS))
        {
            _state = 0;
        }
        break;
    }
    }
}

static void ezpi_indicator_LED_wifi_connected_effect()
{
    ezpi_indicator_LED_fade_orange(INDICATOR_LED_FADE_TIME_MS);
}

static void ezpi_indicator_LED_internet_connected_effect()
{
    ezpi_indicator_LED_fade_blue(INDICATOR_LED_FADE_TIME_MS);
}

static void ezpi_indicator_LED_cloud_connected_effect()
{
    ezpi_indicator_LED_fade_green(INDICATOR_LED_FADE_TIME_MS);
}

static int ezpi_indicator_led_init(void)
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
                EZPI_service_loop_add("indicator-loop", ezpi_indicator_LED_loop, 10, NULL);
                ret = 1;
            }
        }
    }

    return ret;
}

#elif defined(CONFIG_IDF_TARGET_ESP32) // // CONFIG_IDF_TARGET_ESP32S3 OR OR CONFIG_IDF_TARGET_ESP32C3
static void ezpi_indicator_LED_fade_effect(uint32_t fade_ms)
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

static void ezpi_indicator_LED_power_on_effect()
{
    ezpi_indicator_LED_fade_effect(50);
}

static void ezpi_indicator_LED_wifi_connected_effect()
{
    ezpi_indicator_LED_fade_effect(10);
}

static void ezpi_indicator_LED_internet_connected_effect()
{
    ezpi_indicator_LED_fade_effect(30);
}

static void ezpi_indicator_LED_cloud_connected_effect()
{
    ezpi_indicator_LED_fade_effect(80);
}

static int ezpi_indicator_led_init(void)
{
    int ret = 0;
    if (ESP_OK == ledc_timer_config(&indicator_pwm_timer_cfg))
    {
        if (ESP_OK == ledc_channel_config(&indicator_pwm_channel_cfg))
        {
            ret = 1;
            EZPI_service_loop_add("indicator-loop", ezpi_indicator_LED_loop, 1, NULL);
        }
    }

    return ret;
}

#endif // CONFIG_IDF_TARGET_ESP32S3 OR OR CONFIG_IDF_TARGET_ESP32C3

#endif // CONFIG_EZPI_ENABLE_LED_INDICATOR

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
