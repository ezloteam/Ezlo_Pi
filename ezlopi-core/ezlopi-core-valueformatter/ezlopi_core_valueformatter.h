#ifndef EZLOPI_VALUEFORMATER_H
#define EZLOPI_VALUEFORMATER_H

#include <stdbool.h>

const char *ezlopi_valueformatter_bool(bool val);
char *ezlopi_valueformatter_float(float val);
char *ezlopi_valueformatter_double(double val);
char *ezlopi_valueformatter_int(int val);
char *ezlopi_valueformatter_int32(int32_t val);
char *ezlopi_valueformatter_uint32(uint32_t val);
char *ezlopi_valueformatter_rgb(uint8_t r, uint8_t g, uint8_t b);

#endif // EZLOPI_VALUEFORMATER_H
