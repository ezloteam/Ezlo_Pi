#ifndef __EZLOPI_ONEWIRE_H__
#define __EZLOPI_ONEWIRE_H__

#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"

typedef struct s_ezlopi_onewire
{
    bool enable;
    uint32_t onewire_pin;
} s_ezlopi_onewire_t;

#endif // __EZLOPI_ONEWIRE_H__
