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

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_cloud_modes.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_sntp.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_modes_cjson.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_service_loop.h"
#include "ezlopi_service_modes.h"
// #include "ezlopi_service_webprov.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static SemaphoreHandle_t sg_modes_loop_smphr = NULL;

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static void __broadcast_modes_alarmed_for_uid(const char *dev_id_str);
static bool __check_if_devid_in_alarm_off(s_house_modes_t *curr_house_mode, uint32_t u_id);
static void __broadcast_alarmed_state_for_valid_ids(void);
static bool __check_if_device_is_bypassed(cJSON *cj_bypass_devices, const char *device_id_str);
static bool __check_if_entry_delay_finished(s_ezlopi_modes_t *ez_mode);
static void __modes_create_non_bypass_alerts(s_ezlopi_modes_t *ez_mode, s_house_modes_t *curr_house_mode);
static void __modes_check_main_for_trigger(s_ezlopi_modes_t *ez_mode);
static void __modes_main_phase_broadcast(s_ezlopi_modes_t *ez_mode);
static ezlopi_error_t __check_mode_switch_condition(s_ezlopi_modes_t *ez_mode);
static void __modes_loop(void *arg);

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

//---------------------------------------------------------------------------------------------
void ezlopi_service_modes_init(void)
{
    // initialize modes-loop
    sg_modes_loop_smphr = xSemaphoreCreateBinary();
    xSemaphoreGive(sg_modes_loop_smphr);
    ezlopi_service_modes_start(5000);
}

bool ezlopi_service_modes_stop(uint32_t wait_ms)
{
    bool ret = false;
    // uint32_t start_tick = xTaskGetTickCount() / portTICK_PERIOD_MS;

    if (sg_modes_loop_smphr && (xSemaphoreTake(sg_modes_loop_smphr, wait_ms / portTICK_RATE_MS)))
    {
        // 1. Stop the mode-looping process
        EZPI_service_loop_remove(__modes_loop);
        // 2. Clear the alert-loop
        if (NULL != EZPI_core_modes_get_alert_head())
        {
            EZPI_core_modes_remove_all_alerts();
        }

        xSemaphoreGive(sg_modes_loop_smphr);
        TRACE_W("removed modes-loop");
        ret = true;
    }

    return ret;
}

bool ezlopi_service_modes_start(uint32_t wait_ms)
{
    bool ret = false;

    if (EZPI_core_modes_get_custom_modes() && xSemaphoreTake(sg_modes_loop_smphr, wait_ms / portTICK_RATE_MS))
    {
        ret = true;

        // 1. Check if valid-alerts are ready for curr_houseMode
        if (NULL == EZPI_core_modes_get_alert_head())
        {
            s_ezlopi_modes_t *ez_mode = EZPI_core_modes_get_custom_modes();
            s_house_modes_t *curr_house_mode = EZPI_core_modes_get_current_house_modes();
            if (ez_mode && curr_house_mode)
            {
                TRACE_D("*_alert_head -> empty ==> creating list of alerts");
                if (true == curr_house_mode->armed) // if the new Mode is armed ; create 'non_bypass_alert_ll'
                {
                    __modes_create_non_bypass_alerts(ez_mode, curr_house_mode);
                }
            }
        }

        // 2. Start the modes-loop
        xSemaphoreGive(sg_modes_loop_smphr);
        EZPI_service_loop_add("modes-loop", __modes_loop, 1000, NULL);
        TRACE_I("added modes-loop");
    }

    return ret;
}

//---------------------------------------------------------------------------------------------
static void __broadcast_modes_alarmed_for_uid(const char *dev_id_str)
{
    cJSON *cj_alarm_mesg = EZPI_core_modes_cjson_prep_alarm_mesg(dev_id_str);
    // CJSON_TRACE("----------------- broadcasting - cj_alarm_mesg", cj_alarm_mesg);

    if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_alarm_mesg, EZPI_core_sntp_get_current_time_sec()))
    {
        cJSON_Delete(__FUNCTION__, cj_alarm_mesg);
    }
}

static bool __check_if_devid_in_alarm_off(s_house_modes_t *curr_house_mode, uint32_t u_id)
{
    bool ret = false;
    if (curr_house_mode && u_id)
    {
        cJSON *cj_alarm_off = NULL;
        char device_id_str[32] = {0};
        snprintf(device_id_str, sizeof(device_id_str), "%08x", u_id);

        cJSON_ArrayForEach(cj_alarm_off, curr_house_mode->cj_alarms_off_devices) // 'alarm_devid' should not be here
        {
            if (EZPI_STRNCMP_IF_EQUAL(device_id_str, cj_alarm_off->valuestring, strlen(device_id_str) + 1, cj_alarm_off->str_value_len))
            {
                ret = true;
                break;
            }
        }
    }
    return ret;
}

