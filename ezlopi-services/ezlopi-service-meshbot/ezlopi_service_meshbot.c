/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
 * @file    ezlopi_service_meshbot.c
 * @brief   Contains function definitions for meshbot related operations
 * @authors Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 1.0
 * @date    October 20th, 2023 7:50 PM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_trace.h"
#include "EZLOPI_USER_CONFIG.h"
#include "pt.h"

#include "ezlopi_core_scenes_status_changed.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_sntp.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_loop.h"
#include "ezlopi_service_meshbot.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/**
 * @brief Struct that holds the thread context
 *
 */
typedef struct s_thread_ctx
{
    struct pt pt;                     /**< P-thread structure */
    uint32_t curr_ticks;              /**< Current thread ticks passed */
    uint32_t start_cond;              /**< Start condition */
    uint32_t stopped_cond;            /**< Stopped condition */
    uint32_t delay_ms;                /**< Delay for the thread */
    l_action_block_v2_t *action_node; /**< Action node for thread context */
} s_thread_ctx_t;
/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
/**
 * @brief Function registered as function loop
 *
 * @param arg Function loop argument
 */
static void __scenes_loop(void *arg);
/**
 * @brief Proth-thread function for scene
 *
 * @param[in] scene_node Scene node proto-thread is linked to
 * @param[in] routine_delay_ms Dealy in ms for proto-thread to wait for
 * @return char
 */
static char ezpi_scene_proto_thread(l_scenes_list_v2_t *scene_node, uint32_t routine_delay_ms);
/**
 * @brief Function to perfrom scene stop operation
 *
 * @param[in] scene_node Pointer to the scene to stop
 * @return int
 */
static int ezpi_execute_scene_stop(l_scenes_list_v2_t *scene_node);
/**
 * @brief Function to perfrom scene start operation
 *
 * @param[in] scene_node Pointer to the scene to start
 * @return int
 */
static int ezpi_execute_scene_start(l_scenes_list_v2_t *scene_node);
/**
 * @brief Function to perfrom operation on action block
 *
 * @param[in] scene_node Pointer to the scene whose action block is to be executed
 * @param[in] action_block Pointer to the action block to execute
 * @return int
 */
static int ezpi_execute_action_block(l_scenes_list_v2_t *scene_node, l_action_block_v2_t *action_block);
/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
uint32_t EZPI_meshbot_service_stop_for_scene_id(uint32_t _id)
{
    uint32_t ret = 0;
    if (EZPI_meshobot_service_stop_scene(EZPI_core_scenes_get_by_id_v2(_id)))
    {
        ret = 1;
    }
    return ret;
}

uint32_t EZPI_meshobot_service_stop_scene(l_scenes_list_v2_t *scene_node)
{
    int ret = 0;
    if (scene_node)
    {
        if (EZPI_SUCCESS == EZPI_meshbot_stop_without_broadcast(scene_node))
        {
            // triggering broadcast
            EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_stopped_str, EZPI_core_sntp_get_current_time_sec());
            ret = 1;
        }
    }
    return ret;
}

