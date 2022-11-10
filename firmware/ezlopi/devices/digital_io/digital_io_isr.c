#include <string.h>
#include "sdkconfig.h"

#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "trace.h"
#include "frozen.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"

static xQueueHandle gpio_evt_queue = NULL;
static const uint32_t switch_debounce_time = 1000;

typedef struct s_event_arg
{
    TickType_t time;
    s_ezlopi_device_properties_t *properties;
} s_event_arg_t;

static void digital_io_isr_service(void *pv);
static void digital_io_isr_setup_isr(s_ezlopi_device_properties_t *properties);

void digital_io_isr_service_init(s_ezlopi_device_properties_t *properties, void (*__upcall)(s_ezlopi_device_properties_t *properties))
{
    static bool service_started;

    if (false == service_started)
    {
        TRACE_I("Started gpio-isr service");
        gpio_install_isr_service(0);
        gpio_evt_queue = xQueueCreate(20, sizeof(s_ezlopi_device_properties_t *));
        xTaskCreate(digital_io_isr_service, "digital-io-isr-service", 2 * 2048, NULL, 3, NULL);
        service_started = true;
    }

    digital_io_isr_setup_isr(properties);
}

static void IRAM_ATTR __gpio_isr_handler(void *arg)
{
    void *tmp_arg = arg;
    if (gpio_evt_queue)
    {
        xQueueSendFromISR(gpio_evt_queue, &tmp_arg, NULL);
    }
}

static void digital_io_isr_setup_isr(s_ezlopi_device_properties_t *properties)
{
    s_event_arg_t *event_arg = malloc(sizeof(s_event_arg_t));

    if (event_arg)
    {
        event_arg->time = 0;
        event_arg->properties = properties;
        gpio_intr_enable(properties->interface.gpio.gpio_in.gpio_num);
        gpio_isr_handler_add(properties->interface.gpio.gpio_in.gpio_num, __gpio_isr_handler, (void *)event_arg);
    }
    else
    {
        gpio_reset_pin(properties->interface.gpio.gpio_in.gpio_num);
    }
}

static void digital_io_isr_service(void *pv)
{
    l_ezlopi_configured_devices_t *p_devices_list = ezlopi_devices_list_get_configured_items();

    while (1)
    {
        s_event_arg_t *event_arg = NULL;
        xQueueReceive(gpio_evt_queue, &event_arg, portMAX_DELAY);

        if (NULL != event_arg)
        {
            TickType_t tick_now = xTaskGetTickCount();

            if ((tick_now - event_arg->time) > (switch_debounce_time / portTICK_RATE_MS))
            {
                TRACE_I("****** ISR-event:: pin: %u", event_arg->properties->interface.gpio.gpio_in.gpio_num);
                event_arg->properties->interface.gpio.gpio_out.value = (EZLOPI_GPIO_LOW == event_arg->properties->interface.gpio.gpio_out.value) ? EZLOPI_GPIO_HIGH : EZLOPI_GPIO_LOW;
                gpio_set_level(event_arg->properties->interface.gpio.gpio_out.gpio_num, event_arg->properties->interface.gpio.gpio_out.value);
                ezlopi_device_value_updated_from_device(event_arg->properties);
                event_arg->time = tick_now;
            }
        }
    }
}
