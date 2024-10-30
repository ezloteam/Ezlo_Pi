#include <string.h>
#include <ctype.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_errors.h"
#include "EZLOPI_USER_CONFIG.h"

#define UPDATE_STRING_VALUE(buffer, data, offset, length)  \
    {                                                      \
        if (data)                                          \
        {                                                  \
            snprintf(buffer + offset, length, "%s", data); \
        }                                                  \
    }

static uint32_t g_provisioning_status = 0;
static const esp_partition_t *partition_ctx_v3 = NULL;

/* Should not free, Since these informations are used by web-socket-client and are used to reconnect. */
/* will help decrease heap-fragmentation */
static char *__ca_cert = NULL;
static char *__pvt_key = NULL;
static char *__pub_key = NULL;

int ezlopi_calculate_certificate_length(const char *certificate_string, const char *end_marker)
{
    char *end_pos = strstr(certificate_string, end_marker);
    if (end_pos != NULL)
    {
        int length_up_to_end_marker = (end_pos - certificate_string) + strlen(end_marker);
        return length_up_to_end_marker;
    }
    else
    {
        return 0;
    }
}

static void ezlopi_replace_newline_escape(char *str)
{
    int i = 0, j = 0;

    while (str[i] != '\0')
    {
        if (str[i] == '\\' && str[i + 1] == 'n')
        {
            str[j++] = '\n'; // Replace "\\n" with '\n'
            i += 2;          // Skip past the '\\' and 'n'
        }
        else
        {
            str[j++] = str[i++];
        }
    }

    str[j] = '\0';
}

