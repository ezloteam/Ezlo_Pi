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
 * @file    ezlopi_core_modes.c
 * @brief   These function perform opertation on HouseModes
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
// #include "ezlopi_util_trace.h"

// #include "ezlopi_core_modes.h"
// #include "ezlopi_core_errors.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_api_methods.h"
#include "ezlopi_core_modes_cjson.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_devices.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_modes.h"
#include "EZLOPI_USER_CONFIG.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)

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
static s_ezlopi_modes_t *sg_custom_modes = NULL;
static s_house_modes_t *sg_current_house_mode = NULL;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

s_ezlopi_modes_t *EZPI_core_modes_get_custom_modes(void)
{
    return sg_custom_modes;
}

ezlopi_error_t EZPI_core_modes_set_current_house_mode(s_house_modes_t *new_house_mode)
{
    sg_current_house_mode = new_house_mode;
    return EZPI_SUCCESS;
}

s_house_modes_t *EZPI_core_modes_get_current_house_modes(void)
{
    return sg_current_house_mode;
}

s_house_modes_t *EZPI_core_modes_get_house_mode_by_id(uint32_t house_mode_id)
{
    s_house_modes_t *_house_mode = NULL;

    if (house_mode_id == sg_custom_modes->mode_home._id)
    {
        _house_mode = &sg_custom_modes->mode_home;
    }
    else if (house_mode_id == sg_custom_modes->mode_away._id)
    {
        _house_mode = &sg_custom_modes->mode_away;
    }
    else if (house_mode_id == sg_custom_modes->mode_night._id)
    {
        _house_mode = &sg_custom_modes->mode_night;
    }
    else if (house_mode_id == sg_custom_modes->mode_vacation._id)
    {
        _house_mode = &sg_custom_modes->mode_vacation;
    }
    else
    {
        TRACE_E("house-mode-id does not match with existing house-modes!");
    }

    return _house_mode;
}

s_house_modes_t *EZPI_core_modes_get_house_mode_by_name(char *house_mode_name)
{
    s_house_modes_t *_house_mode = NULL;

    if (house_mode_name)
    {
        size_t len = strlen(house_mode_name);
        if (EZPI_STRNCMP_IF_EQUAL(house_mode_name, sg_custom_modes->mode_home.name, len, strlen(sg_custom_modes->mode_home.name) + 1))
        {
            _house_mode = &sg_custom_modes->mode_home;
        }
        else if (EZPI_STRNCMP_IF_EQUAL(house_mode_name, sg_custom_modes->mode_away.name, len, strlen(sg_custom_modes->mode_away.name) + 1))
        {
            _house_mode = &sg_custom_modes->mode_away;
        }
        else if (EZPI_STRNCMP_IF_EQUAL(house_mode_name, sg_custom_modes->mode_night.name, len, strlen(sg_custom_modes->mode_night.name) + 1))
        {
            _house_mode = &sg_custom_modes->mode_night;
        }
        else if (EZPI_STRNCMP_IF_EQUAL(house_mode_name, sg_custom_modes->mode_vacation.name, len, strlen(sg_custom_modes->mode_vacation.name) + 1))
        {
            _house_mode = &sg_custom_modes->mode_vacation;
        }
        else
        {
            TRACE_E("modes-name does not match with existing modes!");
        }
    }

    return _house_mode;
}

ezlopi_error_t EZPI_core_modes_api_get_modes(cJSON *cj_result)
{
    return EZPI_core_modes_cjson_get_modes(cj_result);
}

ezlopi_error_t EZPI_core_modes_api_get_current_mode(cJSON *cj_result)
{
    return EZPI_core_modes_cjson_get_current_mode(cj_result);
}

ezlopi_error_t EZPI_core_modes_api_switch_mode(s_house_modes_t *switch_to_house_mode)
{
    ezlopi_service_modes_stop(5000);
    sg_custom_modes->switch_to_mode_id = switch_to_house_mode->_id;
    sg_custom_modes->time_is_left_to_switch_sec = 0;

    // #. If the curr-house-mode already was in 'pre-alarming[EntryDelay]' MODE ; set 'canceled' broadcast status
    if (EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY == sg_custom_modes->alarmed.phase &&
        EZLOPI_MODES_ALARM_STATUS_BEGIN == sg_custom_modes->alarmed.status)
    {
        TRACE_D("switching to MODE : %d", sg_custom_modes->switch_to_mode_id);
        sg_custom_modes->alarmed.status = EZLOPI_MODES_ALARM_STATUS_CANCELED;
    }

    // The broadcast is triggered when 'modes-loop' is started
    ezlopi_service_modes_start(5000);

    return 1;
}

