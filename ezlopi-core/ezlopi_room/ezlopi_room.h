#ifndef __EZLOPI_ROOM_H__
#define __EZLOPI_ROOM_H__

#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#include "cJSON.h"

typedef struct s_ezlopi_room
{
    char name[32];
    uint32_t _id;
    struct s_ezlopi_room *next;
} s_ezlopi_room_t;

s_ezlopi_room_t *ezlopi_room_get_room_head(void);

void ezlopi_room_init(void);
int ezlopi_room_delete(cJSON *cj_room);
int ezlopi_room_delete_all(void);
int ezlopi_room_name_set(cJSON *cj_room);
int ezlopi_room_add_to_nvs(cJSON *cj_room);
s_ezlopi_room_t *ezlopi_room_add_to_list(cJSON *cj_room);

#endif // __EZLOPI_ROOM_H__