# set(config_src "config.cpp")

file(GLOB config_src "*/*.c")
# list(REMOVE_ITEM config_src "example/*")
# list(REMOVE_ITEM config_src "examples/*")

message(STATUS "HERE " ${config_src})

idf_component_register(SRCS ${config_src}
    INCLUDE_DIRS "."
    REQUIRES esp_common esp-tls esp_http_client common_components trace)