static int ezlopi_factory_info_v3_set_4kb(const char *data, uint32_t offset, uint32_t len)
{
    int ret = 0;
    char *tmp_variable = ezlopi_malloc(__FUNCTION__, len + 1);

    if (tmp_variable)
    {
        snprintf(tmp_variable, len + 1, "%.*s", len, data);

        if (ESP_OK == esp_partition_erase_range(partition_ctx_v3, offset, EZLOPI_FINFO_READ_LEN_4KB))
        {
            if (ESP_OK == esp_partition_write(partition_ctx_v3, offset, tmp_variable, len + 1))
            {
                TRACE_S("Flash write successful");
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

        ezlopi_free(__FUNCTION__, tmp_variable);
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

static char *ezlopi_factory_info_v3_read_string(const char *name, e_ezlopi_factory_info_v3_offset_t offset, e_ezlopi_factory_info_v3_length_t length)
{
    char *read_string = NULL;

    if (ezlopi_factory_info_v3_init())
    {
        // TRACE_D("%s", name);
        char *tmp_buffer = (char *)ezlopi_malloc(name, length);
        if (tmp_buffer)
        {
            if (ESP_OK == esp_partition_read(partition_ctx_v3, offset, tmp_buffer, length))
            {
                int s_length = (strlen(tmp_buffer) < length) ? strlen(tmp_buffer) : length;
                read_string = (char *)ezlopi_malloc(name, s_length + 1);

                if (NULL != read_string)
                {
                    if (1) // (isprint(tmp_buffer[0]))
                    {
                        snprintf(read_string, s_length + 1, "%s", tmp_buffer);
                    }
                    else
                    {
                        ezlopi_free(__FUNCTION__, read_string);
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

            // dump("tmp_buffer", tmp_buffer, 0, length);
            ezlopi_free(__FUNCTION__, tmp_buffer);
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
        ezlopi_free(__FUNCTION__, arg);
        arg = NULL;
    }
}

void print_factory_info_v3(void)
{
#if (1 == ENABLE_TRACE)
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
    char *provisioning_token = ezlopi_factory_info_get_v3_provision_token();
    // const char* provision_server = ezlopi_factory_info_v3_get_provisioning_server();
    const char *device_type = ezlopi_factory_info_v3_get_device_type();
    char *ca_certificate = ezlopi_factory_info_v3_get_ca_certificate();
    char *ssl_private_key = ezlopi_factory_info_v3_get_ssl_private_key();
    char *ssl_shared_key = ezlopi_factory_info_v3_get_ssl_shared_key();
    char *local_key = ezlopi_factory_info_v3_get_local_key();
    // char* ezlopi_config = ezlopi_factory_info_v3_get_ezlopi_config();

    TRACE_W("----------------- Factory Info -----------------");
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
    // TRACE_W("WIFI-PASSWORD [off: 0x%04X, size: 0x%04X]:         %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_WIFI_PASS, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_WIFI_PASS, wifi_password ? wifi_password : "null");
    // TRACE_W("PROVISIONING-TOKEN [off: 0x%04X, size: 0x%04X]:    %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_PROVISIONING_TOKEN, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_PROVISIONING_TOKEN, provisioning_token ? provisioning_token : "null");
    TRACE_W("CLOUD_SERVER [off: 0x%04X, size: 0x%04X]:          %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_CLOUD_SERVER_URL, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_CLOUD_SERVER_URL, cloud_server ? cloud_server : "null");
    TRACE_W("DEVICE_TYPE [off: 0x%04X, size: 0x%04X]:           %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_EZLOPI_DEVICE_TYPE, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_EZLOPI_DEVICE_TYPE, device_type ? device_type : "null");
    TRACE_W("LOCAL_KEY [off: 0x%04X, size: 0x%04X]:           %s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_LOCAL_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_LOCAL_KEY, local_key ? local_key : "null");
    // TRACE_W("CA_CERTIFICATE [off: 0x%04X, size: 0x%04X]:        \n%s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_CA_CERTIFICATE, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_CA_CERTIFICATE, ca_certificate ? ca_certificate : "null");
    // TRACE_W("SSL_PRIVATE_KEY [off: 0x%04X, size: 0x%04X]:       \n%s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_PRIVATE_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_SSL_PRIVATE_KEY, ssl_private_key ? ssl_private_key : "null");
    // TRACE_W("SSL_SHARED_KEY [off: 0x%04X, size: 0x%04X]:        \n%s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_SHARED_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_SSL_SHARED_KEY, ssl_shared_key ? ssl_shared_key : "null");
    // TRACE_W("EZLOPI_CONFIG [off: 0x%04X, size: 0x%04X]:         \n%s", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_EZLOPI_CONFIG_JSON, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_EZLOPI_CONFIG_JSON, ezlopi_config ? ezlopi_config : "null");
    TRACE_W("-------------------------------------------------");

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
    ezlopi_factory_info_v3_free(provisioning_token);
    ezlopi_factory_info_v3_free(local_key);
    // ezlopi_factory_info_v3_free(ca_certificate); // allocated once for all, do not free
    // ezlopi_factory_info_v3_free(ssl_private_key); // allocated once for all, do not free
    // ezlopi_factory_info_v3_free(ssl_shared_key); // allocated once for all, do not free
    // ezlopi_factory_info_v3_free(ezlopi_config);
#endif
}

/** Getter */
uint32_t ezlopi_factory_info_v3_get_provisioning_status(void)
{
    if (ezlopi_factory_info_v3_init())
    {
        char *cloud_server = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_CLOUD_SERVER_URL, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_CLOUD_SERVER_URL);
        if (cloud_server && strstr(cloud_server, "https://"))
        {
            g_provisioning_status = 1;
            ezlopi_free(__FUNCTION__, cloud_server);
        }
        else
        {
            g_provisioning_status = 0;
        }
    }

    return g_provisioning_status;
}

uint16_t ezlopi_factory_info_v3_get_version(void)
{
    uint16_t _version = 0ULL;

    if (ezlopi_factory_info_v3_init())
    {
        uint8_t tmp_version_arr[EZLOPI_FINFO_LEN_VERSION];
        memset(tmp_version_arr, 0, EZLOPI_FINFO_LEN_VERSION);
        esp_partition_read(partition_ctx_v3, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_FMW_VERSION, E_EZLOPI_FACTORY_INFO_HUB_DATA), &tmp_version_arr, EZLOPI_FINFO_LEN_VERSION);

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
    char *read_data = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_DEVICE_NAME, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_DEVICE_NAME);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            ezlopi_free(__FUNCTION__, read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_v3_get_manufacturer(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_MANUF_NAME, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_MANUF_NAME);

    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            ezlopi_free(__FUNCTION__, read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_v3_get_brand(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_BRAND_NAME, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_BRAND_NAME);

    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            ezlopi_free(__FUNCTION__, read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_v3_get_model(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_MODEL_NAME, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_MODEL_NAME);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            ezlopi_free(__FUNCTION__, read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_get_v3_provision_token(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_PROVISIONING_TOKEN, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_PROVISIONING_TOKEN);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            ezlopi_free(__FUNCTION__, read_data);
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

    if (ezlopi_factory_info_v3_get_provisioning_status() != 1)
    {
        return 0;
    }

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
    else
    {
        TRACE_E("Factory info un-initialized!");
    }

    return _id;
}

char *ezlopi_factory_info_v3_get_device_uuid(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_DEVICE_UUID, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_DEVICE_UUID);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            ezlopi_free(__FUNCTION__, read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_v3_get_provisioning_uuid(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_PROVISIONING_UUID, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_PROV_UUID);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            ezlopi_free(__FUNCTION__, read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_v3_get_ssid(void)
{
    char *read_data = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_WIFI_SSID, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_WIFI_SSID);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            ezlopi_free(__FUNCTION__, read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_v3_get_password(void)
{

    char *read_data = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_WIFI_PASS, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_WIFI_PASS);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            ezlopi_free(__FUNCTION__, read_data);
            read_data = NULL;
        }
    }

    return read_data;
}

char *ezlopi_factory_info_v3_get_ezlopi_mac(void)
{

    char *read_data = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_DEVICE_MAC, E_EZLOPI_FACTORY_INFO_HUB_DATA), EZLOPI_FINFO_LEN_DEVICE_MAC);
    if (read_data)
    {
        if (!isprint(read_data[0]))
        {
            ezlopi_free(__FUNCTION__, read_data);
            read_data = NULL;
        }
    }
    return read_data;
}

char *ezlopi_factory_info_v3_get_cloud_server(void)
{
    char *cloud_server = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_CLOUD_SERVER_URL, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_CLOUD_SERVER_URL);
    if (cloud_server && strstr(cloud_server, "https://"))
    {
        g_provisioning_status = 1;
        return cloud_server;
    }
    else
    {
        ezlopi_free(__FUNCTION__, cloud_server);
        return NULL;
    }
}

char *ezlopi_factory_info_v3_get_local_key(void)
{
    char *local_key = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_LOCAL_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_LOCAL_KEY);

    if (local_key)
    {
        if (!isprint(local_key[0]))
        {
            ezlopi_free(__FUNCTION__, local_key);
            local_key = NULL;
        }
    }
    return local_key;
}

char *ezlopi_factory_info_v3_get_provisioning_server(void)
{
    char *provisioning_server = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_PROVISIONING_SERVER_URL, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_PROVISIONING_SERVER_URL);
    if (provisioning_server && strstr(provisioning_server, "https://"))
    {
        return provisioning_server;
    }
    else
    {
        ezlopi_free(__FUNCTION__, provisioning_server);
        return NULL;
    }
}

const char *ezlopi_factory_info_v3_get_device_type(void)
{
    return "ezlopi_generic";
}

void ezlopi_factory_info_v3_free_ca_certificate(void)
{
    if (__ca_cert)
    {
        ezlopi_free(__FUNCTION__, __ca_cert);
        __ca_cert = NULL;
    }
}
char *ezlopi_factory_info_v3_get_ca_certificate(void)
{
    if (NULL == __ca_cert)
    {
        __ca_cert = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_CA_CERTIFICATE, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_CA_CERTIFICATE);
        ezlopi_replace_newline_escape(__ca_cert); // Workaround : Escape Sequence "\n"
    }
    return __ca_cert;
}

void ezlopi_factory_info_v3_free_ssl_private_key(void)
{
    if (__pvt_key)
    {
        ezlopi_free(__FUNCTION__, __pvt_key);
        __pvt_key = NULL;
    }
}
char *ezlopi_factory_info_v3_get_ssl_private_key(void)
{
    if (NULL == __pvt_key)
    {
        __pvt_key = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_PRIVATE_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_SSL_PRIVATE_KEY);
        ezlopi_replace_newline_escape(__pvt_key); // Workaround : Escape Sequence "\n"
    }
    return __pvt_key;
}

void ezlopi_factory_info_v3_free_ssl_shared_key(void)
{
    if (__pub_key)
    {
        ezlopi_free(__FUNCTION__, __pub_key);
        __pub_key = NULL;
    }
}
char *ezlopi_factory_info_v3_get_ssl_shared_key(void)
{
    if (NULL == __pub_key)
    {
        __pub_key = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_SHARED_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_SSL_SHARED_KEY);
        ezlopi_replace_newline_escape(__pub_key); // Workaround : Escape Sequence "\n"
    }
    return __pub_key;
}

char *ezlopi_factory_info_v3_get_ezlopi_config(void)
{
    // TRACE_E("ezlopi-config address: %d", ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_EZLOPI_CONFIG_JSON, E_EZLOPI_FACTORY_INFO_CONN_DATA));
    char *ret = ezlopi_factory_info_v3_read_string(__FUNCTION__, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_EZLOPI_CONFIG_JSON, E_EZLOPI_FACTORY_INFO_CONN_DATA), EZLOPI_FINFO_LEN_EZLOPI_CONFIG_JSON);
    if (false == isprint(ret[0]))
    {
        if (ret)
        {
            ezlopi_free(__FUNCTION__, ret);
            ret = NULL;
        }
    }
    return ret;
}

int ezlopi_factory_info_v3_set_basic(s_basic_factory_info_t *ezlopi_config_basic)
{
    int ret = 0;

    if (ezlopi_config_basic)
    {
        if (ezlopi_factory_info_v3_init())
        {
            char *tmp_buffer = (char *)ezlopi_malloc(__FUNCTION__, EZLOPI_FINFO_READ_LEN_4KB);
            if (tmp_buffer)
            {
                if (ESP_OK == esp_partition_read(partition_ctx_v3, EZLOPI_FINFO_OFFSET_HUB_DATA, tmp_buffer, EZLOPI_FINFO_READ_LEN_4KB))
                {
                    if (NULL != ezlopi_config_basic->device_name)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_NAME, 0, EZLOPI_FINFO_LEN_DEVICE_NAME);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_NAME, ezlopi_config_basic->device_name, EZLOPI_FINFO_LEN_DEVICE_NAME);
                        TRACE_S("device_name: %s", ezlopi_config_basic->device_name);
                    }

                    if (NULL != ezlopi_config_basic->manufacturer)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_MANUF_NAME, 0, EZLOPI_FINFO_LEN_MANUF_NAME);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_MANUF_NAME, ezlopi_config_basic->manufacturer, EZLOPI_FINFO_LEN_MANUF_NAME);
                        TRACE_S("manufacturer: %s", ezlopi_config_basic->manufacturer);
                    }

                    if (NULL != ezlopi_config_basic->brand)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_BRAND_NAME, 0, EZLOPI_FINFO_LEN_BRAND_NAME);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_BRAND_NAME, ezlopi_config_basic->brand, EZLOPI_FINFO_LEN_BRAND_NAME);
                        TRACE_S("brand: %s", ezlopi_config_basic->brand);
                    }
                    if (NULL != ezlopi_config_basic->model_number)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_MODEL_NAME, 0, EZLOPI_FINFO_LEN_MODEL_NAME);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_MODEL_NAME, ezlopi_config_basic->model_number, EZLOPI_FINFO_LEN_MODEL_NAME);
                        TRACE_S("model_number: %s", ezlopi_config_basic->model_number);
                    }
                    if (NULL != ezlopi_config_basic->device_type)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_EZLOPI_DEVICE_TYPE, 0, EZLOPI_FINFO_LEN_EZLOPI_DEVICE_TYPE);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_EZLOPI_DEVICE_TYPE, ezlopi_config_basic->device_type, EZLOPI_FINFO_LEN_EZLOPI_DEVICE_TYPE);
                        TRACE_S("device_type: %s", ezlopi_config_basic->device_type);
                    }

                    if (NULL != ezlopi_config_basic->device_mac)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_MAC, 0, EZLOPI_FINFO_LEN_DEVICE_MAC);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_MAC, ezlopi_config_basic->device_mac, EZLOPI_FINFO_LEN_DEVICE_MAC);
                        TRACE_S("EzloPi Device MAC: %s", ezlopi_config_basic->device_mac);
                    }

                    if (0 != ezlopi_config_basic->id)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_ID, 0, EZLOPI_FINFO_LEN_DEVICE_ID);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_ID, &ezlopi_config_basic->id, EZLOPI_FINFO_LEN_DEVICE_ID);
                        TRACE_S("EzloPi Device ID: %lld", ezlopi_config_basic->id);
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
                        TRACE_S("Config Version: %d", ezlopi_config_basic->config_version);
                    }

                    if (NULL != ezlopi_config_basic->local_key)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_LOCAL_KEY, 0, EZLOPI_FINFO_LEN_LOCAL_KEY);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_LOCAL_KEY, ezlopi_config_basic->local_key, EZLOPI_FINFO_LEN_LOCAL_KEY);
                        TRACE_S("local_key: %s", ezlopi_config_basic->local_key);
                    }

                    if (NULL != ezlopi_config_basic->cloud_server)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_CLOUD_SERVER_URL, 0, EZLOPI_FINFO_LEN_CLOUD_SERVER_URL);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_CLOUD_SERVER_URL, ezlopi_config_basic->cloud_server, EZLOPI_FINFO_LEN_CLOUD_SERVER_URL);
                        TRACE_S("cloud_server: %s", ezlopi_config_basic->cloud_server);
                    }
                    if (NULL != ezlopi_config_basic->device_uuid)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_UUID, 0, EZLOPI_FINFO_LEN_DEVICE_UUID);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_DEVICE_UUID, ezlopi_config_basic->device_uuid, EZLOPI_FINFO_LEN_DEVICE_UUID);
                        TRACE_S("device_uuid: %s", ezlopi_config_basic->device_uuid);
                    }
                    if (NULL != ezlopi_config_basic->prov_uuid)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_PROVISIONING_UUID, 0, EZLOPI_FINFO_LEN_PROV_UUID);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_PROVISIONING_UUID, ezlopi_config_basic->prov_uuid, EZLOPI_FINFO_LEN_PROV_UUID);
                        TRACE_S("prov_uuid: %s", ezlopi_config_basic->prov_uuid);
                    }
                    if (NULL != ezlopi_config_basic->provision_server)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_PROVISIONING_SERVER_URL, 0, EZLOPI_FINFO_LEN_PROVISIONING_SERVER_URL);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_PROVISIONING_SERVER_URL, ezlopi_config_basic->provision_server, EZLOPI_FINFO_LEN_PROVISIONING_SERVER_URL);
                        TRACE_S("provision_server: %s", ezlopi_config_basic->provision_server);
                    }

                    if (NULL != ezlopi_config_basic->provision_token)
                    {
                        memset(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_PROVISIONING_TOKEN, 0, EZLOPI_FINFO_LEN_PROVISIONING_TOKEN);
                        memcpy(tmp_buffer + EZLOPI_FINFO_REL_OFFSET_PROVISIONING_TOKEN, ezlopi_config_basic->provision_token, EZLOPI_FINFO_LEN_PROVISIONING_TOKEN);
                        TRACE_S("provision_token: %s", ezlopi_config_basic->provision_token);
                    }

                    if (ESP_OK == esp_partition_erase_range(partition_ctx_v3, EZLOPI_FINFO_OFFSET_CONN_DATA, EZLOPI_FINFO_READ_LEN_4KB))
                    {
                        if (ESP_OK == esp_partition_write(partition_ctx_v3, EZLOPI_FINFO_OFFSET_CONN_DATA, tmp_buffer, EZLOPI_FINFO_READ_LEN_4KB))
                        {
                            ret = (ret == 1) ? 1 : 0;
                        }
                        else
                        {
                            ret = 0;
                        }
                    }
                    else
                    {
                        ret = 0;
                    }
                }
                else
                {
                    TRACE_E("Couldn't fetch 'string' from id-flash-region!");
                }
                // ESP_LOG_BUFFER_HEXDUMP("PROV", tmp_buffer, length, ESP_LOG_INFO);
                ezlopi_free(__FUNCTION__, tmp_buffer);
            }
        }
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

            char *tmp_buffer = (char *)ezlopi_malloc(__FUNCTION__, EZLOPI_FINFO_READ_LEN_4KB);
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
                    else
                    {
                        TRACE_E("failed to save wifi");
                    }
                }
                else
                {
                    TRACE_E("Couldn't fetch 'string' from id-flash-region!");
                }

                ezlopi_free(__FUNCTION__, tmp_buffer);
            }
            else
            {
                TRACE_E("'tmp_buffer' malloc failed!");
            }
        }
    }

    return ret;
}

