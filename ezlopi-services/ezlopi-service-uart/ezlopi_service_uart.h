#ifndef _EZLOPI_SERVICE_UART_H_
#define _EZLOPI_SERVICE_UART_H_

#include "../../build/config/sdkconfig.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "EZLOPI_USER_CONFIG.h"

#ifdef __cplusplus
extern "C"
{
#endif

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

    typedef enum e_ezlopi_uart_cmd
    {
        EZPI_UART_CMD_RESET = 0,
        EZPI_UART_CMD_INFO,
        EZPI_UART_CMD_WIFI,
        EZPI_UART_CMD_SET_CONFIG,
        EZPI_UART_CMD_GET_CONFIG,
        EZPI_UART_CMD_UART_CONFIG,
        EZPI_UART_CMD_LOG_CONFIG,
        EZPI_UART_CMD_SET_PROV,
        EZPI_UART_CMD_MAX
    } e_ezlopi_uart_cmd_t;

    typedef enum e_ezlopi_uart_cmd_status
    {
        EZPI_UART_CMD_STATUS_FAIL = 0,
        EZPI_UART_CMD_STATUS_SUCCESS,
        EZPI_UART_CMD_STATUS_MAX
    } e_ezlopi_uart_cmd_status_t;

    void EZPI_SERV_uart_init(void);

#ifndef CONFIG_IDF_TARGET_ESP32
    void EZPI_SERV_cdc_init();
#endif // NOT defined CONFIG_IDF_TARGET_ESP32
    int EZPI_SERV_uart_tx_data(int len, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_SERVICE_UART_H_