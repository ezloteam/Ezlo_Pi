#ifndef _EZLOPI_CORE_CJSON_MACROS_H_
#define _EZLOPI_CORE_CJSON_MACROS_H_

#include "ezlopi_util_trace.h"

#define CJSON_GET_VALUE_DOUBLE(root, item_name, item_val)     \
    {                                                         \
        cJSON *o_item = cJSON_GetObjectItem(root, item_name); \
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
        cJSON *o_item = cJSON_GetObjectItem(root, item_name); \
        if (o_item)                                           \
        {                                                     \
            if (o_item->type == cJSON_False)                  \
            {                                                 \
                item_val = false;                             \
            }                                                 \
            else                                              \
            {                                                 \
                item_val = true;                              \
            }                                                 \
            item_val = o_item->valuedouble;                      \
        }                                                     \
        else                                                  \
        {                                                     \
            item_val = false;                                 \
            TRACE_E("%s not found!", item_name);              \
        }                                                     \
    }


#define CJSON_GET_VALUE_STRING(root, item_name, item_val)     \
    {                                                         \
        cJSON *o_item = cJSON_GetObjectItem(root, item_name); \
        if (o_item && o_item->valuestring)                    \
        {                                                     \
            item_val = o_item->valuestring;                   \
        }                                                     \
        else                                                  \
        {                                                     \
            item_val = NULL;                                  \
            TRACE_E("%s: NULL", item_name);                   \
        }                                                     \
    }


#define CJSON_TRACE(name, object)                                         \
    {                                                                     \
        if (object)                                                       \
        {                                                                 \
            char *obj_str = cJSON_Print(object);                          \
            if (obj_str)                                                  \
            {                                                             \
                TRACE_D("%s: %s", name ? name : ezlopi__str, obj_str); \
                free(obj_str);                                            \
            }                                                             \
        }                                                                 \
        else                                                              \
        {                                                                 \
            TRACE_E("%s: Null", name ? name : "");                        \
        }                                                                 \
    }


#define CJSON_GET_VALUE_STRING_BY_COPY(root, item_name, item_val)     \
    {                                                                 \
        char *tmp_item_val = NULL;                                    \
        CJSON_GET_VALUE_STRING(root, item_name, tmp_item_val);        \
        if (tmp_item_val)                                             \
        {                                                             \
            snprintf(item_val, sizeof(item_val), "%s", tmp_item_val); \
        }                                                             \
    }


#define ASSIGN_DEVICE_NAME(digital_io_device_properties, dev_name)                                \
    {                                                                                             \
        if ((NULL != dev_name) && ('\0' != dev_name[0]))                                          \
        {                                                                                         \
            snprintf(digital_io_device_properties->ezlopi_cloud.device_name,                      \
                     sizeof(digital_io_device_properties->ezlopi_cloud.device_name),              \
                     "%s", dev_name);                                                             \
        }                                                                                         \
        else                                                                                      \
        {                                                                                         \
            snprintf(digital_io_device_properties->ezlopi_cloud.device_name,                      \
                     sizeof(digital_io_device_properties->ezlopi_cloud.device_name),              \
                     "dev-%d:digital_out", digital_io_device_properties->ezlopi_cloud.device_id); \
        }                                                                                         \
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


#define CJSON_ASSIGN_ID(cj_object, id, id_str)                   \
    {                                                            \
        if (id && cj_object && id_str)                           \
        {                                                        \
            char tmp_str[32];                                    \
            snprintf(tmp_str, sizeof(tmp_str), "%08x", id);      \
            cJSON_AddStringToObject(cj_object, id_str, tmp_str); \
        }                                                        \
    }


#define CJSON_ASSIGN_NUMBER_AS_STRING(cj_obj, num, name_str)         \
    {                                                                \
        if (cj_obj && name_str)                                      \
        {                                                            \
            char tmp_str[32];                                        \
            snprintf(tmp_str, sizeof(tmp_str), "%u", (uint32_t)num); \
            cJSON_AddStringToObject(cj_obj, name_str, tmp_str);      \
        }                                                            \
    }

#endif // _EZLOPI_CORE_CJSON_MACROS_H_