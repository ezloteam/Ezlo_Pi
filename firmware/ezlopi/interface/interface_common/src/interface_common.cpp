#include <string>
#include <cstring>
#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "debug.h"
#include "nvs_storage.h"
#include "interface_common.h"

#define USE_RTOS_QUEUE 1
#if (1 == USE_RTOS_QUEUE)
static xQueueHandle gpio_evt_queue;
#else
static SemaphoreHandle_t gpio_smphr;
#endif
static uint8_t gpio_state[28] = {0}; // value of pin

#warning "WARNING: Remove this after test"
static uint8_t gpio_conf[28] = {0xff, 0xff, 0x00, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0, 0, 0, 0, 0, 0, 0, 0xff, 0, 0, 0};
// static uint8_t gpio_conf[28] = {0xff, 0xff, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0xff, 0, 0, 0, 0, 0, 0, 0, 0xff, 0, 0, 0};

interface_common *interface_common::interface_common_ = nullptr;
interface_common *interface_common::get_instance(void)
{
    if (nullptr == interface_common_)
    {
        interface_common_ = new interface_common();
        interface_common_->init();
    }

    return interface_common_;
}

void interface_common::init(void)
{
    nvs_storage *nvs_store = nvs_storage::get_instance();
#if (1 == USE_RTOS_QUEUE)
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
#else
    gpio_smphr = xSemaphoreCreateBinary();
#endif

    uint8_t gpio_conf_read[28];
    memset(gpio_conf_read, 0, sizeof(gpio_conf_read));

    esp_err_t err = nvs_store->read_gpio_config(gpio_conf_read, sizeof(gpio_conf_read));

    TRACE_D("'Gpio_config' nvs read error: %s", esp_err_to_name(err));
    dump(gpio_conf_read, 0, sizeof(gpio_conf_read));

    if (ESP_OK == err)
    {
        for (int idx = 0; idx < 28; idx++)
        {
            if (gpio_conf[idx] != 0xff)
            {
                gpio_conf[idx] = gpio_conf_read[idx];
            }
        }
    }

    gpio_config_sets(0xff);
}

uint32_t interface_common::get_gpio_isr_event(uint32_t wait_ms)
{
    uint32_t gpio_num = UINT32_MAX;
#if (1 == USE_RTOS_QUEUE)
    if (gpio_evt_queue)
    {
        xQueueReceive(gpio_evt_queue, (void *)&gpio_num, wait_ms);
    }
#else
    if (gpio_smphr)
    {
        xSemaphoreTake(gpio_smphr, wait_ms);
    }
#endif
    return gpio_num;
}

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_n = (uint32_t)arg;
#if (1 == USE_RTOS_QUEUE)
    if (gpio_evt_queue)
    {
        xQueueSendFromISR(gpio_evt_queue, &gpio_n, NULL);
    }
#else
    if (gpio_smphr)
    {
        xSemaphoreGiveFromISR(gpio_smphr, NULL);
    }
#endif
}

void interface_common::set_config(uint8_t val, int idx)
{
    if (idx < sizeof(gpio_conf))
    {
        gpio_conf[idx] = val;
    }
    else
    {
        TRACE_D("Erorr: gpio_conf out of index!\n");
    }
}

uint8_t interface_common::get_config(int idx)
{
    return gpio_conf[idx];
}

void interface_common::inst_out_button(gpio_num_t gpioPin, uint8_t state, gpio_pull_mode_t pull)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << gpioPin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&io_conf);
    gpio_set_level(gpioPin, state);
}

void interface_common::inst_input_button(gpio_num_t gpioPin, gpio_pull_mode_t pull)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << gpioPin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = (pull == GPIO_PULLUP_ONLY) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pull_down_en = (pull == GPIO_PULLUP_ONLY) ? GPIO_PULLDOWN_DISABLE : GPIO_PULLDOWN_ENABLE,
        // .intr_type = GPIO_INTR_ANYEDGE,
        .intr_type = (pull == GPIO_PULLUP_ONLY) ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE,
    };

    gpio_config(&io_conf);
    gpio_intr_enable(gpioPin);
    gpio_isr_handler_add(gpioPin, gpio_isr_handler, (void *)gpioPin);
}

void interface_common::gpio_config_sets(uint32_t n)
{
    uint32_t a = 0, b = 28;
    if (n < 28)
    {
        a = n;
        b = n + 1;
    }
    else if (0xff != n)
    {
        return;
    }

    for (uint32_t i = a; i < b; i++)
    {
        if (gpio_conf[i] != 0xff)
        {
            uint8_t gg_b = gpio_conf[i] & 3;
            gpio_reset_pin((gpio_num_t)i);
            if (gg_b == 1 || gg_b == 2)
            {
                gpio_pull_mode_t pull = GPIO_PULLUP_ONLY;
                if (gpio_conf[i] & 4)
                {
                    pull = GPIO_PULLDOWN_ONLY;
                }
                if (gpio_conf[i] & 1)
                {
                    inst_input_button((gpio_num_t)i, pull);
                }
                else
                {
                    uint8_t st = 0;
                    if (gpio_conf[i] & 8)
                    {
                        st = 0xff;
                    }
                    inst_out_button((gpio_num_t)i, st, pull);
                }
                vTaskDelay(20 / portTICK_RATE_MS);
                gpio_state[i] = (uint8_t)gpio_get_level((gpio_num_t)i) & 0xff;
            }
        }
    }
}

void interface_common::gpio_state_set(uint32_t pin, uint32_t state)
{
    TRACE_W("gpio_conf[%d]: %d - state: %d", pin, gpio_conf[pin], state);
    if ((pin < sizeof(gpio_conf)) && (0xff != gpio_conf[pin]) && (0 == (0x01 & gpio_conf[pin])))
    {
        TRACE_W("GPIO_NUM_%d -> level: %d", pin, state);
        gpio_set_level((gpio_num_t)pin, state);
        gpio_state[pin] = (uint8_t)(state & 0xff);
    }
}

uint32_t interface_common::gpio_state_get(uint32_t pin)
{
    uint32_t ret = 0;
    if ((pin < sizeof(gpio_conf)) && (0xff != gpio_conf[pin]) && (0x01 & gpio_conf[pin]))
    {
        gpio_state[pin] = (uint8_t)gpio_get_level((gpio_num_t)pin) & 0xff;
    }

    ret = gpio_state[pin] & 0xFF;

    return ret;
}
