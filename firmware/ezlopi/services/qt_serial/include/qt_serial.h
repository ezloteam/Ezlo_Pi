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

    typedef enum s_command
    {
        GET_CONFIG = 0xa0,
        SET_CONFIG,
        SET_VOL,
        SET_WiFi,
        FIRST_DEV,
        SET_DEV,
        GET_DEV,
        END_DEV,
    } s_command_t;

    int qt_serial_init(void);
    int qt_serial_respond_to_qt(int len, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif // __QT_SERIAL_H__