#ifndef _EZLOPI_SERVICE_UART_H_
#define _EZLOPI_SERVICE_UART_H_

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define EZPI_UART_SERV_FLW_CTRL_STR_SIZE 10

    void EZPI_SERV_uart_init(void);
    int EZPI_SERVICE_uart_tx_data(int len, uint8_t* data);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_SERVICE_UART_H_