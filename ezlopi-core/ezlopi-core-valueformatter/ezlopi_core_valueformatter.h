#ifndef EZLOPI_VALUEFORMATER_H
#define EZLOPI_VALUEFORMATER_H

#include <stdbool.h>
#include "cjext.h"

void ezlopi_valueformatter_bool_to_cjson(cJSON* cj_root, bool value, const char * scale);
void ezlopi_valueformatter_float_to_cjson(cJSON* cj_root, float value, const char * scale);
void ezlopi_valueformatter_double_to_cjson(cJSON* cj_root, double value, const char * scale);
void ezlopi_valueformatter_int32_to_cjson(cJSON* cj_root, int value, const char * scale);
void ezlopi_valueformatter_uint32_to_cjson(cJSON* cj_root, uint32_t value, const char * scale);

#endif // EZLOPI_VALUEFORMATER_H
