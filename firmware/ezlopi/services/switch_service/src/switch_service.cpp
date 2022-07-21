#include <string>
#include <cstring>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "switch_service.h"
#include "interface_common.h"
#include "web_provisioning.h"
#include "devices_common.h"
#include "items.h"
#include "debug.h"

typedef struct s_prev_state
{
    TickType_t tick;
    uint32_t state;
} s_prev_state_t;

#define NUMBER_OF_GPIO 28
static const uint32_t max_switch_wait = 3000;
static s_prev_state_t prev_state[NUMBER_OF_GPIO];
static TickType_t gpio_intr_tick_arr[NUMBER_OF_GPIO];

switch_service *switch_service::switch_service_ = nullptr;
switch_service *switch_service::get_instance(void)
{
    if (nullptr == switch_service_)
    {
        switch_service_ = new switch_service();
        switch_service_->init();
    }

    return switch_service_;
}

uint32_t switch_service::debounce(uint32_t gpio_n)
{
    uint32_t current_state = gpio_get_level((gpio_num_t)gpio_n);
    uint32_t last_state = current_state;
    uint32_t debounce_start = xTaskGetTickCount();
    const uint32_t debounce_timeout = 3000; // mili-seconds

    while (current_state == last_state)
    {
        vTaskDelay(40);
        if ((xTaskGetTickCount() - debounce_start) > debounce_timeout)
        {
            break;
        }
        current_state = gpio_get_level((gpio_num_t)gpio_n);
    }

    return last_state;
}

void switch_service::gpio_intr_proces(void *pv)
{
    items *items_ctx = items::get_instance();
    interface_common *gpio_ctx = interface_common::get_instance();
    devices_common *devices_ctx = devices_common::get_instance();
    web_provisioning *web_prov_ctx = web_provisioning::get_instance();
    s_device_properties_t *device_list = devices_ctx->device_list();

    while (1)
    {
        const uint32_t event_gpio_n = gpio_ctx->get_gpio_isr_event(UINT32_MAX);
        TRACE_D("Got gpio event -> GPIO num: %d", event_gpio_n);
        TickType_t tick_now = xTaskGetTickCount();

        if ((UINT32_MAX != event_gpio_n) &&
            (event_gpio_n < NUMBER_OF_GPIO) &&
            ((tick_now - gpio_intr_tick_arr[event_gpio_n]) > 1000)) // debounce time is 100 miliseconds
        {
            uint32_t _state = gpio_ctx->gpio_state_get(device_list[event_gpio_n].out_gpio);

            // while ((xTaskGetTickCount() - tick_now) < max_switch_wait)
            // {
            //     if (_state != gpio_ctx->gpio_state_get(device_list[event_gpio_n].out_gpio))
            //     {
            //         break;
            //     }
            //     vTaskDelay(100);
            // }

            TRACE_B("debounce time: %u", tick_now - gpio_intr_tick_arr[event_gpio_n]);
            gpio_intr_tick_arr[event_gpio_n] = tick_now;

            for (int idx = 0; idx < MAX_DEV; idx++)
            {
                if (event_gpio_n == device_list[idx].input_gpio)
                {
                    uint32_t new_state = gpio_ctx->gpio_state_get(device_list[idx].out_gpio) ? 0 : 1;
                    TRACE_B("Setting pin: %d -> %d", device_list[idx].out_gpio, new_state)
                    gpio_ctx->gpio_state_set(device_list[idx].out_gpio, new_state);
                    string j_response = items_ctx->update(NULL, 0, NULL, web_prov_ctx->get_message_count(), idx);
                    web_prov_ctx->send_to_nma_websocket(j_response);
                    break;
                }
            }
        }
    }
}

void switch_service::init(void)
{
    memset(gpio_intr_tick_arr, 0, sizeof(gpio_intr_tick_arr));
    xTaskCreate(gpio_intr_proces, "GPIO INTR PROCESS", 10 * 2048, NULL, 3, NULL);
}