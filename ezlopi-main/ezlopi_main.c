#include <stdio.h>

#include <esp_system.h>
#include <driver/adc.h>
#include <driver/i2c.h>
#include <driver/gpio.h>
#include <mbedtls/config.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_util_trace.h"
#include "EZLOPI_USER_CONFIG.h"

#include "ezlopi_core_ezlopi.h"
#include "ezlopi_service_ota.h"

#include "ezlopi_service_ble.h"
#if CONFIG_EZLOPI_BLE_ENABLE == 1
#endif

#include "ezlopi_service_uart.h"
#include "ezlopi_service_timer.h"
#include "ezlopi_service_modes.h"
#include "ezlopi_service_meshbot.h"
#include "ezlopi_service_gpioisr.h"
#include "ezlopi_service_webprov.h"
#include "ezlopi_service_ws_server.h"
#include "ezlopi_service_broadcast.h"

#include "pt.h"

#define ENABLE_HEARTBIT_LED 0

static void __blinky(void* pv);
static void __init_heartbeat_led(void);
static void __toggle_heartbeat_led(void);

static struct pt pt1;

PT_THREAD(example(struct pt* pt))
{
    static uint32_t curr_ticks;
    PT_BEGIN(pt);

    while (1)
    {
        curr_ticks = xTaskGetTickCount();
        PT_WAIT_UNTIL(pt, (xTaskGetTickCount() - curr_ticks) > 1000);
        __toggle_heartbeat_led();
    }

    PT_END(pt);
}

void app_main(void)
{
    gpio_install_isr_service(0);

    gpio_isr_service_init();

    ezlopi_init();

    EZPI_SERVICE_uart_init();

    timer_service_init();

#if CONFIG_EZLOPI_BLE_ENABLE == 1
    ezlopi_ble_service_init();
#endif

    ezlopi_service_broadcast_init();
    ezlopi_service_ws_server_start();
    ezlopi_service_web_provisioning_init();

    ezlopi_service_ota_init();
#if CONFIG_EZLPI_SERV_ENABLE_MODES
    ezlopi_service_modes_init();
#endif
#if CONFIG_EZPI_SERV_ENABLE_MESHBOTS
    ezlopi_scenes_meshbot_init();
#endif

    xTaskCreate(__blinky, "__blinky", 2 * 2048, NULL, 1, NULL);
}

static void __init_heartbeat_led(void)
{
#if (1 == ENABLE_HEARTBIT_LED)
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&io_conf);
#endif
}

static void __toggle_heartbeat_led(void)
{
#if (1 == ENABLE_HEARTBIT_LED)
    static uint32_t state = 0;

    state ^= 1;
    gpio_set_level(GPIO_NUM_2, state);
#endif
}

static void __blinky(void* pv)
{
    __init_heartbeat_led();

    PT_INIT(&pt1);
    uint32_t count = 0;

    while (1)
    {
        if (count++ > 1000)
        {
            count = 0;

            trace_wb("----------------------------------------------");
            trace_wb("esp_get_free_heap_size - %f kB", esp_get_free_heap_size() / 1024.0);
            trace_wb("esp_get_minimum_free_heap_size: %f kB", esp_get_minimum_free_heap_size() / 1024.0);
            trace_wb("----------------------------------------------");
        }

        vTaskDelay(5 / portTICK_PERIOD_MS);
        example(&pt1);
    }
}
