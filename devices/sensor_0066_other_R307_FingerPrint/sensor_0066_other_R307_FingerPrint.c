#include "cJSON.h"
#include "trace.h"
#include "items.h"
#include "stdint.h"
#include "string.h"
#include "time.h"
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
#include "ezlopi_valueformatter.h"

#include "sensor_0066_other_R307_FingerPrint.h"
//---------------------------------------------------------------------------------------------------------------
static void IRAM_ATTR gpio_notify_isr(void *param)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)param;
    if (item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        if (false == (user_data->__busy_guard))
        {
            vTaskNotifyGiveFromISR(user_data->notifyHandler, NULL);
        }
    }
}

static int __0066_prepare(void *arg);
static int __0066_init(l_ezlopi_item_t *item);
static int __0066_set_value(l_ezlopi_item_t *item, void *arg);
static int __0066_get_value_cjson(l_ezlopi_item_t *item, void *arg);

static void Fingerprint_Operation_task(void *params);
static void uart_0066_fingerprint_upcall(uint8_t *buffer, s_ezlopi_uart_object_handle_t uart_object_handle);

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_enroll_cloud_properties(l_ezlopi_item_t *item, uint32_t item_id, cJSON *cj_device, server_packet_t *user_data);
static void __prepare_item_action_cloud_properties(l_ezlopi_item_t *item, uint32_t item_id, cJSON *cj_device, server_packet_t *user_data);
static void __prepare_item_ids_cloud_properties(l_ezlopi_item_t *item, uint32_t item_id, cJSON *cj_device, server_packet_t *user_data);
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device);

static e_sensor_fp_items_t sensor_fp_item_ids[SENSOR_FP_ITEM_ID_MAX];

//---------------------------------------------------------------------------------------------------------------
int sensor_0066_other_R307_FingerPrint(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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

//-------------------------------------------------------------------------------------------------------------------------
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *dev_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", dev_name);
    ASSIGN_DEVICE_NAME_V2(device, dev_name);
    device->cloud_properties.category = category_generic_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor; /*needs to be changed to custom*/
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}
static void __prepare_item_enroll_cloud_properties(l_ezlopi_item_t *item, uint32_t item_id, cJSON *cj_device, server_packet_t *user_data)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = false;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_learn_fingerprint; // For match
    item->cloud_properties.value_type = value_type_bool;                   // ID + %
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = item_id;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = user_data;
}
static void __prepare_item_action_cloud_properties(l_ezlopi_item_t *item, uint32_t item_id, cJSON *cj_device, server_packet_t *user_data)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_fingerprint_action; // For Enrollment
    item->cloud_properties.value_type = value_type_fingerprint_action;      // ID only
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = item_id;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = user_data;
}
static void __prepare_item_ids_cloud_properties(l_ezlopi_item_t *item, uint32_t item_id, cJSON *cj_device, server_packet_t *user_data)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_fingerprint_ids; // For List
    item->cloud_properties.value_type = value_type_array;                // Range
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = item_id;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = user_data;
}

