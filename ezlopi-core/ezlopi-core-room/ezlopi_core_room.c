/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_room.c
 * @brief   perform some function on rooms
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include <time.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_room.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_constants.h"
#include "EZLOPI_USER_CONFIG.h"

// #include "ezlopi_service_webprov.h"
/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static void __sort_by_pos(void);
static void __free_all_room_nodes(s_ezlopi_room_t *room);
static int __free_room_from_list_by_id(uint32_t room_id);
static void __update_cloud_room_deleted(uint32_t room_id);
static int __remove_room_from_nvs_by_id(uint32_t a_room_id);
static e_room_subtype_t __get_subtype_enum(char *subtype_str);
// static const char *__get_subtype_name(e_room_subtype_t subtype);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

static s_ezlopi_room_t *l_room_head = NULL;

static const char *sc_room_subtype_name[] = {
#ifndef ROOM_SUBTYPE
#define ROOM_SUBTYPE(name, e_num) name,
#include "ezlopi_core_room_subtype_macro.h"
#undef ROOM_SUBTYPE
#endif
};

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
char *EZPI_core_room_get_name_by_id(uint32_t room_id)
{
    char *ret = NULL;

    if (room_id && l_room_head)
    {
        s_ezlopi_room_t *cur_room = l_room_head;
        while (cur_room)
        {
            if (room_id == cur_room->_id)
            {
                ret = cur_room->name;
                break;
            }

            cur_room = cur_room->next;
        }
    }

    return ret;
}

s_ezlopi_room_t *EZPI_core_room_get_room_head(void)
{
    return l_room_head;
}

int EZPI_core_room_name_set(cJSON *cj_room)
{
    int ret = 0;
    if (cj_room)
    {
        cJSON *cj_room_id = cJSON_GetObjectItem(__FUNCTION__, cj_room, ezlopi__id_str);
        cJSON *cj_room_name = cJSON_GetObjectItem(__FUNCTION__, cj_room, ezlopi_name_str);

        if (cj_room_id && cj_room_id->valuestring && cj_room_name && cj_room_name->valuestring)
        {
            uint32_t room_id = strtoul(cj_room_id->valuestring, NULL, 16);
            if (room_id)
            {
                char *rooms_str = EZPI_core_nvs_read_rooms();
                if (rooms_str)
                {
                    cJSON *cj_rooms = cJSON_Parse(__FUNCTION__, rooms_str);
                    ezlopi_free(__FUNCTION__, rooms_str);

                    if (cj_rooms)
                    {
                        cJSON *cj_room_tmp = NULL;
                        cJSON_ArrayForEach(cj_room_tmp, cj_rooms)
                        {
                            cJSON *cj_room_tmp_id = cJSON_GetObjectItem(__FUNCTION__, cj_room_tmp, ezlopi__id_str);
                            if (cj_room_tmp_id && cj_room_tmp_id->valuestring)
                            {
                                uint32_t tmp_room_id = strtoul(cj_room_tmp_id->valuestring, NULL, 16);
                                if (room_id == tmp_room_id)
                                {
                                    cJSON_DeleteItemFromObject(__FUNCTION__, cj_room_tmp, ezlopi_name_str);
                                    ret = cJSON_AddItemToObject(__FUNCTION__, cj_room_tmp, ezlopi_name_str, cJSON_Duplicate(__FUNCTION__, cj_room_name, true));

                                    s_ezlopi_room_t *room_node = l_room_head;
                                    while (room_node)
                                    {
                                        if (room_node->_id == room_id)
                                        {
                                            snprintf(room_node->name, sizeof(room_node->name), "%s", cj_room_name->valuestring);
                                            break;
                                        }
                                        room_node = room_node->next;
                                    }
                                    break;
                                }
                            }
                        }

                        if (ret)
                        {
                            char *updated_rooms = cJSON_PrintBuffered(__FUNCTION__, cj_rooms, 4096, false);
                            TRACE_D("length of 'updated_rooms': %d", strlen(updated_rooms));

                            if (updated_rooms)
                            {
                                EZPI_core_nvs_write_rooms(updated_rooms);
                                ezlopi_free(__FUNCTION__, updated_rooms);
                            }
                        }

                        cJSON_Delete(__FUNCTION__, cj_rooms);
                    }
                }
            }
        }
    }

    return ret;
}