ezlopi_error_t EZPI_core_modes_api_cancel_switch(void)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_custom_modes)
    {
        ezlopi_service_modes_stop(5000);
        sg_custom_modes->switch_to_mode_id = 0;
        sg_custom_modes->time_is_left_to_switch_sec = 0;
        ezlopi_service_modes_start(5000);
        ret = EZPI_SUCCESS;
    }

    return ret;
}

ezlopi_error_t EZPI_core_modes_api_cancel_entry_delay(void)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_custom_modes)
    {
        ret = EZPI_SUCCESS;
        ezlopi_service_modes_stop(5000);
        sg_custom_modes->alarmed.status = EZLOPI_MODES_ALARM_STATUS_CANCELED;
        ezlopi_service_modes_start(5000);
    }

    return ret;
}

ezlopi_error_t EZPI_core_modes_api_skip_entry_delay(void)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_custom_modes)
    {
        ret = EZPI_SUCCESS;
        ezlopi_service_modes_stop(5000);
        sg_custom_modes->alarmed.time_is_left_sec = 0;
        sg_custom_modes->alarmed.phase = EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY;
        sg_custom_modes->alarmed.status = EZLOPI_MODES_ALARM_STATUS_DONE;
        ezlopi_service_modes_start(5000);
    }

    return ret;
}

ezlopi_error_t EZPI_core_modes_api_set_switch_to_delay(uint32_t switch_to_delay)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_current_house_mode)
    {
        ret = EZPI_SUCCESS;
        ezlopi_service_modes_stop(5000);
        sg_custom_modes->switch_to_delay_sec = switch_to_delay;
        EZPI_core_modes_store_to_nvs();
        ezlopi_service_modes_start(5000);
    }
    return ret;
}

ezlopi_error_t EZPI_core_modes_api_set_alarm_delay(uint32_t alarm_to_delay)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_current_house_mode)
    {
        ret = EZPI_SUCCESS;
        ezlopi_service_modes_stop(5000);

        // 1. update in static 'mode-struct'
        // 2. update using 'curr-house_mode' pointer
        sg_custom_modes->alarm_delay = sg_current_house_mode->alarm_delay_sec = alarm_to_delay; // Delay (sec) before sending alert if armed sensors (door/window or motion sensor) tripped

        EZPI_core_modes_store_to_nvs();
        ezlopi_service_modes_start(5000);
    }
    return ret;
}

ezlopi_error_t EZPI_core_modes_api_set_notifications(uint8_t modeId, bool all, cJSON *user_id_aray)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if ((EZLOPI_HOUSE_MODE_REF_ID_NONE < modeId) && (EZLOPI_HOUSE_MODE_REF_ID_MAX > modeId) && user_id_aray && (cJSON_Array == user_id_aray->type))
    {
        ezlopi_service_modes_stop(5000);
        s_house_modes_t *mode_to_upate = EZPI_core_modes_get_house_mode_by_id(modeId);
        if (mode_to_upate)
        {
            mode_to_upate->notify_all = all;

            // Delete previous cj_notification
            if (NULL != mode_to_upate->cj_notifications)
            {
                cJSON_Delete(__func__, mode_to_upate->cj_notifications);
            }

            // Refreshing the notification list.
            mode_to_upate->cj_notifications = cJSON_CreateArray(__FUNCTION__);
            if (mode_to_upate->cj_notifications)
            {
                mode_to_upate->cj_notifications = cJSON_Duplicate(__FUNCTION__, user_id_aray, cJSON_True);
            }

            mode_to_upate->disarmed_default = false;
            EZPI_core_modes_store_to_nvs();

            ret = EZPI_SUCCESS;
        }
        ezlopi_service_modes_start(5000);
    }
    return ret;
}

