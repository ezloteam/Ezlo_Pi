# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "./*.c*")

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash trace json esp_http_client ezlopi-core-factory_info esp-tls app_update esp_https_ota app_update ezlopi-core-reboot version)
