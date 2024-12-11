/**
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
*/

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

#ifndef _EZLOPI_CORE_SCENES_CJSON_H_
#define _EZLOPI_CORE_SCENES_CJSON_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
*                          Include Files
*******************************************************************************/
/**
* Include Sequence
* 1. C standard libraries eg. stdint, string etc.
* 2. Third party component eg. cJSON, freeRTOS etc etc
* 3. ESP-IDF specific eg. driver/gpio, esp_log etc.
* 4. EzloPi HAL specific eg ezlopi_hal_i2c
* 5. EzloPi core specific eg. ezlopi_core_ble
* 6. EzloPi cloud specific eg. ezlopi_cloud_items
* 7. EzloPi Service specific eg ezlopi_service_ble
* 8. EzloPi Sensors and Device Specific eg. ezlopi_device_0001_digitalout_generic
*/
#include "cjext.h"
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
    * @brief Global function template example
    * Convention : Use capital letter for initial word on extern function
    * maincomponent : Main component as hal, core, service etc.
    * subcomponent : Sub component as i2c from hal, ble from service etc
    * functiontitle : EZLOPI_scene_cjson_get_field()
    * @param arg
    *
    */
    cJSON *EZLOPI_scene_cjson_get_field(l_fields_v2_t *field_node);
    cJSON *EZLOPI_scenes_cjson_create_action_block(l_action_block_v2_t *action_block, char *block_type_str);
    cJSON *EZLOPI_scenes_cjson_create_when_block(l_when_block_v2_t *when_block);
    cJSON *EZLOPI_scenes_create_cjson_scene(l_scenes_list_v2_t *scene);
    cJSON *EZLOPI_scenes_create_cjson_scene_list(l_scenes_list_v2_t *scenes_list);
    void EZLOPI_scenes_cjson_add_action_blocks(cJSON *root, l_action_block_v2_t *action_blocks, const char *block_type_str);
    void EZLOPI_scenes_cjson_add_when_blocks(cJSON *root, l_when_block_v2_t *when_blocks);


#ifdef __cplusplus
}
#endif

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_CJSON_H_

/*******************************************************************************
*                          End of File
 *******************************************************************************/