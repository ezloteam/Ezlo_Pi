

#include "driver/rmt.h"
#include "led_strip.h"
#include "esp_err.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_service_led_inidicator.h"

#define COLOR_GET_RED(x) ( 0xFF & ( x >> 16 ))
#define COLOR_GET_GREEN(x) ( 0xFF & ( x >> 8 ) )
#define COLOR_GET_BLUE(x) ( 0xFF & x )

#define CONNECTED_BLINK_PERIOD 500 // 5000ms(0.5 sec)
#define DISCONNECTED_BLINE_PERIOD 1000 // 1000ms(1sec)

#define DEVICE_POWERED_ON_LED_COLOR 0xFFFFFF

#ifdef CONFIG_IDF_TARGET_ESP32S3
#define BLINK_LED_PIN 48
#define RMT_TX_CHANNEL RMT_CHANNEL_0
static rmt_config_t led_rmt_config = RMT_DEFAULT_CONFIG_TX(BLINK_LED_PIN, RMT_TX_CHANNEL);
led_rmt_config.clk_div = 2;
static led_strip_config_t led_strip_config = LED_STRIP_DEFAULT_CONFIG(BLINK_LED_PIN, (led_strip_dev_t)led_rmt_config.channel)
static led_strip_t * strip = NULL;
#endif


int ezlopi_service_led_indicator_init()
{
    int ret = 0;

#warning("Implement ezlopi-component/led-strip")

#ifdef CONFIG_IDF_TARGET_ESP32S3
    ESP_ERROR_CHECK(rmt_config(&led_rmt_config));
    ESP_ERROR_CHECK(rmt_driver_install(led_rmt_config.channel, 0, 0));
    strip = led_strip_new_rmt_ws2812(&led_strip_config);
    if (strip)
    {
        TRACE_E("Error creating WS2821 RGB led instance");
        ret = 0;
    }
    else
    {
        ESP_ERROR_CHECK(strip->clear(strip, 100));
        ESP_ERROR_CHECK(strip->set_pixel(strip, 1, COLOR_GET_RED(DEVICE_POWERED_ON_LED_COLOR), COLOR_GET_GREEN(DEVICE_POWERED_ON_LED_COLOR), COLOR_GET_BLUE(DEVICE_POWERED_ON_LED_COLOR)));
        ESP_ERROR_CHECK(strip->refresh(strip));
    }
#endif

    return ret;
}


