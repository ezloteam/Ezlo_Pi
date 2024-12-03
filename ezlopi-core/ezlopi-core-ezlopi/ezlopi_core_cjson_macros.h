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

#ifndef _EZLOPI_CORE_CJSON_MACROS_H_
#define _EZLOPI_CORE_CJSON_MACROS_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
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
    #define CJSON_GET_VALUE_DOUBLE(root, item_name, item_val)     \
        {                                                         \
            cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, root, item_name); \
            if (o_item)                                           \
            {                                                     \
                item_val = o_item->valuedouble;                   \
            }                                                     \
            else                                                  \
            {                                                     \
                item_val = 0;                                     \
                TRACE_E("%s not found!", item_name);              \
            }                                                     \
        }

    #define CJSON_GET_VALUE_BOOL(root, item_name, item_val)       \
        {                                                         \
            cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, root, item_name); \
            if (o_item)                                           \
            {                                                     \
                if (o_item->type & cJSON_False)                   \
                {                                                 \
                    item_val = false;                             \
                }                                                 \
                else                                              \
                {                                                 \
                    item_val = true;                              \
                }                                                 \
            }                                                     \
            else                                                  \
            {                                                     \
                item_val = false;                                 \
                TRACE_E("%s not found!", item_name);              \
            }                                                     \
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

    #define CJSON_GET_VALUE_STRING_BY_COPY(root, item_name, item_val)                                       \
        {                                                                                                   \
            cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, root, item_name);                             \
            if (o_item && o_item->valuestring && o_item->str_value_len)                                     \
            {                                                                                               \
                snprintf(item_val, sizeof(item_val), "%.*s", o_item->str_value_len, o_item->valuestring);   \
            }                                                                                               \
        }

    #define ASSIGN_DEVICE_NAME_V2(device, dev_name)                          \
        {                                                                    \
            if ((NULL != dev_name) && ('\0' != dev_name[0]))                 \
            {                                                                \
                snprintf(device->cloud_properties.device_name,               \
                         sizeof(device->cloud_properties.device_name),       \
                         "%s", dev_name);                                    \
            }                                                                \
            else                                                             \
            {                                                                \
                snprintf(device->cloud_properties.device_name,               \
                         sizeof(device->cloud_properties.device_name),       \
                         "device-%d", device->cloud_properties.device_id);   \
            }                                                                \
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

    #define CJSON_ASSIGN_ID(cj_object, id, id_str)                   \
        {                                                            \
            if (id && cj_object && id_str)                           \
            {                                                        \
                char tmp_str[32];                                    \
                snprintf(tmp_str, sizeof(tmp_str), "%08x", id);      \
                cJSON_AddStringToObject(__FUNCTION__, cj_object, id_str, tmp_str); \
            }                                                        \
        }

    #define CJSON_ASSIGN_NUMBER_AS_STRING(cj_obj, num, name_str)         \
        {                                                                \
            if (cj_obj && name_str)                                      \
            {                                                            \
                char tmp_str[32];                                        \
                snprintf(tmp_str, sizeof(tmp_str), "%u", (uint32_t)num); \
                cJSON_AddStringToObject(__FUNCTION__, cj_obj, name_str, tmp_str);      \
            }                                                            \
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
    /**
     * @brief Global function template example
     * Convention : Use capital letter for initial word on extern function
     * maincomponent : Main component as hal, core, service etc.
     * subcomponent : Sub component as i2c from hal, ble from service etc
     * functiontitle : Title of the function
     * eg : EZPI_hal_i2c_init()
     * @param arg
     *
     */
    void EZPI_maincomponent_subcomponent_functiontitle?( type_t arg );

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_CJSON_MACROS_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