ezlopi_error_t EZPI_core_modes_api_add_alarm_off(uint8_t modeId, const char *device_id_str)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if ((EZLOPI_HOUSE_MODE_REF_ID_NONE < modeId) && (EZLOPI_HOUSE_MODE_REF_ID_MAX > modeId) && device_id_str)
    {
        ezlopi_service_modes_stop(5000);
        s_house_modes_t *targe_house_mode = EZPI_core_modes_get_house_mode_by_id(modeId);
        if (targe_house_mode)
        {
            cJSON *element_to_check = NULL;
            bool add_to_array = true;
            cJSON_ArrayForEach(element_to_check, targe_house_mode->cj_alarms_off_devices)
            {
                if (EZPI_STRNCMP_IF_EQUAL(device_id_str, element_to_check->valuestring, strlen(device_id_str) + 1, element_to_check->str_value_len))
                {
                    add_to_array = false;
                    break;
                }
            }
            if (add_to_array)
            {
                if (NULL == targe_house_mode->cj_alarms_off_devices)
                {
                    targe_house_mode->cj_alarms_off_devices = cJSON_CreateArray(__func__);
                }
                cJSON_AddItemToArray(targe_house_mode->cj_alarms_off_devices, cJSON_CreateString(__func__, device_id_str));

                if (EZPI_SUCCESS != EZPI_core_modes_store_to_nvs())
                {
                    TRACE_D("Error!! when adding alarm_off");
                }
                else
                {
                    ret = EZPI_SUCCESS;
                }
            }
            targe_house_mode->disarmed_default = false;
        }
        ezlopi_service_modes_start(5000);
    }
    return ret;
}

ezlopi_error_t EZPI_core_modes_api_remove_alarm_off(uint32_t modeId, const char *device_id_str)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if ((EZLOPI_HOUSE_MODE_REF_ID_NONE < modeId) && (EZLOPI_HOUSE_MODE_REF_ID_MAX > modeId) && device_id_str)
    {
        ezlopi_service_modes_stop(5000);
        s_house_modes_t *targe_house_mode = EZPI_core_modes_get_house_mode_by_id(modeId);
        if (targe_house_mode)
        {
            cJSON *element_to_check = NULL;
            int array_index = 0;
            cJSON_ArrayForEach(element_to_check, targe_house_mode->cj_alarms_off_devices)
            {
                if (EZPI_STRNCMP_IF_EQUAL(device_id_str, element_to_check->valuestring, strlen(device_id_str) + 1, element_to_check->str_value_len))
                {
                    cJSON_DeleteItemFromArray(__func__, targe_house_mode->cj_alarms_off_devices, array_index);
                    break;
                }
                array_index++;
            }

            if (EZPI_SUCCESS != EZPI_core_modes_store_to_nvs())
            {
                TRACE_D("Error!! when removing alarm_off");
            }
            else
            {
                ret = EZPI_SUCCESS;
            }
            targe_house_mode->disarmed_default = false;
        }
        ezlopi_service_modes_start(5000);
    }
    return ret;
}

ezlopi_error_t EZPI_core_modes_api_add_cameras_off(uint8_t modeId, const char *device_id_str)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if ((EZLOPI_HOUSE_MODE_REF_ID_NONE < modeId) && (EZLOPI_HOUSE_MODE_REF_ID_MAX > modeId) && device_id_str)
    {
        ezlopi_service_modes_stop(5000);
        s_house_modes_t *mode_to_update = EZPI_core_modes_get_house_mode_by_id(modeId);
        if (mode_to_update)
        {
            bool add_to_array = true;
            cJSON *add_element = NULL;
            cJSON_ArrayForEach(add_element, mode_to_update->cj_cameras_off_devices)
            {
                if (EZPI_STRNCMP_IF_EQUAL(device_id_str, add_element->valuestring, strlen(device_id_str), add_element->str_value_len))
                {
                    add_to_array = false;
                    break;
                }
            }
            if (add_to_array)
            {
                if (NULL == mode_to_update->cj_cameras_off_devices)
                {
                    mode_to_update->cj_cameras_off_devices = cJSON_CreateArray(__FUNCTION__);
                }
                cJSON_AddItemToArray(mode_to_update->cj_cameras_off_devices, cJSON_CreateString(__func__, device_id_str));

                mode_to_update->disarmed_default = false; // disarmedDefault state will change to **false**
                EZPI_core_modes_store_to_nvs();
            }
            ret = EZPI_SUCCESS;
        }
        ezlopi_service_modes_start(5000);
    }

    return ret;
}

