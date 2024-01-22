#ifndef _EZLOPI_HAL_ONEWIRE_H_
#define _EZLOPI_HAL_ONEWIRE_H_

#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"

typedef struct s_ezlopi_onewire
{
    bool enable;
    uint32_t onewire_pin;
} s_ezlopi_onewire_t;

#endif // _EZLOPI_HAL_ONEWIRE_H_
