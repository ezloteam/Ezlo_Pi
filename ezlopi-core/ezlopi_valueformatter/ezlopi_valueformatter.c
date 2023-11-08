#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char *ezlopi_valueformatter_bool(bool val)
{
    return val ? "true" : "false";
}

char *ezlopi_valueformatter_float(float val)
{
#warning "malloc not checked"
    char *formatted_val_str = (char *)malloc(15);
    snprintf(formatted_val_str, 15, "%.2f", val);
    return formatted_val_str;
}

char *ezlopi_valueformatter_double(double val)
{
#warning "malloc not checked"
    char *formatted_val_str = (char *)malloc(15);
    snprintf(formatted_val_str, 15, "%.2lf", val);
    return formatted_val_str;
}

char *ezlopi_valueformatter_int(int val)
{
#warning "malloc not checked"
    char *formatted_val_str = (char *)malloc(15);
    snprintf(formatted_val_str, 15, "%d", val);
    return formatted_val_str;
}

char *ezlopi_valueformatter_int32(int32_t val)
{
#warning "malloc not checked"
    char *formatted_val_str = (char *)malloc(15);
    snprintf(formatted_val_str, 15, "%d", val);
    return formatted_val_str;
}

char *ezlopi_valueformatter_uint32(uint32_t val)
{
#warning "malloc not checked"
    char *formatted_val_str = (char *)malloc(15);
    snprintf(formatted_val_str, 15, "%d", val);
    return formatted_val_str;
}

char *ezlopi_valueformatter_rgb(uint8_t r, uint8_t g, uint8_t b)
{
#warning "malloc not checked"
    char *formatted_val_str = (char *)malloc(10);
    snprintf(formatted_val_str, 10, "#%02x%02x%02x", r, g, b);
    return formatted_val_str;
}