#ifndef __EZLOPI_UART_H__
#define __EZLOPI_UART_H__

#include "stdbool.h"
#include "stdint.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef int ezlo_uart_channel_t;

typedef enum e_ezlopi_uart_channel
{
    EZLOPI_UART_CHANNEL_UNDEFINED = -1,
    EZLOPI_UART_CHANNEL_0 = UART_NUM_0,
    EZLOPI_UART_CHANNEL_1 = UART_NUM_1,
#if UART_NUM_MAX > 2
    EZLOPI_UART_CHANNEL_2 = UART_NUM_2,
#endif
#define EZLOPI_UART_CHANNEL_MAX UART_NUM_MAX
} e_ezlopi_uart_channel_t;

typedef struct s_ezlopi_uart_object *s_ezlopi_uart_object_handle_t;
typedef void (*__uart_upcall)(uint8_t *buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle);

typedef struct s_ezlopi_uart
{
    ezlo_uart_channel_t channel;
    uint32_t baudrate;
    uint32_t tx;
    uint32_t rx;
    bool enable;
} s_ezlopi_uart_t;

struct s_ezlopi_uart_object
{
    void *arg;
    s_ezlopi_uart_t ezlopi_uart;
    __uart_upcall upcall;
    QueueHandle_t ezlopi_uart_queue_handle;
    TaskHandle_t taskHandle;
};

s_ezlopi_uart_object_handle_t ezlopi_uart_init(uint32_t baudrate, uint32_t tx, uint32_t rx, __uart_upcall upcall, void *arg);
ezlo_uart_channel_t ezlopi_uart_get_channel(s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle);

#endif // __EZLOPI_UART_H__