ezlopi_error_t EZPI_core_modes_api_remove_cameras_off(uint8_t modeId, const char *device_id_str)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;

    if ((EZLOPI_HOUSE_MODE_REF_ID_NONE < modeId) && (EZLOPI_HOUSE_MODE_REF_ID_MAX > modeId) && device_id_str)
    {
        ezlopi_service_modes_stop(5000);
        s_house_modes_t *mode_to_update = EZPI_core_modes_get_house_mode_by_id(modeId);
        if (mode_to_update)
        {
            cJSON *remove_element = NULL;
            int array_index = 0;
            cJSON_ArrayForEach(remove_element, mode_to_update->cj_cameras_off_devices)
            {
                if (EZPI_STRNCMP_IF_EQUAL(device_id_str, remove_element->valuestring, strlen(device_id_str) + 1, remove_element->str_value_len))
                {
                    cJSON_DeleteItemFromArray(__FUNCTION__, mode_to_update->cj_cameras_off_devices, array_index);

                    mode_to_update->disarmed_default = false; // disarmedDefault state will change to **false**
                    EZPI_core_modes_store_to_nvs();
                    break;
                }
                array_index++;
            }
            ret = EZPI_SUCCESS;
        }
        ezlopi_service_modes_start(5000);
    }

    return ret;
}

ezlopi_error_t EZPI_core_modes_api_bypass_device_add(uint8_t modeId, cJSON *cj_device_id_array)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if ((EZLOPI_HOUSE_MODE_REF_ID_NONE < modeId) && (EZLOPI_HOUSE_MODE_REF_ID_MAX > modeId) && cj_device_id_array && (cJSON_Array == cj_device_id_array->type))
    {
        ezlopi_service_modes_stop(5000);
        s_house_modes_t *mode_to_update = EZPI_core_modes_get_house_mode_by_id(modeId);
        if (mode_to_update)
        {
            cJSON *bypass_dev_to_add = NULL;
            cJSON_ArrayForEach(bypass_dev_to_add, cj_device_id_array)
            {
                cJSON *element_to_check = NULL;
                bool add_to_array = true;
                cJSON_ArrayForEach(element_to_check, mode_to_update->cj_bypass_devices)
                {
                    if (EZPI_STRNCMP_IF_EQUAL(bypass_dev_to_add->valuestring, element_to_check->valuestring, bypass_dev_to_add->str_value_len, element_to_check->str_value_len))
                    {
                        add_to_array = false;
                        break;
                    }
                }
                if (add_to_array)
                {
                    if (NULL == mode_to_update->cj_bypass_devices)
                    {
                        mode_to_update->cj_bypass_devices = cJSON_CreateArray(__func__);
                    }
                    cJSON_AddItemToArray(mode_to_update->cj_bypass_devices, cJSON_CreateString(__func__, bypass_dev_to_add->valuestring));
                }
            }
            EZPI_core_modes_store_to_nvs();
            ret = EZPI_SUCCESS;
        }
        ezlopi_service_modes_start(5000);
    }
    return ret;
}

ezlopi_error_t EZPI_core_modes_api_bypass_device_remove(uint8_t modeId, cJSON *cj_device_id_array)
{
    ezlopi_error_t ret = 0;
    if ((EZLOPI_HOUSE_MODE_REF_ID_NONE < modeId) && (EZLOPI_HOUSE_MODE_REF_ID_MAX > modeId) && cj_device_id_array && (cJSON_Array == cj_device_id_array->type))
    {
        ezlopi_service_modes_stop(5000);
        s_house_modes_t *mode_to_update = EZPI_core_modes_get_house_mode_by_id(modeId);
        if (mode_to_update)
        {
            cJSON *bypass_dev_to_remove = NULL;
            cJSON_ArrayForEach(bypass_dev_to_remove, cj_device_id_array)
            {
                cJSON *element_to_check = NULL;
                int array_index = 0;
                cJSON_ArrayForEach(element_to_check, mode_to_update->cj_bypass_devices)
                {
                    if (EZPI_STRNCMP_IF_EQUAL(bypass_dev_to_remove->valuestring, element_to_check->valuestring, bypass_dev_to_remove->str_value_len, element_to_check->str_value_len))
                    {
                        cJSON_DeleteItemFromArray(__func__, mode_to_update->cj_bypass_devices, array_index);
                        break;
                    }
                    array_index++;
                }
            }
            EZPI_core_modes_store_to_nvs();
            ret = 1;
        }
        ezlopi_service_modes_start(5000);
    }
    return ret;
}

