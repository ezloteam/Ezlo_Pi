

#ifndef _EZLOPI_CORE_PROCESSES_H
#define _EZLOPI_CORE_PROCESSES_H

#include <cJSON.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define EZLOPI_MINIMAL_STACK_SIZE                                   ( 2048 )

#define EZLOPI_CLOUD_REGISTRATION_PROCESS_STACK_DEPTH               ( 2 * EZLOPI_MINIMAL_STACK_SIZE )

#define EZLOPI_COMPONENT_DNS_HIJACK_SRV_TASK_STACK_DEPTH            ( 2 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_COMPONENT_LUA_ALLOC_TEST_TASK_DEPTH                  ( 32 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_COMPONENT_LUA_CUSTOM_LIB_TEST_TASK_DEPTH             ( 32 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_COMPONENT_LUA_SIMPLE_TEST_TASK_DEPTH                 ( 32 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_COMPONENT_LUA_VFS_TEST_TASK_DEPTH                    ( 32 * EZLOPI_MINIMAL_STACK_SIZE )

#define EZLOPI_CORE_MDNS_SERVICE_TASK_DEPTH                         ( 4 * EZLOPI_MINIMAL_STACK_SIZE)
#define EZLOPI_CORE_OTA_PROCESS_TASK_DEPTH                          ( 4 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_CORE_SCENES_SCRIPT_PROCESS_TASK_DEPTH                ( 2 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_CORE_WIFI_SCANNER_TASK_DEPTH                         ( 2 * EZLOPI_MINIMAL_STACK_SIZE )

#define EZLOPI_MAIN_BLINKY_TASK_DEPTH                               ( 2 * EZLOPI_MINIMAL_STACK_SIZE )

#define EZLOPI_HAL_UART_TASK_DEPTH                                  ( 2 * EZLOPI_MINIMAL_STACK_SIZE )

#define EZLOPI_SENSOR_MPU6050_TASK_DEPTH                            ( 1 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SENSOR_GY271_CALLIBRATION_TASK_DEPTH                 ( 1 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SENSOR_HALL_CALLIBRATION_TASK_DEPTH                  ( 1 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SENSOR_TCS230_CALLIBRATION_TASK_DEPTH                ( 2 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SENSOR_HX711_TASK_DEPTH                              ( 2 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SENSOR_MQ2_TASK_DEPTH                                ( 1 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SENSOR_MQ3_TASK_DEPTH                                ( 1 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SENSOR_MQ4_TASK_DEPTH                                ( 1 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SENSOR_MQ6_TASK_DEPTH                                ( 1 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SENSOR_MQ7_TASK_DEPTH                                ( 1 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SENSOR_MQ8_TASK_DEPTH                                ( 1 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SENSOR_MQ9_TASK_DEPTH                                ( 1 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SENSOR_MQ135_TASK_DEPTH                              ( 1 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SENSOR_R307_FINGER_PRINT_TASK_DEPTH                  ( 2 * EZLOPI_MINIMAL_STACK_SIZE )

#define EZLOPI_SERVICE_MESHBOT_SCENE_PROCESS_1_TASK_DEPTH           ( 2 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SERVICE_MESHBOT_SCENE_PROCESS_2_TASK_DEPTH           ( 3 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SERVICE_BROADCAST_TASK_DEPTH                         ( 2 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SERVICE_GPIO_ISR_TASK_DEPTH                          ( 2 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SERVICE_MODES_TASK_DEPTH                             ( 2 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SERVICE_OTA_PROCESS_TASK_DEPTH                       ( 2 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SERVICE_TIMER_TASK_DEPTH                             ( 2 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SERVICE_UART_TASK_DEPTH                              ( 1 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK_DEPTH             ( 4 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK_DEPTH                ( 3 * EZLOPI_MINIMAL_STACK_SIZE )
#define EZLOPI_SERVICE_LED_INDICATOR_TASK_DEPTH                     ( 2 * EZLOPI_MINIMAL_STACK_SIZE )


typedef enum {
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

    ENUM_EZLOPI_MAIN_BLINKY_TASK, 

    ENUM_EZLOPI_HAL_UART_TASK,

    ENUM_EZLOPI_SENSOR_MPU6050_TASK, 
    ENUM_EZLOPI_SENSOR_GY271_CALLIBRATION_TASK, 
    ENUM_EZLOPI_SENSOR_HALL_CALLIBRATION_TASK, 
    ENUM_EZLOPI_SENSOR_TCS230_CALLIBRATION_TASK, 
    ENUM_EZLOPI_SENSOR_HX711_TASK, 
    ENUM_EZLOPI_SENSOR_MQ2_TASK, 
    ENUM_EZLOPI_SENSOR_MQ3_TASK, 
    ENUM_EZLOPI_SENSOR_MQ4_TASK, 
    ENUM_EZLOPI_SENSOR_MQ6_TASK, 
    ENUM_EZLOPI_SENSOR_MQ7_TASK, 
    ENUM_EZLOPI_SENSOR_MQ8_TASK, 
    ENUM_EZLOPI_SENSOR_MQ9_TASK, 
    ENUM_EZLOPI_SENSOR_MQ135_TASK, 
    ENUM_EZLOPI_SENSOR_R307_FINGER_PRINT_TASK, 

    ENUM_EZLOPI_SERVICE_MESHBOT_SCENE_PROCESS_1_TASK, 
    ENUM_EZLOPI_SERVICE_MESHBOT_SCENE_PROCESS_2_TASK, 
    ENUM_EZLOPI_SERVICE_BROADCAST_TASK, 
    ENUM_EZLOPI_SERVICE_GPIO_ISR_TASK, 
    ENUM_EZLOPI_SERVICE_MODES_TASK, 
    ENUM_EZLOPI_SERVICE_OTA_PROCESS_TASK, 
    ENUM_EZLOPI_SERVICE_TIMER_TASK, 
    ENUM_EZLOPI_SERVICE_UART_TASK, 
    ENUM_EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK, 
    ENUM_EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK, 
    ENUM_EZLOPI_SERVICE_LED_INDICATOR_TASK,

    ENUM_TASK_MAX
}e_ezlopi_task_enum_t;



int ezlopi_core_get_processes_details(cJSON* cj_processes_array);
int ezlopi_core_process_set_process_info(e_ezlopi_task_enum_t task_num, TaskHandle_t *task_handle, size_t task_depth);
int ezlopi_core_process_set_is_deleted(e_ezlopi_task_enum_t task_num);

#endif // _EZLOPI_CORE_PROCESSES_H


