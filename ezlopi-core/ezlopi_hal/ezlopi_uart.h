#ifndef __EZLOPI_UART_H__
#define __EZLOPI_UART_H__

#include "stdbool.h"
#include "stdint.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


#define EZLOPI_UART_CHANNEL_1  1
#if UART_NUM_MAX > 2
#define EZLOPI_UART_CHANNEL_2  2
#endif
#define EZLOPI_UART_CHANNEL_MAX UART_NUM_MAX

typedef int ezlo_uart_channel_t;

typedef struct s_ezlopi_uart
{
    ezlo_uart_channel_t channel;
    uint32_t baudrate;
    uint32_t tx;
    uint32_t rx;
    bool enable;
} s_ezlopi_uart_t;


typedef struct s_ezlopi_uart_object *s_ezlopi_uart_object_handle_t;
typedef void (*__uart_upcall)(uint8_t* buffer, s_ezlopi_uart_object_handle_t uart_object_handle, void* user_args);


s_ezlopi_uart_object_handle_t ezlopi_uart_init(uint32_t baudrate, uint32_t tx, uint32_t rx, __uart_upcall upcall, void* args);
ezlo_uart_channel_t ezlopi_uart_get_channel(s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle);


#endif // __EZLOPI_UART_H__
