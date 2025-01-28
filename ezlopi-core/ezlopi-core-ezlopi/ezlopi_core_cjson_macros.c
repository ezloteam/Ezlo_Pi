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
 * @file    ezlopi_core_cjson_macros.c
 * @brief   perform some function on data
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include "ezlopi_core_cjson_macros.h"

#ifndef EZPI_USE_CJSON_MACRO

#include "cjext.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_cloud_constants.h"

/**
 *
 */
void EZPI_core_cjson_get_value_uint8(cJSON *cj_root, const char *item_name_str, uint8_t *item_val)
{
    *item_val = 0;
    cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, item_name_str);
    if (o_item && (o_item->type & cJSON_Number))
    {
        *item_val = o_item->valuedouble;
    }
}

/**
 *
 */
void EZPI_core_cjson_get_value_float(cJSON *cj_root, const char *item_name_str, float *item_val)
{
    *item_val = 0;
    cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, item_name_str);
    if (o_item)
    {
        *item_val = (float)o_item->valuedouble;
    }
}

void EZPI_core_cjson_get_value_double(cJSON *cj_root, const char *item_name_str, double *item_val)
{
    *item_val = 0;
    cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, item_name_str);
    if (o_item && (o_item->type & cJSON_Number))
    {
        *item_val = o_item->valuedouble;
    }
}

void EZPI_core_cjson_get_value_int(cJSON *cj_root, const char *item_name_str, int *item_val)
{
    *item_val = 0;
    cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, item_name_str);
    if (o_item && (o_item->type & cJSON_Number))
    {
        *item_val = (int)o_item->valuedouble;
    }
}

void EZPI_core_cjson_get_value_uint16(cJSON *cj_root, const char *item_name_str, uint16_t *item_val)
{
    *item_val = 0;
    cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, item_name_str);
    if (o_item && (o_item->type & cJSON_Number))
    {
        *item_val = (uint16_t)o_item->valuedouble;
    }
}

void EZPI_core_cjson_get_value_uint32(cJSON *cj_root, const char *item_name_str, uint32_t *item_val)
{
    *item_val = 0;
    cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, item_name_str);
    if (o_item && (o_item->type & cJSON_Number))
    {
        *item_val = (uint32_t)o_item->valuedouble;
    }
}

uint32_t EZPI_core_cjson_get_id(cJSON *cj_root, const char *item_name_str)
{
    uint32_t item_val = 0;
    cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, item_name_str);
    if (o_item && (o_item->type & cJSON_String))
    {
        item_val = (uint32_t)strtoul(o_item->valuestring, NULL, 16);
    }

    return item_val;
}

void EZPI_core_cjson_get_value_bool(cJSON *cj_root, const char *item_name_str, bool *item_val)
{
    *item_val = false;
    cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, item_name_str);
    if (o_item)
    {
        if (o_item->type & cJSON_True)
        {
            *item_val = true;
        }
    }
}

void EZPI_core_cjson_trace(const char *name_str, cJSON *cj_object)
{
    if (cj_object)
    {
        char *obj_str = cJSON_Print(__FUNCTION__, cj_object);
        if (obj_str)
        {
            TRACE_D("%s[%d]: %s", name_str ? name_str : ezlopi__str, strlen(obj_str), obj_str);
            ezlopi_free(__FUNCTION__, obj_str);
        }
    }
    else
    {
        TRACE_E("%s: Null", name_str ? name_str : ezlopi__str);
    }
}

void EZPI_core_cjson_get_value_string_by_copy(cJSON *cj_root, const char *item_name_str, char *item_val, uint32_t item_val_len)
{
    if (cj_root && item_name_str && item_val && item_val_len)
    {
        cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, item_name_str);
        if (o_item && o_item->valuestring && o_item->str_value_len)
        {
            snprintf(item_val, item_val_len, "%.*s", o_item->str_value_len, o_item->valuestring);
        }
    }
}

void EZPI_core_cjson_get_value_string_by_alloc(cJSON *cj_root, const char *item_name_str, char **item_val_ptr_address)
{
    cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, item_name_str);

    if (o_item && o_item->valuestring && o_item->str_value_len)
    {
        uint32_t value_len = o_item->str_value_len + 1;
        ezlopi_free(__func__, *item_val_ptr_address);
        *item_val_ptr_address = ezlopi_malloc(__func__, value_len);
        if (NULL != *item_val_ptr_address)
        {
            snprintf(*item_val_ptr_address, value_len, "%.*s", o_item->str_value_len, o_item->valuestring);
        }
    }
}

void EZPI_core_cjson_assign_id_as_string(cJSON *cj_object, uint32_t id, const char *id_str)
{
    if (id && cj_object && id_str)
    {
        char tmp_str[32];
        snprintf(tmp_str, sizeof(tmp_str), "%08x", id);
        cJSON_AddStringToObject(__FUNCTION__, cj_object, id_str, tmp_str);
    }
}

void EZPI_core_cjson_assign_number_as_string(cJSON *cj_obj, uint32_t num, const char *name_str)
{
    if (cj_obj && name_str)
    {
        char tmp_str[32];
        snprintf(tmp_str, sizeof(tmp_str), "%u", (uint32_t)num);
        cJSON_AddStringToObject(__FUNCTION__, cj_obj, name_str, tmp_str);
    }
}

int EZPI_core_cjson_get_value_gpio(cJSON *cj_root, const char *item_name_str)
{
    int item_val = -1;
    cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, item_name_str);
    if (o_item && (o_item->type & cJSON_Number))
    {
        item_val = o_item->valuedouble;
    }
    return item_val;
}
#endif // EZPI_USE_CJSON_MACRO