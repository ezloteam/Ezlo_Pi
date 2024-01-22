# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

set(ESP_IDF_COMPONENTS 
        json 
        esp-tls
        esp_http_client
)

set(EZLOPI_COMPONENTS 
        trace
)

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${ESP_IDF_COMPONENTS}
                ${EZLOPI_COMPONENTS}
    )