int ezlopi_factory_info_v3_set_ssl_private_key(cJSON *cj_data)
{
    int ret = 0;
    if (cj_data && cj_data->valuestring && cj_data->str_value_len)
    {
        cj_data->str_value_len = ezlopi_calculate_certificate_length(cj_data->valuestring, "-----END PRIVATE KEY-----"); // Workaround : cJSON is not giving correct length
        // TRACE_E("%.*s", cj_data->str_value_len, cj_data->valuestring);
        ret = ezlopi_factory_info_v3_set_4kb(cj_data->valuestring, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_PRIVATE_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA), cj_data->str_value_len);
    }
    return ret;
}

int ezlopi_factory_info_v3_set_ssl_public_key(cJSON *cj_data)
{
    int ret = 0;
    if (cj_data && cj_data->valuestring && cj_data->str_value_len)
    {
        cj_data->str_value_len = ezlopi_calculate_certificate_length(cj_data->valuestring, "-----END PUBLIC KEY-----"); // Workaround : cJSON is not giving correct length
        // TRACE_E("%.*s", cj_data->str_value_len, cj_data->valuestring);
        ret = ezlopi_factory_info_v3_set_4kb(cj_data->valuestring, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_SHARED_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA), cj_data->str_value_len);
    }
    return ret;
}

