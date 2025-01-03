#include "cjext.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_core_cjson_macros.h"

void EZPI_core_cjson_get_value_double(cJSON *cj_root, char *item_name_str, double *item_val)
{
    *item_val = 0;
    cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, item_name_str);
    if (o_item && (o_item->type & cJSON_Number))
    {
        *item_val = o_item->valuedouble;
    }
}

void EZPI_core_cjson_get_value_bool(cJSON *cj_root, char *item_name_str, bool *item_val)
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

void EZPI_core_cjson_trace(char *name_str, cJSON *cj_object)
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
        TRACE_E("%s: Null", name_str ? name_str : "");
    }
}

#if 0
#define CJSON_GET_VALUE_STRING_BY_COPY(cj_root, char *item_name_str, item_val)                        \
    {                                                                                                 \
        cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, char *item_name_str);              \
        if (o_item && o_item->valuestring && o_item->str_value_len)                                   \
        {                                                                                             \
            snprintf(item_val, sizeof(item_val), "%.*s", o_item->str_value_len, o_item->valuestring); \
        }                                                                                             \
    }

#define CJSON_GET_VALUE_STRING_BY_COPY_INTO_PTR(cj_root, char *item_name_str, item_val_ptr)                          \
    {                                                                                                                \
        cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, char *item_name_str);                             \
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

#define CJSON_GET_VALUE_GPIO(cj_root, char *item_name_str, item_val)                     \
    {                                                                                    \
        cJSON *o_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, char *item_name_str); \
        if (o_item && (o_item->type & cJSON_Number))                                     \
        {                                                                                \
            item_val = o_item->valuedouble;                                              \
        }                                                                                \
        else                                                                             \
        {                                                                                \
            item_val = -1;                                                               \
            TRACE_E("%s not found!", char *item_name_str);                               \
        }                                                                                \
    }
#endif