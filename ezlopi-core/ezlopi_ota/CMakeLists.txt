# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "./*.c*")

idf_component_register(SRCS "ezlopi_ota.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES 
                trace 
                json   
                version 
                nvs_flash 
                app_update 
                esp_https_ota 
                esp-tls 
                esp_http_client 
                ezlopi_factory_info)
