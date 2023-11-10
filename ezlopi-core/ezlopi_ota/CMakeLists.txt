# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "./*.c*")

idf_component_register(SRCS "ezlopi_ota.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash trace json esp_http_client ezlopi_factory_info esp-tls app_update esp_https_ota version)