static void __broadcast_alarmed_state_for_valid_ids(void)
{
    // iterate throught the 'alert_ll' and send broadcast to 'valid_ids'
    l_modes_alert_t *curr_node = EZPI_core_modes_get_alert_head();
    while (curr_node)
    {
        l_ezlopi_device_t *target_device = EZPI_core_device_get_by_id(curr_node->u_id);
        if (target_device && target_device->cloud_properties.armed) // allow to broadcast alert only for valid & armed 'device'.
        {
            if (false == __check_if_devid_in_alarm_off(EZPI_core_modes_get_current_house_modes(), curr_node->u_id))
            {
                char dev_id_str[32] = {0};
                snprintf(dev_id_str, sizeof(dev_id_str), "%08x", curr_node->u_id);
                // send alarmed-broadcast for target 'device_id'
                __broadcast_modes_alarmed_for_uid(dev_id_str);
            }
        }
        curr_node = curr_node->next;
    }
}

static bool __check_if_device_is_bypassed(cJSON *cj_bypass_devices, const char *device_id_str) // this only checks if 'device_id_str' is bypassed
{
    bool ret = false;
    if (cj_bypass_devices && device_id_str)
    {
        cJSON *cj_bypass = NULL;
        cJSON_ArrayForEach(cj_bypass, cj_bypass_devices)
        {
            if (EZPI_STRNCMP_IF_EQUAL(device_id_str, cj_bypass->valuestring, strlen(device_id_str) + 1, cj_bypass->str_value_len))
            {
                ret = true;

                // Trigger 1 -> bypass 'DONE' broadcast.
                s_ezlopi_modes_t *curr_mode = EZPI_core_modes_get_custom_modes();
                if (curr_mode)
                {
                    curr_mode->alarmed.phase = EZLOPI_MODES_ALARM_PHASE_BYPASS;
                    curr_mode->alarmed.status = EZLOPI_MODES_ALARM_STATUS_DONE;

                    // send alarmed-broadcast for target 'device_id'
                    __broadcast_modes_alarmed_for_uid(device_id_str);
                }
                break;
            }
        }
    }
    return ret;
}

static bool __check_if_entry_delay_finished(s_ezlopi_modes_t *ez_mode)
{
    bool entryDelay_finish = false;
    if (ez_mode)
    {
        bool trigger_broadcast = false;

        // 1. First activate --> 'entry-delay' phase immediately
        if (ez_mode->alarmed.phase != EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY)
        {
            TRACE_S("alarm.phase = [%d]--> not entry_delay / / alarm.status = [%d] ", ez_mode->alarmed.phase, ez_mode->alarmed.status);
        }

        //  USE :- [ time_is_left_sec = 0 ] ; To Exit from 'entry-delay' and move to "main"  immediately."
        if ((0 < ez_mode->alarmed.entry_delay_sec) && (0 < ez_mode->alarmed.time_is_left_sec)) // time-left to exit entry-delay
        {
            ez_mode->alarmed.phase = EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY;
            // First check if 'EntryDelay' is canceled.
            if (EZLOPI_MODES_ALARM_STATUS_CANCELED != ez_mode->alarmed.status) // begin or done
            {
                // a. Trigger entry-Delay 'begin' broadcast (at the start)
                if (EZLOPI_MODES_ALARM_STATUS_BEGIN != ez_mode->alarmed.status) // done
                {
                    ez_mode->alarmed.status = EZLOPI_MODES_ALARM_STATUS_BEGIN;
                    trigger_broadcast = true;
                }

                // b. Check if the mode is given
                if (0 == ez_mode->alarmed.time_is_left_sec) // Number of seconds left to the end of the Entry delay.
                {
                    ez_mode->alarmed.status = EZLOPI_MODES_ALARM_STATUS_DONE;
                    trigger_broadcast = true;
                    entryDelay_finish = true;
                }
                else
                {
                    ez_mode->alarmed.time_is_left_sec--;
                    TRACE_D("time_is_left_to_ALARM_sec: %u", ez_mode->alarmed.time_is_left_sec);
                }
            }
            else // for canceled entry-delay
            {
                TRACE_D("entryDelay_Status : 'Canceled' ---> time_left is set to '0' ");
                ez_mode->alarmed.time_is_left_sec = 0;
                ez_mode->alarmed.status = EZLOPI_MODES_ALARM_STATUS_CANCELED;
                trigger_broadcast = true;
                entryDelay_finish = true;
            }
        }
        else // Here if "entryDelay-time-left" is 0 // you don't need to trigger a broadcast
        {
            ez_mode->alarmed.status = EZLOPI_MODES_ALARM_STATUS_DONE;
            entryDelay_finish = true;
        }

        TRACE_W("alarm.phase = [%d] / alarm.status = [%d]", ez_mode->alarmed.phase, ez_mode->alarmed.status);
        //-------------------------------------------------------------------------------------

        if (true == trigger_broadcast)
        {
            //// Exclude :- a list of devices that should not raise alarms in the house mode
            __broadcast_alarmed_state_for_valid_ids();
        }
    }
    return entryDelay_finish;
}

