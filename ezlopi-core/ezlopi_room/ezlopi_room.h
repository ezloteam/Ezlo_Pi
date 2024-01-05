#ifndef __EZLOPI_ROOM_H__
#define __EZLOPI_ROOM_H__

#include "string.h"
#include "stdlib.h"
#include "stdio.h"

typedef struct s_ezlopi_room
{
    char name[32];
    uint32_t _id;
    struct s_ezlopi_room *next;
} s_ezlopi_room_t;

#endif // __EZLOPI_ROOM_H__