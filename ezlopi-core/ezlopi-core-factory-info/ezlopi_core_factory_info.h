#ifndef _EZLOPI_CORE_FACTORY_INFO_H_
#define _EZLOPI_CORE_FACTORY_INFO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "EZLOPI_USER_CONFIG.h"
#include "esp_partition.h"


#define EZLOPI_DEVICE_TYPE_TEST_DEVICE -1
#define EZLOPI_DEVICE_TYPE_GENERIC 0


#ifdef EZPI_DEV_TYPE_GENERIC
#define EZLOPI_DEVICE_TYPE EZLOPI_DEVICE_TYPE_GENERIC
#else
#define EZLOPI_DEVICE_TYPE EZLOPI_DEVICE_TYPE_TEST_DEVICE
#endif 

#define EZLOPI_FACTORY_INFO_V3_PARTITION_NAME "prov_data"
#define EZLOPI_FACTORY_INFO_V3_PARTITION_SIZE 0x8000 // 32KB
#define EZLOPI_FACTORY_INFO_V3_PARTITION_TYPE 0x40
#define EZLOPI_FACTORY_INFO_V3_SUBTYPE ESP_PARTITION_SUBTYPE_APP_FACTORY // ESP_PARTITION_SUBTYPE_ANY

#define EZLOPI_FINFO_OFFSET_CONN_DATA 0x0000
#define EZLOPI_FINFO_OFFSET_HUB_DATA 0x6000

