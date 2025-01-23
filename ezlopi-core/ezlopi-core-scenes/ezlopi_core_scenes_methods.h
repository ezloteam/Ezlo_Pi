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
 * @file    ezlopi_core_scenes_methods.h
 * @brief   perform some function on data
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */
#ifndef _EZLOPI_CORE_SCENES_METHODS_H_
#define _EZLOPI_CORE_SCENES_METHODS_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <string.h>

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
    typedef enum e_scenes_method_category
    {
        METHOD_CATEGORY_NAN = 0,
        METHOD_CATEGORY_WHEN_TIME,
        METHOD_CATEGORY_WHEN_MODES,
        METHOD_CATEGORY_WHEN_LOGIC,
        METHOD_CATEGORY_WHEN_FUNCTION,
        METHOD_CATEGORY_UNDEFINED,
        METHOD_CATEGORY_MAX
    } e_scenes_method_category_t;

    typedef enum e_scene_method_type
    {
#define EZLOPI_SCENE(method, name, func, category) EZLOPI_SCENE_##method,
#include "ezlopi_core_scenes_method_types.h"
#undef EZLOPI_SCENE
    } e_scene_method_type_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Returns the enum equivalent of scene method with 'method_name'
     *
     * @param method_name Name of the scene-method
     * @return e_scene_method_type_t
     */
    e_scene_method_type_t EZPI_scenes_method_get_type_enum(char *method_name);
    /**
     * @brief Return "method_name" of the corresponding 'enum_method_type'
     *
     * @param method_type 'enum_method_type' of target scene-method
     * @return const char*
     */
    const char *EZPI_scene_get_scene_method_name(e_scene_method_type_t method_type);
    /**
     * @brief Returns the 'category-enum' equivalent of scene method with 'method_name'
     *
     * @param method_name Name of the scene-method whose category is to be determined
     * @return e_scenes_method_category_t
     */
    e_scenes_method_category_t EZPI_scene_get_scene_method_category_enum(char *method_name);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_METHODS_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/

/* then methods */
