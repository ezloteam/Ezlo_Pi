#include "string.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_partition.h"

#include "factory_info.h"
#include "debug.h"

static s_factory_info_t factory_info;
static const esp_partition_t *partition_ctx = NULL;

static unsigned long long factory_info_get_id(void);
static char *factory_info_get_uuid(void);
static char *factory_info_get_zwave_region(void);
static char *factory_info_get_default_wifi_ssid(void);
static char *factory_info_get_default_wifi_password(void);
static char *factory_info_get_name(void);
static char *factory_info_get_device_mac(void);

static char *factory_info_get_provisioning_server(void);
static char *factory_info_get_provisioning_token(void);
static char *factory_info_get_cloud_server(void);
static char *factory_info_get_ca_certificate(void);
static char *factory_info_get_ssl_private_key(void);
static char *factory_info_get_ssl_shared_key(void);

static void factory_info_set_default(void);
static char *factory_info_read_string_from_nvs_flash(int offset, uint32_t length);
static int factory_info_write_string_to_nvs_flash(int offset, int size, struct json_token *token);

#define free_and_assign_new(buff, new_data) \
    {                                       \
        if (buff)                           \
        {                                   \
            free(buff);                     \
        }                                   \
        buff = (char *)new_data;            \
    }

#define PRINT_FACTORY_INFO(name, offset, info)                                      \
    {                                                                               \
        if (NULL != info)                                                           \
        {                                                                           \
            TRACE_D("%s [off: %d, len: %d]: %s", name, offset, strlen(info), info); \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            TRACE_W("%s [off: %d, len: 0]: NULL", name, offset);                    \
        }                                                                           \
    }

static const char *default_uuid = "53f5bdc0-5347-11ec-b2d6-8f260f5287fa";
static const char *default_zwave_region = "US";
static const char *default_wifi_ssid_1 = "nepadldigisys";
static const char *default_wifi_password_1 = "NDS_0ffice";
static const char *default_name = "ezlopi-100004005";
static const char *default_provisioning_server = "https://req-disp-at0m.mios.com";
static const char *default_provisioning_token = "7ed0d422e075fda2d05f2b46acbb9d503f3505577055542f97a195268b3fe8a769eae18bbfe25abba8f98d0866d86a1a3336ecc68eed8a294c0f3bb9c521a176750b20ceb37354caed130a15ea29eb882d84e71476c6fcd1fa59b1bbd49487c8aa965a16706b68e4d08112a58cfe28d2b10b7cad40ddcab6a2d9ebd81c1d0eea";
static const char *default_cloud_server = "https://cloud.ezlo.com:7000";
static const char *default_ca_certificate = "-----BEGIN CERTIFICATE-----\r\nMIICbDCCAhGgAwIBAgIJAOByzaI7aHY9MAoGCCqGSM49BAMDMIGQMQswCQYDVQQG\r\nEwJVUzEUMBIGA1UECAwLIE5ldyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xDzAN\r\nBgNVBAoMBklUIE9wczEPMA0GA1UECwwGSVQgT3BzMRQwEgYDVQQDDAtlWkxPIExU\r\nRCBDQTEhMB8GCSqGSIb3DQEJARYSc3lzYWRtaW5zQGV6bG8uY29tMCAXDTE5MDUz\r\nMTE3MDE0N1oYDzIxMTkwNTA3MTcwMTQ3WjCBkDELMAkGA1UEBhMCVVMxFDASBgNV\r\nBAgMCyBOZXcgSmVyc2V5MRAwDgYDVQQHDAdDbGlmdG9uMQ8wDQYDVQQKDAZJVCBP\r\ncHMxDzANBgNVBAsMBklUIE9wczEUMBIGA1UEAwwLZVpMTyBMVEQgQ0ExITAfBgkq\r\nhkiG9w0BCQEWEnN5c2FkbWluc0BlemxvLmNvbTBWMBAGByqGSM49AgEGBSuBBAAK\r\nA0IABHLQdhLDYsafIFY8pZh96aDGqVm6E4r8nW9s4CfdpXaa/R4CnjaVpDQI7UmQ\r\n9vVDGZn8mcmm7VjKx+TSCS0MIKOjUzBRMB0GA1UdDgQWBBRiTl8Ez1l94jaqcxbi\r\nyxkVC0FkBTAfBgNVHSMEGDAWgBRiTl8Ez1l94jaqcxbiyxkVC0FkBTAPBgNVHRMB\r\nAf8EBTADAQH/MAoGCCqGSM49BAMDA0kAMEYCIQD7EUs8j50jKFd/46Zo95NbrPYQ\r\nPtLTHH9YjUkMEkYD5gIhAMP4y7E1aB78nQrmd3IX8MM32k9dM8xT0MztR16OtsuV\r\n-----END CERTIFICATE-----";
static const char *default_ssl_private_key = "-----BEGIN PRIVATE KEY-----\r\nMIGEAgEAMBAGByqGSM49AgEGBSuBBAAKBG0wawIBAQQg/2cQ79U/nLXvov+J4Kpi\r\nlO4qO88X0HbRmHhvRNKMiJ+hRANCAATYtFGFMTEqaO18wxhqnBBJy1ckbgDAaBGV\r\nSVBju06op4irOXNv7xcnjPqBryAvk862yDnwsUsIwEjtKVZC11sU\r\n-----END PRIVATE KEY-----";
static const char *default_ssl_shared_key = "-----BEGIN CERTIFICATE-----\r\nMIICDDCCAbKgAwIBAgIDAy6fMAoGCCqGSM49BAMCMIGQMQswCQYDVQQGEwJVUzEU\r\nMBIGA1UECAwLIE5ldyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xDzANBgNVBAoM\r\nBklUIE9wczEPMA0GA1UECwwGSVQgT3BzMRQwEgYDVQQDDAtlWkxPIExURCBDQTEh\r\nMB8GCSqGSIb3DQEJARYSc3lzYWRtaW5zQGV6bG8uY29tMCAXDTIxMTIwMjA4MTEw\r\nMVoYDzIyOTUwOTE2MDgxMTAxWjCBjDELMAkGA1UEBhMCVVMxEzARBgNVBAgMCk5l\r\ndyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xEzARBgNVBAoMCmNvbnRyb2xsZXIx\r\nLTArBgNVBAsMJDYzNTFjNzUwLTUzNDctMTFlYy1iMmQ2LThmMjYwZjUyODdmYTES\r\nMBAGA1UEAwwJMTAwMDA0MDA1MFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAE2LRRhTEx\r\nKmjtfMMYapwQSctXJG4AwGgRlUlQY7tOqKeIqzlzb+8XJ4z6ga8gL5POtsg58LFL\r\nCMBI7SlWQtdbFDAKBggqhkjOPQQDAgNIADBFAiEApGxLFOsnD7rbUmvY2sJvmlKp\r\nIxl5rgMrOQH6uKoQxa0CIFfN69EKILiT6R7HnEw76DZwkSwdmX/xpkjvN2bXJiuA\r\n-----END CERTIFICATE-----";

