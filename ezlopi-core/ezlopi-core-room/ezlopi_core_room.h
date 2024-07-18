#ifndef _EZLOPI_CORE_ROOM_H_
#define _EZLOPI_CORE_ROOM_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cjext.h"

typedef enum e_room_subtype
{
#ifndef ROOM_SUBTYPE
#define ROOM_SUBTYPE(name, e_num) ROOM_SUBTYPE_##e_num,
#include "ezlopi_core_room_subtype_macro.h"
#undef ROOM_SUBTYPE
#endif
} e_room_subtype_t;

typedef struct s_ezlopi_room
{
    uint32_t _pos;
    char name[32];
    uint32_t _id;
    e_room_subtype_t subtype;
    uint32_t image_id;
    struct s_ezlopi_room* next;
} s_ezlopi_room_t;

s_ezlopi_room_t* ezlopi_room_get_room_head(void);
char * ezlopi_core_room_get_name_by_id(uint32_t room_id);

void ezlopi_room_init(void);
int ezlopi_room_delete(cJSON* cj_room);
int ezlopi_room_delete_all(void);
int ezlopi_room_name_set(cJSON* cj_room);
int ezlopi_room_add_to_nvs(cJSON* cj_room);
int ezlopi_room_reorder(cJSON* cj_rooms_ids);
s_ezlopi_room_t* ezlopi_room_add_to_list(cJSON* cj_room);

#endif // _EZLOPI_CORE_ROOM_H_