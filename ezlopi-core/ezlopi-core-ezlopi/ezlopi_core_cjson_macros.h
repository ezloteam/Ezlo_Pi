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
 * @file    ezlopi_core_cjson_macros.h
 * @brief   Macros for cjson operations
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */
#ifndef _EZLOPI_CORE_CJSON_MACROS_H_
#define _EZLOPI_CORE_CJSON_MACROS_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "cjext.h"
#include "ezlopi_util_trace.h"

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

    // 0x17d6e0
    // 0x17c2b0
    // 0x17bc70
    // 0x17bb30
    // 0x17bb20

    // #define EZPI_USE_CJSON_MACRO

#ifndef EZPI_USE_CJSON_MACRO
    void EZPI_core_cjson_trace(char *name_str, cJSON *cj_object);
    void EZPI_core_cjson_get_value_int(cJSON *cj_root, char *item_name_str, int *item_val);
    void EZPI_core_cjson_get_value_bool(cJSON *cj_root, char *item_name_str, bool *item_val);
    void EZPI_core_cjson_get_value_float(cJSON *cj_root, char *item_name_str, float *item_val);
    void EZPI_core_cjson_get_value_uint8(cJSON *cj_root, char *item_name_str, uint8_t *item_val);
    void EZPI_core_cjson_get_value_double(cJSON *cj_root, char *item_name_str, double *item_val);
    void EZPI_core_cjson_get_value_uint16(cJSON *cj_root, char *item_name_str, uint16_t *item_val);
    void EZPI_core_cjson_get_value_uint32(cJSON *cj_root, char *item_name_str, uint32_t *item_val);
    void EZPI_core_cjson_get_value_string_by_copy(cJSON *cj_root, char *item_name_str, char *item_val, uint32_t item_val_len);

#define CJSON_TRACE(name_str, cj_object) EZPI_core_cjson_trace(name_str, cj_object)

#define CJSON_GET_VALUE_INT(cj_root, item_name_str, item_val) EZPI_core_cjson_get_value_int(cj_root, item_name_str, &item_val)
#define CJSON_GET_VALUE_BOOL(cj_root, item_name_str, item_val) EZPI_core_cjson_get_value_bool(cj_root, item_name_str, &item_val)
#define CJSON_GET_VALUE_FLOAT(cj_root, item_name_str, item_val) EZPI_core_cjson_get_value_float(cj_root, item_name_str, &item_val)
#define CJSON_GET_VALUE_UINT8(cj_root, item_name_str, item_val) EZPI_core_cjson_get_value_uint8(cj_root, item_name_str, &item_val)
#define CJSON_GET_VALUE_DOUBLE(cj_root, item_name_str, item_val) EZPI_core_cjson_get_value_double(cj_root, item_name_str, &item_val)
#define CJSON_GET_VALUE_UINT16(cj_root, item_name_str, item_val) EZPI_core_cjson_get_value_uint16(cj_root, item_name_str, &item_val)
#define CJSON_GET_VALUE_UINT32(cj_root, item_name_str, item_val) EZPI_core_cjson_get_value_uint32(cj_root, item_name_str, &item_val)
#define CJSON_GET_VALUE_STRING_BY_COPY(cj_root, item_name_str, item_val, item_val_size) EZPI_core_cjson_get_value_string_by_copy(cj_root, item_name_str, item_val, item_val_size)

#else // EZPI_USE_CJSON_MACRO
#define CJSON_GET_VALUE_DOUBLE(root, item_name, item_val)                   \
    {                                                                       \
        cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, root, item_name); \
        if (o_item)                                                         \
        {                                                                   \
            item_val = o_item->valuedouble;                                 \
        }                                                                   \
        else                                                                \
        {                                                                   \
            item_val = 0;                                                   \
            TRACE_E("%s not found!", item_name);                            \
        }                                                                   \
    }

#define CJSON_GET_VALUE_BOOL(root, item_name, item_val)                     \
    {                                                                       \
        cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, root, item_name); \
        if (o_item)                                                         \
        {                                                                   \
            if (o_item->type & cJSON_False)                                 \
            {                                                               \
                item_val = false;                                           \
            }                                                               \
            else                                                            \
            {                                                               \
                item_val = true;                                            \
            }                                                               \
        }                                                                   \
        else                                                                \
        {                                                                   \
            item_val = false;                                               \
            TRACE_E("%s not found!", item_name);                            \
        }                                                                   \
    }