s_factory_info_t *factory_info_get_info(void)
{
    return &factory_info;
}

s_factory_info_t *factory_info_init(void)
{
    memset(&factory_info, 0, sizeof(s_factory_info_t));
    partition_ctx = esp_partition_find_first((esp_partition_type_t)FACTORY_INFO_PARTITION_TYPE, FACTORY_INFO_PARTITION_SUBTYPE, (const char *)FACTORY_INFO_PARTITION_NAME);

    if (partition_ctx != NULL)
    {
        TRACE_I("Partition found '%s' at offset '0x%x' with size '0x%x'",
                partition_ctx->label, partition_ctx->address, partition_ctx->size);

        factory_info.id = factory_info_get_id();
        factory_info.controller_uuid = factory_info_get_uuid();
        factory_info.zwave_region = factory_info_get_zwave_region();
        factory_info.default_wifi_ssid = factory_info_get_default_wifi_ssid();
        factory_info.default_wifi_password = factory_info_get_default_wifi_password();
        factory_info.name = factory_info_get_name();
        factory_info.cloud_server = factory_info_get_cloud_server();
        factory_info.provisioning_server = factory_info_get_provisioning_server();
        factory_info.provisioning_token = factory_info_get_provisioning_token();
        factory_info.ca_certificate = factory_info_get_ca_certificate();
        factory_info.ssl_private_key = factory_info_get_ssl_private_key();
        factory_info.ssl_shared_key = factory_info_get_ssl_shared_key();
    }
    else
    {
        TRACE_E("Partition \"id\" not found!!");
    }

    factory_info_set_default();

    TRACE_D("id [off: %d, len: %d]: %llu", ID_OFFSET, sizeof(long long), factory_info.id);

    PRINT_FACTORY_INFO("controller_uuid", UUID_OFFSET, factory_info.controller_uuid);
    PRINT_FACTORY_INFO("zwave_region", ZWAVE_REGION_OFFSET, factory_info.zwave_region);
    PRINT_FACTORY_INFO("name", PRODUCT_NAME_OFFSET, factory_info.name);
    PRINT_FACTORY_INFO("cloud_server", CLOUD_SERVER_OFFSET, factory_info.cloud_server);
    PRINT_FACTORY_INFO("provisioning_server", PROVISIONING_SERVER_OFFSET, factory_info.provisioning_server);
    PRINT_FACTORY_INFO("provisioning_token", PROVISIONING_TOKEN_OFFSET, factory_info.provisioning_token);
    PRINT_FACTORY_INFO("ca_certificate", CA_CERTIFICATE_OFFSET, factory_info.ca_certificate);
    PRINT_FACTORY_INFO("ssl_private_key", SSL_PRIVATE_KEY_OFFSET, factory_info.ssl_private_key);
    PRINT_FACTORY_INFO("ssl_shared_key", SSL_SHARED_KEY_OFFSET, factory_info.ssl_shared_key);

    // TRACE_D("controller_uuid [off: %d, len: %d]: \n%s", UUID_OFFSET, strlen(factory_info.controller_uuid), factory_info.controller_uuid);
    // TRACE_D("zwave_region [off: %d, len: %d]: \n%s", ZWAVE_REGION_OFFSET, strlen(factory_info.zwave_region), factory_info.zwave_region);
    // TRACE_D("name [off: %d, len: %d]: \n%s", PRODUCT_NAME_OFFSET, strlen(factory_info.name), factory_info.name);
    // TRACE_D("cloud_server [off: %d, len: %d]: \n%s", CLOUD_SERVER_OFFSET, strlen(factory_info.cloud_server), factory_info.cloud_server);
    // TRACE_D("provisioning_server [off: %d, len: %d]: \n%s", PROVISIONING_SERVER_OFFSET, strlen(factory_info.provisioning_server), factory_info.provisioning_server);
    // TRACE_D("provisioning_token [off: %d, len: %d]: \n%s", PROVISIONING_TOKEN_OFFSET, strlen(factory_info.provisioning_token), factory_info.provisioning_token);
    // TRACE_D("ca_certificate [off: %d, len: %d]: \n%s", CA_CERTIFICATE_OFFSET, strlen(factory_info.ca_certificate), factory_info.ca_certificate);
    // TRACE_D("ssl_private_key [off: %d, len: %d]: \n%s", SSL_PRIVATE_KEY_OFFSET, strlen(factory_info.ssl_private_key), factory_info.ssl_private_key);
    // TRACE_D("ssl_shared_key [off: %d, len: %d]: \n%s", SSL_SHARED_KEY_OFFSET, strlen(factory_info.ssl_shared_key), factory_info.ssl_shared_key);

    return &factory_info;
}