ezlopi_error_t EZPI_meshbot_stop_without_broadcast(l_scenes_list_v2_t *scene_node)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (scene_node)
    {
        if (ezpi_execute_scene_stop(scene_node))
        {
            // TRACE_S("stop scene_id : %#x [%d] ", scene_node->_id, scene_node->status);
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

uint32_t EZPI_meshbot_service_start_scene(l_scenes_list_v2_t *scene_node)
{
    int ret = 0;
    if (scene_node)
    {
        if (ezpi_execute_scene_start(scene_node))
        {
            // TRACE_S("start scene_id : %#x [%d] ", scene_node->_id, scene_node->status);
            EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_started_str, EZPI_core_sntp_get_current_time_sec());
            ret = 1;
        }
    }
    return ret;
}

uint32_t EZPI_scenes_service_run_by_id(uint32_t _id) // Run once
{
    uint32_t ret = 0;
    l_scenes_list_v2_t *scene_node = EZPI_core_scenes_get_by_id_v2(_id);
    // TRACE_S("Scene-id:  %#x", _id);

    if (scene_node)
    {
        if (ezpi_execute_scene_stop(scene_node))
        {
            if (scene_node->then_block)
            {
                EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_started_str, EZPI_core_sntp_get_current_time_sec());

                if (1 == ezpi_execute_action_block(scene_node, scene_node->then_block))
                {
                    EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_finished_str, EZPI_core_sntp_get_current_time_sec());
                }
                else
                {
                    EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_failed_str, EZPI_core_sntp_get_current_time_sec());
                }
            }
            else if (scene_node->else_block)
            {
                if (1 == ezpi_execute_action_block(scene_node, scene_node->else_block))
                {
                    EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_finished_str, EZPI_core_sntp_get_current_time_sec());
                }
                else
                {
                    EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_failed_str, EZPI_core_sntp_get_current_time_sec());
                }
            }
            else
            {
                EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_failed_str, EZPI_core_sntp_get_current_time_sec());
            }

            vTaskDelay(10 / portTICK_RATE_MS);
            ezpi_execute_scene_start(scene_node);
        }
        else
        {
            EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_failed_str, EZPI_core_sntp_get_current_time_sec());
        }
    }

    return ret;
}

uint32_t EZPI_meshbot_execute_scene_else_action_group(uint32_t scene_id)
{
    int ret = 0;
    l_scenes_list_v2_t *scene_node = EZPI_core_scenes_get_by_id_v2(scene_id);
    if (scene_node)
    {
        if (scene_node->else_block)
        {
            EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_started_str, EZPI_core_sntp_get_current_time_sec());

            if (1 == ezpi_execute_action_block(scene_node, scene_node->else_block))
            {
                EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_finished_str, EZPI_core_sntp_get_current_time_sec());
            }
            else
            {
                EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_failed_str, EZPI_core_sntp_get_current_time_sec());
                ret = 1;
            }
        }
    }
    else
    {
        EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_failed_str, EZPI_core_sntp_get_current_time_sec());
        ret = 1;
    }

    return ret;
}

void EZPI_scenes_meshbot_init(void)
{
    l_scenes_list_v2_t *scene_node = EZPI_core_scenes_get_scene_head_v2();
    while (scene_node)
    {
        scene_node->status = EZLOPI_SCENE_STATUS_STOPPED;
        if (scene_node->enabled && scene_node->when_block && (scene_node->else_block || scene_node->then_block))
        {
            s_thread_ctx_t *thread_ctx = ezlopi_malloc(__FUNCTION__, sizeof(s_thread_ctx_t));
            if (thread_ctx)
            {
                memset(thread_ctx, 0, sizeof(s_thread_ctx_t));
                PT_INIT(&thread_ctx->pt);
                scene_node->thread_ctx = (void *)thread_ctx;
                scene_node->status = EZLOPI_SCENE_STATUS_RUN;
            }
        }
        else
        {
            scene_node->status = EZLOPI_SCENE_STATUS_STOPPED;
        }

        scene_node = scene_node->next;
    }

    EZPI_service_loop_add("meshbot-loop", __scenes_loop, 100, NULL);
}

