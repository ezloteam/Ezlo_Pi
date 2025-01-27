/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
 * @file    ezlopi_core_room.h
 * @brief   perform some function on rooms
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 1.0
 * @date    January 5th, 2024 7:09 PM
 */

#ifndef _EZLOPI_CORE_ROOM_H_
#define _EZLOPI_CORE_ROOM_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cjext.h"

#include "ezlopi_core_errors.h"

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/
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
        struct s_ezlopi_room *next;
    } s_ezlopi_room_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief Function to get room head
     *
     * @return s_ezlopi_room_t*
     */
    s_ezlopi_room_t *EZPI_core_room_get_room_head(void);
    /**
     * @brief Function to get room_name by id
     *
     * @param room_id
     * @return char*
     */
    char *EZPI_core_room_get_name_by_id(uint32_t room_id);
    /**
     * @brief Function to initialize room
     *
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_room_init(void);
    /**
     * @brief Function to delete room
     *
     * @param cj_room target room object
     * @return int
     */
    int EZPI_core_room_delete(cJSON *cj_room);
    /**
     * @brief Function to delete all rooms
     *
     * @return int
     */
    int EZPI_core_room_delete_all(void);
    /**
     * @brief Function to set room name
     *
     * @param cj_room object containing room name
     * @return int
     */
    int EZPI_core_room_name_set(cJSON *cj_room);
    /**
     * @brief Function to add nvs
     *
     * @param cj_room room-obj to add
     * @return int
     */
    int EZPI_core_room_add_to_nvs(cJSON *cj_room);
    /**
     * @brief Function to reorder room
     *
     * @param cj_rooms_ids cjson room ids
     * @return int
     */
    int EZPI_core_room_reorder(cJSON *cj_rooms_ids);
    /**
     * @brief Function to add room to list
     *
     * @param cj_room
     * @return s_ezlopi_room_t*
     */
    s_ezlopi_room_t *EZPI_core_room_add_to_list(cJSON *cj_room);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_ROOM_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
