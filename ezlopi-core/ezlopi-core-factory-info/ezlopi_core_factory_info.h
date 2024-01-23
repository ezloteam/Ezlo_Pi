#ifndef _EZLOPI_CORE_FACTORY_INFO_H_
#define _EZLOPI_CORE_FACTORY_INFO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define ID_BIN_VERSION_1 1
#define ID_BIN_VERSION_2 2
#define ID_BIN_VERSION_3 3

// #define ID_BIN_VERSION ID_BIN_VERSION_1
#define ID_BIN_VERSION ID_BIN_VERSION_3

#define EZLOPI_DEVICE_TYPE_TEST_DEVICE -1
#define EZLOPI_DEVICE_TYPE_GENERIC 0

#define EZLOPI_DEVICE_TYPE EZLOPI_DEVICE_TYPE_GENERIC
    // #define EZLOPI_DEVICE_TYPE EZLOPI_DEVICE_TYPE_TEST_DEVICE

#include "esp_partition.h"

#if (ID_BIN_VERSION_3 == ID_BIN_VERSION)

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

#elif (ID_BIN_VERSION_2 == ID_BIN_VERSION)
#define EZLOPI_FACTORY_INFO_V2_PARTITION_NAME "id"
#define EZLOPI_FACTORY_INFO_V2_PARTITION_SIZE 0xF000 // 20480 // 20KB
#define EZLOPI_FACTORY_INFO_V2_PARTITION_TYPE 0x40
#define EZLOPI_FACTORY_INFO_V2_SUBTYPE ESP_PARTITION_SUBTYPE_APP_FACTORY // ESP_PARTITION_SUBTYPE_ANY
#endif

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
    CONFIG_VERSION_OFFSET = 0X0000 + 0x02,
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

#elif (ID_BIN_VERSION_3 == ID_BIN_VERSION)
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
    char *device_name;
    char *manufacturer;
    char *brand;
    char *model_number;
    unsigned long long id;
    char *device_uuid;
    char *prov_uuid;
    // char *wifi_ssid;
    // char *wifi_password;
    char *device_mac;
    char *cloud_server;
    char *device_type;
    char *provision_server;
    char *provision_token;
    char *user_id;
    uint16_t config_version;
} s_basic_factory_info_t;

#endif

    uint32_t ezlopi_factory_info_v3_get_abs_address(uint32_t relative_offset, e_factory_info_v3_partition_type_t partition_type);

    void print_factory_info_v3(void);
    const esp_partition_t *ezlopi_factory_info_v3_init(void);

    uint32_t ezlopi_factory_info_v3_get_provisioning_status(void);

    uint16_t ezlopi_factory_info_v3_get_version(void);
    char *ezlopi_factory_info_v3_get_name(void);
    char *ezlopi_factory_info_v3_get_manufacturer(void);
    char *ezlopi_factory_info_v3_get_brand(void);
    char *ezlopi_factory_info_v3_get_model(void);
    unsigned long long ezlopi_factory_info_v3_get_id(void);
    char *ezlopi_factory_info_v3_get_device_uuid(void);
    char *ezlopi_factory_info_v3_get_provisioning_uuid(void);
    char *ezlopi_factory_info_v3_get_ssid(void);
    char *ezlopi_factory_info_v3_get_password(void);
    char *ezlopi_factory_info_v3_get_ezlopi_mac(void);
    char *ezlopi_factory_info_v3_get_cloud_server(void);
    char *ezlopi_factory_info_v3_get_device_type(void);
    char *ezlopi_factory_info_v3_get_ca_certificate(void);
    char *ezlopi_factory_info_v3_get_ssl_private_key(void);
    char *ezlopi_factory_info_v3_get_ssl_shared_key(void);
    char *ezlopi_factory_info_v3_get_ezlopi_config(void);
    char *ezlopi_factory_info_get_v3_provision_token(void);
    char *ezlopi_factory_info_v3_get_provisioning_server(void);
    uint16_t ezlopi_factory_info_v3_get_config_version(void);

    int ezlopi_factory_info_v3_set_wifi(char *ssid, char *password);

    int ezlopi_factory_info_v3_set_basic(s_basic_factory_info_t *ezlopi_config_basic);
    int ezlopi_factory_info_v3_set_ezlopi_config(const char *data);
    int ezlopi_factory_info_v3_set_ca_cert(const char *data);
    int ezlopi_factory_info_v3_set_ssl_shared_key(const char *data);
    int ezlopi_factory_info_v3_set_ssl_public_key(const char *data);
    int ezlopi_factory_info_v3_set_ssl_private_key(const char *data);

    void ezlopi_factory_info_v3_free(void *arg);
    int ezlopi_factory_info_v3_factory_reset(void);

#if (EZLOPI_DEVICE_TYPE_GENERIC == EZLOPI_DEVICE_TYPE)

#elif (EZLOPI_DEVICE_TYPE_TEST_DEVICE == EZLOPI_DEVICE_TYPE)
static const char *ezlopi_config_test =
    "{\
        \"cmd\": 3,\
        \"dev_detail\":\
        [\
{\
        \"dev_type\":6,\
        \"dev_name\":\"PMS5003\",\
        \"id_room\":\"\",\
        \"id_item\":37,\
        \"gpio1\":40,\
        \"gpio2\":41,\
        \"gpio3\":39,\
        \"gpio4\":42,\
        \"baudrate\":9600\
        }\
        ],\
    \"dev_total\": 1}";
#endif

#if 0 // V2
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

    /** Functions to free **/
    void ezlopi_factory_info_v2_free(void *arg);
    void ezlopi_factory_info_v2_free_ezlopi_config(void);

    int ezlopi_factory_info_v2_factory_reset(void);

#endif // v2

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_FACTORY_INFO_H_
