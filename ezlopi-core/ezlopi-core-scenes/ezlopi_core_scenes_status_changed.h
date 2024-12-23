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
 * @file    ezlopi_core_scenes_status_changed.h
 * @brief   Function to broadcast scene_status info
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
*/

#ifndef _EZLOPI_CORE_SCENES_STATUS_CHANGED_H_
#define _EZLOPI_CORE_SCENES_STATUS_CHANGED_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS


/*******************************************************************************
*                          Include Files
*******************************************************************************/
#include "ezlopi_core_scenes_v2.h"

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

    /*******************************************************************************
    *                          Extern Data Declarations
    *******************************************************************************/

    /*******************************************************************************
    *                          Extern Function Prototypes
    *******************************************************************************/
    /**
     * @brief This function returns 'scene_status' in string literal
     *
     * @param scene_status
     * @return const char*
     */
    const char *EZPI_core_scenes_status_to_string(e_scene_status_v2_t scene_status);
    /**
     * @brief This funtion broadcast changes to 'scene_status' of a scene
     *
     * @param scene_node Target scene_node
     * @param status_str The status to broadcast
     * @return int
     */
    int EZPI_core_scenes_status_change_broadcast(l_scenes_list_v2_t *scene_node, const char *status_str);

#ifdef __cplusplus
}
#endif


#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_STATUS_CHANGED_H_

/*******************************************************************************
*                          End of File
*******************************************************************************/