int ezlopi_factory_info_v3_set_ssl_shared_key(cJSON *cj_data)
{
    int ret = 0;
    if (cj_data && cj_data->valuestring && cj_data->str_value_len)
    {
        cj_data->str_value_len = ezlopi_calculate_certificate_length(cj_data->valuestring, "-----END CERTIFICATE-----"); // Workaround : cJSON is not giving correct length
        // TRACE_E("%.*s", cj_data->str_value_len, cj_data->valuestring);
        ret = ezlopi_factory_info_v3_set_4kb(cj_data->valuestring, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_SSL_SHARED_KEY, E_EZLOPI_FACTORY_INFO_CONN_DATA), cj_data->str_value_len);
    }
    return ret;
}

int ezlopi_factory_info_v3_set_ca_cert(cJSON *cj_data)
{
    int ret = 0;
    if (cj_data && cj_data->valuestring && cj_data->str_value_len)
    {
        cj_data->str_value_len = ezlopi_calculate_certificate_length(cj_data->valuestring, "-----END CERTIFICATE-----"); // Workaround : cJSON is not giving correct length
        // TRACE_E("%.*s", cj_data->str_value_len, cj_data->valuestring);
        ret = ezlopi_factory_info_v3_set_4kb(cj_data->valuestring, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_CA_CERTIFICATE, E_EZLOPI_FACTORY_INFO_CONN_DATA), cj_data->str_value_len);
    }
    return ret;
}

