#include "string.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_partition.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "trace.h"
#include "ezlopi_factory_info.h"

static s_ezlopi_factory_info_t *factory_info = NULL;
static const esp_partition_t *partition_ctx = NULL;

static unsigned long long ezlopi_factory_info_get_id(void);
static char *ezlopi_factory_info_get_controller_uuid(void);
static char *ezlopi_factory_info_get_zwave_region(void);
static char *ezlopi_factory_info_get_default_wifi_ssid(void);
static char *ezlopi_factory_info_get_default_wifi_password(void);
static char *ezlopi_factory_info_get_name(void);
static void ezlopi_factory_info_get_ezlopi_mac(uint8_t *mac_buf);
static char *ezlopi_factory_info_manufacturer(void);
static char *ezlopi_factory_info_get_brand(void);
static char *ezlopi_factory_info_get_model(void);
static char *ezlopi_factory_info_get_ezlopi_device_type(void);

static char *ezlopi_factory_info_get_provisioning_uuid(void);
static char *ezlopi_factory_info_get_provisioning_server(void);
static char *ezlopi_factory_info_get_provisioning_token(void);
static char *ezlopi_factory_info_get_cloud_server(void);
static char *ezlopi_factory_info_get_ca_certificate(void);
static char *ezlopi_factory_info_get_ssl_private_key(void);
static char *ezlopi_factory_info_get_ssl_shared_key(void);

static void ezlopi_factory_info_set_default(void);
static char *ezlopi_factory_info_read_string_from_flash(int offset, uint32_t length);
static int ezlopi_factory_info_write_string_to_flash(int offset, uint8_t *data, uint32_t len);
static int ezlopi_factory_info_erase_range_of_flash(uint32_t offset, uint32_t len);

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

ezlopi_factory_info_v2_t *factory_info_v2 = NULL;

ezlopi_factory_info_v2_t *ezlzopi_factory_info_v2_get_info(void)
{
    if (NULL == factory_info_v2)
    {
        ezlopi_factory_info_v2_init();
    }

    return factory_info_v2;
}

/**********************************************************************************/

s_ezlopi_factory_info_t *ezlopi_factory_info_get_info(void)
{
    if (NULL == factory_info)
    {
        ezlopi_factory_info_init();
    }

    return factory_info;
}