static unsigned long long factory_info_get_id(void)
{
    unsigned long long _id = 0ULL;

    uint8_t tmp_id_arr[8];
    memset(tmp_id_arr, 0, 8);
    esp_partition_read(partition_ctx, HUB_INFO_0_OFFSET + ID_OFFSET, &tmp_id_arr, ID_LENGTH);
    for (int i = 0; i < 8; i++)
    {
        _id |= tmp_id_arr[i] << (i * 8);
    }

    return _id;
}

static char *factory_info_get_uuid(void)
{
    char *uuid_string = NULL;
    if (partition_ctx)
    {
        uuid_string = (char *)malloc(40);

        if (uuid_string)
        {
            char *u_buf = (char *)malloc(UUID_LENGTH);

            if (u_buf)
            {
                if (ESP_OK == esp_partition_read(partition_ctx, HUB_INFO_0_SIZE + UUID_OFFSET, u_buf, UUID_LENGTH))
                {
                    if (uuid_string)
                    {
                        snprintf(uuid_string, 40, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                                 u_buf[0], u_buf[1], u_buf[2], u_buf[3], u_buf[4], u_buf[5], u_buf[6], u_buf[7],
                                 u_buf[8], u_buf[9], u_buf[10], u_buf[11], u_buf[12], u_buf[13], u_buf[14], u_buf[15]);
                    }
                }
                else
                {
                    TRACE_E("Couldn't fetch 'uuid' from factory_info!");
                }

                free(u_buf);
            }

            trace_imp("UUID from flash: %s", uuid_string);
        }
        else
        {
            trace_err("UUID malloc failed!");
        }
    }

    return uuid_string;
}

static char *factory_info_get_zwave_region(void)
{
    return NULL;
}

static char *factory_info_get_default_wifi_ssid(void)
{
    return NULL;
}

static char *factory_info_get_default_wifi_password(void)
{
    return NULL;
}

static char *factory_info_get_name(void)
{
    return factory_info_read_string_from_nvs_flash(HUB_INFO_0_OFFSET + PRODUCT_NAME_OFFSET, PRODUCT_NAME_LENGTH);
}

static char *factory_info_get_device_mac(void)
{
    return NULL;
}

