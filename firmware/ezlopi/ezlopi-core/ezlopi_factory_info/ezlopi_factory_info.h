#ifndef __FACTORY_INFO_H__
#define __FACTORY_INFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define ID_BIN_VERSION_1 1
#define ID_BIN_VERSION_2 2

#define ID_BIN_VERSION ID_BIN_VERSION_1

#define EZLOPI_GENERIC 0
#define EZLOPI_SWITCH_BOX 1

#define EZLOPI_DEVICE_TYPE EZLOPI_SWITCH_BOX

#include "esp_partition.h"
#include "frozen.h"

#define EZLOPI_FACTORY_INFO_V2_PARTITION_NAME "id"
#define EZLOPI_FACTORY_INFO_V2_PARTITION_SIZE 0x1000 // 20480 // 20KB
#define EZLOPI_FACTORY_INFO_V2_PARTITION_TYPE 0x40
#define EZLOPI_FACTORY_INFO_V2_SUBTYPE ESP_PARTITION_SUBTYPE_APP_FACTORY // ESP_PARTITION_SUBTYPE_ANY

#if (ID_BIN_VERSION_2 == ID_BIN_VERSION)
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
        CA_CERTIFICATE_OFFSET = 0x1000,
        SSL_PRIVATE_KEY_OFFSET = 0x2000,
        SSL_SHARED_KEY_OFFSET = 0x3000,
        EZLOPI_CONFIG_OFFSET = 0x4000,
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
    PROVISIONING_UUID_OFFSET = 0x0000 + 0x0314, /// fggggggggggg
    SSID_OFFSET = 0xE000 + 0x0024,
    PASSWORD_OFFSET = 0xE000 + 0x0044,
    DEVICE_MAC_OFFSET = 0xE000 + 0x00C4,
    CLOUD_SERVER_OFFSET = 0x0000 + 0x0214,
    DEVICE_TYPE_OFFSET = 0xE000 + 0x018A,
    CA_CERTIFICATE_OFFSET = 0x0000 + 0x3000,
    SSL_PRIVATE_KEY_OFFSET = 0x0000 + 0x4000,
    SSL_SHARED_KEY_OFFSET = 0x0000 + 0x5000,
} e_ezlopi_factory_info_v2_offset_t;

typedef enum e_ezlopi_factory_info_v2_length
{
    VERSION_LENGTH = 0x0002,
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
} e_ezlopi_factory_info_v2_length_t;
#endif

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

#if (EZLOPI_GENERIC == EZLOPI_DEVICE_TYPE)

#elif (EZLOPI_SWITCH_BOX == EZLOPI_DEVICE_TYPE)
static const char *switch_box_constant_config =
    "{\
        \"cmd\": 3,\
        \"dev_detail\":\
        [\
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
            }\
        ],\
    \"dev_total\": 1}";
#endif

#ifdef __cplusplus
}
#endif

#endif // __FACTORY_INFO_H__