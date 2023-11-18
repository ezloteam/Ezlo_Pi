#include "string.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_partition.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "trace.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_nvs.h"


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

static int ezlopi_factory_info_v2_set_4kb(char *data, uint32_t offset);
static char *ezlopi_factory_info_v2_read_string(e_ezlopi_factory_info_v2_offset_t offset, e_ezlopi_factory_info_v2_length_t length);

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
#if (ID_BIN_VERSION_2 == ID_BIN_VERSION)
    char *ezlopi_config = ezlopi_factory_info_v2_get_ezlopi_config();
#elif (ID_BIN_VERSION_1 == ID_BIN_VERSION)
    char *ezlopi_config = ezlopi_nvs_read_config_data_str();
#endif

    TRACE_D("----------------- Factory Info -----------------");
    // TRACE_W("VERSION[off: 0x%04X, size: 0x%04X]:                %d", VERSION_OFFSET, VERSION_LENGTH, version);
    TRACE_W("SERIAL-ID [off: 0x%04X, size: 0x%04X]:             %llu", ID_OFFSET, ID_LENGTH, id);
    // TRACE_W("MAC [off: 0x%04X, size: 0x%04X]:                   %02X:%02X:%02X:%02X:%02X:%02X", DEVICE_MAC_OFFSET, DEVICE_MAC_LENGTH, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    TRACE_W("NAME [off: 0x%04X, size: 0x%04X]:                  %s", NAME_OFFSET, NAME_LENGTH, name ? name : "null");
    // TRACE_W("MANUFACTURER [off: 0x%04X, size: 0x%04X]:          %s", MANUFACTURER_OFFSET, MANUFACTURER_LENGTH, manufacturer ? manufacturer : "null");
    // TRACE_W("BRAND [off: 0x%04X, size: 0x%04X]:                 %s", BRAND_OFFSET, BRAND_LENGTH, brand ? brand : "null");
    // TRACE_W("MODEL [off: 0x%04X, size: 0x%04X]:                 %s", MODEL_OFFSET, MODEL_LENGTH, model ? model : "null");
    TRACE_W("DEVICE_UUID [off: 0x%04X, size: 0x%04X]:           %s", DEVICE_UUID_OFFSET, DEVICE_UUID_LENGTH, device_uuid ? device_uuid : "null");
    // TRACE_W("PROVISIONING_UUID [off: 0x%04X, size: 0x%04X]:     %s", PROVISIONING_UUID_OFFSET, PROVISIONING_UUID_LENGTH, provisioning_uuid ? provisioning_uuid : "null");
    TRACE_W("WIFI-SSID [off: 0x%04X, size: 0x%04X]:             %s", SSID_OFFSET, SSID_LENGTH, wifi_ssid ? wifi_ssid : "null");
    // TRACE_W("WIFI-PASSWORD [off: 0x%04X, size: 0x%04X]:         %s", PASSWORD_OFFSET, PASSWORD_LENGTH, wifi_password ? wifi_password : "null");
    // TRACE_W("CLOUD_SERVER [off: 0x%04X, size: 0x%04X]:          %s", CLOUD_SERVER_OFFSET, CLOUD_SERVER_LENGTH, cloud_server);
    // TRACE_W("DEVICE_TYPE [off: 0x%04X, size: 0x%04X]:           %s", DEVICE_TYPE_OFFSET, DEVICE_TYPE_LENGTH, device_type);
    // TRACE_W("CA_CERTIFICATE [off: 0x%04X, size: 0x%04X]:        %s", CA_CERTIFICATE_OFFSET, CA_CERTIFICATE_LENGTH, ca_certificate);
    // TRACE_W("SSL_PRIVATE_KEY [off: 0x%04X, size: 0x%04X]:       %s", SSL_PRIVATE_KEY_OFFSET, SSL_PRIVATE_KEY_LENGTH, ssl_private_key);
    // TRACE_W("SSL_SHARED_KEY [off: 0x%04X, size: 0x%04X]:        %s", SSL_SHARED_KEY_OFFSET, SSL_SHARED_KEY_LENGTH, ssl_shared_key);
#if (ID_BIN_VERSION_2 == ID_BIN_VERSION)
    TRACE_W("EZLOPI_CONFIG [off: 0x%04X, size: 0x%04X]:         %s", EZLOPI_CONFIG_OFFSET, EZLOPI_CONFIG_LENGTH, ezlopi_config);
