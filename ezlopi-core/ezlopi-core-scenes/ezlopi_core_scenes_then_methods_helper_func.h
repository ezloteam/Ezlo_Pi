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
 * @file    ezlopi_core_scenes_then_methods_helper_func.h
 * @brief   These are sub-functions utilized in file 'ezlopi_core_scenes_then_methods.c'
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_SCENES_THEN_METHODS_HELPER_FUNCTIONS_H_
#define _EZLOPI_CORE_SCENES_THEN_METHODS_HELPER_FUNCTIONS_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ezlopi_core_http.h"

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
    typedef struct s_sendhttp_method
    {
        const char *field_name;
        void (*field_func)(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field);
    } s_sendhttp_method_t;
    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief This function parses and populates 'HTTP_request_type' into a 's_ezlopi_core_http_mbedtls_t' struct
     *
     * @param tmp_http_data Destination Struct to store parsed data
     * @param curr_field Source of data
     */
    void EZPI_parse_http_request_type(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field);
    /**
     * @brief This function parses and populates 'HTTP_url' into a 's_ezlopi_core_http_mbedtls_t' struct
     *
     * @param tmp_http_data
     * @param curr_field
     */
    void EZPI_parse_http_url(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field);
    /**
     * @brief This function parses and populates 'HTTP_content' into a 's_ezlopi_core_http_mbedtls_t' struct
     *
     * @param tmp_http_data
     * @param curr_field
     */
    void EZPI_parse_http_content(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field);
    /**
     * @brief This function parses and populates 'HTTP_content_type' into a 's_ezlopi_core_http_mbedtls_t' struct
     *
     * @param tmp_http_data
     * @param curr_field
     */
    void EZPI_parse_http_content_type(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field);
    /**
     * @brief This function parses and populates 'HTTP_headers' into a 's_ezlopi_core_http_mbedtls_t' struct
     *
     * @param tmp_http_data
     * @param curr_field
     */
    void EZPI_parse_http_headers(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field);
    /**
     * @brief This function parses and populates 'HTTP_skipsecurity' into a 's_ezlopi_core_http_mbedtls_t' struct
     *
     * @param tmp_http_data
     * @param curr_field
     */
    void EZPI_parse_http_skipsecurity(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field);
    /**
     * @brief This function parses and populates 'HTTP_creds' into a 's_ezlopi_core_http_mbedtls_t' struct
     *
     * @param tmp_http_data
     * @param curr_field
     */
    void EZPI_parse_http_creds(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field);
    /**
     * @brief This function free the custom_struct 's_ezlopi_core_http_mbedtls_t'
     *
     * @param config
     */
    void EZPI_free_http_mbedtls_struct(s_ezlopi_core_http_mbedtls_t *config);
    /**
     * @brief Function to update information in scene 'Expression/Variable'.
     *
     * @param expression_name Target expression name
     * @param code_str  Target
     * @param value_type
     * @param cj_metadata
     * @param cj_params
     * @param var_value
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_scenes_then_helper_set_expn_var(char *expression_name, const char *code_str, const char *value_type, cJSON *cj_metadata, cJSON *cj_params, l_fields_v2_t *var_value);
    /**
     * @brief Function to update the toggle value in
     *
     * @param item_id
     * @param item_id_str
     * @return int
     */
    int EZPI_core_scenes_then_helper_toggleValue(uint32_t item_id, const char *item_id_str);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS
#endif /*_EZLOPI_CORE_SCENES_THEN_METHODS_HELPER_FUNCTIONS_H_*/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
