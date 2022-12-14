# Ezlo_Pi Guide
## Using ezlopi_ble:

***Refer to the example in folder 'services/ezlopi_ble_service' for detialed use case.***
1. **Dependencies**:
    - ezlopi-core
        - ezlopi_ble
            - ezlopi_ble_buffer.h
            - ezlopi_ble_gap.h
            - ezlopi_ble_gatt.h
            - ezlopi_ble_profile.h
            - ezlopi_ble_config.h
    - components
        - trace.h    
2. **Create a ble service:**
    1. Create service uuid 
        ```c
        esp_bt_uuid_t uuid = {.len = ESP_UUID_LEN_16, .uuid.uuid16 = 0x00E3};
        ```
    2. Create Unique BLE Service Handle
        ```c
        #define XYZ_SERVICE_HANDLE 1
        ```
    3. Create service
        ```c
        s_gatt_service_t *service = ezlopi_ble_gatt_create_service(BLE_USER_ID_SERVICE_HANDLE, &uuid);
        ```
3. **Create a Characteristics:**
    1. Create uuid:
        ```c
        esp_bt_uuid_t uuid = {.len = ESP_UUID_LEN_16, .uuid.uuid16 = 0xE301};
        ```
    2. Create permission:   
        ```c
        esp_gatt_perm_t permission = ESP_GATT_PERM_WRITE | ESP_GATT_PERM_READ;
        ```
    3. Create properties: 
        ```c
        esp_gatt_char_prop_t properties = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ;
        ```
    4. Define read function: 
        ```c
        static void read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
        {
            if (value)
            {
                static char *user_id;
                if (NULL == user_id)
                {
                    ezlopi_nvs_read_user_id_str(&user_id);
                }

                if (NULL != user_id)
                {
                    uint32_t total_data_len = strlen(user_id);
                    uint32_t max_data_buffer_size = ezlopi_ble_gatt_get_max_data_size();
                    uint32_t copy_size = ((total_data_len - param->read.offset) < max_data_buffer_size) ? (total_data_len - param->read.offset) : max_data_buffer_size;
                    if ((0 != total_data_len) && (total_data_len > param->read.offset))
                    {
                        strncpy((char *)value->value, user_id + param->read.offset, copy_size);
                        value->len = copy_size;
                    }
                    else
                    {
                        value->len = 1;
                        value->value[0] = 0;
                    }

                    if ((param->read.offset + copy_size) >= total_data_len)
                    {
                        free(user_id);
                        user_id = NULL;
                    }
                }
                else
                {
                    value->len = 1;
                    value->value[0] = 0;
                }
            }
        }
        ```
    5. Declare an static linked_buffer:
    <br> This variable will be used in write function only when data are in chunks.
        ```c
        static s_linked_buffer_t *user_id_linked_buffer;
        ```
    6. Define write function: 
    <br> This function is called when ble write characteristcs value is written from the client
        ```c
        static void write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
        {
            if (0 == param->write.is_prep) // Data received in single packet
            {
                char tmp_buffer[37];
                strncpy(tmp_buffer, (char *)param->write.value, (36 < param->write.len) ? 36 : param->write.len);
                ezlopi_nvs_write_user_id_str(tmp_buffer);
            }
            else // data received in chunks
            {
                if (NULL == user_id_linked_buffer)
                {
                    user_id_linked_buffer = ezlopi_ble_buffer_create(param);
                }
                else
                {
                    ezlopi_ble_buffer_add_to_buffer(user_id_linked_buffer, param);
                }
            }
        }
        ```
    7. Define write execute function:
    <br> This function will be called when data receiving in chunks (write fuction) are complete.
        ```c
        static void write_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
        {
            ezlopi_ble_buffer_accumulate_to_start(user_id_linked_buffer);

            ezlopi_ble_buffer_free_buffer(user_id_linked_buffer);
            user_id_linked_buffer = NULL;
        }
        ```
    8. Add Characteristics to the Service created in step 1.:
        ```c
        s_gatt_char_t * characterisc = ezlopi_ble_gatt_add_characteristic(service, &uuid, permission, properties, read_func, write_func, write_exec_func);
        ```
