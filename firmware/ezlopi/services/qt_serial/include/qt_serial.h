#ifndef __QT_SERIAL_H__
#define __QT_SERIAL_H__

#include <string>
#include <cstring>
#include <iostream>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

class qt_serial
{
private:

protected:
    qt_serial() {}
    static qt_serial *qt_serial_;

    static void qt_serial_read_process(void *pv);
    static void pars(int len, uint8_t *data);
    static void pars_command(int len, uint8_t *data);

public:
    int init(void);
    static qt_serial *get_instance(void);

    int is_new_wifi(void);
    static int respond_to_qt(int len, uint8_t *data);

    qt_serial(qt_serial &other) = delete;
    void operator=(const qt_serial &) = delete;
};

#endif // __QT_SERIAL_H__