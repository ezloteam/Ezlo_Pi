#ifndef __EZLOPI_DEVICES_H__
#define __EZLOPI_DEVICES_H__

#include "ezlopi_gpio.h"
#include "ezlopi_uart.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_spi_master.h"
#include "ezlopi_onewire.h"

typedef enum e_ezlopi_device_interface_type
{
    EZLOPI_DEVICE_INTERFACE_NONE = 0,
    EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT = 1,
    EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT = 2,
    EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT = 3,
    EZLOPI_DEVICE_INTERFACE_ANALOG_OUTPUT = 4,
    EZLOPI_DEVICE_INTERFACE_PWM = 5,
    EZLOPI_DEVICE_INTERFACE_UART = 6,
    EZLOPI_DEVICE_INTERFACE_ONEWIRE_MASTER = 7,
    EZLOPI_DEVICE_INTERFACE_I2C_MASTER = 8,
    EZLOPI_DEVICE_INTERFACE_SPI_MASTER = 9,
    EZLOPI_DEVICE_INTERFACE_MAX
} e_ezlopi_device_interface_type_t;

typedef enum e_ezlopi_item_type
{
    EZLOPI_ITEM_NONE = 0,
    EZLOPI_ITEM_LED = 1,
    EZLOPI_ITEM_RELAY = 2,
    EZLOPI_ITEM_PLUG = 3,
    EZLOPI_ITEM_SPK = 4, // SPK -> [S: Switch][P: Push Buttom][K: Key]
    EZLOPI_ITEM_MPU6050 =5,
    EZLOPI_ITEM_ADXL345 = 6,
    EZLOPI_ITEM_
} e_ezlopi_item_type_t;

typedef struct e_ezlopi_devices
{
    e_ezlopi_device_interface_type_t interface;
    // hardware interface
    union
    {
        s_ezlopi_uart_t uart;
        s_ezlopi_i2c_master_t i2c_master;
        s_ezlopi_spi_master_t spi_master;
        s_ezlopi_onewire_t onewire_master;
        s_ezlopi_gpios_t gpio;
    } comm;

    // cloud information
    char name[20]; // device name
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
