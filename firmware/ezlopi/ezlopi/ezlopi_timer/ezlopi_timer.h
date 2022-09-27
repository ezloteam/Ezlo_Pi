
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

#ifndef EZLOPI_TIMER_H
#define EZLOPI_TIMER_H

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/timer.h"
#include "esp_err.h"
#include "esp_log.h"

/*******************************************************************************
 *                          Type & Macro Declarations
 *******************************************************************************/
#define EZLOPI_TIMER_TAG "EZLOPI TIMER TAG"

#define EZLOPI_TIMER_GRP_0 TIMER_GROUP_0
#define EZLOPI_TIMER_GRP_1 TIMER_GROUP_1
#define EZLOPI_TIMER_IDX_0 TIMER_0
#define EZLOPI_TIMER_IDX_1 TIMER_1

#define EZLOPI_TIMER_DIVIDER (1000)                           //  Hardware timer clock divider
#define EZLOPI_TIMER_SCALE (TIMER_BASE_CLK / EZLOPI_TIMER_DIVIDER) // convert counter value to seconds


typedef enum
{
    EZLOPI_ACTIONS_NOTIFY_50_MS     = 2,
    EZLOPI_ACTIONS_NOTIFY_100_MS    = 3,
    EZLOPI_ACTIONS_NOTIFY_200_MS    = 4,
    EZLOPI_ACTIONS_NOTIFY_500_MS    = 5,
}ezlopi_action_notify_t;

typedef struct
{
    int timer_group;
    int timer_idx;
    int alarm_interval;
    bool auto_reload;
    ezlopi_action_notify_t notify;
} ezlopi_timer_info_t;

/**
 * @brief A structure to pass events from the timer ISR to task
 *
 */
typedef struct
{
    ezlopi_timer_info_t info;
    uint64_t timer_counter_value;
} ezlopi_timer_event_t;



xQueueHandle s_timer_queue;

void ezlopi_tg_timer_init(ezlopi_action_notify_t ezlopi_action);



#endif // EZLOPI_TIMER_H