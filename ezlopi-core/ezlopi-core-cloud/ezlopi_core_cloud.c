#include <esp_system.h>

#include "ezlopi_core_cloud.h"

#define CRC16_POLY 0x4460

const uint32_t DEVICE_ID_START = 0x10000000;
const uint32_t ITEM_ID_START = 0x20000000;
const uint32_t ROOM_ID_START = 0x30000000;
const uint32_t SETTINGS_ID_START = 0x50000000;
const uint32_t SCENE_ID_START = 0x60000000;
const uint32_t SCRIPT_ID_START = 0x70000000;
const uint32_t EXPRESSION_ID_START = 0x80000000;
const uint32_t MODES_ID_START = 0x90000000;
const uint32_t SCENE_GROUP_ID_START = 0xA0000000;

static uint32_t g_device_id = 0;
static uint32_t g_item_id = 0;
static uint32_t g_room_id = 0;
static uint32_t g_gateway_id = 0;
static uint32_t g_settings_id = 0;
static uint32_t g_scene_id = 0;
static uint32_t g_script_id = 0;
static uint32_t g_expression_id = 0;
static uint32_t g_scene_group_id = 0;

static uint32_t ezlopi_get_mac_crc(void)
{
    uint32_t crc = 0;
    uint8_t mac_base[6] = { 0 };
    esp_efuse_mac_get_default((uint8_t*)mac_base);

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

void ezlopi_cloud_update_device_id(uint32_t device_id)
{
    g_device_id = (device_id > g_device_id) ? device_id : g_device_id;
}

uint32_t ezlopi_cloud_generate_device_id(void)
{
    if (0 == g_device_id)
    {
        g_device_id = DEVICE_ID_START + ezlopi_get_mac_crc();
    }
    else
    {
        g_device_id++;
    }
    // TRACE_D("g_device_id: %u\r\n", g_device_id);
    return g_device_id;
}

uint32_t ezlopi_cloud_generate_item_id(void)
{
    if (0 == g_item_id)
    {
        g_item_id = ITEM_ID_START + ezlopi_get_mac_crc();
    }
    else
    {
        g_item_id++;
    }
    // TRACE_D("g_item_id: %u\r\n", g_item_id);

    return g_item_id;
}

uint32_t ezlopi_cloud_generate_gateway_id(void)
{
    if (0 == g_gateway_id)
    {
        g_gateway_id = 0x457a5069;
    }
    // TRACE_D("g_gateway_id: %u\r\n", g_gateway_id);
    return g_gateway_id;
}

uint32_t ezlopi_cloud_generate_settings_id(void)
{
    if (0 == g_settings_id)
    {
        g_settings_id = SETTINGS_ID_START + ezlopi_get_mac_crc();
    }
    else
    {
        g_settings_id++;
    }
    // TRACE_D("g_settings_id: %u\r\n", g_gateway_id);
    return g_settings_id;
}

void ezlopi_cloud_update_scene_id(uint32_t a_scene_id)
{
    g_scene_id = (a_scene_id > g_scene_id) ? a_scene_id : g_scene_id;
}

uint32_t ezlopi_cloud_generate_scene_id(void)
{
    g_scene_id = (0 == g_scene_id) ? (SCENE_ID_START + ezlopi_get_mac_crc()) : (g_scene_id + 1);
    return g_scene_id;
}

void ezlopi_cloud_update_script_id(uint32_t a_script_id)
{
    g_script_id = (a_script_id > g_script_id) ? a_script_id : g_script_id;
}

uint32_t ezlopi_cloud_generate_script_id(void)
{
    g_script_id = (0 == g_script_id) ? (SCRIPT_ID_START + ezlopi_get_mac_crc()) : (g_script_id + 1);
    return g_script_id;
}

void ezlopi_cloud_update_expression_id(uint32_t a_expression_id)
{
    g_expression_id = (a_expression_id > g_expression_id) ? a_expression_id : g_expression_id;
}

uint32_t ezlopi_cloud_generate_expression_id(void)
{
    g_expression_id = (0 == g_expression_id) ? (EXPRESSION_ID_START + ezlopi_get_mac_crc()) : (g_expression_id + 1);
    return g_expression_id;
}

void ezlopi_cloud_update_room_id(uint32_t a_room_id)
{
    g_room_id = (a_room_id > g_room_id) ? a_room_id : g_room_id;
}

uint32_t ezlopi_cloud_generate_room_id(void)
{
    g_room_id = (0 == g_room_id) ? (ROOM_ID_START + ezlopi_get_mac_crc()) : (g_room_id + 1);
    return g_room_id;
}

uint32_t ezlopi_cloud_generate_scene_group_id(void)
{
    g_scene_group_id = (0 == g_scene_group_id) ? (SCENE_GROUP_ID_START + ezlopi_get_mac_crc()) : (g_scene_group_id + 1);
    return g_scene_group_id;
}
