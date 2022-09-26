



#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/timer.h"
#include "esp_err.h"
#include "esp_log.h"

#ifndef EZLOPI_TIMER_H
#define EZLOPI_TIMER_H

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

bool IRAM_ATTR timer_group_isr_callback(void *args);
void ezlopi_tg_timer_init(int group, int timer, bool auto_reload, float notify_in_sec);

#endif // EZLOPI_TIMER_H