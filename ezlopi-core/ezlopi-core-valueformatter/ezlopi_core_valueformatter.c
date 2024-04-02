#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>

#include "ezlopi_cloud_constants.h"
#include "ezlopi_core_valueformatter.h"

void ezlopi_valueformatter_bool_to_cjson(l_ezlopi_item_t* item, cJSON* cj_root, bool value)
{
    cJSON_AddBoolToObject(cj_root, ezlopi_value_str, value);
    cJSON_AddStringToObject(cj_root, ezlopi_valueFormatted_str, value ? ezlopi_true_str : ezlopi_false_str);

    if (item->cloud_properties.scale)
    {
        cJSON_AddStringToObject(cj_root, ezlopi_scale_str, item->cloud_properties.scale);
    }
}

void ezlopi_valueformatter_float_to_cjson(l_ezlopi_item_t* item, cJSON* cj_root, float value)
{
    cJSON_AddNumberToObject(cj_root, ezlopi_value_str, value);

    char valueFormatted[16];
    snprintf(valueFormatted, sizeof(valueFormatted), "%.02f", value);
    cJSON_AddStringToObject(cj_root, ezlopi_valueFormatted_str, valueFormatted);

    if (item->cloud_properties.scale)
    {
        cJSON_AddStringToObject(cj_root, ezlopi_scale_str, item->cloud_properties.scale);
    }
}

void ezlopi_valueformatter_double_to_cjson(l_ezlopi_item_t* item, cJSON* cj_root, double value)
{
    cJSON_AddNumberToObject(cj_root, ezlopi_value_str, value);

    char valueFormatted[16];
    snprintf(valueFormatted, sizeof(valueFormatted), "%.2lf", value);
    cJSON_AddStringToObject(cj_root, ezlopi_valueFormatted_str, valueFormatted);

    if (item->cloud_properties.scale)
    {
        cJSON_AddStringToObject(cj_root, ezlopi_scale_str, item->cloud_properties.scale);
    }
}

void ezlopi_valueformatter_int32_to_cjson(l_ezlopi_item_t* item, cJSON* cj_root, int value)
{
    cJSON_AddNumberToObject(cj_root, ezlopi_value_str, value);

    char valueFormatted[16];
    snprintf(valueFormatted, sizeof(valueFormatted), "%d", value);
    cJSON_AddStringToObject(cj_root, ezlopi_valueFormatted_str, valueFormatted);

    if (item->cloud_properties.scale)
    {
        cJSON_AddStringToObject(cj_root, ezlopi_scale_str, item->cloud_properties.scale);
    }
}

void ezlopi_valueformatter_uint32_to_cjson(l_ezlopi_item_t* item, cJSON* cj_root, uint32_t value)
{
    cJSON_AddNumberToObject(cj_root, ezlopi_value_str, value);

    char valueFormatted[16];
    snprintf(valueFormatted, sizeof(valueFormatted), "%u", value);
    cJSON_AddStringToObject(cj_root, ezlopi_valueFormatted_str, valueFormatted);

    if (item->cloud_properties.scale)
    {
        cJSON_AddStringToObject(cj_root, ezlopi_scale_str, item->cloud_properties.scale);
    }
}
