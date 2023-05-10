
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
#include "trace.h"
#include "ezlopi_event_queue.h"

typedef struct s_ezlopi_timer
{
    timer_config_t internal;
    int group;
    int index;
    int alarm_value;
    e_ezlopi_actions_t event_type;
} s_ezlopi_timer_t;

/*******************************************************************************
 *                          Static Function variables
 *******************************************************************************/

#define TIMER_ERROR_CHECK(err, message) \
    {                                   \
        if (ESP_OK != err)              \
        {                               \
            TRACE_E(message);           \
            ret = 0;                    \
        }                               \
    }

#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32
#define MAX_TIMER 4
const static int timer_group_index_pair[MAX_TIMER][2] = {
    {EZLOPI_TIMER_GRP_0, EZLOPI_TIMER_IDX_0},
    {EZLOPI_TIMER_GRP_0, EZLOPI_TIMER_IDX_1},
    {EZLOPI_TIMER_GRP_1, EZLOPI_TIMER_IDX_0},
    {EZLOPI_TIMER_GRP_1, EZLOPI_TIMER_IDX_1},
};
#elif CONFIG_IDF_TARGET_ESP32C3
#define MAX_TIMER 2
const static int timer_group_index_pair[MAX_TIMER][2] = {
    {EZLOPI_TIMER_GRP_0, EZLOPI_TIMER_IDX_0},
    {EZLOPI_TIMER_GRP_1, EZLOPI_TIMER_IDX_0},
};
#endif

static void send_event_to_queue(e_ezlopi_actions_t action)
{
    s_ezlo_event_t *event_data = malloc(sizeof(s_ezlo_event_t));
    if (NULL != event_data)
    {
        event_data->arg = NULL;
        event_data->action = action;
        if (0 == ezlopi_event_queue_send(event_data, true))
        {
            free(event_data);
        }
    }
}

/*******************************************************************************
 *                          Static Function Definition
 *******************************************************************************/
static bool IRAM_ATTR timer_group_isr_callback(void *args)
{
    BaseType_t high_task_awoken = pdFALSE;
    s_ezlopi_timer_t *_timer_conf = (s_ezlopi_timer_t *)args;

    uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(_timer_conf->group, _timer_conf->index);

    if (!_timer_conf->internal.auto_reload)
    {
        timer_counter_value += _timer_conf->alarm_value * EZLOPI_TIMER_SCALE;
        timer_group_set_alarm_value_in_isr(_timer_conf->group, _timer_conf->index, timer_counter_value);
    }
#if 0
    if (EZLOPI_ACTION_NOTIFY_1000_MS == _timer_conf->event_type)
    {
        send_event_to_queue(EZLOPI_ACTION_NOTIFY_1000_MS);
    }

    if (EZLOPI_ACTION_NOTIFY_200_MS == _timer_conf->event_type)
    {
        static int count;
        if (0 == (count % 5))
        {
            send_event_to_queue(EZLOPI_ACTION_NOTIFY_1000_MS);
            count = 0;
        }
        count++;
    }
#endif

    if (EZLOPI_ACTION_NOTIFY_50_MS == _timer_conf->event_type)
    {
        static int count;
        // send_event_to_queue(EZLOPI_ACTION_NOTIFY_50_MS);

        // if (0 == (count % 2)) // 100 ms
        // {
        //     send_event_to_queue(EZLOPI_ACTION_NOTIFY_100_MS);
        // }

        // if (0 == (count % 4)) // 200 ms
        // {
        //     send_event_to_queue(EZLOPI_ACTION_NOTIFY_200_MS);
        // }

        // if (0 == (count % 10)) // 500 ms
        // {
        //     send_event_to_queue(EZLOPI_ACTION_NOTIFY_500_MS);
        // }

        if (0 == (count % 20)) // 1000 ms
        {
            send_event_to_queue(EZLOPI_ACTION_NOTIFY_1000_MS);
            count = 0;
        }

        count++;
    }

    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
}

