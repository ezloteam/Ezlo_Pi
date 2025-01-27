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
 * @file    ezlopi_core_processes.h
 * @brief   Function to perform operation on ezlopi-process/task
 * @author
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_PROCESSES_H_
#define _EZLOPI_CORE_PROCESSES_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "cjext.h"

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/

#define EZLOPI_MINIMAL_STACK_SIZE (2048)

#define EZLOPI_CLOUD_REGISTRATION_PROCESS_STACK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)

#define EZLOPI_COMPONENT_DNS_HIJACK_SRV_TASK_STACK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_COMPONENT_LUA_ALLOC_TEST_TASK_DEPTH (32 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_COMPONENT_LUA_CUSTOM_LIB_TEST_TASK_DEPTH (32 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_COMPONENT_LUA_SIMPLE_TEST_TASK_DEPTH (32 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_COMPONENT_LUA_VFS_TEST_TASK_DEPTH (32 * EZLOPI_MINIMAL_STACK_SIZE)

#define EZLOPI_CORE_MDNS_SERVICE_TASK_DEPTH (4 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_CORE_OTA_PROCESS_TASK_DEPTH (4 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_CORE_SCENES_SCRIPT_PROCESS_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_CORE_WIFI_SCANNER_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_CORE_WIFI_TRY_CONNECT_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)

#define EZLOPI_MAIN_BLINKY_TASK_DEPTH (3 * EZLOPI_MINIMAL_STACK_SIZE)

#define EZLOPI_HAL_UART_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)

    // #define EZLOPI_SENSOR_MPU6050_TASK_DEPTH (1 * EZLOPI_MINIMAL_STACK_SIZE)
    // #define EZLOPI_SENSOR_GY271_CALLIBRATION_TASK_DEPTH (1 * EZLOPI_MINIMAL_STACK_SIZE)
    // #define EZLOPI_SENSOR_HALL_CALLIBRATION_TASK_DEPTH (1 * EZLOPI_MINIMAL_STACK_SIZE)
    // #define EZLOPI_SENSOR_TCS230_CALLIBRATION_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)
    // #define EZLOPI_SENSOR_HX711_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)
    // #define EZLOPI_SENSOR_MQ2_TASK_DEPTH (1 * EZLOPI_MINIMAL_STACK_SIZE)
    // #define EZLOPI_SENSOR_MQ3_TASK_DEPTH (1 * EZLOPI_MINIMAL_STACK_SIZE)
    // #define EZLOPI_SENSOR_MQ4_TASK_DEPTH (1 * EZLOPI_MINIMAL_STACK_SIZE)
    // #define EZLOPI_SENSOR_MQ6_TASK_DEPTH (1 * EZLOPI_MINIMAL_STACK_SIZE)
    // #define EZLOPI_SENSOR_MQ7_TASK_DEPTH (1 * EZLOPI_MINIMAL_STACK_SIZE)
    // #define EZLOPI_SENSOR_MQ8_TASK_DEPTH (1 * EZLOPI_MINIMAL_STACK_SIZE)
    // #define EZLOPI_SENSOR_MQ9_TASK_DEPTH (1 * EZLOPI_MINIMAL_STACK_SIZE)
    // #define EZLOPI_SENSOR_MQ135_TASK_DEPTH (1 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_SENSOR_R307_FINGER_PRINT_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)

