#include <string>
#include <cstring>
#include <iostream>
#include "esp_log.h"
#include "factory_info.h"
#include "esp_system.h"
#include "frozen.h"
#include "debug.h"

using namespace std;
static unsigned long long factory_serial_number = 0;
factory_info *factory_info::factory_info_ = nullptr;

extern "C" unsigned long long factory_info_serial_number_c(void)
{
    return factory_serial_number;
}

factory_info *factory_info::get_instance(void)
{
    if (factory_info_ == nullptr)
    {
        factory_info_ = new factory_info();
        factory_info_->init();
        factory_serial_number = factory_info_->id;
    }
    return factory_info_;
}

void factory_info::set_default_if_not_found(void)
{
    if (((NULL == provisioning_server) || ('\0' == provisioning_server[0])) ||
        ((NULL == provisioning_token) || ('\0' == provisioning_token[0])) ||
        ((NULL == cloud_server) || ('\0' == cloud_server[0])) ||
        ((NULL == ca_certificate) || (NULL == strstr(ca_certificate, "-----BEGIN CERTIFICATE-----"))) ||
        ((NULL == ssl_private_key) || (NULL == strstr(ssl_private_key, "-----BEGIN PRIVATE KEY-----"))) ||
        ((NULL == ssl_shared_key) || (NULL == strstr(ssl_shared_key, "-----BEGIN CERTIFICATE-----"))))
    {
        id = 100004005ULL;
        controller_uuid = "53f5bdc0-5347-11ec-b2d6-8f260f5287fa";
        zwave_region = "US";
        default_wifi_ssid = "nepadldigisys";
        default_wifi_password = "NDS_0ffice";
        name = "ezlopi-101";
        provisioning_server = "https://req-disp-at0m.mios.com";
        provisioning_token = "7ed0d422e075fda2d05f2b46acbb9d503f3505577055542f97a195268b3fe8a769eae18bbfe25abba8f98d0866d86a1a3336ecc68eed8a294c0f3bb9c521a176750b20ceb37354caed130a15ea29eb882d84e71476c6fcd1fa59b1bbd49487c8aa965a16706b68e4d08112a58cfe28d2b10b7cad40ddcab6a2d9ebd81c1d0eea";
        cloud_server = "https://cloud.ezlo.com:7000";
        ca_certificate =
            "-----BEGIN CERTIFICATE-----\r\n"
            "MIICbDCCAhGgAwIBAgIJAOByzaI7aHY9MAoGCCqGSM49BAMDMIGQMQswCQYDVQQG\r\n"
            "EwJVUzEUMBIGA1UECAwLIE5ldyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xDzAN\r\n"
            "BgNVBAoMBklUIE9wczEPMA0GA1UECwwGSVQgT3BzMRQwEgYDVQQDDAtlWkxPIExU\r\n"
            "RCBDQTEhMB8GCSqGSIb3DQEJARYSc3lzYWRtaW5zQGV6bG8uY29tMCAXDTE5MDUz\r\n"
            "MTE3MDE0N1oYDzIxMTkwNTA3MTcwMTQ3WjCBkDELMAkGA1UEBhMCVVMxFDASBgNV\r\n"
            "BAgMCyBOZXcgSmVyc2V5MRAwDgYDVQQHDAdDbGlmdG9uMQ8wDQYDVQQKDAZJVCBP\r\n"
            "cHMxDzANBgNVBAsMBklUIE9wczEUMBIGA1UEAwwLZVpMTyBMVEQgQ0ExITAfBgkq\r\n"
            "hkiG9w0BCQEWEnN5c2FkbWluc0BlemxvLmNvbTBWMBAGByqGSM49AgEGBSuBBAAK\r\n"
            "A0IABHLQdhLDYsafIFY8pZh96aDGqVm6E4r8nW9s4CfdpXaa/R4CnjaVpDQI7UmQ\r\n"
            "9vVDGZn8mcmm7VjKx+TSCS0MIKOjUzBRMB0GA1UdDgQWBBRiTl8Ez1l94jaqcxbi\r\n"
            "yxkVC0FkBTAfBgNVHSMEGDAWgBRiTl8Ez1l94jaqcxbiyxkVC0FkBTAPBgNVHRMB\r\n"
            "Af8EBTADAQH/MAoGCCqGSM49BAMDA0kAMEYCIQD7EUs8j50jKFd/46Zo95NbrPYQ\r\n"
            "PtLTHH9YjUkMEkYD5gIhAMP4y7E1aB78nQrmd3IX8MM32k9dM8xT0MztR16OtsuV\r\n"
            "-----END CERTIFICATE-----";

        ssl_private_key =
            "-----BEGIN PRIVATE KEY-----\r\n"
            "MIGEAgEAMBAGByqGSM49AgEGBSuBBAAKBG0wawIBAQQg/2cQ79U/nLXvov+J4Kpi\r\n"
            "lO4qO88X0HbRmHhvRNKMiJ+hRANCAATYtFGFMTEqaO18wxhqnBBJy1ckbgDAaBGV\r\n"
            "SVBju06op4irOXNv7xcnjPqBryAvk862yDnwsUsIwEjtKVZC11sU\r\n"
            "-----END PRIVATE KEY-----";

        ssl_shared_key =
            "-----BEGIN CERTIFICATE-----\r\n"
            "MIICDDCCAbKgAwIBAgIDAy6fMAoGCCqGSM49BAMCMIGQMQswCQYDVQQGEwJVUzEU\r\n"
            "MBIGA1UECAwLIE5ldyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xDzANBgNVBAoM\r\n"
            "BklUIE9wczEPMA0GA1UECwwGSVQgT3BzMRQwEgYDVQQDDAtlWkxPIExURCBDQTEh\r\n"
            "MB8GCSqGSIb3DQEJARYSc3lzYWRtaW5zQGV6bG8uY29tMCAXDTIxMTIwMjA4MTEw\r\n"
            "MVoYDzIyOTUwOTE2MDgxMTAxWjCBjDELMAkGA1UEBhMCVVMxEzARBgNVBAgMCk5l\r\n"
            "dyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xEzARBgNVBAoMCmNvbnRyb2xsZXIx\r\n"
            "LTArBgNVBAsMJDYzNTFjNzUwLTUzNDctMTFlYy1iMmQ2LThmMjYwZjUyODdmYTES\r\n"
            "MBAGA1UEAwwJMTAwMDA0MDA1MFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAE2LRRhTEx\r\n"
            "KmjtfMMYapwQSctXJG4AwGgRlUlQY7tOqKeIqzlzb+8XJ4z6ga8gL5POtsg58LFL\r\n"
            "CMBI7SlWQtdbFDAKBggqhkjOPQQDAgNIADBFAiEApGxLFOsnD7rbUmvY2sJvmlKp\r\n"
            "Ixl5rgMrOQH6uKoQxa0CIFfN69EKILiT6R7HnEw76DZwkSwdmX/xpkjvN2bXJiuA\r\n"
            "-----END CERTIFICATE-----";

        ssl_public_key = NULL;
    }
    else
    {
        TRACE_I("** Found 'factory_info'.");
    }
}
int factory_info::init(void)
{
    int ret = 0;
    // partition_ctx = NULL;
    partition_ctx = esp_partition_find_first(static_cast<esp_partition_type_t>(FACTORY_INFO_PARTITION_TYPE), FACTORY_INFO_PARTITION_SUBTYPE, FACTORY_INFO_PARTITION_NAME);
    if (partition_ctx != NULL)
    {
        TRACE_I("Partition found '%s' at offset '0x%x' with size '0x%x'",
                partition_ctx->label, partition_ctx->address, partition_ctx->size);

        id = get_id();
        controller_uuid = get_uuid();
        zwave_region = get_zwave_region();
        default_wifi_ssid = get_default_wifi_ssid();
        default_wifi_password = get_default_wifi_password();
        name = get_name();
        cloud_server = get_cloud_server();
        provisioning_server = get_provisioning_server();
        provisioning_token = get_provisioning_token();
        ca_certificate = get_ca_certificate();
        ssl_private_key = get_ssl_private_key();
        ssl_shared_key = get_ssl_shared_key();

        TRACE_D("id[off: %d, len: %d]: %llu\r\n", ID_OFFSET, sizeof(long long), id);
        TRACE_D("controller_uuid[off:%d, len: %d]: \n%s\r\n", UUID_OFFSET, strlen(controller_uuid), controller_uuid);
        TRACE_D("zwave_region[off:%d, len: %d]: \n%s\r\n", ZWAVE_REGION_OFFSET, strlen(zwave_region), zwave_region);
        TRACE_D("name[off:%d, len: %d]: \n%s\r\n", PRODUCT_NAME_OFFSET, strlen(name), name);
        TRACE_D("cloud_server[off:%d, len: %d]: \n%s\r\n", CLOUD_SERVER_OFFSET, strlen(cloud_server), cloud_server);
        TRACE_D("provisioning_server[off:%d, len: %d]: \n%s\r\n", PROVISIONING_SERVER_OFFSET, strlen(provisioning_server), provisioning_server);
        TRACE_D("provisioning_token[off:%d, len: %d]: \n%s\r\n", PROVISIONING_TOKEN_OFFSET, strlen(provisioning_token), provisioning_token);
        TRACE_D("ca_certificate[off:%d, len: %d]: \n%s\r\n", CA_CERTIFICATE_OFFSET, strlen(ca_certificate), ca_certificate);
        TRACE_D("ssl_private_key[off:%d, len: %d]: \n%s\r\n", SSL_PRIVATE_KEY_OFFSET, strlen(ssl_private_key), ssl_private_key);
        TRACE_D("ssl_shared_key[off:%d, len: %d]: \n%s\r\n", SSL_SHARED_KEY_OFFSET, strlen(ssl_shared_key), ssl_shared_key);
    }
    else
    {
        TRACE_E("Partition \"id\" not found!!");
    }

    set_default_if_not_found();

    return ret;
}

