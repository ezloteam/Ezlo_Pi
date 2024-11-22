/**
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */
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
    extern const char *category_not_defined;
    extern const char *category_interface;
    extern const char *category_dimmable_light;
    extern const char *category_switch;
    extern const char *category_security_sensor;
    extern const char *category_generic_sensor;
    // extern const  char *category_hvac;
    // extern const  char *category_camera;
    extern const char *category_door_lock;
    // extern const  char *category_window_cov;
    // extern const  char *category_remote_control;
    extern const char *category_ir_tx;
    // extern const  char *category_generic_io;
    // extern const  char *category_serial_port;
    // extern const  char *category_scene_controller;
    // extern const  char *category_av;
    extern const char *category_humidity;
    extern const char *category_temperature;
    extern const char *category_light_sensor;
    // extern const  char *category_zwave_int;
    // extern const  char *category_insteon_int;
    // extern const  char *category_power_meter;
    // extern const  char *category_alarm_panel;
    // extern const  char *category_alarm_partition;
    // extern const  char *category_siren;
    extern const char *category_weather;
    // extern const  char *category_philips_controller;
    // extern const  char *category_appliance;
    // extern const  char *category_uv_sensor;
    // extern const  char *category_mouse_trap;
    // extern const  char *category_doorbell;
    // extern const  char *category_keypad;
    // extern const  char *category_garage_door;
    extern const char *category_flow_meter;
    // extern const  char *category_voltage_sensor;
    // extern const  char *category_state_sensor;
    extern const char *category_level_sensor;
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
