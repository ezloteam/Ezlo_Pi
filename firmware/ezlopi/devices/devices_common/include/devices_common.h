#ifndef __DEVICES_COMMON_H__
#define __DEVICES_COMMON_H__

#include <string.h>
#include "inttypes.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_DEV 10
#define DEVICE_TYPE_ID "ezlopi" // manufacturer deviceTypeId

#if 0
    typedef enum e_device_type
    {
        LED = 0,
        SWITCH,
        PLUG,
        TAMPER,
        MPU6050 = 5,
    } e_device_type_t;
#endif

    typedef enum e_dev_type
    {
        EZPI_DEV_TYPE_RESTRICTED,
        EZPI_DEV_TYPE_DIGITAL_OP,
        EZPI_DEV_TYPE_DIGITAL_IP,
        EZPI_DEV_TYPE_ANALOG_IP,
        EZPI_DEV_TYPE_ANALOG_OP,
        EZPI_DEV_TYPE_PWM,
        EZPI_DEV_TYPE_UART,
        EZPI_DEV_TYPE_ONE_WIRE,
        EZPI_DEV_TYPE_I2C,
        EZPI_DEV_TYPE_SPI,
        EZPI_DEV_TYPE_TOTAL,
        EZPI_DEV_TYPE_INPUT_ONLY = 253,
        EZPI_DEV_TYPE_OUTPUT_ONLY,
        EZPI_DEV_TYPE_UNCONFIGURED
    } e_dev_type_t;

    typedef enum e_item_type
    {
        EZPI_ITEM_TYPE_NONE,
        EZPI_ITEM_TYPE_LED,
        EZPI_ITEM_TYPE_RELAY,
        EZPI_ITEM_TYPE_PLUG,
        EZPI_ITEM_TYPE_BUTTON,
        EZPI_ITEM_TYPE_MPU6050,
        EZPI_ITEM_TYPE_ADXL345,
        EZPI_ITEM_TYPE_GY271,
        EZPI_ITEM_TYPE_MCP4725,
        EZPI_ITEM_TYPE_GY530,
        EZPI_ITEM_TYPE_DS1307,
        EZPI_ITEM_TYPE_MAX30100,
        EZPI_ITEM_TYPE_BMP280_I2C,
        EZPI_ITEM_TYPE_BMP280_SPI,
        EZPI_ITEM_TYPE_LNA219,
        EZPI_ITEM_TYPE_DHT11,
        EZPI_ITEM_TYPE_DHT22,
        EZPI_ITEM_TYPE_POT_GENERIC,
        EZPI_ITEM_TYPE_TOTAL
    } e_item_type_t;

    typedef struct s_device_properties
    {
        char device_id[9];
        uint8_t dev_type;
        char name[20]; // device Name
        char roomId[9];
        char roomName[20];
        char item_id[9]; // item id
        char item_name[20];
        bool input_vol;
        bool out_vol;
        uint8_t input_gpio;
        uint8_t out_gpio;
        bool input_pullup;
        bool output_pullup;
        bool is_input;
        bool input_inv;
        bool out_inv;
        bool is_meter;
        char category[32];
        char subcategory[32];
        char devicType[32];
        char value_type[16];
        bool has_getter;
        bool has_setter;
    } s_device_properties_t;

    void devices_common_init_devices(void);
    s_device_properties_t *devices_common_device_list(void);
    int devices_common_get_device_by_item_id(const char *item_id);
    const char *devices_common_get_device_type_id(void);

#ifdef __cplusplus
}
#endif

#endif // __DEVICES_COMMON_H__