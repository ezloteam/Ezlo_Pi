#ifndef _EZLOPI_CORE_SCENES_THEN_METHODS_HELPER_FUNCTIONS_H_
#define _EZLOPI_CORE_SCENES_THEN_METHODS_HELPER_FUNCTIONS_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "ezlopi_core_http.h"

typedef struct s_sendhttp_method
{
    const char* field_name;
    void (*field_func)(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field);
} s_sendhttp_method_t;

void parse_http_request_type(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field);
void parse_http_url(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field);
void parse_http_content(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field);
void parse_http_content_type(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field);
void parse_http_headers(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field);
void parse_http_skipsecurity(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field);
void parse_http_creds(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field);
void free_http_mbedtls_struct(s_ezlopi_core_http_mbedtls_t* config);

ezlopi_error_t ezlopi_core_scene_then_helper_setexpression_setvariable(char * expression_name, const char * code_str, const char * value_type, cJSON * cj_metadata, cJSON * cj_params, l_fields_v2_t * var_value);

int ezlopi_core_scene_then_helper_toggleValue(uint32_t item_id, const char * item_id_str);
#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS
#endif /*_EZLOPI_CORE_SCENES_THEN_METHODS_HELPER_FUNCTIONS_H_*/