s_ezlopi_factory_info_t *ezlopi_factory_info_init(void)
{
    factory_info = malloc(sizeof(s_ezlopi_factory_info_t));
    if (NULL != factory_info)
    {
        memset(factory_info, 0, sizeof(s_ezlopi_factory_info_t));
        partition_ctx = esp_partition_find_first((esp_partition_type_t)FACTORY_INFO_PARTITION_TYPE, FACTORY_INFO_PARTITION_SUBTYPE, (const char *)FACTORY_INFO_PARTITION_NAME);

        if (partition_ctx != NULL)
        {
            TRACE_I("Partition found '%s' at offset '0x%x' with size '0x%x'",
                    partition_ctx->label, partition_ctx->address, partition_ctx->size);

            factory_info->h_version = 0;
            factory_info->id = ezlopi_factory_info_get_id(); // serial number of the device
            factory_info->controller_uuid = ezlopi_factory_info_get_controller_uuid();
            factory_info->zwave_region = ezlopi_factory_info_get_zwave_region();
            factory_info->default_wifi_ssid = ezlopi_factory_info_get_default_wifi_ssid();
            factory_info->default_wifi_password = ezlopi_factory_info_get_default_wifi_password();
            factory_info->product_name = ezlopi_factory_info_get_name();
            ezlopi_factory_info_get_ezlopi_mac(factory_info->ezlopi_mac);
            factory_info->ezlopi_manufacturer = ezlopi_factory_info_manufacturer();
            factory_info->ezlopi_brand = ezlopi_factory_info_get_brand();
            factory_info->ezlopi_model = ezlopi_factory_info_get_model();
            factory_info->ezlopi_device_type = ezlopi_factory_info_get_ezlopi_device_type();

            factory_info->provisioning_uuid = ezlopi_factory_info_get_provisioning_uuid();
            factory_info->provisioning_server = ezlopi_factory_info_get_provisioning_server();
            factory_info->provisioning_token = ezlopi_factory_info_get_provisioning_token();
            factory_info->cloud_server = ezlopi_factory_info_get_cloud_server();
            factory_info->ezlopi_config = ezlopi_factory_info_get_ezlopi_config();
            factory_info->ca_certificate = ezlopi_factory_info_get_ca_certificate();
            factory_info->ssl_private_key = ezlopi_factory_info_get_ssl_private_key();
            factory_info->ssl_shared_key = ezlopi_factory_info_get_ssl_shared_key();
        }
        else
        {
            TRACE_E("Partition \"id\" not found!!");
        }

        ezlopi_factory_info_set_default();

        TRACE_D("id [off: %d, len: %d]: %llu", ID_OFFSET, sizeof(long long), factory_info->id);
        PRINT_FACTORY_INFO("controller_uuid", UUID_OFFSET, factory_info->controller_uuid);
        PRINT_FACTORY_INFO("zwave_region", ZWAVE_REGION_OFFSET, factory_info->zwave_region);
        PRINT_FACTORY_INFO("default_wifi_ssid", WIFI_SSID_OFFSET, factory_info->default_wifi_ssid);
        PRINT_FACTORY_INFO("default_wifi_password", WIFI_PASSWORD_OFFSET, factory_info->default_wifi_password);
        PRINT_FACTORY_INFO("product_name", PRODUCT_NAME_OFFSET, factory_info->product_name);
        TRACE_D("ezlopi_mac [off: %d, len: %d]: %02x:%02x:%02x:%02x:%02x:%02x", EZLOPI_MAC_OFFSET, EZLOPI_MAC_LENGTH,
                factory_info->ezlopi_mac[5], factory_info->ezlopi_mac[4], factory_info->ezlopi_mac[3], factory_info->ezlopi_mac[2], factory_info->ezlopi_mac[1], factory_info->ezlopi_mac[0]);
        PRINT_FACTORY_INFO("ezlopi_manufacturer", MANUFACTURER_OFFSET, factory_info->ezlopi_manufacturer);
        PRINT_FACTORY_INFO("ezlopi_brand", MANUFACTURER_OFFSET, factory_info->ezlopi_brand);
        PRINT_FACTORY_INFO("ezlopi_model", MANUFACTURER_OFFSET, factory_info->ezlopi_model);
        PRINT_FACTORY_INFO("ezlopi_device_type", MANUFACTURER_OFFSET, factory_info->ezlopi_device_type);

        PRINT_FACTORY_INFO("provisioning_uuid", PROVISIONING_UUID_OFFSET, factory_info->provisioning_uuid);
        PRINT_FACTORY_INFO("provisioning_server", PROVISIONING_SERVER_OFFSET, factory_info->provisioning_server);
        PRINT_FACTORY_INFO("provisioning_token", PROVISIONING_TOKEN_OFFSET, factory_info->provisioning_token);
        PRINT_FACTORY_INFO("cloud_server", CLOUD_SERVER_OFFSET, factory_info->cloud_server);
        PRINT_FACTORY_INFO("ezlopi_config", CLOUD_SERVER_OFFSET, factory_info->ezlopi_config);
        PRINT_FACTORY_INFO("ca_certificate", CA_CERTIFICATE_OFFSET, factory_info->ca_certificate);
        PRINT_FACTORY_INFO("ssl_private_key", SSL_PRIVATE_KEY_OFFSET, factory_info->ssl_private_key);
        PRINT_FACTORY_INFO("ssl_shared_key", SSL_SHARED_KEY_OFFSET, factory_info->ssl_shared_key);
    }

    return factory_info;
}

char *ezlopi_factory_info_get_ezlopi_config(void)
{
    if (NULL == factory_info->ezlopi_config)
    {
        // factory_info->ezlopi_config = ezlopi_factory_info_read_string_from_flash(CONNECTION_INFO_0_OFFSET + EZLOPI_CONFIG_OFFSET, EZLOPI_CONFIG_LENGTH);
        factory_info->ezlopi_config = switch_box_constant_config;
    }

    return factory_info->ezlopi_config;
}

int ezlopi_factory_info_set_ezlopi_config(char *ezlopi_config)
{
    int ret = -1;
    uint32_t data_len = strlen(ezlopi_config);

    if ((NULL != ezlopi_config) && (EZLOPI_CONFIG_LENGTH > data_len))
    {
        if (ESP_OK == ezlopi_factory_info_erase_range_of_flash(CONNECTION_INFO_0_OFFSET + EZLOPI_CONFIG_OFFSET, EZLOPI_CONFIG_LENGTH))
        {
            ret = ezlopi_factory_info_write_string_to_flash(CONNECTION_INFO_0_OFFSET + EZLOPI_CONFIG_OFFSET, (uint8_t *)ezlopi_config, data_len + 1);
        }
        else
        {
            TRACE_E("'ezlopi-cnfig' erase failed!");
        }
    }
    else
    {
        TRACE_E("Error writing to flash!");
    }

    return ret;
}