string factory_info::update_factory_info(char *payload, int len)
{
    string ret = "";
    static const char *key_list[] = {
        "default_wifi_ssid",
        "default_wifi_password",
        "product_name",
        "cloud_server",
        "token",
    };

    for (int idx = 0; idx < sizeof(key_list) / sizeof(char *); idx++)
    {
        struct json_token j_token = {NULL, 0, JSON_TYPE_INVALID};
        if (json_scanf(payload, len, "{%s: %T}", key_list[idx], &j_token))
        {
            TRACE_D("{'%s': '%.*s'}\r\n", key_list[idx], j_token.len, j_token.ptr);
        }
    }

    return ret;
}

char *factory_info::read_string_from_partition(int offset, int length)
{
    char *ret = NULL;
    if (partition_ctx)
    {
        char *buffer = (char *)malloc(length);
        if (buffer)
        {
            if (ESP_OK == esp_partition_read(partition_ctx, offset, buffer, length))
            {
                int s_length = (strlen(buffer) < length) ? strlen(buffer) : length;
                ret = (char *)malloc(s_length + 1);
                if (NULL != ret)
                {
                    snprintf(ret, s_length + 1, "%s", buffer);
                }
            }
            else
            {
                TRACE_E("Couldn't fetch 'wifi_password' from factory_info!");
            }
            free(buffer);
        }
    }

    return ret;
}

