#ifndef __QT_SERIAL_H__
#define __QT_SERIAL_H__

#include <string.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#ifdef __cplusplus
extern "C"
{
#endif

    void qt_serial_init(void);
    int qt_serial_tx_data(int len, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif // __QT_SERIAL_H__