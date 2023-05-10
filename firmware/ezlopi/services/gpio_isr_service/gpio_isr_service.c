#include <string.h>
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "trace.h"
#include "gpio_isr_service.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_devices_list.h"

static QueueHandle_t gpio_evt_queue = NULL;
static const uint32_t default_debounce_time = 1000;

typedef struct s_event_arg
{
    TickType_t time;
    TickType_t debounce_ms;
    s_ezlopi_device_properties_t *properties;
    f_interrupt_upcall_t __upcall;
} s_event_arg_t;

static void IRAM_ATTR __gpio_isr_handler(void *arg);
static void digital_io_isr_service(void *pv);

void gpio_isr_service_init(void)
{
    TRACE_I("Started gpio-isr service");
    gpio_evt_queue = xQueueCreate(20, sizeof(s_ezlopi_device_properties_t *));
    xTaskCreate(digital_io_isr_service, "digital-io-isr-service", 2 * 2048, NULL, 3, NULL);
}

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
            TRACE_I("Successfully added GPIO ISR handler.");
        }
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

static void digital_io_isr_service(void *pv)
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
