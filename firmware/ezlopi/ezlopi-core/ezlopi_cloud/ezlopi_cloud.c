#include "ezlopi_cloud.h"

static uint32_t device_id = 0;
static uint32_t item_id = 0;
static uint32_t room_id = 0;
static uint32_t gateway_id = 0;

uint32_t ezlopi_cloud_generate_device_id(void)
{
    device_id = (0 == device_id) ? 0x30000001 : device_id + 1;
    // TRACE_D("device_id: %u\r\n", device_id);
    return device_id;
}

uint32_t ezlopi_cloud_generate_item_id(void)
{
    item_id = (0 == item_id) ? 0x20000001 : item_id + 1;
    // TRACE_D("item_id: %u\r\n", item_id);
    return item_id;
}

uint32_t ezlopi_cloud_generate_room_id(void)
{
    room_id = (0 == room_id) ? 0x10000001 : room_id + 1;
    // TRACE_D("room_id: %u\r\n", room_id);
    return room_id;
}

uint32_t ezlopi_cloud_generate_gateway_id(void)
{
    gateway_id = (0 == gateway_id) ? 0x40000001 : gateway_id + 1;
    // TRACE_D("gateway_id: %u\r\n", gateway_id);
    return gateway_id;
}