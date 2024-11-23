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
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lstate.h"

#include "ezlopi_core_factory_info.h"

#include "corelib.h"
#include "ezlopi_util_trace.h"

#include "core_getters_api.h"
#include "core_adders_api.h"
#include "core_removers_api.h"
#include "core_updaters_api.h"
#include "core_others_api.h"
#include "core_external_gateway_api.h"

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
static const struct luaL_Reg lcore_funcs[] = {
#define CORELIB_FUNCTION(f_name, l_func) {.name = f_name, .func = l_func},
#include "corelib_macro.h"
#undef CORELIB_FUNCTION
    {.name = NULL, .func = NULL},
};

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/**
 * @brief Global/extern function template example
 * Convention : Use capital letter for initial word on extern function
 * @param arg
 */
int luaopen_lcore(lua_State *lua_state)
{
    luaL_newlib(lua_state, lcore_funcs);
    return 1;
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
