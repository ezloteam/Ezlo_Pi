#include <esp_system.h>
#include "ezlopi_cloud.h"

#define CRC16_POLY 0x4460

static const uint32_t DEVICE_ID_START = 0x10000000;
static const uint32_t ITEM_ID_START = 0x20000000;
static const uint32_t ROOM_ID_START = 0x30000000;
static const uint32_t SETTINGS_ID_START = 0x50000000;
static const uint32_t SCENE_ID_START = 0x60000000;
static const uint32_t SCRIPT_ID_START = 0x70000000;

static uint64_t device_id = 0;
static uint64_t item_id = 0;
static uint64_t room_id = 0;
static uint64_t gateway_id = 0;
static uint64_t settings_id = 0;
static uint32_t scene_id = 0;
static uint32_t script_id = 0;

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
                crc = (crc << 1) ^ CRC16_POLY;
            else
                crc <<= 1;
        }
    }

    return (crc << 8) & 0x00FFFF00;
}

uint32_t ezlopi_cloud_generate_device_id(void)
{
    if (0 == device_id)
    {
        device_id = DEVICE_ID_START + ezlopi_get_mac_crc();
    }
    else
    {
        device_id++;
    }
    // TRACE_D("device_id: %u\r\n", device_id);
    return device_id;
}

uint32_t ezlopi_cloud_generate_item_id(void)
{
    if (0 == item_id)
    {
        item_id = ITEM_ID_START + ezlopi_get_mac_crc();
    }
    else
    {
        item_id++;
    }
    // TRACE_D("item_id: %u\r\n", item_id);

    return item_id;
}

uint32_t ezlopi_cloud_generate_room_id(void)
{
    if (0 == room_id)
    {
        room_id = ROOM_ID_START + ezlopi_get_mac_crc();
    }
    else
    {
        room_id++;
    }
    // TRACE_D("room_id: %u\r\n", room_id);
    return room_id;
}

uint32_t ezlopi_cloud_generate_gateway_id(void)
{
    if (0 == gateway_id)
    {
        gateway_id = 0x457a5069;
    }
    // TRACE_D("gateway_id: %u\r\n", gateway_id);
    return gateway_id;
}

uint32_t ezlopi_cloud_generate_settings_id(void)
{
    if (0 == settings_id)
    {
        settings_id = SETTINGS_ID_START + ezlopi_get_mac_crc();
    }
    else
    {
        settings_id++;
    }
    // TRACE_D("settings_id: %u\r\n", gateway_id);
    return settings_id;
}

void ezlopi_cloud_update_scene_id(uint32_t a_scene_id)
{
    scene_id = (a_scene_id > scene_id) ? a_scene_id : scene_id;
}
uint32_t ezlopi_cloud_generate_scene_id(void)
{
    scene_id = (0 == scene_id) ? (SCENE_ID_START + ezlopi_get_mac_crc()) : (scene_id + 1);
    return scene_id;
}

void ezlopi_cloud_update_script_id(uint32_t a_script_id)
{
    script_id = (a_script_id > script_id) ? a_script_id : script_id;
}
uint32_t ezlopi_cloud_generate_script_id(void)
{
    script_id = (0 == script_id) ? (SCRIPT_ID_START + ezlopi_get_mac_crc()) : (script_id + 1);
    return script_id;
}