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
 * @file    ezlopi_core_log.h
 * @brief   Function operaters on system-logs
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_LOG_H_
#define _EZLOPI_CORE_LOG_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
#include "ezlopi_util_trace.h"
#include "ezlopi_core_errors.h"

#include <stdbool.h>

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

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Function to set the log severtity
     *
     */
    void EZPI_core_read_set_log_severities(void);
    /**
     * @brief Function to set the log severity
     *
     * @param severity Target servertiy
     */
    void EZPI_core_read_set_log_severities_internal(e_trace_severity_t severity);
    /**
     * @brief Function to  send logs everity
     *
     * @param severity_enable Flag to enable severity
     * @param severity_str Target severity
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_cloud_log_severity_process_str(bool severity_enable, const char *severity_str);
    /**
     * @brief  Function to send log severity by id
     *
     * @param severity_level_id Target severity lvl
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_cloud_log_severity_process_id(const e_trace_severity_t severity_level_id);
    /**
     * @brief Function to get severity values
     *
     * @return const char**
     */
    const char **EZPI_core_cloud_log_get_severity_enums();
    /**
     * @brief Funciton to get curr severity enum str
     *
     * @return const char*
     */
    const char *EZPI_core_cloud_log_get_current_severity_enum_str();
    /**
     * @brief Function to get curr severity enum val
     *
     * @return e_trace_severity_t
     */
    e_trace_severity_t EZPI_core_cloud_log_get_current_severity_enum_val();
    /**
     * @brief Function to send-cloud log severity info
     *
     * @param severity current severity
     * @param log_str Log Message
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_send_cloud_log(int severity, const char *log_str);
    /**
     * @brief Funciton to decide log methods (cloud or serial)
     *
     */
    void EZPI_core_set_log_upcalls();
    /**
     * @brief Funciton to process cloud log severity
     *
     * @param severity_str src severity
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_serial_log_severity_process_str(const char *severity_str);
    /**
     * @brief Funciton to process serial cloud log severity
     *
     * @param severity_level_id src severity lvl
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_serial_log_severity_process_id(const e_trace_severity_t severity_level_id);
    /**
     * @brief Function to get current cloud log-severity str
     *
     * @return const char*
     */
    const char *EZPI_core_serial_log_get_current_severity_enum_str();
    /**
     * @brief Function to get current serial log-severity str
     *
     * @return e_trace_severity_t
     */
    e_trace_severity_t EZPI_core_serial_log_get_current_severity_enum_val();

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_UTIL_TRACE_EN

#endif // _EZLOPI_CORE_LOG_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
