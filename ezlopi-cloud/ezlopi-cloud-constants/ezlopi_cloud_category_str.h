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
 * @file    ezlopi_cloud_category_str.h
 * @brief
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 1.0
 * @date    October 12th, 2022 8:24 PM
 */

#ifndef __EZLOPI_CATEGORY_H__
#define __EZLOPI_CATEGORY_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

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

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /// @brief https://api.ezlo.com/devices/categories/index.html

    /**
     * @brief Variable to be used for undefined type cloud category
     *
     */
    extern const char *category_not_defined;
    /**
     * @brief Variable to be used for interface cloud category
     *
     */
    extern const char *category_interface;
    /**
     * @brief Variable to be used for dimmable light type cloud category
     *
     */
    extern const char *category_dimmable_light;
    /**
     * @brief Variable to be used for switch type cloud category
     *
     */
    extern const char *category_switch;
    /**
     * @brief Variable to be used for security type cloud category
     *
     */
    extern const char *category_security_sensor;
    /**
     * @brief Variable to be used for generic type cloud category
     *
     */
    extern const char *category_generic_sensor;
    /**
     * @brief Variable to be used for hvac type cloud category
     *
     */
    // extern const  char *category_hvac;
    /**
     * @brief Variable to be used for camera type cloud category
     *
     */
    // extern const  char *category_camera;
    /**
     * @brief Variable to be used for door lock type cloud category
     *
     */
    extern const char *category_door_lock;
    /**
     * @brief Variable to be used for window cov type cloud category
     *
     */
    // extern const  char *category_window_cov;
    /**
     * @brief Variable to be used for remote control type cloud category
     *
     */
    // extern const  char *category_remote_control;
    /**
     * @brief Variable to be used for IR/TX type cloud category
     *
     */
    extern const char *category_ir_tx;
    /**
     * @brief Variable to be used for generic IO type cloud category
     *
     */
    // extern const  char *category_generic_io;
    /**
     * @brief Variable to be used for serial port type cloud category
     *
     */
    // extern const  char *category_serial_port;
    /**
     * @brief Variable to be used for scene controller type cloud category
     *
     */
    // extern const  char *category_scene_controller;
    /**
     * @brief Variable to be used for av type cloud category
     *
     */
    // extern const  char *category_av;
    /**
     * @brief Variable to be used for humidity type cloud category
     *
     */
    extern const char *category_humidity;
    /**
     * @brief Variable to be used for temperature type cloud category
     *
     */
    extern const char *category_temperature;
    /**
     * @brief Variable to be used for light sensor type cloud category
     *
     */
    extern const char *category_light_sensor;
    /**
     * @brief Variable to be used for zwave type cloud category
     *
     */
    // extern const  char *category_zwave_int;
    /**
     * @brief Variable to be used for insteon type cloud category
     *
     */
    // extern const  char *category_insteon_int;
    /**
     * @brief Variable to be used for power meter type cloud category
     *
     */
    // extern const  char *category_power_meter;
    /**
     * @brief Variable to be used for alarm panel type cloud category
     *
     */
    // extern const  char *category_alarm_panel;
    /**
     * @brief Variable to be used for alarm partition type cloud category
     *
     */
    // extern const  char *category_alarm_partition;
    /**
     * @brief Variable to be used for siren type cloud category
     *
     */
    // extern const  char *category_siren;
    /**
     * @brief Variable to be used for weather type cloud category
     *
     */
    extern const char *category_weather;
    /**
     * @brief Variable to be used for philips controller type cloud category
     *
     */
    // extern const  char *category_philips_controller;
    /**
     * @brief Variable to be used for appliance type cloud category
     *
     */
    // extern const  char *category_appliance;
    /**
     * @brief Variable to be used for uv sensor type cloud category
     *
     */
    // extern const  char *category_uv_sensor;
    /**
     * @brief Variable to be used for mouse trap type cloud category
     *
     */
    // extern const  char *category_mouse_trap;
    /**
     * @brief Variable to be used for door bell type cloud category
     *
     */
    // extern const  char *category_doorbell;
    /**
     * @brief Variable to be used for keypad type cloud category
     *
     */
    // extern const  char *category_keypad;
    /**
     * @brief Variable to be used for garage door type cloud category
     *
     */
    // extern const  char *category_garage_door;
    /**
     * @brief Variable to be used for flow meter type cloud category
     *
     */
    extern const char *category_flow_meter;
    /**
     * @brief Variable to be used for voltage sensor type cloud category
     *
     */
    // extern const  char *category_voltage_sensor;
    /**
     * @brief Variable to be used for state sensor type cloud category
     *
     */
    // extern const  char *category_state_sensor;
    /**
     * @brief Variable to be used for level sensor type cloud category
     *
     */
    extern const char *category_level_sensor;
    /**
     * @brief Variable to be used for clock type cloud category
     *
     */
    // extern const  char *category_clock;

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_CATEGORY_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
