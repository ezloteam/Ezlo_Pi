

#include "ezlopi_timer.h"


bool IRAM_ATTR timer_group_isr_callback(void *args)
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
        .timer_counter_value = timer_counter_value
    };

    if (!info->auto_reload)
    {
        timer_counter_value += info->alarm_interval * EZLOPI_TIMER_SCALE;
        timer_group_set_alarm_value_in_isr(info->timer_group, info->timer_idx, timer_counter_value);
    }

    /* Now just send the event data back to the main program task */
    xQueueSendFromISR(s_timer_queue, &evt, &high_task_awoken);

    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
}



void ezlopi_tg_timer_init(int group, int timer, bool auto_reload, float notify_in_sec)
{
    int alarm_value = (int)(notify_in_sec * EZLOPI_TIMER_SCALE);
    ESP_LOGE(EZLOPI_TIMER_TAG, "The alarm value are: %d", alarm_value);
    /* Select and initialize basic parameters of the timer */
    timer_config_t config = {
        .divider = EZLOPI_TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = auto_reload,
    }; // default clock source is APB
    timer_init(group, timer, &config);

    timer_set_counter_value(group, timer, 0);
    timer_set_alarm_value(group, timer, alarm_value);
    timer_enable_intr(group, timer);

    ezlopi_timer_info_t *timer_info = calloc(1, sizeof(ezlopi_timer_info_t));
    timer_info->timer_group = group;
    timer_info->timer_idx = timer;
    timer_info->auto_reload = auto_reload;
    timer_info->alarm_interval = notify_in_sec;
    if((int)(notify_in_sec*100) == (int)(0.05*100))
    {
        timer_info->notify = EZLOPI_ACTIONS_NOTIFY_50_MS;
    }
    else if ((int)(notify_in_sec * 100) == (int)(0.1 * 100))
    {
        timer_info->notify = EZLOPI_ACTIONS_NOTIFY_100_MS;
    }
    else if ((int)(notify_in_sec * 100) == (int)(0.2 * 100))
    {
        timer_info->notify = EZLOPI_ACTIONS_NOTIFY_200_MS;
    }
    else if ((int)(notify_in_sec * 100) == (int)(0.5 * 100))
    {
        timer_info->notify = EZLOPI_ACTIONS_NOTIFY_500_MS;
    }
    else
    {
        ESP_LOGE(EZLOPI_TIMER_TAG, "Invalid alarm time received, setting to defult!");
        timer_info->notify = EZLOPI_ACTIONS_NOTIFY_500_MS;
    }
    timer_isr_callback_add(group, timer, timer_group_isr_callback, timer_info, 0);

    timer_start(group, timer);
}

