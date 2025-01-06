file(GLOB_RECURSE info_src "*.c")

set(ESP_IDF_COMPONENTS 
)

set(EZLOPI_HAL_COMPONENTS 
)

set(EZLOPI_CORE_COMPONENTS 
ezlopi-core-sntp
ezlopi-core-ezlopi
ezlopi-core-errors
)

set(EZLOPI_CLOUD_COMPONENTS 
        ezlopi-cloud-constants
)

set(EZLOPI_SERV_COMPONENTS
)

set(EZLOPI_UTIL_COMPONENTS 
        ezlopi-util-version
)

idf_component_register(SRCS  "${info_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${ESP_IDF_COMPONENTS}
                ${EZLOPI_HAL_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS}
                ${EZLOPI_CLOUD_COMPONENTS}
                ${EZLOPI_SERV_COMPONENTS}
                ${EZLOPI_UTIL_COMPONENTS}
    )
