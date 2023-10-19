#ifndef __EZLOPI_DEVICES_LIST_H__
#define __EZLOPI_DEVICES_LIST_H__

#include <stdint.h>
#include <stdbool.h>
#include <cJSON.h>
#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
#include "sdkconfig.h"

#define EZLOPI_SENSOR_NONE 0
#define EZLOPI_DEVICE_0001_DIGITALOUT_GENERIC 1
#define EZLOPI_DEVICE_0002_DIGITALOUT_RELAY 2
#define EZLOPI_DEVICE_0003_DIGTALOUT_PLUG 3
#define EZLOPI_DEVICE_0004_DIGITALIN_GENERIC 4
#define EZLOPI_SENSOR_0008_I2C_LTR303ALS 8
#define EZLOPI_DEVICE_0009_OTHER_RMT_SK6812 9

// #define EZLOPI_SENSOR_0010_BME680_I2C 10

#define EZLOPI_SENSOR_0012_I2C_BME280 12
#define EZLOPI_SENSOR_0016_ONEWIRE_DHT22 16
// #define EZLOPI_SENSOR_0017_POTENTIOMETER 17

// #if CONFIG_IDF_TARGET_ESP32
// #define EZLOPI_SENSOR_0018_DOOR 18
// #endif

#define EZLOPI_SENSOR_0019_DIGITALIN_PIR 19
// #define EZLOPI_SENSOR_0020_JOYSTICK_2_AXIS 20
#define EZLOPI_SENSOR_0021_UART_MB1013 21
#define EZLOPI_DEVICE_0022_PWM_DIMMABLE_BULB 22
#define EZLOPI_SENSOR_0023_DIGITALIN_TOUCH_SWITCH_TTP223B 23

// #if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
#define EZLOPI_SENSOR_0024_OTHER_HCSR04 24
// #endif

#define EZLOPI_SENSOR_0025_DIGITALIN_LDR 25
#define EZLOPI_SENSOR_0026_ADC_LDR 26
#define EZLOPI_SENSOR_0027_ADC_WATER_LEAK 27
// #define EZLOPI_SENSOR_0028_SOUND_SENSOR_SPI 28

#define EZLOPI_SENSOR_0029_I2C_GXHTC3 29
#define EZLOPI_SENSOR_0030_ONEWIRE_DS18B20 30
#define EZLOPI_SENSOR_0031_OTHER_JSNSR04T 31
#define EZLOPI_SENSOR_0032_ADC_SOILMOISTURE 32

#define EZLOPI_SENSOR_0033_ADC_TURBIDITY 33
#define EZLOPI_SENSOR_0034_DIGITALIN_PROXIMITY 34
#define EZLOPI_SENSOR_0035_DIGITALIN_TOUCH_SENSOR_TPP223B 35
#define EZLOPI_DEVICE_0038_OTHER_RGB 38

// #define EZLOPI_SENSOR_0029_GXHTC3_RH_T_I2C 35 

#define EZLOPI_SENSOR_0044_I2C_TSL2561_LUMINOSITY 44
#define EZLOPI_SENSOR_0048_OTHER_MQ4_CH4_DETECTOR 48
#define EZLOPI_SENSOR_0049_OTHER_MQ2_LPG_DETECTOR 49
#define EZLOPI_SENSOR_0050_OTHER_MQ3_ALCOHOL_DETECTOR 50
#define EZLOPI_SENSOR_0051_OTHER_MQ8_H2_DETECTOR 51
#define EZLOPI_SENSOR_0052_OTHER_MQ135_NH3_DETECTOR 52
#define EZLOPI_SENSOR_0053_UART_GYGPS6MV2 53
#define EZLOPI_SENSOR_0054_PWM_YFS201_FLOWMETER 54
#define EZLOPI_SENSOR_0055_ADC_FLEXRESISTOR 55
#define EZLOPI_SENSOR_0056_ADC_FSR 56
#define EZLOPI_SENSOR_0057_OTHER_FLAME_DETECTOR 57
#define EZLOPI_SENSOR_0059_OTHER_MQ6_LPG_DETECTOR 59
#define EZLOPI_SENSOR_0060_DIGITALIN_VIBRATION_DETECTOR 60
#define EZLOPI_SENSOR_0061_DIGITALIN_REED_SWITCH 61
#define EZLOPI_SENSOR_0062_OTHER_MQ7_CO_DETECTOR 62
#define EZLOPI_SENSOR_0063_OTHER_MQ9_LPG_FLAMEABLE_DETECTOR 63
#define EZLOPI_SENSOR_0065_DIGITALIN_FLOAT_SWITCH 65

/**
 * @brief defining the type of sensor call function
 *
 */
// int sensor_bme280(e_ezlopi_actions_t action, void *arg);
#if 0 // v2.x
typedef int (*f_sensor_call_t)(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);
#endif
typedef int (*f_sensor_call_v3_t)(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#if 0 // v2.x
typedef struct s_ezlopi_device
{
    uint32_t id;
    f_sensor_call_t func;
} s_ezlopi_device_t;
#endif

typedef struct s_ezlopi_device_v3
{
    uint32_t id;
    f_sensor_call_v3_t func;
} s_ezlopi_device_v3_t;

#if 0 // v2.x
typedef struct l_ezlopi_configured_devices
{
    void *user_arg;
    s_ezlopi_device_t *device;
    s_ezlopi_device_properties_t *properties;
    struct l_ezlopi_configured_devices *next;

} l_ezlopi_configured_devices_t;
#endif

typedef struct s_ezlopi_prep_arg
{
    cJSON *cjson_device;
    s_ezlopi_device_v3_t *device;
} s_ezlopi_prep_arg_t;

s_ezlopi_device_v3_t *ezlopi_devices_list_get_list_v3(void);

/**
 * @brief Provides the list of available sensors
 *
 * @return f_sensor_call_t*
 */
#if 0 // v2.x
s_ezlopi_device_t *ezlopi_devices_list_get_list(void);

// void ezlopi_devices_list_add(s_ezlopi_device_t *device);
// int ezlopi_devices_list_add(s_ezlopi_device_t *device);
// int ezlopi_devices_list_add(s_ezlopi_device_t *device, s_ezlopi_device_properties_t *properties);
int ezlopi_devices_list_add(s_ezlopi_device_t *device, s_ezlopi_device_properties_t *properties, void *user_arg);
l_ezlopi_configured_devices_t *ezlopi_devices_list_get_configured_items(void);
#endif

#if 0
/**
 * @brief 'ezlopi_sensor_get_next'
 *
 * @param current_sensor
 * @return s_sensors_list_t*
 */
s_sensors_list_t *ezlopi_sensor_get_next(s_sensors_list_t *current_sensor);
#endif

#endif // __EZLOPI_DEVICES_LIST_H__
