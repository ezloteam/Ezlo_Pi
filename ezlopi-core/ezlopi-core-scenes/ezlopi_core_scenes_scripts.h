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
 * @file    ezlopi_core_scenes_scripts.h
 * @brief   Perform operations on Scene-scipts
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
*/

#ifndef _EZLOPI_CORE_SCENES_SCRIPTS_H_
#define _EZLOPI_CORE_SCENES_SCRIPTS_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
*                          Include Files
*******************************************************************************/
#include <inttypes.h>
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
    typedef enum e_script_state
    {
        SCRIPT_STATE_NONE = 0,
        SCRIPT_STATE_RUNNING,
        SCRIPT_STATE_NOT_RUNNING,
        SCRIPT_STATE_PAUSED,
        SCRIPT_STATE_DELETE,
    } e_script_state_t;

    typedef struct l_ezlopi_scenes_script
    {
        uint32_t id;
        char name[32];
        char *code;
        e_script_state_t state;
        void *script_handle;

        struct l_ezlopi_scenes_script *next;
    } l_ezlopi_scenes_script_t;

    /*******************************************************************************
    *                          Extern Data Declarations
    *******************************************************************************/

    /*******************************************************************************
    *                          Extern Function Prototypes
    *******************************************************************************/
    /**
     * @brief This function initializes the scenes scripts
     */
    void EZPI_scenes_scripts_init(void);
    /**
     * @brief This function returns scene-script 'head-node'
     *
     * @return l_ezlopi_scenes_script_t*
     */
    l_ezlopi_scenes_script_t *EZPI_scenes_scripts_get_head(void);
    /**
     * @brief This function adds a node to scene-script
     *
     * @param script_id New script_id
     * @param cj_script Scource of the new-script information
     * @return uint32_t
     */
    uint32_t EZPI_scenes_scripts_add_to_head(uint32_t script_id, cJSON *cj_script);
    /**
     * @brief This function updates a existing node
     *
     * @param cj_script Scource of the new-script information
     */
    void EZPI_scenes_scripts_update(cJSON *cj_script);
    /**
     * @brief This function halts running scripts by id
     *
     * @param script_id Id of target script
     */
    void EZPI_scenes_scripts_stop_by_id(uint32_t script_id);
    /**
     * @brief This function halts running scripts by node
     *
     * @param script_node Target script node
     */
    void EZPI_scenes_scripts_stop(l_ezlopi_scenes_script_t *script_node);
    /**
     * @brief This function triggers activation of perticular script
     *
     * @param script_id Id of target script
     */
    void EZPI_scenes_scripts_run_by_id(uint32_t script_id);
    /**
     * @brief This function completely removes a perticular script from ll
     *
     * @param script_id Id of target script
     */
    void EZPI_scenes_scripts_delete_by_id(uint32_t script_id);
    /**
     * @brief Clear the script factory info
     *
     */
    void EZPI_scenes_scripts_factory_info_reset(void);

#ifdef __cplusplus
}
#endif

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_SCRIPTS_H_

/*******************************************************************************
*                          End of File
*******************************************************************************/