int ezlopi_factory_info_v3_set_ezlopi_config(cJSON *cj_data)
{
    int ret = 0;
    if (cj_data)
    {
        char *tmp_data = ezlopi_malloc(__FUNCTION__, 4 * 1024);
        if (tmp_data)
        {
            if (cJSON_PrintPreallocated(__FUNCTION__, cj_data, tmp_data, 4 * 1024, false))
            {
                // TRACE_D("added-chipset: %.*s", tmp_data);
                ret = ezlopi_factory_info_v3_set_4kb(tmp_data, ezlopi_factory_info_v3_get_abs_address(EZLOPI_FINFO_REL_OFFSET_EZLOPI_CONFIG_JSON, E_EZLOPI_FACTORY_INFO_CONN_DATA), strlen(tmp_data) + 1);
            }

            ezlopi_free(__FUNCTION__, tmp_data);
        }
    }

    return ret;
}

ezlopi_error_t ezlopi_factory_info_v3_factory_reset(void)
{
    ezlopi_error_t error = EZPI_FAILED;
    if (ezlopi_factory_info_v3_init())
    {

        if (ESP_OK == esp_partition_erase_range(partition_ctx_v3, 0, EZLOPI_FACTORY_INFO_V3_PARTITION_SIZE))
        {
            error = EZPI_SUCCESS;
        }
    }

    return error;
}

int ezlopi_factory_info_v3_scenes_factory_soft_reset(void)
{
    int ret = 0;
    if (ezlopi_factory_info_v3_set_wifi("ezlopitest", "ezlopitest"))
    {
        ret = 1;
    }
    else
    {
        TRACE_E("Could not activate factory/soft reset");
    }
    return ret;
}