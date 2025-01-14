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
 * @file    ezlopi_cloud_favorite.c
 * @brief
 * @author
 * @version
 * @date
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <string.h>
#include "cjext.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_cloud_favorite.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"
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
void EZPI_favorite_list_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_devices_req = NULL;
    // cJSON *cj_items_req = NULL;
    // cJSON *cj_rules_req = NULL;

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    cJSON *cj_param = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);

    if (cj_param && cj_param->valuestring)
    {
        cJSON *cj_request_list = cJSON_GetObjectItem(__FUNCTION__, cj_param, "request");
        if (cj_request_list && cj_request_list->valuestring)
        {
            int array_size = cJSON_GetArraySize(cj_request_list);
            for (int i = 0; i < array_size; i++)
            {
                cJSON *elem = cJSON_GetArrayItem(cj_request_list, i);
                if (elem && elem->valuestring)
                {
                    if (strstr(ezlopi_devices_str, elem->valuestring))
                    {
                        cj_devices_req = elem;
                    }
                    else if (strstr(ezlopi_items_str, elem->valuestring))
                    {
                        // cj_items_req = elem;
                    }
                    else if (strstr("rules", elem->valuestring))
                    {
                        // cj_rules_req = elem;
                    }
                }
            }
        }
    }

    if (cj_result)
    {
        cJSON *cj_favorites = cJSON_AddObjectToObject(__FUNCTION__, cj_result, ezlopi_favorites_str);
        if (cj_favorites)
        {
            cJSON *cj_device_list = cj_devices_req ? cJSON_AddArrayToObject(__FUNCTION__, cj_favorites, ezlopi_devices_str) : NULL;
            // cJSON *cj_items_list = cj_items_req ? cJSON_AddArrayToObject(__FUNCTION__, cj_favorites, ezlopi_items_str) : NULL;
            // cJSON *cj_rules_list = cj_rules_req ? cJSON_AddArrayToObject(__FUNCTION__, cj_favorites, "rules") : NULL;

            l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
            while (curr_device)
            {
                l_ezlopi_item_t *curr_item = curr_device->items;
                while (curr_item)
                {
#warning "Needs works here [Krishna]"
                    cJSON *cj_device = cJSON_CreateObject(__FUNCTION__);
                    if (cj_device)
                    {
                        cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi__id_str, ezlopi__str);

                        if (!cJSON_AddItemToArray(cj_device_list, cj_device))
                        {
                            cJSON_Delete(__FUNCTION__, cj_device);
                        }
                    }
                    curr_item = curr_item->next;
                }

                curr_device = curr_device->next;
            }
        }
    }
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
