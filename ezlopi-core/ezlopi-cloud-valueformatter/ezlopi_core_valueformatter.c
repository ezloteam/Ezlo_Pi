#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cjext.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_core_valueformatter.h"

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
