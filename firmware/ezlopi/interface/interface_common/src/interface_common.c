#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "debug.h"
#include "nvs_storage.h"
#include "interface_common.h"
#include "devices_common.h"

#define USE_RTOS_QUEUE 1

#if (1 == USE_RTOS_QUEUE)
static xQueueHandle gpio_evt_queue;
#else
static SemaphoreHandle_t gpio_smphr;
#endif

static uint8_t gpio_output_state[GPIO_NUM_MAX] = {0}; // value of pin

void interface_common_init_v2(void)
{
    memset(gpio_output_state, 0, sizeof(gpio_output_state));
    s_device_properties_t *device_list = devices_common_device_list();

    for (int idx = 0; idx < MAX_DEV; idx++)
    {
        if ((EZPI_DEV_TYPE_DIGITAL_OP == device_list[idx].dev_type) ||
            (EZPI_DEV_TYPE_DIGITAL_IP == device_list[idx].dev_type))
        {
            if ((device_list[idx].input_gpio >= 0) && (device_list[idx].input_gpio < GPIO_NUM_MAX))
            {
                gpio_reset_pin(device_list[idx].input_gpio);

                if (EZPI_DEV_TYPE_ONE_WIRE == device_list[idx].dev_type)
                {
                    TRACE_B("Onewire gpio - Num: %d, pullup: %d, Val: %d, Inv: %d",
                            device_list[idx].input_gpio, device_list[idx].input_pullup,
                            device_list[idx].input_vol, device_list[idx].input_inv);

                    gpio_pad_select_gpio(device_list[idx].input_gpio);
                }
                else
                {
                    TRACE_B("Input gpio - Num: %d, pullup: %d, Val: %d, Inv: %d",
                            device_list[idx].input_gpio, device_list[idx].input_pullup,
                            device_list[idx].input_vol, device_list[idx].input_inv);

                    interface_common_inst_input_button((gpio_num_t)device_list[idx].input_gpio, device_list[idx].input_pullup);
                }
            }

            if ((device_list[idx].out_gpio >= 0) && (device_list[idx].out_gpio < GPIO_NUM_MAX))
            {
                TRACE_B("Output gpio - Num: %d, pullup: %d, Val: %d, Inv: %d",
                        device_list[idx].out_gpio, device_list[idx].output_pullup,
                        device_list[idx].out_vol, device_list[idx].out_inv);

                uint8_t state = (device_list[idx].out_inv ? (device_list[idx].out_vol ? 0 : 1) : device_list[idx].out_vol);
                gpio_reset_pin((gpio_num_t)device_list[idx].out_gpio);
                interface_common_inst_out_button((gpio_num_t)device_list[idx].out_gpio, state, device_list[idx].output_pullup);
            }
        }
    }
}

void interface_common_init(void)
{
#if (1 == USE_RTOS_QUEUE)
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
#else
    gpio_smphr = xSemaphoreCreateBinary();
#endif

    interface_common_init_v2();
}

uint32_t interface_get_message_count_in_queue(void)
{
    if (gpio_evt_queue)
        return uxQueueMessagesWaiting(gpio_evt_queue);
    else
        return 0;
}

uint32_t interface_common_get_gpio_isr_event(uint32_t wait_ms)
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

void interface_common_inst_out_button(gpio_num_t gpioPin, uint8_t state, gpio_pull_mode_t pull)
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

void interface_common_inst_input_button(gpio_num_t gpioPin, gpio_pull_mode_t pull)
{
    if (gpioPin < GPIO_NUM_MAX)
    {
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << gpioPin),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = (pull == 1) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
            .pull_down_en = (pull == 1) ? GPIO_PULLDOWN_DISABLE : GPIO_PULLDOWN_ENABLE,
            // .intr_type = GPIO_INTR_ANYEDGE,
            .intr_type = (pull == 1) ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE,
        };

        gpio_config(&io_conf);
        gpio_intr_enable(gpioPin);
        gpio_isr_handler_add(gpioPin, gpio_isr_handler, (void *)gpioPin);
    }
}

void interface_common_gpio_state_set(uint32_t pin, uint32_t state)
{
    TRACE_W("GPIO_PIN_%d:: cur_state: %d, update-to state: %d, ", pin, gpio_output_state[pin], state);
    if (pin < GPIO_NUM_MAX)
    {
        TRACE_W("GPIO_NUM_%d -> level: %d", pin, state);
        gpio_set_level((gpio_num_t)pin, state);
        gpio_output_state[pin] = (uint8_t)(state & 0xff);
    }
}

uint32_t interface_common_gpio_get_output_state(uint32_t pin)
{
    uint32_t ret = 0xff;

    if (pin < GPIO_NUM_MAX)
    {
        ret = gpio_output_state[pin];
    }

    return ret;
}

uint32_t interface_common_gpio_get_input_state(uint32_t pin)
{
    uint32_t ret = 0;
    if (pin < GPIO_NUM_MAX)
    {
        gpio_output_state[pin] = (uint8_t)(gpio_get_level((gpio_num_t)pin) & 0xff);
    }

    ret = gpio_output_state[pin] & 0xFF;
    return ret;
}