static void __modes_create_non_bypass_alerts(s_ezlopi_modes_t *ez_mode, s_house_modes_t *curr_house_mode)
{
    if (ez_mode && curr_house_mode && (NULL == EZPI_core_modes_get_alert_head()))
    {
        cJSON *cj_alarm = NULL;
        cJSON_ArrayForEach(cj_alarm, ez_mode->cj_alarms) // 'alarm_device_id' to trigger alerts
        {
            // 1.1.  Non-bypass-loop creation for listed 'device_id'
            if (cj_alarm->valuestring && cj_alarm->str_value_len)
            {
                if (false == __check_if_device_is_bypassed(curr_house_mode->cj_bypass_devices, cj_alarm->valuestring)) // donot trigger alert if device is bypassed
                {
                    uint32_t devId_to_add = (uint32_t)strtoul(cj_alarm->valuestring, NULL, 16);
                    if (EZPI_SUCCESS == EZPI_core_modes_add_alert(devId_to_add, ez_mode))
                    {
                        TRACE_D("alert_ll --> Adding :%s [%08x]", cj_alarm->valuestring, devId_to_add);
#warning "need to discuss about toggle devices's : arm_flag-->true";
                        l_ezlopi_device_t *target_security_device = EZPI_core_device_get_by_id(devId_to_add);
                        if (target_security_device && (false == target_security_device->cloud_properties.armed))
                        {
                            TRACE_D("Device :%08x --> Armed = true", devId_to_add);
                            target_security_device->cloud_properties.armed = true; //
                        }
                    }
                }
            }
        }
    }
}

static void __modes_check_main_for_trigger(s_ezlopi_modes_t *ez_mode)
{
    if (ez_mode)
    {
        l_modes_alert_t *curr_node = EZPI_core_modes_get_alert_head();
        while (curr_node)
        { // check if the alert has been triggered for any of node in 'alert_ll'.
            if (true == curr_node->alert_trig)
            {
                if (0 == curr_node->timeleft_to_abort_ll)
                {
                    if (0 == curr_node->timeleft_to_alarm_ll)
                    { // trigger a broadcast  --> from: 'MAIN-begin' to 'MAIN-done'
                        ez_mode->alarmed.status = EZLOPI_MODES_ALARM_STATUS_DONE;
                        __broadcast_alarmed_state_for_valid_ids();

                        // Reset the 'alert_trig' flag, 'abort-window' and 'alarm-delay' in this "ALERT-NODE"
                        curr_node->timeleft_to_alarm_ll = curr_node->abort_window_ll;
                        curr_node->timeleft_to_alarm_ll = curr_node->alarm_delay_ll;
                        curr_node->alert_trig = false;
                    }
                    else
                    {
                        curr_node->timeleft_to_alarm_ll--;
                    }
                }
                else
                {
                    curr_node->timeleft_to_abort_ll--;
                }
            }

            curr_node = curr_node->next;
        }
    }
}