int EZPI_core_room_delete(cJSON *cj_room)
{
    int ret = 0;
    if (cj_room)
    {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        CJSON_TRACE("cj_room", cj_room);
#endif

        cJSON *cj_room_id = cJSON_GetObjectItem(__FUNCTION__, cj_room, ezlopi__id_str);

        if (cj_room_id && cj_room_id->valuestring)
        {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            CJSON_TRACE("cj_room_id", cj_room_id);
#endif

            uint32_t room_id = strtoul(cj_room_id->valuestring, NULL, 16);

            if (room_id)
            {
                TRACE_I("room_id: %8x", room_id);

                ret = __free_room_from_list_by_id(room_id);
                TRACE_I("ret: %d", ret);
                ret = __remove_room_from_nvs_by_id(room_id);
                TRACE_I("ret: %d", ret);
            }
        }
    }

    return ret;
}

int EZPI_core_room_delete_all(void)
{
    int ret = 1;

    __free_all_room_nodes(l_room_head);
    l_room_head = NULL;

    EZPI_core_nvs_write_rooms("[]");

    return ret;
}

int EZPI_core_room_add_to_nvs(cJSON *cj_room)
{
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    CJSON_TRACE("cj_room", cj_room);
#endif

    int ret = 0;
    if (cj_room)
    {
        int room_free = 1;
        char *rooms_str = EZPI_core_nvs_read_rooms();

        if (NULL == rooms_str)
        {
            rooms_str = "[]";
            room_free = 0;
        }

        cJSON *cj_rooms = cJSON_Parse(__FUNCTION__, rooms_str);
        if (room_free)
        {
            ezlopi_free(__FUNCTION__, rooms_str);
        }

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        CJSON_TRACE("cj_rooms", cj_rooms);
#endif

        if (cj_rooms)
        {
            if (cJSON_AddItemReferenceToArray(__FUNCTION__, cj_rooms, cj_room))
            {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                CJSON_TRACE("cj_rooms", cj_rooms);
#endif

                char *updated_rooms_str = cJSON_PrintBuffered(__FUNCTION__, cj_rooms, 4096, false);
                TRACE_D("length of 'updated_rooms_str': %d", strlen(updated_rooms_str));

                if (updated_rooms_str)
                {
                    if (EZPI_core_nvs_write_rooms(updated_rooms_str))
                    {
                        TRACE_I("room saved");
                        ret = 1;
                    }

                    ezlopi_free(__FUNCTION__, updated_rooms_str);
                }
            }

            cJSON_Delete(__FUNCTION__, cj_rooms);
        }
    }

    return ret;
}

int EZPI_core_room_reorder(cJSON *cj_rooms_ids)
{
    int ret = 0;

    if (cj_rooms_ids)
    {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        CJSON_TRACE("new-order ids", cj_rooms_ids);
#endif

        uint32_t rooms_id_arr_size = cJSON_GetArraySize(cj_rooms_ids);

        if (rooms_id_arr_size)
        {
            int idx = 0;
            cJSON *cj_reordered_rooms = cJSON_CreateArray(__FUNCTION__);

            if (cj_reordered_rooms)
            {
                cJSON *cj_room_id = NULL;

                cJSON_ArrayForEach(cj_room_id, cj_rooms_ids)
                {
                    uint32_t room_id = strtoul(cj_room_id->valuestring, NULL, 16);
                    if (room_id)
                    {
                        s_ezlopi_room_t *room_node = l_room_head;
                        while (room_node)
                        {
                            if (room_id == room_node->_id)
                            {
                                room_node->_pos = idx;

                                cJSON *cj_room = cJSON_CreateObject(__FUNCTION__);
                                if (cj_room)
                                {
                                    cJSON_AddStringToObject(__FUNCTION__, cj_room, ezlopi__id_str, cj_room_id->valuestring);
                                    cJSON_AddStringToObject(__FUNCTION__, cj_room, ezlopi_name_str, room_node->name);

                                    if (!cJSON_AddItemToArray(cj_reordered_rooms, cj_room))
                                    {
                                        cJSON_Delete(__FUNCTION__, cj_room);
                                    }
                                }
                            }

                            room_node = room_node->next;
                        }
                    }

                    idx++;
                }

                __sort_by_pos();

                if (cJSON_GetArraySize(cj_reordered_rooms))
                {
                    char *reordered_rooms_str = cJSON_PrintBuffered(__FUNCTION__, cj_reordered_rooms, 4096, false);
                    TRACE_D("length of 'reordered_rooms_str': %d", strlen(reordered_rooms_str));

                    if (reordered_rooms_str)
                    {
                        EZPI_core_nvs_write_rooms(reordered_rooms_str);
                        ezlopi_free(__FUNCTION__, reordered_rooms_str);
                    }
                }

                cJSON_Delete(__FUNCTION__, cj_reordered_rooms);
            }
        }
    }

    return ret;
}

