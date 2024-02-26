#include <string.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_service_gpioisr.h"

static QueueHandle_t gpio_evt_queue = NULL;
static const uint32_t default_debounce_time = 1000;

typedef struct s_event_arg
{
    TickType_t time;
    TickType_t debounce_ms;
    // s_ezlopi_device_properties_t *properties;
    l_ezlopi_item_t *item;
    f_interrupt_upcall_t __upcall;
} s_event_arg_t;

static void gpio_isr_process_v3(void *pv);
static void IRAM_ATTR __gpio_isr_handler(void *arg);

void gpio_isr_service_init(void)
{
    TRACE_S("Started gpio-isr service");
    gpio_evt_queue = xQueueCreate(20, sizeof(s_event_arg_t *));
    xTaskCreate(gpio_isr_process_v3, "gpio_isr_process_v3", 2 * 2048, NULL, 3, NULL);
    // xTaskCreate(gpio_isr_process, "digital-io-isr-service", 2 * 2048, NULL, 3, NULL);
}

void gpio_isr_service_register_v3(l_ezlopi_item_t *item, f_interrupt_upcall_t __upcall, TickType_t debounce_ms)
{
    s_event_arg_t *event_arg = malloc(sizeof(s_event_arg_t));

    if (event_arg)
    {
        event_arg->time = 0;
        // event_arg->properties = properties;
        event_arg->item = item;
        event_arg->__upcall = __upcall;
        event_arg->debounce_ms = (0 == debounce_ms) ? default_debounce_time : debounce_ms;
        gpio_intr_enable(item->interface.gpio.gpio_in.gpio_num);

        if (gpio_isr_handler_add(item->interface.gpio.gpio_in.gpio_num, __gpio_isr_handler, (void *)event_arg))
        {
            TRACE_E("Error while adding GPIO ISR handler.");
            gpio_reset_pin(item->interface.gpio.gpio_in.gpio_num);
        }
        else
        {
            TRACE_S("Successfully added GPIO ISR handler for pin: %d.", item->interface.gpio.gpio_in.gpio_num);
        }
    }
}

static void gpio_isr_process_v3(void *pv)
{
    while (1)
    {
        s_event_arg_t *event_arg = NULL;
        xQueueReceive(gpio_evt_queue, &event_arg, portMAX_DELAY);

        if (NULL != event_arg)
        {
            TickType_t tick_now = xTaskGetTickCount();

            if ((tick_now - event_arg->time) > (event_arg->debounce_ms / portTICK_RATE_MS))
            {
                event_arg->__upcall(event_arg->item);
                event_arg->time = tick_now;
            }
        }

        // vTaskDelay(1);
    }
}

static void IRAM_ATTR __gpio_isr_handler(void *arg)
{
    void *tmp_arg = arg;
    if (gpio_evt_queue)
    {
        xQueueSendFromISR(gpio_evt_queue, &tmp_arg, NULL);
    }
}

#if 0
void gpio_isr_service_register(s_ezlopi_device_properties_t *properties, f_interrupt_upcall_t __upcall, TickType_t debounce_ms)
{
    s_event_arg_t *event_arg = malloc(sizeof(s_event_arg_t));

    if (event_arg)
    {
        event_arg->time = 0;
        event_arg->properties = properties;
        event_arg->__upcall = __upcall;
        event_arg->debounce_ms = (0 == debounce_ms) ? default_debounce_time : debounce_ms;
        gpio_intr_enable(properties->interface.gpio.gpio_in.gpio_num);

        if (gpio_isr_handler_add(properties->interface.gpio.gpio_in.gpio_num, __gpio_isr_handler, (void *)event_arg))
        {
            TRACE_E("Error while adding GPIO ISR handler.");
            gpio_reset_pin(properties->interface.gpio.gpio_in.gpio_num);
        }
        else
        {
            TRACE_S("Successfully added GPIO ISR handler.");
        }
    }
}

static void gpio_isr_process(void *pv)
{
    while (1)
    {
        s_event_arg_t *event_arg = NULL;
        xQueueReceive(gpio_evt_queue, &event_arg, portMAX_DELAY);

        if (NULL != event_arg)
        {
            TickType_t tick_now = xTaskGetTickCount();

            if ((tick_now - event_arg->time) > (event_arg->debounce_ms / portTICK_RATE_MS))
            {
                event_arg->__upcall(event_arg->properties);
                event_arg->time = tick_now;
            }
        }

        vTaskDelay(1);
    }
}
#endif