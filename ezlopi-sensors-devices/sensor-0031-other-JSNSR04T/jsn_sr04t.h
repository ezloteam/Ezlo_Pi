#ifndef _JSN_SR04T_H_
#define _JSN_SR04T_H_

#include <stdio.h>
#include "driver/gpio.h"
#include "driver/rmt.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_actions.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
// #include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_item_name_str.h"
#include "sdkconfig.h"
#include "cjext.h"

#define minimum_detection_value_in_cm 25
#define maximum_detection_value_in_cm 600
static const char TAG1[] = "JSN_SR04T_V3";

/// @brief Structure for configuring the JSN_SR04T module sensor

typedef struct
{
    bool is_init;                /*!< Has this component instance been initialized? */
    gpio_num_t trigger_gpio_num; /*!< The GPIO# that is wired up to the TRIGGER pin of the sensor */
    gpio_num_t echo_gpio_num;    /*!< The GPIO# that is wired up to the ECHO pin of the sensor */
    rmt_channel_t rmt_channel;   /*!< Which RMT Channel number to use (handy when using multiple components that use the RMT peripheral */
    double offset_cm;            /*!< The distance (cm) between the sensor and the artifact to be monitored. Typical for a stilling well setup. This distance is subtracted from the actual measurement. Also used to circumvent the dead measurement zone of the sensor (+-25cm). */
    uint32_t no_of_errors;       /*!< Runtime Statistics: the total number of errors when interacting with the sensor */
    uint32_t no_of_samples;      /*!< How many samples to read to come to one weighted measurement? */
    // double max_range_allowed_in_samples_cm;/*<! Reject a set of measurements if the range is higher than this prop. Statistics Dispersion Range outlier detection method */
} jsn_sr04t_config_t;

#define JSN_SR04T_CONFIG_DEFAULT()        \
    {                                     \
        .is_init = false,                 \
        .trigger_gpio_num = GPIO_NUM_MAX, \
        .echo_gpio_num = GPIO_NUM_MAX,    \
        .rmt_channel = RMT_CHANNEL_MAX,   \
        .offset_cm = 0.0,                 \
        .no_of_errors = 0,                \
        .no_of_samples = 5,               \
    }

typedef struct
{
    bool data_received; /*!< Has data been received from the device? */
    bool is_an_error;   /*!< Is the data an error? */
    uint32_t raw;       /*!< The raw measured value (from RMT) */
    double distance_cm; /*!< This distance is adjusted with the distance_sensor_to_artifact_cm (subtracted). */
} jsn_sr04t_raw_data_t;

/*
 * DATA
 */

typedef struct
{
    bool data_received; /*!< Has data been received from the device? */
    bool is_an_error;   /*!< Is the data an error? */
    double distance_cm; /*!< The measured distance. The distance_sensor_to_artifact_cm is already subtracted from the original measured distance. */
} jsn_sr04t_data_t;

#define JSN_SR04T_DATA_DEFAULT() \
    {                            \
        .data_received = false,  \
        .is_an_error = false,    \
        .distance_cm = 0.0,      \
    }

// void jsn_sr04t_print_data(jsn_sr04t_data_t jsn_sr04t_data);
esp_err_t init_JSN_SR04T(jsn_sr04t_config_t* jsn_sr04t_config);
esp_err_t raw_measeurement(jsn_sr04t_config_t* jsn_sr04t_config, jsn_sr04t_raw_data_t* jsn_sr04t_raw_data);
esp_err_t measurement(jsn_sr04t_config_t* jsn_sr04t_config, jsn_sr04t_data_t* jsn_sr04t_data);

#if 0 // v2.x
static int ezlopi_JSN_SR04T_prepare_and_add(void* args);
static s_ezlopi_device_properties_t* JSN_SR04T_sensor_prepare(cJSON* cjson_device);
static int ezlopi_JSN_SR04T_init(s_ezlopi_device_properties_t* properties);
static int ezlopi_JSN_SR04T_update_value(s_ezlopi_device_properties_t* properties, void* arg);
static int ezlopi_JSN_SR04T_get_value_cjson(s_ezlopi_device_properties_t* properties, void* args);
int JSN_SR04T(e_ezlopi_actions_t action, s_ezlopi_device_properties_t* properties, void* arg, void* user_arg);
#endif

#endif //_JSN_SR04T_H_