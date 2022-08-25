# set(config_src "config.cpp")

file(GLOB_RECURSE wifi_src "src/*.c*")

idf_component_register(SRCS "${wifi_src}"
    INCLUDE_DIRS "include"
    REQUIRES nvs_storage qt_serial esp_netif debug factory_info)