static char *factory_info_get_provisioning_server(void)
{
    return factory_info_read_string_from_nvs_flash(CONNECTION_INFO_0_OFFSET + PROVISIONING_SERVER_OFFSET, PROVISIONING_SERVER_LENGTH);
}

static char *factory_info_get_provisioning_token(void)
{
    return factory_info_read_string_from_nvs_flash(CONNECTION_INFO_0_OFFSET + PROVISIONING_TOKEN_OFFSET, PROVISIONING_TOKEN_LENGTH);
}

static char *factory_info_get_cloud_server(void)
{
    return factory_info_read_string_from_nvs_flash(CONNECTION_INFO_0_OFFSET + CLOUD_SERVER_OFFSET, CLOUD_SERVER_LENGTH);
}

static char *factory_info_get_ca_certificate(void)
{
    return factory_info_read_string_from_nvs_flash(CONNECTION_INFO_0_OFFSET + CA_CERTIFICATE_OFFSET, CA_CERTIFICATE_LENGTH);
}

static char *factory_info_get_ssl_private_key(void)
{
    return factory_info_read_string_from_nvs_flash(CONNECTION_INFO_0_OFFSET + SSL_PRIVATE_KEY_OFFSET, SSL_PRIVATE_KEY_LENGTH);
}

static char *factory_info_get_ssl_shared_key(void)
{
    return factory_info_read_string_from_nvs_flash(CONNECTION_INFO_0_OFFSET + SSL_SHARED_KEY_OFFSET, SSL_SHARED_KEY_LENGTH);
}

static void factory_info_set_default(void)
{
    if (((NULL == factory_info.provisioning_server) || ('\0' == factory_info.provisioning_server[0])) ||
        ((NULL == factory_info.provisioning_token) || ('\0' == factory_info.provisioning_token[0])) ||
        ((NULL == factory_info.cloud_server) || ('\0' == factory_info.cloud_server[0])) ||
        ((NULL == factory_info.ca_certificate) || (NULL == strstr(factory_info.ca_certificate, "-----BEGIN CERTIFICATE-----"))) ||
        ((NULL == factory_info.ssl_private_key) || (NULL == strstr(factory_info.ssl_private_key, "-----BEGIN PRIVATE KEY-----"))) ||
        ((NULL == factory_info.ssl_shared_key) || (NULL == strstr(factory_info.ssl_shared_key, "-----BEGIN CERTIFICATE-----"))))
    {
        TRACE_W("ID-Info incomplete. Setting up default ID-Info!");

        factory_info.id = 100004005ULL;
        free_and_assign_new(factory_info.controller_uuid, default_uuid);
        free_and_assign_new(factory_info.zwave_region, default_zwave_region);
        free_and_assign_new(factory_info.default_wifi_ssid, default_wifi_ssid_1);
        free_and_assign_new(factory_info.default_wifi_password, default_wifi_password_1);
        free_and_assign_new(factory_info.name, default_name);
        free_and_assign_new(factory_info.provisioning_server, default_provisioning_server);
        free_and_assign_new(factory_info.provisioning_token, default_provisioning_token);
        free_and_assign_new(factory_info.cloud_server, default_cloud_server);
        free_and_assign_new(factory_info.ca_certificate, default_ca_certificate);
        free_and_assign_new(factory_info.ssl_private_key, default_ssl_private_key);
        free_and_assign_new(factory_info.ssl_shared_key, default_ssl_shared_key);
        free_and_assign_new(factory_info.ssl_public_key, NULL);
    }
    else
    {
        TRACE_I("** Found 'factory_info'.");
    }
}

static char *factory_info_read_string_from_nvs_flash(int offset, uint32_t length)
{
    char *read_string = NULL;

    if (partition_ctx)
    {
        char *buffer = (char *)malloc(length);
        if (buffer)
        {
            if (ESP_OK == esp_partition_read(partition_ctx, offset, buffer, length))
            {
                int s_length = (strlen(buffer) < length) ? strlen(buffer) : length;
                read_string = (char *)malloc(s_length + 1);
                if (NULL != read_string)
                {
                    snprintf(read_string, s_length + 1, "%s", buffer);
                }
            }
            else
            {
                TRACE_E("Couldn't fetch 'wifi_password' from factory_info!");
            }
            free(buffer);
        }
    }

    return read_string;
}

static int factory_info_write_string_to_nvs_flash(int offset, int size, struct json_token *token)
{
    int ret = 0;
    if (partition_ctx != NULL)
    {
        size_t length = (size_t)((token->len > size) ? size : token->len);
        ret = esp_partition_write(partition_ctx, offset, token->ptr, length);
        ret = ret | esp_partition_write(partition_ctx, offset + length, 0x00, 1);
    }
    return ret;
}
