#include <string.h>
#include <ctype.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_factory_info.h"

#if (ID_BIN_VERSION_2 == ID_BIN_VERSION)
static char *g_ca_certificate = NULL;
static char *g_ssl_private_key = NULL;
static char *g_ssl_shared_key = NULL;
static char *g_ezlopi_config = NULL;
#endif

static const esp_partition_t *partition_ctx_v3 = NULL;
static uint32_t g_provisioning_status = 0;

static int ezlopi_factory_info_v3_set_4kb(const char *data, uint32_t offset)
{
    int ret = 0;
    if (data)
    {
        if (ESP_OK == esp_partition_erase_range(partition_ctx_v3, offset, EZLOPI_FINFO_READ_LEN_4KB))
        {
            if (ESP_OK == esp_partition_write(partition_ctx_v3, offset, data, strlen(data) + 1))
            {
                TRACE_I("Flash write succeessful");
                ret = 1;
            }
            else
            {
                TRACE_E("esp-partition write failed!");
            }
        }
        else
        {
            TRACE_E("esp-partition erase failed!");
        }
    }

    return ret;
}

uint32_t ezlopi_factory_info_v3_get_abs_address(uint32_t relative_offset, e_factory_info_v3_partition_type_t partition_type)
{
    if (partition_type == E_EZLOPI_FACTORY_INFO_CONN_DATA)
    {
        return (relative_offset + EZLOPI_FINFO_OFFSET_CONN_DATA);
    }
    else if (partition_type == E_EZLOPI_FACTORY_INFO_HUB_DATA)
    {
        return (relative_offset + EZLOPI_FINFO_OFFSET_HUB_DATA);
    }
    else
    {
        return 0;
    }
}