static int ezlopi_timer_alarm_enable(s_ezlopi_timer_t *timer_conf);
static void ezlopi_timer_init_timer_event(int timer_num, int time_ms, e_ezlopi_actions_t event_type);
static void ezlopi_timer_setup_struct(s_ezlopi_timer_t *timer_config, e_ezlopi_actions_t event_type, int group, int index, int alarm_ms);

#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32
void ezlopi_timer_start_50ms(void)
{
    ezlopi_timer_init_timer_event(0, 50, EZLOPI_ACTION_NOTIFY_50_MS);
}

// void ezlopi_timer_start_200ms(void)
// {
//     ezlopi_timer_init_timer_event(1, 200, EZLOPI_ACTION_NOTIFY_200_MS);
// }

// void ezlopi_timer_start_500ms(void)
// {
//     ezlopi_timer_init_timer_event(2, 500, EZLOPI_ACTION_NOTIFY_500_MS);
// }

void ezlopi_timer_start_1000ms(void)
{
    ezlopi_timer_init_timer_event(3, 1000, EZLOPI_ACTION_NOTIFY_1000_MS);
}
#else
void ezlopi_timer_start_1000ms(void)
{
    ezlopi_timer_init_timer_event(0, 1000, EZLOPI_ACTION_NOTIFY_1000_MS);
}
#endif

static void ezlopi_timer_init_timer_event(int timer_num, int time_ms, e_ezlopi_actions_t event_type)
{
    if (timer_num < MAX_TIMER)
    {
        s_ezlopi_timer_t *timer_config = malloc(sizeof(s_ezlopi_timer_t));

        if (timer_config)
        {
            ezlopi_timer_setup_struct(timer_config, event_type, timer_group_index_pair[timer_num][0], timer_group_index_pair[timer_num][1], time_ms);
            ezlopi_timer_alarm_enable(timer_config);
        }
        else
        {
            TRACE_E("Timer config malloc failed!");
        }
    }
    else
    {
        TRACE_E("Error: Timer max!");
    }
}

static void ezlopi_timer_setup_struct(s_ezlopi_timer_t *timer_config, e_ezlopi_actions_t event_type, int group, int index, int alarm_ms)
{
    timer_config->event_type = event_type;
    timer_config->alarm_value = (alarm_ms * (EZLOPI_TIMER_SCALE / 1000));
    timer_config->group = group;
    timer_config->index = index;
    timer_config->internal.alarm_en = TIMER_ALARM_EN,
    timer_config->internal.auto_reload = true,
    timer_config->internal.counter_dir = TIMER_COUNT_UP;
    timer_config->internal.divider = EZLOPI_TIMER_DIVIDER;
    timer_config->internal.intr_type = TIMER_INTR_LEVEL;
}

static int ezlopi_timer_alarm_enable(s_ezlopi_timer_t *timer_conf)
{
    int ret = 1;

    TIMER_ERROR_CHECK(timer_init(timer_conf->group, timer_conf->index, &timer_conf->internal), "timer_init failed!");
    TIMER_ERROR_CHECK(timer_set_counter_value(timer_conf->group, timer_conf->index, 0), "timer_set_counter_value failed!");
    TIMER_ERROR_CHECK(timer_set_alarm_value(timer_conf->group, timer_conf->index, timer_conf->alarm_value), "timer_set_alarm_value failed!");
    TIMER_ERROR_CHECK(timer_enable_intr(timer_conf->group, timer_conf->index), "timer_enable_intr failed!");
    TIMER_ERROR_CHECK(timer_isr_callback_add(timer_conf->group, timer_conf->index, timer_group_isr_callback, timer_conf, 0), "timer_isr_callback_add failed!");
    TIMER_ERROR_CHECK(timer_start(timer_conf->group, timer_conf->index), "timer_start failed!");

    return ret;
}
