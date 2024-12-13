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
* @file    ezlopi_core_scenes_delete.c
* @brief   This files include functions that delete scence-info or scene itself
* @author  xx
* @version 0.1
* @date    12th DEC 2024
*/

/*******************************************************************************
*                          Include Files
*******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <string.h>

#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_delete.h"

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
void EZPI_scenes_delete_user_notifications(l_user_notification_v2_t *user_notifications)
{
    if (user_notifications)
    {
        EZPI_scenes_delete_user_notifications(user_notifications->next);
        user_notifications->next = NULL;
        ezlopi_free(__FUNCTION__, user_notifications);
    }
}

void EZPI_scenes_delete_house_modes(l_house_modes_v2_t *house_modes)
{
    if (house_modes)
    {
        EZPI_scenes_delete_house_modes(house_modes->next);
        house_modes->next = NULL;
        ezlopi_free(__FUNCTION__, house_modes);
    }
}

void EZPI_scenes_delete_fields(l_fields_v2_t *fields)
{
    if (fields)
    {
        EZPI_scenes_delete_fields(fields->next);
        if (fields->user_arg)
        {
            free(fields->user_arg);
        }
        fields->next = NULL;
        EZPI_scenes_delete_field_value(fields);
        ezlopi_free(__FUNCTION__, fields);
    }
}

void EZPI_scenes_delete_action_blocks(l_action_block_v2_t *action_blocks)
{
    if (action_blocks)
    {
        if (NULL != action_blocks->block_options.cj_function)
        {
            cJSON_Delete(__FUNCTION__, action_blocks->block_options.cj_function);
        }
        EZPI_scenes_delete_fields(action_blocks->fields);
        EZPI_scenes_delete_action_blocks(action_blocks->next);
        action_blocks->next = NULL;
        ezlopi_free(__FUNCTION__, action_blocks);
    }
}

void EZPI_scenes_delete_when_blocks(l_when_block_v2_t *when_blocks)
{
    if (when_blocks)
    {
        if (NULL != when_blocks->cj_block_meta)
        {
            cJSON_Delete(__FUNCTION__, when_blocks->cj_block_meta);
        }

        if (NULL != when_blocks->block_options.cj_function)
        {
            cJSON_Delete(__FUNCTION__, when_blocks->block_options.cj_function);
        }

        if (NULL != when_blocks->when_grp)
        {
            ezlopi_free(__FUNCTION__, when_blocks->when_grp);
            when_blocks->when_grp = NULL;
        }
        EZPI_scenes_delete_fields(when_blocks->fields);
        EZPI_scenes_delete_when_blocks(when_blocks->next);
        when_blocks->next = NULL;
        ezlopi_free(__FUNCTION__, when_blocks);
    }
}

void EZPI_scenes_delete(l_scenes_list_v2_t *scenes_list)
{
    if (scenes_list)
    {
        EZPI_scenes_delete_user_notifications(scenes_list->user_notifications);
        EZPI_scenes_delete_house_modes(scenes_list->house_modes);
        EZPI_scenes_delete_action_blocks(scenes_list->then_block);
        EZPI_scenes_delete_action_blocks(scenes_list->else_block);
        EZPI_scenes_delete_when_blocks(scenes_list->when_block);

        if (NULL != scenes_list->thread_ctx)
        {
            ezlopi_free(__FUNCTION__, scenes_list->thread_ctx);
            scenes_list->thread_ctx = NULL;
        }

        if (NULL != scenes_list->meta)
        {
            cJSON_Delete(__FUNCTION__, scenes_list->meta);
            scenes_list->meta = NULL;
        }

        EZPI_scenes_delete(scenes_list->next);
        scenes_list->next = NULL;
        ezlopi_free(__FUNCTION__, scenes_list);
    }
}

void EZPI_scenes_delete_field_value(l_fields_v2_t *field)
{
    switch (field->field_value.e_type)
    {
    case VALUE_TYPE_NUMBER:
    {
        field->field_value.u_value.value_double = 0;
        break;
    }
    case VALUE_TYPE_STRING:
    {
        if (field->field_value.u_value.value_string)
        {
            ezlopi_free(__FUNCTION__, field->field_value.u_value.value_string);
            field->field_value.u_value.value_string = NULL;
        }
        break;
    }
    case VALUE_TYPE_BOOL:
    {
        field->field_value.u_value.value_bool = false;
        break;
    }
    case VALUE_TYPE_CJSON:
    {
        if (field->field_value.u_value.cj_value)
        {
            cJSON_Delete(__FUNCTION__, field->field_value.u_value.cj_value);
            field->field_value.u_value.cj_value = NULL;
        }
        break;
    }
    case VALUE_TYPE_BLOCK:
    {
        if (field->field_value.u_value.when_block)
        {
            EZPI_scenes_delete_when_blocks(field->field_value.u_value.when_block);
            field->field_value.u_value.when_block = NULL;
        }
        break;
    }
    case VALUE_TYPE_HOUSE_MODE_ID_ARRAY:
    {
        if (field->field_value.u_value.house_modes)
        {
            TRACE_S("here : free house_mode_arr");
            EZPI_scenes_delete_house_modes(field->field_value.u_value.house_modes);
            field->field_value.u_value.house_modes = NULL;
        }
        break;
    }
    default:
    {
        break;
    }
    }
}

#if 0
// void ezlopi_scenes_delete_by_id(uint32_t _id)
// {
//     EZPI_scenes_delete(ezlopi_scenes_pop_by_id_v2(_id)); /*THIS 'ezlopi_scenes_pop_by_id_v2' creates linking issue*/
// }
#endif
/*******************************************************************************
*                         Static Function Definitions
*******************************************************************************/

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS
/*******************************************************************************
*                          End of File
*******************************************************************************/
