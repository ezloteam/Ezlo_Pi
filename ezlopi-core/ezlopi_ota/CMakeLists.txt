# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "./*.c*")

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash trace json esp_http_client esp-tls esp_https_ota app_update
    ezlopi_factory_info ezlopi_reboot common_components version)