ezlopi_error_t EZPI_core_modes_api_set_protect(uint32_t modeId, bool protect_state)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_custom_modes)
    {
        ezlopi_service_modes_stop(5000);
        s_house_modes_t *house_mode = NULL; /*0,1,2,3*/
        if (NULL != (house_mode = EZPI_core_modes_get_house_mode_by_id(modeId)))
        {
            house_mode->protect = protect_state;

            if (EZPI_SUCCESS != EZPI_core_modes_store_to_nvs())
            {
                TRACE_E("Error!! , [id = %d] protection failed", modeId);
            }
            else
            {
                ret = EZPI_SUCCESS;
            }
        }
        ezlopi_service_modes_start(5000);
    }
    return ret;
}

ezlopi_error_t EZPI_core_modes_api_set_protect_button(char *service_str, uint32_t deviceId, uint8_t *status) // status => [ BIT0 = added ; BIT1 = Updated ; BIT2 = removed ]
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (service_str && deviceId)
    {
        if ((NULL != EZPI_core_device_get_by_id(deviceId)) && sg_custom_modes) // IF 'device_id' exists then add to 'protect_button_ll'
        {
            ezlopi_service_modes_stop(5000);

            if (NULL != sg_custom_modes->l_protect_buttons)
            {
                size_t len = strlen(service_str) + 1;
                s_protect_buttons_t *curr_button = sg_custom_modes->l_protect_buttons; // Start from the 'head'
                while (curr_button)
                {
                    if (deviceId == curr_button->device_id)
                    {
                        if (EZPI_STRNCMP_IF_EQUAL(curr_button->service_name, service_str, strlen(curr_button->service_name) + 1, len))
                        {
                            // ### removing node
                            //---------------------------------------------------------------------------------------
                            *status = BIT2;                                        // 'removed-flag' is set
                            if (sg_custom_modes->l_protect_buttons == curr_button) // first compare with head_node
                            {
                                s_protect_buttons_t *__del_node = sg_custom_modes->l_protect_buttons;
                                sg_custom_modes->l_protect_buttons = sg_custom_modes->l_protect_buttons->next;

                                // clearing all members of the target-node
                                __del_node->func = NULL;
                                __del_node->next = NULL;
                                ezlopi_free(__func__, __del_node);
                            }
                            else
                            {
                                s_protect_buttons_t *_node = sg_custom_modes->l_protect_buttons;
                                while (_node->next)
                                {
                                    if (_node->next == curr_button)
                                    {
                                        s_protect_buttons_t *__del_node = _node->next;
                                        _node->next = _node->next->next;

                                        // clearing all members of the target-node
                                        __del_node->func = NULL;
                                        __del_node->next = NULL;
                                        ezlopi_free(__func__, __del_node);
                                        break;
                                    }

                                    _node = _node->next;
                                }
                            }

                            //---------------------------------------------------------------------------------------
                        }
                        else
                        {
                            *status = BIT1; // 'updated-flag' is set
                            snprintf(curr_button->service_name, sizeof(curr_button->service_name), "%s", service_str);
                        }

                        break;
                    }

                    if (curr_button->next)
                    {
                        curr_button = curr_button->next;
                    }
                    else // ### Adding the 'new button', to last-node of ll
                    {
                        TRACE_D("[Tail->next==NULL] ; adding new 'protect_button' to the tail_node  ");
                        curr_button->next = (s_protect_buttons_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_protect_buttons_t));
                        if (curr_button->next)
                        {
                            *status = BIT0; // 'added-flag' is set
                            memset(curr_button->next, 0, sizeof(s_protect_buttons_t));
                            snprintf(curr_button->next->service_name, sizeof(curr_button->next->service_name), "%s", service_str);
                            curr_button->next->device_id = deviceId;
                            curr_button->next->func = NULL;
                            curr_button->next->next = NULL;
                        }

                        break;
                    }
                }
            }
            else // ### Adding to the head node :- 'sg_custom_modes->l_protect_buttons'
            {
                sg_custom_modes->l_protect_buttons = (s_protect_buttons_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_protect_buttons_t));
                if (sg_custom_modes->l_protect_buttons)
                {
                    *status = BIT0; // 'added-flag' is set
                    memset(sg_custom_modes->l_protect_buttons, 0, sizeof(s_protect_buttons_t));
                    snprintf(sg_custom_modes->l_protect_buttons->service_name, sizeof(sg_custom_modes->l_protect_buttons->service_name), "%s", service_str);
                    sg_custom_modes->l_protect_buttons->device_id = deviceId;
                    sg_custom_modes->l_protect_buttons->func = NULL;
                    sg_custom_modes->l_protect_buttons->next = NULL;
                }
            }

            EZPI_core_modes_store_to_nvs();

            ret = EZPI_SUCCESS;
            ezlopi_service_modes_start(5000);
        }
    }

    return ret;
}

