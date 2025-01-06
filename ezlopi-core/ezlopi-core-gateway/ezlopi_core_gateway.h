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
 * @file    main.h
 * @brief   perform some function on data
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
*/
#ifndef _EZLOPI_CORE_GATEWAY_H_
#define _EZLOPI_CORE_GATEWAY_H_

/*******************************************************************************
*                          Include Files
*******************************************************************************/


#include <stdlib.h>
#include <stdint.h>

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
    typedef struct s_ezlopi_gateway
    {
        bool ready;
        bool status;
        bool manual_device_adding;
        uint32_t _id;
        uint32_t settings;

        char *label;
        char *name;
        char *pluginid;

        char *clear_item_dictionary_command;
        char *set_item_dictionary_value_command;
        char *add_item_dictionary_value_command;
        char *remove_item_dictionary_value_command;
        char *set_setting_dictionary_value_command;

        char *check_device_command;
        char *force_remove_device_command;
        char *update_device_firmware_command;

        char *reset_setting_command;
        char *set_item_value_command;
        char *set_setting_value_command;
        char *set_gateway_setting_vlaue_command;

    } s_ezlopi_gateway_t;

    /*******************************************************************************
    *                          Extern Data Declarations
    *******************************************************************************/

    /*******************************************************************************
    *                          Extern Function Prototypes
    *******************************************************************************/

    /**
     * @brief Function to return gateway info
     *
     * @return s_ezlopi_gateway_t*
     */
    s_ezlopi_gateway_t *EZPI_core_gateway_get(void);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_GATEWAY_H_
/*******************************************************************************
*                          End of File
*******************************************************************************/




