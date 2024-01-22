
#include <string.h>

#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "esp_wifi.h"
#include "cJSON.h"

#include "trace.h"
#include "dns_hijacking.h"

#include "ezlopi_service_uri.h"

static const char *error_page_data = "\
    <!DOCTYPE html>\
        <html>\
            <body>\
                <h1>Error processing request</h1>\
            </body>\
        </html>\
";

static const char *success_page_data = "\
    <!DOCTYPE html>\
        <html>\
            <body>\
                <h1>Wifi set</h1>\
                <span>Wifi details set successfully.</span>\
            </body>\
        </html>\
";

static esp_err_t ezlopi_capture_base_uri_handler(httpd_req_t *req);
static esp_err_t ezlopi_capture_config_uri_handle(httpd_req_t *req);
esp_err_t ezlopi_http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);

static httpd_uri_t ezlopi_capture_base_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = ezlopi_capture_base_uri_handler,
};

static httpd_uri_t ezlopi_capture_config_uri = {
    .uri = "/config",
    .method = HTTP_POST,
    .handler = ezlopi_capture_config_uri_handle,
};

static httpd_handle_t httpd_server_handle = NULL;
static bool wifi_cred_available = false;
static char buffer[256];
static int bytes_read = 0;

void ezlopi_begin_ap_server_service()
{
    httpd_config_t httpd_configuration = HTTPD_DEFAULT_CONFIG();
    TRACE_B("Starting HTTP server");
    ESP_ERROR_CHECK(httpd_start(&httpd_server_handle, &httpd_configuration));
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpd_server_handle, &ezlopi_capture_base_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpd_server_handle, &ezlopi_capture_config_uri));
    ESP_ERROR_CHECK(httpd_register_err_handler(httpd_server_handle, HTTPD_404_NOT_FOUND, ezlopi_http_404_error_handler));
    wifi_cred_available = false;
}

static esp_err_t ezlopi_capture_base_uri_handler(httpd_req_t *req)
{
    esp_err_t error = ESP_OK;
    TRACE_B("%s", __func__);

    ESP_LOGI("ESP_SERVER", "URL:- %s", req->uri);
    char host[50];
    error = httpd_req_get_hdr_value_str(req, "Host", host, sizeof(host) - 1);
    ESP_LOGE("HOST_TAG", "Incoming header : %s", host);

    esp_vfs_spiffs_conf_t spiffs_config = {
        .base_path = "/spiffs",
        .max_files = 5,
        .format_if_mount_failed = false,
        .partition_label = NULL,
    };

    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&spiffs_config));
    size_t total_used = 0;
    size_t total_available = 0;
    ESP_ERROR_CHECK(esp_spiffs_info(NULL, &total_available, &total_used));
    TRACE_B("Partition size: total available = %d, total used = %d", total_available, total_used);
    TRACE_B("Reading spiffs content.");

    FILE *f = fopen("/spiffs/login.html", "r");
    if (NULL != f)
    {
        fseek(f, 0, SEEK_END);
        size_t file_size = ftell(f);
        TRACE_B("file size is %d", file_size);
        fseek(f, 0, SEEK_SET);
        char *login_data = (char *)malloc(file_size);
        if (NULL != login_data)
        {
            memset(login_data, 0, file_size);
            size_t read_len = fread(login_data, 1, file_size, f);
            fclose(f);
            ESP_ERROR_CHECK(httpd_resp_send(req, login_data, read_len));
            ESP_ERROR_CHECK(httpd_resp_send(req, NULL, 0));
        }
        else
        {
            TRACE_E("No memory available.");
        }
    }
    else
    {
        TRACE_E("File handler is NULL");
        ESP_ERROR_CHECK(httpd_resp_send(req, error_page_data, strlen(error_page_data)));
        ESP_ERROR_CHECK(httpd_resp_send(req, NULL, 0));
    }
    esp_vfs_spiffs_unregister(NULL);
    return error;
}

static esp_err_t ezlopi_capture_config_uri_handle(httpd_req_t *req)
{
    esp_err_t error = ESP_OK;
    TRACE_B("%s", __func__);

    ESP_LOGI("ESP_SERVER", "URL:- %s", req->uri);
    char host[50];
    error = httpd_req_get_hdr_value_str(req, "Host", host, sizeof(host) - 1);
    ESP_LOGE("HOST_TAG", "Incoming header : %s", host);
    TRACE_B("content len is %d", req->content_len);

    bytes_read = httpd_req_recv(req, buffer, req->content_len);
    buffer[bytes_read] = '\0';

    TRACE_B("Data bytes read is %d", bytes_read);
    TRACE_E("Received data is: %s", buffer);

    wifi_cred_available = true;

    ESP_ERROR_CHECK(httpd_resp_send(req, success_page_data, strlen(success_page_data)));
    ESP_ERROR_CHECK(httpd_resp_send(req, NULL, 0));

    return error;
}

// HTTP Error (404) Handler - Redirects all requests to the root page
esp_err_t ezlopi_http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    // Set status
    httpd_resp_set_status(req, "302 Temporary Redirect");
    // Redirect to the "/" root directory
    httpd_resp_set_hdr(req, "Location", "/");
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

    TRACE_B("Redirecting to root");
    return ESP_OK;
}

void ezlopi_end_ap_server_service()
{
    TRACE_B("Stopping HTTP server.");
    ESP_ERROR_CHECK(httpd_stop(httpd_server_handle));
}

int ezlopi_get_wifi_cred(char *wifi_cred)
{
    int ret = 0;
    if (wifi_cred_available)
    {
        strncpy(wifi_cred, buffer, bytes_read);
        ret = bytes_read;
    }
    else
    {
        free(wifi_cred);
        ret = -1;
    }
    return ret;
}
