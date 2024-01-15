#include "string.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_partition.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "trace.h"

#include "ezlopi_nvs.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_cloud_constants.h"

static const esp_partition_t *partition_ctx_v2 = NULL;
static char *g_ca_certificate = NULL;
static char *g_ssl_private_key = NULL;
static char *g_ssl_shared_key = NULL;
static char *g_ezlopi_config = NULL;
static uint32_t g_provisioning_status = 0;

const char *ezlopi_device_type_str[] = {
    "ezlopi_generic",
    "ezlopi_device_switchbox",
    "ezlopi_device_irblaster",
    "ezlopi_sensor_sound",
    "ezlopi_sensor_ambienttrackerpro",
};

static int ezlopi_factory_info_v2_set_4kb(char *name, char *data, uint32_t offset);
static char *ezlopi_factory_info_v2_read_string(e_ezlopi_factory_info_v2_offset_t offset, e_ezlopi_factory_info_v2_length_t length);

#define UPDATE_STRING_VALUE(buffer, data, offset, length)  \
    {                                                      \
        if (data)                                          \
        {                                                  \
            snprintf(buffer + offset, length, "%s", data); \
        }                                                  \
    }

const esp_partition_t *ezlopi_factory_info_v2_init(void)
{
    if (NULL == partition_ctx_v2)
    {
        partition_ctx_v2 = esp_partition_find_first(EZLOPI_FACTORY_INFO_V2_PARTITION_TYPE, EZLOPI_FACTORY_INFO_V2_SUBTYPE, EZLOPI_FACTORY_INFO_V2_PARTITION_NAME);
    }
    return partition_ctx_v2;
}

void ezlopi_factory_info_v2_free(void *arg)
{
    if (arg)
    {
        free(arg);
    }
}

