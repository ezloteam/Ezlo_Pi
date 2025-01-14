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
 * @file    ezlopi_core_scenes_value.c
 * @brief   Function to operate on scene-value-types
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_value.h"

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
static const char *sg_scenes_value_type_name[] = {
#define EZLOPI_VALUE_TYPE(type, name) name,
#include "ezlopi_core_scenes_value_types.h"
#undef EZLOPI_VALUE_TYPE
};

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

const char *EZPI_core_scenes_get_scene_value_type_name(e_scene_value_type_v2_t value_type)
{
    const char *ret = ezlopi__str;
    if ((value_type >= EZLOPI_VALUE_TYPE_NONE) && (value_type < EZLOPI_VALUE_TYPE_MAX))
    {
        ret = sg_scenes_value_type_name[value_type];
    }

    return ret;
}

e_scene_value_type_v2_t EZPI_core_scene_get_scene_value_type_enum(const char *value_type_str)
{
    e_scene_value_type_v2_t ret = EZLOPI_VALUE_TYPE_NONE;
    if (value_type_str)
    {
        for (int i = EZLOPI_VALUE_TYPE_NONE; i < EZLOPI_VALUE_TYPE_MAX; i++)
        {
            if (EZPI_STRNCMP_IF_EQUAL(sg_scenes_value_type_name[i], value_type_str, strlen(sg_scenes_value_type_name[i]) + 1, strlen(value_type_str) + 1))
            {
                ret = i;
                break;
            }
        }
    }
    return ret;
}

e_scene_value_type_v2_t EZPI_core_scenes_value_get_type(cJSON *cj_root, const char *type_key_str)
{
    char *value_type_str = NULL;
    e_scene_value_type_v2_t ret = EZLOPI_VALUE_TYPE_NONE;

    if (cj_root && (NULL == cj_root->valuestring))
    {
        cJSON *cj_value_type = cJSON_GetObjectItem(__FUNCTION__, cj_root, type_key_str);
        if (cj_value_type && cj_value_type->valuestring)
        {
            value_type_str = cj_value_type->valuestring;
        }
    }
    else if ((cj_root->type == cJSON_String) && (cj_root->valuestring))
    {
        value_type_str = cj_root->valuestring;
    }

    ret = EZPI_core_scene_get_scene_value_type_enum(value_type_str);

    return ret;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          End of File
 *******************************************************************************/