

/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

/**
 * @file    ezlopi_service_uri.c
 * @brief
 * @authors Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 1.0
 * @date    January 6th, 2024 11:03 PM
 */
#include <string.h>

#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "esp_wifi.h"
#include "esp_http_server.h"

#include "cjext.h"
#include "ezlopi_util_trace.h"
#include "dns_hijacking.h"
#include "EZLOPI_USER_CONFIG.h"

#include "ezlopi_service_uri.h"
/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/**
 * @brief Function to handle root("/") uri
 *
 * @param req Pointer to incoming request
 * @return esp_err_t
 */
static esp_err_t ezpi_capture_base_uri_handler(httpd_req_t *req);
/**
 * @brief Fuctiont to handle config("/config") uri
 *
 * @param req Pointer to incoming request
 * @return esp_err_t
 */
static esp_err_t ezpi_capture_config_uri_handle(httpd_req_t *req);
/**
 * @brief Function to handle http errors
 *
 * @param req Pointer to incoming request
 * @param err HTTP error that triggered the function
 * @return esp_err_t
 */
esp_err_t ezpi_http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);

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

static httpd_uri_t ezlopi_capture_base_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = ezpi_capture_base_uri_handler,
};

static httpd_uri_t ezlopi_capture_config_uri = {
    .uri = "/config",
    .method = HTTP_POST,
    .handler = ezpi_capture_config_uri_handle,
};

static httpd_handle_t httpd_server_handle = NULL;
static bool wifi_cred_available = false;
static char buffer[256];
static int bytes_read = 0;
/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_begin_ap_server_service()
{
    httpd_config_t httpd_configuration = HTTPD_DEFAULT_CONFIG();
    TRACE_I("Starting HTTP server");
    ESP_ERROR_CHECK(httpd_start(&httpd_server_handle, &httpd_configuration));
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpd_server_handle, &ezlopi_capture_base_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpd_server_handle, &ezlopi_capture_config_uri));
    ESP_ERROR_CHECK(httpd_register_err_handler(httpd_server_handle, HTTPD_404_NOT_FOUND, ezpi_http_404_error_handler));
    wifi_cred_available = false;
}

void EZPI_end_ap_server_service()
{
    TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_INFO, "Stopping HTTP server (AP).");
    TRACE_I("Stopping HTTP server.");
    ESP_ERROR_CHECK(httpd_stop(httpd_server_handle));
}

int EZPI_end_ap_server_serviceget_wifi_cred(char *wifi_cred)
{
    int ret = 0;
    if (wifi_cred_available)
    {
        strncpy(wifi_cred, buffer, bytes_read);
        ret = bytes_read;
    }
    else
    {
        ezlopi_free(__FUNCTION__, wifi_cred);
        ret = -1;
    }
    return ret;
}
/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/
static esp_err_t ezpi_capture_base_uri_handler(httpd_req_t *req)
{
    esp_err_t error = ESP_OK;
    TRACE_I("%s", __func__);

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
    TRACE_I("Partition size: total available = %d, total used = %d", total_available, total_used);
    TRACE_I("Reading spiffs content.");

    FILE *f = fopen("/spiffs/login.html", "r");
    if (NULL != f)
    {
        fseek(f, 0, SEEK_END);
        size_t file_size = ftell(f);
        TRACE_I("file size is %d", file_size);
        fseek(f, 0, SEEK_SET);
        char *login_data = (char *)ezlopi_malloc(__FUNCTION__, file_size);
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
            TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_ERROR, "No memory available!");
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

static esp_err_t ezpi_capture_config_uri_handle(httpd_req_t *req)
{
    esp_err_t error = ESP_OK;
    TRACE_I("%s", __func__);

    ESP_LOGI("ESP_SERVER", "URL:- %s", req->uri);
    char host[50];
    error = httpd_req_get_hdr_value_str(req, "Host", host, sizeof(host) - 1);
    ESP_LOGE("HOST_TAG", "Incoming header : %s", host);
    TRACE_I("content len is %d", req->content_len);

    bytes_read = httpd_req_recv(req, buffer, req->content_len);
    buffer[bytes_read] = '\0';

    TRACE_I("Data bytes read is %d", bytes_read);
    TRACE_E("Received data is: %s", buffer);

    wifi_cred_available = true;

    ESP_ERROR_CHECK(httpd_resp_send(req, success_page_data, strlen(success_page_data)));
    ESP_ERROR_CHECK(httpd_resp_send(req, NULL, 0));

    return error;
}

// HTTP Error (404) Handler - Redirects all requests to the root page
esp_err_t ezpi_http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    // Set status
    httpd_resp_set_status(req, "302 Temporary Redirect");
    // Redirect to the "/" root directory
    httpd_resp_set_hdr(req, "Location", "/");
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

    TRACE_I("Redirecting to root");
    return ESP_OK;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