void print_factory_info_v2(void)
{
    // char *name = ezlopi_factory_info_v2_get_name();
    // char *manufacturer = ezlopi_factory_info_v2_get_manufacturer();
    // char *brand = ezlopi_factory_info_v2_get_brand();
    // char *model = ezlopi_factory_info_v2_get_model();

    uint8_t mac[6];
    memset(mac, 0, 6);
    ezlopi_factory_info_v2_get_ezlopi_mac(mac);

    uint16_t version = ezlopi_factory_info_v2_get_version();
    unsigned long long id = ezlopi_factory_info_v2_get_id();

    char *name = ezlopi_factory_info_v2_get_name();
    char *manufacturer = ezlopi_factory_info_v2_get_manufacturer();
    char *brand = ezlopi_factory_info_v2_get_brand();
    char *model = ezlopi_factory_info_v2_get_model();
    char *device_uuid = ezlopi_factory_info_v2_get_device_uuid();
    char *provisioning_uuid = ezlopi_factory_info_v2_get_provisioning_uuid();
    char *wifi_ssid = ezlopi_factory_info_v2_get_ssid();
    char *wifi_password = ezlopi_factory_info_v2_get_password();
    char *cloud_server = ezlopi_factory_info_v2_get_cloud_server();
    char *device_type = ezlopi_factory_info_v2_get_device_type();
    char *ca_certificate = ezlopi_factory_info_v2_get_ca_certificate();
    char *ssl_private_key = ezlopi_factory_info_v2_get_ssl_private_key();
    char *ssl_shared_key = ezlopi_factory_info_v2_get_ssl_shared_key();

    char *ezlopi_config = ezlopi_factory_info_v2_get_ezlopi_config();

    TRACE_D("----------------- Factory Info -----------------");
    // TRACE_W("VERSION[off: 0x%04X, size: 0x%04X]:                %d", VERSION_OFFSET, VERSION_LENGTH, version);
    TRACE_W("SERIAL-ID [off: 0x%04X, size: 0x%04X]:             %llu", ID_OFFSET, ID_LENGTH, id);
    // TRACE_W("MAC [off: 0x%04X, size: 0x%04X]:                   %02X:%02X:%02X:%02X:%02X:%02X", DEVICE_MAC_OFFSET, DEVICE_MAC_LENGTH, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    TRACE_W("NAME [off: 0x%04X, size: 0x%04X]:                  %s", NAME_OFFSET, NAME_LENGTH, name ? name : ezlopi_null_str);
    // TRACE_W("MANUFACTURER [off: 0x%04X, size: 0x%04X]:          %s", MANUFACTURER_OFFSET, MANUFACTURER_LENGTH, manufacturer ? manufacturer : ezlopi_null_str);
    // TRACE_W("BRAND [off: 0x%04X, size: 0x%04X]:                 %s", BRAND_OFFSET, BRAND_LENGTH, brand ? brand : ezlopi_null_str);
    // TRACE_W("MODEL [off: 0x%04X, size: 0x%04X]:                 %s", MODEL_OFFSET, MODEL_LENGTH, model ? model : ezlopi_null_str);
    TRACE_W("DEVICE_UUID [off: 0x%04X, size: 0x%04X]:           %s", DEVICE_UUID_OFFSET, DEVICE_UUID_LENGTH, device_uuid ? device_uuid : ezlopi_null_str);
    // TRACE_W("PROVISIONING_UUID [off: 0x%04X, size: 0x%04X]:     %s", PROVISIONING_UUID_OFFSET, PROVISIONING_UUID_LENGTH, provisioning_uuid ? provisioning_uuid : ezlopi_null_str);
    TRACE_W("WIFI-SSID [off: 0x%04X, size: 0x%04X]:             %s", SSID_OFFSET, SSID_LENGTH, wifi_ssid ? wifi_ssid : ezlopi_null_str);
    // TRACE_W("WIFI-PASSWORD [off: 0x%04X, size: 0x%04X]:         %s", PASSWORD_OFFSET, PASSWORD_LENGTH, wifi_password ? wifi_password : ezlopi_null_str);
    // TRACE_W("CLOUD_SERVER [off: 0x%04X, size: 0x%04X]:          %s", CLOUD_SERVER_OFFSET, CLOUD_SERVER_LENGTH, cloud_server);
    // TRACE_W("DEVICE_TYPE [off: 0x%04X, size: 0x%04X]:           %s", DEVICE_TYPE_OFFSET, DEVICE_TYPE_LENGTH, device_type);
    // TRACE_W("CA_CERTIFICATE [off: 0x%04X, size: 0x%04X]:        %s", CA_CERTIFICATE_OFFSET, CA_CERTIFICATE_LENGTH, ca_certificate);
    // TRACE_W("SSL_PRIVATE_KEY [off: 0x%04X, size: 0x%04X]:       %s", SSL_PRIVATE_KEY_OFFSET, SSL_PRIVATE_KEY_LENGTH, ssl_private_key);
    // TRACE_W("SSL_SHARED_KEY [off: 0x%04X, size: 0x%04X]:        %s", SSL_SHARED_KEY_OFFSET, SSL_SHARED_KEY_LENGTH, ssl_shared_key);
#if (ID_BIN_VERSION_2 == ID_BIN_VERSION)
    TRACE_W("EZLOPI_CONFIG [off: 0x%04X, size: 0x%04X]:         %s", EZLOPI_CONFIG_OFFSET, EZLOPI_CONFIG_LENGTH, ezlopi_config);
#elif (ID_BIN_VERSION_1 == ID_BIN_VERSION)
    // TRACE_W("EZLOPI_CONFIG:                                     %s", ezlopi_config ? ezlopi_config : ezlopi__str);
#endif
    TRACE_D("-------------------------------------------------");

    ezlopi_factory_info_v2_free(name);
    ezlopi_factory_info_v2_free(manufacturer);
    ezlopi_factory_info_v2_free(brand);
    ezlopi_factory_info_v2_free(model);
    ezlopi_factory_info_v2_free(device_uuid);
    ezlopi_factory_info_v2_free(provisioning_uuid);
    ezlopi_factory_info_v2_free(wifi_ssid);
    ezlopi_factory_info_v2_free(wifi_password);
    ezlopi_factory_info_v2_free(cloud_server);
    ezlopi_factory_info_v2_free_ezlopi_config();
}

/** Getter */
uint32_t ezlopi_factory_info_v2_get_provisioning_status(void)
{
    return g_provisioning_status;
}

uint16_t ezlopi_factory_info_v2_get_version(void)
{
    uint16_t _version = 0ULL;

    if (ezlopi_factory_info_v2_init())
    {
        uint8_t tmp_version_arr[2];
        memset(tmp_version_arr, 0, 2);
        int read_couont = esp_partition_read(partition_ctx_v2, VERSION_OFFSET, &tmp_version_arr, VERSION_LENGTH);
        TRACE_B("read-count: %d", read_couont);

        for (int i = 0; i < 8; i++)
        {
            _version |= tmp_version_arr[i] << (i * 8);
        }
    }
    else
    {
        TRACE_E("'ezlopi_factory_info_v2' failed");
    }

    return _version;
}

