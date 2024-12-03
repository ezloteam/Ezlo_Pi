/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 * @note
 * ESP32 sample connections
 * gpio1 => gpio_s0             GPIO_NUM_25
 * gpio2 => gpio_s1             GPIO_NUM_26
 * gpio3 => gpio_s2             GPIO_NUM_12
 * gpio4 => gpio_s3             GPIO_NUM_14
 * gpio5 => gpio_output_en      GPIO_NUM_13
 * gpio6 => gpio_pulse_output   GPIO_NUM_15
 */

#ifndef _SENSOR_0040_OTHER_TCS230_H_
#define _SENSOR_0040_OTHER_TCS230_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "driver/gpio.h"

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_errors.h"

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
    typedef enum e_TCS230_queue
    {
        TCS230_QUEUE_RESET = 0,
        TCS230_QUEUE_AVAILABLE,
        TCS230_QUEUE_FULL,
    } e_TCS230_queue_t;

    typedef enum e_TCS230_color
    {
        COLOR_SENSOR_COLOR_RED = 0,
        COLOR_SENSOR_COLOR_BLUE,
        COLOR_SENSOR_COLOR_CLEAR,
        COLOR_SENSOR_COLOR_GREEN,
        COLOR_SENSOR_COLOR_MAX
    } e_TCS230_color_t;

    typedef enum e_TCS230_freq_scaling
    {
        COLOR_SENSOR_FREQ_SCALING_POWER_DOWN = 0,
        COLOR_SENSOR_FREQ_SCALING_2_PERCENT,
        COLOR_SENSOR_FREQ_SCALING_20_PERCENT,
        COLOR_SENSOR_FREQ_SCALING_100_PERCENT,
        COLOR_SENSOR_FREQ_SCALING_MAX
    } e_TCS230_freq_scaling_t;

    typedef struct s_TCS230_calib_data
    {
        // contains larger time_periods
        int32_t least_red_timeP;
        int32_t least_green_timeP;
        int32_t least_blue_timeP;

        // contains smaller time_periods
        int32_t most_red_timeP;
        int32_t most_green_timeP;
        int32_t most_blue_timeP;
    } s_TCS230_calib_data_t;

    typedef struct s_TCS230_pin
    {
        uint32_t gpio_s0;
        uint32_t gpio_s1;
        uint32_t gpio_s2;
        uint32_t gpio_s3;
        uint32_t gpio_output_en;    // 'gpio_output_en' pin acts as a gate to start/stop output from 'gpio_pulse_output' pin
        uint32_t gpio_pulse_output; // 'gpio_pulse_output' pin produces pulses, whose periods determine the corresponding color.
    } s_TCS230_pin_t;

    typedef struct s_TCS230_data
    {
        bool calibration_complete;
        s_TCS230_calib_data_t calib_data;
        uint32_t red_mapped;
        uint32_t green_mapped;
        uint32_t blue_mapped;
        s_TCS230_pin_t TCS230_pin;
    } s_TCS230_data_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Global function template example
     * Convention : Use capital letter for initial word on extern function
     * maincomponent : Main component as hal, core, service etc.
     * subcomponent : Sub component as i2c from hal, ble from service etc
     * functiontitle : Title of the function
     * eg : EZPI_hal_i2c_init()
     * @param arg
     *
     */
    bool tcs230_set_filter_color(l_ezlopi_item_t *item, e_TCS230_color_t color);
    bool tcs230_set_frequency_scaling(l_ezlopi_item_t *item, e_TCS230_freq_scaling_t scale);

    void calculate_max_min_color_values(gpio_num_t gpio_output_en, gpio_num_t gpio_pulse_output, int32_t *least_color_timeP, int32_t *most_color_timeP);
    bool get_tcs230_sensor_value(l_ezlopi_item_t *item);

    //------------------------------------------------------------------------
    //              Action
    //------------------------------------------------------------------------
    ezlopi_error_t sensor_0040_other_TCS230(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#ifdef __cplusplus
}
#endif

#endif // _SENSOR_0040_OTHER_TCS230_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
