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

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cjext.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_core_valueformatter.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/**
 * @brief Global/extern function template example
 * Convention : Use capital letter for initial word on extern function
 * @param arg
 */
void ezlopi_valueformatter_bool_to_cjson(cJSON* cj_root, bool value, const char * scale)
{
    cJSON_AddBoolToObject(__FUNCTION__, cj_root, ezlopi_value_str, value);
    cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_valueFormatted_str, value ? ezlopi_true_str : ezlopi_false_str);
}

void ezlopi_valueformatter_float_to_cjson(cJSON* cj_root, float value, const char * scale)
{
    cJSON_AddNumberToObject(__FUNCTION__, cj_root, ezlopi_value_str, value);

    char valueFormatted[16];
    snprintf(valueFormatted, sizeof(valueFormatted), "%.02f", value);
    cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_valueFormatted_str, valueFormatted);
}

void ezlopi_valueformatter_double_to_cjson(cJSON* cj_root, double value, const char * scale)
{
    cJSON_AddNumberToObject(__FUNCTION__, cj_root, ezlopi_value_str, value);

    char valueFormatted[16];
    snprintf(valueFormatted, sizeof(valueFormatted), "%.2lf", value);
    cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_valueFormatted_str, valueFormatted);
}

void ezlopi_valueformatter_int32_to_cjson(cJSON* cj_root, int value, const char * scale)
{
    cJSON_AddNumberToObject(__FUNCTION__, cj_root, ezlopi_value_str, value);

    char valueFormatted[16];
    snprintf(valueFormatted, sizeof(valueFormatted), "%d", value);
    cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_valueFormatted_str, valueFormatted);
}

void ezlopi_valueformatter_uint32_to_cjson(cJSON* cj_root, uint32_t value, const char * scale)
{
    cJSON_AddNumberToObject(__FUNCTION__, cj_root, ezlopi_value_str, value);

    char valueFormatted[16];
    snprintf(valueFormatted, sizeof(valueFormatted), "%u", value);
    cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_valueFormatted_str, valueFormatted);
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
