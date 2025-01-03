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
 * @file    ezlopi_core_scenes_print.c
 * @brief   These function print scene information
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS
#include "ezlopi_core_scenes_print.h"

#if ENABLE_SCENES_PRINT

// #include "ezlopi_util_trace.h"

// #include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_value.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_constants.h"

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
void EZPI_print_block_options(s_block_options_v2_t *block_options, l_fields_v2_t *fields, const char *tab)
{
#if (1 == ENABLE_TRACE)
    TRACE_D("%s\t\t|-- blockOptions:", (NULL != tab ? tab : ""));
    TRACE_D("%s\t\t\t|-- method", (NULL != tab ? tab : ""));
    TRACE_D("%s\t\t\t\t|-- name: %s", (NULL != tab ? tab : ""), block_options->method.name);
    TRACE_D("%s\t\t\t\t|-- args:", (NULL != tab ? tab : ""));

    while (fields)
    {
        TRACE_D("%s\t\t\t\t\t|-- %s: %s", (NULL != tab ? tab : ""), fields->name, fields->name);
        fields = fields->next;
    }

    if (0 == strncmp(ezlopi_function_str, block_options->method.name, 9) && (NULL != block_options->cj_function))
    {
        TRACE_D("%s\t\t\t|-- function", (NULL != tab ? tab : ""));
        const char *name = "\t\t\t\t|--";
        char *obj_str = cJSON_Print(__FUNCTION__, block_options->cj_function);
        if (obj_str)
        {
            TRACE_D("%s%s[%d]: %s", (NULL != tab ? tab : ""), (name ? name : ezlopi__str), strlen(obj_str), obj_str);
            ezlopi_free(__FUNCTION__, obj_str);
        }
        else
        {
            TRACE_E("%s\t\t|--%s: Null", (NULL != tab ? tab : ""), (name ? name : ""));
        }
    }

#endif
}

