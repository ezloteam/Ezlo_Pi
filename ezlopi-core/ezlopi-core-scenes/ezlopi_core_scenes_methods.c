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
 * @file    ezlopi_core_scenes_methods.c
 * @brief   This file contains function to return scene-methods
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "ezlopi_util_trace.h"

// #include "ezlopi_core_scenes_methods.h"
#include "ezlopi_core_scenes_v2.h"

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
static const char *ezlopi_scenes_methods_name[] = {
#define EZLOPI_SCENE(method_type, name, func, category) name,
#include "ezlopi_core_scenes_method_types.h"
#undef EZLOPI_SCENE
};

static e_scenes_method_category_t ezlopi_scenes_method_category_enum[] = {
#define EZLOPI_SCENE(method_type, name, func, category) category,
#include "ezlopi_core_scenes_method_types.h"
#undef EZLOPI_SCENE
};

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

const char *EZPI_scene_get_scene_method_name(e_scene_method_type_t method_type)
{
    const char *ret = NULL;
    if ((method_type > EZLOPI_SCENE_METHOD_TYPE_NONE) && (method_type < EZLOPI_SCENE_METHOD_TYPE_MAX))
    {
        ret = ezlopi_scenes_methods_name[method_type];
    }
    return ret;
}

e_scene_method_type_t EZPI_scenes_method_get_type_enum(char *method_name)
{
    e_scene_method_type_t method_type = EZLOPI_SCENE_METHOD_TYPE_NONE;
    if (method_name)
    {
        size_t method_len = strlen(method_name);
        for (e_scene_method_type_t i = EZLOPI_SCENE_WHEN_METHOD_IS_ITEM_STATE; i < EZLOPI_SCENE_METHOD_TYPE_MAX; i++)
        {
            size_t max_len = (method_len > strlen(ezlopi_scenes_methods_name[i])) ? method_len : strlen(ezlopi_scenes_methods_name[i]);
            if (0 == strncmp(method_name, ezlopi_scenes_methods_name[i], max_len))
            {
                method_type = i;
                break;
            }
        }
    }

    return method_type;
}

e_scenes_method_category_t EZPI_scene_get_scene_method_category_enum(char *method_name)
{
    e_scenes_method_category_t ret = METHOD_CATEGORY_NAN;
    if (method_name)
    {
        e_scene_method_type_t method_type = EZPI_scenes_method_get_type_enum(method_name);

        if ((method_type > EZLOPI_SCENE_METHOD_TYPE_NONE) && (method_type < EZLOPI_SCENE_METHOD_TYPE_MAX))
        {
            ret = ezlopi_scenes_method_category_enum[method_type];
        }
    }
    return ret;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS
/*******************************************************************************
 *                          End of File
 *******************************************************************************/
