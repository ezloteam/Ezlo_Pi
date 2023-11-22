#ifndef __FACTORY_INFO_H__
#define __FACTORY_INFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define ID_BIN_VERSION_1 1
#define ID_BIN_VERSION_2 2

#define ID_BIN_VERSION ID_BIN_VERSION_1

#define EZLOPI_DEVICE_TYPE_TEST_DEVICE -1
#define EZLOPI_DEVICE_TYPE_GENERIC 0
#define EZLOPI_DEVICE_TYPE_SWITCH_BOX 1
#define EZLOPI_DEVICE_TYPE_IR_BLASTER 2
#define EZLOPI_DEVICE_TYPE_SOUND_SENSOR 3
#define EZLOPI_DEVICE_TYPE_AMBIENT_TRACKER_PRO 4

    // #define EZLOPI_DEVICE_TYPE EZLOPI_DEVICE_TYPE_GENERIC
#define EZLOPI_DEVICE_TYPE EZLOPI_DEVICE_TYPE_TEST_DEVICE

#include "esp_partition.h"

#define EZLOPI_FACTORY_INFO_V2_PARTITION_NAME "id"
#define EZLOPI_FACTORY_INFO_V2_PARTITION_SIZE 0xF000 // 20480 // 20KB
#define EZLOPI_FACTORY_INFO_V2_PARTITION_TYPE 0x40
#define EZLOPI_FACTORY_INFO_V2_SUBTYPE ESP_PARTITION_SUBTYPE_APP_FACTORY // ESP_PARTITION_SUBTYPE_ANY

#if (ID_BIN_VERSION_2 == ID_BIN_VERSION)
    typedef enum e_ezlopi_factory_info_v2_offset
    {
        // VERSION_OFFSET = 0x0000,
        // NAME_OFFSET = 0x0002,
        // MANUFACTURER_OFFSET = 0x0082,
        // BRAND_OFFSET = 0x00C2,
        // MODEL_OFFSET = 0x0102,
        // ID_OFFSET = 0x0142,
        // DEVICE_UUID_OFFSET = 0x014A,
        // PROVISIONING_UUID_OFFSET = 0x0172,
        // SSID_OFFSET = 0x019A,
        // PASSWORD_OFFSET = 0x01DA,
        // DEVICE_MAC_OFFSET = 0x021A,
        // CLOUD_SERVER_OFFSET = 0x0220,
        // DEVICE_TYPE_OFFSET = 0x02A0,

        // CA_CERTIFICATE_OFFSET = 0x1000,
        // SSL_PRIVATE_KEY_OFFSET = 0x2000,
        // SSL_SHARED_KEY_OFFSET = 0x3000,
        // EZLOPI_CONFIG_OFFSET = 0x4000,
        VERSION_OFFSET = 0xE000 + 0x0002,
        NAME_OFFSET = 0xE000 + 0x0084,
        MANUFACTURER_OFFSET = 0xE000 + 0x00CA,
        BRAND_OFFSET = 0xE000 + 0x010A,
        MODEL_OFFSET = 0xE000 + 0x014A,
        ID_OFFSET = 0xE000 + 0x0004,
        DEVICE_UUID_OFFSET = 0xE000 + 0x01AA,
        PROVISIONING_UUID_OFFSET = 0x0000 + 0x0314, /// fggggggggggg
        SSID_OFFSET = 0xE000 + 0x0024,
        PASSWORD_OFFSET = 0xE000 + 0x0044,
        DEVICE_MAC_OFFSET = 0xE000 + 0x00C4,
        CLOUD_SERVER_OFFSET = 0x0000 + 0x0214,
        DEVICE_TYPE_OFFSET = 0xE000 + 0x018A,
        CA_CERTIFICATE_OFFSET = 0x3000,
        SSL_PRIVATE_KEY_OFFSET = 0x4000,
        SSL_SHARED_KEY_OFFSET = 0x5000,
        EZLOPI_CONFIG_OFFSET = 0x1000,
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
        SSL_PRIVATE_KEY_LENGTH = 0x1000,
        SSL_SHARED_KEY_LENGTH = 0x1000,
        EZLOPI_CONFIG_LENGTH = 0x1000,
    } e_ezlopi_factory_info_v2_length_t;

#elif (ID_BIN_VERSION_1 == ID_BIN_VERSION)
typedef enum e_ezlopi_factory_info_v2_offset
{
    VERSION_OFFSET = 0xE000 + 0x0002,
    NAME_OFFSET = 0xE000 + 0x0084,
    MANUFACTURER_OFFSET = 0xE000 + 0x00CA,
    BRAND_OFFSET = 0xE000 + 0x010A,
    MODEL_OFFSET = 0xE000 + 0x014A,
    ID_OFFSET = 0xE000 + 0x0004,
    DEVICE_UUID_OFFSET = 0xE000 + 0x01AA,
    SSID_OFFSET = 0xE000 + 0x0024,
    PASSWORD_OFFSET = 0xE000 + 0x0044,
    DEVICE_MAC_OFFSET = 0xE000 + 0x00C4,
    DEVICE_TYPE_OFFSET = 0xE000 + 0x018A,

    CLOUD_SERVER_OFFSET = 0x0000 + 0x0214,
    PROVISIONING_SERVER_OFFSET = 0X000 + 0X14,
    PROVISIONING_TOKEN_OFFSET = 0X000 + 0X114,
    PROVISIONING_UUID_OFFSET = 0x0000 + 0x0314,
    CONFIG_VERSION_OFFSET = 0X000 + 0x02,
    CA_CERTIFICATE_OFFSET = 0x0000 + 0x3000,
    SSL_PRIVATE_KEY_OFFSET = 0x0000 + 0x4000,
    SSL_SHARED_KEY_OFFSET = 0x0000 + 0x5000,
    EZLOPI_CONFIG_OFFSET = 0x0000 + 0x1000,
} e_ezlopi_factory_info_v2_offset_t;

