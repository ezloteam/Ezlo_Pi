/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    jsn_sr04t.h
 * @brief   perform some function on jsn_sr04t
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    xx
 */

#ifndef _JSN_SR04T_H_
#define _JSN_SR04T_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdio.h>
#include "driver/gpio.h"
#include "driver/rmt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_actions.h"

#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_item_name_str.h"

#include "../../build/config/sdkconfig.h"
#include "cjext.h"

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/
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

#define minimum_detection_value_in_cm 25
#define maximum_detection_value_in_cm 600

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief Function to operate on actions
     *
     * @param action Current Action to Operate on
     * @param item Target-Item node
     * @param arg Arg for action
     * @param user_arg User-arg
     * @return ezlopi_error_t
     */
    // void jsn_sr04t_print_data(jsn_sr04t_data_t jsn_sr04t_data);

    /**
     * @brief Function to initialize JSNSR04T_sensor
     *
     * @param jsn_sr04t_config Pointer to Sensor Configuration
     * @return esp_err_t
     */
    esp_err_t JSN_sr04t_init(jsn_sr04t_config_t *jsn_sr04t_config);
    /**
     * @brief Function to get raw measurement calculations from sensor
     *
     * @param jsn_sr04t_config Pointer to Sensor Configuration
     * @param jsn_sr04t_raw_data Pointer to Sensor raw Data
     * @return esp_err_t
     */
    esp_err_t JSN_sr04t_raw_calc(jsn_sr04t_config_t *jsn_sr04t_config, jsn_sr04t_raw_data_t *jsn_sr04t_raw_data);
    /**
     * @brief Function to process the input sensor-data
     *
     * @param jsn_sr04t_config Pointer to Sensor Configuration
     * @param jsn_sr04t_data Pointer to Input Sensor data
     * @return esp_err_t
     */
    esp_err_t JSN_sr04t_measurement(jsn_sr04t_config_t *jsn_sr04t_config, jsn_sr04t_data_t *jsn_sr04t_data);

#ifdef __cplusplus
}
#endif

#endif //_JSN_SR04T_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/