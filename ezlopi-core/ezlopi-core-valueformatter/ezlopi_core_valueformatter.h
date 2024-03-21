#ifndef EZLOPI_VALUEFORMATER_H
#define EZLOPI_VALUEFORMATER_H

#include <stdbool.h>
#include "ezlopi_core_devices.h"

#define EZPI_VALUEFORMATTER_BOOL(val) val ? ezlopi_true_str : ezlopi_false_str


void ezlopi_valueformatter_float_to_cjson(l_ezlopi_item_t* item, cJSON* cj_root, float value);


char* ezlopi_valueformatter_double(double val);
char* ezlopi_valueformatter_int(int val);
char* ezlopi_valueformatter_int32(int32_t val);
char* ezlopi_valueformatter_uint32(uint32_t val);
char* ezlopi_valueformatter_rgb(uint8_t r, uint8_t g, uint8_t b);

#endif // EZLOPI_VALUEFORMATER_H
