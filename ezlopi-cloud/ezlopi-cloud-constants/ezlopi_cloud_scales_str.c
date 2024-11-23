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
/// @brief https://api.ezlo.com/devices/item_value_types/index.html#scalable-types

/// @brief Temperature
const char *scales_celsius = "celsius";
const char *scales_fahrenheit = "fahrenheit";

/// @brief humidity
const char *scales_percent = "percent";
const char *scales_gram_per_cubic_meter = "gram_per_cubic_meter";

/// @brief kilo_volt_ampere_hour
const char *scales_kilo_volt_ampere_hour = "kilo_volt_ampere_hour";

/// @brief reactive power
const char *scales_kilo_volt_ampere_reactive = "kilo_volt_ampere_reactive";

/// @brief amount_of_useful_energy
const char *scales_amount_of_useful_energy = "amount_of_useful_energy";

/// @brief length
const char *scales_centi_meter = "centi_meter";
const char *scales_meter = "meter";
const char *scales_feet = "feet";

/// @brief LUX
const char *scales_lux = "lux";

/// @brief pressure
const char *scales_kilo_pascal = "kilo_pascal";

/// @brief electrical_resistance
const char *scales_ohm_meter = "ohm_meter";

/// @brief substance_amount
const char *scales_micro_gram_per_cubic_meter = "micro_gram_per_cubic_meter";
const char *scales_mole_per_cubic_meter = "mole_per_cubic_meter";
const char *scales_particles_per_deciliter = "particles_per_deciliter";
const char *scales_parts_per_million = "parts_per_million";
const char *scales_milli_gram_per_liter = "milli_gram_per_liter";

/// @brief angle
const char *scales_north_pole_degress = "north_pole_degress";

/// @brief volume_flow
const char *scales_cubic_meter_per_hour = "cubic_meter_per_hour";
const char *scales_cubic_feet_per_minute = "cubic_feet_per_minute";
const char *scales_liter_per_hour = "liter_per_hour";

/// @brief force
const char *scales_newton = "newton";

/// @brief electric_potential
const char *scales_milli_volt = "milli_volt";
const char *scales_volt = "volt";

/// @brief acceleration
const char *scales_meter_per_square_second = "meter_per_square_second";

/// @brief electric_current
const char *scales_ampere = "ampere";
const char *scales_milli_ampere = "milli_ampere";

/// @brief mass
const char *scales_kilo_gram = "kilo_gram";
const char *scales_pounds = "pounds";

/// @brief irradiance
const char *scales_watt_per_square_meter = "watt_per_square_meter";

/// @brief frequency
const char *scales_revolutions_per_minute = "revolutions_per_minute";
const char *scales_hertz = "hertz";
const char *scales_kilo_hertz = "kilo_hertz";
const char *scales_breaths_per_minute = "breaths_per_minute";
const char *scales_beats_per_minute = "beats_per_minute";

/// @brief magnetic field strength
const char *scales_guass = "guass";
const char *scales_tesla = "tesla";
const char *scales_weber_per_square_meter = "weber_per_square_meter";

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