static unsigned long long ezlopi_factory_info_get_id(void)
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

static char *ezlopi_factory_info_get_controller_uuid(void)
{
    return ezlopi_factory_info_read_string_from_flash(HUB_INFO_0_OFFSET + UUID_OFFSET, PROVISIONING_SERVER_LENGTH);
}

static char *ezlopi_factory_info_get_provisioning_uuid(void)
{
    return ezlopi_factory_info_read_string_from_flash(CONNECTION_INFO_0_OFFSET + PROVISIONING_UUID_OFFSET, PROVISIONING_SERVER_LENGTH);
}

static char *ezlopi_factory_info_get_zwave_region(void)
{
    return NULL;
}

static char *ezlopi_factory_info_get_default_wifi_ssid(void)
{
    return ezlopi_factory_info_read_string_from_flash(HUB_INFO_0_OFFSET + WIFI_SSID_OFFSET, WIFI_SSID_LENGTH);
}

static char *ezlopi_factory_info_get_default_wifi_password(void)
{
    return ezlopi_factory_info_read_string_from_flash(HUB_INFO_0_OFFSET + WIFI_PASSWORD_OFFSET, WIFI_PASSWORD_LENGTH);
}

static char *ezlopi_factory_info_get_name(void)
{
    return ezlopi_factory_info_read_string_from_flash(HUB_INFO_0_OFFSET + PRODUCT_NAME_OFFSET, PRODUCT_NAME_LENGTH);
}

static void ezlopi_factory_info_get_ezlopi_mac(uint8_t *mac_buf)
{
    memset(mac_buf, 0, 6);

    if (partition_ctx)
    {
        // CONNECTION_INFO_0_OFFSET + SSL_SHARED_KEY_OFFSET, SSL_SHARED_KEY_LENGTH
        if (ESP_OK == esp_partition_read(partition_ctx, HUB_INFO_0_OFFSET + EZLOPI_MAC_OFFSET, mac_buf, EZLOPI_MAC_LENGTH))
        {
            TRACE_I("'ezlopi-mac' read from factory_info success");
            TRACE_I("EZLO-PI-MAC: %02x:%02x:%02x:%02x:%02x:%02x", mac_buf[0], mac_buf[1], mac_buf[2], mac_buf[3], mac_buf[4], mac_buf[5]);
        }
        else
        {
            TRACE_E("Couldn't fetch 'ezlopi-mac' from factory_info!");
        }
    }
}

static char *ezlopi_factory_info_manufacturer(void)
{
    return ezlopi_factory_info_read_string_from_flash(HUB_INFO_0_OFFSET + MANUFACTURER_OFFSET, MANUFACTURER_LENGTH);
}

static char *ezlopi_factory_info_get_brand(void)
{
    return ezlopi_factory_info_read_string_from_flash(HUB_INFO_0_OFFSET + BRAND_OFFSET, BRAND_LENGTH);
}

static char *ezlopi_factory_info_get_model(void)
{
    return ezlopi_factory_info_read_string_from_flash(HUB_INFO_0_OFFSET + MODEL_OFFSET, MODEL_LENGTH);
}

static char *ezlopi_factory_info_get_ezlopi_device_type(void)
{
    return ezlopi_factory_info_read_string_from_flash(HUB_INFO_0_OFFSET + EZLOPI_DEVICE_TYPE_OFFSET, EZLOPI_DEVICE_TYPE_LENGTH);
}

static char *ezlopi_factory_info_get_provisioning_server(void)
{
    return ezlopi_factory_info_read_string_from_flash(CONNECTION_INFO_0_OFFSET + PROVISIONING_SERVER_OFFSET, PROVISIONING_SERVER_LENGTH);
}

static char *ezlopi_factory_info_get_provisioning_token(void)
{
    return ezlopi_factory_info_read_string_from_flash(CONNECTION_INFO_0_OFFSET + PROVISIONING_TOKEN_OFFSET, PROVISIONING_TOKEN_LENGTH);
}

static char *ezlopi_factory_info_get_cloud_server(void)
{
    return ezlopi_factory_info_read_string_from_flash(CONNECTION_INFO_0_OFFSET + CLOUD_SERVER_OFFSET, CLOUD_SERVER_LENGTH);
}

