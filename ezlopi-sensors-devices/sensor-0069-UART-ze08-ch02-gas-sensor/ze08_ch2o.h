
#ifndef _ZE08_CH2O_H_
#define _ZE08_CH2O_H_

#include <stddef.h>
#include "driver/gpio.h"
#include "ezlopi_hal_uart.h"

#define ZE08_BUFFER_MAXLENGTH     9
#define ZE08_CH2O_RX_PIN          GPIO_NUM_45
#define ZE08_CH2O_TX_PIN          GPIO_NUM_48
#define ZE08_CH2O_UART_BUAD_RATE  9600
#define DEBUG_ZE08_CH2O           1

typedef struct ze08_ch2o_sensor_data
{
    bool available;
    float ppm;
} ze08_ch2o_sensor_data_t;


/**
 * @brief Intitializes ZE08-CH2O sensor
 * @param ze08_uart_config
 * @param data
 * @return
 *      - true: sensor initialized.
 *      - false: error.
 */
bool ze08_ch2o_sensor_init(s_ezlopi_uart_t *ze08_uart_config, ze08_ch2o_sensor_data_t *data);

#endif // _ZE08_CH2O_H_
