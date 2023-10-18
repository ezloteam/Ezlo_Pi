#include "ezlopi_cloud.h"

static const uint32_t ROOM_ID_START = 0x10000001;
static const uint32_t ITEM_ID_START = 0x20000001;
static const uint32_t DEVICE_ID_START = 0x30000001;
static const uint32_t GATEWAY_ID_START = 0x40000001;
static const uint32_t SCENE_ID_START = 0x50000001;
static const uint32_t SCRIPT_ID_START = 0x60000001;

static uint32_t device_id = 0;
static uint32_t item_id = 0;
static uint32_t room_id = 0;
static uint32_t gateway_id = 0;
static uint32_t scene_id = 0;
static uint32_t script_id = 0;

uint32_t ezlopi_cloud_get_gateway_id(void)
{
    if (0 == gateway_id)
    {
        ezlopi_cloud_generate_gateway_id();
    }

    return gateway_id;
}

void ezlopi_cloud_update_room_id(uint32_t a_room_id)
{
    room_id = (a_room_id > room_id) ? a_room_id : room_id;
}
uint32_t ezlopi_cloud_generate_room_id(void)
{
    room_id = (0 == room_id) ? ROOM_ID_START : room_id + 1;
    // TRACE_D("room_id: %u\r\n", room_id);
    return room_id;
}

void ezlopi_cloud_update_item_id(uint32_t a_item_id)
{
    item_id = (a_item_id > item_id) ? a_item_id : item_id;
}
uint32_t ezlopi_cloud_generate_item_id(void)
{
    item_id = (0 == item_id) ? ITEM_ID_START : item_id + 1;
    // TRACE_D("item_id: %u\r\n", item_id);
    return item_id;
}

void ezlopi_cloud_update_device_id(uint32_t a_device_id)
{
    device_id = (a_device_id > device_id) ? a_device_id : device_id;
}
uint32_t ezlopi_cloud_generate_device_id(void)
{
    device_id = (0 == device_id) ? DEVICE_ID_START : device_id + 1;
    // TRACE_D("device_id: %u\r\n", device_id);
    return device_id;
}

void ezlopi_cloud_update_gateway_id(uint32_t a_gateway_id)
{
    gateway_id = (a_gateway_id > gateway_id) ? a_gateway_id : gateway_id;
}
uint32_t ezlopi_cloud_generate_gateway_id(void)
{
    gateway_id = (0 == gateway_id) ? GATEWAY_ID_START : gateway_id + 1;
    // TRACE_D("gateway_id: %u\r\n", gateway_id);
    return gateway_id;
}

void ezlopi_cloud_update_scene_id(uint32_t a_scene_id)
{
    scene_id = (a_scene_id > scene_id) ? a_scene_id : scene_id;
}
uint32_t ezlopi_cloud_generate_scene_id(void)
{
    scene_id = (0 == scene_id) ? SCENE_ID_START : scene_id + 1;
    return scene_id;
}

void ezlopi_cloud_update_script_id(uint32_t a_script_id)
{
    script_id = (a_script_id > script_id) ? a_script_id : script_id;
}
uint32_t ezlopi_cloud_generate_script_id(void)
{
    script_id = (0 == script_id) ? SCRIPT_ID_START : script_id + 1;
    return script_id;
}