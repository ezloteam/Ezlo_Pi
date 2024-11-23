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
/// @brief https://api.ezlo.com/devices/item_value_types/index.html
const char *value_type_int = "int";
// const  char *value_type_int64 = "int64";
const char *value_type_bool = "bool";
const char *value_type_float = "float";
const  char *value_type_scalable = "scalable";
const char *value_type_rgb = "rgb";
// const  char *value_type_usercode = "userCode";
// const  char *value_type_dailyintervalarray = "array.daily_interval";
// const  char *value_type_weeklyintervalarray = "array.weekly_interval";
const char *value_type_string = "string";
// const  char *value_type_table = "table";
const char *value_type_array = "array";
const char *value_type_token = "token";
// const  char *value_type_camerastream = "cameraStream";
const char *value_type_illuminance = "illuminance";
const char *value_type_pressure = "pressure";
const char *value_type_substance_amount = "substance_amount";
// const  char *value_type_power = "power";
const char *value_type_humidity = "humidity";
// const  char *value_type_velocity = "velocity";
const char *value_type_acceleration = "acceleration";
// const  char *value_type_direction = "direction";
const char *value_type_general_purpose = "general_purpose";
// const  char *value_type_acidity = "acidity";
const char *value_type_electric_potential = "electric_potential";
const char *value_type_electric_current = "electric_current";
const char *value_type_force = "force";
// const  char *value_type_irradiance = "irradiance";
// const  char *value_type_precipitation = "precipitation";
const char *value_type_length = "length";
const char *value_type_mass = "mass";
const char *value_type_volume_flow = "volume_flow";
// const  char *value_type_volume = "volume";
const char *value_type_angle = "angle";
const char *value_type_frequency = "frequency";
// const  char *value_type_seismic_intensity = "seismic_intensity";
// const  char *value_type_seismic_magnitude = "seismic_magnitude";
const char *value_type_ultraviolet = "ultraviolet";
const char *value_type_electrical_resistance = "electrical_resistance";
// const  char *value_type_electrical_conductivity = "electrical_conductivity";
// const  char *value_type_loudness = "loudness";
const char *value_type_moisture = "moisture";
// const  char *value_type_time = "time";
// const  char *value_type_radon_concentration = "radon_concentration";
// const  char *value_type_blood_pressure = "blood_pressure";
// const  char *value_type_energy = "energy";
// const  char *value_type_rf_signal_strength = "rf_signal_strength";
const char *value_type_temperature = "temperature";
// const  char *value_type_button_state = "button_state";
// const  char *value_type_action = "action";
// const  char *value_type_user_lock_operation = "user_lock_operation";
// const  char *value_type_dictionary = "dictionary";
// const  char *value_type_amount_of_useful_energy = "amount_of_useful_energy";
// const  char *value_type_kilo_volt_ampere_hour = "kilo_volt_ampere_hour";
// const  char *value_type_reactive_power_instant = "reactive_power_instant";
// const  char *value_type_reactive_power_consumption = "reactive_power_consumption";

/// @brief value in string https://api.ezlo.com/devices/item_enums/index.html
// const  char *value_idle = "idle";
// const  char *value_manual_lock_operation = "manual_lock_operation";
// const  char *value_rf_lock_operation = "rf_lock_operation";
// const  char *value_rf_unlock_operation = "rf_unlock_operation";
// const  char *value_keypad_lock_operation = "keypad_lock_operation";
// const  char *value_keypad_unlock_operation = "keypad_unlock_operation";
// const  char *value_manual_not_fully_locked_operation = "manual_not_fully_locked_operation";

const char *value_type_fingerprint_action = "fingerprint_action";
const char *value_type_presence_operation_mode = "presence_operation_mode";

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
