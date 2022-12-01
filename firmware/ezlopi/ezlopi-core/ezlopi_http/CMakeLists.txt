# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_http.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace esp_http_client common_components esp-tls)