#define EZLOPI_FINFO_READ_LEN_4KB 4096
    typedef enum e_factory_info_v3_partition_type
    {
        E_EZLOPI_FACTORY_INFO_CONN_DATA,
        E_EZLOPI_FACTORY_INFO_HUB_DATA,
        E_EZLOPI_FACTORY_INFO_MAX
    } e_factory_info_v3_partition_type_t;

    typedef enum e_ezlopi_factory_info_v3_offset
    {
        // Offset Connection Data
        EZLOPI_FINFO_REL_OFFSET_CONFIG_VERSION = 0x02,
        EZLOPI_FINFO_REL_OFFSET_PROVISIONING_UUID = 0x0004,
        EZLOPI_FINFO_REL_OFFSET_DEVICE_UUID = 0x0082,
        EZLOPI_FINFO_REL_OFFSET_PROVISIONING_SERVER_URL = 0X100,
        EZLOPI_FINFO_REL_OFFSET_PROVISIONING_TOKEN = 0X200,
        EZLOPI_FINFO_REL_OFFSET_CLOUD_SERVER_URL = 0x0400,
        EZLOPI_FINFO_REL_OFFSET_EZLOPI_CONFIG_JSON = 0x1000,
        EZLOPI_FINFO_REL_OFFSET_CA_CERTIFICATE = 0x3000,
        EZLOPI_FINFO_REL_OFFSET_SSL_PRIVATE_KEY = 0x4000,
        EZLOPI_FINFO_REL_OFFSET_SSL_SHARED_KEY = 0x5000,

        // Offset Hub Data
        EZLOPI_FINFO_REL_OFFSET_SERIAL_NUMBER = 0,
        EZLOPI_FINFO_REL_OFFSET_FMW_VERSION = 0x0002,
        EZLOPI_FINFO_REL_OFFSET_FMW_BUILD = 0x0006,
        EZLOPI_FINFO_REL_OFFSET_DEVICE_ID = 0X008,
        EZLOPI_FINFO_REL_OFFSET_WIFI_SSID = 0x0020,
        EZLOPI_FINFO_REL_OFFSET_WIFI_PASS = 0x0040,
        EZLOPI_FINFO_REL_OFFSET_DEVICE_MAC = 0x0080,
        EZLOPI_FINFO_REL_OFFSET_DEVICE_NAME = 0x0C0,
        EZLOPI_FINFO_REL_OFFSET_MANUF_NAME = 0x0100,
        EZLOPI_FINFO_REL_OFFSET_BRAND_NAME = 0x0140,
        EZLOPI_FINFO_REL_OFFSET_MODEL_NAME = 0x0180,
        EZLOPI_FINFO_REL_OFFSET_EZLOPI_DEVICE_TYPE = 0x01C0

    } e_ezlopi_factory_info_v3_offset_t;

    typedef enum e_ezlopi_factory_info_v3_length
    {
        EZLOPI_FINFO_LEN_VERSION = 0x0004,
        EZLOPI_FINFO_LEN_CONFIG_VERSION = 0x0002,
        EZLOPI_FINFO_LEN_DEVICE_NAME = 0x0040,
        EZLOPI_FINFO_LEN_MANUF_NAME = 0x0040,
        EZLOPI_FINFO_LEN_BRAND_NAME = 0x0040,
        EZLOPI_FINFO_LEN_MODEL_NAME = 0x0040,
        EZLOPI_FINFO_LEN_DEVICE_ID = 0x0008,
        EZLOPI_FINFO_LEN_DEVICE_UUID = 0x007E,
        EZLOPI_FINFO_LEN_PROV_UUID = 0x007E,
        EZLOPI_FINFO_LEN_WIFI_SSID = 0x0020,
        EZLOPI_FINFO_LEN_WIFI_PASS = 0x0040,
        EZLOPI_FINFO_LEN_DEVICE_MAC = 0x0040,
        EZLOPI_FINFO_LEN_CLOUD_SERVER_URL = 0x00100,
        EZLOPI_FINFO_LEN_EZLOPI_DEVICE_TYPE = 0x0040,
        EZLOPI_FINFO_LEN_CA_CERTIFICATE = 0x1000,
        EZLOPI_FINFO_LEN_SSL_PRIVATE_KEY = 0x1000,
        EZLOPI_FINFO_LEN_SSL_SHARED_KEY = 0x1000,
        EZLOPI_FINFO_LEN_EZLOPI_CONFIG_JSON = 0x1000,
        EZLOPI_FINFO_LEN_PROVISIONING_TOKEN = 0x200,
        EZLOPI_FINFO_LEN_PROVISIONING_SERVER_URL = 0x100
    } e_ezlopi_factory_info_v3_length_t;

    typedef struct s_basic_factory_info
    {
        int16_t version;
        char* device_name;
        char* manufacturer;
        char* brand;
        char* model_number;
        unsigned long long id;
        char* device_uuid;
        char* prov_uuid;
        // char *wifi_ssid;
        // char *wifi_password;
        char* device_mac;
        char* cloud_server;
        char* device_type;
        char* provision_server;
        char* provision_token;
        char* user_id;
        uint16_t config_version;
    } s_basic_factory_info_t;

    uint32_t ezlopi_factory_info_v3_get_abs_address(uint32_t relative_offset, e_factory_info_v3_partition_type_t partition_type);

    void print_factory_info_v3(void);
    const esp_partition_t* ezlopi_factory_info_v3_init(void);

    uint32_t ezlopi_factory_info_v3_get_provisioning_status(void);

    uint16_t ezlopi_factory_info_v3_get_version(void);
    char* ezlopi_factory_info_v3_get_name(void);
    char* ezlopi_factory_info_v3_get_manufacturer(void);
    char* ezlopi_factory_info_v3_get_brand(void);
    char* ezlopi_factory_info_v3_get_model(void);
    unsigned long long ezlopi_factory_info_v3_get_id(void);
    char* ezlopi_factory_info_v3_get_device_uuid(void);
    char* ezlopi_factory_info_v3_get_provisioning_uuid(void);
    char* ezlopi_factory_info_v3_get_ssid(void);
    char* ezlopi_factory_info_v3_get_password(void);
    char* ezlopi_factory_info_v3_get_ezlopi_mac(void);
    char* ezlopi_factory_info_v3_get_cloud_server(void);
    const char* ezlopi_factory_info_v3_get_device_type(void);
    char* ezlopi_factory_info_v3_get_ca_certificate(void);
    char* ezlopi_factory_info_v3_get_ssl_private_key(void);
    char* ezlopi_factory_info_v3_get_ssl_shared_key(void);
    char* ezlopi_factory_info_v3_get_ezlopi_config(void);
    char* ezlopi_factory_info_get_v3_provision_token(void);
    char* ezlopi_factory_info_v3_get_provisioning_server(void);
    uint16_t ezlopi_factory_info_v3_get_config_version(void);

    int ezlopi_factory_info_v3_set_wifi(char* ssid, char* password);

    int ezlopi_factory_info_v3_set_basic(s_basic_factory_info_t* ezlopi_config_basic);
    int ezlopi_factory_info_v3_set_ezlopi_config(const char* data);
    int ezlopi_factory_info_v3_set_ca_cert(const char* data);
    int ezlopi_factory_info_v3_set_ssl_shared_key(const char* data);
    int ezlopi_factory_info_v3_set_ssl_public_key(const char* data);
    int ezlopi_factory_info_v3_set_ssl_private_key(const char* data);

    void ezlopi_factory_info_v3_free(void* arg);
    int ezlopi_factory_info_v3_factory_reset(void);

