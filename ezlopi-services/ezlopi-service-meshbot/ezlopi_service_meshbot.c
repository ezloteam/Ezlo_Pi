#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "trace.h"

#include "ezlopi_core_scenes_status_changed.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_meshbot.h"

static void __scenes_process(void *arg);
static int __execute_then_condition(l_scenes_list_v2_t *scene_node);
static int __execute_else_condition(l_scenes_list_v2_t *scene_node);

uint32_t ezlopi_meshbot_service_stop_for_scene_id(uint32_t _id)
{
    uint32_t ret = 0;
    l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_by_id_v2(_id);
    if (scene_node)
    {
        if (EZLOPI_SCENE_STATUS_RUNNING == scene_node->status)
        {
            scene_node->status = EZLOPI_SCENE_STATUS_STOP;
        }

        while (EZLOPI_SCENE_STATUS_STOPPED != scene_node->status)
        {
            vTaskDelay(50 / portTICK_RATE_MS);
        }

        ezlopi_scenes_status_change_broadcast(scene_node, scene_status_stopped_str);

        ret = 1;
    }

    return ret;
}

uint32_t ezlopi_meshobot_service_stop_scene(l_scenes_list_v2_t *scene_node)
{
    int ret = 0;
    if (scene_node)
    {
        if (EZLOPI_SCENE_STATUS_RUNNING == scene_node->status)
        {
            scene_node->status = EZLOPI_SCENE_STATUS_STOP;
        }

        while (EZLOPI_SCENE_STATUS_STOPPED != scene_node->status)
        {
            vTaskDelay(50 / portTICK_RATE_MS);
        }

        ezlopi_scenes_status_change_broadcast(scene_node, scene_status_stopped_str);

        ret = 1;
    }
    return ret;
}

uint32_t ezlopi_meshbot_service_start_scene(l_scenes_list_v2_t *scene_node)
{
    int ret = 0;
    if (scene_node)
    {
        if ((EZLOPI_SCENE_STATUS_NONE == scene_node->status) ||
            (EZLOPI_SCENE_STATUS_STOPPED == scene_node->status))
        {
            xTaskCreate(__scenes_process, scene_node->name, 2 * 2048, scene_node, 2, NULL);
            ret = 1;
        }
    }
    return ret;
}

// uint32_t ezlopi_scenes_service_pause_for_scene_id(uint32_t _id)
// {
//     uint32_t ret = 0;
//     l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_scenes_head_v2();
//     while (scene_node)
//     {
//         if (_id == scene_node->_id)
//         {
//             scene_node->enabled = 0;
//             scene_node->status = EZLOPI_SCENE_STATUS_PAUSE;
//             ret = 1;
//         }
//         scene_node = scene_node->next;
//     }
//     return ret;
// }

uint32_t ezlopi_scenes_service_run_by_id(uint32_t _id)
{
    uint32_t ret = 0;
    TRACE_D("Scene-id: %d", _id);
    l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_by_id_v2(_id);

    if (scene_node)
    {
        if (scene_node->then_block)
        {
            ezlopi_scenes_status_change_broadcast(scene_node, scene_status_started_str);
            if (1 == __execute_then_condition(scene_node))
            {
                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_finished_str);
            }
            else
            {
                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
            }
        }
        else if (scene_node->else_block)
        {
            if (1 == __execute_else_condition(scene_node))
            {
                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_finished_str);
            }
            else
            {
                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
            }
        }
        else
        {
            ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
        }
    }

    return ret;
}

void ezlopi_scenes_meshbot_init(void)
{
    l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_scenes_head_v2();
    while (scene_node)
    {
        if (scene_node->enabled && scene_node->when_block && (scene_node->else_block || scene_node->then_block))
        {
            xTaskCreate(__scenes_process, scene_node->name, 2 * 2048, scene_node, 2, NULL);
        }
        else
        {
            scene_node->status = EZLOPI_SCENE_STATUS_STOPPED;
        }
        scene_node = scene_node->next;
    }
}

