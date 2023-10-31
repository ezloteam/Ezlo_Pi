# set(config_src "config.cpp")

file(GLOB config_src "*/*.c")

idf_component_register(SRCS "esp8266_wrapper.c" ${config_src}
    INCLUDE_DIRS "."
    REQUIRES esp_common esp-tls esp_http_client common_components trace)



    