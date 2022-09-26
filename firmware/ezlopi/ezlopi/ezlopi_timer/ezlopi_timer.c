
/* ===========================================================================
** Copyright (C) 2022 Ezlo Innovation Inc
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

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ezlopi_timer.h"

/*******************************************************************************
 *                          Static Function Definition
 *******************************************************************************/
static bool IRAM_ATTR timer_group_isr_callback(void *args)
{
    BaseType_t high_task_awoken = pdFALSE;
    ezlopi_timer_info_t *info = (ezlopi_timer_info_t *)args;

    uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(info->timer_group, info->timer_idx);

    /* Prepare basic event data that will be then sent back to task */
    ezlopi_timer_event_t evt = {
        .info.timer_group = info->timer_group,
        .info.timer_idx = info->timer_idx,
        .info.auto_reload = info->auto_reload,
        .info.alarm_interval = info->alarm_interval,
        .info.notify = info->notify,
        .timer_counter_value = timer_counter_value};

    if (!info->auto_reload)
    {
        timer_counter_value += info->alarm_interval * EZLOPI_TIMER_SCALE;
        timer_group_set_alarm_value_in_isr(info->timer_group, info->timer_idx, timer_counter_value);
    }

    /* Now just send the event data back to the main program task */
    xQueueSendFromISR(s_timer_queue, &evt, &high_task_awoken);

    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
}


/*******************************************************************************
 *                          Static Function variables
 *******************************************************************************/
static bool notify_50_ms_state = false;
static bool notify_100_ms_state = false;
static bool notify_200_ms_state = false;
static bool notify_500_ms_state = false;

static int timer_group = 0;
static int timer_idx = 0;
static int alarm_value = 50;

void initiator(int tm_grp, int tm_idx, float al_val, bool *flag)
{
    *flag = 1;
    timer_group = tm_grp;
    timer_idx = tm_idx;
    alarm_value = (int)(al_val * EZLOPI_TIMER_SCALE);
}

void ezlopi_tg_timer_init(ezlopi_action_notify_t ezlopi_action)
{
    ezlopi_timer_info_t *timer_info = calloc(1, sizeof(ezlopi_timer_info_t));

    switch (ezlopi_action)
    {
    case EZLOPI_ACTIONS_NOTIFY_50_MS:
    {
        if (!notify_50_ms_state)
        {
            ESP_LOGI(EZLOPI_TIMER_TAG, "EZLOPI_ACTIONS_NOTIFY_50_MS assigned to timer group 0, index 0");
            timer_info->notify = EZLOPI_ACTIONS_NOTIFY_50_MS;
            initiator(EZLOPI_TIMER_GRP_0, EZLOPI_TIMER_IDX_0, 0.05, &notify_50_ms_state);
        }
        ESP_LOGE(EZLOPI_TIMER_TAG, "The timer was initialized!! continuing...");
        break;
    }
    case EZLOPI_ACTIONS_NOTIFY_100_MS:
    {
        if (!notify_100_ms_state)
        {
            ESP_LOGI(EZLOPI_TIMER_TAG, "EZLOPI_ACTIONS_NOTIFY_100_MS assigned to timer group 0, index 1");
            timer_info->notify = EZLOPI_ACTIONS_NOTIFY_100_MS;
            initiator(EZLOPI_TIMER_GRP_0, EZLOPI_TIMER_IDX_1, 0.1, &notify_100_ms_state);
        }
        ESP_LOGE(EZLOPI_TIMER_TAG, "The timer was initialized!! continuing...");
        break;
    }
    case EZLOPI_ACTIONS_NOTIFY_200_MS:
    {
        if (!notify_200_ms_state)
        {
            ESP_LOGI(EZLOPI_TIMER_TAG, "EZLOPI_ACTIONS_NOTIFY_200_MS assigned to timer group 1, index 0");
            timer_info->notify = EZLOPI_ACTIONS_NOTIFY_200_MS;
            initiator(EZLOPI_TIMER_GRP_1, EZLOPI_TIMER_IDX_0, 0.2, &notify_200_ms_state);
        }
        ESP_LOGE(EZLOPI_TIMER_TAG, "The timer was initialized!! continuing...");
        break;
    }
    case EZLOPI_ACTIONS_NOTIFY_500_MS:
    {
        if (!notify_500_ms_state)
        {
            ESP_LOGI(EZLOPI_TIMER_TAG, "EZLOPI_ACTIONS_NOTIFY_500_MS assigned to timer group 1, index 1");
            timer_info->notify = EZLOPI_ACTIONS_NOTIFY_500_MS;
            initiator(EZLOPI_TIMER_GRP_1, EZLOPI_TIMER_IDX_1, 0.5, &notify_500_ms_state);
        }
        ESP_LOGE(EZLOPI_TIMER_TAG, "The timer was initialized!! continuing...");
        break;
    }
    default:
    {
        ESP_LOGE(EZLOPI_TIMER_TAG, "Invalid action provided!!, %d", ezlopi_action);
        return;
        break;
    }
    }

    /* Select and initialize basic parameters of the timer */
    timer_config_t config = {
        .divider = EZLOPI_TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = true,
    }; // default clock source is APB

    timer_init(timer_group, timer_idx, &config);
    timer_set_counter_value(timer_group, timer_idx, 0);
    timer_set_alarm_value(timer_group, timer_idx, alarm_value);
    timer_enable_intr(timer_group, timer_idx);

    timer_info->timer_group = timer_group;
    timer_info->timer_idx = timer_idx;
    timer_info->auto_reload = true;
    timer_info->alarm_interval = alarm_value;

    timer_isr_callback_add(timer_group, timer_idx, timer_group_isr_callback, timer_info, 0);

    timer_start(timer_group, timer_idx);
}
