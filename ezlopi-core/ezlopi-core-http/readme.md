## Example

static void \_**\_check_and_free(char \*to_be_freed)
{
if (to_be_freed)
{
ezlopi_free(**FUNCTION\_\_, to_be_freed);
}
}

void send_http_request(void)
{
char *cloud_url = ezlopi_factory_info_v2_get_cloud_server();
char *private_key = ezlopi_factory_info_v2_get_ssl_private_key();
char *shared_key = ezlopi_factory_info_v2_get_ssl_shared_key();
char *ca_cert = ezlopi_factory_info_v2_get_ca_certificate();

    cJSON *headers = cJSON_CreateObject(__FUNCTION__);
    if (headers && cloud_url && private_key && shared_key && ca_cert)
    {
        char location[256];
        strncpy(location, "api/v1/controller/sync?version=1", 256);
        cJSON_AddStringToObject(__FUNCTION__, headers, "controller-key", ""); // add controller key here
        char *response = EZPI_core_http_post_request(cloud_url, location, headers, private_key, shared_key, ca_cert);
        if (response)
        {
            TRACE_I("Http post request response:\r\n%s", response);
            ezlopi_free(__FUNCTION__, response);
        }
    }

    ____check_and_free(cloud_url);
    ____check_and_free(private_key);
    ____check_and_free(shared_key);
    ____check_and_free(ca_cert);
    cJSON_Delete(__FUNCTION__, headers);

}
