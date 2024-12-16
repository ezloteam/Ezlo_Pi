
/**
 * @file    ezlopi_cloud_offline_login.c
 * @brief
 * @author
 * @version
 * @date
 */
/* ===========================================================================
** Copyright (C) 2022 Ezlo Innovation Inc
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ezlopi_core_offline_login.h"
#include "ezlopi_core_devices_list.h"

#include "ezlopi_cloud_constants.h"

/**
 * @brief Function to prepare error response to the offline login request
 *
 * @param cj_response Outgoing JSON response
 * @param error_code Error code to send
 * @param error_message Message to attach to the response
 * @param error_data Data that describes the error message
 */
static void ezlopi_cloud_prepare_error(cJSON *cj_response, int error_code, const char *error_message, const char *error_data);

void EZPI_CLOUD_offline_login(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            ezlopi_error_t error = ezlopi_core_offline_login_perform(cj_params);
            if (EZPI_ERR_WRONG_PARAM == error)
            {
                ezlopi_cloud_prepare_error(cj_response, -32602, "Wrong params", "rpc.params.invalid");
            }
            else if (EZPI_FAILED == error)
            {
                ezlopi_cloud_prepare_error(cj_response, -32500, "Already logged in", "user.login.alreadylogged");
            }
            else if (EZPI_ERR_INVALID_CREDENTIALS == error)
            {
                ezlopi_cloud_prepare_error(cj_response, -32500, "Bad password", "user.login.badpassword");
            }
        }
        else
        {
            ezlopi_cloud_prepare_error(cj_response, -32600, "Bad request", "rpc.params.notfound");
        }
    }
}

static void ezlopi_cloud_prepare_error(cJSON *cj_response, int error_code, const char *error_message, const char *error_data)
{
    cJSON *cj_error = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_error_str);
    if (cj_error)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_error, ezlopi_code_str, error_code);
        cJSON_AddStringToObject(__FUNCTION__, cj_error, ezlopi_message_str, error_message);
        cJSON_AddStringToObject(__FUNCTION__, cj_error, ezlopi_data_str, error_data);
    }
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
