#include "cJSON.h"
#include "trace.h"
#include "items.h"
#include "stdint.h"
#include "string.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
#include "gpio_isr_service.h"
#include "esp_err.h"
#include "esp_types.h"

#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "ezlopi_uart.h"
#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_scales_str.h"

#include "sensor_0066_UART_R307_FingerPrint.h"
//---------------------------------------------------------------------------------------------------------------

static int __0066_prepare(void *arg);
static int __0066_init(l_ezlopi_item_t *item);
static int __0066_set_value(l_ezlopi_item_t *item, void *arg);
static int __0066_get_value_cjson(l_ezlopi_item_t *item, void *arg);

static void Mode_Change_Callback_Task(void *params);
static void Fingerprint_Operation_task(void *params);
static void sensor_touch_callback(l_ezlopi_item_t *item);
static void uart_0066_fingerprint_upcall(uint8_t *buffer, s_ezlopi_uart_object_handle_t uart_object_handle);

static void __prepare_device_uart_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item1_match_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, server_packet_t *uart_arg);
static void __prepare_item2_enroll_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, server_packet_t *uart_arg);
static void __prepare_item3_list_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, server_packet_t *uart_arg);
static void __prepare_item4_erase_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, server_packet_t *uart_arg);
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device);
//---------------------------------------------------------------------------------------------------------------
int sensor_0066_uart_r307_fingerprint(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0066_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0066_init(item);
        break;
    }
    case EZLOPI_ACTION_SET_VALUE:
    {
        ret = __0066_set_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0066_get_value_cjson(item, arg);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}