PT_THREAD(ezpi_scene_proto_thread(l_scenes_list_v2_t *scene_node, uint32_t routine_delay_ms))
{
    s_thread_ctx_t *ctx = (s_thread_ctx_t *)scene_node->thread_ctx;
    PT_BEGIN(&ctx->pt);

    if ((EZLOPI_SCENE_STATUS_RUN == scene_node->status) || (EZLOPI_SCENE_STATUS_RUNNING == scene_node->status))
    {
        scene_node->status = EZLOPI_SCENE_STATUS_RUNNING;
        uint32_t when_condition_returned = 0;
        l_when_block_v2_t *when_condition_node = scene_node->when_block;

        if (when_condition_node)
        {
            f_scene_method_v2_t when_method = EZPI_core_scenes_get_method_v2(when_condition_node->block_options.method.type);
            if (when_method)
            {
                when_condition_returned = when_method(scene_node, (void *)when_condition_node);
                if (when_condition_returned)
                {
                    if (ctx->start_cond < 2)
                    {
                        ctx->stopped_cond = 0;

                        if (ctx->start_cond)
                        {
                            scene_node->executed_date = EZPI_core_sntp_get_current_time_sec(); // executed date/time when scene was activated
                            EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_started_str, EZPI_core_sntp_get_current_time_sec());
                        }

                        l_action_block_v2_t *then_block_node = scene_node->then_block;
                        while (then_block_node)
                        {
                            uint32_t delay_ms = (then_block_node->delay.days * (24 * 60 * 60) + then_block_node->delay.hours * (60 * 60) + then_block_node->delay.minutes * 60 + then_block_node->delay.seconds) * 1000;

                            if (delay_ms)
                            {
                                ctx->delay_ms = delay_ms;
                                ctx->action_node = then_block_node;

                                ctx->curr_ticks = xTaskGetTickCount();
                                PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > ctx->delay_ms);

                                then_block_node = ctx->action_node;
                            }

                            // TRACE_D("then-method: %p", then_method);
                            f_scene_method_v2_t then_method = EZPI_core_scenes_get_method_v2(then_block_node->block_options.method.type);
                            if (then_method)
                            {
                                then_method(scene_node, (void *)then_block_node); // then method executed here

                                if (then_block_node->next)
                                {
                                    EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_partially_finished_str, EZPI_core_sntp_get_current_time_sec());
                                }
                                else
                                {
                                    EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_finished_str, EZPI_core_sntp_get_current_time_sec());
                                }

                                ctx->start_cond += 1;
                            }
                            else
                            {
                                EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_failed_str, EZPI_core_sntp_get_current_time_sec());
                            }

                            ctx->delay_ms = 10;
                            ctx->action_node = then_block_node;

                            ctx->curr_ticks = xTaskGetTickCount();
                            PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > ctx->delay_ms);

                            then_block_node = ctx->action_node;
                            then_block_node = then_block_node->next;
                        }
                    }
                }
                else if (ctx->stopped_cond < 2)
                {
                    l_action_block_v2_t *else_block_node = scene_node->else_block;
                    while (else_block_node)
                    {
                        uint32_t delay_ms = (else_block_node->delay.days * (24 * 60 * 60) + else_block_node->delay.hours * (60 * 60) + else_block_node->delay.minutes * 60 + else_block_node->delay.seconds) * 1000;

                        if (delay_ms)
                        {
                            ctx->delay_ms = delay_ms;
                            ctx->action_node = else_block_node;

                            ctx->curr_ticks = xTaskGetTickCount();
                            PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > ctx->delay_ms);

                            else_block_node = ctx->action_node;
                        }

                        f_scene_method_v2_t else_method = EZPI_core_scenes_get_method_v2(else_block_node->block_options.method.type);
                        // TRACE_D("else-method: %p", else_method);
                        if (else_method)
                        {
                            else_method(scene_node, (void *)else_block_node);
                            TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_INFO, "meshbot[%.*s]: else condition.", sizeof(scene_node->name), scene_node->name);
                        }

                        ctx->delay_ms = 10;
                        ctx->action_node = else_block_node;

                        ctx->curr_ticks = xTaskGetTickCount();
                        PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > ctx->delay_ms);

                        else_block_node = ctx->action_node;
                        else_block_node = else_block_node->next;
                    }

                    if (1 == ctx->stopped_cond)
                    { // avoid broadcasting twice when the 'scene' is in 'stop' conditon
                        EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_stopped_str, EZPI_core_sntp_get_current_time_sec());
                    }

                    ctx->stopped_cond += 1;
                    ctx->start_cond = 0;
                }
            }

            when_condition_node = when_condition_node->next;
        }
    }

    if (EZLOPI_SCENE_STATUS_STOP == scene_node->status)
    {
        scene_node->status = EZLOPI_SCENE_STATUS_STOPPED;
        EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_stopped_str, EZPI_core_sntp_get_current_time_sec());
        break;
    }

    ctx->curr_ticks = xTaskGetTickCount();
    PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > routine_delay_ms);
    PT_END(&ctx->pt);
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/
static void __scenes_loop(void *arg)
{
    l_scenes_list_v2_t *scene_node = EZPI_core_scenes_get_scene_head_v2();

    while (scene_node)
    {
        if (scene_node->thread_ctx)
        {
            ezpi_scene_proto_thread(scene_node, 1000);

            if (EZLOPI_SCENE_STATUS_STOPPED == scene_node->status)
            {
                ezlopi_free(__FUNCTION__, scene_node->thread_ctx);
                scene_node->thread_ctx = NULL;
            }

            vTaskDelay(1 / portTICK_RATE_MS);
        }

        scene_node = scene_node->next;
    }
}

