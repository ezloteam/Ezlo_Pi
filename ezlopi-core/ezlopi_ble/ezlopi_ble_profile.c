#include <string.h>
#include "trace.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_config.h"
#include "ezlopi_ble_profile.h"

static s_gatt_service_t *gatt_head_service = NULL;

static void ezlopi_ble_gatt_append_descriptor_to_characteristic(s_gatt_char_t *characteristic, s_gatt_descr_t *descriptor);
static void ezlopi_ble_gatt_append_characterstic_to_service(s_gatt_service_t *service_obj, s_gatt_char_t *character_object);
static void ezlopi_ble_gatt_service_append_to_head(s_gatt_service_t *service_obj);
static s_gatt_service_t *ezlopi_ble_gatt_search_service_by_characteristic(s_gatt_char_t *characteristic);
static char *ezlopi_ble_gatt_event_to_string(esp_gatts_cb_event_t event);
s_gatt_service_t *ezlopi_ble_profile_get_head(void)
{
    return gatt_head_service;
}

s_gatt_descr_t *ezlopi_ble_profile_get_initiating_descriptor(s_gatt_char_t *characteristic)
{
    s_gatt_descr_t *curr_desc = NULL;

    if (characteristic)
    {
        curr_desc = characteristic->descriptor;
        while (curr_desc)
        {
            if (GATT_STATUS_PROCESSING == curr_desc->status)
            {
                break;
            }
            curr_desc = curr_desc->next;
        }
    }

    return curr_desc;
}

s_gatt_descr_t *ezlopi_ble_profile_get_descriptor_to_init(s_gatt_char_t *characteristic)
{
    s_gatt_descr_t *curr_desc = NULL;
    if (characteristic)
    {
        curr_desc = characteristic->descriptor;
        while (curr_desc)
        {
            if (0 == curr_desc->handle)
            {
                break;
            }
            curr_desc = curr_desc->next;
        }
    }
    return curr_desc;
}

s_gatt_char_t *ezlopi_ble_profile_get_initiating_characterstics(s_gatt_service_t *service)
{
    s_gatt_char_t *curr_char = NULL;

    if (service)
    {
        curr_char = service->characteristics;
        while (curr_char)
        {
            if (GATT_STATUS_PROCESSING == curr_char->status)
            {
                break;
            }
            curr_char = curr_char->next;
        }
    }

    return curr_char;
}

s_gatt_char_t *ezlopi_ble_profile_get_characterstics_to_init(s_gatt_service_t *service)
{
    s_gatt_char_t *curr_char = NULL;

    if (service)
    {
        curr_char = service->characteristics;
        while (curr_char)
        {
            if (0 == curr_char->handle)
            {
                break;
            }
            curr_char = curr_char->next;
        }
    }

    return curr_char;
}

s_gatt_service_t *ezlopi_ble_profile_get_by_app_id(uint16_t app_id)
{
    s_gatt_service_t *curr_head = gatt_head_service;

    while (curr_head)
    {
        if (app_id == curr_head->app_id)
        {
            break;
        }
        curr_head = curr_head->next;
    }
    return curr_head;
}

s_gatt_service_t *ezlopi_ble_profile_get_service_by_gatts_if(uint16_t gatts_if)
{
    s_gatt_service_t *curr_head = gatt_head_service;

    while (curr_head)
    {
        if (gatts_if == curr_head->gatts_if)
        {
            break;
        }
        curr_head = curr_head->next;
    }
    return curr_head;
}

int ezlopi_ble_gatt_number_of_services(void)
{
    int count = 0;
    s_gatt_service_t *curr_head = gatt_head_service;

    while (curr_head)
    {
        count++;
        curr_head = curr_head->next;
    }
    return count;
}

s_gatt_service_t *ezlopi_ble_gatt_create_service(uint16_t app_id, esp_bt_uuid_t *service_uuid)
{
    s_gatt_service_t *service_obj = malloc(sizeof(s_gatt_service_t));
    if (service_obj)
    {
        memset(service_obj, 0, sizeof(s_gatt_service_t));
        service_obj->app_id = app_id;
        service_obj->num_handles = 1;
        service_obj->service_id.id.inst_id = 0x00;
        service_obj->service_id.is_primary = true;
        memcpy(&service_obj->service_id.id.uuid, service_uuid, sizeof(esp_bt_uuid_t));
        ezlopi_ble_gatt_service_append_to_head(service_obj);
    }
    else
    {
        TRACE_E("Failed to create gatt-service!");
    }

    return service_obj;
}