s_ezlopi_room_t *EZPI_core_room_add_to_list(cJSON *cj_room)
{
    s_ezlopi_room_t *new_room = ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_room_t));
    if (new_room)
    {
        memset(new_room, 0, sizeof(s_ezlopi_room_t));

        if (cj_room)
        {
            cJSON *cj_room_id = cJSON_GetObjectItem(__FUNCTION__, cj_room, ezlopi__id_str);
            if (cj_room_id && cj_room_id->valuestring)
            {
                new_room->_id = strtoul(cj_room_id->valuestring, NULL, 16);
                EZPI_core_cloud_update_room_id(new_room->_id);
            }
            else
            {
                char tmp_str[32];
                new_room->_id = EZPI_core_cloud_generate_room_id();
                snprintf(tmp_str, sizeof(tmp_str), "%08x", new_room->_id);
                cJSON_AddStringToObject(__FUNCTION__, cj_room, ezlopi__id_str, tmp_str);
            }

            cJSON *cj_room_subtype = cJSON_GetObjectItem(__FUNCTION__, cj_room, ezlopi_subtype_str);
            if (cj_room_subtype && cj_room_subtype->valuestring)
            {
                new_room->subtype = __get_subtype_enum(cj_room_subtype->valuestring);
            }
            else
            {
                new_room->subtype = ROOM_SUBTYPE_NONE;
            }

            cJSON *cj_room_name = cJSON_GetObjectItem(__FUNCTION__, cj_room, ezlopi_name_str);
            if (cj_room_name && cj_room_name->valuestring)
            {
                snprintf(new_room->name, sizeof(new_room->name), "%s", cj_room_name->valuestring);
            }
        }
    }

    return new_room;
}

ezlopi_error_t EZPI_room_init(void)
{
    ezlopi_error_t error = EZPI_ERR_ROOM_INIT_FAILED;
    char *rooms_str = EZPI_core_nvs_read_rooms();
    if (rooms_str)
    {
        TRACE_D("rooms: %s", rooms_str);

        cJSON *cj_rooms = cJSON_Parse(__FUNCTION__, rooms_str);
        ezlopi_free(__FUNCTION__, rooms_str);

        if (cj_rooms)
        {
            int idx = 0;
            cJSON *cj_room = NULL;
            s_ezlopi_room_t *curr_room_node = NULL;

            cJSON_ArrayForEach(cj_room, cj_rooms)
            {
                s_ezlopi_room_t *new_room = EZPI_core_room_add_to_list(cj_room);
                if (new_room)
                {
                    new_room->_pos = idx;
                    if (curr_room_node)
                    {
                        curr_room_node->next = new_room;
                        curr_room_node = curr_room_node->next;
                    }
                    else
                    {
                        l_room_head = new_room;
                        curr_room_node = l_room_head;
                    }
                }

                idx++;
            }
            error = EZPI_SUCCESS;
        }
    }
    return error;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

static void __update_cloud_room_deleted(uint32_t room_id)
{
    cJSON *cj_response = cJSON_CreateObject(__FUNCTION__);
    {
        // cJSON_AddNumberToObject(__FUNCTION__, cj_response, ezlopi_startTime_str, EZPI_core_sntp_get_current_time_sec());

        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_room_deleted_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_method_str, method_hub_room_all_delete);

        cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            char tmp_str[32];
            snprintf(tmp_str, sizeof(tmp_str), "%08x", room_id);
            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi__id_str, tmp_str);
        }

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
#endif

        if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_response, EZPI_core_sntp_get_current_time_sec()))
        {
            cJSON_Delete(__FUNCTION__, cj_response);
        }
    }
}

static void __free_all_room_nodes(s_ezlopi_room_t *room)
{
    if (room)
    {
        __free_all_room_nodes(room->next);
        __update_cloud_room_deleted(room->_id);
        ezlopi_free(__FUNCTION__, room);
    }
}