ezlopi_error_t EZPI_core_modes_api_add_protect_devices(cJSON *user_id_aray)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;

    if (user_id_aray && (cJSON_Array == user_id_aray->type))
    {
        ezlopi_service_modes_stop(5000);

        if (sg_custom_modes)
        {
            cJSON *dev_to_add = NULL;
            cJSON_ArrayForEach(dev_to_add, user_id_aray)
            {
                bool add_to_array = true;
                cJSON *dev_to_check = NULL;
                cJSON_ArrayForEach(dev_to_check, sg_custom_modes->cj_devices)
                {
                    if (EZPI_STRNCMP_IF_EQUAL(dev_to_add->valuestring, dev_to_check->valuestring, (dev_to_add->str_value_len), (dev_to_check->str_value_len)))
                    {
                        add_to_array = false;
                        break;
                    }
                }
                if (add_to_array)
                {
                    if (NULL == sg_custom_modes->cj_devices)
                    {
                        sg_custom_modes->cj_devices = cJSON_CreateArray(__FUNCTION__);
                    }
                    cJSON_AddItemToArray(sg_custom_modes->cj_devices, cJSON_CreateString(__FUNCTION__, dev_to_add->valuestring));
                }
            }
            EZPI_core_modes_store_to_nvs();
            ret = EZPI_SUCCESS;
        }

        ezlopi_service_modes_start(5000);
    }

    return ret;
}

ezlopi_error_t EZPI_core_modes_api_remove_protect_devices(cJSON *user_id_aray)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (user_id_aray && (cJSON_Array == user_id_aray->type) && sg_custom_modes)
    {
        ezlopi_service_modes_stop(5000);
        cJSON *element_to_remove = NULL;
        cJSON_ArrayForEach(element_to_remove, user_id_aray)
        {
            cJSON *element_to_check = NULL;
            int element_index = 0;
            cJSON_ArrayForEach(element_to_check, sg_custom_modes->cj_devices)
            {
                if (EZPI_STRNCMP_IF_EQUAL(element_to_remove->valuestring, element_to_check->valuestring, (element_to_remove->str_value_len), (element_to_check->str_value_len)))
                {
                    cJSON_DeleteItemFromArray(__FUNCTION__, sg_custom_modes->cj_devices, element_index);
                    break;
                }
                element_index++;
            }
        }
        EZPI_core_modes_store_to_nvs();
        ezlopi_service_modes_start(5000);
        ret = EZPI_SUCCESS;
    }
    return ret;
}

ezlopi_error_t EZPI_core_modes_api_set_entry_delay(uint32_t normal_sec, uint32_t extended_sec, uint32_t long_extended_sec, uint32_t instant_sec)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_custom_modes)
    {
        ezlopi_service_modes_stop(5000);
        s_ezlopi_modes_t *curr_mode = EZPI_core_modes_get_custom_modes();
        if (curr_mode)
        {
            curr_mode->entry_delay.normal_delay_sec = normal_sec;
            curr_mode->entry_delay.extended_delay_sec = extended_sec;
            curr_mode->entry_delay.long_extended_delay_sec = long_extended_sec;
            curr_mode->entry_delay.instant_delay_sec = instant_sec;

            if (EZPI_SUCCESS != EZPI_core_modes_store_to_nvs())
            {
                TRACE_E("Error!! , failed to set new entry_dalay");
            }
            else
            {
                ret = EZPI_SUCCESS;
            }
        }
        ezlopi_service_modes_start(5000);
    }

    return ret;
}

ezlopi_error_t EZPI_core_modes_api_reset_entry_delay(void)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_custom_modes)
    {
        ezlopi_service_modes_stop(5000);
        s_ezlopi_modes_t *curr_mode = EZPI_core_modes_get_custom_modes();
        if (curr_mode)
        {
            curr_mode->entry_delay.normal_delay_sec = 30;
            curr_mode->entry_delay.extended_delay_sec = 60;
            curr_mode->entry_delay.long_extended_delay_sec = 120;
            curr_mode->entry_delay.instant_delay_sec = 0;

            if (EZPI_SUCCESS != EZPI_core_modes_store_to_nvs())
            {
                TRACE_E("Error!! , failed to set new entry_dalay");
            }
            else
            {
                ret = EZPI_SUCCESS;
            }
        }
        ezlopi_service_modes_start(5000);
    }

    return ret;
}

