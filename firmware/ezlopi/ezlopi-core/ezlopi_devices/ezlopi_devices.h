#ifndef __EZLOPI_DEVICES_H__
#define __EZLOPI_DEVICES_H__

#include "ezlopi_i2c_master.h"
#include "ezlopi_spi_master.h"
#include "ezlopi_uart.h"

typedef struct e_ezlopi_devices
{
    union comm
    {
        s_ezlopi_i2c_master_t i2c;
        s_ezlopi_spi_master_t spi;
        s_ezlopi_spi_master_t uart;
    };

} e_ezlopi_devices_t;

#endif // __EZLOPI_DEVICE_H__
