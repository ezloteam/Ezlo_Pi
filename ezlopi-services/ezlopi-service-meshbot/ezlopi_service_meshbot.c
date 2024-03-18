#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_scenes_status_changed.h"
#include "ezlopi_core_scenes_v2.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_meshbot.h"
#include "pt.h"

typedef struct s_thread_ctx {
    struct pt pt;
    uint32_t curr_ticks;
    uint32_t start_cond;
    uint32_t stopped_cond;
    uint32_t delay_ms;
    l_action_block_v2_t* action_node;
    // l_scenes_list_v2_t* scene_node;
} s_thread_ctx_t;

static void __scenes_thread_process(void* pv);
// static void __scenes_process(void* arg);
// static int __execute_else_condition(l_scenes_list_v2_t* scene_node);
static char __scene_proto_thread(l_scenes_list_v2_t* scene_node, uint32_t routine_delay_ms);

uint32_t ezlopi_meshbot_service_stop_for_scene_id(uint32_t _id)
{
    uint32_t ret = 0;
    l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_by_id_v2(_id);
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

uint32_t ezlopi_meshobot_service_stop_scene(l_scenes_list_v2_t* scene_node)
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

uint32_t ezlopi_meshbot_service_start_scene(l_scenes_list_v2_t* scene_node)
{
    int ret = 0;
    if (scene_node)
    {
        if ((EZLOPI_SCENE_STATUS_NONE == scene_node->status) ||
            (EZLOPI_SCENE_STATUS_STOPPED == scene_node->status))
        {
            // xTaskCreate(__scenes_process, scene_node->name, 2 * 2048, scene_node, 2, NULL);
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
    l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_by_id_v2(_id);

    if (scene_node)
    {
        if (scene_node->then_block)
        {
            ezlopi_scenes_status_change_broadcast(scene_node, scene_status_started_str);



            // if (1 == __execute_then_condition(scene_node))
            if (1)
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
            // if (1 == __execute_else_condition(scene_node))
            if (1)
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

uint32_t ezlopi_meshbot_execute_scene_else_action_group(uint32_t scene_id)
{
    int ret = 0;
    l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_by_id_v2(scene_id);
    if (scene_node)
    {
        if (scene_node->else_block)
        {
            // if (1 == __execute_else_condition(scene_node))
            if (1)
            {
                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_finished_str);
            }
            else
            {
                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
                ret = 1;
            }
        }
    }
    else
    {
        ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
        ret = 1;
    }
    return ret;
}

void ezlopi_scenes_meshbot_init(void)
{
    uint32_t start_thread = 0;
    l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_scenes_head_v2();
    while (scene_node)
    {

        // if (scene_node->enabled && scene_node->when_block && (scene_node->else_block || scene_node->then_block))
        if (scene_node->when_block && (scene_node->else_block || scene_node->then_block))
        {
            start_thread = 1;

            s_thread_ctx_t* thread_ctx = malloc(sizeof(s_thread_ctx_t));
            if (thread_ctx)
            {
                memset(thread_ctx, 0, sizeof(s_thread_ctx_t));
                PT_INIT(&thread_ctx->pt);
                scene_node->arg = (void*)thread_ctx;
                scene_node->status = EZLOPI_SCENE_STATUS_RUN;
            }
        }
        else
        {
            scene_node->status = EZLOPI_SCENE_STATUS_STOPPED;
        }

        scene_node = scene_node->next;
    }

    if (start_thread)
    {
        TRACE_D("starting thread process");
        xTaskCreate(__scenes_thread_process, "Scene-task", 2 * 2048, NULL, 2, NULL);
    }
    else
    {
        TRACE_E("scene not available!");
    }
}


PT_THREAD(__scene_proto_thread(l_scenes_list_v2_t* scene_node, uint32_t routine_delay_ms))
{
    s_thread_ctx_t* ctx = scene_node->arg;
    PT_BEGIN(&ctx->pt);

    while (1)
    {
        if ((EZLOPI_SCENE_STATUS_RUN == scene_node->status) || (EZLOPI_SCENE_STATUS_RUNNING == scene_node->status))
        {
            scene_node->status = EZLOPI_SCENE_STATUS_RUNNING;

            uint32_t when_condition_returned = 0;
            l_when_block_v2_t* when_condition_node = scene_node->when_block;

            if (when_condition_node)
            {
                f_scene_method_v2_t when_method = ezlopi_scene_get_method_v2(when_condition_node->block_options.method.type);
                if (when_method)
                {
                    when_condition_returned = when_method(scene_node, (void*)when_condition_node);
                    if (when_condition_returned)
                    {
                        TRACE_D("here");
                        if (ctx->start_cond < 2)
                        {
                            TRACE_D("here");
                            ctx->stopped_cond = 0;

                            if (ctx->start_cond)
                            {
                                TRACE_D("here");
                                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_started_str);
                            }

                            l_action_block_v2_t* then_block_node = scene_node->then_block;
                            while (then_block_node)
                            {
                                uint32_t delay_ms = (then_block_node->delay.days * (24 * 60 * 60)
                                    + then_block_node->delay.hours * (60 * 60)
                                    + then_block_node->delay.minutes * 60
                                    + then_block_node->delay.seconds) * 1000;

                                if (delay_ms)
                                {
                                    ctx->delay_ms = delay_ms;
                                    ctx->action_node = then_block_node;

                                    ctx->curr_ticks = xTaskGetTickCount();
                                    TRACE_D("entering delay: %d", ctx->curr_ticks);
                                    PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > ctx->delay_ms);
                                    TRACE_D("exiting delay: %d", xTaskGetTickCount());

                                    then_block_node = ctx->action_node;
                                }

                                f_scene_method_v2_t then_method = ezlopi_scene_get_method_v2(then_block_node->block_options.method.type);
                                TRACE_D("then-method: %p", then_method);
                                if (then_method)
                                {
                                    then_method(scene_node, (void*)then_block_node); //then method executed here

                                    if (then_block_node->next)
                                    {
                                        ezlopi_scenes_status_change_broadcast(scene_node, scene_status_partially_finished_str);
                                    }
                                    else
                                    {
                                        ezlopi_scenes_status_change_broadcast(scene_node, scene_status_finished_str);
                                    }

                                    ctx->start_cond += 1;
                                }
                                else
                                {
                                    ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
                                }

                                vTaskDelay(10);

                                then_block_node = then_block_node->next;
                            }
                        }
                        else
                        {
                            TRACE_D("Meshobot '%s' is Idle.", scene_node->name);
                        }
                    }
                    else if (ctx->stopped_cond < 2)
                    {
                        l_action_block_v2_t* else_block_node = scene_node->else_block;
                        while (else_block_node)
                        {
                            uint32_t delay_ms = (else_block_node->delay.days * (24 * 60 * 60)
                                + else_block_node->delay.hours * (60 * 60)
                                + else_block_node->delay.minutes * 60
                                + else_block_node->delay.seconds) * 1000;

                            if (delay_ms)
                            {
                                ctx->delay_ms = delay_ms;
                                ctx->action_node = else_block_node;

                                ctx->curr_ticks = xTaskGetTickCount();
                                TRACE_D("entering delay: %d", ctx->curr_ticks);
                                PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > ctx->delay_ms);
                                TRACE_D("exiting delay: %d", xTaskGetTickCount());

                                else_block_node = ctx->action_node;
                            }

                            f_scene_method_v2_t else_method = ezlopi_scene_get_method_v2(else_block_node->block_options.method.type);
                            TRACE_D("else-method: %p", else_method);
                            if (else_method)
                            {
                                else_method(scene_node, (void*)else_block_node);
                            }

                            else_block_node = else_block_node->next;
                        }

                        ezlopi_scenes_status_change_broadcast(scene_node, scene_status_stopped_str);

                        ctx->stopped_cond += 1;
                        ctx->start_cond = 0;

                    }
                    else
                    {
                        TRACE_D("Meshobot '%s' is Idle.", scene_node->name);
                    }
                }
                else
                {
                    TRACE_E("method not found");
                }

                when_condition_node = when_condition_node->next;
            }
        }

        if (EZLOPI_SCENE_STATUS_STOP == scene_node->status)
        {
            free(scene_node->arg);
            scene_node->arg = NULL;
            scene_node->status = EZLOPI_SCENE_STATUS_STOPPED;
            break;
        }

        ctx->curr_ticks = xTaskGetTickCount();
        TRACE_D("entering delay: %d", ctx->curr_ticks);
        PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > routine_delay_ms);
        TRACE_D("exiting delay: %d", xTaskGetTickCount());
    }

    PT_END(&ctx->pt);
}

static void __scenes_thread_process(void* pv)
{
    while (1)
    {
        l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_scenes_head_v2();
        while (scene_node)
        {
            if (scene_node->arg)
            {
                __scene_proto_thread(scene_node, 1000); // 
            }

            vTaskDelay(100);
            scene_node = scene_node->next;
        }
    }
}

#if 0
static void __scenes_process(void* arg)
{
    l_scenes_list_v2_t* scene_node = (l_scenes_list_v2_t*)arg;
    scene_node->status = EZLOPI_SCENE_STATUS_RUN;
    // TRACE_I("task - '%s': Running", scene_node->name);
    uint32_t stopped_condition_count = 0;
    uint32_t started_condition_fired_count = 0;

    while (1)
    {
        if ((EZLOPI_SCENE_STATUS_RUN == scene_node->status) || (EZLOPI_SCENE_STATUS_RUNNING == scene_node->status))
        {
            scene_node->status = EZLOPI_SCENE_STATUS_RUNNING;

            uint32_t when_condition_returned = 0;
            l_when_block_v2_t* when_condition_node = scene_node->when_block;

            if (when_condition_node)
            {
                f_scene_method_v2_t when_method = ezlopi_scene_get_method_v2(when_condition_node->block_options.method.type);
                if (when_method)
                {
                    when_condition_returned = when_method(scene_node, (void*)when_condition_node);
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

                        TRACE_I("here");
                    }
                    else if (stopped_condition_count < 2)
                    {
                        __execute_else_condition(scene_node);
                        if (ezlopi_scenes_status_change_broadcast(scene_node, scene_status_stopped_str))
                        {
                            stopped_condition_count += 1;
                        }

                        started_condition_fired_count = 0;
                        TRACE_I("here");
                    }
                    else
                    {
                        TRACE_D("Meshobot '%s' is Idle.", scene_node->name);
                    }
                }
                else {
                    TRACE_E("method not found");
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
#endif


static int __execute_else_condition(l_scenes_list_v2_t* scene_node)
{
    int ret = 0;
    s_thread_ctx_t* ctx = (s_thread_ctx_t*)scene_node->arg;
    l_action_block_v2_t* else_node = scene_node->else_block;
    while (else_node)
    {
        uint32_t delay_ms = (else_node->delay.days * (24 * 60 * 60) + else_node->delay.hours * (60 * 60) + else_node->delay.minutes * 60 + else_node->delay.seconds) * 1000;
        TRACE_D("scene-delay_ms: %d", delay_ms);
        if (delay_ms)
        {
            // vTaskDelay(delay_ms / portTICK_RATE_MS);
            ctx->curr_ticks = xTaskGetTickCount();
            // PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > delay_ms);
        }

        const char* method_name = ezlopi_scene_get_scene_method_name(else_node->block_options.method.type);
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
            else_method(scene_node, (void*)else_node);
        }

        else_node = else_node->next;
    }

    return ret;
}
