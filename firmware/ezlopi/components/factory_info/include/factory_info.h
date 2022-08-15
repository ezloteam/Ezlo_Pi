#ifndef __FACTORY_INFO_H__
#define __FACTORY_INFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

// #include <string>
#include "esp_partition.h"
#include "frozen.h"

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
#define UUID_LENGTH 16
#define ZWAVE_REGION_LENGTH 8
#define WIFI_SSID_LENGTH 32
#define WIFI_PASSWORD_LENGTH 32
#define PRODUCT_NAME_LENGTH 32

#define H_SN_OFFSET 0x00
#define H_VERSION_OFFSET 0x02
#define ID_OFFSET 0x04
#define UUID_OFFSET 12
#define ZWAVE_REGION_OFFSET 28
#define WIFI_SSID_OFFSET 36
#define WIFI_PASSWORD_OFFSET 68
#define PRODUCT_NAME_OFFSET 132

// Connection-INFO-0/1
#define C_SN_LENGTH 2
#define C_VERSION_LENGTH 2
#define PROVISIONING_SERVER_LENGTH 256
#define PROVISIONING_TOKEN_LENGTH 256
#define CLOUD_SERVER_LENGTH 128
#define CA_CERTIFICATE_LENGTH 0x2000
#define SSL_PRIVATE_KEY_LENGTH 0x1000
#define SSL_SHARED_KEY_LENGTH 0x2000

#define C_SN_OFFSET 0
#define C_VERSION_OFFSET 2
#define PROVISIONING_SERVER_OFFSET 4
#define PROVISIONING_TOKEN_OFFSET 260
#define CLOUD_SERVER_OFFSET 516
#define CA_CERTIFICATE_OFFSET 0x1000
#define SSL_PRIVATE_KEY_OFFSET 0x3000
#define SSL_SHARED_KEY_OFFSET 0x4000

    typedef struct s_factory_info
    {
        short h_version;
        unsigned long long id;
        char *name;
        char *controller_uuid;
        char *zwave_region;
        char *default_wifi_ssid;
        char *default_wifi_password;
        char *device_mac;

        char *cloud_server;
        char *provisioning_server;
        char *provisioning_token;
        char *ca_certificate;
        char *ssl_private_key;
        char *ssl_shared_key;
        char *ssl_public_key;
    } s_factory_info_t;

    s_factory_info_t *factory_info_init(void);
    s_factory_info_t *factory_info_get_info(void);

#ifdef __cplusplus
}
#endif

#endif // __FACTORY_INFO_H__