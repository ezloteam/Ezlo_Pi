#ifndef __EZLOPI_DEVICES_H__
#define __EZLOPI_DEVICES_H__

#include "ezlopi_gpio.h"
#include "ezlopi_uart.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_spi_master.h"
#include "ezlopi_onewire.h"

typedef struct e_ezlopi_devices
{
    // hardware interface
    union comm
    {
        s_ezlopi_uart_t uart;
        s_ezlopi_i2c_master_t i2c;
        s_ezlopi_spi_master_t spi;
        s_ezlopi_onewire_t onewire;
        s_ezlopi_gpio_t gpio;
    };

    // cloud information
    char name[20]; // device name
    uint8_t dev_type;
    char device_id[9];

    char room_id[9];
    char room_name[20];

    char item_id[9];
    char item_name[20];

    char *category;
    char *subcategory;
    char *device_type;
    char *value_type;

    bool has_getter;
    bool has_setter;

} e_ezlopi_devices_t;

#endif // __EZLOPI_DEVICE_H__
