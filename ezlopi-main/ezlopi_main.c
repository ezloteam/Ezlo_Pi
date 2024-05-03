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
#include "ezlopi_core_log.h"

#include "ezlopi_service_ble.h"
#include "ezlopi_service_uart.h"
#include "ezlopi_service_timer.h"
#include "ezlopi_service_modes.h"
#include "ezlopi_service_meshbot.h"
#include "ezlopi_service_gpioisr.h"
#include "ezlopi_service_webprov.h"
#include "ezlopi_service_ws_server.h"
#include "ezlopi_service_broadcast.h"
#include "ezlopi_service_led_indicator.h"

#include "pt.h"
#include "ezlopi_core_ble_config.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_util_heap.h"

static void blinky(void* pv);


void app_main(void)
{

#ifdef CONFIG_EZPI_ENABLE_LED_INDICATOR
    ezlopi_service_led_indicator_init();
#endif // CONFIG_EZPI_ENABLE_LED_INDICATOR
    gpio_install_isr_service(0);

    gpio_isr_service_init();

    ezlopi_init();

#if defined(CONFIG_EZPI_ENABLE_UART_PROVISIONING)
    EZPI_SERV_uart_init();
#endif

    timer_service_init();

#if defined(CONFIG_EZPI_BLE_ENABLE)
    ezlopi_ble_service_init();
#endif

#if defined(CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER) || defined(CONFIG_EZPI_WEBSOCKET_CLIENT)
    ezlopi_service_broadcast_init();
#endif

#if defined(CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER)
    ezlopi_service_ws_server_start();
#endif

#if defined(CONFIG_EZPI_WEBSOCKET_CLIENT)
    ezlopi_service_web_provisioning_init();
#endif

#if defined(CONFIG_EZPI_ENABLE_OTA)
    ezlopi_service_ota_init();
#endif

#if CONFIG_EZLPI_SERV_ENABLE_MODES
    ezlopi_service_modes_init();
#endif

#if CONFIG_EZPI_SERV_ENABLE_MESHBOTS
    ezlopi_scenes_meshbot_init();
#endif
    ezlopi_core_log_set_broadcaster();

    TaskHandle_t ezlopi_main_blinky_task_handle = NULL;
    xTaskCreate(blinky, "blinky", EZLOPI_MAIN_BLINKY_TASK_DEPTH, NULL, 1, &ezlopi_main_blinky_task_handle);
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_MAIN_BLINKY_TASK, &ezlopi_main_blinky_task_handle, EZLOPI_MAIN_BLINKY_TASK_DEPTH);

}

static void blinky(void* pv)
{

    uint32_t count = 0;

    while (1)
    {
        uint32_t low_heap_start_time = xTaskGetTickCount();
        float free_heap_kb = esp_get_free_heap_size() / 1024.0;

        UBaseType_t total_task_numbers = uxTaskGetNumberOfTasks();
        TaskStatus_t task_array[total_task_numbers];

        TRACE_D("----------------------------------------------");
        TRACE_D("Free Heap Size: %.2f KB", esp_get_free_heap_size() / 1024.0);
        TRACE_D("Minimum Free Heap Size: %.2f KB", esp_get_minimum_free_heap_size() / 1024.0);

#ifdef CONFIG_EZPI_HEAP_ENABLE
        ezlopi_util_heap_trace();

        if (free_heap_kb <= 10)
        {
            TRACE_W("CRITICAL-WARNING: low heap detected..");
            ezlopi_util_heap_trace();
        }
        else if ((xTaskGetTickCount() - low_heap_start_time) > (15000 / portTICK_PERIOD_MS))
        {
            ezlopi_util_heap_trace();
            vTaskDelay(2000 / portTICK_RATE_MS);
            TRACE_E("CRITICAL-ERROR: low heap time-out detected!");
            TRACE_W("Rebooting.....");
            esp_restart();
        }
        else
        {
            low_heap_start_time = xTaskGetTickCount();
    }
#endif // CONFIG_EZPI_HEAP_ENABLE

#if 0

        uxTaskGetSystemState(task_array, total_task_numbers, NULL);

        for (int i = 0; i < total_task_numbers; i++) {
            if (task_array[i].pcTaskName)
            {
                TRACE_D("Process Name: %s, \tPID: %d, \tBase: %p, \tWatermark: %.2f KB",
                    task_array[i].pcTaskName,
                    task_array[i].xTaskNumber,
                    task_array[i].pxStackBase,
                    task_array[i].usStackHighWaterMark / 1024.0);
            }
}
#endif 
        TRACE_D("----------------------------------------------");
#ifdef CONFIG_EZPI_HEAP_ENABLE
        ezlopi_util_heap_flush();
#endif // CONFIG_EZPI_HEAP_ENABLE        
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
