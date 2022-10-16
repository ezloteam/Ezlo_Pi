#ifndef __EZLOPI_UART_H__
#define __EZLOPI_UART_H__

typedef struct s_ezlopi_uart
{
    bool enable;
    uint32_t tx;
    uint32_t rx;
    uint32_t channel;
    uint32_t baudrate;
} s_ezlopi_uart_t;

#endif // __EZLOPI_UART_H__
