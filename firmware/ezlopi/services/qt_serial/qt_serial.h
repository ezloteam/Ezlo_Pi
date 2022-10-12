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
    int qt_serial_respond_to_qt(int len, uint8_t *data);
    //--------------- Changes
    void QT_GET_INFO();
    void QT_SET_WIFI(const char *data);
    void QT_RESPONE(uint8_t cmd, uint8_t status_write, uint8_t status_connect);
    void QT_SET_DATA(const char *data);
    void QT_READ_DATA(void);
#ifdef __cplusplus
}
#endif

#endif // __QT_SERIAL_H__