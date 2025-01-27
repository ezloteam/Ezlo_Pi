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
 * @file    ezlopi_core_gateway.c
 * @brief   Function to operate on gateway
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 1.0
 * @date    October 18th, 2023 9:03 PM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdbool.h>
#include "ezlopi_core_gateway.h"
#include "ezlopi_cloud_constants.h"

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
static s_ezlopi_gateway_t ezlopi_gateway = {
    .ready = true,
    .status = false,
    .manual_device_adding = false,
    ._id = 0x457a5069,
    .settings = 3,

    .label = "virtual gateway",
    .name = "test_plugin",
    .pluginid = "test_plugin",

    .clear_item_dictionary_command = "",
    .set_item_dictionary_value_command = "HUB:test_plugin/scripts/set_item_dictionary_value",
    .add_item_dictionary_value_command = "HUB:test_plugin/scripts/add_item_dictionary_value",
    .remove_item_dictionary_value_command = "HUB:test_plugin/scripts/remove_item_dictionary_value",
    .set_setting_dictionary_value_command = "",

    .check_device_command = "",
    .force_remove_device_command = "",
    .update_device_firmware_command = "",

    .reset_setting_command = "",
    .set_item_value_command = "HUB:test_plugin/scripts/set_item_value",
    .set_setting_value_command = "HUB:test_plugin/scripts/set_setting_value",
    .set_gateway_setting_vlaue_command = "HUB:test_plugin/scripts/set_gateway_setting_value",
};

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
s_ezlopi_gateway_t *EZPI_core_gateway_get(void)
{
    return &ezlopi_gateway;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