void EZPI_print_fields(l_fields_v2_t *fields, const char *tab)
{
#if (1 == ENABLE_TRACE)
    static uint8_t block_tab_count = 0; // this is only used here ---> // to increase block-tabs
    TRACE_D("%s\t\t|-- fields: ", (NULL != tab ? tab : ""));
    int field_count = 0;
    while (fields)
    {
        TRACE_D("%s\t\t\t|---------- field_count: %d ----------", (NULL != tab ? tab : ""), ++field_count);
        TRACE_D("%s\t\t\t|-- name: %s", (NULL != tab ? tab : ""), fields->name);

        const char *value_type_name = EZPI_core_scenes_get_scene_value_type_name(fields->value_type);
        TRACE_D("%s\t\t\t|-- type: %s", (NULL != tab ? tab : ""), value_type_name ? value_type_name : ezlopi__str);

        switch (fields->value_type)
        {
        case EZLOPI_VALUE_TYPE_INT:
            /*case EZLOPI_VALUE_TYPE_HOUSE_MODE_ID:*/
        case EZLOPI_VALUE_TYPE_TEMPERATURE:
        case EZLOPI_VALUE_TYPE_HUMIDITY:
        case EZLOPI_VALUE_TYPE_VELOCITY:
        case EZLOPI_VALUE_TYPE_DIRECTION:
        case EZLOPI_VALUE_TYPE_GENERAL_PURPOSE:
        case EZLOPI_VALUE_TYPE_ACIDITY:
        case EZLOPI_VALUE_TYPE_ELECTRIC_POTENTIAL:
        case EZLOPI_VALUE_TYPE_ELECTRIC_CURRENT:
        case EZLOPI_VALUE_TYPE_FORCE:
        case EZLOPI_VALUE_TYPE_IRRADIANCE:
        case EZLOPI_VALUE_TYPE_PRECIPITATION:
        case EZLOPI_VALUE_TYPE_LENGTH:
        case EZLOPI_VALUE_TYPE_MASS:
        case EZLOPI_VALUE_TYPE_VOLUME_FLOW:
        case EZLOPI_VALUE_TYPE_VOLUME:
        case EZLOPI_VALUE_TYPE_ANGLE:
        case EZLOPI_VALUE_TYPE_FREQUENCY:
        case EZLOPI_VALUE_TYPE_SEISMIC_INTENSITY:
        case EZLOPI_VALUE_TYPE_SEISMIC_MAGNITUDE:
        case EZLOPI_VALUE_TYPE_ULTRAVIOLET:
        case EZLOPI_VALUE_TYPE_ELECTRICAL_RESISTANCE:
        case EZLOPI_VALUE_TYPE_ELECTRICAL_CONDUCTIVITY:
        case EZLOPI_VALUE_TYPE_LOUDNESS:
        case EZLOPI_VALUE_TYPE_MOISTURE:
        case EZLOPI_VALUE_TYPE_TIME:
        case EZLOPI_VALUE_TYPE_RADON_CONCENTRATION:
        case EZLOPI_VALUE_TYPE_BLOOD_PRESSURE:
        case EZLOPI_VALUE_TYPE_ENERGY:
        case EZLOPI_VALUE_TYPE_RF_SIGNAL_STRENGTH:
        case EZLOPI_VALUE_TYPE_KILO_VOLT_AMPERE_HOUR:
        case EZLOPI_VALUE_TYPE_REACTIVE_POWER_INSTANT:
        case EZLOPI_VALUE_TYPE_AMOUNT_OF_USEFUL_ENERGY:
        case EZLOPI_VALUE_TYPE_REACTIVE_POWER_CONSUMPTION:
        {
            TRACE_D("%s\t\t\t|-- value: %d", (NULL != tab ? tab : ""), (int)fields->field_value.u_value.value_double);
            break;
        }
        case EZLOPI_VALUE_TYPE_BOOL:
        {
            TRACE_D("%s\t\t\t|-- value: [%d]%s", (NULL != tab ? tab : ""), fields->field_value.u_value.value_bool, fields->field_value.u_value.value_bool ? ezlopi_true_str : ezlopi_false_str);
            break;
        }
        case EZLOPI_VALUE_TYPE_FLOAT:
        case EZLOPI_VALUE_TYPE_ILLUMINANCE:
        case EZLOPI_VALUE_TYPE_PRESSURE:
        case EZLOPI_VALUE_TYPE_SUBSTANCE_AMOUNT:
        case EZLOPI_VALUE_TYPE_POWER:
        case EZLOPI_VALUE_TYPE_ACCELERATION:
        {
            TRACE_D("%s\t\t\t|-- value: %f", (NULL != tab ? tab : ""), fields->field_value.u_value.value_double);
            break;
        }
        case EZLOPI_VALUE_TYPE_STRING:
        case EZLOPI_VALUE_TYPE_ITEM:
        case EZLOPI_VALUE_TYPE_DEVICE:
        case EZLOPI_VALUE_TYPE_24_HOURS_TIME:
        case EZLOPI_VALUE_TYPE_INTERVAL:
        case EZLOPI_VALUE_TYPE_HMS_INTERVAL:
        case EZLOPI_VALUE_TYPE_SCENEID:
        case EZLOPI_VALUE_TYPE_EXPRESSION:
        case EZLOPI_VALUE_TYPE_HOUSE_MODE_ID:
        {
            TRACE_D("%s\t\t\t|-- value: %s", (NULL != tab ? tab : ""), fields->field_value.u_value.value_string);
            break;
        }
        case EZLOPI_VALUE_TYPE_BLOCK:
        case EZLOPI_VALUE_TYPE_BLOCKS:
        {
            TRACE_D("%s\t\t\t|-- value:", (NULL != tab ? tab : ""));
            // TRACE_S("%s\t\t\t|-- block_tab_count : [%d]", (NULL != tab ? tab : ""), block_tab_count);

            char append_tab[32] = {
                '\t',
                '\t',
                '\t',
            };
            if (tab)
            {
                size_t tot_len = ((strlen(append_tab) + strlen(tab)) < sizeof(append_tab) ? (strlen(append_tab) + strlen(tab)) : 31);
                memset(append_tab, '\t', tot_len);
            }
            append_tab[31] = '\0';

            block_tab_count++;
            EZPI_print_when_blocks((l_when_block_v2_t *)fields->field_value.u_value.when_block, append_tab);
            if (block_tab_count > 0)
            {
                block_tab_count--;
            }

            break;
        }
        case EZLOPI_VALUE_TYPE_CREDENTIAL:
        case EZLOPI_VALUE_TYPE_DICTIONARY:
        case EZLOPI_VALUE_TYPE_WEEKLY_INTERVAL:
        case EZLOPI_VALUE_TYPE_DAILY_INTERVAL:
        case EZLOPI_VALUE_TYPE_ARRAY:
        case EZLOPI_VALUE_TYPE_24_HOURS_TIME_ARRAY:
        case EZLOPI_VALUE_TYPE_INT_ARRAY:
        case EZLOPI_VALUE_TYPE_HOUSE_MODE_ID_ARRAY:
        case EZLOPI_VALUE_TYPE_RGB:
        case EZLOPI_VALUE_TYPE_OBJECT:
        {
            const char *name = "\t\t\t|-- value";
            char *obj_str = cJSON_Print(__FUNCTION__, fields->field_value.u_value.cj_value);
            if (obj_str)
            {
                TRACE_D("%s%s[%d]: %s", (NULL != tab ? tab : ""), (name ? name : ezlopi__str), strlen(obj_str), obj_str);
                ezlopi_free(__FUNCTION__, obj_str);
            }
            else
            {
                TRACE_E("%s\t\t|--%s: Null", (NULL != tab ? tab : ""), (name ? name : ""));
            }

            break;
        }
        case EZLOPI_VALUE_TYPE_ENUM:
        case EZLOPI_VALUE_TYPE_TOKEN:
        {
            if (VALUE_TYPE_STRING == fields->field_value.e_type)
            {
                TRACE_D("%s\t\t\t|-- value: %s", (NULL != tab ? tab : ""), fields->field_value.u_value.value_string);
            }
            else if (VALUE_TYPE_CJSON == fields->field_value.e_type)
            {
                const char *name = "\t\t\t|-- value";
                char *obj_str = cJSON_Print(__FUNCTION__, fields->field_value.u_value.cj_value);
                if (obj_str)
                {
                    TRACE_D("%s%s[%d]: %s", (NULL != tab ? tab : ""), (name ? name : ezlopi__str), strlen(obj_str), obj_str);
                    ezlopi_free(__FUNCTION__, obj_str);
                }
                else
                {
                    TRACE_E("%s\t\t|--%s: Null", (NULL != tab ? tab : ""), (name ? name : ""));
                }
            }
            else
            {
                TRACE_W("Value type not Implemented!, value_type: %d", fields->value_type);
            }
            break;
        }
        case EZLOPI_VALUE_TYPE_CAMERA_STREAM:
        case EZLOPI_VALUE_TYPE_USER_CODE:
        case EZLOPI_VALUE_TYPE_BUTTON_STATE:
        case EZLOPI_VALUE_TYPE_USER_LOCK_OPERATION:
        case EZLOPI_VALUE_TYPE_USER_CODE_ACTION:
        case EZLOPI_VALUE_TYPE_SOUND_INFO:
        case EZLOPI_VALUE_TYPE_CAMERA_HOTZONE:
        case EZLOPI_VALUE_TYPE_HOTZONE_MATCH:
        case EZLOPI_VALUE_TYPE_GEOFENCE:
        case EZLOPI_VALUE_TYPE_NONE:
        case EZLOPI_VALUE_TYPE_MAX:
        {
            TRACE_W("Value type not Implemented!, value_type: %d", fields->value_type);
            break;
        }
        default:
        {
            TRACE_E("Value type not matched!");
            break;
        }
        }

        fields = fields->next;
    }
    TRACE_D("%s\t\t\t|------------------------------------", (NULL != tab ? tab : ""));
#endif
}

