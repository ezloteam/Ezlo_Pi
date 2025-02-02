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
 * @file    gateways_commands_str.h
 * @brief   Declerations for gateway commands strings
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 1.0
 * @date    December 25th, 2023 9:01 PM
 */

#ifndef __GATEWAYS_COMMANDS_STR_H__
#define __GATEWAYS_COMMANDS_STR_H__

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

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Global function template example
     * Convention : Use capital letter for initial word on extern function
     * maincomponent : Main component as hal, core, service etc.
     * subcomponent : Sub component as i2c from hal, ble from service etc
     * functiontitle : Title of the function
     * eg : EZPI_hal_i2c_init()
     * @param arg
     *
     */
    extern const char *gw_addItemDictionaryValueCommand_str;
    extern const char *gw_checkDeviceCommand_str;
    extern const char *gw_clearItemDictionaryCommand_str;
    extern const char *gw_forceRemoveDeviceCommand_str;
    extern const char *gw_manualDeviceAdding_str;
    extern const char *gw_pluginId_str;
    extern const char *gw_not_ready_str;
    extern const char *gw_removeItemDictionaryValueCommand_str;
    extern const char *gw_resetSettingCommand_str;
    extern const char *gw_setGatewaySettingValueCommand_str;
    extern const char *gw_setItemDictionaryValueCommand_str;
    extern const char *gw_setItemValueCommand_str;
    extern const char *gw_setSettingDictionaryValueCommand_str;
    extern const char *gw_setSettingValueCommand_str;
    extern const char *gw_updateDeviceFirmwareCommand_str;

#ifdef __cplusplus
}
#endif

#endif // __GATEWAYS_COMMANDS_STR_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