unsigned long long factory_info::get_id(void)
{
    unsigned long long tmp_id = 0ULL;
    // esp_partition_read(partition_ctx, HUB_INFO_0_OFFSET + ID_OFFSET, &tmp_id, ID_LENGTH);

    uint8_t tmp_id_arr[8];
    long long unsigned int id_parsed = 0LLU;
    memset(tmp_id_arr, 0, 8);
    esp_partition_read(partition_ctx, HUB_INFO_0_OFFSET + ID_OFFSET, &tmp_id_arr, ID_LENGTH);

    for (int i = 0; i < 8; i++)
    {
        tmp_id |= tmp_id_arr[i] << (i * 8);
    }
    return tmp_id; // read_string_from_partition(ID_OFFSET, ID_LENGTH);
}
char *factory_info::get_uuid(void)
{
    char *ret = NULL;
    if (partition_ctx)
    {
        ret = (char *)malloc(40);
        if (ret)
        {
            char *u_buf = (char *)malloc(UUID_LENGTH);
            if (u_buf)
            {
                if (ESP_OK == esp_partition_read(partition_ctx, HUB_INFO_0_SIZE + UUID_OFFSET, u_buf, UUID_LENGTH))
                {
                    if (ret)
                    {
                        snprintf(ret, 40, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                                 u_buf[0], u_buf[1], u_buf[2], u_buf[3], u_buf[4], u_buf[5], u_buf[6], u_buf[7],
                                 u_buf[8], u_buf[9], u_buf[10], u_buf[11], u_buf[12], u_buf[13], u_buf[14], u_buf[15]);
                    }
                }
                else
                {
                    TRACE_E("Couldn't fetch 'wifi_password' from factory_info!");
                }
                free(u_buf);
            }

            trace_imp("UUID from flash: %s", ret);
        }
        else
        {
            trace_err("UUID malloc failed!");
        }
    }
    return ret;
}
char *factory_info::get_zwave_region(void)
{
    return read_string_from_partition(HUB_INFO_0_OFFSET + ZWAVE_REGION_OFFSET, ZWAVE_REGION_LENGTH);
}
char *factory_info::get_default_wifi_ssid(void)
{
    return read_string_from_partition(HUB_INFO_0_OFFSET + WIFI_SSID_OFFSET, WIFI_SSID_LENGTH);
}
char *factory_info::get_default_wifi_password(void)
{
    return read_string_from_partition(HUB_INFO_0_OFFSET + WIFI_PASSWORD_OFFSET, WIFI_PASSWORD_LENGTH);
}
char *factory_info::get_name(void)
{
    return read_string_from_partition(HUB_INFO_0_OFFSET + PRODUCT_NAME_OFFSET, PRODUCT_NAME_LENGTH);
}
char *factory_info::get_provisioning_server(void)
{
    return read_string_from_partition(CONNECTION_INFO_0_OFFSET + PROVISIONING_SERVER_OFFSET, PROVISIONING_SERVER_LENGTH);
}
char *factory_info::get_provisioning_token(void)
{
    return read_string_from_partition(CONNECTION_INFO_0_OFFSET + PROVISIONING_TOKEN_OFFSET, PROVISIONING_TOKEN_LENGTH);
}
char *factory_info::get_cloud_server(void)
{
    return read_string_from_partition(CONNECTION_INFO_0_OFFSET + CLOUD_SERVER_OFFSET, CLOUD_SERVER_LENGTH);
}
char *factory_info::get_ca_certificate(void)
{
    return read_string_from_partition(CONNECTION_INFO_0_OFFSET + CA_CERTIFICATE_OFFSET, CA_CERTIFICATE_LENGTH);
}
char *factory_info::get_ssl_private_key(void)
{
    return read_string_from_partition(CONNECTION_INFO_0_OFFSET + SSL_PRIVATE_KEY_OFFSET, SSL_PRIVATE_KEY_LENGTH);
}
char *factory_info::get_ssl_shared_key(void)
{
    return read_string_from_partition(CONNECTION_INFO_0_OFFSET + SSL_SHARED_KEY_OFFSET, SSL_SHARED_KEY_LENGTH);
}