static int ezpi_execute_scene_stop(l_scenes_list_v2_t *scene_node)
{
    int ret = 0;
    if (scene_node)
    {
        if ((EZLOPI_SCENE_STATUS_RUN == scene_node->status) || (EZLOPI_SCENE_STATUS_RUNNING == scene_node->status))
        {
            scene_node->status = EZLOPI_SCENE_STATUS_STOP;
        }

        uint32_t loop_count = 100;

        while (EZLOPI_SCENE_STATUS_STOPPED != scene_node->status && loop_count--)
        {
            vTaskDelay(50 / portTICK_RATE_MS);
        }

        if (EZLOPI_SCENE_STATUS_STOPPED == scene_node->status)
        {
            ret = 1;
        }
    }

    return ret;
}

static int ezpi_execute_scene_start(l_scenes_list_v2_t *scene_node)
{
    int ret = 0;
    if (scene_node && (true == scene_node->enabled) && (NULL == scene_node->thread_ctx))
    {
        if ((EZLOPI_SCENE_STATUS_NONE == scene_node->status) ||
            (EZLOPI_SCENE_STATUS_STOPPED == scene_node->status))
        {
            scene_node->thread_ctx = (void *)ezlopi_malloc(__FUNCTION__, sizeof(s_thread_ctx_t));
            if (scene_node->thread_ctx)
            {
                memset(scene_node->thread_ctx, 0, sizeof(s_thread_ctx_t));
                PT_INIT(&((s_thread_ctx_t *)scene_node->thread_ctx)->pt);
                scene_node->status = EZLOPI_SCENE_STATUS_RUN;
                ret = 1;
            }
        }
    }

    return ret;
}

static int ezpi_execute_action_block(l_scenes_list_v2_t *scene_node, l_action_block_v2_t *action_block)
{
    int ret = 0;
    while (action_block)
    {
        uint32_t delay_ms = (action_block->delay.days * (24 * 60 * 60) + action_block->delay.hours * (60 * 60) + action_block->delay.minutes * 60 + action_block->delay.seconds) * 1000;

        if (delay_ms)
        {
            vTaskDelay(delay_ms / portTICK_RATE_MS);
        }

        f_scene_method_v2_t action_method = EZPI_core_scenes_get_method_v2(action_block->block_options.method.type);

        if (action_method)
        {
            action_method(scene_node, (void *)action_block);
            ret = 1;
        }

        if (NULL != action_block->next) // ((SCENE_BLOCK_TYPE_THEN == action_block->block_type))
        {
            EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_partially_finished_str, EZPI_core_sntp_get_current_time_sec());
        }

        action_block = action_block->next;
    }

    return ret;
}

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS
/*******************************************************************************
 *                          End of File
 *******************************************************************************/
