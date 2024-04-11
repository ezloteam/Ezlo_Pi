
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "esp_err.h"
#include "esp_check.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_info.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_hal_uart.h"

static void ezlopi_uart_channel_task(void* args);
static ezlo_uart_channel_t get_available_channel();

s_ezlopi_uart_object_handle_t ezlopi_uart_init(uint32_t baudrate, uint32_t tx, uint32_t rx, __uart_upcall upcall, void* arg)
{
    static QueueHandle_t ezlo_uart_channel_queue;
    s_ezlopi_uart_object_handle_t uart_object_handle = (struct s_ezlopi_uart_object*)malloc(sizeof(struct s_ezlopi_uart_object));
    memset(uart_object_handle, 0, sizeof(struct s_ezlopi_uart_object));
    uart_object_handle->arg = arg;
    ezlo_uart_channel_t channel = get_available_channel();
    if (NULL == upcall)
    {
        TRACE_E("NULL upcall found.");
    }
    else if (-1 != channel)
    {
        uart_config_t uart_config = {
            .baud_rate = baudrate,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .rx_flow_ctrl_thresh = 0,
            .source_clk = UART_SCLK_APB,
        };

        if (EZLOPI_UART_CHANNEL_1 == channel)
        {
            ESP_ERROR_CHECK(uart_driver_install(channel, 256, 256, 32, &ezlo_uart_channel_queue, 0));
            uart_object_handle->ezlopi_uart_queue_handle = ezlo_uart_channel_queue;
        }
#ifdef EZLOPI_UART_CHANNEL_2
        else if (EZLOPI_UART_CHANNEL_2 == channel)
        {
            ESP_ERROR_CHECK(uart_driver_install(channel, 256, 256, 32, &ezlo_uart_channel_queue, 0));
            uart_object_handle->ezlopi_uart_queue_handle = ezlo_uart_channel_queue;
        }
#endif
        ESP_ERROR_CHECK(uart_param_config(channel, &uart_config));
        ESP_ERROR_CHECK(uart_set_pin(channel, tx, rx, -1, -1));
        TRACE_S("UART configured successfully.");
        uart_object_handle->ezlopi_uart.channel = channel;
        uart_object_handle->ezlopi_uart.baudrate = baudrate;
        uart_object_handle->ezlopi_uart.tx = tx;
        uart_object_handle->ezlopi_uart.rx = rx;
        uart_object_handle->ezlopi_uart.enable = true;
        uart_object_handle->upcall = upcall;

        xTaskCreate(ezlopi_uart_channel_task, "ezlopi_uart_channel_task", 2048 * 2, (void*)uart_object_handle, 13, &(uart_object_handle->taskHandle));
    }
    else
    {
        TRACE_E("All channels are busy.");
    }

    return uart_object_handle;
}

static ezlo_uart_channel_t get_available_channel()
{
    TRACE_E("EZLOPI_UART_CHANNEL_MAX is : %d", EZLOPI_UART_CHANNEL_MAX);
    for (ezlo_uart_channel_t channel = EZLOPI_UART_CHANNEL_1; channel < EZLOPI_UART_CHANNEL_MAX; channel++)
    {
        if (!uart_is_driver_installed(channel))
        {
            return channel;
        }
    }
    return -1;
}

static void ezlopi_uart_channel_task(void* args)
{
    uart_event_t event;
    uint8_t* buffer = NULL;

    // s_ezlopi_uart_object_t *ezlopi_uart_object = (s_ezlopi_uart_object_t*)args;
    s_ezlopi_uart_object_handle_t ezlopi_uart_object = (s_ezlopi_uart_object_handle_t)args;

    while (1)
    {
        int data_len = 0;
        // Start reveceiving UART events for first channel.
        if (xQueueReceive(ezlopi_uart_object->ezlopi_uart_queue_handle, (void*)&event, portMAX_DELAY))
        {
            switch (event.type)
            {
            case UART_DATA:
            {
                uint32_t bufferred_data_len = 0;
                uart_get_buffered_data_len(ezlopi_uart_object->ezlopi_uart.channel, &bufferred_data_len);
                if (bufferred_data_len)
                {
                    buffer = malloc(bufferred_data_len);
                    if (buffer)
                    {
                        data_len = uart_read_bytes(ezlopi_uart_object->ezlopi_uart.channel, buffer, event.size, 100 / portTICK_PERIOD_MS);
                    }
                    else
                    {
                        uart_flush_input(ezlopi_uart_object->ezlopi_uart.channel);
                    }
                }
                break;
            }
            case UART_BREAK:
            case UART_BUFFER_FULL:
            case UART_FIFO_OVF:
            {
                TRACE_E("UART_FIFO_. event -> %d", event.type);
                uart_flush_input(ezlopi_uart_object->ezlopi_uart.channel);
                break;
            }
            default:
            {
                TRACE_E("Default event encountered. %d", event.type);
                break;
            }
            }
        }
        ezlopi_uart_object->upcall(buffer, data_len, ezlopi_uart_object);
        if (buffer)
        {
            free(buffer);
            buffer = NULL;
        }
        data_len = 0;
    }
}

