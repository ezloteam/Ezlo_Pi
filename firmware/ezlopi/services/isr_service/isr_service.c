

#include "trace.h"
#include "isr_service.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


static xQueueHandle isr_event_queue = NULL;
static const uint32_t switch_debounce_time = 1000U;

static void isr_service_task(void *args);
static void isr_service_setup(s_ezlopi_device_properties_t *properties, f_interrupt_upcall_t upcall);

void isr_service_init(s_ezlopi_device_properties_t *properties, f_interrupt_upcall_t upcall)
{
    static bool isr_service_started = false;

    if(false == isr_service_started)
    {
        TRACE_I("Initializing ISR service.");
        gpio_install_isr_service(ESP_INTR_FLAG_EDGE);
        TRACE_I("ISR service installed.");
        isr_event_queue = xQueueCreate(20, sizeof(s_isr_event_args_t));
        xTaskCreate(isr_service_task, "isr_service_task", 2*2048, NULL, 3, NULL);
        isr_service_started = true;
    }

    isr_service_setup(properties, upcall);
}

static void IRAM_ATTR _isr_service_handler(void *args)
{
    void *temp_arg = args;
    if(isr_event_queue)
    {
        xQueueSendFromISR(isr_event_queue, &temp_arg, NULL);
    }
}

static void isr_service_setup(s_ezlopi_device_properties_t *properties, f_interrupt_upcall_t upcall)
{
    s_isr_event_args_t *event_args = (s_isr_event_args_t*)malloc(sizeof(s_isr_event_args_t));
    if(event_args)
    {
        event_args->isr_timer = 0;
        event_args->properties = properties;
        event_args->upcall = upcall;
        gpio_intr_enable(properties->interface.gpio.gpio_in.gpio_num);
        TRACE_E("GPIO interrupt enabled.");
        gpio_isr_handler_add(event_args->properties->interface.gpio.gpio_in.gpio_num, _isr_service_handler, (void*)event_args);
    }
}

static void isr_service_task(void *args)
{

    while(1)
    {
        s_isr_event_args_t *event_args = NULL;
        xQueueReceive(isr_event_queue, &event_args, portMAX_DELAY);
        if(NULL != event_args)
        {
            TickType_t tick_now = xTaskGetTickCount();
            if((tick_now - event_args->isr_timer) > (switch_debounce_time / portTICK_RATE_MS))
            {
                TRACE_I("********ISR called for GPIO_%d", event_args->properties->interface.gpio.gpio_in.gpio_num);
                event_args->upcall(event_args->properties);
            }
        }
    }
}





