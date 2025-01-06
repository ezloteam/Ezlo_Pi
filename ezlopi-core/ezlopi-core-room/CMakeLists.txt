file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-devices 
        ezlopi-core-errors
)
set(EZLOPI_SERVICE_COMPONENTS 
        ezlopi-service-meshbot
)

idf_component_register(SRCS "ezlopi_core_room.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_CORE_COMPONENTS}
                ${EZLOPI_SERVICE_COMPONENTS}
            )