static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    if (item && cj_device)
    {
        item->interface_type = EZLOPI_DEVICE_INTERFACE_MAX; // other
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        if (sensor_fp_item_ids[SENSOR_FP_ITEM_ID_ENROLL] == item->cloud_properties.item_id) // while initialization; setup only once (during Match item setup)
        {
            CJSON_GET_VALUE_INT(cj_device, "gpio1", item->interface.uart.tx);
            CJSON_GET_VALUE_INT(cj_device, "gpio2", item->interface.uart.rx);
            CJSON_GET_VALUE_INT(cj_device, "gpio3", user_data->intr_pin);
            item->interface.uart.baudrate = FINGERPRINT_UART_BAUDRATE;
            item->interface.uart.enable = true;
        }
        else
        {
            item->interface.uart.enable = false;
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------

static int __0066_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *dev_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    for (uint8_t i = 0; i < SENSOR_FP_ITEM_ID_MAX; i++)
    {
        sensor_fp_item_ids[i] = ezlopi_cloud_generate_item_id();
    }

    if (dev_prep_arg && (dev_prep_arg->cjson_device))
    {
        cJSON *cj_device = (dev_prep_arg->cjson_device);
        /* device-1 */
        l_ezlopi_device_t *fingerprint_device = ezlopi_device_add_device();
        if (fingerprint_device)
        {
            server_packet_t *user_data = (server_packet_t *)malloc(sizeof(server_packet_t));
            if (user_data)
            {
                memset(user_data, 0, sizeof(server_packet_t));
                __prepare_device_cloud_properties(fingerprint_device, cj_device);

                /* Preparation for uart items */
                l_ezlopi_item_t *fingerprint_item_enroll = ezlopi_device_add_item_to_device(fingerprint_device, sensor_0066_other_R307_FingerPrint);
                if (fingerprint_item_enroll)
                {
                    __prepare_item_enroll_cloud_properties(fingerprint_item_enroll, sensor_fp_item_ids[SENSOR_FP_ITEM_ID_ENROLL], cj_device, user_data);
                    __prepare_item_interface_properties(fingerprint_item_enroll, cj_device);
                }
                l_ezlopi_item_t *fingerprint_item_action = ezlopi_device_add_item_to_device(fingerprint_device, sensor_0066_other_R307_FingerPrint);
                if (fingerprint_item_action)
                {
                    __prepare_item_action_cloud_properties(fingerprint_item_action, sensor_fp_item_ids[SENSOR_FP_ITEM_ID_ACTION], cj_device, user_data);
                    __prepare_item_interface_properties(fingerprint_item_action, cj_device);
                }
                l_ezlopi_item_t *fingerprint_item_ids = ezlopi_device_add_item_to_device(fingerprint_device, sensor_0066_other_R307_FingerPrint);
                if (fingerprint_item_ids)
                {
                    __prepare_item_ids_cloud_properties(fingerprint_item_ids, sensor_fp_item_ids[SENSOR_FP_ITEM_ID_FP_IDS], cj_device, user_data);
                    __prepare_item_interface_properties(fingerprint_item_ids, cj_device);
                }

                if ((NULL == fingerprint_item_enroll) || (NULL == fingerprint_item_action) || (NULL == fingerprint_item_ids))
                {
                    ezlopi_device_free_device(fingerprint_device);
                    free(user_data);
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

static int __0066_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (NULL != item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        if ((true == (item->interface.uart.enable)) && GPIO_IS_VALID_GPIO(user_data->intr_pin) && GPIO_IS_VALID_GPIO(item->interface.uart.tx) && GPIO_IS_VALID_GPIO(item->interface.uart.rx))
        {
            gpio_num_t intr_pin = user_data->intr_pin;
            TRACE_W("tx:%d ; rx%d ; intr:%d", item->interface.uart.tx, item->interface.uart.rx, intr_pin);

            s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle = ezlopi_uart_init(item->interface.uart.baudrate, item->interface.uart.tx, item->interface.uart.rx, uart_0066_fingerprint_upcall, item);
            item->interface.uart.channel = ezlopi_uart_get_channel(ezlopi_uart_object_handle);

            const gpio_config_t FingerPrint_intr_gpio_config = {
                .pin_bit_mask = (1ULL << (intr_pin)),
                .intr_type = GPIO_INTR_NEGEDGE,
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
            };

            if (0 == gpio_config(&FingerPrint_intr_gpio_config))
            {
                if (FINGERPRINT_OK != fingerprint_config(item))
                {
                    TRACE_E("Need to Reconfigure : Fingerprint sensor ..... Please, Reset ESP32.");
                }
                else
                {
                    Update_ID_status_list(item);
                    Wait_till_system_free(item, 200);
                    if (NULL == (user_data->notifyHandler))
                    {
                        TRACE_I(" ---->>> Creating Fingerprint_activation Task <<<----");
                        xTaskCreate(Fingerprint_Operation_task, "Fingerprint_activation", 2048 * 2, item, 1, &(user_data->notifyHandler));
                    }
                    if (gpio_isr_handler_add(intr_pin, gpio_notify_isr, item))
                    {
                        TRACE_E("Error!! : Failed to add GPIO ISR handler.");
                        gpio_isr_handler_remove(intr_pin);
                    }
                    ret = 1;
                }
            }
            else
            {
                TRACE_E("Error!! : Problem is 'GPIO_intr_pin' Config......");
            }
        }
    }
    return ret;
}

static int __0066_get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    cJSON *cj_result = (cJSON *)arg;
    if (cj_result && item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        if (sensor_fp_item_ids[SENSOR_FP_ITEM_ID_ENROLL] == item->cloud_properties.item_id)
        {
            TRACE_E("op mode : %d", user_data->opmode);
            if (user_data->opmode != FINGERPRINT_ENROLLMENT_MODE)
            {
                cJSON_AddFalseToObject(cj_result, "value");
                cJSON_AddStringToObject(cj_result, "valueFormatted", "false");
            }
            else
            {
                cJSON_AddTrueToObject(cj_result, "value");
                cJSON_AddStringToObject(cj_result, "valueFormatted", "true");
            }
        }
        else if (sensor_fp_item_ids[SENSOR_FP_ITEM_ID_ACTION] == item->cloud_properties.item_id)
        {
            cJSON *cj_value = cJSON_CreateObject();

            cJSON_AddNumberToObject(cj_value, "id", user_data->matched_id);
            cJSON_AddNumberToObject(cj_value, "confidence_level", user_data->matched_confidence_level);
            cJSON_AddItemToObject(cj_result, "value", cj_value);
            cJSON_AddStringToObject(cj_result, "valueFormatted", "");
        }
        else if (sensor_fp_item_ids[SENSOR_FP_ITEM_ID_FP_IDS] == item->cloud_properties.item_id)
        {
            cJSON_AddStringToObject(cj_result, "elementType", "int");
            cJSON *cj_value_array = cJSON_AddArrayToObject(cj_result, "value");

            for (int idx = 1; idx <= FINGERPRINT_MAX_CAPACITY_LIMIT; idx++)
            {
                if (true == (user_data->validity[idx]))
                {
                    cJSON_AddItemToArray(cj_value_array, cJSON_CreateNumber(idx));
                }
            }
            cJSON_AddStringToObject(cj_result, "valueFormatted", "");
        }
        else
        {
        }
        ret = 1;
    }
    return ret;
}

static void fingerprint_enroll_timeout_check(void *pv)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)pv;

    if (item)
    {
        bool flag_break = false;
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        for (uint32_t i = 0; i < 30; i++)
        {
            TRACE_E("Here: %d", i);
            if (user_data->opmode == FINGERPRINT_MATCH_MODE)
            {
                flag_break = true;
                TRACE_E("Here");
                break;
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        if (flag_break != true)
        {
            TRACE_E("Here");
            user_data->opmode = FINGERPRINT_MATCH_MODE;
            ezlopi_device_value_updated_from_device_v3(item);
        }
    }

    vTaskDelete(NULL);
}

static int __0066_set_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    static xTaskHandle *task_handler = NULL;
    cJSON *cjson_params = (cJSON *)arg;
    if ((NULL != cjson_params) && (NULL != item))
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        TRACE_E("ptr : %p", user_data);

        char *cjson_params_str = cJSON_Print(cjson_params);
        if (cjson_params)
        {
            TRACE_D("cjson_params: %s", cjson_params_str);
            free(cjson_params_str);
        }

        if (sensor_fp_item_ids[SENSOR_FP_ITEM_ID_ENROLL] == item->cloud_properties.item_id) // Set conditon trigger enroll mode
        {
            cJSON *cj_value_cmd = cJSON_GetObjectItem(cjson_params, "value");
            if (cj_value_cmd)
            {
                TRACE_E("HERE!! enroll");
                if (cJSON_IsTrue(cj_value_cmd)) // true conditon
                {
                    if (task_handler == NULL)
                    {
                        xTaskCreate(fingerprint_enroll_timeout_check, "timeout_timer", 2048, item, 2, task_handler);
                    }

                    time(&user_data->timeout_start_time); // !< reset the internal timer_start_time
                    user_data->opmode = FINGERPRINT_ENROLLMENT_MODE;
                }
                else
                {
                    user_data->opmode = FINGERPRINT_MATCH_MODE;
                }
            }
            ezlopi_device_value_updated_from_device_v3(item);
        }
        else if (sensor_fp_item_ids[SENSOR_FP_ITEM_ID_FP_IDS] == item->cloud_properties.item_id) // Set condition erase IDs
        {
            cJSON *cj_value_ids = cJSON_GetObjectItem(cjson_params, "value");
            if ((cj_value_ids != NULL) && cJSON_IsArray(cj_value_ids))
            {
                time(&user_data->timeout_start_time); // !< reset the internal timer_start_time
                uint16_t value_array_size = cJSON_GetArraySize(cj_value_ids);
                if (value_array_size > 0)
                {
                    TRACE_W("HERE!! erase specific");
                    for (uint16_t i = 0; i < value_array_size; i++) // first protect => [2,4,5]
                    {
                        cJSON *fp_id = cJSON_GetArrayItem(cj_value_ids, i);
                        TRACE_I("Protected ID:[#%d]", (fp_id->valueint));
                        user_data->protect[fp_id->valueint] = 1; // protect this ID -> 2/4/5
                    }
                    user_data->opmode = FINGERPRINT_ERASE_WITH_IDS_MODE;
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                    xTaskNotifyGive(user_data->notifyHandler); // activate the task
                }
                else
                {
                    TRACE_W("HERE!! erase all");
                    user_data->opmode = FINGERPRINT_ERASE_ALL_MODE;
                    xTaskNotifyGive(user_data->notifyHandler); // activate the task
                }
            }
        }
    }
    return ret;
}

static void uart_0066_fingerprint_upcall(uint8_t *buffer, s_ezlopi_uart_object_handle_t uart_object_handle)
{
    char *temp_buf = (char *)malloc(256);
    if (NULL != temp_buf)
    {
        memset(temp_buf, 0, 256);

        if ((NULL != buffer) && (uart_object_handle->arg))
        {
            memcpy(temp_buf, buffer, 256);

            l_ezlopi_item_t *item = (l_ezlopi_item_t *)uart_object_handle->arg;
            server_packet_t *user_data = (server_packet_t *)item->user_arg;

            uint16_t package_len = 0;
            uint8_t another_buffer[MAX_PACKET_LENGTH_VAL] = {0};
            memcpy(another_buffer, temp_buf, MAX_PACKET_LENGTH_VAL);
            // Programmed only for ACK_operation [0x07h] with 256byte results.
            if (another_buffer[6] == FINGERPRINT_PID_ACKPACKET)
            {
                // Check the Header+Addr bytes and Then copy PID,package_len,confirmation_code and checksum to ->  recieved_packet buffer
                uint8_t idx = 0;
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

                        break;
                    }
                    case 1: // header -> ACK = 0x01h
                    {
                        if (FINGERPRINT_HEADER_LSB != (another_buffer[idx]))
                        {
                            TRACE_E("Header code mismatch....");
                            __err_message = true;
                        }
                        break;
                    }
                    case 2:
                    case 3:
                    case 4:
                    case 5: // Address -> ACK = 0xFFh
                    {
                        if (FINGERPRINT_DEVICE_ADDR_BIT != (another_buffer[idx]))
                        {
                            TRACE_E("DEV_ADDR code mismatch....");
                            __err_message = true;
                        }
                        break;
                    }
                    case 6: // PID -> ACK = 0x07h
                    {
                        if (FINGERPRINT_PID_ACKPACKET != (another_buffer[idx]))
                        {
                            TRACE_E("ACK code mismatch....");
                            __err_message = true;
                        }
                        break;
                    }
                    case 7: // P_len -> ACK = 0x__h [MSB]
                    {
                        package_len = (uint16_t)(another_buffer[idx]);
                        break;
                    }
                    case 8: // P_len -> ACK = 0x__h [LSB]
                    {
                        package_len = (package_len << 8) + (uint16_t)(another_buffer[idx] & 0xFF);
                        break;
                    }
                    default:
                        break;
                    }
                    idx++;
                }

                if (!__err_message)
                {
                    size_t _copy_size = (3 + (int)package_len);
                    memcpy((user_data->recieved_buffer), (another_buffer + 6), (_copy_size < MAX_PACKET_LENGTH_VAL) ? (_copy_size) : (MAX_PACKET_LENGTH_VAL)); // pid + p_len + confirmation + checksum
                }
                else
                {
                    memset((user_data->recieved_buffer), 0, sizeof(user_data->recieved_buffer));
                }
            }
        }
        free(temp_buf);
    }
}