void EZPI_print_house_modes(l_house_modes_v2_t *house_modes)
{
#if (1 == ENABLE_TRACE)
    TRACE_D("\t|-- house_modes: ");
    while (house_modes)
    {
        TRACE_D("\t\t|-- %s", house_modes->house_mode);
        house_modes = house_modes->next;
    }
#endif
}

void EZPI_print_user_notifications(l_user_notification_v2_t *user_notification)
{
#if (1 == ENABLE_TRACE)
    TRACE_D("\t|-- user_notifications: ");
    while (user_notification)
    {
        TRACE_D("\t\t|-- %s", user_notification->user_id);
        user_notification = user_notification->next;
    }
#endif
}

void EZPI_print_when_blocks(l_when_block_v2_t *when_blocks, const char *tab)
{
#if (1 == ENABLE_TRACE)
    TRACE_D("%s\t|-- when: ", (NULL != tab ? tab : ""));
    while (when_blocks)
    {
        if (0 < when_blocks->blockId)
        {
            TRACE_D("%s\t\t|-- block_enable: %d", (NULL != tab ? tab : ""), when_blocks->block_enable);
            TRACE_D("%s\t\t|-- blockId: %08x", (NULL != tab ? tab : ""), when_blocks->blockId);
        }

        if (when_blocks->when_grp->grp_blockName && (0 < strlen(when_blocks->when_grp->grp_blockName)))
        {
            TRACE_S("%s\t\t|-- group_blockName: %s", (NULL != tab ? tab : ""), when_blocks->when_grp->grp_blockName);
            TRACE_S("%s\t\t|-- group_blockId: %08x", (NULL != tab ? tab : ""), when_blocks->when_grp->grp_id);
        }

        TRACE_D("%s\t\t|-- block_status_reset_once: %d", (NULL != tab ? tab : ""), when_blocks->block_status_reset_once);

        {
            const char *name = "\t\t|-- block_meta:";
            char *obj_str = cJSON_Print(__FUNCTION__, when_blocks->cj_block_meta);
            if (obj_str)
            {
                TRACE_D("%s%s[%d]: %s", (NULL != tab ? tab : ""), (name ? name : ezlopi__str), strlen(obj_str), obj_str);
                ezlopi_free(__FUNCTION__, obj_str);
            }
            else
            {
                TRACE_E("%s%s: Null", (NULL != tab ? tab : ""), (name ? name : ""));
            }
        }

        EZPI_print_block_options(&when_blocks->block_options, when_blocks->fields, tab);
        TRACE_D("%s\t\t|-- blockType: when", (NULL != tab ? tab : ""));
        EZPI_print_fields(when_blocks->fields, tab);
        when_blocks = when_blocks->next;
    }
#endif
}