char *ezlopi_factory_info_v2_get_name(void)
{
    return ezlopi_factory_info_v2_read_string(NAME_OFFSET, NAME_LENGTH);
}

char *ezlopi_factory_info_v2_get_manufacturer(void)
{
    return ezlopi_factory_info_v2_read_string(MANUFACTURER_OFFSET, MANUFACTURER_LENGTH);
}

char *ezlopi_factory_info_v2_get_brand(void)
{
    return ezlopi_factory_info_v2_read_string(BRAND_OFFSET, BRAND_LENGTH);
}

char *ezlopi_factory_info_v2_get_model(void)
{
    return ezlopi_factory_info_v2_read_string(MODEL_OFFSET, MODEL_LENGTH);
}

char *ezlopi_factory_info_get_v2_provision_token(void)
{
    return ezlopi_factory_info_v2_read_string(PROVISIONING_TOKEN_OFFSET, PROVISIONING_TOKEN_LENGTH);
}

uint16_t ezlopi_factory_info_v2_get_config_version(void)
{
    uint16_t config_version = 0;

    if (ezlopi_factory_info_v2_init())
    {
        uint8_t tmp_config_version_arr[2];
        memset(tmp_config_version_arr, 0, 2);
        esp_partition_read(partition_ctx_v2, CONFIG_VERSION_OFFSET, tmp_config_version_arr, CONFIG_VERSION_LENGTH);
        config_version = (tmp_config_version_arr[0] << 8) | tmp_config_version_arr[1];
    }

    return config_version;
}

unsigned long long ezlopi_factory_info_v2_get_id(void)
{
    unsigned long long _id = 0ULL;

    if (ezlopi_factory_info_v2_init())
    {
        uint8_t tmp_id_arr[8];
        memset(tmp_id_arr, 0, 8);
        esp_partition_read(partition_ctx_v2, ID_OFFSET, &tmp_id_arr, ID_LENGTH);

        for (int i = 0; i < 8; i++)
        {
            _id |= tmp_id_arr[i] << (i * 8);
        }
    }

    return _id;
}

char *ezlopi_factory_info_v2_get_device_uuid(void)
{
    return ezlopi_factory_info_v2_read_string(DEVICE_UUID_OFFSET, DEVICE_UUID_LENGTH);
}

char *ezlopi_factory_info_v2_get_provisioning_uuid(void)
{
    return ezlopi_factory_info_v2_read_string(PROVISIONING_UUID_OFFSET, PROVISIONING_UUID_LENGTH);
}

char *ezlopi_factory_info_v2_get_ssid(void)
{
    return ezlopi_factory_info_v2_read_string(SSID_OFFSET, SSID_LENGTH);
}

char *ezlopi_factory_info_v2_get_password(void)
{
    return ezlopi_factory_info_v2_read_string(PASSWORD_OFFSET, PASSWORD_LENGTH);
}