s_gatt_char_t *ezlopi_ble_gatt_add_characteristic(s_gatt_service_t *service_obj, esp_bt_uuid_t *uuid, esp_gatt_perm_t permission, esp_gatt_char_prop_t properties,
                                                  f_upcall_t read_func, f_upcall_t write_func, f_upcall_t write_exec_func)
{
    s_gatt_char_t *character_object = NULL;
    if (service_obj)
    {
        character_object = malloc(sizeof(s_gatt_char_t));
        if (character_object)
        {
            memset(character_object, 0, sizeof(s_gatt_char_t));
            character_object->control.auto_rsp = ESP_GATT_AUTO_RSP;
            character_object->property = properties;
            character_object->permission = permission;
            character_object->read_upcall = read_func;
            character_object->write_upcall = write_func;
            character_object->write_exce_upcall = write_exec_func;
            memcpy(&character_object->uuid, uuid, sizeof(esp_bt_uuid_t));
            ezlopi_ble_gatt_append_characterstic_to_service(service_obj, character_object);
            service_obj->num_handles += 2;
        }
        else
        {
            TRACE_E("Failed to create gatt-characteristic!");
        }
    }

    return character_object;
}

s_gatt_descr_t *ezlopi_ble_gatt_add_descriptor(s_gatt_char_t *charcteristic, esp_bt_uuid_t *uuid, esp_gatt_perm_t permission,
                                               f_upcall_t read_func, f_upcall_t write_func, f_upcall_t write_exec_func)
{
    s_gatt_descr_t *descriptor_obj = NULL;

    if (charcteristic)
    {
        descriptor_obj = malloc(sizeof(s_gatt_descr_t));

        if (descriptor_obj)
        {
            memset(descriptor_obj, 0, sizeof(s_gatt_descr_t));
            descriptor_obj->read_upcall = read_func;
            descriptor_obj->write_upcall = write_func;
            descriptor_obj->write_exce_upcall = write_exec_func;

            if (NULL == uuid)
            {
                descriptor_obj->uuid.len = ESP_UUID_LEN_16;
                descriptor_obj->uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_DESCRIPTION;
            }
            else
            {
                memcpy(&descriptor_obj->uuid, uuid, sizeof(esp_bt_uuid_t));
            }

            descriptor_obj->permission = permission;
            ezlopi_ble_gatt_append_descriptor_to_characteristic(charcteristic, descriptor_obj);
            s_gatt_service_t *cur_service = ezlopi_ble_gatt_search_service_by_characteristic(charcteristic);
            if (cur_service)
            {
                cur_service->num_handles += 1;
            }
        }
        else
        {
            TRACE_E("Failed to create gatt-descriptor!");
        }
    }

    return descriptor_obj;
}

static s_gatt_service_t *ezlopi_ble_gatt_search_service_by_characteristic(s_gatt_char_t *characteristic)
{
    if (gatt_head_service)
    {
        s_gatt_service_t *curr_service = gatt_head_service;

        while (curr_service)
        {
            s_gatt_char_t *cur_char = curr_service->characteristics;

            while (cur_char)
            {
                if (cur_char == characteristic)
                {
                    return curr_service;
                }

                cur_char = cur_char->next;
            }

            curr_service = curr_service->next;
        }
    }

    return NULL;
}

static void ezlopi_ble_gatt_append_characterstic_to_service(s_gatt_service_t *service_obj, s_gatt_char_t *character_object)
{
    if (NULL == service_obj->characteristics)
    {
        service_obj->characteristics = character_object;
    }
    else
    {
        s_gatt_char_t *cur_char = service_obj->characteristics;
        while (cur_char->next)
        {
            cur_char = cur_char->next;
        }

        cur_char->next = character_object;
    }
}

static void ezlopi_ble_gatt_append_descriptor_to_characteristic(s_gatt_char_t *characteristic, s_gatt_descr_t *descriptor)
{
    if (characteristic)
    {
        if (NULL == characteristic->descriptor)
        {
            characteristic->descriptor = descriptor;
        }
        else
        {
            s_gatt_descr_t *cur_descriptor = characteristic->descriptor;

            while (cur_descriptor->next)
            {
                cur_descriptor = cur_descriptor->next;
            }

            cur_descriptor->next = descriptor;
        }
    }
}

static void ezlopi_ble_gatt_service_append_to_head(s_gatt_service_t *service_obj)
{
    if (gatt_head_service)
    {
        s_gatt_service_t *cur_service = gatt_head_service;
        while (cur_service->next)
        {
            cur_service = cur_service->next;
        }

        cur_service->next = service_obj;
    }
    else
    {
        gatt_head_service = service_obj;
    }
}