4. **Create Descriptor**
    <br> This process is similar as step 2. (i.e. Create Characteristics), except one step. Descriptor does not require the argument properties.
5. **Initialize BLE stack**
    ```c
    char ble_device_name[32];
    snprintf(ble_device_name, sizeof(ble_device_name), "ezlopi_100004005");

    static esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    CHECK_PRINT_ERROR(esp_bt_controller_init(&bt_cfg), "initialize controller failed");
    CHECK_PRINT_ERROR(esp_bt_controller_enable(ESP_BT_MODE_BLE), "enable controller failed");
    CHECK_PRINT_ERROR(esp_bluedroid_init(), "init bluetooth failed");
    CHECK_PRINT_ERROR(esp_bluedroid_enable(), "enable bluetooth failed");
    CHECK_PRINT_ERROR(esp_ble_gatts_register_callback(ezlopi_ble_gatts_event_handler), "gatts register error, error code");
    CHECK_PRINT_ERROR(esp_ble_gap_register_callback(ezlopi_ble_gap_event_handler), "gap register error");
    CHECK_PRINT_ERROR(esp_ble_gap_set_device_name(ble_device_name), "Set device name failed!");
    ```
6. **Register Service**
    <br> 'XYZ_SERVICE_HANDLE' is used from section 1.2
    ```c
    esp_ble_gatts_app_register(XYZ_SERVICE_HANDLE);
    ```
7. **Initiate Security features**
    ```c
    const uint32_t default_passkey = 123456;
    uint32_t passkey;
    ezlopi_nvs_read_ble_passkey(&passkey);
    passkey = (0 == passkey) ? default_passkey : passkey;
    passkey = (passkey > 999999) ? default_passkey : passkey;
    TRACE_D("Ble passkey: %d", passkey);

    const esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND; // ESP_LE_AUTH_REQ_BOND_MITM;
    const esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;
    const uint8_t key_size = 16;
    const uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
    const uint8_t oob_support = ESP_BLE_OOB_ENABLE; // ESP_BLE_OOB_DISABLE;
    const uint8_t init_key = (ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    const uint8_t rsp_key = (ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t)), "failed -set - ESP_BLE_SM_SET_STATIC_PASSKEY");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_AUTHEN_REQ_MODE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_IOCAP_MODE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_MAX_KEY_SIZE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_OOB_SUPPORT");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_SET_INIT_KEY");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_SET_RSP_KEY");

    ```




## Current Scenario of BLE onboarding
```c
|--------------------------------------------------|
| Service: 0x00FF (Wi-Fi Credentials)              |
|   |----------------------------------------------|
|   | - Characteristics: 0xFF01                    |
|   | - Permission: Read, write                    |
|   | - Value: {"SSID":"ssid","PSD":"password"}    |
|   |----------------------------------------------|
|--------------------------------------------------|
| Service: 0x00EE (Wi-Fi Status)                   |
|   |----------------------------------------------|
|   | - Characteristic: 0xEE01                     |
|   | - Permission: Read, Notify                   |
|   | - Value: 0 (Not connected), 1 (Connected)    |
|   |----------------------------------------------|
|--------------------------------------------------|
| Service: 0xE100 (Wi-Fi Error)                    |
|   |----------------------------------------------|
|   | - Characteristic: 0xE101                     |
|   | - Permission: Read, Notify                   |
|   | - Value: (Error message in string)           |
|   |----------------------------------------------|
|--------------------------------------------------|
| Service: 0xE200 (BLE Passkey)                    |
|   |----------------------------------------------|
|   | - Characteristic: 0xE101                     |
|   | - Permission: Write                          |
|   | - Value: (unsigned integer < 999999)         |
|   |----------------------------------------------|
|--------------------------------------------------|
| Service: 0xE300 (User-ID)                        |
|   |----------------------------------------------|
|   | - Characteristic: 0xE301                     |
|   | - Permission: Read, Write                    |
|   | - Value: (16 byte UUID)                      |
|   |----------------------------------------------|
|--------------------------------------------------|
``` 



