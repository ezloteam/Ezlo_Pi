#include <stdio.h>

#include <esp_system.h>
#include <driver/adc.h>
#include <driver/i2c.h>
#include <driver/gpio.h>
#include <mbedtls/config.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_ezlopi.h"
#include "ezlopi_service_ota.h"
#include "ezlopi_service_ble.h"
#include "ezlopi_service_uart.h"
#include "ezlopi_service_timer.h"
#include "ezlopi_service_modes.h"
#include "ezlopi_service_meshbot.h"
#include "ezlopi_service_gpioisr.h"
#include "ezlopi_service_webprov.h"
#include "ezlopi_service_ws_server.h"
#include "ezlopi_service_broadcast.h"

#include "ezlopi_core_ble_config.h"
#include "ezlopi_core_processes.h"

#define ENABLE_HEARTBIT_LED 0

static void blinky(void* pv);

void app_main(void)
{
    gpio_install_isr_service(0);

    gpio_isr_service_init();
    EZPI_SERVICE_uart_init();

    ezlopi_init();

    timer_service_init();
    ezlopi_ble_service_init();

    ezlopi_scenes_meshbot_init();
    ezlopi_service_modes_init();

    ezlopi_service_ws_server_start();
    ezlopi_service_web_provisioning_init();

    ota_service_init();
    ezlopi_service_broadcast_init();

    TaskHandle_t ezlopi_main_blinky_task_handle = NULL;
    xTaskCreate(blinky, "blinky", EZLOPI_MAIN_BLINKY_TASK_DEPTH, NULL, 1, &ezlopi_main_blinky_task_handle);
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_MAIN_BLINKY_TASK, &ezlopi_main_blinky_task_handle, EZLOPI_MAIN_BLINKY_TASK_DEPTH);
}

static void blinky(void* pv)
{
#if (1 == ENABLE_HEARTBIT_LED)
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    uint32_t state = 0;
    gpio_config(&io_conf);
#endif

    uint32_t count = 0;
    while (1)
    {
#if (1 == ENABLE_HEARTBIT_LED)
        state ^= 1;
        gpio_set_level(GPIO_NUM_2, state);
#endif

        if (count++ > 10)
        {
            count = 0;

            UBaseType_t total_task_numbers = uxTaskGetNumberOfTasks();
            TaskStatus_t task_array[total_task_numbers];

            TRACE_D("----------------------------------------------");
            TRACE_D("Free Heap Size: %.2f KB", esp_get_free_heap_size() / 1024.0);
            TRACE_D("Minimum Free Heap Size: %.2f KB", esp_get_minimum_free_heap_size() / 1024.0);

            uxTaskGetSystemState(task_array, total_task_numbers, NULL);

            for (int i = 0; i < total_task_numbers; i++) {
                TRACE_D("Process Name: %s, \tPID: %d, \tBase: %p, \tWatermark: %.2f KB",
                    task_array[i].pcTaskName,
                    task_array[i].xTaskNumber,
                    task_array[i].pxStackBase,
                    task_array[i].usStackHighWaterMark / 1024.0);
            }

            TRACE_D("----------------------------------------------");

        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
