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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef _EZLOPI_CORE_SCENES_SCRIPTS_H_
#define _EZLOPI_CORE_SCENES_SCRIPTS_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <inttypes.h>

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
     * @brief Global function template example
     * Convention : Use capital letter for initial word on extern function
     * maincomponent : Main component as hal, core, service etc.
     * subcomponent : Sub component as i2c from hal, ble from service etc
     * functiontitle : Title of the function
     * eg : EZPI_hal_i2c_init()
     * @param arg
     *
     */
    void ezlopi_scenes_scripts_init(void);

    l_ezlopi_scenes_script_t *ezlopi_scenes_scripts_get_head(void);
    uint32_t ezlopi_scenes_scripts_add_to_head(uint32_t script_id, cJSON *cj_script);

    void ezlopi_scenes_scripts_update(cJSON *cj_script);

    void ezlopi_scenes_scripts_stop_by_id(uint32_t script_id);
    void ezlopi_scenes_scripts_stop(l_ezlopi_scenes_script_t *script_node);

    void ezlopi_scenes_scripts_run_by_id(uint32_t script_id);
    void ezlopi_scenes_scripts_delete_by_id(uint32_t script_id);

    void ezlopi_scenes_scripts_factory_info_reset(void);


#ifdef __cplusplus
}
#endif

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_SCRIPTS_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