#elif (ID_BIN_VERSION_1 == ID_BIN_VERSION)
    // TRACE_W("EZLOPI_CONFIG:                                     %s", ezlopi_config ? ezlopi_config : "");
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
    // ezlopi_factory_info_v2_free(device_type);
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
        return ezlopi_device_type_str[EZLOPI_DEVICE_TYPE_GENERIC];
    }
    case EZLOPI_DEVICE_TYPE_GENERIC:
    case EZLOPI_DEVICE_TYPE_SWITCH_BOX:
    case EZLOPI_DEVICE_TYPE_IR_BLASTER:
    case EZLOPI_DEVICE_TYPE_SOUND_SENSOR:
    case EZLOPI_DEVICE_TYPE_AMBIENT_TRACKER_PRO:
    {
        ret = ezlopi_device_type_str[EZLOPI_DEVICE_TYPE];
    }
    default:
    {
        ret = undefined;
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

#if 0
/** Setter */
uint16_t ezlopi_factory_info_v2_set_version(void)
{
    uint16_t _version = 0ULL;

    if (ezlopi_factory_info_v2_init())
    {
        uint8_t tmp_version_arr[2];
        memset(tmp_version_arr, 0, 2);
        esp_partition_read(partition_ctx_v2, VERSION_OFFSET, &tmp_version_arr, VERSION_LENGTH);

        for (int i = 0; i < 8; i++)
        {
            _version |= tmp_version_arr[i] << (i * 8);
        }
    }

    return _version;
}

char *ezlopi_factory_info_v2_set_name(void)
{
    return ezlopi_factory_info_v2_read_string(NAME_OFFSET, NAME_LENGTH);
}

char *ezlopi_factory_info_v2_set_manufacturer(void)
{
    return ezlopi_factory_info_v2_read_string(MANUFACTURER_OFFSET, MANUFACTURER_LENGTH);
}

char *ezlopi_factory_info_v2_set_brand(void)
{
    return ezlopi_factory_info_v2_read_string(BRAND_OFFSET, BRAND_LENGTH);
}

char *ezlopi_factory_info_v2_set_model(void)
{
    return ezlopi_factory_info_v2_read_string(MODEL_OFFSET, MODEL_LENGTH);
}

unsigned long long ezlopi_factory_info_v2_set_id(void)
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

char *ezlopi_factory_info_v2_set_device_uuid(void)
{
    return ezlopi_factory_info_v2_read_string(DEVICE_UUID_OFFSET, DEVICE_UUID_LENGTH);
}

char *ezlopi_factory_info_v2_set_provisioning_uuid(void)
{
    return ezlopi_factory_info_v2_read_string(PROVISIONING_UUID_OFFSET, PROVISIONING_UUID_LENGTH);
}

char *ezlopi_factory_info_v2_set_ssid(void)
{
    return ezlopi_factory_info_v2_read_string(SSID_OFFSET, SSID_LENGTH);
}

char *ezlopi_factory_info_v2_set_password(void)
{
    return ezlopi_factory_info_v2_read_string(PASSWORD_OFFSET, PASSWORD_LENGTH);
}

uint8_t *ezlopi_factory_info_v2_set_ezlopi_mac(void)
{
    uint8_t *tmp_mac_arr = malloc(DEVICE_MAC_LENGTH);

    if (tmp_mac_arr)
    {
        if (ezlopi_factory_info_v2_init())
        {
            memset(tmp_mac_arr, 0, DEVICE_MAC_LENGTH);
            esp_partition_read(partition_ctx_v2, DEVICE_MAC_OFFSET, &tmp_mac_arr, DEVICE_MAC_LENGTH);
        }
    }

    return tmp_mac_arr;
}

char *ezlopi_factory_info_v2_set_cloud_server(void)
{
    return ezlopi_factory_info_v2_read_string(CLOUD_SERVER_OFFSET, CLOUD_SERVER_LENGTH);
}

char *ezlopi_factory_info_v2_set_ca_certificate(void)
{
    return ezlopi_factory_info_v2_read_string(CA_CERTIFICATE_OFFSET, CA_CERTIFICATE_LENGTH);
}

char *ezlopi_factory_info_v2_set_ssl_private_key(void)
{
    return ezlopi_factory_info_v2_read_string(SSL_PRIVATE_KEY_OFFSET, SSL_PRIVATE_KEY_LENGTH);
}

char *ezlopi_factory_info_v2_set_ssl_shared_key(void)
{
    return ezlopi_factory_info_v2_read_string(SSL_SHARED_KEY_OFFSET, SSL_SHARED_KEY_LENGTH);
}
#endif

#define UPDATE_STRING_VALUE(buffer, data, offset, length)  \
    {                                                      \
        if (data)                                          \
        {                                                  \
            snprintf(buffer + offset, length, "%s", data); \
        }                                                  \
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
    return ezlopi_factory_info_v2_set_4kb(data, 0x4000);
}

int ezlopi_factory_info_v2_set_ssl_public_key(char *data)
{
    return ezlopi_factory_info_v2_set_4kb(data, 0x4000);
}

int ezlopi_factory_info_v2_set_ssl_shared_key(char *data)
{
    return ezlopi_factory_info_v2_set_4kb(data, 0x5000);
}

int ezlopi_factory_info_v2_set_ca_cert(char *data)
{
    return ezlopi_factory_info_v2_set_4kb(data, 0x3000);
}

int ezlopi_factory_info_v2_set_ezlopi_config(char *data)
{
    return ezlopi_factory_info_v2_set_4kb(data, 0x1000);
}

static int ezlopi_factory_info_v2_set_4kb(char *data, uint32_t offset)
{
    int ret = 0;
    if (data)
    {
        if (ESP_OK == esp_partition_erase_range(partition_ctx_v2, offset, 0x1000))
        {
            if (ESP_OK == esp_partition_write(partition_ctx_v2, offset, data, strlen(data) + 1))
            {
                ret = 1;
            }
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

#if 0
/**********************************************************************************/

s_ezlopi_factory_info_t *ezlopi_factory_info_get_info(void)
{
    if (NULL == factory_info)
    {
        ezlopi_factory_info_init();
    }

    return factory_info;
}

static void ezlopi_factory_info_print(void)
{
    if (factory_info_v2)
    {
        TRACE_D("Version [off: %d, len: %d]: %llu", ID_OFFSET, sizeof(long long), factory_info_v2->basic.version);
        TRACE_D("id [off: %d, len: %d]: %llu", ID_OFFSET, sizeof(long long), factory_info_v2->id);
        PRINT_FACTORY_INFO("controller_uuid", UUID_OFFSET, factory_info_v2->controller_uuid);
        PRINT_FACTORY_INFO("zwave_region", ZWAVE_REGION_OFFSET, factory_info_v2->zwave_region);
        PRINT_FACTORY_INFO("default_wifi_ssid", WIFI_SSID_OFFSET, factory_info_v2->default_wifi_ssid);
        PRINT_FACTORY_INFO("default_wifi_password", WIFI_PASSWORD_OFFSET, factory_info_v2->default_wifi_password);
        PRINT_FACTORY_INFO("product_name", PRODUCT_NAME_OFFSET, factory_info_v2->product_name);
        TRACE_D("ezlopi_mac [off: %d, len: %d]: %02x:%02x:%02x:%02x:%02x:%02x", EZLOPI_MAC_OFFSET, EZLOPI_MAC_LENGTH,
                factory_info_v2->ezlopi_mac[5], factory_info_v2->ezlopi_mac[4], factory_info_v2->ezlopi_mac[3], factory_info_v2->ezlopi_mac[2], factory_info_v2->ezlopi_mac[1], factory_info_v2->ezlopi_mac[0]);
        PRINT_FACTORY_INFO("ezlopi_manufacturer", MANUFACTURER_OFFSET, factory_info_v2->ezlopi_manufacturer);
        PRINT_FACTORY_INFO("ezlopi_brand", MANUFACTURER_OFFSET, factory_info_v2->ezlopi_brand);
        PRINT_FACTORY_INFO("ezlopi_model", MANUFACTURER_OFFSET, factory_info_v2->ezlopi_model);
        PRINT_FACTORY_INFO("ezlopi_device_type", MANUFACTURER_OFFSET, factory_info_v2->ezlopi_device_type);

        PRINT_FACTORY_INFO("provisioning_uuid", PROVISIONING_UUID_OFFSET, factory_info_v2->provisioning_uuid);
        PRINT_FACTORY_INFO("provisioning_server", PROVISIONING_SERVER_OFFSET, factory_info_v2->provisioning_server);
        PRINT_FACTORY_INFO("provisioning_token", PROVISIONING_TOKEN_OFFSET, factory_info_v2->provisioning_token);
        PRINT_FACTORY_INFO("cloud_server", CLOUD_SERVER_OFFSET, factory_info_v2->cloud_server);
        PRINT_FACTORY_INFO("ezlopi_config", CLOUD_SERVER_OFFSET, factory_info_v2->ezlopi_config);
        PRINT_FACTORY_INFO("ca_certificate", CA_CERTIFICATE_OFFSET, factory_info_v2->ca_certificate);
        PRINT_FACTORY_INFO("ssl_private_key", SSL_PRIVATE_KEY_OFFSET, factory_info_v2->ssl_private_key);
        PRINT_FACTORY_INFO("ssl_shared_key", SSL_SHARED_KEY_OFFSET, factory_info_v2->ssl_shared_key);
    }
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

#endif
