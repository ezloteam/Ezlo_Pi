#ifndef _EZLOPI_SERVICE_UART_H_
#define _EZLOPI_SERVICE_UART_H_

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "EZLOPI_USER_CONFIG.h"


#ifdef __cplusplus
extern "C"
{
#endif

#if defined(CONFIG_EZPI_ENABLE_UART_PROVISIONING)

    // typedef enum e_ezlopi_item_type
    // {
    //     EZLOPI_ITEM_NONE = 0,
    //     EZLOPI_ITEM_LED = 1,
    //     EZLOPI_ITEM_RELAY = 2,
    //     EZLOPI_ITEM_PLUG = 3,
    //     EZLOPI_ITEM_SPK = 4, // SPK -> [S: Switch][P: Push Buttom][K: Key]
    //     EZLOPI_ITEM_MPU6050 =5,
    //     EZLOPI_ITEM_ADXL345 = 6,
    //     EZLOPI_ITEM_
    // } e_ezlopi_item_type_t;
#define EZLOPI_WIFI_MIN_PASS_CHAR 8
#define EZLOPI_WIFI_CONN_RETRY_ATTEMPT 2
#define EZLOPI_WIFI_CONN_ATTEMPT_INTERVAL 5000

    void EZPI_SERVICE_uart_init(void);
    int EZPI_SERVICE_uart_tx_data(int len, uint8_t* data);

#endif

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_SERVICE_UART_H_