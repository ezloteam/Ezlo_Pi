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
 * @file    ezlopi_cloud_scales_str.h
 * @brief
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 1.0
 * @date    October 13th, 2022 7:24 PM
 */

#ifndef __EZLOPI_SCALES_STR_H__
#define __EZLOPI_SCALES_STR_H__

/// @brief https://api.ezlo.com/devices/item_value_types/index.html#scalable-types
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
    /// @brief https://api.ezlo.com/devices/item_value_types/index.html#scalable-types

    /// @brief Temperature
    extern const char *scales_celsius;
    extern const char *scales_fahrenheit;

    /// @brief humidity
    extern const char *scales_percent;
    extern const char *scales_gram_per_cubic_meter;

    /// @brief kilo_volt_ampere_hour
    extern const char *scales_kilo_volt_ampere_hour;

    /// @brief reactive power
    extern const char *scales_kilo_volt_ampere_reactive;

    /// @brief amount_of_useful_energy
    extern const char *scales_amount_of_useful_energy;

    /// @brief length
    extern const char *scales_centi_meter;
    extern const char *scales_meter;
    extern const char *scales_feet;

    /// @brief LUX
    extern const char *scales_lux;

    /// @brief pressure
    extern const char *scales_kilo_pascal;

    /// @brief electrical_resistance
    extern const char *scales_ohm_meter;

    /// @brief substance_amount
    extern const char *scales_micro_gram_per_cubic_meter;
    extern const char *scales_mole_per_cubic_meter;
    extern const char *scales_particles_per_deciliter;
    extern const char *scales_parts_per_million;
    extern const char *scales_milli_gram_per_liter;

    /// @brief angle
    extern const char *scales_north_pole_degress;

    /// @brief volume_flow
    extern const char *scales_cubic_meter_per_hour;
    extern const char *scales_cubic_feet_per_minute;
    extern const char *scales_liter_per_hour;

    /// @brief force
    extern const char *scales_newton;

    /// @brief electric_potential
    extern const char *scales_milli_volt;
    extern const char *scales_volt;

    /// @brief acceleration
    extern const char *scales_meter_per_square_second;

    /// @brief electric_current
    extern const char *scales_ampere;
    // const static char *scales_milli_ampere = "milli_ampere";

    /// @brief mass
    extern const char *scales_kilo_gram;
    // const static char *scales_pounds = "pounds"

    /// @brief irradiance
    extern const char *scales_watt_per_square_meter;

    /// @brief frequency
    extern const char *scales_revolutions_per_minute;
    extern const char *scales_hertz;
    extern const char *scales_kilo_hertz;
    extern const char *scales_breaths_per_minute;
    extern const char *scales_beats_per_minute;

    /// @brief magnetic field strength
    extern const char *scales_guass;
    extern const char *scales_tesla;
    extern const char *scales_weber_per_square_meter;
    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_SCALES_STR_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
