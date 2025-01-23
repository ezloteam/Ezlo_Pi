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
 * @file    ezlopi_core_cloud.c
 * @brief   perform some function on core-cloud properties
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Nabin Dangi
 *          Riken Maharjan
 * @version 0.1
 * @date    October 31st, 2023 6:55 PM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <esp_system.h>

#include "ezlopi_core_cloud.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
#define CRC16_POLY 0x4460

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static uint32_t ezlopi_get_mac_crc(void);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static uint32_t g_device_id = 0;
static uint32_t g_item_id = 0;
static uint32_t g_room_id = 0;
static uint32_t g_gateway_id = 0;
static uint32_t g_settings_id = 0;
static uint32_t g_scene_id = 0;
static uint32_t g_script_id = 0;
static uint32_t g_expression_id = 0;
static uint32_t g_scene_group_id = 0;
static uint32_t g_scene_when_blockId = 0;
static uint32_t g_device_group_id = 0;
static uint32_t g_item_group_id = 0;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/
const uint32_t DEVICE_ID_START = 0x10000000;
const uint32_t ITEM_ID_START = 0x20000000;
const uint32_t ROOM_ID_START = 0x30000000;
const uint32_t SETTINGS_ID_START = 0x50000000;
const uint32_t SCENE_ID_START = 0x60000000;
const uint32_t SCRIPT_ID_START = 0x70000000;
const uint32_t EXPRESSION_ID_START = 0x80000000;
const uint32_t MODES_ID_START = 0x90000000;
const uint32_t SCENE_GROUP_ID_START = 0xA0000000;
const uint32_t SCENE_WHEN_BLOCKID_START = 0xB0000000;
const uint32_t DEVICE_GROUPID_START = 0xC0000000;
const uint32_t ITEM_GROUPID_START = 0xD0000000;

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_core_cloud_update_device_id(uint32_t device_id)
{
    g_device_id = (device_id > g_device_id) ? device_id : g_device_id;
}

uint32_t EZPI_core_cloud_generate_device_id(void)
{
    if (0 == g_device_id)
    {
        g_device_id = DEVICE_ID_START + ezlopi_get_mac_crc();
    }
    else
    {
        g_device_id++;
    }
    // TRACE_D("g_device_id: %u", g_device_id);
    return g_device_id;
}

uint32_t EZPI_core_cloud_generate_item_id(void)
{
    if (0 == g_item_id)
    {
        g_item_id = ITEM_ID_START + ezlopi_get_mac_crc();
    }
    else
    {
        g_item_id++;
    }
    // TRACE_D("g_item_id: %u", g_item_id);

    return g_item_id;
}

uint32_t EZPI_core_cloud_generate_gateway_id(void)
{
    if (0 == g_gateway_id)
    {
        g_gateway_id = 0x457a5069;
    }
    // TRACE_D("g_gateway_id: %u", g_gateway_id);
    return g_gateway_id;
}

uint32_t EZPI_core_cloud_generate_settings_id(void)
{
    if (0 == g_settings_id)
    {
        g_settings_id = SETTINGS_ID_START + ezlopi_get_mac_crc();
    }
    else
    {
        g_settings_id++;
    }
    // TRACE_D("g_settings_id: %u", g_gateway_id);
    return g_settings_id;
}

void EZPI_core_cloud_update_scene_id(uint32_t a_scene_id)
{
    g_scene_id = (a_scene_id > g_scene_id) ? a_scene_id : g_scene_id;
}

uint32_t EZPI_core_cloud_generate_scene_id(void)
{
    g_scene_id = (0 == g_scene_id) ? (SCENE_ID_START + ezlopi_get_mac_crc()) : (g_scene_id + 1);
    return g_scene_id;
}

void EZPI_core_cloud_update_when_blockId(uint32_t a_when_blockId)
{
    g_scene_when_blockId = (a_when_blockId > g_scene_when_blockId) ? a_when_blockId : g_scene_when_blockId;
}

uint32_t EZPI_core_cloud_generate_scene_when_blockId(void)
{
    g_scene_when_blockId = (0 == g_scene_when_blockId) ? (SCENE_WHEN_BLOCKID_START + ezlopi_get_mac_crc()) : (g_scene_when_blockId + 1);
    return g_scene_when_blockId;
}

void EZPI_core_cloud_update_script_id(uint32_t a_script_id)
{
    g_script_id = (a_script_id > g_script_id) ? a_script_id : g_script_id;
}

uint32_t EZPI_core_cloud_generate_script_id(void)
{
    g_script_id = (0 == g_script_id) ? (SCRIPT_ID_START + ezlopi_get_mac_crc()) : (g_script_id + 1);
    return g_script_id;
}

void EZPI_core_cloud_update_expression_id(uint32_t a_expression_id)
{
    g_expression_id = (a_expression_id > g_expression_id) ? a_expression_id : g_expression_id;
}

uint32_t EZPI_core_cloud_generate_expression_id(void)
{
    g_expression_id = (0 == g_expression_id) ? (EXPRESSION_ID_START + ezlopi_get_mac_crc()) : (g_expression_id + 1);
    return g_expression_id;
}

void EZPI_core_cloud_update_room_id(uint32_t a_room_id)
{
    g_room_id = (a_room_id > g_room_id) ? a_room_id : g_room_id;
}

uint32_t EZPI_core_cloud_generate_room_id(void)
{
    g_room_id = (0 == g_room_id) ? (ROOM_ID_START + ezlopi_get_mac_crc()) : (g_room_id + 1);
    return g_room_id;
}

uint32_t EZPI_core_cloud_generate_scene_group_id(void)
{
    g_scene_group_id = (0 == g_scene_group_id) ? (SCENE_GROUP_ID_START + ezlopi_get_mac_crc()) : (g_scene_group_id + 1);
    return g_scene_group_id;
}

void EZPI_core_cloud_update_group_id(uint32_t a_device_id)
{
    g_scene_group_id = (a_device_id > g_scene_group_id) ? a_device_id : g_scene_group_id;
}

uint32_t EZPI_core_cloud_generate_device_group_id(void)
{
    g_device_group_id = (0 == g_device_group_id) ? (DEVICE_GROUPID_START + ezlopi_get_mac_crc()) : (g_device_group_id + 1);
    return g_device_group_id;
}

void EZPI_core_cloud_update_device_group_id(uint32_t a_device_group_id)
{
    g_device_group_id = (a_device_group_id > g_device_group_id) ? a_device_group_id : g_device_group_id;
}

uint32_t EZPI_core_cloud_generate_item_group_id(void)
{
    g_item_group_id = (0 == g_item_group_id) ? (ITEM_GROUPID_START + ezlopi_get_mac_crc()) : (g_item_group_id + 1);
    return g_item_group_id;
}

void EZPI_core_cloud_update_item_group_id(uint32_t a_item_group_id)
{
    g_item_group_id = (a_item_group_id > g_item_group_id) ? a_item_group_id : g_item_group_id;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
static uint32_t ezlopi_get_mac_crc(void)
{
    uint32_t crc = 0;
    uint8_t mac_base[6] = {0};
    esp_efuse_mac_get_default((uint8_t *)mac_base);

    // Perform CRC calculation on each byte of the MAC address
    for (int i = 0; i < 6; i++)
    {
        crc ^= ((uint16_t)mac_base[i]) << 8;

        for (int j = 0; j < 8; j++)
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ CRC16_POLY;
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return (crc << 8) & 0x00FFFF00;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
