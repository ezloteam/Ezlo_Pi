#ifndef __FACTORY_INFO_H__
#define __FACTORY_INFO_H__

#include <string>
#include "esp_partition.h"
#include "frozen.h"

using namespace std;

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

class factory_info
{
private:
    const esp_partition_t *partition_ctx = NULL;
    void set_default_if_not_found(void);
    char *read_string_from_partition(int offset, int length);
    int write_string_to_flash(int offset, int size, struct json_token *token);

protected:
    factory_info() {}
    static factory_info *factory_info_;

public:
    unsigned long long id = 0ULL;
    char *name = NULL;
    short h_version = 0;
    char *controller_uuid = NULL;
    char *zwave_region = NULL;
    char *default_wifi_ssid = NULL;
    char *default_wifi_password = NULL;
    char *cloud_server = NULL;
    char *provisioning_server = NULL;
    char *provisioning_token = NULL;
    char *ca_certificate = NULL;
    char *ssl_private_key = NULL;
    char *ssl_shared_key = NULL;
    char *ssl_public_key = NULL;

    unsigned long long get_id(void);
    char *get_uuid(void);
    char *get_zwave_region(void);
    char *get_default_wifi_ssid(void);
    char *get_default_wifi_password(void);
    char *get_name(void);

    char *get_provisioning_server(void);
    char *get_provisioning_token(void);
    char *get_cloud_server(void);
    char *get_ca_certificate(void);
    char *get_ssl_private_key(void);
    char *get_ssl_shared_key(void);

    int set_id(char *payload, int len);
    int set_uuid(char *payload, int len);
    int set_zwave_region(char *payload, int len);
    int set_default_wifi_ssid(char *payload, int len);
    int set_default_wifi_password(char *payload, int len);
    int set_name(char *payload, int len);

    int set_provisioning_server(char *payload, int len);
    int set_provisioning_token(char *payload, int len);
    int set_cloud_server(char *payload, int len);
    int set_ca_certificate(char *payload, int len);
    int set_ssl_private_key(char *payload, int len);
    int set_ssl_shared_key(char *payload, int len);

    static string update_factory_info(char *payload, int len);

    int init(void);
    static factory_info *get_instance(void);

    factory_info(factory_info &other) = delete;
    void operator=(const factory_info &) = delete;
};

#endif // __FACTORY_INFO_H__