#ifndef _EZLOPI_CORE_SCENES_THEN_METHODS_HELPER_FUNCTIONS_H_
#define _EZLOPI_CORE_SCENES_THEN_METHODS_HELPER_FUNCTIONS_H_

#include "ezlopi_core_http.h"

void parse_http_url(s_ezlopi_core_http_mbedtls_t *tmp_http_data, const char *field_value_string);
void parse_http_content(s_ezlopi_core_http_mbedtls_t *tmp_http_data, const char *field_value_string);
void parse_http_content_type(s_ezlopi_core_http_mbedtls_t *tmp_http_data, const char *field_value_string);
void parse_http_headers(s_ezlopi_core_http_mbedtls_t *tmp_http_data, cJSON *cj_value);
void parse_http_skipsecurity(s_ezlopi_core_http_mbedtls_t *tmp_http_data, bool value_bool);
void parse_http_creds(s_ezlopi_core_http_mbedtls_t *tmp_http_data, cJSON *cj_value);
void free_http_mbedtls_struct(s_ezlopi_core_http_mbedtls_t *config);

#endif /*_EZLOPI_CORE_SCENES_THEN_METHODS_HELPER_FUNCTIONS_H_*/