ezlopi_error_t EZPI_core_modes_api_set_disarmed_default(uint8_t modeId, bool disarmedDefault)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if ((modeId > EZLOPI_HOUSE_MODE_REF_ID_NONE) && (modeId < EZLOPI_HOUSE_MODE_REF_ID_MAX))
    {
        ezlopi_service_modes_stop(5000);
        s_ezlopi_modes_t *custom_modes = EZPI_core_modes_get_custom_modes();
        if (custom_modes)
        {
            if (modeId == EZLOPI_HOUSE_MODE_REF_ID_HOME)
            {
                custom_modes->mode_home.disarmed_default = disarmedDefault;
            }
            else if (modeId == EZLOPI_HOUSE_MODE_REF_ID_NIGHT)
            {
                custom_modes->mode_night.disarmed_default = disarmedDefault;
            }
            else if (modeId == EZLOPI_HOUSE_MODE_REF_ID_AWAY)
            {
                custom_modes->mode_away.disarmed_default = disarmedDefault;
            }
            else if (modeId == EZLOPI_HOUSE_MODE_REF_ID_VACATION)
            {
                custom_modes->mode_vacation.disarmed_default = disarmedDefault;
            }
            EZPI_core_modes_store_to_nvs();
            ret = EZPI_SUCCESS;
        }
        ezlopi_service_modes_start(5000);
    }
    return ret;
}

ezlopi_error_t EZPI_core_modes_api_add_disarmed_device(uint8_t modeId, const char *device_id_str)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if ((EZLOPI_HOUSE_MODE_REF_ID_NONE < modeId) && (EZLOPI_HOUSE_MODE_REF_ID_MAX > modeId) && device_id_str)
    {
        ezlopi_service_modes_stop(5000);
        s_house_modes_t *mode_to_update = EZPI_core_modes_get_house_mode_by_id(modeId);
        if (mode_to_update)
        {
            bool add_to_array = true;
            cJSON *add_element = NULL;
            cJSON_ArrayForEach(add_element, mode_to_update->cj_disarmed_devices)
            {
                if (EZPI_STRNCMP_IF_EQUAL(device_id_str, add_element->valuestring, strlen(device_id_str) + 1, add_element->str_value_len))
                {
                    add_to_array = false;
                    break;
                }
            }
            if (add_to_array)
            {
                if (NULL == mode_to_update->cj_disarmed_devices)
                {
                    mode_to_update->cj_disarmed_devices = cJSON_CreateArray(__FUNCTION__);
                }
                cJSON_AddItemToArray(mode_to_update->cj_disarmed_devices, cJSON_CreateString(__func__, device_id_str));

                mode_to_update->disarmed_default = false; // disarmedDefault state will change to **false**
                EZPI_core_modes_store_to_nvs();
            }
            ret = EZPI_SUCCESS;
        }
        ezlopi_service_modes_start(5000);
    }

    return ret;
}

ezlopi_error_t EZPI_core_modes_api_remove_disarmed_device(uint8_t modeId, const char *device_id_str)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;

    if ((EZLOPI_HOUSE_MODE_REF_ID_NONE < modeId) && (EZLOPI_HOUSE_MODE_REF_ID_MAX > modeId) && device_id_str)
    {
        ezlopi_service_modes_stop(5000);
        s_house_modes_t *mode_to_update = EZPI_core_modes_get_house_mode_by_id(modeId);
        if (mode_to_update)
        {
            cJSON *remove_element = NULL;
            int array_index = 0;
            cJSON_ArrayForEach(remove_element, mode_to_update->cj_disarmed_devices)
            {
                if (EZPI_STRNCMP_IF_EQUAL(device_id_str, remove_element->valuestring, strlen(device_id_str) + 1, remove_element->str_value_len))
                {
                    cJSON_DeleteItemFromArray(__FUNCTION__, mode_to_update->cj_disarmed_devices, array_index);

                    mode_to_update->disarmed_default = false; // disarmedDefault state will change to **false**
                    EZPI_core_modes_store_to_nvs();
                    break;
                }
                array_index++;
            }
            ret = EZPI_SUCCESS;
        }
        ezlopi_service_modes_start(5000);
    }

    return ret;
}

