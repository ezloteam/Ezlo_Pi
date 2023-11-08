# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_actions.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES 
                trace 
                esp_common 
                esp-tls 
                esp_http_client)