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
 * @file    ezlopi_cloud_category_str.c
 * @brief   Contains definitions of cloud constant strings
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    1st January 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/
#ifndef __EZLOPI_CATEGORY_H__
#define __EZLOPI_CATEGORY_H__
/// @brief https://api.ezlo.com/devices/categories/index.html
const char *category_not_defined = "";
const char *category_interface = "interface";
const char *category_dimmable_light = "dimmable_light";
const char *category_switch = "switch";
const char *category_security_sensor = "security_sensor";
const char *category_generic_sensor = "generic_sensor";
// const  char *category_hvac = "hvac";
// const  char *category_camera = "camera";
const char *category_door_lock = "door_lock";
// const  char *category_window_cov = "window_cov";
// const  char *category_remote_control = "remote_control";
const char *category_ir_tx = "ir_tx";
// const  char *category_generic_io = "generic_io";
// const  char *category_serial_port = "serial_port";
// const  char *category_scene_controller = "scene_controller";
// const  char *category_av = "av";
const char *category_humidity = "humidity";
const char *category_temperature = "temperature";
const char *category_light_sensor = "light_sensor";
// const  char *category_zwave_int = "zwave_int";
// const  char *category_insteon_int = "insteon_int";
// const  char *category_power_meter = "power_meter";
// const  char *category_alarm_panel = "alarm_panel";
// const  char *category_alarm_partition = "alarm_partition";
// const  char *category_siren = "siren";
const char *category_weather = "weather";
// const  char *category_philips_controller = "philips_controller";
// const  char *category_appliance = "appliance";
// const  char *category_uv_sensor = "uv_sensor";
// const  char *category_mouse_trap = "mouse_trap";
// const  char *category_doorbell = "doorbell";
// const  char *category_keypad = "keypad";
// const  char *category_garage_door = "garage_door";
const char *category_flow_meter = "flow_meter";
// const  char *category_voltage_sensor = "voltage_sensor";
// const  char *category_state_sensor = "state_sensor";
const char *category_level_sensor = "level_sensor";
// const  char *category_clock = "clock";

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

#endif //__EZLOPI_CATEGORY_H__
/*******************************************************************************
 *                          End of File
 *******************************************************************************/
