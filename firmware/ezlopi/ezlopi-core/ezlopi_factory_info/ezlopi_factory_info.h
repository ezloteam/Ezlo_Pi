#ifndef __FACTORY_INFO_H__
#define __FACTORY_INFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

// #include <string>
#include "esp_partition.h"
#include "frozen.h"

#define EZLOPI_FACTORY_INFO_V2_PARTITION_NAME "id"
#define EZLOPI_FACTORY_INFO_V2_PARTITION_SIZE 0x1000 // 20480 // 20KB
#define EZLOPI_FACTORY_INFO_V2_PARTITION_TYPE 0x40
#define EZLOPI_FACTORY_INFO_V2_SUBTYPE ESP_PARTITION_SUBTYPE_APP_FACTORY // ESP_PARTITION_SUBTYPE_ANY

    typedef enum e_ezlopi_factory_info_v2_offset
    {
        VERSION_OFFSET = 0x0000,
        NAME_OFFSET = 0x0002,
        MANUFACTURER_OFFSET = 0x0082,
        BRAND_OFFSET = 0x00C2,
        MODEL_OFFSET = 0x0102,
        ID_OFFSET = 0x0142,
        DEVICE_UUID_OFFSET = 0x014A,
        PROVISIONING_UUID_OFFSET = 0x0172,
        SSID_OFFSET = 0x019A,
        PASSWORD_OFFSET = 0x01DA,
        DEVICE_MAC_OFFSET = 0x021A,
        CLOUD_SERVER_OFFSET = 0x0220,
        DEVICE_TYPE_OFFSET = 0x02A0,
        CA_CERTIFICATE_OFFSET = 0x0400,
        SSL_PRIVATE_KEY_OFFSET = 0x1400,
        SSL_SHARED_KEY_OFFSET = 0x2400,
        EZLOPI_CONFIG_OFFSET = 0x3400,
    } e_ezlopi_factory_info_v2_offset_t;

    typedef enum e_ezlopi_factory_info_v2_length
    {
        VERSION_LENGTH = 0x0002,
        NAME_LENGTH = 0x0080,
        MANUFACTURER_LENGTH = 0x0040,
        BRAND_LENGTH = 0x0040,
        MODEL_LENGTH = 0x0040,
        ID_LENGTH = 0x0008,
        DEVICE_UUID_LENGTH = 0x0028,
        PROVISIONING_UUID_LENGTH = 0x0028,
        SSID_LENGTH = 0x0040,
        PASSWORD_LENGTH = 0x0040,
        DEVICE_MAC_LENGTH = 0x0006,
        CLOUD_SERVER_LENGTH = 0x0080,
        DEVICE_TYPE_LENGTH = 0x0020,
        CA_CERTIFICATE_LENGTH = 0x1000,
        SSL_PRIVATE_KEY_LENGTH = 0x2000,
        SSL_SHARED_KEY_LENGTH = 0x3000,
        EZLOPI_CONFIG_LENGTH = 0x4000,
    } e_ezlopi_factory_info_v2_length_t;

    void print_factory_info_v2(void);
    void ezlopi_factory_info_v2_free(void *arg);
    uint16_t ezlopi_factory_info_v2_get_version(void);
    char *ezlopi_factory_info_v2_get_name(void);
    char *ezlopi_factory_info_v2_get_manufacturer(void);
    char *ezlopi_factory_info_v2_get_brand(void);
    char *ezlopi_factory_info_v2_get_model(void);
    unsigned long long ezlopi_factory_info_v2_get_id(void);
    char *ezlopi_factory_info_v2_get_device_uuid(void);
    char *ezlopi_factory_info_v2_get_provisioning_uuid(void);
    char *ezlopi_factory_info_v2_get_ssid(void);
    char *ezlopi_factory_info_v2_get_password(void);
    void ezlopi_factory_info_v2_get_ezlopi_mac(uint8_t *mac);
    char *ezlopi_factory_info_v2_get_cloud_server(void);
    char *ezlopi_factory_info_v2_get_device_type(void);
    char *ezlopi_factory_info_v2_get_ca_certificate(void);
    char *ezlopi_factory_info_v2_get_ssl_private_key(void);
    char *ezlopi_factory_info_v2_get_ssl_shared_key(void);
    char *ezlopi_factory_info_v2_get_ezlopi_config(void);

#if 0
    /* ezlopi_factory_info_v2 */
    typedef struct ezlopi_factory_info_v2_basic
    {
        uint16_t version;
        char *name;
        char *manufacturer;
        char *brand;
        char *model;
        unsigned long long id;
        char *device_uuid;
        char *provisioning_uuid;
        char *ssid;
        char *password;
        char *ezlo_device_mac;
        char *cloud_server;
    } ezlopi_factory_info_v2_basic_t;

////
/////////
///////
/* ezlopi_factory_info */
#define ENABLE_FACTORY_INFO_ENCRYPTION 0

// #define FACTORY_INFO_PARTITION_NAME "factory_id"
#define FACTORY_INFO_PARTITION_NAME "id"
#define FACTORY_INFO_PARTITION_SIZE 0x10000 // 64KB
#define FACTORY_INFO_PARTITION_TYPE 0x40
#define FACTORY_INFO_PARTITION_SUBTYPE ESP_PARTITION_SUBTYPE_ANY

