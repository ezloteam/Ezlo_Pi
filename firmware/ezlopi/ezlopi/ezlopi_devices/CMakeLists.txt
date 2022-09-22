# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES esp_common esp-tls esp_http_client common_components debug)