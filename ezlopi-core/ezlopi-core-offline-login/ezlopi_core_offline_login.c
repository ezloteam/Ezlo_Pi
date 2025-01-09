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
 * @file    ezlopi_core_offline_login.c
 * @brief   Function to perform offine login operation
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include "cjext.h"
#include "ezlopi_core_nvs.h"
#include "EZLOPI_USER_CONFIG.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_offline_login.h"

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
static bool logged_in = false;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
ezlopi_error_t EZPI_core_offline_login_perform(cJSON *cj_params)
{
    ezlopi_error_t error = EZPI_SUCCESS;
    if (logged_in)
    {
        error = EZPI_FAILED;
    }
    else
    {
        cJSON *cj_user = cJSON_GetObjectItem(__FUNCTION__, cj_params, "user");
        cJSON *cj_token = cJSON_GetObjectItem(__FUNCTION__, cj_params, "token");
        if (cj_user && cj_token && (cJSON_IsString(cj_user)) && (cJSON_IsString(cj_token)))
        {

            // char *stored_uesr_id = EZPI_core_nvs_read_user_id_str();
            // if (NULL != stored_uesr_id)
            {
                // if (0 == strncmp(stored_uesr_id, cj_user->valuestring, strlen(stored_uesr_id)))
                {
                    const char *password_saved = EZPI_core_factory_info_v3_get_local_key();
                    if (NULL != password_saved)
                    {
                        TRACE_D("password: %s", password_saved);
                        if (0 == strncmp(password_saved, cj_token->valuestring, strlen(password_saved)))
                        {
                            logged_in = true;
                        }
                        else
                        {
                            error = EZPI_ERR_INVALID_CREDENTIALS;
                        }
                        EZPI_core_factory_info_v3_free(password_saved);
                    }
                    else
                    {
                        error = EZPI_ERR_INVALID_CREDENTIALS;
                    }
                }
                // else
                // {
                //     error = EZPI_ERR_WRONG_PARAM;
                // }
                // ezlopi_free(__FUNCTION__, stored_uesr_id);
            }
        }
        else
        {
            error = EZPI_ERR_WRONG_PARAM;
        }
    }

    return error;
}

ezlopi_error_t EZPI_core_offline_logout_perform()
{
    ezlopi_error_t error = EZPI_FAILED;
    if (logged_in)
    {
        logged_in = false;
        error = EZPI_SUCCESS;
    }
    return error;
}

bool EZPI_core_offline_is_user_logged_in()
{
    return logged_in;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/