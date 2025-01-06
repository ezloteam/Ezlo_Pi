/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef __EZLOPI_USER_CONFIG_H__
#define __EZLOPI_USER_CONFIG_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "driver/uart.h"
#include "ezlopi_core_heap.h"
#include "../build/config/sdkconfig.h"

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
#if defined(CONFIG_EZPI_DISTRO_FULL_OPTION)
#define CONFIG_EZPI_DISTRO_NAME "EZPI_DISTRO_FULL_OPTION"
#elif defined(CONFIG_EZPI_DISTRO_LOCAL)
#define CONFIG_EZPI_DISTRO_NAME "EZPI_DISTRO_LOCAL"
#elif defined(CONFIG_EZPI_DISTRO_LOCAL_MESHBOT)
#define CONFIG_EZPI_DISTRO_NAME "EZPI_DISTRO_LOCAL_MESHBOT"
#elif defined(CONFIG_EZPI_DISTRO_CLOUD)
#define CONFIG_EZPI_DISTRO_NAME "EZPI_DISTRO_CLOUD"
#elif defined(CONFIG_EZPI_DISTRO_CLOUD_MESHBOT)
#define CONFIG_EZPI_DISTRO_NAME "EZPI_DISTRO_CLOUD_MESHBOT"
#elif defined(CONFIG_EZPI_DISTRO_BLE_CLOUD)
#define CONFIG_EZPI_DISTRO_NAME "EZPI_DISTRO_BLE_CLOUD"
#elif defined(CONFIG_EZPI_DISTRO_WIFI_HUB)
#define CONFIG_EZPI_DISTRO_NAME "EZPI_DISTRO_WIFI_HUB"
#elif defined(CONFIG_EZPI_DISTRO_BLE_HUB)
#define CONFIG_EZPI_DISTRO_NAME "EZPI_DISTRO_BLE_HUB"
#elif defined(CONFIG_EZPI_DISTRO_MINIMAL)
#define CONFIG_EZPI_DISTRO_NAME "EZPI_DISTRO_MINIMAL"
#else
#define CONFIG_EZPI_DISTRO_NAME "EZPI_DISTRO_CUSTOM"
#endif

#if defined(CONFIG_EZPI_HEAP_ENABLE)
#ifdef malloc
#undef malloc
#endif
#define ezlopi_malloc(who, x) EZPI_core_util_heap_malloc(who, x, __FILENAME__, __LINE__)

#ifdef calloc
#undef calloc
#endif
#define ezlopi_calloc(who, x, y) EZPI_core_util_heap_calloc(who, x, y, __FILENAME__, __LINE__)

#ifdef free
#undef free
#endif
#define ezlopi_free(who, x) EZPI_core_util_heap_free(who, x, __FILENAME__, __LINE__)

#ifdef realloc
#undef realloc
#endif
#define ezlopi_realloc(who, x, y) EZPI_core_util_heap_realloc(who, x, y, __FILENAME__, __LINE__)

#else // CONFIG_EZPI_HEAP_ENABLE

#define ezlopi_malloc(who, x) malloc(x)
#define ezlopi_calloc(who, x, y) calloc(x, y)
#define ezlopi_free(who, x) \
    if (NULL != x)          \
        free(x);
#define ezlopi_realloc(who, x, y) realloc(x, y)

#endif

// #else

/////////////////////////////////////////////////////////////
/////////////// Do not change here //////////////////
////////////////////////////////////////////////////////////

// EzloPi Versions
#define EZPI_VERSION_API_SERIAL "1.0.0"
#define EZPI_VERSION_API_BLE "1.0.0"

// User Configs
#define EZPI_USR_CONFIG_ENABLE_HEARTBIT_LED
#define EZPI_USR_CONFIG_ENABLE_LOG
#define EZPI_DEV_TYPE_GENERIC 1

// Util
#define EZPI_UTIL_ENABLE_TRACE 1

// Core
#define EZPI_CORE_ENABLE_ETH

// Cloud

// Service
#define EZPI_SERV_ENABLE_MESHBOTS

// EzloPi BLE Service Pass key
#define EZPI_SERV_BLE_PASSKEY_EN 0
#define EZPI_SERV_BLE_PAIRING_EN 1
#define EZPI_SERV_BLE_ENABLE_READ_PROV
#define EZPI_SERV_BLE_ENABLE_STAT_PROV

// EzloPi WiFi Configuration
#define EZPI_CORE_WIFI_CONNECT_TIMEOUT 5000
#define EZPI_CORE_WIFI_PASS_CHAR_MIN_LEN 8
#define EZPI_CORE_WIFI_CONN_ATTEMPT_INTERVAL 5000
#define EZPI_CORE_WIFI_CONN_RETRY_ATTEMPT 2

    // EzloPi Default Serial Configuartion

#define EZPI_SERV_UART_NUM_DEFAULT UART_NUM_0

#define EZPI_SERV_UART_NUM EZPI_SERV_UART_NUM_DEFAULT

#define EZPI_SERV_UART_BAUD_DEFAULT (uint32_t)115200
#define EZPI_SERV_UART_PARITY_DEFAULT (uint32_t) UART_PARITY_DISABLE
#define EZPI_SERV_UART_START_BIT_DEFAULT (uint32_t)0
#define EZPI_SERV_UART_STOP_BIT_DEFAULT (uint32_t)1
#define EZPI_SERV_UART_FRAME_SIZE_DEFAULT (uint32_t)8
#define EZPI_SERV_UART_FLOW_CTRL_DEFAULT (uint32_t) UART_HW_FLOWCTRL_DISABLE

#define EZPI_SERV_UART_RX_BUFFER_SIZE (uint32_t)3072

#define EZPI_UART_SERV_FLW_CTRL_STR_SIZE 10

#if defined(CONFIG_IDF_TARGET_ESP32)
#define EZPI_SERV_UART_TXD_PIN (GPIO_NUM_1)
#define EZPI_SERV_UART_RXD_PIN (GPIO_NUM_3)
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
#define EZPI_SERV_UART_TXD_PIN (GPIO_NUM_21)
#define EZPI_SERV_UART_RXD_PIN (GPIO_NUM_20)
#elif defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32S2)
#define EZPI_SERV_UART_TXD_PIN (GPIO_NUM_43)
#define EZPI_SERV_UART_RXD_PIN (GPIO_NUM_44)
#endif

// Log Configuration
#define EZPI_CORE_LOG_BUFFER_SIZE 1024

// Web Socket Configurations
#define EZPI_CORE_WSS_TASK_STACK_SIZE 6144  // 6 * 1024
#define EZPI_CORE_WSS_DATA_BUFFER_SIZE 6144 // 6 * 1024
#define EZPI_CORE_WSS_PING_PONG_TIMEOUT_SEC 10
#define EZPI_CORE_WSS_PING_INTERVAL_SEC 5

    /////////////////////////////////////////////////////////////
    /////////////// Disable user configs here //////////////////
    ////////////////////////////////////////////////////////////

#undef EZPI_USR_CONFIG_ENABLE_HEARTBIT_LED
#undef EZPI_CORE_ENABLE_ETH
#undef EZPI_DEV_TYPE_GENERIC
// #undef EZPI_SERV_ENABLE_MESHBOTS
#undef EZPI_UTIL_ENABLE_TRACE
    // #endif

#undef EZPI_SERV_BLE_ENABLE_READ_PROV
#undef EZPI_SERV_BLE_ENABLE_STAT_PROV

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_USER_CONFIG_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