// static void ezlopi_ble_gatt_print_service(s_gatt_service_t *service);
// static void ezlopi_ble_gatt_print_characteristic(s_gatt_char_t *characteristic);
// static void ezlopi_ble_gatt_print_descriptor(s_gatt_char_t *descriptor);
// static void ezlopi_ble_gatt_print_uuid(esp_bt_uuid_t *uuid, char *msg);

void ezlopi_ble_profile_print(void)
{
    TRACE_I("BLE PROFILES: ");

    s_gatt_service_t *cur_service = gatt_head_service;

    while (cur_service)
    {
        ezlopi_ble_gatt_print_service(cur_service);

        s_gatt_char_t *cur_character = cur_service->characteristics;
        while (cur_character)
        {
            ezlopi_ble_gatt_print_characteristic(cur_character);

            s_gatt_descr_t *cur_descriptor = cur_character->descriptor;
            while (cur_descriptor)
            {
                ezlopi_ble_gatt_print_descriptor(cur_descriptor);
                TRACE_B("|    |    |----------------------------------------------------");
                cur_descriptor = cur_descriptor->next;
            }

            TRACE_B("|    |---------------------------------------------------------");
            cur_character = cur_character->next;
        }

        TRACE_B("---------------------------------------------------------------");
        cur_service = cur_service->next;
    }
}

void ezlopi_ble_gatt_print_descriptor(s_gatt_descr_t *descriptor)
{
    if (descriptor)
    {
        TRACE_B("|    |    |--------------Descriptor----------------------------");
        ezlopi_ble_gatt_print_uuid(&descriptor->uuid, "|    |    |-");
        TRACE_B("|    |    |- handle: %d", descriptor->handle);
        TRACE_B("|    |    |- permission: 0x%02x", descriptor->permission);
        TRACE_B("|    |    |- status: 0x%02x", descriptor->status);
    }
}

void ezlopi_ble_gatt_print_characteristic(s_gatt_char_t *characteristic)
{
    if (characteristic)
    {
        TRACE_B("|    |--------------------Characteristic-----------------------");
        ezlopi_ble_gatt_print_uuid(&characteristic->uuid, "|    |-");
        TRACE_B("|    |- handle: %d", characteristic->handle);
        TRACE_B("|    |- permission: 0x%02x", characteristic->permission);
        TRACE_B("|    |- property: 0x%02x", characteristic->property);
        TRACE_B("|    |- status: 0x%02x", characteristic->status);
        // TRACE_B("|    |- read_upcall: 0x%02x", (uint32_t)characteristic->read_upcall);
        // TRACE_B("|    |- write_upcall: 0x%02x", (uint32_t)characteristic->write_upcall);
        // TRACE_B("|    |- write_exce_upcall: 0x%02x", (uint32_t)characteristic->write_exce_upcall);
    }
}

void ezlopi_ble_gatt_print_service(s_gatt_service_t *service)
{
    TRACE_B("--------------------------Service-------------------------------");
    ezlopi_ble_gatt_print_uuid(&service->service_id.id.uuid, "|-");
    TRACE_B("|- app-id: %d", service->app_id);
    TRACE_B("|- conn-id: %d", service->conn_id);
    TRACE_B("|- gatts-if: %d", service->gatts_if);
    TRACE_B("|- num handles: %d", service->num_handles);
    TRACE_B("|- service handle: %d", service->service_handle);
    TRACE_B("|- status: %d", service->status);
}

void ezlopi_ble_gatt_print_uuid(esp_bt_uuid_t *uuid, char *msg)
{
    msg = msg ? msg : "";

    if (uuid)
    {
        if (ESP_UUID_LEN_16 == uuid->len)
        {
            TRACE_B("%s uuid: 0x%04X", msg, uuid->uuid.uuid16);
        }
        else if (ESP_UUID_LEN_32 == uuid->len)
        {
            TRACE_B("%s uuid: 0x%08X", msg, uuid->uuid.uuid32);
        }
        else
        {
            TRACE_B("%s uuid: %02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X", msg,
                    uuid->uuid.uuid128[15], uuid->uuid.uuid128[14], uuid->uuid.uuid128[13], uuid->uuid.uuid128[12],
                    uuid->uuid.uuid128[11], uuid->uuid.uuid128[10],
                    uuid->uuid.uuid128[9], uuid->uuid.uuid128[8],
                    uuid->uuid.uuid128[7], uuid->uuid.uuid128[6],
                    uuid->uuid.uuid128[5], uuid->uuid.uuid128[4], uuid->uuid.uuid128[3], uuid->uuid.uuid128[2], uuid->uuid.uuid128[1], uuid->uuid.uuid128[0]);
        }
    }
}