static void __sort_by_pos(void)
{
    uint32_t size = 0;
    s_ezlopi_room_t *room_ptr_arr[32];
    s_ezlopi_room_t *curr_room = l_room_head;

    curr_room = l_room_head;
    memset(room_ptr_arr, 0, sizeof(room_ptr_arr));

    while (curr_room)
    {
        size++;
        room_ptr_arr[curr_room->_pos] = curr_room;
        curr_room = curr_room->next;
    }

    l_room_head = room_ptr_arr[0];
    l_room_head->next = NULL;
    curr_room = l_room_head;

    int idx = 1;

    while (idx < size)
    {
        curr_room->next = room_ptr_arr[idx];
        curr_room->next->next = NULL;
        idx++;
    }
}

// static const char *__get_subtype_name(e_room_subtype_t subtype)
// {
//     const char *ret = ezlopi__str;
//     if ((subtype > ROOM_SUBTYPE_NONE) && (subtype < ROOM_SUBTYPE_MAX))
//     {
//         ret = sc_room_subtype_name[subtype];
//     }
//     return ret;
// }

static e_room_subtype_t __get_subtype_enum(char *subtype_str)
{
    e_room_subtype_t ret = ROOM_SUBTYPE_NONE;
    if (subtype_str)
    {
        while ((ret <= ROOM_SUBTYPE_MAX) && sc_room_subtype_name[ret])
        {
            if (EZPI_STRNCMP_IF_EQUAL(sc_room_subtype_name[ret], subtype_str, strlen(sc_room_subtype_name[ret]) + 1, strlen(subtype_str) + 1))
            {
                break;
            }
            ret++;
        }
    }
    return ret;
}

static int __free_room_from_list_by_id(uint32_t room_id)
{
    int ret = 0;

    if (l_room_head)
    {
        TRACE_I("l_room_head->_id == room_id ->  %8x == %8x", l_room_head->_id, room_id);

        if (l_room_head->_id == room_id)
        {
            ret = 1;
            s_ezlopi_room_t *free_node = l_room_head;
            l_room_head = l_room_head->next;
            ezlopi_free(__FUNCTION__, free_node);
        }
        else
        {
            s_ezlopi_room_t *room_node = l_room_head;

            while (room_node->next)
            {
                s_ezlopi_room_t *free_node = room_node->next;
                TRACE_I("free_node->_id == room_id -> %8x == %8x", free_node->_id, room_id);

                if (free_node->_id == room_id)
                {
                    ret = 1;
                    room_node->next = free_node->next;
                    ezlopi_free(__FUNCTION__, free_node);
                    break;
                }

                room_node = room_node->next;
            }
        }
    }

    return ret;
}

static int __remove_room_from_nvs_by_id(uint32_t a_room_id)
{
    int ret = 0;
    char *rooms_str = EZPI_core_nvs_read_rooms();

    if (rooms_str)
    {
        TRACE_I("rooms_str: %s", rooms_str);
        cJSON *cj_rooms = cJSON_Parse(__FUNCTION__, rooms_str);
        ezlopi_free(__FUNCTION__, rooms_str);

        if (cj_rooms)
        {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            CJSON_TRACE("cj_rooms", cj_rooms);
#endif

            int idx = 0;
            cJSON *cj_room_tmp = NULL;

            cJSON_ArrayForEach(cj_room_tmp, cj_rooms)
            {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                CJSON_TRACE("cj_room_tmp", cj_room_tmp);
#endif

                cJSON *cj_room_tmp_id = cJSON_GetObjectItem(__FUNCTION__, cj_room_tmp, ezlopi__id_str);
                if (cj_room_tmp_id && cj_room_tmp_id->valuestring)
                {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                    CJSON_TRACE("cj_room_tmp_id", cj_room_tmp_id);
#endif

                    uint32_t tmp_room_id = strtoul(cj_room_tmp_id->valuestring, NULL, 16);
                    if (a_room_id == tmp_room_id)
                    {
                        TRACE_I("tmp_room_id: %8x", tmp_room_id);
                        cJSON_DeleteItemFromArray(__FUNCTION__, cj_rooms, idx);
                        ret = 1;
                        break;
                    }
                }

                idx++;
            }

            if (ret)
            {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                CJSON_TRACE("cj_rooms", cj_rooms);
#endif
                char *updated_rooms = cJSON_PrintBuffered(__FUNCTION__, cj_rooms, 4096, false);
                TRACE_D("length of 'updated_rooms': %d", strlen(updated_rooms));

                if (updated_rooms)
                {
                    ret = 1;
                    EZPI_core_nvs_write_rooms(updated_rooms);
                    ezlopi_free(__FUNCTION__, updated_rooms);
                }
            }

            cJSON_Delete(__FUNCTION__, cj_rooms);
        }
    }

    return ret;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