static char *ezlopi_factory_info_v3_read_string(e_ezlopi_factory_info_v3_offset_t offset, e_ezlopi_factory_info_v3_length_t length)
{
    char *read_string = NULL;

    if (ezlopi_factory_info_v3_init())
    {
        char *tmp_buffer = (char *)malloc(length);
        if (tmp_buffer)
        {
            if (ESP_OK == esp_partition_read(partition_ctx_v3, offset, tmp_buffer, length))
            {
                // dump("tmp_buffer", tmp_buffer, 0, length);
                // vTaskDelay(1);

                int s_length = (strlen(tmp_buffer) < length) ? strlen(tmp_buffer) : length;
                read_string = (char *)malloc(s_length + 1);
                if (NULL != read_string)
                {
                    if (1) // (isprint(tmp_buffer[0]))
                    {
                        snprintf(read_string, s_length + 1, "%s", tmp_buffer);
                    }
                    else
                    {
                        free(read_string);
                        read_string = NULL;
                    }
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

const esp_partition_t *ezlopi_factory_info_v3_init(void)
{
    if (NULL == partition_ctx_v3)
    {
        partition_ctx_v3 = esp_partition_find_first(EZLOPI_FACTORY_INFO_V3_PARTITION_TYPE, EZLOPI_FACTORY_INFO_V3_SUBTYPE, EZLOPI_FACTORY_INFO_V3_PARTITION_NAME);
    }
    return partition_ctx_v3;
}

void ezlopi_factory_info_v3_free(void *arg)
{
    if (arg)
    {
        free(arg);
        arg = NULL;
    }
}

void print_factory_info_v3(void)
{
    // uint16_t version = ezlopi_factory_info_v3_get_version();
    unsigned long long id = ezlopi_factory_info_v3_get_id();

    char *mac = ezlopi_factory_info_v3_get_ezlopi_mac();
    char *name = ezlopi_factory_info_v3_get_name();
    char *manufacturer = ezlopi_factory_info_v3_get_manufacturer();
    char *brand = ezlopi_factory_info_v3_get_brand();
    char *model = ezlopi_factory_info_v3_get_model();
    char *device_uuid = ezlopi_factory_info_v3_get_device_uuid();
    char *provisioning_uuid = ezlopi_factory_info_v3_get_provisioning_uuid();
    char *wifi_ssid = ezlopi_factory_info_v3_get_ssid();
    char *wifi_password = ezlopi_factory_info_v3_get_password();
    char *cloud_server = ezlopi_factory_info_v3_get_cloud_server();
    char *device_type = ezlopi_factory_info_v3_get_device_type();
    char *ca_certificate = ezlopi_factory_info_v3_get_ca_certificate();
    char *ssl_private_key = ezlopi_factory_info_v3_get_ssl_private_key();
    char *ssl_shared_key = ezlopi_factory_info_v3_get_ssl_shared_key();
#if (ID_BIN_VERSION_2 == ID_BIN_VERSION)
    char *ezlopi_config = ezlopi_factory_info_v2_get_ezlopi_config();
#elif (ID_BIN_VERSION_3 == ID_BIN_VERSION)
    char *ezlopi_config = ezlopi_factory_info_v3_get_ezlopi_config();
#elif (ID_BIN_VERSION_1 == ID_BIN_VERSION)
    char *ezlopi_config = ezlopi_nvs_read_config_data_str();
#endif

    TRACE_D("----------------- Factory Info -----------------");
    // TRACE_W("VERSION[off: 0x%04X, size: 0x%04X]:                %d", VERSION_OFFSET, VERSION_LENGTH, version);
    TRACE_W("SERIAL-ID [off: 0x%04X, size: 0x%04X]:             %llu", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_DEVICE_ID, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_DEVICE_ID, id);
    TRACE_W("MAC [off: 0x%04X, size: 0x%04X]:                   %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_DEVICE_MAC, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_DEVICE_MAC, mac ? mac : "null");
    TRACE_W("NAME [off: 0x%04X, size: 0x%04X]:                  %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_DEVICE_NAME, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_DEVICE_NAME, name ? name : "null");
    TRACE_W("MANUFACTURER [off: 0x%04X, size: 0x%04X]:          %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_MANUF_NAME, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_MANUF_NAME, manufacturer ? manufacturer : "null");
    TRACE_W("BRAND [off: 0x%04X, size: 0x%04X]:                 %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_BRAND_NAME, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_BRAND_NAME, brand ? brand : "null");
    TRACE_W("MODEL [off: 0x%04X, size: 0x%04X]:                 %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_MODEL_NAME, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_MODEL_NAME, model ? model : "null");
    TRACE_W("DEVICE_UUID [off: 0x%04X, size: 0x%04X]:           %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_DEVICE_UUID, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_DEVICE_UUID, device_uuid ? device_uuid : "null");
    TRACE_W("PROVISIONING_UUID [off: 0x%04X, size: 0x%04X]:     %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_PROVISIONING_UUID, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_PROV_UUID, provisioning_uuid ? provisioning_uuid : "null");
    TRACE_W("WIFI-SSID [off: 0x%04X, size: 0x%04X]:             %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_WIFI_SSID, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_WIFI_SSID, wifi_ssid ? wifi_ssid : "null");
    TRACE_W("WIFI-PASSWORD [off: 0x%04X, size: 0x%04X]:         %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_WIFI_PASS, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_WIFI_PASS, wifi_password ? wifi_password : "null");
    TRACE_W("CLOUD_SERVER [off: 0x%04X, size: 0x%04X]:          %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_CLOUD_SERVER_URL, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_CLOUD_SERVER_URL, cloud_server ? cloud_server : "null");
    TRACE_W("DEVICE_TYPE [off: 0x%04X, size: 0x%04X]:           %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_EZLOPI_DEVICE_TYPE, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_EZLOPI_DEVICE_TYPE, device_type ? device_type : "null");
    TRACE_W("CA_CERTIFICATE [off: 0x%04X, size: 0x%04X]:        %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_CA_CERTIFICATE, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_CA_CERTIFICATE, ca_certificate ? ca_certificate : "null");
    TRACE_W("SSL_PRIVATE_KEY [off: 0x%04X, size: 0x%04X]:       %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_PRIVATE_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_SSL_PRIVATE_KEY, ssl_private_key ? ssl_private_key : "null");
    TRACE_W("SSL_SHARED_KEY [off: 0x%04X, size: 0x%04X]:        %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_SHARED_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_SSL_SHARED_KEY, ssl_shared_key ? ssl_shared_key : "null");
    TRACE_W("EZLOPI_CONFIG [off: 0x%04X, size: 0x%04X]:         %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_EZLOPI_CONFIG_JSON, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_EZLOPI_CONFIG_JSON, ezlopi_config ? ezlopi_config : "null");
    ;

    TRACE_D("-------------------------------------------------");

    ezlopi_factory_info_v3_free(mac);
    ezlopi_factory_info_v3_free(name);
    ezlopi_factory_info_v3_free(manufacturer);
    ezlopi_factory_info_v3_free(brand);
    ezlopi_factory_info_v3_free(model);
    ezlopi_factory_info_v3_free(device_uuid);
    ezlopi_factory_info_v3_free(provisioning_uuid);
    ezlopi_factory_info_v3_free(wifi_ssid);
    ezlopi_factory_info_v3_free(wifi_password);
    ezlopi_factory_info_v3_free(cloud_server);
    ezlopi_factory_info_v3_free(ca_certificate);
    ezlopi_factory_info_v3_free(ssl_private_key);
    ezlopi_factory_info_v3_free(ssl_shared_key);
}

/** Getter */
uint32_t ezlopi_factory_info_v3_get_provisioning_status(void)
{
    return g_provisioning_status;
}

uint16_t ezlopi_factory_info_v3_get_version(void)
{
    uint16_t _version = 0ULL;

    if (ezlopi_factory_info_v3_init())
    {
        uint8_t tmp_version_arr[EZLOPI_FINFO_LEN_VERSION];
        memset(tmp_version_arr, 0, EZLOPI_FINFO_LEN_VERSION);
        int read_couont = esp_partition_read(partition_ctx_v3, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_FMW_VERSION, E_EZLOPI_FACTORY_INFO_HUB_DATA), &tmp_version_arr, EZLOPI_FINFO_LEN_VERSION);
        TRACE_B("read-count: %d", read_couont);

        for (int i = 0; i < 4; i++)
        {
            _version |= tmp_version_arr[i] << (i * 8);
        }
    }
    else
    {
        TRACE_E("'ezlopi_factory_info_v3' failed");
    }

    return _version;
}

char *ezlopi_factory_info_v3_get_name(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_DEVICE_NAME, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_DEVICE_NAME);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            free(read_data);
            read_data = NULL;
        }
    }
    return read_data;
    return read_data;
}

char *ezlopi_factory_info_v3_get_manufacturer(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_MANUF_NAME, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_MANUF_NAME);

    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            free(read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_v3_get_brand(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_BRAND_NAME, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_BRAND_NAME);

    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            free(read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_v3_get_model(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_MODEL_NAME, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_MODEL_NAME);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            free(read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_get_v3_provision_token(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_PROVISIONING_TOKEN, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_PROVISIONING_TOKEN);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            free(read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

uint16_t ezlopi_factory_info_v3_get_config_version(void)
{
    uint16_t config_version = 0;

    if (ezlopi_factory_info_v3_init())
    {
        uint8_t tmp_config_version_arr[2];
        memset(tmp_config_version_arr, 0, 2);
        esp_partition_read(partition_ctx_v3, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_CONFIG_VERSION, E_EZLOPI_FACTORY_INFO_CONN_DATA), tmp_config_version_arr, EZLOPI_FINFO_LEN_CONFIG_VERSION);
        config_version = (tmp_config_version_arr[0] << 8) | tmp_config_version_arr[1];
    }

    return config_version;
}

unsigned long long ezlopi_factory_info_v3_get_id(void)
{
    unsigned long long _id = 0ULL;

    if (ezlopi_factory_info_v3_init())
    {
        uint8_t tmp_id_arr[8];
        memset(tmp_id_arr, 0, 8);
        esp_partition_read(partition_ctx_v3, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_DEVICE_ID, E_EZLOPI_FACTORY_INFO_HUB_DATA), tmp_id_arr, EZLOPI_FINFO_LEN_DEVICE_ID);

        for (int i = 0; i < 8; i++)
        {
            _id |= tmp_id_arr[i] << (i * 8);
        }
    }

    return _id;
}

char *ezlopi_factory_info_v3_get_device_uuid(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_DEVICE_UUID, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_DEVICE_UUID);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            free(read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_v3_get_provisioning_uuid(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_PROVISIONING_UUID, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_PROV_UUID);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            free(read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_v3_get_ssid(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_WIFI_SSID, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_WIFI_SSID);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            free(read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_v3_get_password(void)
{

    char *read_data = ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_WIFI_PASS, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_WIFI_PASS);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            free(read_data);
            read_data = NULL;
        }
    }

    return read_data;
}

char *ezlopi_factory_info_v3_get_ezlopi_mac(void)
{

    char *read_data = ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_DEVICE_MAC, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_DEVICE_MAC);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            free(read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_v3_get_cloud_server(void)
{
    char *cloud_server = ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_CLOUD_SERVER_URL, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_CLOUD_SERVER_URL);
    if (cloud_server && strstr(cloud_server, "https://"))
    {
        g_provisioning_status = 1;
    }
    return cloud_server;
}

char *ezlopi_factory_info_v3_get_provisioning_server(void)
{
    char *provisioning_server = ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_PROVISIONING_SERVER_URL, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_PROVISIONING_SERVER_URL);
    if (provisioning_server && strstr(provisioning_server, "https://"))
    {
        return provisioning_server;
    }
    else
    {
        return NULL;
    }
}

char *ezlopi_factory_info_v3_get_device_type(void)
{
    return "ezlopi_generic";
}

char *ezlopi_factory_info_v3_get_ca_certificate(void)
{

    return ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_CA_CERTIFICATE, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_CA_CERTIFICATE);
}

char *ezlopi_factory_info_v3_get_ssl_private_key(void)
{
    return ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_PRIVATE_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_SSL_PRIVATE_KEY);
}

char *ezlopi_factory_info_v3_get_ssl_shared_key(void)
{

    return ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_SHARED_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_SSL_SHARED_KEY);
}

char *ezlopi_factory_info_v3_get_ezlopi_config(void)
{
    return ezlopi_factory_info_v3_read_string(ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_EZLOPI_CONFIG_JSON, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_EZLOPI_CONFIG_JSON);
}

#if 0 // IF version 2
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

int ezlopi_factory_info_v3_set_basic(s_basic_factory_info_t *ezlopi_config_basic)
{
    int ret = 0;

    if (ezlopi_config_basic)
    {
#if (ID_BIN_VERSION_1 == ID_BIN_VERSION)
        if (ezlopi_factory_info_v2_init())
        {
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
        }
#endif

#if (ID_BIN_VERSION_3 == ID_BIN_VERSION)
        if (ezlopi_factory_info_v3_init())
        {
            char *tmp_buffer = (char *)malloc(EZLOPI_FINFO_READ_LEN_4KB);
            if (tmp_buffer)
            {
                if (ESP_OK == esp_partition_read(partition_ctx_v3, EZLOPI_FINFO_OFFSET_HUB_DATA, tmp_buffer, EZLOPI_FINFO_READ_LEN_4KB))
                {
                    if (NULL != ezlopi_config_basic->device_name)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_NAME, 0, EZLOPI_FINFO_LEN_DEVICE_NAME);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_NAME, ezlopi_config_basic->device_name, EZLOPI_FINFO_LEN_DEVICE_NAME);
                        TRACE_I("device_name: %s", ezlopi_config_basic->device_name);
                    }

                    if (NULL != ezlopi_config_basic->manufacturer)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_MANUF_NAME, 0, EZLOPI_FINFO_LEN_MANUF_NAME);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_MANUF_NAME, ezlopi_config_basic->manufacturer, EZLOPI_FINFO_LEN_MANUF_NAME);
                        TRACE_I("manufacturer: %s", ezlopi_config_basic->manufacturer);
                    }

                    if (NULL != ezlopi_config_basic->brand)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_BRAND_NAME, 0, EZLOPI_FINFO_LEN_BRAND_NAME);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_BRAND_NAME, ezlopi_config_basic->brand, EZLOPI_FINFO_LEN_BRAND_NAME);
                        TRACE_I("brand: %s", ezlopi_config_basic->brand);
                    }
                    if (NULL != ezlopi_config_basic->model_number)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_MODEL_NAME, 0, EZLOPI_FINFO_LEN_MODEL_NAME);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_MODEL_NAME, ezlopi_config_basic->model_number, EZLOPI_FINFO_LEN_MODEL_NAME);
                        TRACE_I("model_number: %s", ezlopi_config_basic->model_number);
                    }
                    if (NULL != ezlopi_config_basic->device_uuid)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_UUID, 0, EZLOPI_FINFO_LEN_DEVICE_UUID);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_UUID, ezlopi_config_basic->device_uuid, EZLOPI_FINFO_LEN_DEVICE_UUID);
                        TRACE_I("device_uuid: %s", ezlopi_config_basic->device_uuid);
                    }
                    if (NULL != ezlopi_config_basic->device_type)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_EZLOPI_DEVICE_TYPE, 0, EZLOPI_FINFO_LEN_EZLOPI_DEVICE_TYPE);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_EZLOPI_DEVICE_TYPE, ezlopi_config_basic->device_type, EZLOPI_FINFO_LEN_EZLOPI_DEVICE_TYPE);
                        TRACE_I("device_type: %s", ezlopi_config_basic->device_type);
                    }

                    if (NULL != ezlopi_config_basic->device_mac)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_MAC, 0, EZLOPI_FINFO_LEN_DEVICE_MAC);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_MAC, ezlopi_config_basic->device_mac, EZLOPI_FINFO_LEN_DEVICE_MAC);
                        TRACE_I("EzloPi Device MAC: %s", ezlopi_config_basic->device_mac);
                    }

                    if (0 != ezlopi_config_basic->id)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_ID, 0, EZLOPI_FINFO_LEN_DEVICE_ID);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_ID, &ezlopi_config_basic->id, EZLOPI_FINFO_LEN_DEVICE_ID);
                        TRACE_I("EzloPi Device ID: %lld", ezlopi_config_basic->id);
                    }

                    if (ESP_OK == esp_partition_erase_range(partition_ctx_v3, EZLOPI_FINFO_OFFSET_HUB_DATA, EZLOPI_FINFO_READ_LEN_4KB))
                    {
                        if (ESP_OK == esp_partition_write(partition_ctx_v3, EZLOPI_FINFO_OFFSET_HUB_DATA, tmp_buffer, EZLOPI_FINFO_READ_LEN_4KB))
                        {
                            ret = 1;
                        }
                    }
                }
                else
                {
                    TRACE_E("Couldn't fetch 'data' from id-flash-region!");
                }

                if (ESP_OK == esp_partition_read(partition_ctx_v3, EZLOPI_FINFO_OFFSET_CONN_DATA, tmp_buffer, EZLOPI_FINFO_READ_LEN_4KB))
                {
                    if (0 != ezlopi_config_basic->config_version)
                    {
                        uint8_t config_version_arry[2] = {0};
                        config_version_arry[0] = (uint8_t)((ezlopi_config_basic->config_version & 0xFF00) >> 8);
                        config_version_arry[1] = (uint8_t)(ezlopi_config_basic->config_version & 0x00FF);
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_CONFIG_VERSION, 0, EZLOPI_FINFO_LEN_CONFIG_VERSION);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_CONFIG_VERSION, config_version_arry, EZLOPI_FINFO_LEN_CONFIG_VERSION);
                        TRACE_I("Config Version: %d", ezlopi_config_basic->config_version);
                    }

                    if (NULL != ezlopi_config_basic->cloud_server)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_CLOUD_SERVER_URL, 0, EZLOPI_FINFO_LEN_CLOUD_SERVER_URL);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_CLOUD_SERVER_URL, ezlopi_config_basic->cloud_server, EZLOPI_FINFO_LEN_CLOUD_SERVER_URL);
                        TRACE_I("cloud_server: %s", ezlopi_config_basic->cloud_server);
                    }
                    if (NULL != ezlopi_config_basic->prov_uuid)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_PROVISIONING_UUID, 0, EZLOPI_FINFO_LEN_PROV_UUID);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_PROVISIONING_UUID, ezlopi_config_basic->prov_uuid, EZLOPI_FINFO_LEN_PROV_UUID);
                        TRACE_I("prov_uuid: %s", ezlopi_config_basic->prov_uuid);
                    }
                    if (NULL != ezlopi_config_basic->provision_server)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_PROVISIONING_SERVER_URL, 0, EZLOPI_FINFO_LEN_PROVISIONING_SERVER_URL);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_PROVISIONING_SERVER_URL, ezlopi_config_basic->provision_server, EZLOPI_FINFO_LEN_PROVISIONING_SERVER_URL);
                        TRACE_I("provision_server: %s", ezlopi_config_basic->provision_server);
                    }

                    if (NULL != ezlopi_config_basic->provision_token)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_PROVISIONING_TOKEN, 0, EZLOPI_FINFO_LEN_PROVISIONING_TOKEN);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_PROVISIONING_TOKEN, ezlopi_config_basic->provision_token, EZLOPI_FINFO_LEN_PROVISIONING_TOKEN);
                        TRACE_I("provision_token: %s", ezlopi_config_basic->provision_token);
                    }

                    if (ESP_OK == esp_partition_erase_range(partition_ctx_v3, EZLOPI_FINFO_OFFSET_CONN_DATA, EZLOPI_FINFO_READ_LEN_4KB))
                    {
                        if (ESP_OK == esp_partition_write(partition_ctx_v3, EZLOPI_FINFO_OFFSET_CONN_DATA, tmp_buffer, EZLOPI_FINFO_READ_LEN_4KB))
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
        }