int factory_info::write_string_to_flash(int offset, int size, struct json_token *token)
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
int factory_info::set_id(char *payload, int len)
{
    int ret = 0;
    struct json_token id_tok = JSON_INVALID_TOKEN;
    if (json_scanf(payload, len, "{id: %T}", &id_tok))
    {
        string id_str = "";
        id_str.assign(id_tok.ptr, id_tok.len);
        id = stol(id_str, nullptr, 10);
        if (ESP_OK == esp_partition_write(partition_ctx, ID_OFFSET, (void *)&id, ID_LENGTH))
        {
            ret = 1;
        }
    }
    return ret;
}
int factory_info::set_uuid(char *payload, int len)
{
    int ret = 0;
    struct json_token uuid_tok = JSON_INVALID_TOKEN;
    if (json_scanf(payload, len, "{uuid: %T}", &uuid_tok))
    {
        string uuid_str = "";
        uuid_str.assign(uuid_tok.ptr, uuid_tok.len);
        uint8_t uuid_byte[16];
        // string_to_uuid();
    }
    return ret;
}
int factory_info::set_zwave_region(char *payload, int len)
{
    int ret = 0;

    return ret;
}
int factory_info::set_default_wifi_ssid(char *payload, int len)
{
    int ret = 0;
    return ret;
}
int factory_info::set_default_wifi_password(char *payload, int len)
{
    int ret = 0;
    return ret;
}
int factory_info::set_name(char *payload, int len)
{
    int ret = 0;
    return ret;
}
int factory_info::set_provisioning_server(char *payload, int len)
{
    int ret = 0;
    return ret;
}
int factory_info::set_provisioning_token(char *payload, int len)
{
    int ret = 0;
    return ret;
}
int factory_info::set_cloud_server(char *payload, int len)
{
    int ret = 0;
    return ret;
}
int factory_info::set_ca_certificate(char *payload, int len)
{
    int ret = 0;
    return ret;
}
int factory_info::set_ssl_private_key(char *payload, int len)
{
    int ret = 0;
    return ret;
}
int factory_info::set_ssl_shared_key(char *payload, int len)
{
    int ret = 0;
    return ret;
}

