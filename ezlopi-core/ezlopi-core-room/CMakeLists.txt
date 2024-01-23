file(GLOB_RECURSE config_src "*.c")


set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-devices 
)
set(EZLOPI_SERVICE_COMPONENTS 
        ezlopi-service-meshbot
)

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_CORE_COMPONENTS}
                ${EZLOPI_SERVICE_COMPONENTS}
            )