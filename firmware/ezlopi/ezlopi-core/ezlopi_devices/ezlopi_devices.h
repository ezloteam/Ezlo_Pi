#ifndef __EZLOPI_DEVICES_H__
#define __EZLOPI_DEVICES_H__

#include "ezlopi_gpio.h"
#include "ezlopi_uart.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_spi_master.h"
#include "ezlopi_onewire.h"
#include "ezlopi_cloud.h"

#define CJSON_GET_VALUE_INT(root, item_name, item_val)        \
    {                                                         \
        cJSON *o_item = cJSON_GetObjectItem(root, item_name); \
        if (o_item)                                           \
        {                                                     \
            item_val = o_item->valueint;                      \
        }                                                     \
    }

#define CJSON_GET_VALUE_STRING(root, item_name, item_val)     \
    {                                                         \
        cJSON *o_item = cJSON_GetObjectItem(root, item_name); \
        if (o_item)                                           \
        {                                                     \
            item_val = o_item->valuestring;                   \
        }                                                     \
    }

#define ASSIGN_DEVICE_NAME(digital_io_device_properties, device_name)                             \
    {                                                                                             \
        if ((NULL != device_name) && ('\0' != device_name[0]))                                    \
        {                                                                                         \
            snprintf(digital_io_device_properties->ezlopi_cloud.device_name,                      \
                     sizeof(digital_io_device_properties->ezlopi_cloud.device_name),              \
                     "%s", device_name);                                                          \
        }                                                                                         \
        else                                                                                      \
        {                                                                                         \
            snprintf(digital_io_device_properties->ezlopi_cloud.device_name,                      \
                     sizeof(digital_io_device_properties->ezlopi_cloud.device_name),              \
                     "dev-%d:digital_out", digital_io_device_properties->ezlopi_cloud.device_id); \
        }                                                                                         \
    }

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

typedef struct s_ezlopi_device_properties
{
    e_ezlopi_device_interface_type_t interface_type;
    uint16_t device_subtype; // id_item : from qt-app or ezlogic app
    // hardware interface
    union
    {
        s_ezlopi_uart_t uart;
        s_ezlopi_i2c_master_t i2c_master;
        s_ezlopi_spi_master_t spi_master;
        s_ezlopi_onewire_t onewire_master;
        s_ezlopi_gpios_t gpio;
    } interface;

    s_ezlopi_cloud_info_t ezlopi_cloud;

} s_ezlopi_device_properties_t;

void ezlopi_device_init(void);
uint16_t ezlopi_device_generate_device_id(void);
uint16_t ezlopi_device_generate_item_id(void);
uint16_t ezlopi_device_generate_room_id(void);

void ezlopi_device_print_properties(s_ezlopi_device_properties_t *device);

#endif // __EZLOPI_DEVICE_H__