typedef enum e_ezlopi_factory_info_v2_length
{
    VERSION_LENGTH = 0x0002,
    CONFIG_VERSION_LENGTH = 0x0002,
    NAME_LENGTH = 0x0040,
    MANUFACTURER_LENGTH = 0x0040,
    BRAND_LENGTH = 0x0040,
    MODEL_LENGTH = 0x0040,
    ID_LENGTH = 0x0008,
    DEVICE_UUID_LENGTH = 0x0028,
    PROVISIONING_UUID_LENGTH = 0x0028,
    SSID_LENGTH = 0x0020,
    PASSWORD_LENGTH = 0x0040,
    DEVICE_MAC_LENGTH = 0x0006,
    CLOUD_SERVER_LENGTH = 0x00100,
    DEVICE_TYPE_LENGTH = 0x0020,
    CA_CERTIFICATE_LENGTH = 0x1000,
    SSL_PRIVATE_KEY_LENGTH = 0x1000,
    SSL_SHARED_KEY_LENGTH = 0x2000,
    EZLOPI_CONFIG_LENGTH = 0x1000,
    PROVISIONING_TOKEN_LENGTH = 0x100,
    PROVISIONING_SERVER_LENGTH = 0x100
} e_ezlopi_factory_info_v2_length_t;

typedef struct s_basic_factory_info
{
    int16_t version;
    char *device_name;
    char *manufacturer;
    char *brand;
    char *model_number;
    unsigned long long id;
    char *device_uuid;
    char *prov_uuid;
    // char *wifi_ssid;
    // char *wifi_password;
    uint8_t device_mac[6];
    char *cloud_server;
    char *device_type;
    char *provision_server;
    char *provision_token;
    char *user_id;
    uint16_t config_version;
} s_basic_factory_info_t;
#endif

    void print_factory_info_v2(void);
    const esp_partition_t *ezlopi_factory_info_v2_init(void);

    uint32_t ezlopi_factory_info_v2_get_provisioning_status(void);

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
    char *ezlopi_factory_info_get_v2_provision_token(void);
    char *ezlopi_factory_info_v2_get_provisioning_server(void);
    uint16_t ezlopi_factory_info_v2_get_config_version(void);

    int ezlopi_factory_info_v2_set_wifi(char *ssid, char *password);

    int ezlopi_factory_info_v2_set_basic(s_basic_factory_info_t *ezlopi_config_basic);
    int ezlopi_factory_info_v2_set_ezlopi_config(char *data);
    int ezlopi_factory_info_v2_set_ca_cert(char *data);
    int ezlopi_factory_info_v2_set_ssl_shared_key(char *data);
    int ezlopi_factory_info_v2_set_ssl_public_key(char *data);
    int ezlopi_factory_info_v2_set_ssl_private_key(char *data);

    void ezlopi_factory_info_v2_free(void *arg);
    int ezlopi_factory_info_v2_factory_reset(void);

#if (EZLOPI_DEVICE_TYPE_GENERIC == EZLOPI_DEVICE_TYPE)

#elif (EZLOPI_DEVICE_TYPE_IR_BLASTER == EZLOPI_DEVICE_TYPE)
static const char *ir_blaster_constant_config =
    "{\
        \"cmd\": 3,\
        \"dev_detail\":\
        [\
            {\
                \"dev_name\": \"IR_Blaster1\",\
                \"dev_type\": 5,\
                \"gpio\": 3,\
                \"id_item\": 30,\
                \"id_room\": \"\",\
                \"pwm_resln\": 8,\
                \"freq_hz\": 50,\
                \"duty_cycle\": 30\
            }\
        ],\
    \"dev_total\": 1}";
#elif (EZLOPI_DEVICE_TYPE_SWITCH_BOX == EZLOPI_DEVICE_TYPE)
static const char *switch_box_constant_config =
    "{\
        \"cmd\": 3,\
        \"dev_detail\":\
        [\
            {\
                \"dev_name\" : \"GXHTC3-Sensor\",\
                \"dev_type\" : 8,\
                \"gpio_scl\" : 45,\
                \"gpio_sda\" : 46,\
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
#elif (EZLOPI_DEVICE_TYPE_TEST_DEVICE == EZLOPI_DEVICE_TYPE)
#define EZLOPI_DEVICE_TYPE_NAME "generic"
static const char *test_device_constant_config =
#if 0
    "{\
    \"cmd\":3,\
    \"dev_detail\":[{\
                \"dev_type\": 10,\
                \"dev_name\": \"fingerprint\",\
                \"id_room\": \"\",\
                \"id_item\": 66,\
                \"gpio1\": 17,\
                \"gpio2\": 16,\
                \"gpio3\": 25\
            }\
    ],\
    \"dev_total\":2\
    }";
#endif
    "{\
    \"cmd\":3,\
    \"dev_detail\":[{\
                \"dev_type\": 7,\
                \"dev_name\": \"DHT 11\",\
                \"id_room\": \"\",\
                \"id_item\": 15,\
                \"gpio\": 18\
            }\
    ],\
    \"dev_total\":2\
    }";

#endif
#ifdef __cplusplus
}
#endif

#endif // __FACTORY_INFO_H__
