#ifndef _EZLOPI_CORE_FACTORY_INFO_H_
#define _EZLOPI_CORE_FACTORY_INFO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <esp_partition.h>

#include "cjext.h"
#include "EZLOPI_USER_CONFIG.h"


#define EZLOPI_DEVICE_TYPE_TEST_DEVICE -1
#define EZLOPI_DEVICE_TYPE_GENERIC 0

#if CONFIG_EZPI_DEV_TYPE_TEST == 1
#define EZLOPI_DEVICE_TYPE EZLOPI_DEVICE_TYPE_TEST_DEVICE
#else 
#define EZLOPI_DEVICE_TYPE EZLOPI_DEVICE_TYPE_GENERIC
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

    void ezlopi_factory_info_v3_free_ca_certificate(void);
    void ezlopi_factory_info_v3_free_ssl_private_key(void);
    void  ezlopi_factory_info_v3_free_ssl_shared_key(void);

    char* ezlopi_factory_info_v3_get_ca_certificate(void);
    char* ezlopi_factory_info_v3_get_ssl_private_key(void);
    char* ezlopi_factory_info_v3_get_ssl_shared_key(void);

    char* ezlopi_factory_info_v3_get_ezlopi_config(void);
    char* ezlopi_factory_info_get_v3_provision_token(void);
    char* ezlopi_factory_info_v3_get_provisioning_server(void);
    uint16_t ezlopi_factory_info_v3_get_config_version(void);

    int ezlopi_factory_info_v3_set_wifi(char* ssid, char* password);

    int ezlopi_factory_info_v3_set_basic(s_basic_factory_info_t* ezlopi_config_basic);
    
    int ezlopi_factory_info_v3_set_ezlopi_config(cJSON * cj_data);
    int ezlopi_factory_info_v3_set_ca_cert(cJSON * cj_data);
    int ezlopi_factory_info_v3_set_ssl_shared_key(cJSON * cj_data);
    int ezlopi_factory_info_v3_set_ssl_public_key(cJSON * cj_data);
    int ezlopi_factory_info_v3_set_ssl_private_key(cJSON * cj_data);

    void ezlopi_factory_info_v3_free(void* arg);
    int ezlopi_factory_info_v3_factory_reset(void);

    int ezlopi_factory_info_v3_scenes_factory_soft_reset(void);

    int EZPI_CORE_factory_info_prov_init(void);

#if (EZLOPI_DEVICE_TYPE_GENERIC == EZLOPI_DEVICE_TYPE)

#elif (EZLOPI_DEVICE_TYPE_TEST_DEVICE == EZLOPI_DEVICE_TYPE)
    static const char* ezlopi_config_test =
        "{\
        \"cmd\": 3,\
        \"dev_detail\":\
        [\
            {\
                \"dev_type\": 1,\
                \"dev_name\": \"switch_temp\",\
                \"id_room\": \"\",\
                \"id_item\": 2,\
                \"gpio_in\": 0,\
                \"gpio_out\": 25,\
                \"gpio_out\": 25,\
                \"pullup_ip\": false,\
                \"pullup_op\": false,\
                \"is_ip\": false,\
                \"ip_inv\": false,\
                \"op_inv\": false,\
                \"val_ip\": false,\
                \"val_op\": false\
            },\
            {\
                \"dev_type\": 7,\
                \"dev_name\": \"temp_humi\",\
                \"id_room\": \"\",\
                \"id_item\": 16,\
                \"gpio\": 18\
            },\
            {\
                \"dev_type\": 1,\
                \"dev_name\": \"for_test\",\
                \"id_room\": \"\",\
                \"id_item\": 2,\
                \"gpio_in\": 0,\
                \"gpio_out\": 32,\
                \"pullup_ip\": false,\
                \"pullup_op\": false,\
                \"is_ip\": false,\
                \"ip_inv\": false,\
                \"op_inv\": false,\
                \"val_ip\": false,\
                \"val_op\": false\
            }\
        ], \
        \"dev_total\": 3}";
#endif



#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_FACTORY_INFO_H_
