# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "./*.c*")

idf_component_register(SRCS "ezlopi_settings.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash
             trace 
             json 
             esp_http_client 
             ezlopi_factory_info 
             ezlopi_settings
             ezlopi_cloud_constants
             esp-tls 
             app_update 
             esp_https_ota 
             version)