//---------------------------------------------------------------------------------------------------------------
static void __prepare_device_uart_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *dev_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", dev_name);
    ASSIGN_DEVICE_NAME_V2(device, dev_name);
    device->cloud_properties.category = category_generic_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor; /*needs to be changed to custom*/
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}
static void __prepare_item1_match_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, server_packet_t *uart_arg)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_sound_select; // For match
    item->cloud_properties.value_type = value_type_array;             // ID + %
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = uart_arg;
}
static void __prepare_item2_enroll_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, server_packet_t *uart_arg)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_sound_volume; // For Enrollment
    item->cloud_properties.value_type = value_type_int;               // ID only
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = uart_arg;
}
static void __prepare_item3_list_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, server_packet_t *uart_arg)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_humidity; // For List
    item->cloud_properties.value_type = value_type_array;         // Range
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = uart_arg;
}
static void __prepare_item4_erase_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, server_packet_t *uart_arg)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_switch; // For erase_with_ID
    item->cloud_properties.value_type = value_type_bool;        // Range
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = uart_arg;
}
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    if (item && cj_device)
    {
        item->interface_type = EZLOPI_DEVICE_INTERFACE_MAX; // other
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        CJSON_GET_VALUE_INT(cj_device, "gpio1", item->interface.uart.tx);
        CJSON_GET_VALUE_INT(cj_device, "gpio2", item->interface.uart.rx);
        CJSON_GET_VALUE_INT(cj_device, "gpio3", user_data->fp_interface.intr_pin);
        item->interface.uart.baudrate = FINGERPRINT_UART_BAUDRATE;

        if (ezlopi_item_name_sound_select == item->cloud_properties.item_name) // while initialization; setup only once (during Match item setup)
        {
            user_data->fp_interface.fp_enable = true;
        }
        else
        {
            user_data->fp_interface.fp_enable = false;
        }
    }
}
//---------------------------------------------------------------------------------------------------------
static int __0066_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *dev_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (dev_prep_arg && (dev_prep_arg->cjson_device))
    {
        cJSON *cj_device = (dev_prep_arg->cjson_device);
        /* device-1 */
        l_ezlopi_device_t *fingerprint_device = ezlopi_device_add_device();
        if (fingerprint_device)
        {
            server_packet_t *uart_arg = (server_packet_t *)malloc(sizeof(server_packet_t));
            if (uart_arg)
            {
                memset(uart_arg, 0, sizeof(server_packet_t));
                __prepare_device_uart_cloud_properties(fingerprint_device, cj_device);

                /* Preparation for uart items */
                l_ezlopi_item_t *fingerprint_item1 = ezlopi_device_add_item_to_device(fingerprint_device, sensor_0066_uart_r307_fingerprint);
                if (fingerprint_item1)
                {
                    __prepare_item1_match_cloud_properties(fingerprint_item1, cj_device, uart_arg);
                    __prepare_item_interface_properties(fingerprint_item1, cj_device);
                }
                l_ezlopi_item_t *fingerprint_item2 = ezlopi_device_add_item_to_device(fingerprint_device, sensor_0066_uart_r307_fingerprint);
                if (fingerprint_item2)
                {
                    __prepare_item2_enroll_cloud_properties(fingerprint_item2, cj_device, uart_arg);
                    __prepare_item_interface_properties(fingerprint_item2, cj_device);
                }
                l_ezlopi_item_t *fingerprint_item3 = ezlopi_device_add_item_to_device(fingerprint_device, sensor_0066_uart_r307_fingerprint);
                if (fingerprint_item3)
                {
                    __prepare_item3_list_cloud_properties(fingerprint_item3, cj_device, uart_arg);
                    __prepare_item_interface_properties(fingerprint_item3, cj_device);
                }
                l_ezlopi_item_t *fingerprint_item4 = ezlopi_device_add_item_to_device(fingerprint_device, sensor_0066_uart_r307_fingerprint);
                if (fingerprint_item4)
                {
                    __prepare_item4_erase_cloud_properties(fingerprint_item4, cj_device, uart_arg);
                    __prepare_item_interface_properties(fingerprint_item4, cj_device);
                }

                if ((NULL == fingerprint_item1) || (NULL == fingerprint_item2) || (NULL == fingerprint_item3) || (NULL == fingerprint_item4))
                {
                    ezlopi_device_free_device(fingerprint_device);
                    free(uart_arg);
                }
            }
            else
            {
                ezlopi_device_free_device(fingerprint_device);
            }
        }
    }
    return ret;
}
//----------------------------------------------------------------------------------------------------------------------
static int __0066_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (NULL != item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        if (ezlopi_item_name_sound_select == item->cloud_properties.item_name)
        {
            if ((true == user_data->fp_interface.fp_enable) && GPIO_IS_VALID_GPIO(user_data->fp_interface.intr_pin) && GPIO_IS_VALID_GPIO(item->interface.uart.tx) && GPIO_IS_VALID_GPIO(item->interface.uart.rx))
            {
                s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle = ezlopi_uart_init(item->interface.uart.baudrate, item->interface.uart.tx, item->interface.uart.rx, uart_0066_fingerprint_upcall, item);
                item->interface.uart.channel = ezlopi_uart_get_channel(ezlopi_uart_object_handle);

                /* This is most crutial part ;*/
                if (FINGERPRINT_OK != fingerprint_config(item))
                {
                    TRACE_E("Need to reconfigure the fingerprint sensor ..... ");
                }
                else
                {
                    TRACE_W(" ......UART Initailization complete......");
                }
                // setting up the GPIO_intr
                const gpio_config_t FingerPrint_config = {
                    .pin_bit_mask = (1ULL << (user_data->fp_interface.intr_pin)),
                    .mode = GPIO_MODE_INPUT,
                    .pull_up_en = GPIO_PULLUP_ENABLE,
                    .pull_down_en = GPIO_PULLDOWN_DISABLE,
                    .intr_type = GPIO_INTR_NEGEDGE,
                };
                if (0 == gpio_config(&FingerPrint_config))
                {
                    TRACE_W(" ......Fingerprint Setup complete......");
                    // Now create a task to do some work,according to the opmode
                    xTaskCreate(Fingerprint_Operation_task, "Fingerprint_activation", 2048, item, 1, &(user_data->fingerprint_receiverHandler));
                    // Now set a intr callback that triggers above 'Fingerprint_Operation_task', (according to the opmode)
                    gpio_isr_service_register_v3(item, (void *)sensor_touch_callback, 200);
                    ret = 1;
                }
                else
                {
                    TRACE_W(" ......ERROR: Problem is 'GPIO_intr_pin' setup......");
                }
            }
        }
    }
    return ret;
}
//----------------------------------------------------------------------------------------------------------------------
static int __0066_get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    cJSON *cj_result = (cJSON *)arg;
    if (cj_result && item)
    {
        if (ezlopi_item_name_sound_select == item->cloud_properties.item_name) // Match
        {
            // char valueFormatted[5];
            // server_packet_t *user_data = (server_packet_t *)item->user_arg;
            // snprintf(valueFormatted, 6, "%d", (int)(user_data->user_id));
            // cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            // cJSON_AddBoolToObject(cj_result, "value", (int)(user_data->user_id));
        }
        if (ezlopi_item_name_sound_volume == item->cloud_properties.item_name) // enroll
        {
            // char valueFormatted[5];
            // server_packet_t *user_data = (server_packet_t *)item->user_arg;
            // snprintf(valueFormatted, 6, "%d", (int)(user_data->confidence_level));
            // cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            // cJSON_AddBoolToObject(cj_result, "value", (int)(user_data->confidence_level));
        }
        if (ezlopi_item_name_humidity == item->cloud_properties.item_name) // list
        {
            // char valueFormatted[5];
            // server_packet_t *user_data = (server_packet_t *)item->user_arg;
            // snprintf(valueFormatted, 6, "%d", (int)(user_data->confidence_level));
            // cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            // cJSON_AddBoolToObject(cj_result, "value", (int)(user_data->confidence_level));
        }
        if (ezlopi_item_name_switch == item->cloud_properties.item_name) // erase_with_ID
        {
            // char valueFormatted[5];
            // server_packet_t *user_data = (server_packet_t *)item->user_arg;
            // snprintf(valueFormatted, 6, "%d", (int)(user_data->confidence_level));
            // cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            // cJSON_AddBoolToObject(cj_result, "value", (int)(user_data->confidence_level));
        }
        ret = 1;
    }
    return ret;
}