static void Fingerprint_Operation_task(void *params)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)params;
    if (NULL != item)
    {
        int uart_channel_num = item->interface.uart.channel;
        static const time_t timeout_seconds = 30;
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        time(&user_data->timeout_start_time); // !< reset the internal timer_start_time

        for (;;)
        {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            time_t now = 0;
            time(&now);
            if ((now - user_data->timeout_start_time) >= timeout_seconds)
            {
                user_data->opmode = FINGERPRINT_MATCH_MODE;
                ezlopi_device_value_updated_from_device_item_id_v3(sensor_fp_item_ids[SENSOR_FP_ITEM_ID_ENROLL]);
            }

            gpio_isr_handler_remove(user_data->intr_pin);
            user_data->__busy_guard = true;

            switch (user_data->opmode)
            {
            case FINGERPRINT_MATCH_MODE:
            {
                LedControl(uart_channel_num, 0, (user_data->recieved_buffer), 200);
                uint16_t tempelate_count;
                uint16_t temp_id = user_data->user_id;
                if (ReadTempNum(item->interface.uart.channel, &tempelate_count, (user_data->recieved_buffer), 500))
                {
                    if (tempelate_count > 0)
                    {
                        for (uint16_t j = 1; j <= FINGERPRINT_MAX_CAPACITY_LIMIT; j++)
                        {
                            if (true == user_data->validity[j]) // if user_id is occupied then only perform 'match_action'
                            {
                                user_data->user_id = j;
                                if (Match_ID(item))
                                {
                                    user_data->matched_id = user_data->user_id;
                                    user_data->matched_confidence_level = (((user_data->confidence_level) > (uint16_t)100) ? 100 : (user_data->confidence_level));
                                    TRACE_B(" ---->  Matched ID: [%d] ; Confidence : [%d]", (user_data->matched_id), (user_data->matched_confidence_level));
                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    TRACE_E("No USER_ID found ; internal library is empty.... Please trigger [ENROLLMENT] mode to add fingerprint");
                }
                ezlopi_device_value_updated_from_device_item_id_v3(sensor_fp_item_ids[SENSOR_FP_ITEM_ID_ACTION]);

                user_data->user_id = temp_id;
                LedControl(uart_channel_num, 1, (user_data->recieved_buffer), 200);
                break;
            }
            case FINGERPRINT_ENROLLMENT_MODE:
            {
                LedControl(uart_channel_num, 0, (user_data->recieved_buffer), 200);

                uint16_t current_id = Find_immediate_vaccant_ID(item);

                if ((current_id) > 0)
                {
                    current_id = Enroll_Fingerprint(item);
                    if (0 != current_id)
                    {
                        if ((user_data->user_id) == current_id)
                        {
                            TRACE_B("RESULT:...Enrollment of user_id[%d].... process => Success", current_id);
                            user_data->validity[user_data->user_id] = true;

                            user_data->opmode = FINGERPRINT_MATCH_MODE;
                            ezlopi_device_value_updated_from_device_item_id_v3(sensor_fp_item_ids[SENSOR_FP_ITEM_ID_ENROLL]);
                            ezlopi_device_value_updated_from_device_item_id_v3(sensor_fp_item_ids[SENSOR_FP_ITEM_ID_FP_IDS]);
                        }
                        else
                        {
                            TRACE_B("RESULT:...Duplicate in user_id[%d]....  process => Blocked", current_id);
                        }
                    }
                }
                else
                {
                    TRACE_E("ALL user_id => occupied ; ... Delete IDs to enroll again.");
                }
                LedControl(uart_channel_num, 1, (user_data->recieved_buffer), 200);
                break;
            }

            case FINGERPRINT_ERASE_WITH_IDS_MODE:
            {
                for (uint16_t i = 1; i <= FINGERPRINT_MAX_CAPACITY_LIMIT; i++)
                {
                    if (0 == user_data->protect[i]) // first check if this ID is unprotected
                    {
                        if (true == user_data->validity[i]) // second check if the id is occupied
                        {
                            if (Delete(item->interface.uart.channel, i, 1, (user_data->recieved_buffer), 500)) // then delete and update 'validity[]' status
                            {
                                user_data->validity[user_data->user_id] = false;
                            }
                        }
                    }
                    else // Remove protection for this ID
                    {
                        user_data->protect[i] = 0; // unprotect here
                    }
                }
#if 0
                // NOW : UNPROTECT all after, delete phase
                // for (uint16_t i = 1; i <= FINGERPRINT_MAX_CAPACITY_LIMIT; i++)
                // {
                //     user_data->protect[i] = 0;
                // }
#endif
                if (sensor_fp_item_ids[SENSOR_FP_ITEM_ID_FP_IDS] == item->cloud_properties.item_id)
                {
                    ezlopi_device_value_updated_from_device_v3(item);
                }
                user_data->opmode = FINGERPRINT_MATCH_MODE;
                break;
            }
            case FINGERPRINT_ERASE_ALL_MODE:
            {
                if (Erase_all_ID(item))
                {
                    for (uint16_t i = 1; i <= FINGERPRINT_MAX_CAPACITY_LIMIT; i++)
                    {
                        user_data->validity[i] = false;
                    }
                    if (sensor_fp_item_ids[SENSOR_FP_ITEM_ID_FP_IDS] == item->cloud_properties.item_id)
                    {
                        ezlopi_device_value_updated_from_device_v3(item);
                    }
                }
                user_data->opmode = FINGERPRINT_MATCH_MODE;
                break;
            }

            default:
            {
                TRACE_E("Invalid OPMODE is set..... {%d}. Reverting Back to default: 'MATCH_MODE-[0]'", user_data->opmode);
                user_data->opmode = FINGERPRINT_MATCH_MODE;
                break;
            }
            }

            user_data->__busy_guard = false;
            gpio_isr_handler_add(user_data->intr_pin, gpio_notify_isr, item);

            TRACE_B("           >> Remove finger  &  Wait => [1sec] ; To activate next Task_notify<<");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
}