void ezlopi_factory_info_v2_get_ezlopi_mac(uint8_t *mac)
{

    if (mac)
    {
        memset(mac, 0, DEVICE_MAC_LENGTH);
        if (ezlopi_factory_info_v2_init())
        {

            esp_err_t err = esp_partition_read(partition_ctx_v2, DEVICE_MAC_OFFSET, mac, DEVICE_MAC_LENGTH);
            if (err != ESP_OK)
            {
                TRACE_E("Error reading from the partiton !");
            }
            else
            {
                TRACE_I("MAC: %x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            }
        }
    }
}

char *ezlopi_factory_info_v2_get_cloud_server(void)
{
    char *cloud_server = ezlopi_factory_info_v2_read_string(CLOUD_SERVER_OFFSET, CLOUD_SERVER_LENGTH);
    if (cloud_server && strstr(cloud_server, "https://"))
    {
        g_provisioning_status = 1;
    }
    return cloud_server;
}

char *ezlopi_factory_info_v2_get_provisioning_server(void)
{
    char *provisioning_server = ezlopi_factory_info_v2_read_string(PROVISIONING_SERVER_OFFSET, PROVISIONING_SERVER_LENGTH);
    if (provisioning_server && strstr(provisioning_server, "https://"))
    {
        return provisioning_server;
    }
    else
    {
        return NULL;
    }
}

char *ezlopi_factory_info_v2_get_device_type(void)
{
    const static char *undefined = "undefined";
    char *ret = undefined;
    // return ezlopi_factory_info_v2_read_string(DEVICE_TYPE_OFFSET, DEVICE_TYPE_LENGTH);
    switch (EZLOPI_DEVICE_TYPE)
    {
    case EZLOPI_DEVICE_TYPE_TEST_DEVICE:
    {
        ret = ezlopi_device_type_str[EZLOPI_DEVICE_TYPE_GENERIC];
        break;
    }
    case EZLOPI_DEVICE_TYPE_GENERIC:
    case EZLOPI_DEVICE_TYPE_SWITCH_BOX:
    case EZLOPI_DEVICE_TYPE_IR_BLASTER:
    case EZLOPI_DEVICE_TYPE_SOUND_SENSOR:
    case EZLOPI_DEVICE_TYPE_AMBIENT_TRACKER_PRO:
    {

        ret = ezlopi_device_type_str[EZLOPI_DEVICE_TYPE];
        break;
    }
    default:
    {
        ret = undefined;
        break;
    }
    }

    return ret;
}

char *ezlopi_factory_info_v2_get_ca_certificate(void)
{
    if (NULL == g_ca_certificate)
    {
        g_ca_certificate = ezlopi_factory_info_v2_read_string(CA_CERTIFICATE_OFFSET, CA_CERTIFICATE_LENGTH);
    }
    return g_ca_certificate;
}

char *ezlopi_factory_info_v2_get_ssl_private_key(void)
{
    if (NULL == g_ssl_private_key)
    {
        g_ssl_private_key = ezlopi_factory_info_v2_read_string(SSL_PRIVATE_KEY_OFFSET, SSL_PRIVATE_KEY_LENGTH);
    }
    return g_ssl_private_key;
}

char *ezlopi_factory_info_v2_get_ssl_shared_key(void)
{
    if (NULL == g_ssl_shared_key)
    {
        g_ssl_shared_key = ezlopi_factory_info_v2_read_string(SSL_SHARED_KEY_OFFSET, SSL_SHARED_KEY_LENGTH);
    }
    return g_ssl_shared_key;
}

char *ezlopi_factory_info_v2_get_ezlopi_config(void)
{
#if (EZLOPI_DEVICE_TYPE_SWITCH_BOX == EZLOPI_DEVICE_TYPE)
    return switch_box_constant_config;
#elif (EZLOPI_DEVICE_TYPE_IR_BLASTER == EZLOPI_DEVICE_TYPE)
    return ir_blaster_constant_config;
#elif (EZLOPI_DEVICE_TYPE_TEST_DEVICE == EZLOPI_DEVICE_TYPE)
    return test_device_constant_config;
#elif (EZLOPI_DEVICE_TYPE_GENERIC == EZLOPI_DEVICE_TYPE)
    if (NULL == g_ezlopi_config)
    {
        g_ezlopi_config = ezlopi_factory_info_v2_read_string(EZLOPI_CONFIG_OFFSET, EZLOPI_CONFIG_LENGTH);
    }
    return g_ezlopi_config;
#elif (EZLOPI_DEVICE_TYPE_AMBIENT_TRACKER_PRO == EZLOPI_DEVICE_TYPE)
    return ambient_tracker_constant_config;
#endif
    return NULL;
}

int ezlopi_factory_info_v2_set_basic(s_basic_factory_info_t *ezlopi_config_basic)
{
    int ret = 0;

    if (ezlopi_config_basic)
    {
        TRACE_W("Here");
        if (ezlopi_factory_info_v2_init())
        {
#if (ID_BIN_VERSION_1 == ID_BIN_VERSION)
            uint32_t length = 4 * 1024;
            uint32_t flash_offset = 0xE000; // hub_0_offset

            char *tmp_buffer = (char *)malloc(length);
            if (tmp_buffer)
            {
                if (ESP_OK == esp_partition_read(partition_ctx_v2, flash_offset, tmp_buffer, length))
                {
                    if (NULL != ezlopi_config_basic->device_name)
                    {
                        // UPDATE_STRING_VALUE(tmp_buffer, ezlopi_config_basic->device_name, 0x0084, NAME_LENGTH);
                        memset(tmp_buffer + 0x0084, 0, NAME_LENGTH);
                        memcpy(tmp_buffer + 0x0084, ezlopi_config_basic->device_name, NAME_LENGTH);
                        TRACE_I("device_name: %s", ezlopi_config_basic->device_name);
                    }

                    if (NULL != ezlopi_config_basic->manufacturer)
                    {
                        // UPDATE_STRING_VALUE(tmp_buffer, ezlopi_config_basic->manufacturer, 0x00CA, MANUFACTURER_LENGTH);
                        memset(tmp_buffer + 0x00CA, 0, MANUFACTURER_LENGTH);
                        memcpy(tmp_buffer + 0x00CA, ezlopi_config_basic->manufacturer, MANUFACTURER_LENGTH);
                        TRACE_I("manufacturer: %s", ezlopi_config_basic->manufacturer);
                    }

                    if (NULL != ezlopi_config_basic->brand)
                    {
                        // UPDATE_STRING_VALUE(tmp_buffer, ezlopi_config_basic->brand, 0x010A, BRAND_LENGTH);
                        memset(tmp_buffer + 0x010A, 0, BRAND_LENGTH);
                        memcpy(tmp_buffer + 0x010A, ezlopi_config_basic->brand, BRAND_LENGTH);
                        TRACE_I("brand: %s", ezlopi_config_basic->brand);
                    }
                    if (NULL != ezlopi_config_basic->model_number)
                    {
                        // UPDATE_STRING_VALUE(tmp_buffer, ezlopi_config_basic->model_number, 0x014A, MODEL_LENGTH);
                        memset(tmp_buffer + 0x014A, 0, MODEL_LENGTH);
                        memcpy(tmp_buffer + 0x014A, ezlopi_config_basic->model_number, MODEL_LENGTH);
                        TRACE_I("model_number: %s", ezlopi_config_basic->model_number);
                    }
                    if (NULL != ezlopi_config_basic->device_uuid)
                    {
                        // UPDATE_STRING_VALUE(tmp_buffer, ezlopi_config_basic->device_uuid, 0x01AA, DEVICE_UUID_LENGTH);
                        memset(tmp_buffer + 0x01AA, 0, DEVICE_UUID_LENGTH);
                        memcpy(tmp_buffer + 0x01AA, ezlopi_config_basic->device_uuid, DEVICE_UUID_LENGTH);
                        TRACE_I("device_uuid: %s", ezlopi_config_basic->device_uuid);
                    }
                    // snprintf(tmp_buffer + 0x0024, length, "%s", ezlopi_config_basic->wifi_ssid);
                    // snprintf(tmp_buffer + 0x0044, length, "%s", ezlopi_config_basic->wifi_password);
                    if (NULL != ezlopi_config_basic->device_type)
                    {
                        // UPDATE_STRING_VALUE(tmp_buffer, ezlopi_config_basic->device_type, 0x018A, DEVICE_TYPE_LENGTH);
                        memset(tmp_buffer + 0x018A, 0, DEVICE_TYPE_LENGTH);
                        memcpy(tmp_buffer + 0x018A, ezlopi_config_basic->device_type, DEVICE_TYPE_LENGTH);
                        TRACE_I("device_type: %s", ezlopi_config_basic->device_type);
                    }

                    if (NULL != ezlopi_config_basic->device_mac)
                    {
                        memset(tmp_buffer + 0x00C4, 0, DEVICE_MAC_LENGTH);
                        memcpy(tmp_buffer + 0x00C4, ezlopi_config_basic->device_mac, DEVICE_MAC_LENGTH);
                    }

                    if (0 != ezlopi_config_basic->id)
                    {
                        memset(tmp_buffer + 0x0004, 0, ID_LENGTH);
                        memcpy(tmp_buffer + 0x0004, &ezlopi_config_basic->id, ID_LENGTH);
                        TRACE_I("id: %lld", ezlopi_config_basic->id);
                    }

                    if (ESP_OK == esp_partition_erase_range(partition_ctx_v2, flash_offset, length))
                    {
                        if (ESP_OK == esp_partition_write(partition_ctx_v2, flash_offset, tmp_buffer, length))
                        {
                            ret = 1;
                        }
                    }
                }
                else
                {
                    TRACE_E("Couldn't fetch 'data' from id-flash-region!");
                }

                flash_offset = 0x0000;

                if (ESP_OK == esp_partition_read(partition_ctx_v2, flash_offset, tmp_buffer, length))
                {
                    if (0 != ezlopi_config_basic->config_version)
                    {
                        uint8_t config_version_arry[2] = {0};
                        config_version_arry[0] = (uint8_t)((ezlopi_config_basic->config_version & 0xFF00) >> 8);
                        config_version_arry[1] = (uint8_t)(ezlopi_config_basic->config_version & 0x00FF);
                        memset(tmp_buffer + 0x0002, 0, CONFIG_VERSION_LENGTH);
                        memcpy(tmp_buffer + 0x0002, config_version_arry, CONFIG_VERSION_LENGTH);
                        TRACE_I("Config Version: %d", ezlopi_config_basic->config_version);
                    }

                    if (NULL != ezlopi_config_basic->cloud_server)
                    {
                        // UPDATE_STRING_VALUE(tmp_buffer, ezlopi_config_basic->cloud_server, 0x0214, CLOUD_SERVER_LENGTH);
                        memset(tmp_buffer + 0x0214, 0, CLOUD_SERVER_LENGTH);
                        memcpy(tmp_buffer + 0x0214, ezlopi_config_basic->cloud_server, CLOUD_SERVER_LENGTH);
                        TRACE_I("cloud_server: %s", ezlopi_config_basic->cloud_server);
                    }
                    if (NULL != ezlopi_config_basic->prov_uuid)
                    {
                        // UPDATE_STRING_VALUE(tmp_buffer, ezlopi_config_basic->prov_uuid, 0x0314, PROVISIONING_UUID_LENGTH);
                        memset(tmp_buffer + 0x314, 0, PROVISIONING_UUID_LENGTH);
                        memcpy(tmp_buffer + 0x314, ezlopi_config_basic->prov_uuid, PROVISIONING_UUID_LENGTH);
                        TRACE_I("prov_uuid: %s", ezlopi_config_basic->prov_uuid);
                    }
                    if (NULL != ezlopi_config_basic->provision_server)
                    {
                        // UPDATE_STRING_VALUE(tmp_buffer, ezlopi_config_basic->provision_server, 0x0014, CLOUD_SERVER_LENGTH);
                        memset(tmp_buffer + 0x0014, 0, CLOUD_SERVER_LENGTH);
                        memcpy(tmp_buffer + 0x0014, ezlopi_config_basic->provision_server, CLOUD_SERVER_LENGTH);
                        TRACE_I("provision_server: %s", ezlopi_config_basic->provision_server);
                    }

                    if (NULL != ezlopi_config_basic->provision_token)
                    {
                        // UPDATE_STRING_VALUE(tmp_buffer, ezlopi_config_basic->provision_token, 0x0114, PROVISIONING_TOKEN_LENGTH + 1);
                        memset(tmp_buffer + 0x0114, 0, PROVISIONING_TOKEN_LENGTH);
                        memcpy(tmp_buffer + 0x0114, ezlopi_config_basic->provision_token, PROVISIONING_TOKEN_LENGTH);
                        TRACE_I("provision_token: %s", ezlopi_config_basic->provision_token);
                    }

                    if (ESP_OK == esp_partition_erase_range(partition_ctx_v2, flash_offset, length))
                    {
                        if (ESP_OK == esp_partition_write(partition_ctx_v2, flash_offset, tmp_buffer, length))
                        {
                            ret = (ret == 1) ? 1 : 0;
                        }
                    }
                }
                else
                {
                    TRACE_E("Couldn't fetch 'string' from id-flash-region!");
                }
                // ESP_LOG_BUFFER_HEXDUMP("PROV", tmp_buffer, length, ESP_LOG_INFO);
                free(tmp_buffer);
            }
#endif
        }
    }

    return ret;
}

int ezlopi_factory_info_v2_set_wifi(char *ssid, char *password)
{
    int ret = 0;

    if (ssid && password)
    {
        if (ezlopi_factory_info_v2_init())
        {
            uint32_t length = 4 * 1024;
            uint32_t hub_0_offset = 0xE000;

            char *tmp_buffer = (char *)malloc(length);
            if (tmp_buffer)
            {
                if (ESP_OK == esp_partition_read(partition_ctx_v2, hub_0_offset, tmp_buffer, length))
                {
                    snprintf(tmp_buffer + 0x0024, length, "%s", ssid);
                    snprintf(tmp_buffer + 0x0044, length, "%s", password);
                    if (ESP_OK == esp_partition_erase_range(partition_ctx_v2, hub_0_offset, length))
                    {
                        if (ESP_OK == esp_partition_write(partition_ctx_v2, hub_0_offset, tmp_buffer, length))
                        {
                            ret = 1;
                        }
                    }
                }
                else
                {
                    TRACE_E("Couldn't fetch 'string' from id-flash-region!");
                }

                free(tmp_buffer);
            }
            else
            {
                TRACE_E("'tmp_buffer' malloc failed!");
            }
        }
    }

    return ret;
}

int ezlopi_factory_info_v2_set_ssl_private_key(char *data)
{
    return ezlopi_factory_info_v2_set_4kb(ezlopi_ssl_private_key_str, data, SSL_PRIVATE_KEY_OFFSET);
}

int ezlopi_factory_info_v2_set_ssl_public_key(char *data)
{
    // return ezlopi_factory_info_v2_set_4kb("ssl-public-key", data, 0x4000);
    return 0;
}

int ezlopi_factory_info_v2_set_ssl_shared_key(char *data)
{
    return ezlopi_factory_info_v2_set_4kb("ssl-shared-key", data, SSL_SHARED_KEY_OFFSET);
}

int ezlopi_factory_info_v2_set_ca_cert(char *data)
{
    return ezlopi_factory_info_v2_set_4kb("ca-cert", data, CA_CERTIFICATE_OFFSET);
}

int ezlopi_factory_info_v2_set_ezlopi_config(char *data)
{
    int ret = ezlopi_factory_info_v2_set_4kb("ezlopi-config", data, EZLOPI_CONFIG_OFFSET);
    if (1 == ret)
    {
        ezlopi_factory_info_v2_free_ezlopi_config();
    }

    return ret;
}

static int ezlopi_factory_info_v2_set_4kb(char *name, char *data, uint32_t offset)
{
    int ret = 0;
    if (data)
    {
        if (ESP_OK == (ret = esp_partition_erase_range(partition_ctx_v2, offset, 0x1000)))
        {
            if (ESP_OK == (ret = esp_partition_write(partition_ctx_v2, offset, data, strlen(data) + 1)))
            {
                char *updated_data_from_flash = ezlopi_factory_info_v2_read_string(offset, 0x1000);
                if (updated_data_from_flash)
                {
                    free(updated_data_from_flash);
                }

                ret = 1;
            }
            else
            {
                TRACE_E("%s: esp-partition write failed!", name ? name : ezlopi__str);
            }
        }
        else
        {
            TRACE_E("%s: esp-partition erase failed!", name ? name : ezlopi__str);
        }
    }

    return ret;
}

int ezlopi_factory_info_v2_factory_reset(void)
{
    int ret = 0;
    if (ezlopi_factory_info_v2_init())
    {

        if (ESP_OK == esp_partition_erase_range(partition_ctx_v2, 0, EZLOPI_FACTORY_INFO_V2_PARTITION_SIZE))
        {
            ret = 1;
        }
    }

    return ret;
}

/** Free **/
void ezlopi_factory_info_v2_free_ezlopi_config(void)
{
    if (g_ezlopi_config)
    {
        free(g_ezlopi_config);
        g_ezlopi_config = NULL;
    }
}

/** Reader */
static char *ezlopi_factory_info_v2_read_string(e_ezlopi_factory_info_v2_offset_t offset, e_ezlopi_factory_info_v2_length_t length)
{
    char *read_string = NULL;

    if (ezlopi_factory_info_v2_init())
    {
        char *tmp_buffer = (char *)malloc(length);
        if (tmp_buffer)
        {
            if (ESP_OK == esp_partition_read(partition_ctx_v2, offset, tmp_buffer, length))
            {
                // dump("tmp_buffer", tmp_buffer, 0, length);
                // vTaskDelay(1);

                int s_length = (strlen(tmp_buffer) < length) ? strlen(tmp_buffer) : length;
                read_string = (char *)malloc(s_length + 1);
                if (NULL != read_string)
                {
                    snprintf(read_string, s_length + 1, "%s", tmp_buffer);
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

            free(tmp_buffer);
        }
        else
        {
            TRACE_E("'tmp_buffer' malloc failed!");
        }
    }

    return read_string;
}
