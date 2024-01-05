#include "cJSON.h"
#include "trace.h"

#include "ezlopi_nvs.h"
#include "ezlopi_room.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud.h"

static s_ezlopi_room_t *l_room_head = NULL;

s_ezlopi_room_t *ezlopi_room_add_to_list(cJSON *cj_room)
{
    s_ezlopi_room_t *new_room = malloc(sizeof(s_ezlopi_room_t));
    if (new_room)
    {
        memset(new_room, 0, sizeof(s_ezlopi_room_t));

        if (cj_room)
        {
            cJSON *cj_room_id = cJSON_GetObjectItem(cj_room, ezlopi__id_str);
            if (cj_room_id && cj_room_id->valuestring)
            {
                new_room->_id = strtoul(cj_room_id->valuestring, NULL, 16);
                ezlopi_cloud_update_room_id(new_room->_id);
            }
            else
            {
                new_room->_id = ezlopi_cloud_generate_room_id();
                char tmp_str[32];
                snprintf(tmp_str, sizeof(tmp_str), "%08x", new_room->_id);
                cJSON_AddStringToObject(cj_room, ezlopi__id_str, tmp_str);
            }

            cJSON *cj_room_name = cJSON_GetObjectItem(cj_room, ezlopi_name_str);
            if (cj_room_name && cj_room_name->valuestring)
            {
                snprintf(new_room->name, sizeof(new_room->name), "%s", cj_room->valuestring);
            }
        }
    }

    return new_room;
}

void ezlopi_room_init(void)
{
    char *rooms_str = ezlopi_nvs_read_room_ids();
    if (rooms_str)
    {
        cJSON *cj_rooms = cJSON_Parse(rooms_str);
        free(rooms_str);

        if (cj_rooms)
        {
            int idx = 0;
            cJSON *cj_room = NULL;
            s_ezlopi_room_t *curr_room_node = NULL;

            while (NULL != (cj_room = cJSON_GetArrayItem(cj_rooms, idx++)))
            {

            }
        }
    }
}