#if 0
string factory_info::get_dev_id(void)
{
    std::string ret = "";

    uint8_t base_mac[6];
    esp_read_mac(base_mac, ESP_MAC_WIFI_STA);
    ret += "EZPI01-"; // [type][version]
    for (auto &_m : base_mac)
    {
        ret += hex_chars[(_m >> 4) & 0x0F];
        ret += hex_chars[_m & 0x0F];
    }

    return ret;
}
string factory_info::get_default_wifi_ssid(void)
{
    string ret = "";

    if (NULL != partition_ctx)
    {
        char *ssid = (char *)malloc(WIFI_SSID_LENGTH);
        if (ssid)
        {
            if (ESP_OK == esp_partition_read(partition_ctx, WIFI_SSID_OFFSET, ssid, WIFI_SSID_LENGTH))
            {
                ret.assign(ssid, WIFI_SSID_LENGTH);
            }
            else
            {
                TRACE_E("Couldn't fetch 'wifi_ssid' from factory_info!");
            }
            free(ssid);
        }
    }

    return ret;
}
string factory_info::get_default_wifi_password(void)
{
    string ret = "";

    if (NULL != partition_ctx)
    {
        char *password = (char *)malloc(WIFI_PASSWORD_LENGTH);
        if (password)
        {
            if (ESP_OK == esp_partition_read(partition_ctx, WIFI_PASSWORD_OFFSET, password, WIFI_PASSWORD_LENGTH))
            {
                ret.assign(password, WIFI_PASSWORD_LENGTH);
            }
            else
            {
                TRACE_E("Couldn't fetch 'wifi_password' from factory_info!");
            }
            free(password);
        }
    }

    return ret;
}
string factory_info::get_product_name(void)
{
    string ret = "";

    if (NULL != partition_ctx)
    {
        char *product_name = (char *)malloc(PRODUCT_NAME_LENGTH);
        if (product_name)
        {
            if (ESP_OK == esp_partition_read(partition_ctx, PRODUCT_NAME_OFFSET, product_name, PRODUCT_NAME_LENGTH))
            {
                ret.assign(product_name, PRODUCT_NAME_LENGTH);
            }
            else
            {
                TRACE_E("Couldn't fetch 'product_name' from factory_info!");
            }
            free(product_name);
        }
    }

    return ret;
}
string factory_info::get_cloud_server(void)
{
    string ret = "";

    if (NULL != partition_ctx)
    {
        char *cloud_server = (char *)malloc(CLOUD_SERVER_LENGTH);
        if (cloud_server)
        {
            if (ESP_OK == esp_partition_read(partition_ctx, CLOUD_SERVER_OFFSET, cloud_server, CLOUD_SERVER_LENGTH))
            {
                ret.assign(cloud_server, CLOUD_SERVER_LENGTH);
            }
            else
            {
                TRACE_E("Couldn't fetch 'cloud_server' from factory_info!");
            }
            free(cloud_server);
        }
    }

    return ret;
}
string factory_info::get_token(void)
{
    string ret = "";

    if (NULL != partition_ctx)
    {
        char *token = (char *)malloc(TOKEN_LENGTH);
        if (token)
        {
            if (ESP_OK == esp_partition_read(partition_ctx, TOKEN_OFFSET, token, TOKEN_LENGTH))
            {
                ret.assign(token, TOKEN_LENGTH);
            }
            else
            {
                TRACE_E("Couldn't fetch 'token' from factory_info!");
            }
            free(token);
        }
    }

    return ret;
}

