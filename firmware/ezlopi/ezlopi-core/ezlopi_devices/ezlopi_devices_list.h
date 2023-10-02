#ifndef __EZLOPI_DEVICES_LIST_H__
#define __EZLOPI_DEVICES_LIST_H__

#include <stdint.h>
#include <stdbool.h>
#include <cJSON.h>
#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
#include "sdkconfig.h"

#define EZLOPI_SENSOR_NONE 0
#define EZLOPI_SENSOR_0001_LED 1
// #define EZLOPI_SENSOR_0002_RELAY 2
// #define EZLOPI_SENSOR_0003_PLUG 3
#define EZLOPI_SENSOR_0008_ALS_LTR303_I2C_SENSOR 8
#define EZLOPI_DEVICE_0009_SK6812_LED_STRIP 9

#define EZLOPI_SENSOR_0010_BME680_I2C 10

#define EZLOPI_SENSOR_0012_BME280_I2C 12
#define EZLOPI_SENSOR_0016_DHT22_SENSOR 16
// #define EZLOPI_SENSOR_0017_POTENTIOMETER 17

// #if CONFIG_IDF_TARGET_ESP32
// #define EZLOPI_SENSOR_0018_DOOR 18
// #endif

#define EZLOPI_SENSOR_0019_PIR 19
// #define EZLOPI_SENSOR_0020_JOYSTICK_2_AXIS 20
// #define EZLOPI_SENSOR_0021_ULTRASONIC_HRLV_MAXSENSOR_EZ_MB1013 21
#define EZLOPI_SENSOR_0022_DIMMABLE_BULB 22
#define EZLOPI_SENSOR_0023_TTP_223B_TOUCH_SWITCH 23

// #if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
#define EZLOPI_SENSOR_0024_ULTRASONIC_HC_SR04_SENSOR 24
// #endif

#define EZLOPI_SENSOR_0025_LDR_DIGITAL_MODULE_SENSOR 25
#define EZLOPI_SENSOR_0026_LDR_ANALOGUE_SENSOR 26
#define EZLOPI_SENSOR_0027_WATER_LEAK 27
// #define EZLOPI_SENSOR_0028_SOUND_SENSOR_SPI 28

#define EZLOPI_SENSOR_029_IR_BLASTER 29
#define EZLOPI_SENSOR_030_DS18B20 30
// #define EZLOPI_SENSOR_031_JSN_SR04T_WaterLevelSensor 31
#define EZLOPI_SENSOR_032_SOIL_MOISTURE 32

#define EZLOPI_SENSOR_033_TURBIDITY_SENSOR 33
#define EZLOPI_SENSOR_034_PROXIMITY_SENSOR 34
#define EZLOPI_SENSOR_0035_TPP_32B_TOUCH_SENSOR 35
#define EZLOPI_DEVICE_0038_RGB_LED 38

#define EZLOPI_SENSOR_0044_I2C_TSL2561_LUMINOSITY 44

// #define EZLOPI_SENSOR_0029_GXHTC3_RH_T_I2C 35

// #define EZLOPI_SENSOR_1024_DEVICE_HEALTH 1024
#define EZLOPI_SENSOR_0055_ADC_FLEXRESISTOR 55

/**
 * @brief defining the type of sensor call function
 *
 */
// int sensor_bme280(e_ezlopi_actions_t action, void *arg);
typedef int (*f_sensor_call_t)(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);
typedef int (*f_sensor_call_v3_t)(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

typedef struct s_ezlopi_device
{
    uint32_t id;
    f_sensor_call_t func;
} s_ezlopi_device_t;

typedef struct s_ezlopi_device_v3
{
    uint32_t id;
    f_sensor_call_v3_t func;
} s_ezlopi_device_v3_t;

// #if 1

typedef struct l_ezlopi_configured_devices
{
    void *user_arg;
    s_ezlopi_device_t *device;
    s_ezlopi_device_properties_t *properties;
    struct l_ezlopi_configured_devices *next;

} l_ezlopi_configured_devices_t;

typedef struct s_ezlopi_prep_arg
{
    cJSON *cjson_device;
    s_ezlopi_device_t *device;

} s_ezlopi_prep_arg_t;

s_ezlopi_device_v3_t *ezlopi_devices_list_get_list_v3(void);

/**
 * @brief Provides the list of available sensors
 *
 * @return f_sensor_call_t*
 */
s_ezlopi_device_t *ezlopi_devices_list_get_list(void);

// void ezlopi_devices_list_add(s_ezlopi_device_t *device);
// int ezlopi_devices_list_add(s_ezlopi_device_t *device);
// int ezlopi_devices_list_add(s_ezlopi_device_t *device, s_ezlopi_device_properties_t *properties);
int ezlopi_devices_list_add(s_ezlopi_device_t *device, s_ezlopi_device_properties_t *properties, void *user_arg);
l_ezlopi_configured_devices_t *ezlopi_devices_list_get_configured_items(void);

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
