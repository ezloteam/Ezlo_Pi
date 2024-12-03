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

#ifndef __EZLOPI_DEVICE_TYPES_STR_H__
#define __EZLOPI_DEVICE_TYPES_STR_H__

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
    /// @brief https://api.ezlo.com/devices/devices/index.html
    extern const char *dev_type_device;
    extern const char *dev_type_sensor;
    extern const char *dev_type_sensor_motion;
    // extern const  char *dev_type_sensor_multisensor;
    // extern const  char *dev_type_light_bulb;
    // extern const  char *dev_type_light_strip;
    // extern const  char *dev_type_light_controller;
    // extern const  char *dev_type_thermostat;
    extern const char *dev_type_switch_outlet;
    extern const char *dev_type_dimmer_outlet;
    extern const char *dev_type_switch_inwall;
    // extern const  char *dev_type_dimmer_inwall;
    // extern const  char *dev_type_multi_outlet;
    extern const char *dev_type_doorlock;
    // extern const  char *dev_type_siren;
    // extern const  char *dev_type_doorbell;
    // extern const  char *dev_type_repeater;
    // extern const  char *dev_type_shutter_valve;
    // extern const  char *dev_type_shutter_garage;
    // extern const  char *dev_type_shutter_roller;
    // extern const  char *dev_type_controller_remote;
    // extern const  char *dev_type_controller_portable;
    // extern const  char *dev_type_controller_panic;
    // extern const  char *dev_type_mousetrap;
    extern const char *dev_type_transmitter_ir;
    // extern const  char *dev_type_meter_power;
    // extern const  char *dev_type_meter_flow;
    // extern const  char *dev_type_camera

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_DEVICE_TYPES_STR_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