static char *ezlopi_factory_info_get_ca_certificate(void)
{
    return ezlopi_factory_info_read_string_from_flash(CONNECTION_INFO_0_OFFSET + CA_CERTIFICATE_OFFSET, CA_CERTIFICATE_LENGTH);
}

static char *ezlopi_factory_info_get_ssl_private_key(void)
{
    return ezlopi_factory_info_read_string_from_flash(CONNECTION_INFO_0_OFFSET + SSL_PRIVATE_KEY_OFFSET, SSL_PRIVATE_KEY_LENGTH);
}

static char *ezlopi_factory_info_get_ssl_shared_key(void)
{
    return ezlopi_factory_info_read_string_from_flash(CONNECTION_INFO_0_OFFSET + SSL_SHARED_KEY_OFFSET, SSL_SHARED_KEY_LENGTH);
}

static void ezlopi_factory_info_set_default(void)
{
    if (((NULL == factory_info->provisioning_server) || ('\0' == factory_info->provisioning_server[0])) ||
        ((NULL == factory_info->provisioning_token) || ('\0' == factory_info->provisioning_token[0])) ||
        ((NULL == factory_info->cloud_server) || ('\0' == factory_info->cloud_server[0])) ||
        ((NULL == factory_info->ca_certificate) || (NULL == strstr(factory_info->ca_certificate, "-----BEGIN CERTIFICATE-----"))) ||
        ((NULL == factory_info->ssl_private_key) || (NULL == strstr(factory_info->ssl_private_key, "-----BEGIN PRIVATE KEY-----"))) ||
        ((NULL == factory_info->ssl_shared_key) || (NULL == strstr(factory_info->ssl_shared_key, "-----BEGIN CERTIFICATE-----"))))
    {
        TRACE_W("ID-Info incomplete. Setting up default ID-Info!");

        factory_info->id = 100004005ULL;
        free_and_assign_new(factory_info->controller_uuid, default_uuid);
        free_and_assign_new(factory_info->zwave_region, default_zwave_region);
        free_and_assign_new(factory_info->default_wifi_ssid, default_wifi_ssid_1);
        free_and_assign_new(factory_info->default_wifi_password, default_wifi_password_1);
        free_and_assign_new(factory_info->product_name, default_name);

        free_and_assign_new(factory_info->provisioning_server, default_provisioning_server);
        free_and_assign_new(factory_info->provisioning_token, default_provisioning_token);
        free_and_assign_new(factory_info->cloud_server, default_cloud_server);
        free_and_assign_new(factory_info->ca_certificate, default_ca_certificate);
        free_and_assign_new(factory_info->ssl_private_key, default_ssl_private_key);
        free_and_assign_new(factory_info->ssl_shared_key, default_ssl_shared_key);
        // free_and_assign_new(factory_info->ssl_public_key, NULL);
    }
    else
    {
        TRACE_I("** Found 'factory_info'.");
    }
}

static char *ezlopi_factory_info_read_string_from_flash(int offset, uint32_t length)
{
    char *read_string = NULL;

    if (partition_ctx)
    {
        char *buffer = (char *)malloc(length);
        if (buffer)
        {
            if (ESP_OK == esp_partition_read(partition_ctx, offset, buffer, length))
            {
                // dump("buffer", buffer, 0, length);
                // vTaskDelay(1);

                int s_length = (strlen(buffer) < length) ? strlen(buffer) : length;
                read_string = (char *)malloc(s_length + 1);
                if (NULL != read_string)
                {
                    snprintf(read_string, s_length + 1, "%s", buffer);
                }
                else
                {
                    TRACE_E("'read_string' malloc failed!");
                }
            }
            else
            {
                TRACE_E("Couldn't fetch 'string' from id-flash-region!");
            }
            free(buffer);
        }
        else
        {
            TRACE_E("MALLOC faield!");
        }
    }
    else
    {
        TRACE_E("'partition_ctx' is null!");
    }

    vTaskDelay(0);

    return read_string;
}

static int ezlopi_factory_info_erase_range_of_flash(uint32_t offset, uint32_t len)
{
    esp_err_t ret = ESP_OK;
    if (partition_ctx)
    {
        ret = esp_partition_erase_range(partition_ctx, offset, 0x1000);
    }

    return ret;
}

static int ezlopi_factory_info_write_string_to_flash(int offset, uint8_t *data, uint32_t len)
{
    int ret = -1;
    if (partition_ctx != NULL)
    {
        ret = (ESP_OK == esp_partition_write(partition_ctx, offset, data, len)) ? len : ret;
    }

    return ret;
}
