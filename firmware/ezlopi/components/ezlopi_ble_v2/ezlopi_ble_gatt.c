#include "trace.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_config.h"

static void ezlopi_ble_gatt_append_characterstic_to_service(s_gatt_service_t *service_obj, s_gatt_char_t *character_object);
static void ezlopi_ble_gatt_service_append_to_head(s_gatt_service_t *service_obj);

static char *ezlopi_ble_gatt_event_to_string(esp_gatts_cb_event_t event);

static s_gatt_service_t *gatt_head_service = NULL;

s_gatt_service_t *ezlopi_ble_gatt_create_service(esp_bt_uuid_t *service_uuid)
{
    s_gatt_service_t *service_obj = malloc(sizeof(s_gatt_service_t));
    if (service_obj)
    {
        memset(service_obj, 0, sizeof(s_gatt_service_t));
        ezlopi_ble_gatt_service_append_to_head(service_obj);
        service_obj->num_handles = 1;
        service_obj->service_id.id.inst_id = 0x00;
        service_obj->service_id.is_primary = true;
        service_obj->service_id.id.uuid.len = ESP_UUID_LEN_128;
        memcpy(service_obj->service_id.id.uuid.uuid.uuid128, service_uuid->uuid.uuid128, ESP_UUID_LEN_128);
    }

    return service_obj;
}

s_gatt_char_t *ezlopi_ble_gatt_add_characteristic(s_gatt_service_t *service_obj, esp_bt_uuid_t *uuid, esp_gatt_perm_t permission, esp_gatt_char_prop_t properties)
{
    s_gatt_char_t *character_object = NULL;
    if (service_obj)
    {
        character_object = malloc(sizeof(s_gatt_char_t));
        if (character_object)
        {
            memset(character_object, 0, sizeof(s_gatt_char_t));
            // character_object->uuid.uuid.uuid128
            ezlopi_ble_gatt_append_characterstic_to_service(service_obj, character_object);
        }
    }

    return character_object;
}

s_gatt_descr_t *ezlopi_ble_gatt_add_descriptor(s_gatt_char_t *charcteristic, esp_bt_uuid_t uuid, )
{
    return NULL;
}

void ezlopi_ble_gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    TRACE_I("BLE GATT Event: %s", ezlopi_ble_gatt_event_to_string(event));

#if 1
    /* If event is register event, store the gatts_if for each profile */

    if (ESP_GATTS_REG_EVT == event)
    {
        if (param->reg.status == ESP_GATT_OK)
        {
            // gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        }
        else
        {
            TRACE_W("Reg app failed, app_id %04x, status %d", param->reg.app_id, param->reg.status);
            return;
        }
    }

    /* If the gatts_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    // do
    // {
    //     int idx;
    //     for (idx = 0; idx < sizeof(gl_profile_tab) / sizeof(gatts_profile_inst_t); idx++)
    //     {
    //         if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
    //             gatts_if == gl_profile_tab[idx].gatts_if)
    //         {
    //             if (gl_profile_tab[idx].gatts_cb)
    //             {
    //                 gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
    //             }
    //         }
    //     }
    // } while (0);
#endif
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

static char *ezlopi_ble_gatt_event_to_string(esp_gatts_cb_event_t event)
{
    char *ret = "GATT event not defined!";

    switch (event)
    {
    case ESP_GATTS_REG_EVT:
    {
        ret = "ESP_GATTS_REG_EVT";
        break;
    }
    case ESP_GATTS_READ_EVT:
    {
        ret = "ESP_GATTS_READ_EVT";
        break;
    }
    case ESP_GATTS_WRITE_EVT:
    {
        ret = "ESP_GATTS_WRITE_EVT";
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
    {
        ret = "ESP_GATTS_EXEC_WRITE_EVT";
        break;
    }
    case ESP_GATTS_MTU_EVT:
    {
        ret = "ESP_GATTS_MTU_EVT";
        break;
    }
    case ESP_GATTS_CONF_EVT:
    {
        ret = "ESP_GATTS_CONF_EVT";
        break;
    }
    case ESP_GATTS_UNREG_EVT:
    {
        ret = "ESP_GATTS_UNREG_EVT";
        break;
    }
    case ESP_GATTS_CREATE_EVT:
    {
        ret = "ESP_GATTS_CREATE_EVT";
        break;
    }
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
    {
        ret = "ESP_GATTS_ADD_INCL_SRVC_EVT";
        break;
    }
    case ESP_GATTS_ADD_CHAR_EVT:
    {
        ret = "ESP_GATTS_ADD_CHAR_EVT";
        break;
    }
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
    {
        ret = "ESP_GATTS_ADD_CHAR_DESCR_EVT";
        break;
    }
    case ESP_GATTS_DELETE_EVT:
    {
        ret = "ESP_GATTS_DELETE_EVT";
        break;
    }
    case ESP_GATTS_START_EVT:
    {
        ret = "ESP_GATTS_START_EVT";
        break;
    }
    case ESP_GATTS_STOP_EVT:
    {
        ret = "ESP_GATTS_STOP_EVT";
        break;
    }
    case ESP_GATTS_CONNECT_EVT:
    {
        ret = "ESP_GATTS_CONNECT_EVT";
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT:
    {
        ret = "ESP_GATTS_DISCONNECT_EVT";
        break;
    }
    case ESP_GATTS_OPEN_EVT:
    {
        ret = "ESP_GATTS_OPEN_EVT";
        break;
    }
    case ESP_GATTS_CANCEL_OPEN_EVT:
    {
        ret = "ESP_GATTS_CANCEL_OPEN_EVT";
        break;
    }
    case ESP_GATTS_CLOSE_EVT:
    {
        ret = "ESP_GATTS_CLOSE_EVT";
        break;
    }
    case ESP_GATTS_LISTEN_EVT:
    {
        ret = "ESP_GATTS_LISTEN_EVT";
        break;
    }
    case ESP_GATTS_CONGEST_EVT:
    {
        ret = "ESP_GATTS_CONGEST_EVT";
        break;
    }
    case ESP_GATTS_RESPONSE_EVT:
    {
        ret = "ESP_GATTS_RESPONSE_EVT";
        break;
    }
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
    {
        ret = "ESP_GATTS_CREAT_ATTR_TAB_EVT";
        break;
    }
    case ESP_GATTS_SET_ATTR_VAL_EVT:
    {
        ret = "ESP_GATTS_SET_ATTR_VAL_EVT";
        break;
    }
    case ESP_GATTS_SEND_SERVICE_CHANGE_EVT:
    {
        ret = "ESP_GATTS_SEND_SERVICE_CHANGE_EVT";
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static void ezlopi_ble_gatt_event_register_func(void)
{
}

static void ezlopi_ble_gatt_service_append_to_head(s_gatt_service_t *service_obj)
{
    if (gatt_head_service)
    {
        s_gatt_service_t *cur_service = gatt_head_service;
        while (cur_service->next)
        {
            cur_service->next = cur_service->next->next;
        }

        cur_service->next = service_obj;
    }
    else
    {
        gatt_head_service = service_obj;
    }
}