void EZPI_print_action_blocks(l_action_block_v2_t *action_block)
{
#if (1 == ENABLE_TRACE)
    while (action_block)
    {
        TRACE_D("\t|-- %s: ", (SCENE_BLOCK_TYPE_THEN == action_block->block_type) ? ezlopi_then_str : ezlopi_else_str);
        EZPI_print_block_options(&action_block->block_options, action_block->fields, NULL);
        TRACE_D("\t\t|-- blockType: then");
        TRACE_D("\t\t|-- _tempId: %.*s", sizeof(action_block->_tempId), action_block->_tempId);

        TRACE_D("\t\t|-- Delay:: days: %d, hours: %d, minutes: %d, seconds: %d", action_block->delay.days, action_block->delay.hours, action_block->delay.minutes, action_block->delay.seconds);

        EZPI_print_fields(action_block->fields, NULL);

        action_block = action_block->next;
        if (action_block)
        {
            TRACE_D("\t\t|--");
        }
    }
#endif
}

void EZPI_scenes_print(l_scenes_list_v2_t *scene_link_list)
{
#if (1 == ENABLE_TRACE)
    int scene_count = 0;
    while (scene_link_list)
    {
        TRACE_D("\t----------------------- scene_count: %d ------------------------", ++scene_count);
        TRACE_D("\t|-- id: 0x%08x", scene_link_list->_id);
        TRACE_D("\t|-- enabled: %d", scene_link_list->enabled);
        TRACE_D("\t|-- is_group: %d", scene_link_list->is_group);
        TRACE_D("\t|-- group_id: %08x", scene_link_list->group_id);
        TRACE_D("\t|-- name: %s", scene_link_list->name);
        CJSON_TRACE("\t|-- meta: ", scene_link_list->meta);

        TRACE_D("\t|-- parent_id: %s", scene_link_list->parent_id);
        EZPI_print_user_notifications(scene_link_list->user_notifications);
        EZPI_print_house_modes(scene_link_list->house_modes);
        EZPI_print_when_blocks(scene_link_list->when_block, NULL);
        EZPI_print_action_blocks(scene_link_list->then_block);
        EZPI_print_action_blocks(scene_link_list->else_block);
        TRACE_D("\t---------------------------------------------------------------");

        vTaskDelay(5);

        scene_link_list = scene_link_list->next;
    }
#endif
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

#endif // ENABLE_SCENES_PRINT
#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS
/*******************************************************************************
 *                          End of File
 *******************************************************************************/