static void __modes_main_phase_broadcast(s_ezlopi_modes_t *ez_mode)
{
    if (ez_mode && (NULL != EZPI_core_modes_get_alert_head())) // must have atleast one alert device
    {
        // 1. broadcast 'MAIN-begin' status (Should happen only Once)
        if (EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY == ez_mode->alarmed.phase &&
            (EZLOPI_MODES_ALARM_STATUS_DONE == ez_mode->alarmed.status || EZLOPI_MODES_ALARM_STATUS_CANCELED == ez_mode->alarmed.status))
        {
            ez_mode->alarmed.phase = EZLOPI_MODES_ALARM_PHASE_MAIN;
            ez_mode->alarmed.status = EZLOPI_MODES_ALARM_STATUS_BEGIN;

            __broadcast_alarmed_state_for_valid_ids(); // broadcast for 'valid' ids only
        }
        else
        { // 2. SET --> [PHASE :- MAIN]
            ez_mode->alarmed.phase = EZLOPI_MODES_ALARM_PHASE_MAIN;
        }

        // 3. iterate through 'alert-ll' to trigger
        __modes_check_main_for_trigger(ez_mode);

        TRACE_W("alarm.phase = [%d] / alarm.status = [%d]", ez_mode->alarmed.phase, ez_mode->alarmed.status);
    }
}

static ezlopi_error_t __check_mode_switch_condition(s_ezlopi_modes_t *ez_mode)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (ez_mode->switch_to_mode_id)
    {
        if (0 < ez_mode->time_is_left_to_switch_sec)
        {
            ez_mode->time_is_left_to_switch_sec--;
            TRACE_D("time_is_left_to_SWITCH_sec: %u", ez_mode->time_is_left_to_switch_sec);
        }
        else
        {
            // find the 'house-mode', to switch to using 'switch_to_mode_id'.
            s_house_modes_t *new_house_mode = EZPI_core_modes_get_house_mode_by_id(ez_mode->switch_to_mode_id);
            if (new_house_mode)
            {
                uint8_t bypass_clear_modeId = ez_mode->current_mode_id;

                // switch to new 'MODE'
                TRACE_I("switching-to-mode: %s (id: %u)", new_house_mode->name, new_house_mode->_id);
                ez_mode->current_mode_id = ez_mode->switch_to_mode_id;
                ez_mode->switch_to_mode_id = 0;

                // After the transition, bypass devices list is cleared for the previous house mode.
                s_house_modes_t *prev_house_mode = EZPI_core_modes_get_house_mode_by_id(bypass_clear_modeId);
                if (prev_house_mode && prev_house_mode->cj_bypass_devices)
                {
                    // TRACE_D("clearing all alerts");
#if 0
                    // if switching has took place during 'entry-delay'phase---> send 'canceled' as alarmed-broadcast
                    if (EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY == ez_mode->alarmed.phase &&
                        EZLOPI_MODES_ALARM_STATUS_CANCELED == ez_mode->alarmed.status)
                    {
                        __broadcast_alarmed_state_for_valid_ids();
                    }
#endif
                    // Also Remove all the 'alert-nodes'
                    EZPI_core_modes_remove_all_alerts();

                    cJSON_Delete(__FUNCTION__, prev_house_mode->cj_bypass_devices);
                    prev_house_mode->cj_bypass_devices = NULL;
                }

                // set the new 'house-mode' as the active 'custom-mode'
                if (EZPI_SUCCESS == EZPI_core_modes_set_current_house_mode(new_house_mode))
                {
                    // 1. assign 'delayToSwitch'
                    ez_mode->time_is_left_to_switch_sec = ez_mode->switch_to_delay_sec = new_house_mode->switch_to_delay_sec;

                    // 2. assign 'alert/alarm_delay' mode.
                    if (new_house_mode->armed)
                    {
                        ez_mode->alarm_delay = new_house_mode->alarm_delay_sec; // 'Delay_sec' before sending alert if armed sensors (door/window or motion sensor) tripped
                    }

                    // 3. Refresh 'timeleftTo-exit-EntryDelay' second info for --->  non-zero 'entryDelay'
                    if (0 < ez_mode->alarmed.entry_delay_sec)
                    {
                        ez_mode->alarmed.time_is_left_sec = ez_mode->alarmed.entry_delay_sec;
                    }

                    // 4. Store to nvs
                    EZPI_core_modes_store_to_nvs();
                    cJSON *cj_update = EZPI_core_modes_cjson_changed();
                    // CJSON_TRACE("----------------- broadcasting - cj_update", cj_update);

                    if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_update, EZPI_core_sntp_get_current_time_sec()))
                    {
                        cJSON_Delete(__FUNCTION__, cj_update);
                    }

                    ret = EZPI_SUCCESS;
                }
            }
        }
    }

    return ret;
}

