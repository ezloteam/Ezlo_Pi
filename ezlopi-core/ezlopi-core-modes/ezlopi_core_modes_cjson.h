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
 * @file    ezlopi_core_modes_cjson.h
 * @brief   These function perform operations related to house-mode cjsons
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef __EZLOPI_CORE_MODES_CJSON_H__
#define __EZLOPI_CORE_MODES_CJSON_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
// #include "cjext.h"
// #include <stdint.h>
#include <string.h>

#include "ezlopi_core_modes.h"
#include "ezlopi_core_errors.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
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
     * @brief This function returns an oject containing current House-Modes information
     *
     * @param cj_dest Destination object for House-mode infomation.
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_cjson_get_modes(cJSON *cj_dest);
    /**
     * @brief This function returns object contaings modeId of currently active House-Modes
     *
     * @param cj_dest Destination object where house-mode id is stored
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_cjson_get_current_mode(cJSON *cj_dest);
    /**
     * @brief This function returns a new house-mode node from the source-obj 'cj_modes'
     *
     * @param cj_modes Source object used to extract info from.
     * @return s_ezlopi_modes_t
     */
    s_ezlopi_modes_t *EZPI_core_modes_cjson_parse_modes(cJSON *cj_modes);
    /**
     * @brief This function returns cjson indicating MODES_status
     *
     * @return cJSON *
     */
    cJSON *EZPI_core_modes_cjson_changed(void);
    /**
     * @brief This function prepares and returns cjson message containing 'Alarmed_status' of an alert-device
     *
     * @param dev_id_str Id of target alert-device.
     * @return cJSON *
     */
    cJSON *EZPI_core_modes_cjson_prep_alarm_mesg(const char *dev_id_str);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MODES

#endif // __EZLOPI_CORE_MODES_CJSON_H__
/*******************************************************************************
 *                          End of File
 *******************************************************************************/