#if (EZLOPI_DEVICE_TYPE_GENERIC == EZLOPI_DEVICE_TYPE)

#elif (EZLOPI_DEVICE_TYPE_TEST_DEVICE == EZLOPI_DEVICE_TYPE)
    static const char* ezlopi_config_test =
        "{\
        \"cmd\": 3,\
        \"dev_detail\":\
        [\
            {\
                \"dev_type\": 1,\
                \"dev_name\": \"LED\",\
                \"id_room\": \"\",\
                \"id_item\": 1,\
                \"gpio_in\": 0,\
                \"gpio_out\": 1,\
                \"pullup_ip\": false,\
                \"pullup_op\": true,\
                \"is_ip\": false,\
                \"ip_inv\": false,\
                \"op_inv\": false,\
                \"val_ip\": false,\
                \"val_op\": false\
            },\
            {\
                \"dev_type\": 2,\
                \"dev_name\": \"Switch\",\
                \"id_room\": \"\",\
                \"id_item\": 4,\
                \"gpio\": 21,\
                \"logic_inv\": false,\
                \"pull_up\": true,\
                \"val_ip\": false,\
                \"ip_inv\": false\
            },\
            {\
                \"dev_type\": 3,\
                \"dev_name\": \"Water_leak\",\
                \"id_room\": \"\",\
                \"id_item\": 27,\
                \"gpio\": 7\
            },\
            {\
                \"dev_type\": 5,\
                \"dev_name\": \"RGB-Backlight\",\
                \"gpio\": 48,\
                \"id_item\": 9,\
                \"id_room\": \"\",\
                \"pwm_resln\": 3,\
                \"freq_hz\": 50,\
                \"duty_cycle\": 10\
            },\
            {\
                \"dev_type\": 6,\
                \"dev_name\": \"UART_MB1013\",\
                \"id_item\": 21,\
                \"id_room\": \"\",\
                \"gpio_tx\": 17,\
                \"gpio_rx\": 18,\
                \"baud_rate\": 9600\
            },\
            {\
                \"dev_type\": 7,\
                \"dev_name\": \"DHT 11\",\
                \"id_room\": \"\",\
                \"id_item\": 15,\
                \"gpio\": 15\
            },\
            {\
                \"dev_type\": 8,\
                \"dev_name\": \"GY271\",\
                \"id_room\": \"\",\
                \"id_item\": 7,\
                \"gpio_sda\": 40,\
                \"gpio_scl\": 41,\
                \"pullup_scl\": true,\
                \"pullup_sda\": true,\
                \"slave_addr\": 13\
            },\
             {\
                \"dev_name\": \"MQ9_LPG\",\
                \"dev_type\": 10,\
                \"id_item\": 63,\
                \"id_room\": \"\",\
                \"gpio1\": 25,\
                \"gpio2\": 6\
            }\
        ],\
    \"dev_total\": 8}";
#endif



#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_FACTORY_INFO_H_
