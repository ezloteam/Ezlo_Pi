#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "trace.h"

#include "ezlopi_scenes_v2.h"
#include "ezlopi_meshbot_service.h"

static void __scenes_process(void *arg);

uint32_t ezlopi_scenes_service_stop_for_scene_id(uint32_t _id)
{
    uint32_t ret = 0;
    l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_scenes_head_v2();
    while (scene_node)
    {
        if (_id == scene_node->_id)
        {
            scene_node->enabled = 0;
            scene_node->status = EZLOPI_SCENE_STATUS_STOP;
            ret = 1;
        }
        scene_node = scene_node->next;
    }

    return ret;
}

uint32_t ezlopi_scenes_meshbot_delete_by_id(uint32_t _id)
{
    uint32_t ret = 0;
    l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_scenes_head_v2();
    while (scene_node)
    {
        if (_id == scene_node->_id)
        {
            scene_node->enabled = 0;
            if (EZLOPI_SCENE_STATUS_RUNNING == scene_node->status)
            {
                scene_node->status = EZLOPI_SCENE_STATUS_DELETE;
            }
            else
            {
                ezlopi_scenes_delete(ezlopi_scenes_pop_by_id_v2(_id));
            }
            ret = 1;
        }
        scene_node = scene_node->next;
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

// uint32_t ezlopi_scenes_service_start_for_scene_id(uint32_t _id)
// {
//     uint32_t ret = 0;
//     l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_scenes_list();
//     while (scene_node)
//     {
//         if (_id == scene_node->_id)
//         {
//             scene_node->enabled = 0;
//             scene_node->status = EZLOPI_SCENE_STATUS_RUN;
//             ret = 1;
//         }
//         scene_node = scene_node->next;
//     }
//     return ret;
// }

// void ezlopi_scenes_service_reinit(void)
// {
//     l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_scenes_list();
//     while (scene_node)
//     {
//         scene_node->status = EZLOPI_SCENE_STATUS_STOP;
//         vTaskDelay(20);
//         if (scene_node->enabled)
//         {
//             xTaskCreate(__scenes_process, scene_node->name, 2 * 2048, scene_node, 2, &scene_node->task_handle);
//         }
//         scene_node = scene_node->next;
//     }
// }

void ezlopi_scenes_meshbot_init(void)
{
    l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_scenes_head_v2();
    while (scene_node)
    {
        if (scene_node->enabled)
        {
            xTaskCreate(__scenes_process, scene_node->name, 2 * 2048, scene_node, 2, NULL);
        }
        scene_node = scene_node->next;
    }
}

static void __scenes_process(void *arg)
{
    l_scenes_list_v2_t *scene_node = (l_scenes_list_v2_t *)arg;
    scene_node->status = EZLOPI_SCENE_STATUS_RUN;
    TRACE_B("task - '%s': Running", scene_node->name);

    while (1)
    {
        if (EZLOPI_SCENE_STATUS_RUN == scene_node->status)
        {
            l_when_block_v2_t *when_condition_node = scene_node->when;
            while (when_condition_node)
            {
                f_scene_method_v2_t when_method = ezlopi_scene_get_method_v2(when_condition_node->block_options.method.type);
                if (when_method)
                {
                    int ret = when_method(scene_node, (void *)when_condition_node);
                    if (ret)
                    {
                        TRACE_B("When condition is true.");
                        l_then_block_v2_t *curr_then = scene_node->then;
                        while (curr_then)
                        {
                            uint32_t delay_ms = (curr_then->delay.days * (24 * 60 * 60) + curr_then->delay.hours * (60 * 60) + curr_then->delay.minutes * 60 + curr_then->delay.seconds) * 1000;
                            if (delay_ms)
                            {
                                vTaskDelay(delay_ms / portTICK_RATE_MS);
                            }

                            TRACE_D("delay_ms: %d", delay_ms);
                            const char *method_name = ezlopi_scene_get_scene_method_name(curr_then->block_options.method.type);
                            if (method_name)
                            {
                                TRACE_D("Calling: %s", method_name);
                            }
                            else
                            {
                                TRACE_E("Error: Method is NULL!");
                            }

                            f_scene_method_v2_t then_method = ezlopi_scene_get_method_v2(curr_then->block_options.method.type);
                            TRACE_D("then-method: %p", then_method);
                            if (then_method)
                            {
                                ret = then_method(scene_node, (void *)curr_then);
                            }

                            curr_then = curr_then->next;

                            if ((EZLOPI_SCENE_STATUS_STOP == scene_node->status) || (EZLOPI_SCENE_STATUS_DELETE == scene_node->status))
                            {
                                break;
                            }
                        }
                    }
                }

                if ((EZLOPI_SCENE_STATUS_STOP == scene_node->status) || (EZLOPI_SCENE_STATUS_DELETE == scene_node->status))
                {
                    break;
                }
                when_condition_node = when_condition_node->next;
            }

            if ((EZLOPI_SCENE_STATUS_STOP == scene_node->status) || (EZLOPI_SCENE_STATUS_DELETE == scene_node->status))
            {
                break;
            }
        }

        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    scene_node->task_handle = NULL;
    scene_node->enabled = false;

    if ((EZLOPI_SCENE_STATUS_STOP == scene_node->status) || (EZLOPI_SCENE_STATUS_DELETE == scene_node->status))
    {
        ezlopi_scenes_delete(ezlopi_scenes_pop_by_id_v2(scene_node->_id));
    }
    else
    {
        scene_node->status = EZLOPI_SCENE_STATUS_STOPPED;
    }

    vTaskDelete(NULL);
}
