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
 * @file    ezlopi_cloud_scenes_expressions.h
 * @brief   Decleration for cloud scenes expressions functions
 * @author  ezlopi_team_np
 * @version 1.0
 * @date    January 20, 2024
 */

#ifndef __SCENES_EXPRESSIONS_H__
#define __SCENES_EXPRESSIONS_H__

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "cjext.h"

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
     * @brief Function that responds to hub.scenes.expressions.list.add method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_expressions_list(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.script.set method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_expressions_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.script.delete method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_expressions_delete(cJSON *cj_request, cJSON *cj_response);

    ///////////// updaters
    /**
     * @brief Updater function with method hub.expression.changed
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_expressions_added_changed(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function with method hub.expression.deleted
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_expressions_deleted(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // __SCENES_EXPRESSIONS_H__