#endif
    }
    return ret;
}

int ezlopi_factory_info_v3_set_wifi(char *ssid, char *password)
{
    int ret = 0;

    if (ssid && password)
    {
        if (ezlopi_factory_info_v3_init())
        {

            char *tmp_buffer = (char *)malloc(EZLOPI_FINFO_READ_LEN_4KB);
            if (tmp_buffer)
            {
                if (ESP_OK == esp_partition_read(partition_ctx_v3, EZLOPI_FINFO_OFFSET_HUB_DATA, tmp_buffer, EZLOPI_FINFO_READ_LEN_4KB))
                {
                    snprintf(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_WIFI_SSID, EZLOPI_FINFO_LEN_WIFI_SSID, "%s", ssid);
                    snprintf(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_WIFI_PASS, EZLOPI_FINFO_LEN_WIFI_PASS, "%s", password);
                    if (ESP_OK == esp_partition_erase_range(partition_ctx_v3, EZLOPI_FINFO_OFFSET_HUB_DATA, EZLOPI_FINFO_READ_LEN_4KB))
                    {
                        if (ESP_OK == esp_partition_write(partition_ctx_v3, EZLOPI_FINFO_OFFSET_HUB_DATA, tmp_buffer, EZLOPI_FINFO_READ_LEN_4KB))
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

int ezlopi_factory_info_v3_set_ssl_private_key(const char *data)
{
    return ezlopi_factory_info_v3_set_4kb(data, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_PRIVATE_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA));
}

// int ezlopi_factory_info_v3_set_ssl_public_key(char *data)
// {
//     return ezlopi_factory_info_v3_set_4kb(data, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_SHARED_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA));
// }

int ezlopi_factory_info_v3_set_ssl_shared_key(const char *data)
{
    return ezlopi_factory_info_v3_set_4kb(data, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_SHARED_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA));
}

int ezlopi_factory_info_v3_set_ca_cert(const char *data)
{
    return ezlopi_factory_info_v3_set_4kb(data, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_CA_CERTIFICATE, E_EZLOPI_FACTORY_INFO_CONN_DATA));
}

int ezlopi_factory_info_v3_set_ezlopi_config(const char *data)
{
    return ezlopi_factory_info_v3_set_4kb(data, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_EZLOPI_CONFIG_JSON, E_EZLOPI_FACTORY_INFO_CONN_DATA));
}

int ezlopi_factory_info_v3_factory_reset(void)
{
    int ret = 0;
    if (ezlopi_factory_info_v3_init())
    {

        if (ESP_OK == esp_partition_erase_range(partition_ctx_v3, 0, EZLOPI_FACTORY_INFO_V3_PARTITION_SIZE))
        {
            ret = 1;
        }
    }

    return ret;
}

#if 0
static int ezlopi_factory_info_v2_set_4kb(char *data, uint32_t offset);
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

#endif
