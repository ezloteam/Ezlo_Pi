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
 * @file    ezlopi_core_protect.h
 * @brief   perform some function Based on the security configuration, using which broadcasts are tuned to have parts related to the Protect services.
 * @author  Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_PROTECT_H_
#define _EZLOPI_CORE_PROTECT_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "cjext.h"

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
    typedef enum e_sound_type
    {
        EZLOPI_SECURITY_EVENT_NONE = 0,
        EZLOPI_SECURITY_EVENT_POLICE,
        EZLOPI_SECURITY_EVENT_SILENT,
        EZLOPI_SECURITY_EVENT_AUXILIARY,
        EZLOPI_SECURITY_EVENT_FIRE,
        EZLOPI_SECURITY_EVENT_SPECIAL,
        EZLOPI_SECURITY_EVENT_CO,
        EZLOPI_SECURITY_EVENT_MAX,
    } e_sound_type_t;

    typedef enum e_entry_delay
    {
        EZLOPI_ENTRY_DELAY_NONE = 0,
        EZLOPI_ENTRY_DELAY_NORMAL,
        EZLOPI_ENTRY_DELAY_LONG_EXTENDED,
        EZLOPI_ENTRY_DELAY_EXTENDED,
        EZLOPI_ENTRY_DELAY_INSTANT,
        EZLOPI_ENTRY_DELAY_MAX,
    } e_entry_delay_t;

    typedef struct s_security_config
    {
        uint32_t u_id; // Device group identifier. Human-readable names for predefined groups, autogenerated values

        const char *name;        // [128 bytes limit] Predefined security configuration name
        const char *description; // [128 bytes limit] Long security config description
        const char *follower;    // enum["yes", "no"] If no - no follower functionality on the group. If yes - group has unconditional follower functionality.
        bool persistant;         // Default is true for security configs created from the config file
        bool editable;           // Default is true. If false, should deny any attempts to modify the security config.
        bool alarm;              // Default is true. If true, alarm is caused by security and protect events of the devices. If false, no security events are initiated.
        bool chime;              // If true, chime should ring

        e_sound_type_t sound_type;  // Sound type that should be played by annunciators In response on the security event.
        e_entry_delay_t entryDelay; // Entry Delay name. Security event should have that entry delay if it happens to a device that belongs to the group.

        cJSON *armedWhen; // enum ArmingType[] // Arming modes. Devices that belong to the Security Config should be armed in all listed modes, and disarmed in all non-listed modes
        cJSON *tags;      // Device groups to reconnect when pairing

        // Add signal structure HERE

        struct s_security_config *next;
    } s_security_config_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_PROTECT_H_
/*******************************************************************************
 *                          End of File
 *******************************************************************************/
