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
 * @file    main.c
 * @brief   perform some function on data
 * @author  
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef __CORE_GETTERS_API_H__
#define __CORE_GETTERS_API_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "lua/lua.h"

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
    int lcore_get_product_name(lua_State *lua_state);
    int lcore_get_gateways(lua_State *lua_state);
    int lcore_get_gateway(lua_State *lua_state);
    int lcore_get_device(lua_State *lua_state);
    int lcore_get_devices(lua_State *lua_state);
    int lcore_get_devices_ids(lua_State *lua_state);
    int lcore_get_root_device_id(lua_State *lua_state);
    int lcore_get_item(lua_State *lua_state);
    int lcore_get_items(lua_State *lua_state);
    int lcore_get_items_by_device_id(lua_State *lua_state);
    int lcore_get_setting(lua_State *lua_state);
    int lcore_get_setting_ids_by_device_id(lua_State *lua_state);
    int lcore_get_gateway_setting(lua_State *lua_state);
    int lcore_get_gateway_setting_ids_by_gateway_id(lua_State *lua_state);
    int lcore_get_rooms(lua_State *lua_state);
    int lcore_get_room(lua_State *lua_state);

#ifdef __cplusplus
}
#endif

#endif // __CORE_GETTERS_API_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
