file(GLOB_RECURSE info_src "*.c")

set(ESP_IDF_COMPONENTS 

)

set(EZLOPI_COMPONENTS 
        
)

set(EZLOPI_CORE_COMPONENTS 

)

set(EZLOPI_UTIL_COMPONENTS 
        ezlopi-util-version
)

idf_component_register(SRCS  "${info_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${ESP_IDF_COMPONENTS}
                ${EZLOPI_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS}
                ${EZLOPI_UTIL_COMPONENTS}
    )