#define HUB_INFO_0_OFFSET 0xE000
#define HUB_INFO_1_OFFSET 0xF000
#define CONNECTION_INFO_0_OFFSET 0x0000
#define CONNECTION_INFO_1_OFFSET 0x7000

#define HUB_INFO_0_SIZE 0x1000
#define HUB_INFO_1_SIZE 0x1000
#define CONNECTION_INFO_0_SIZE 0x7000
#define CONNECTION_INFO_1_SIZE 0x7000

// HUB-INFO-0/1
#define H_SN_LENGTH 2
#define H_VERSION_LENGTH 2
#define ID_LENGTH 8
#define UUID_LENGTH 40
#define ZWAVE_REGION_LENGTH 8
#define WIFI_SSID_LENGTH 32
#define WIFI_PASSWORD_LENGTH 32
#define PRODUCT_NAME_LENGTH 32
#define EZLOPI_MAC_LENGTH 6
#define MANUFACTURER_LENGTH 64
#define BRAND_LENGTH 64
#define MODEL_LENGTH 64
#define EZLOPI_DEVICE_TYPE_LENGTH 32

#define H_SN_OFFSET 0x00
#define H_VERSION_OFFSET 0x02
#define ID_OFFSET 0x04
// #define UUID_OFFSET 0x0C
#define UUID_OFFSET 0x1AA
#define ZWAVE_REGION_OFFSET 0x1C
#define WIFI_SSID_OFFSET 0x24
#define WIFI_PASSWORD_OFFSET 0x44
#define PRODUCT_NAME_OFFSET 0x84
#define EZLOPI_MAC_OFFSET 0xC4
#define MANUFACTURER_OFFSET 0xCA
#define BRAND_OFFSET 0x10A
#define MODEL_OFFSET 0x14A
#define EZLOPI_DEVICE_TYPE_OFFSET 0x18A

// Connection-INFO-0/1
#define C_SN_LENGTH 2
#define C_VERSION_LENGTH 2
#define PROVISIONING_UUID_LENGTH 40
#define PROVISIONING_SERVER_LENGTH 256
#define PROVISIONING_TOKEN_LENGTH 256
#define CLOUD_SERVER_LENGTH 256
#define EZLOPI_CONFIG_LENGTH 0x1000
#define CA_CERTIFICATE_LENGTH 0x1000
#define SSL_PRIVATE_KEY_LENGTH 0x1000
#define SSL_SHARED_KEY_LENGTH 0x2000

#define C_SN_OFFSET 0x00
#define C_VERSION_OFFSET 0x02
// #define PROVISIONING_UUID_OFFSET 0x04
#define PROVISIONING_UUID_OFFSET 0x314
#define PROVISIONING_SERVER_OFFSET 0x14
#define PROVISIONING_TOKEN_OFFSET 0x114
#define CLOUD_SERVER_OFFSET 0x214
#define EZLOPI_CONFIG_OFFSET 0x1000
#define CA_CERTIFICATE_OFFSET 0x3000
#define SSL_PRIVATE_KEY_OFFSET 0x4000
#define SSL_SHARED_KEY_OFFSET 0x5000

    typedef struct s_ezlopi_factory_info
    {
        short h_version;
        unsigned long long id;
        char *controller_uuid;
        char *zwave_region;
        char *default_wifi_ssid;
        char *default_wifi_password;
        char *product_name;
        uint8_t ezlopi_mac[6];
        char *ezlopi_manufacturer;
        char *ezlopi_brand;
        char *ezlopi_model;
        char *ezlopi_device_type;

        char *provisioning_uuid;
        char *provisioning_server;
        char *provisioning_token;
        char *cloud_server;
        char *ezlopi_config;
        char *ca_certificate;
        char *ssl_private_key;
        char *ssl_shared_key;

        // char *ssl_public_key;
    } s_ezlopi_factory_info_t;

    s_ezlopi_factory_info_t *ezlopi_factory_info_init();
    s_ezlopi_factory_info_t *ezlopi_factory_info_get_info(void);
    int ezlopi_factory_info_set_ezlopi_config(char *ezlopi_config);
    char *ezlopi_factory_info_get_ezlopi_config(void);

    static const char *switch_box_constant_config =
        "{\
        \"cmd\": 3,\
        \"dev_detail\":\
        [\
            {\
                \"dev_name\" : \"GXHTC3-Sensor\",\
                \"dev_type\" : 8,\
                \"gpio_scl\" : 26,\
                \"gpio_sda\" : 16,\
                \"id_item\" : 29,\
                \"id_room\" : \"\",\
                \"pullup_scl\" : true,\
                \"pullup_sda\" : true,\
                \"slave_addr\" : 0\
            },\
            {\
                \"dev_name\": \"Switch 1\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 39,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 2\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 38,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 3\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 15,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 4\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 4,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 5\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 5,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 6\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 6,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 7\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 7,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 8\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 12,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 9\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 17,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 10\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 18,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Master Switch\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 255,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            }\
        ],\
    \"dev_total\": 1}";
#endif

#ifdef __cplusplus
}
#endif

#endif // __FACTORY_INFO_H__