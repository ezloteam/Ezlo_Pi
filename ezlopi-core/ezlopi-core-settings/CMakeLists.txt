# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "./*.c*")

set(ESP_IDF_COMPONENTS
        cjext
)
set(EZLOPI_CLOUD_COMPONENTS
        ezlopi-cloud-constants
)


idf_component_register(SRCS "ezlopi_core_settings.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${ESP_IDF_COMPONENTS}
                ${EZLOPI_CLOUD_COMPONENTS}
    )
