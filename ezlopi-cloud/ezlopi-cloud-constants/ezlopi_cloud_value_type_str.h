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
 * @file    ezlopi_cloud_valye_type_str.h
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef __EZLOPI_VALUES_STR_H__
#define __EZLIPI_VALUES_STR_H__
#pragma once

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

    /// @brief https://api.ezlo.com/devices/item_value_types/index.html
    extern const char *value_type_int;
    // extern const  char *value_type_int64;
    extern const char *value_type_bool;
    extern const char *value_type_float;
    extern const char *value_type_scaleble;
    extern const char *value_type_rgb;
    // extern const  char *value_type_usercode;
    // extern const  char *value_type_dailyintervalarraydaily_interval";
    // extern const  char *value_type_weeklyintervalarrayweekly_interval";
    extern const char *value_type_string;
    // extern const  char *value_type_table;
    extern const char *value_type_array;
    extern const char *value_type_token;
    // extern const  char *value_type_camerastream;
    extern const char *value_type_illuminance;
    extern const char *value_type_pressure;
    extern const char *value_type_substance_amount;
    // extern const  char *value_type_power;
    extern const char *value_type_humidity;
    // extern const  char *value_type_velocity;
    extern const char *value_type_acceleration;
    // extern const  char *value_type_direction;
    extern const char *value_type_general_purpose;
    // extern const  char *value_type_acidity;
    extern const char *value_type_electric_potential;
    extern const char *value_type_electric_current;
    extern const char *value_type_force;
    // extern const  char *value_type_irradiance;
    // extern const  char *value_type_precipitation;
    extern const char *value_type_length;
    extern const char *value_type_mass;
    extern const char *value_type_volume_flow;
    // extern const  char *value_type_volume;
    extern const char *value_type_angle;
    extern const char *value_type_frequency;
    // extern const  char *value_type_seismic_intensity;
    // extern const  char *value_type_seismic_magnitude;
    extern const char *value_type_ultraviolet;
    extern const char *value_type_electrical_resistance;
    // extern const  char *value_type_electrical_conductivity;
    // extern const  char *value_type_loudness;
    extern const char *value_type_moisture;
    // extern const  char *value_type_time;
    // extern const  char *value_type_radon_concentration;
    // extern const  char *value_type_blood_pressure;
    // extern const  char *value_type_energy;
    // extern const  char *value_type_rf_signal_strength;
    extern const char *value_type_temperature;
    // extern const  char *value_type_button_state;
    // extern const  char *value_type_action;
    // extern const  char *value_type_user_lock_operation;
    // extern const  char *value_type_dictionary;
    // extern const  char *value_type_amount_of_useful_energy;
    // extern const  char *value_type_kilo_volt_ampere_hour;
    // extern const  char *value_type_reactive_power_instant;
    // extern const  char *value_type_reactive_power_consumption;

    /// @brief value in string https://api.ezlo.com/devices/item_enums/index.html
    // extern const  char *value_idle;
    // extern const  char *value_manual_lock_operation;
    // extern const  char *value_rf_lock_operation;
    // extern const  char *value_rf_unlock_operation;
    // extern const  char *value_keypad_lock_operation;
    // extern const  char *value_keypad_unlock_operation;
    // extern const  char *value_manual_not_fully_locked_operation;
    extern const char *value_type_fingerprint_action;
    extern const char *value_type_presence_operation_mode;
    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_VLAUES_STR_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
