# set(config_src "config.cpp")

file(GLOB_RECURSE wifi_src "./*.c*")

set(EZLOPI_COMPONENTS 
        dns_hijacking
)
set(EZLOPI_SERVICE_COMPONENTS 
        ezlopi-service-uart
)


idf_component_register(SRCS "${wifi_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_COMPONENTS}
                ${EZLOPI_SERVICE_COMPONENTS}
        )
