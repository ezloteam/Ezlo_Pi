#include <string.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "switch_service.h"
#include "interface_common.h"
#include "web_provisioning.h"
#include "devices_common.h"
#include "items.h"
#include "debug.h"
#include "freertos/portmacro.h"
#include "wss.h"

#define NUMBER_OF_GPIO 28
static const uint32_t switch_debounce_time = 1000;
static TickType_t gpio_intr_tick_arr[NUMBER_OF_GPIO];

static void __gpio_intr_proces(void *pv);

void switch_service_init(void)
{
    memset(gpio_intr_tick_arr, 0, sizeof(gpio_intr_tick_arr));
    xTaskCreate(__gpio_intr_proces, "__gpio_intr_proces", 10 * 2048, NULL, 3, NULL);
}

static void __gpio_intr_proces(void *pv)
{
    s_device_properties_t *device_list = devices_common_device_list();

    while (1)
    {
        const uint32_t event_gpio_n = interface_common_get_gpio_isr_event(UINT32_MAX);
        TRACE_D("Got gpio event -> GPIO num: %d", event_gpio_n);
        TickType_t tick_now = xTaskGetTickCount();

        if ((UINT32_MAX != event_gpio_n) &&
            (event_gpio_n < NUMBER_OF_GPIO) &&
            ((tick_now - gpio_intr_tick_arr[event_gpio_n]) > switch_debounce_time)) // debounce time is 100 miliseconds
        {
            // uint32_t _state = interface_common_gpio_state_get(device_list[event_gpio_n].out_gpio);

            TRACE_B("debounce time: %u", tick_now - gpio_intr_tick_arr[event_gpio_n]);
            gpio_intr_tick_arr[event_gpio_n] = tick_now;

            for (int idx = 0; idx < MAX_DEV; idx++)
            {
                if (event_gpio_n == device_list[idx].input_gpio)
                {
                    uint32_t new_state = interface_common_gpio_state_get(device_list[idx].out_gpio) ? 0 : 1;
                    TRACE_B("Setting pin: %d -> %d", device_list[idx].out_gpio, new_state)
                    interface_common_gpio_state_set(device_list[idx].out_gpio, new_state);
                    char *j_response = items_update_with_device_index(NULL, 0, NULL, web_provisioning_get_message_count(), idx);
                    if (j_response)
                    {
                        wss_client_send(j_response, strlen(j_response));
                        free(j_response);
                        j_response = NULL;
                    }

                    break;
                }
            }
        }
    }
}

#if 0
static uint32_t __debounce(uint32_t gpio_n)
{
    uint32_t current_state = gpio_get_level((gpio_num_t)gpio_n);
    uint32_t last_state = current_state;
    uint32_t debounce_start = xTaskGetTickCount();
    const uint32_t debounce_timeout = 3000; // mili-seconds

    while (current_state == last_state)
    {
        vTaskDelay(40 / portTICK_RATE_MS);
        if ((xTaskGetTickCount() - debounce_start) > debounce_timeout)
        {
            break;
        }
        current_state = gpio_get_level((gpio_num_t)gpio_n);
    }

    return last_state;
}
#endif