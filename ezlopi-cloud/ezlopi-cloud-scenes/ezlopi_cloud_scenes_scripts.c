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
 * @file    ezlopi_cloud_scenes_scripts.c
 * @brief   Definitions for cloud scenes scripts functions
 * @author  ezlopi_team_np
 * @version 1.0
 * @date    January 20, 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <stdlib.h>

#include "ezlopi_core_scenes_scripts.h"

#include "ezlopi_cloud_scenes_scripts.h"
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

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_scenes_scripts_list(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddArrayToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            l_ezlopi_scenes_script_t *script_nodes = EZPI_scenes_scripts_get_head();
            while (script_nodes)
            {
                cJSON *cj_script = cJSON_CreateObject(__FUNCTION__);
                if (cj_script)
                {
                    char script_id_str[32];
                    snprintf(script_id_str, sizeof(script_id_str), "%08x", script_nodes->id);
                    cJSON_AddStringToObject(__FUNCTION__, cj_script, ezlopi__id_str, script_id_str);
                    cJSON_AddStringToObject(__FUNCTION__, cj_script, ezlopi_name_str, script_nodes->name);

                    if (!cJSON_AddItemToArray(cj_result, cj_script))
                    {
                        cJSON_Delete(__FUNCTION__, cj_script);
                    }
                }

                script_nodes = script_nodes->next;
            }
        }
    }
}

void EZPI_scenes_scripts_get(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            uint32_t script_id_num = 0;
            cJSON *cj_script_id = NULL;
            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                cj_script_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
                if (cj_script_id && cj_script_id->valuestring)
                {
                    script_id_num = strtoul(cj_script_id->valuestring, NULL, 16);
                }
            }

            if (script_id_num)
            {
                l_ezlopi_scenes_script_t *script_nodes = EZPI_scenes_scripts_get_head();
                while (script_nodes)
                {
                    if (script_id_num == script_nodes->id)
                    {
                        cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi__id_str, cJSON_Duplicate(__FUNCTION__, cj_script_id, true));
                        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_name_str, script_nodes->name);
                        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_code_str, script_nodes->code);

                        break;
                    }

                    script_nodes = script_nodes->next;
                }
            }
        }
    }
}

void EZPI_scenes_scripts_add(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        uint32_t script_id = EZPI_scenes_scripts_add_to_head(0, cj_params);

        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            if (script_id)
            {
                char tmp_id_str[32];
                snprintf(tmp_id_str, sizeof(tmp_id_str), "%08x", script_id);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi__id_str, tmp_id_str);
            }
        }
    }
}

void EZPI_scenes_scripts_delete(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

        uint32_t script_id_num = 0;
        cJSON *cj_script_id = NULL;

        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cj_script_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
            if (cj_script_id && cj_script_id->valuestring)
            {
                script_id_num = strtoul(cj_script_id->valuestring, NULL, 16);
                EZPI_scenes_scripts_stop_by_id(script_id_num);
                EZPI_scenes_scripts_delete_by_id(script_id_num);
            }
        }
    }
}

void EZPI_scenes_scripts_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        EZPI_scenes_scripts_update(cj_params);
    }
}

void EZPI_scenes_scripts_run(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_script_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
        if (cj_script_id && cj_script_id->valuestring)
        {
            uint32_t script_id = strtoul(cj_script_id->valuestring, NULL, 16);
            if (script_id)
            {
                EZPI_scenes_scripts_run_by_id(script_id);
            }
        }
    }
}
/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/
#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
