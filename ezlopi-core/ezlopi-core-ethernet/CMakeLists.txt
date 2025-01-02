file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-actions 
)
set(EZLOPI_HAL_COMPONENTS 
        ezlopi-hal-spi
)


idf_component_register(SRCS "ezlopi_core_ethernet.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_CORE_COMPONENTS}
                ${EZLOPI_HAL_COMPONENTS}
    )