ezlo_uart_channel_t ezlopi_uart_get_channel(s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle)
{
    return ezlopi_uart_object_handle->ezlopi_uart.channel;
}

void EZPI_HAL_uart_init(void)
{
    uint32_t baud = EZPI_SERV_UART_BAUD_DEFAULT;
    uint32_t parity_val = EZPI_SERV_UART_PARITY_DEFAULT;
    uint32_t start_bits = EZPI_SERV_UART_START_BIT_DEFAULT;
    uint32_t stop_bits = EZPI_SERV_UART_STOP_BIT_DEFAULT;
    uint32_t frame_size = EZPI_SERV_UART_FRAME_SIZE_DEFAULT;
    uint32_t flow_control = EZPI_SERV_UART_FLOW_CTRL_DEFAULT;

    if (EZPI_CORE_nvs_read_baud(&baud))
    {
        EZPI_CORE_nvs_write_baud(EZPI_SERV_UART_BAUD_DEFAULT);
    }
    vTaskDelay(1 / portTICK_RATE_MS);
    if (EZPI_CORE_nvs_read_parity(&parity_val))
    {
        EZPI_CORE_nvs_write_parity(EZPI_SERV_UART_PARITY_DEFAULT);
    }
    vTaskDelay(1 / portTICK_RATE_MS);
    if (EZPI_CORE_nvs_read_start_bits(&start_bits))
    {
        EZPI_CORE_nvs_write_start_bits(EZPI_SERV_UART_START_BIT_DEFAULT);
    }
    vTaskDelay(1 / portTICK_RATE_MS);
    if (EZPI_CORE_nvs_read_stop_bits(&stop_bits))
    {
        EZPI_CORE_nvs_write_stop_bits(EZPI_SERV_UART_STOP_BIT_DEFAULT);
    }
    vTaskDelay(1 / portTICK_RATE_MS);
    if (EZPI_CORE_nvs_read_frame_size(&frame_size))
    {
        EZPI_CORE_nvs_write_frame_size(EZPI_SERV_UART_FRAME_SIZE_DEFAULT);
    }
    vTaskDelay(1 / portTICK_RATE_MS);
    if (EZPI_CORE_nvs_read_flow_control(&flow_control))
    {
        EZPI_CORE_nvs_write_flow_control(EZPI_SERV_UART_FLOW_CTRL_DEFAULT);
    }
    vTaskDelay(1 / portTICK_RATE_MS);
    uart_word_length_t frame_size_val = EZPI_CORE_info_get_frame_size(frame_size);

    char flw_ctrl_bffr[EZPI_UART_SERV_FLW_CTRL_STR_SIZE + 1];
    EZPI_CORE_info_get_flow_ctrl_to_name(flow_control, flw_ctrl_bffr);

    TRACE_I("Serial Configuration:");
    TRACE_I("  Baud Rate: %d", baud);
    TRACE_I("  Parity: %d", parity_val);
    TRACE_I("  Start Bits: %d", start_bits);
    TRACE_I("  Stop Bits: %d", stop_bits);
    TRACE_I("  Frame Size: %d", frame_size);
    TRACE_I("  Flow Control: %s", flw_ctrl_bffr);

    vTaskDelay(10 / portTICK_RATE_MS);

    const uart_config_t uart_config = {
        .baud_rate = baud,
        .data_bits = frame_size_val,
        .parity = (uart_parity_t)parity_val,
        .stop_bits = (uart_stop_bits_t)stop_bits,
        .flow_ctrl = (uart_hw_flowcontrol_t)flow_control,
        .source_clk = UART_SCLK_APB,
    };

    // Uninstall previous initialized drivers 
    // uart_driver_delete(EZPI_SERV_UART_NUM_DEFAULT);
    // vTaskDelay(10);
    // We won't use a buffer for sending data.
    uart_driver_install(EZPI_SERV_UART_NUM_DEFAULT, EZPI_SERV_UART_RX_BUFFER_SIZE * 2, 0, 0, NULL, 0);
    vTaskDelay(10);
    uart_param_config(EZPI_SERV_UART_NUM_DEFAULT, &uart_config);
    uart_set_pin(EZPI_SERV_UART_NUM_DEFAULT, EZPI_SERV_UART_TXD_PIN, EZPI_SERV_UART_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}