static void __scenes_process(void *arg)
{
    l_scenes_list_v2_t *scene_node = (l_scenes_list_v2_t *)arg;
    scene_node->status = EZLOPI_SCENE_STATUS_RUN;
    TRACE_B("task - '%s': Running", scene_node->name);
    uint32_t stopped_condition_count = 0;
    uint32_t started_condition_fired_count = 0;

    while (1)
    {
        if ((EZLOPI_SCENE_STATUS_RUN == scene_node->status) || (EZLOPI_SCENE_STATUS_RUNNING == scene_node->status))
        {
            scene_node->status = EZLOPI_SCENE_STATUS_RUNNING;

            uint32_t when_condition_returned = 0;
            l_when_block_v2_t *when_condition_node = scene_node->when_block;

            if (when_condition_node)
            {
                f_scene_method_v2_t when_method = ezlopi_scene_get_method_v2(when_condition_node->block_options.method.type);
                if (when_method)
                {
                    when_condition_returned = when_method(scene_node, (void *)when_condition_node);
                    if (when_condition_returned)
                    {
                        if (started_condition_fired_count < 2)
                        {
                            stopped_condition_count = 0;

                            int write_status = 1;

                            if (started_condition_fired_count)
                            {
                                if (0 == ezlopi_scenes_status_change_broadcast(scene_node, scene_status_started_str))
                                {
                                    write_status = 0;
                                }
                            }

                            if (1 == __execute_then_condition(scene_node))
                            {
                                if (0 == ezlopi_scenes_status_change_broadcast(scene_node, scene_status_finished_str))
                                {
                                    write_status = 0;
                                }
                            }
                            else
                            {
                                if (0 == ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str))
                                {
                                    write_status = 0;
                                }
                            }

                            if (write_status)
                            {
                                started_condition_fired_count += 1;
                            }
                        }
                        else
                        {
                            TRACE_D("Meshobot '%s' is Idle.", scene_node->name);
                        }
                    }
                    else if (stopped_condition_count < 2)
                    {
                        __execute_else_condition(scene_node);
                        if (ezlopi_scenes_status_change_broadcast(scene_node, scene_status_stopped_str))
                        {
                            stopped_condition_count += 1;
                        }

                        started_condition_fired_count = 0;
                    }
                    else
                    {
                        TRACE_D("Meshobot '%s' is Idle.", scene_node->name);
                    }
                }

                when_condition_node = when_condition_node->next;
            }
        }

        if (EZLOPI_SCENE_STATUS_STOP == scene_node->status)
        {
            break;
        }

        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    scene_node->task_handle = NULL;
    scene_node->status = EZLOPI_SCENE_STATUS_STOPPED;

    vTaskDelete(NULL);
}

static int __execute_then_condition(l_scenes_list_v2_t *scene_node)
{
    int ret = 0;
    l_action_block_v2_t *then_node = scene_node->then_block;
    while (then_node)
    {
        uint32_t delay_ms = (then_node->delay.days * (24 * 60 * 60) + then_node->delay.hours * (60 * 60) + then_node->delay.minutes * 60 + then_node->delay.seconds) * 1000;
        TRACE_D("scene-delay_ms: %d", delay_ms);
        if (delay_ms)
        {
            vTaskDelay(delay_ms / portTICK_RATE_MS);
        }

        const char *method_name = ezlopi_scene_get_scene_method_name(then_node->block_options.method.type);
        if (method_name)
        {
            TRACE_D("Calling: %s", method_name);
        }
        else
        {
            TRACE_E("Error: Method is NULL!");
        }

        f_scene_method_v2_t then_method = ezlopi_scene_get_method_v2(then_node->block_options.method.type);
        TRACE_D("then-method: %p", then_method);
        if (then_method)
        {
            ret = 1;
            then_method(scene_node, (void *)then_node);
        }

        if (then_node->next)
        {
            ezlopi_scenes_status_change_broadcast(scene_node, scene_status_partially_finished_str);
        }

        then_node = then_node->next;
    }

    return ret;
}

static int __execute_else_condition(l_scenes_list_v2_t *scene_node)
{
    int ret = 0;
    l_action_block_v2_t *else_node = scene_node->else_block;
    while (else_node)
    {
        uint32_t delay_ms = (else_node->delay.days * (24 * 60 * 60) + else_node->delay.hours * (60 * 60) + else_node->delay.minutes * 60 + else_node->delay.seconds) * 1000;
        TRACE_D("scene-delay_ms: %d", delay_ms);
        if (delay_ms)
        {
            vTaskDelay(delay_ms / portTICK_RATE_MS);
        }

        const char *method_name = ezlopi_scene_get_scene_method_name(else_node->block_options.method.type);
        if (method_name)
        {
            TRACE_D("Calling: %s", method_name);
        }
        else
        {
            TRACE_E("Error: Method is NULL!");
        }

        f_scene_method_v2_t else_method = ezlopi_scene_get_method_v2(else_node->block_options.method.type);
        TRACE_D("else-method: %p", else_method);
        if (else_method)
        {
            ret = 1;
            else_method(scene_node, (void *)else_node);
        }

        else_node = else_node->next;
    }

    return ret;
}