static int __0066_set_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    cJSON *cjson_params = (cJSON *)arg;
    if ((NULL != cjson_params) && (NULL != item))
    {
        // Mode changing process
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        char *cjson_params_str = cJSON_Print(cjson_params);
        if (cjson_params)
        {
            TRACE_D("cjson_params: %s", cjson_params_str);
            free(cjson_params_str);
        }
        e_FINGERPRINT_OP_MODE_t FingerPrint_mode = user_data->opmode; // Dummy variable required, To compare with previous state
        cJSON *cj_value = cJSON_GetObjectItem(cjson_params, "value"); // what type? array or just number
        if (cj_value)
        {
            switch (cj_value->type)
            {
            case cJSON_Number:
            {
                FingerPrint_mode = cj_value->valueint;
                break;
            }
            case cJSON_Array: /* This is used in mode with ranged inputs like : Erase_with_ID*/
            {                 /*Parse array type cjson_object input*/
                for (uint8_t idx = 0; idx < (cJSON_GetArraySize(cj_value)); idx++)
                {
                    cJSON *res_obj = cJSON_GetArrayItem(cj_value, idx);
                    if ((NULL != res_obj) && (cJSON_Number == (res_obj->type)))
                    {
                        if (0 == idx) /*Mode*/
                        {
                            FingerPrint_mode = cj_value->valueint;
                        }
                        if (1 == idx) /*start_id or user_id*/
                        {
                            user_data->user_id = (((cj_value->valueint) <= FINGERPRINT_MAX_CAPACITY_LIMIT) ? (((cj_value->valueint) > 0) ? (cj_value->valueint) : (FINGERPRINT_STARTING_USER_PAGE_ID))
                                                                                                           : FINGERPRINT_MAX_CAPACITY_LIMIT);
                        }
                        if (2 == idx) /*ID_count(N)*/
                        {
                            user_data->id_counts = ((cj_value->valueint) <= FINGERPRINT_MAX_CAPACITY_LIMIT) ? (cj_value->valueint) : FINGERPRINT_MAX_CAPACITY_LIMIT;
                        }
                    }
                    else
                    {
                        TRACE_W("idx => [%d].... Parse Fail", idx);
                    }
                }

                break;
            }
            default:
                TRACE_W("Setting Fingerprint module into default mode [ 0 => MATCH MODE ]");
                break;
            }
        }
        // First checking the Validity of newly triggered mode [from UI]
        if ((user_data->opmode != FingerPrint_mode) && (FingerPrint_mode < FINGERPRINT_MODE_MAX))
        {
            // change the opmode to new required mode.
            user_data->opmode = FingerPrint_mode;
            // Just check in which item it gets triggered
            TRACE_I("item_name: %s; .... item_id: 0x%08x", item->cloud_properties.item_name, item->cloud_properties.item_id);
            TRACE_I("......Fingerprint Mode: Prev[%d] -> New[%d]", user_data->opmode, FingerPrint_mode);

            // Start The TIMER // Using task for flexibility //
            BaseType_t xReturned = xTaskCreate(Mode_Change_Callback_Task, "opmode Changing Task", 2048, item, 2, NULL);
            if (pdFALSE == xReturned)
            { // The task will delete itself (after 30 sec)!! Unlike timer ; its more convinient in this case.
                TRACE_I("Failed to Create timer.... Reverting to default [MATCH MODE]......");
            }
        }
        else
        {
            TRACE_I("......Fingerprint Mode: Prev[%d] -> New[%d] ", user_data->opmode, FingerPrint_mode);
        }
        ret = 1;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------------------------
static void uart_0066_fingerprint_upcall(uint8_t *buffer, s_ezlopi_uart_object_handle_t uart_object_handle)
{
    TRACE_E("UART_Buffer => %u", buffer);
    if (buffer && (uart_object_handle->arg))
    {
        l_ezlopi_item_t *item = (l_ezlopi_item_t *)uart_object_handle->arg;
        server_packet_t *user_data = (server_packet_t *)item->user_arg;

        uint8_t another_buffer[MAX_PACKET_LENGTH_VAL] = {0};
        memcpy(another_buffer, buffer, MAX_PACKET_LENGTH_VAL);

        TRACE_E("----------- ANOTHER_BUFFER -------------");
        for (int i = 0; i < (sizeof(another_buffer)); i++) //?
        {
            TRACE_I("RECIEVED_ANOTHER_BUFFER  => [%d] %#x", i, another_buffer[i]);
        }
        // programmed only for ACK_operation [0x07h] with 256byte results.
        if (another_buffer[6] == FINGERPRINT_PID_ACKPACKET)
        {
            TRACE_I(".... Extracting the bytes starting from [PID] : %#x ....", another_buffer[6]);
            // Check the Header+Addr bytes and Then copy PID,package_len,confirmation_code and checksum to ->  recieved_packet buffer
            uint8_t idx = 0;
            uint16_t package_len = 0;
            bool __err_message = false;
            while ((!__err_message) && (idx < 9))
            {
                switch (idx)
                {
                case 0: // header -> ACK = 0xEFh
                {
                    if (FINGERPRINT_HEADER_MSB != (another_buffer[idx]))
                    {
                        TRACE_E("Header code mismatch....");
                        __err_message = true;
                    }
                    TRACE_E("%d:- %#x", idx, (another_buffer[idx]));

                    break;
                }
                case 1: // header -> ACK = 0x01h
                {
                    if (FINGERPRINT_HEADER_LSB != (another_buffer[idx]))
                    {
                        TRACE_E("Header code mismatch....");
                        __err_message = true;
                    }
                    TRACE_E("%d:- %#x", idx, (another_buffer[idx]));
                    break;
                }
                case 2:
                case 3:
                case 4:
                case 5: // Address -> ACK = 0xFFh
                {
                    if (FINGERPRINT_DEVICE_ADDR_BIT == (another_buffer[idx]))
                    {
                        TRACE_E("Header code mismatch....");
                        __err_message = true;
                    }
                    TRACE_E("%d:- %#x", idx, (another_buffer[idx]));
                    break;
                }
                case 6: // PID -> ACK = 0x07h
                    TRACE_E("%d:- %#x", idx, (another_buffer[idx]));
                    break;
                case 7: // P_len -> ACK = 0x__h [MSB]
                {
                    // MSB [first]
                    package_len = (uint16_t)(another_buffer[idx]);
                    package_len = (package_len << 8);
                    TRACE_E("%d:- %#x", idx, (another_buffer[idx]));
                    break;
                }
                case 8: // P_len -> ACK = 0x__h [LSB]
                {
                    // LSB [last]
                    package_len |= (uint16_t)(another_buffer[idx] & 0xFF);
                    TRACE_E("%d:- %#x", idx, (another_buffer[idx]));
                    break;
                }
                default:
                    break;
                }
                idx++;
            }

            if (!__err_message)
            {
                TRACE_E("Package_length :- %#x", package_len);
                size_t _copy_size = (3 + (int)package_len);
                memcpy((user_data->recieved_buffer), (another_buffer + 6), (_copy_size < MAX_PACKET_LENGTH_VAL) ? (_copy_size) : (MAX_PACKET_LENGTH_VAL)); // pid + p_len + confirmation + checksum
                // TRACE_I("_BUFFER_TO_BE_PARSED_ [size : %d] ", sizeof(user_data->recieved_buffer));                                                         //?
            }
            else
            {
                memset((user_data->recieved_buffer), 0, sizeof(user_data->recieved_buffer));
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
static void sensor_touch_callback(l_ezlopi_item_t *item)
{
    // generate the notification
    if (NULL != item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        if (false == (user_data->__busy_guard))
        {
            xTaskNotifyGive(user_data->fingerprint_receiverHandler);
        }
    }
}
//--------------------------- Fingerprint_Operation_task -----------------------------------------------------------------
/**
 * @brief  This Task, waits for notifications and acts according to current set operating mode.
 */
static void Fingerprint_Operation_task(void *params)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)params;
    if (NULL != item)
    {
        uint16_t current_id;
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        for (;;)
        {
            // wait for a notification
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

            // set the guard_flag to stop incomming notifications
            user_data->__busy_guard = true;

            // do processing according to set mode in global varible
            switch (user_data->opmode)
            {
            case FINGERPRINT_MATCH_MODE:
                // Buffer variable to indicate in which page/user_id, (only used in match and enroll)
                current_id = user_data->user_id;
                if (Match_ID(item))
                {
                    // update all tiles
                    //  ezlopi_device_value_updated_from_device_v3(item); // sends one item
                    //  ezlopi_device_value_updated_from_device_v3(item); // goes for next item
                    //  ezlopi_device_value_updated_from_device_v3(item); // goes for next item
                }
                break;
            case FINGERPRINT_ENROLLMENT_MODE:
                // Buffer variable to indicate in which page/user_id, (only used in match and enroll)
                current_id = user_data->user_id;
                // Checking if requested ID is empty (or not_valid) ; Then call a function to enroll if that PAGE_ID/user_id is empty
                if (true == Check_PAGEID_Empty(item))
                {
                    current_id = Enroll_Fingerprint(item); /*This return id of currently stored 'USER/PAGE ID' ; If a duplicate fingerprint is already present, then : this returns ID of duplicate instead of new */
                    if (0 != current_id)
                    {
                        if ((user_data->user_id) == current_id)
                        {
                            TRACE_B("RESULT:...Enrollment of user_id[%d].... process => Success", current_id);
                            // update all tiles
                            // ezlopi_device_value_updated_from_device_v3(item); // sends one item
                            // ezlopi_device_value_updated_from_device_v3(item); // goes for next item
                            // ezlopi_device_value_updated_from_device_v3(item); // goes for next item
                        }
                        else
                        {
                            TRACE_B("RESULT:...Duplicate in user_id[%d]....  process => Blocked", current_id);
                        }
                    }
                }
                else
                {
                    TRACE_B("ERROR : user_id[%d] ... occupied .., Try again with different ID", user_data->user_id);
                }
                break;
            case FINGERPRINT_LIST_IDS_MODE:
                Update_ID_status_list(item);
                // update all tiles
                //  ezlopi_device_value_updated_from_device_v3(item); // sends one item
                //  ezlopi_device_value_updated_from_device_v3(item); // goes for next item
                //  ezlopi_device_value_updated_from_device_v3(item); // goes for next item
                break;
            case FINGERPRINT_ERASE_WITH_IDS_MODE:
                if (Erase_Specified_ID(item))
                {
                    // update all tiles
                    //  ezlopi_device_value_updated_from_device_v3(item); // sends one item
                    //  ezlopi_device_value_updated_from_device_v3(item); // goes for next item
                    //  ezlopi_device_value_updated_from_device_v3(item); // goes for next item
                }
                break;
            case FINGERPRINT_ERASE_ALL_MODE:
                if (Erase_all_ID(item))
                {
                    // update all tiles
                    //  ezlopi_device_value_updated_from_device_v3(item); // sends one item
                    //  ezlopi_device_value_updated_from_device_v3(item); // goes for next item
                    //  ezlopi_device_value_updated_from_device_v3(item); // goes for next item
                }
                break;
            default:
                TRACE_E("Invalid fingerprint operation mode is set..... {%d}", user_data->opmode);
                break;
            }

            // Have we got stable/appropriate results?
            //

            vTaskDelay(100 / portTICK_PERIOD_MS);
            user_data->__busy_guard = false;
        }
    }
    vTaskDelete(NULL);
}

//----------------------------------- Mode_Change_Callback_Task to reset the opmode -----------------------------------------------
/**
 * @brief This task is invoked to count for 30seconds and Revert opmode back to Default (MATCH mode).
 */
static void Mode_Change_Callback_Task(void *params)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)params;
    if (NULL != item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        for (uint8_t count = 0; count <= 30; count = count + 2)
        {
            /* Task code goes here. */
            TRACE_I("time : %d sec", count);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        TRACE_I("Reverting from [%d] to the default mode [0 => MATCH_MODE]....", user_data->opmode);
        user_data->opmode = FINGERPRINT_MATCH_MODE; // this affects opmode value for all functions
    }
    vTaskDelete(NULL);
}
