# set(config_src "config.cpp")

file(GLOB_RECURSE wifi_src "./*.c*")

idf_component_register(SRCS "${wifi_src}"
    INCLUDE_DIRS "."
    REQUIRES 
                ezlopi-core-nvs 
                ezlopi-service-uart 
                esp_netif 
                trace 
                ezlopi-core-factory-info 
                ezlopi-core-event-group 
                dns_hijacking 
                ezlopi-service-uri)