#define EZLOPI_SERVICE_MESHBOT_SCENE_PROCESS_1_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_SERVICE_MESHBOT_SCENE_PROCESS_2_TASK_DEPTH (3 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_SERVICE_BROADCAST_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_SERVICE_GPIO_ISR_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_SERVICE_MODES_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_SERVICE_OTA_PROCESS_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_SERVICE_TIMER_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_SERVICE_LOOP_TASK_DEPTH (4 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_SERVICE_UART_TASK_DEPTH (3 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK_DEPTH (4 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK_DEPTH (3 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_SERVICE_LED_INDICATOR_TASK_DEPTH (2 * EZLOPI_MINIMAL_STACK_SIZE)

    typedef enum
    {
        ENUM_EZLOPI_CLOUD_REGISTRATION_PROCESS_STACK = 0,

        ENUM_EZLOPI_COMPONENT_DNS_HIJACK_SRV_TASK_STACK,
        ENUM_EZLOPI_COMPONENT_LUA_ALLOC_TEST_TASK,
        ENUM_EZLOPI_COMPONENT_LUA_CUSTOM_LIB_TEST_TASK,
        ENUM_EZLOPI_COMPONENT_LUA_SIMPLE_TEST_TASK,
        ENUM_EZLOPI_COMPONENT_LUA_VFS_TEST_TASK,

        ENUM_EZLOPI_CORE_MDNS_SERVICE_TASK,
        ENUM_EZLOPI_CORE_OTA_PROCESS_TASK,
        ENUM_EZLOPI_CORE_SCENES_SCRIPT_PROCESS_TASK,
        ENUM_EZLOPI_CORE_WIFI_SCANNER_TASK,
        ENUM_EZLOPI_CORE_WIFI_TRY_CONNECT_TASK,

        ENUM_EZLOPI_MAIN_BLINKY_TASK,

        ENUM_EZLOPI_HAL_UART_TASK,

        // ENUM_EZLOPI_SENSOR_MPU6050_TASK,
        // ENUM_EZLOPI_SENSOR_GY271_CALLIBRATION_TASK,
        // ENUM_EZLOPI_SENSOR_HALL_CALLIBRATION_TASK,
        // ENUM_EZLOPI_SENSOR_TCS230_CALLIBRATION_TASK,
        // ENUM_EZLOPI_SENSOR_HX711_TASK,
        // ENUM_EZLOPI_SENSOR_MQ2_TASK,
        // ENUM_EZLOPI_SENSOR_MQ3_TASK,
        // ENUM_EZLOPI_SENSOR_MQ4_TASK,
        // ENUM_EZLOPI_SENSOR_MQ6_TASK,
        // ENUM_EZLOPI_SENSOR_MQ7_TASK,
        // ENUM_EZLOPI_SENSOR_MQ8_TASK,
        // ENUM_EZLOPI_SENSOR_MQ9_TASK,
        // ENUM_EZLOPI_SENSOR_MQ135_TASK,
        ENUM_EZLOPI_SENSOR_R307_FINGER_PRINT_TASK,

        ENUM_EZLOPI_SERVICE_MESHBOT_SCENE_PROCESS_1_TASK,
        ENUM_EZLOPI_SERVICE_MESHBOT_SCENE_PROCESS_2_TASK,
        ENUM_EZLOPI_SERVICE_BROADCAST_TASK,
        ENUM_EZLOPI_SERVICE_GPIO_ISR_TASK,
        ENUM_EZLOPI_SERVICE_MODES_TASK,
        ENUM_EZLOPI_SERVICE_OTA_PROCESS_TASK,
        ENUM_EZLOPI_SERVICE_TIMER_TASK,
        ENUM_EZLOPI_SERVICE_LOOP_TASK,
        ENUM_EZLOPI_SERVICE_UART_TASK,
        ENUM_EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK,
        ENUM_EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK,
        ENUM_EZLOPI_SERVICE_LED_INDICATOR_TASK,

        ENUM_TASK_MAX
    } e_ezlopi_task_enum_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    /**
     * @brief Function to get process_details
     *
     * @param cj_processes_array Target process-list to get details from
     * @return int
     */
    int ezlopi_core_get_processes_details(cJSON *cj_processes_array);
    /**
     * @brief Funtion to set process info mannually
     *
     * @param task_num Enum of task in the system
     * @param task_handle Task-handle of corresponding task
     * @param task_depth Task-depth
     * @return int
     */
    int EZPI_core_process_set_process_info(e_ezlopi_task_enum_t task_num, TaskHandle_t *task_handle, size_t task_depth);
    /**
     * @brief Function to delete processes
     *
     * @param task_num ID of target-task
     * @return int
     */
    int EZPI_core_process_set_is_deleted(e_ezlopi_task_enum_t task_num);
#else // CONFIG_FREERTOS_USE_TRACE_FACILITY

//---------- These are Not-required ---------------
// #define ezlopi_core_get_processes_details(x)
// #define EZPI_core_process_set_is_deleted(x)
// #define EZPI_core_process_set_process_info(x, y, z)
//-------------------------------------------------

#endif // CONFIG_FREERTOS_USE_TRACE_FACILITY

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_PROCESSES_H_
/*******************************************************************************
 *                          End of File
 *******************************************************************************/
