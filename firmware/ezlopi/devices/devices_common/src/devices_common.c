#include "string.h"

#include "esp_err.h"

#include "devices_common.h"
#include "nvs_storage.h"
#include "debug.h"

uint32_t device_count = 0;
static s_device_properties_t devices[MAX_DEV];
extern void dht11_service_init(uint32_t dht_pin, uint32_t dev_idx);
extern void mpu_service_init(uint8_t scl_pin, uint8_t sda_pin, uint32_t dev_idx);

const char *devices_common_get_device_type_id(void)
{
    return DEVICE_TYPE_ID;
}

s_device_properties_t *devices_common_device_list(void)
{
    return devices;
}

int devices_common_get_device_by_item_id(const char *item_id)
{
    for (int idx = 0; idx < MAX_DEV; idx++)
    {
        if (devices[idx].name[0] && item_id[0])
        {
            TRACE_D("Checking: %.*s | %s", sizeof(devices[idx].item_id), devices[idx].item_id, item_id);
            if (0 == strncmp(devices[idx].item_id, item_id, sizeof(devices[idx].item_id)))
            {
                return idx;
            }
        }
        else
        {
            break;
        }
    }

    return 0xFFFF;
}

void devices_common_init_devices(void)
{
    esp_err_t err;

    device_count = 0;
    memset(devices, 0, sizeof(s_device_properties_t) * MAX_DEV);

    // char *device_id = devices[0].device_id;
    err = nvs_storage_read_device_config((void *)devices, sizeof(s_device_properties_t) * MAX_DEV);

    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
    {
        TRACE_E("NVS get_blob ERROR: %s!\n", esp_err_to_name(err));
    }
    else
    {
        if (err == ESP_OK)
        {
            for (uint8_t i = 0; i < MAX_DEV; i++)
            {

                if (devices[i].name[0])
                {
                    device_count++;
                    devices[i].device_id[7] = 0;
                    snprintf(devices[i].roomName, sizeof(devices[i].roomName), "room_n%d", i);
                    snprintf(devices[i].roomId, sizeof(devices[i].roomId), "acd1%d11", i);

                    switch ((e_device_type_t)devices[i].dev_type)
                    {
                    // case TAMPER:
                    case LED:
                    case SWITCH:
                    case PLUG:
                    {
                        snprintf(devices[i].category, sizeof(devices[i].category), "switch");
                        snprintf(devices[i].subcategory, sizeof(devices[i].subcategory), "in_wall");
                        snprintf(devices[i].item_name, sizeof(devices[i].item_name), "switch");
                        snprintf(devices[i].devicType, sizeof(devices[i].devicType), "switch.inwall");
                        snprintf(devices[i].value_type, sizeof(devices[i].value_type), "bool");
                        devices[i].has_getter = true;
                        devices[i].has_setter = true;
                        break;
                    }
                    case TAMPER:
                    {
                        snprintf(devices[i].category, sizeof(devices[i].category), "temperature");
                        devices[i].subcategory[0] = '\0';
                        // snprintf(devices[i].subcategory, sizeof(devices[i].subcategory), "temperature");
                        snprintf(devices[i].item_name, sizeof(devices[i].item_name), "temp");
                        snprintf(devices[i].devicType, sizeof(devices[i].devicType), "sensor");
                        snprintf(devices[i].value_type, sizeof(devices[i].value_type), "temperature");
                        devices[i].has_getter = true;
                        devices[i].has_setter = false;

                        dht11_service_init(devices[i].input_gpio, i);

                        break;
                    }
                    case MPU6050:
                        
                        snprintf(devices[i].category, sizeof(devices[i].category), "level_sensor");
                        //devices[i].subcategory[0] = '\0';
                        snprintf(devices[i].subcategory, sizeof(devices[i].subcategory), "navigation");
                        snprintf(devices[i].item_name, sizeof(devices[i].item_name), "acceleration_x_axis");
                        snprintf(devices[i].devicType, sizeof(devices[i].devicType), "sensor");
                        snprintf(devices[i].value_type, sizeof(devices[i].value_type), "value");
                        devices[i].has_getter = true;
                        devices[i].has_setter = false;

                        mpu_service_init(22,23,i);
                    default:
                        break;
                    }

                    TRACE_B("############# Device Count: %d, type: %u #############", i, devices[i].dev_type);
                    TRACE_B("Name:           %.*s", sizeof(devices[i].name), devices[i].name);
                    TRACE_B("deviceId:       %.*s", sizeof(devices[i].device_id), devices[i].device_id);
                    TRACE_B("itemId:         %.*s", sizeof(devices[i].item_id), devices[i].item_id);
                    TRACE_B("itemName(type): %.*s", sizeof(devices[i].item_name), devices[i].item_name);
                    TRACE_B("roomId:         %.*s", sizeof(devices[i].roomId), devices[i].roomId);
                    TRACE_B("roomName:       %.*s", sizeof(devices[i].roomName), devices[i].roomName);
                    TRACE_B("device type:    %d", devices[i].dev_type);
                    TRACE_B("input gpio:     %d", devices[i].input_gpio);
                    TRACE_B("input inv:      %d", devices[i].input_inv);
                    TRACE_B("input vol:      %d", devices[i].input_vol);
                    TRACE_B("is input:       %d", devices[i].is_input);
                    TRACE_B("is meter:       %d", devices[i].is_meter);
                    TRACE_B("out gpio:       %d", devices[i].out_gpio);
                    TRACE_B("out inv:        %d", devices[i].out_inv);
                    TRACE_B("out vol:        %d", devices[i].out_vol);
                    TRACE_B("*****************************************");
                }
            }
        }
    }
}