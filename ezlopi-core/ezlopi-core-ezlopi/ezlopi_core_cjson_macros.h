#ifndef _EZLOPI_CORE_CJSON_MACROS_H_
#define _EZLOPI_CORE_CJSON_MACROS_H_

#include "ezlopi_util_trace.h"

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

#endif // _EZLOPI_CORE_CJSON_MACROS_H_