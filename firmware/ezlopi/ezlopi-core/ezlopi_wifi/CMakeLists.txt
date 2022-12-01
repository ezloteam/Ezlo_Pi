# set(config_src "config.cpp")

file(GLOB_RECURSE wifi_src "./*.c*")

idf_component_register(SRCS "ezlopi_wifi.c" "${wifi_src}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi_nvs qt_serial esp_netif trace ezlopi_factory_info)