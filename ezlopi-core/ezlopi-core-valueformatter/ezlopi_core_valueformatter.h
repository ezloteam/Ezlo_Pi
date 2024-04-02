#ifndef EZLOPI_VALUEFORMATER_H
#define EZLOPI_VALUEFORMATER_H

#include <stdbool.h>
#include "ezlopi_core_devices.h"

void ezlopi_valueformatter_bool_to_cjson(l_ezlopi_item_t* item, cJSON* cj_root, bool value);
void ezlopi_valueformatter_float_to_cjson(l_ezlopi_item_t* item, cJSON* cj_root, float value);
void ezlopi_valueformatter_double_to_cjson(l_ezlopi_item_t* item, cJSON* cj_root, double value);
void ezlopi_valueformatter_int32_to_cjson(l_ezlopi_item_t* item, cJSON* cj_root, int value);
void ezlopi_valueformatter_uint32_to_cjson(l_ezlopi_item_t* item, cJSON* cj_root, uint32_t value);

#endif // EZLOPI_VALUEFORMATER_H