static void __modes_loop(void *arg)
{
    if (pdTRUE == xSemaphoreTake(sg_modes_loop_smphr, 1000 / portTICK_PERIOD_MS))
    {
        s_ezlopi_modes_t *ez_mode = EZPI_core_modes_get_custom_modes();
        s_house_modes_t *curr_house_mode = EZPI_core_modes_get_current_house_modes();
        if (ez_mode && curr_house_mode)
        {
            // 1. check if the mode is to be switched.
            if (EZPI_SUCCESS == __check_mode_switch_condition(ez_mode))
            {
                TRACE_D("Mode - Switch completed to [%d]", ez_mode->current_mode_id);
                curr_house_mode = EZPI_core_modes_get_current_house_modes();
                TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_INFO, "mode: switching to: %s (id: %u).", curr_house_mode->name, curr_house_mode->_id);

                // after switching-modes ; Create unique trigger-event-loops for each devices in 'alarm-list'
                if (true == curr_house_mode->armed) // if the new Mode is armed ; create 'non_bypass_alert_ll'
                {
                    __modes_create_non_bypass_alerts(ez_mode, curr_house_mode);
                }
            }
            else
            { // In next second, after switching is DONE. Perform 'MAIN' alarming-phase.
                // 2. Pre-alarming (ENTRY-DELAY) ; Operate on the 'cj_alarms' list to excluding 'cj_alarm_off_devices'
                if (true == curr_house_mode->armed)
                {
                    if (true == __check_if_entry_delay_finished(ez_mode))
                    {
                        // 3. Perform --> 'MAIN' phase operations
                        __modes_main_phase_broadcast(ez_mode);
                    }
                }
            }
        }

        xSemaphoreGive(sg_modes_loop_smphr);
    }
}

#if 0
static TaskHandle_t sg_process_handle = NULL;

int ezlopi_service_modes_stop(void)
{
    if (sg_process_handle)
    {
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
        EZPI_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_MODES_TASK);
#endif
        vTaskDelete(sg_process_handle);
        sg_process_handle = NULL;
        TRACE_W("Modes-service: Stopped!");
    }

    return 1;
}

int ezlopi_service_modes_start(5000void)
{
    int ret = 0;

    if ((NULL == sg_process_handle) && EZPI_core_modes_get_custom_modes())
    {
        ret = 1;
        xTaskCreate(__modes_service, "modes-service", EZLOPI_SERVICE_MODES_TASK_DEPTH, NULL, 3, &sg_process_handle);
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
        EZPI_core_process_set_process_info(ENUM_EZLOPI_SERVICE_MODES_TASK, &sg_process_handle, EZLOPI_SERVICE_MODES_TASK_DEPTH);
#endif
        TRACE_I("Starting modes-service");
    }

    return ret;
}

static void __modes_service(void *pv)
{
    while (1)
    {
        s_ezlopi_modes_t *ez_mode = EZPI_core_modes_get_custom_modes();
        if (ez_mode)
        {
            if (ez_mode->switch_to_mode_id)
            {
                if (ez_mode->time_is_left_to_switch_sec)
                {
                    ez_mode->time_is_left_to_switch_sec--;
                    TRACE_D("time_is_left_to_switch_sec: %u", ez_mode->time_is_left_to_switch_sec);
                }
                else
                {
                    s_house_modes_t *new_house_mode = EZPI_core_modes_get_house_mode_by_id(ez_mode->switch_to_mode_id);

                    if (new_house_mode)
                    {
                        TRACE_I("switching-to-mode: %s (id: %u)", new_house_mode->name, new_house_mode->_id);

                        ez_mode->current_mode_id = ez_mode->switch_to_mode_id;
                        ez_mode->switch_to_mode_id = 0;

                        EZPI_core_modes_set_current_house_mode(new_house_mode);

                        if (new_house_mode->cj_bypass_devices)
                        {
                            cJSON_Delete(__FUNCTION__, new_house_mode->cj_bypass_devices);
                            new_house_mode->cj_bypass_devices = NULL;
                        }

                        EZPI_core_modes_store_to_nvs();

                        cJSON *cj_update = EZPI_core_modes_cjson_changed();
                        CJSON_TRACE("----------------- broadcasting - cj_update", cj_update);

                        if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_update))
                        {
                            cJSON_Delete(__FUNCTION__, cj_update);
                        }
                    }
                }
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_MODES_TASK);
#endif
    vTaskDelete(NULL);
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MODES

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
