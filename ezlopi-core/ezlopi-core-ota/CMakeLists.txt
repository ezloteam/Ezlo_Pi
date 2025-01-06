
file(GLOB_RECURSE config_src "./*.c*")

set(ESP_IDF_COMPONENTS 
        esp_https_ota 
        app_update
)
set(EZLOPI_CORE_COMPONENTS
        ezlopi-core-reset
        ezlopi-core-processes
)

set(EZLOPI_SERVICE_COMPONENTS
        ezlopi-service-ota
)

idf_component_register(SRCS  "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${ESP_IDF_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS} 
                ${EZLOPI_SERVICE_COMPONENTS}
            )