int factory_info::set_default_wifi_ssid(string &ssid)
{
    int ret = 0;
    if (partition_ctx != NULL)
    {
        size_t length = (size_t)((ssid.length() > WIFI_SSID_LENGTH) ? WIFI_SSID_LENGTH : ssid.length());
        ERROR_CHECK(esp_partition_write(partition_ctx, WIFI_SSID_OFFSET, ssid.c_str(), length),
                    "'wifi-ssid' wrote successfully.", "'wifi-ssid' write failed!");
    }
    return ret;
}
int factory_info::set_deafult_wifi_password(string &password)
{
    int ret = 0;
    if (partition_ctx != NULL)
    {
        size_t length = (size_t)((password.length() > WIFI_PASSWORD_LENGTH) ? WIFI_PASSWORD_LENGTH : password.length());
        ERROR_CHECK(esp_partition_write(partition_ctx, WIFI_PASSWORD_OFFSET, password.c_str(), length),
                    "'wifi-password' wrote successfully.", "'wifi-password' write failed!");
    }
    return ret;
}
int factory_info::set_product_name(string &product_name)
{
    int ret = 0;
    if (partition_ctx != NULL)
    {
        size_t length = (size_t)((product_name.length() > PRODUCT_NAME_LENGTH) ? PRODUCT_NAME_LENGTH : product_name.length());
        ERROR_CHECK(esp_partition_write(partition_ctx, PRODUCT_NAME_OFFSET, product_name.c_str(), length),
                    "'product_name' wrote successfully.", "'product_name' write failed!");
    }
    return ret;
}
int factory_info::set_cloud_server(string &cloud_servre)
{
    int ret = 0;
    if (partition_ctx != NULL)
    {
        size_t length = (size_t)((cloud_servre.length() > CLOUD_SERVER_LENGTH) ? CLOUD_SERVER_LENGTH : cloud_servre.length());
        ERROR_CHECK(esp_partition_write(partition_ctx, CLOUD_SERVER_OFFSET, cloud_servre.c_str(), length),
                    "'cloud_server' wrote successfully.", "'cloud_server' write failed!");
    }
    return ret;
}
int factory_info::set_token(string &token)
{
    int ret = 0;
    if (partition_ctx != NULL)
    {
        size_t length = (size_t)((token.length() > TOKEN_LENGTH) ? TOKEN_LENGTH : token.length());
        ERROR_CHECK(esp_partition_write(partition_ctx, TOKEN_OFFSET, token.c_str(), length),
                    "'token' wrote successfully.", "'token' write failed!");
    }
    return ret;
}
#endif