#define CJSON_TRACE(name, object)                                                           \
    {                                                                                       \
        if (object)                                                                         \
        {                                                                                   \
            char *obj_str = cJSON_Print(__FUNCTION__, object);                              \
            if (obj_str)                                                                    \
            {                                                                               \
                TRACE_D("%s[%d]: %s", name ? name : ezlopi__str, strlen(obj_str), obj_str); \
                ezlopi_free(__FUNCTION__, obj_str);                                         \
            }                                                                               \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            TRACE_E("%s: Null", name ? name : "");                                          \
        }                                                                                   \
    }

#define CJSON_GET_VALUE_STRING_BY_COPY(root, item_name, item_val)                                     \
    {                                                                                                 \
        cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, root, item_name);                           \
        if (o_item && o_item->valuestring && o_item->str_value_len)                                   \
        {                                                                                             \
            snprintf(item_val, sizeof(item_val), "%.*s", o_item->str_value_len, o_item->valuestring); \
        }                                                                                             \
    }

#define CJSON_GET_VALUE_INT CJSON_GET_VALUE_DOUBLE
#define CJSON_GET_VALUE_FLOAT CJSON_GET_VALUE_DOUBLE
#define CJSON_GET_VALUE_UINT8 CJSON_GET_VALUE_DOUBLE
#define CJSON_GET_VALUE_UINT16 CJSON_GET_VALUE_DOUBLE
#define CJSON_GET_VALUE_UINT32 CJSON_GET_VALUE_DOUBLE

#endif // EZPI_USE_CJSON_MACRO

#define CJSON_GET_VALUE_STRING_BY_COPY_INTO_PTR(root, item_name, item_val_ptr)                                       \
    {                                                                                                                \
        cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, root, item_name);                                          \
        if (o_item && o_item->valuestring && o_item->str_value_len)                                                  \
        {                                                                                                            \
            ezlopi_free(__func__, item_val_ptr);                                                                     \
            item_val_ptr = ezlopi_malloc(__func__, (o_item->str_value_len + 1));                                     \
            if (NULL != item_val_ptr)                                                                                \
            {                                                                                                        \
                snprintf(item_val_ptr, (o_item->str_value_len), "%.*s", o_item->str_value_len, o_item->valuestring); \
            }                                                                                                        \
        }                                                                                                            \
    }

#define ASSIGN_DEVICE_NAME_V2(device, dev_name)                        \
    {                                                                  \
        if ((NULL != dev_name) && ('\0' != dev_name[0]))               \
        {                                                              \
            snprintf(device->cloud_properties.device_name,             \
                     sizeof(device->cloud_properties.device_name),     \
                     "%s", dev_name);                                  \
        }                                                              \
        else                                                           \
        {                                                              \
            snprintf(device->cloud_properties.device_name,             \
                     sizeof(device->cloud_properties.device_name),     \
                     "device-%d", device->cloud_properties.device_id); \
        }                                                              \
    }

#define CJSON_GET_ID(id, cj_id)                         \
    {                                                   \
        if (cj_id && cj_id->valuestring)                \
        {                                               \
            id = strtoul(cj_id->valuestring, NULL, 16); \
        }                                               \
        else                                            \
        {                                               \
            id = 0;                                     \
        }                                               \
    }

#define CJSON_ASSIGN_ID(cj_object, id, id_str)                                 \
    {                                                                          \
        if (id && cj_object && id_str)                                         \
        {                                                                      \
            char tmp_str[32];                                                  \
            snprintf(tmp_str, sizeof(tmp_str), "%08x", id);                    \
            cJSON_AddStringToObject(__FUNCTION__, cj_object, id_str, tmp_str); \
        }                                                                      \
    }

#define CJSON_ASSIGN_NUMBER_AS_STRING(cj_obj, num, name_str)                  \
    {                                                                         \
        if (cj_obj && name_str)                                               \
        {                                                                     \
            char tmp_str[32];                                                 \
            snprintf(tmp_str, sizeof(tmp_str), "%u", (uint32_t)num);          \
            cJSON_AddStringToObject(__FUNCTION__, cj_obj, name_str, tmp_str); \
        }                                                                     \
    }

#define CJSON_GET_VALUE_GPIO(root, item_name, item_val)                     \
    {                                                                       \
        cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, root, item_name); \
        if (o_item && (o_item->type & cJSON_Number))                        \
        {                                                                   \
            item_val = o_item->valuedouble;                                 \
        }                                                                   \
        else                                                                \
        {                                                                   \
            item_val = -1;                                                  \
            TRACE_E("%s not found!", item_name);                            \
        }                                                                   \
    }

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_CJSON_MACROS_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
