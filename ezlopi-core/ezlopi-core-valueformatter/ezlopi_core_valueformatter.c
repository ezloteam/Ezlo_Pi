#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_valueformatter.h"

const char *ezlopi_valueformatter_bool(bool val)
{
    return val ? ezlopi_true_str : ezlopi_false_str;
}

char *ezlopi_valueformatter_float(float val)
{

    char *formatted_val_str = (char *)malloc(15);
    if (formatted_val_str)
    {
        snprintf(formatted_val_str, 15, "%.2f", val);
        return formatted_val_str;
    }
    else
    {
        return NULL;
    }
}

char *ezlopi_valueformatter_double(double val)
{
    char *formatted_val_str = (char *)malloc(15);
    if (formatted_val_str)
    {
        snprintf(formatted_val_str, 15, "%.2lf", val);
        return formatted_val_str;
    }
    else
    {
        return NULL;
    }
}

char *ezlopi_valueformatter_int(int val)
{
    char *formatted_val_str = (char *)malloc(15);
    if (formatted_val_str)
    {
        snprintf(formatted_val_str, 15, "%d", val);
        return formatted_val_str;
    }
    else
    {
        return NULL;
    }
}

char *ezlopi_valueformatter_int32(int32_t val)
{
    char *formatted_val_str = (char *)malloc(15);
    if (formatted_val_str)
    {
        snprintf(formatted_val_str, 15, "%d", val);
        return formatted_val_str;
    }
    else
    {
        return NULL;
    }
}

char *ezlopi_valueformatter_uint32(uint32_t val)
{
    char *formatted_val_str = (char *)malloc(15);
    if (formatted_val_str)
    {
        snprintf(formatted_val_str, 15, "%d", val);
        return formatted_val_str;
    }
    else
    {
        return NULL;
    }
}

char *ezlopi_valueformatter_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    char *formatted_val_str = (char *)malloc(10);
    if (formatted_val_str)
    {
        snprintf(formatted_val_str, 10, "#%02x%02x%02x", r, g, b);
        return formatted_val_str;
    }
    else
    {
        return NULL;
    }
}