ezlopi_error_t EZPI_core_modes_set_unset_device_armed_status(cJSON *cj_device_array, const bool set)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (cj_device_array && cj_device_array->type == cJSON_Array)
    {
        cJSON *curr_device = NULL;
        cJSON_ArrayForEach(curr_device, cj_device_array)
        {
            uint32_t device_id = strtoul(curr_device->valuestring, NULL, 16);
            l_ezlopi_device_t *device_to_change = EZPI_core_device_get_by_id(device_id);
            if (device_to_change)
            {
                if (device_to_change->cloud_properties.armed != set)
                {
                    device_to_change->cloud_properties.armed = set;

                    // 1. PREPARE  cj_request structure to trigger broadcast for  'armed.set'
                    cJSON *cj_device_armed_broadcast = cJSON_CreateObject(__func__);
                    if (cj_device_armed_broadcast)
                    {
                        cJSON_AddStringToObject(__func__, cj_device_armed_broadcast, ezlopi_method_str, method_hub_device_armed_set);
                        cJSON *cj_params = cJSON_AddObjectToObject(__func__, cj_device_armed_broadcast, ezlopi_params_str);
                        if (cj_params)
                        {
                            char tmp_id[32];
                            memset(tmp_id, 0, 32);
                            snprintf(tmp_id, 32, "%08x", device_to_change->cloud_properties.device_id);
                            cJSON_AddStringToObject(__func__, cj_params, ezlopi__id_str, tmp_id);
                            cJSON_AddBoolToObject(__func__, cj_params, ezlopi_armed_str, set);

                            cJSON *cj_response = cJSON_CreateObject(__func__);
                            if (NULL != cj_response)
                            {
                                // 2. CALL : "EZPI_device_updated" broadcast for devices: switced from [ armed --> disarmed ]
                                EZPI_device_updated(cj_device_armed_broadcast, cj_response);

                                if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_response, EZPI_core_sntp_get_current_time_sec()))
                                {
                                    // TRACE_E("freeing cj_response");
                                    cJSON_Delete(__func__, cj_response);
                                    ret = EZPI_ERR_MODES_FAILED;
                                }
                                else
                                {
                                    // TRACE_D("Sending--> broadcast for device armed toggle");
                                    // The 'cj_response' is freed automatically after broadcast.
                                    ret = EZPI_SUCCESS;
                                }
                            }
                        }

                        cJSON_Delete(__func__, cj_device_armed_broadcast); // this cjson need to be freed manually
                    }
                }
            }
        }
    }
    return ret;
}

ezlopi_error_t EZPI_core_modes_store_to_nvs(void)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    cJSON *cj_modes = cJSON_CreateObject(__FUNCTION__);
    if (cj_modes)
    {
        EZPI_core_modes_cjson_get_modes(cj_modes);
        char *modes_str = cJSON_PrintBuffered(__FUNCTION__, cj_modes, 4096, false);
        TRACE_D("length of 'modes_str': %d", strlen(modes_str));

        cJSON_Delete(__FUNCTION__, cj_modes);

        if (modes_str)
        {
            ret = EZPI_core_nvs_write_modes(modes_str);
            ezlopi_free(__FUNCTION__, modes_str);
        }
    }

    return ret;
}

void EZPI_core_modes_init(void)
{
    uint32_t _is_custom_mode_ok = 0;
    char *custom_modes_str = EZPI_core_nvs_read_modes();

    if (custom_modes_str)
    {
        cJSON *cj_custom_modes = cJSON_Parse(__FUNCTION__, custom_modes_str);
        ezlopi_free(__FUNCTION__, custom_modes_str);

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        // CJSON_TRACE("cj_custom-modes", cj_custom_modes);
#endif

        if (cj_custom_modes)
        {
            _is_custom_mode_ok = 1;
            sg_custom_modes = EZPI_core_modes_cjson_parse_modes(cj_custom_modes);
            EZPI_core_modes_set_current_house_mode(EZPI_core_modes_get_house_mode_by_id(sg_custom_modes->current_mode_id));
            cJSON_Delete(__FUNCTION__, cj_custom_modes);
        }
    }

    if (0 == _is_custom_mode_ok)
    {
        EZPI_core_default_init();
        if (NULL == sg_custom_modes)
        {
            sg_custom_modes = EZPI_core_default_mode_get();
            sg_current_house_mode = &sg_custom_modes->mode_home;
        }
    }
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

#endif // CONFIG_EZPI_SERV_ENABLE_MODES
       /*******************************************************************************
        *                          End of File
        *******************************************************************************/
