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
 */

#ifndef __EZLOPI_SUBCATEGORY_H__
#define __EZLOPI_SUBCATEGORY_H__

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
    extern const char *subcategory_not_defined;
    extern const char *subcategory_dimmable_bulb;
    // extern const  char *subcategory_dimmable_plugged;
    // extern const  char *subcategory_dimmable_in_wall;
    extern const char *subcategory_dimmable_colored;
    // extern const  char *subcategory_interior_plugin;
    // extern const  char *subcategory_exterior_plugin;
    extern const char *subcategory_in_wall;
    // extern const  char *subcategory_refrigerator;
    // extern const  char *subcategory_valve;
    extern const char *subcategory_relay;
    extern const char *subcategory_door;
    extern const char *subcategory_leak;
    extern const char *subcategory_motion;
    // extern const  char *subcategory_glass;
    // extern const  char *subcategory_smoke;
    // extern const  char *subcategory_co;
    // extern const  char *subcategory_co2;
    extern const char *subcategory_gas;
    extern const char *subcategory_heat;
    // extern const  char *subcategory_hvac;
    // extern const  char *subcategory_heater;
    // extern const  char *subcategory_cooler;
    // extern const  char *subcategory_custom_hvac;
    // extern const  char *subcategory_indoor_cam;
    // extern const  char *subcategory_outdoor_cam;
    // extern const  char *subcategory_doorbell_cam;
    // extern const  char *subcategory_window_cov;
    // extern const  char *subcategory_zrtsi;
    extern const char *subcategory_irt;
    // extern const  char *subcategory_usbuirt;
    // extern const  char *subcategory_generic_io;
    // extern const  char *subcategory_repeater;
    // extern const  char *subcategory_controller;
    // extern const  char *subcategory_generic;
    // extern const  char *subcategory_panic;
    // extern const  char *subcategory_light;
    // extern const  char *subcategory_rain;
    extern const char *subcategory_moisture;
    // extern const  char *subcategory_freeze;
    // extern const  char *subcategory_power;
    // extern const  char *subcategory_activity;
    // extern const  char *subcategory_current;
    // extern const  char *subcategory_velocity;
    // extern const  char *subcategory_capacity;
    extern const char *subcategory_water;
    extern const  char *subcategory_particulate_matter;
    // extern const  char *subcategory_frequency;
    // extern const  char *subcategory_health;
    // extern const  char *subcategory_modulation;
    // extern const  char *subcategory_soil;
    // extern const  char *subcategory_airpullution;
    extern const char *subcategory_electricity;
    extern const char *subcategory_sound;
    extern const char *subcategory_navigation;
    // extern const  char *subcategory_seismicity;
    // extern const  char *subcategory_time;

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_SUBCATEGORY_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
